#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "genie/dat/DatFile.h"
#include "genie/dat/TechageEffect.h"
#include "ids.h"


void debugPrintEffect(genie::DatFile *df, uint16_t effectId) {
    const genie::Effect &effect = df->Effects.at(effectId);
    std::cout << effectId << " - " << effect.Name << std::endl;
}

std::string getTechName(genie::DatFile *df, int techId) {
    return techId > 0 ? df->Techs[techId].Name : "-1";
}

std::string getUnitName(genie::DatFile *df, int unitId, int civId=0) {
    return unitId > 0 ? df->Civs[civId].Units[unitId].Name : "-1";
}

std::string getCommandTypeName(genie::DatFile *df, genie::EffectCommand &command) {
    switch (static_cast<CommandType>(command.Type)) {
        case CommandType::SET_ATTRIBUTE_MODIFIER:
            return "Attribute Modifier (Set)";
        case CommandType::RESOURCE_MODIFIER:
            return "Resource Modifier (" + std::string(command.B == 0 ? "set" : "+/-") + ") ";
        case CommandType::ENABLE_DISABLE_UNIT:
            return std::string(command.B == 0 ? "Disable" : "Enable") + " Unit " + getUnitName(df, command.A);
        case CommandType::UPGRADE_UNIT:
            return "Upgrade Unit " + getUnitName(df, command.A) + " -> " + getUnitName(df, command.B);
        case CommandType::ATTRIBUTE_MODIFIER:
            return "Attribute Modifier (+/-)";
        case CommandType::ATTRIBUTE_MULTIPLIER:
            return "Attribute Modifier (Mult)";
        case CommandType::RESOURCE_MULTIPLIER:
            return "Resource Modifier (Mult)";
        case CommandType::SPAWN_UNIT:
            return "Spawn unit " + getUnitName(df, command.A) + " from " + getUnitName(df, command.B);
        case CommandType::SET_TEAM_ATTRIBUTE_MODIFIER:
            return "Team Attribute Modifier (Set)";
        case CommandType::TEAM_ATTRIBUTE_MODIFIER:
            return "Team Attribute Modifier (+/-)";
        case CommandType::TECH_COST_MODIFIER:
            return "Tech Cost Modifier (" + std::string(command.C == 0 ? "set" : "+/-") + ") " + getTechName(df, command.A);
        case CommandType::DISABLE_TECH:
            return "Disable tech " + getTechName(df, command.D);
        case CommandType::TECH_TIME_MODIFIER:
            return "Tech Time Modifier (Set/+/-) " + getTechName(df, command.A);
    }
    return "UNKNOWN COMMAND";
}

int capEffectMultiplication(EffectId effectId, int times) {
    // Cap some effects multiplication so it doesnt break the gameplay too much
    // especially for higher multiplication values
	switch(effectId) {
        case EffectId::HUNS_100_WOOD:
        case EffectId::PERSIANS_KAMANDARAN:
            return std::min(1, times);
        case EffectId::BYZANTINE_BUILDING_HP_DARK:
        case EffectId::BYZANTINE_BUILDING_HP_FEUDAL:
        case EffectId::BYZANTINE_BUILDING_HP_CASTLE:
        case EffectId::BYZANTINE_BUILDING_HP_IMPERIAL:
        case EffectId::MAYAN_TECH_TREE:
        case EffectId::SARACEN_MARKET_BONUS:
            return std::min(3, times);
        case EffectId::PERSIANS_TC_HITPOINTS:
            return std::min(4, times);
        case EffectId::PERSIANS_DOCK_HITPOINTS:
            return std::min(5, times);
        default:
            return times;
	}
}

void addDummyAnnexBuilding(genie::DatFile *df, const std::vector<int16_t> annexUnitIds) {
    genie::Unit dummyAnnexUnit(df->Civs[0].Units[annexUnitIds[0]]);
    dummyAnnexUnit.Building.Annexes.clear();

    const std::vector<std::pair<int, int>> misplacements {
        {0, 0},
        {-1, 0},
        {-1, 1},
        {0, 1}
    };

    // generate the annex instances with the given unit ids
    for (int i = 0; i < 4; ++i) {
        auto newAnnex = genie::unit::BuildingAnnex();
        newAnnex.Misplacement = misplacements[i];
        newAnnex.UnitID = i < annexUnitIds.size() ? annexUnitIds.at(i) : -1;
        dummyAnnexUnit.Building.Annexes.push_back(newAnnex);
    }
    for (auto &civ : df->Civs) {
        civ.Units.push_back(dummyAnnexUnit);
        civ.UnitPointers.push_back(1);
    }
}

void multiplySetAttributeModifierCommand(genie::DatFile *df, genie::EffectCommand &command, int civ_id, int times) {
    switch (static_cast<AttributeType>(command.C)) {
        case AttributeType::BONUS_DAMAGE_RESISTANCE:
            command.D = 1. - std::pow(1. - command.D, times);
            break;
        default:
            break;
    }
}

void multiplyResourceModifierCommand(genie::DatFile *df, genie::EffectCommand &command, int civ_id, int times) {
	if (command.B == 0) { // `set` mode
		auto baseValue = df->Civs[civ_id].Resources.at(command.A);
        // research cost modifier and research time modifier resources have a base value of 0
        // we need to replace that value with 1 so that the bonus calculation works correctly
		if (command.A == TYPE_RESEARCH_COST_MODIFIER || command.A == TYPE_RESEARCH_TIME_MODIFIER) {
			baseValue = 1.0;
		}

		if (command.A == TYPE_GOLD_MINING_PRODUCTIVITY
			|| command.A == TYPE_STONE_MINING_PRODUCTIVITY
			|| command.A == TYPE_WOOD_CHOPPING_PRODUCTIVITY
			|| command.A == TYPE_FOOD_GATHERING_PRODUCTIVITY
			|| command.A == TYPE_FOOD_HERDING_PRODUCTIVITY
            || command.D < baseValue
		) {
			// productivity and "negative" bonuses should always stack multiplicatively
			command.D = std::pow(command.D / baseValue, times) * baseValue;
		}
		else {
            // the rest of the bonuses stack additively
			command.D = (command.D - baseValue) * times + baseValue;
		}
	}
    else { // `add` or `sub` mode
		command.D *= times;
	}
}

void multiplyTcAnnexCommand(genie::DatFile *df, genie::EffectCommand &command, int times) {
    int total_leafs = 0;

    // store unit ids for every created tc annex node
	std::vector<int16_t> annexUnits;
    annexUnits.push_back(command.B);

    int leafCount = 1;

    // find the subtree that will produce less than `times` leaves with the maximum degree (4)
    while (leafCount * 4 < times) {
        addDummyAnnexBuilding(
            df,
            std::vector<int16_t>(4, annexUnits.back())
        );
        annexUnits.push_back(df->Civs[0].Units.size() - 1);
        leafCount *= 4;
    }

    // find how many of each annex unit types are needed to reach `times` replicas
    // add them in a tree using a stack
	std::vector<int16_t> stack;
    for (int i = annexUnits.size() - 1; i >= 0; --i) {
        stack.insert(stack.end(), times / leafCount, annexUnits[i]);
        times %= leafCount;
        leafCount /= 4; // descend one level
        while (stack.size() >= 4) {
            addDummyAnnexBuilding(df, stack);
            stack.erase(stack.begin(), stack.begin()+4);
            stack.push_back(df->Civs[0].Units.size() - 1);
        }
    }
    if (stack.size() != 1) {
        addDummyAnnexBuilding(df, stack);
        stack.clear();
        stack.push_back(df->Civs[0].Units.size() - 1);
    }
    command.B = stack.at(0);
}

void multiplyAttributeModifierCommand(genie::DatFile *df, genie::EffectCommand &command, int times) {
    const AttributeType attributeType = static_cast<AttributeType>(command.C);
    if (attributeType == AttributeType::ARMOR || attributeType == AttributeType::ATTACK) {
        int armorType = int(command.D) / 256;
        int amount = int(command.D) % 256;
        int newAmount = std::min(amount * times, 255);
        command.D = armorType * 256 + newAmount;
    }
    else {
        command.D *= times;
    }
}

void multiplyAttributeMultiplierCommand(genie::DatFile *df, genie::EffectCommand &command, int civId, int times) {
    const AttributeType attributeType = static_cast<AttributeType>(command.C);
    if (attributeType == AttributeType::ARMOR || attributeType == AttributeType::ATTACK) {
        int armorType = int(command.D) / 256;
        float amount = int(command.D) % 256;
        float newAmount = std::min(std::pow(amount/100, times) * 100, 255.);
        command.D = armorType * 256 + newAmount;
    }
    else {
        command.D = std::pow(command.D, times);
    }
    // warn if unit has too many hitpoints
    if (command.A != -1 && attributeType == AttributeType::HITPOINTS) {
        const int unitHitPoints = df->Civs[civId].Units[command.A].HitPoints;
        if (unitHitPoints * command.D >= (1 << 16)) {
            std::cout << "[WARNING] unit with too many hitpoints: " << getUnitName(df, command.A, civId) << " (" << df->Civs[civId].Name << ")" << std::endl;
        }
    }
}

void multiplyResourceMultiplierCommand(genie::DatFile *df, genie::EffectCommand &command, int civId, int times) {
    command.D = std::pow(command.D, times);
}

void multiplySpawnUnitCommand(genie::DatFile *df, genie::EffectCommand &command, int times) {
    command.C *= times;
}

void multiplyTechCostModifierCommand(genie::DatFile *df, genie::EffectCommand &command, int times) {
    if (command.C == 0) { // Set

    } else { // + or -
        command.D *= times;
    }
}

void multiplyTechTimeModifierCommand(genie::DatFile *df, genie::EffectCommand &command, int times) {
    if (command.A == -1) {
        return;
    }
    const genie::Tech &tech = df->Techs[command.A];
    if (command.C == 0) { // `set` mode

    }
    else { // `add` or `sub` mode
        command.D *= times;
        if (command.D <= -tech.ResearchTime) {
            command.D = -tech.ResearchTime + 1;
        }
    }
}

void multiplySetTeamAttributeModifier(genie::DatFile *df, genie::EffectCommand &command, uint16_t civId, int times) {
    if (command.A != -1) {
        float baseValue = df->Civs[civId].Units[command.A].ResourceStorages[0].Amount;
        command.D = (command.D - baseValue) * times + baseValue;
    }
}

void multiplyTeamAttributeModifier(genie::DatFile *df, genie::EffectCommand &command, int times) {
    command.D *= times;
}

void multiplyEffectCommand(genie::DatFile *df, genie::EffectCommand &command, uint16_t effectId, uint16_t civId, int times) {
	std::string commandTypeName = getCommandTypeName(df, command);
    genie::EffectCommand oldValue = command;
    switch (static_cast<CommandType>(command.Type)) {
        case CommandType::SET_ATTRIBUTE_MODIFIER:
            multiplySetAttributeModifierCommand(df, command, civId, times);
            break;
        case CommandType::RESOURCE_MODIFIER:
            multiplyResourceModifierCommand(df, command, civId, times);
            break;
        case CommandType::UPGRADE_UNIT:
            if (command.A == ID_EMPTY_TC_ANNEX) {
                multiplyTcAnnexCommand(df, command, times);
            }
            break;
        case CommandType::ATTRIBUTE_MODIFIER:
            multiplyAttributeModifierCommand(df, command, times);
            break;
        case CommandType::ATTRIBUTE_MULTIPLIER:
            multiplyAttributeMultiplierCommand(df, command, civId, times);
            break;
        case CommandType::RESOURCE_MULTIPLIER:
            multiplyResourceMultiplierCommand(df, command, civId, times);
        case CommandType::SPAWN_UNIT:
            multiplySpawnUnitCommand(df, command, times);
            break;
        case CommandType::SET_TEAM_ATTRIBUTE_MODIFIER:
            multiplySetTeamAttributeModifier(df, command, civId, times);
            break;
        case CommandType::TEAM_ATTRIBUTE_MODIFIER:
            multiplyTeamAttributeModifier(df, command, times);
            break;
        case CommandType::TECH_COST_MODIFIER:
            multiplyTechCostModifierCommand(df, command, times);
            break;
        case CommandType::TECH_TIME_MODIFIER:
            multiplyTechTimeModifierCommand(df, command, times);
            break;
    }
    std::cout << "  " << int(command.Type) << " - " << commandTypeName << " - "
        << "(A=" << oldValue.A << " B=" << oldValue.B << " C=" << oldValue.C << " D=" << oldValue.D << ") => "
        << "(A=" << command.A << " B=" << command.B << " C=" << command.C << " D=" << command.D << ")" << std::endl;
}

void multiplyEffect(genie::DatFile *df, uint16_t effectId, uint16_t civ_id, int times) {
    times = capEffectMultiplication(static_cast<EffectId>(effectId), times);

    auto &effect = df->Effects.at(effectId);
    std::cout << "Effect " << effectId << " \"" << effect.Name << "\"" << std::endl;
    for (auto &command: effect.EffectCommands) {
        multiplyEffectCommand(df, command, effectId, civ_id, times);
    }
}

void multiplyCivilizationBonuses(genie::DatFile *df, int times) {
    std::vector<uint16_t> effectIds;
    for (auto it = df->Civs.begin(); it != df->Civs.end(); ++it) {
        auto civ_id = it - df->Civs.begin();
        if (civ_id == 0) // skip gaia
            continue;

        effectIds.push_back(it->TechTreeID);
        effectIds.push_back(it->TeamBonusID);

        std::cout << "Tech tree (" << it->Name << "):" << std::endl;
        multiplyEffect(df, it->TechTreeID, civ_id, times);
        std::cout << "Team bonus (" << it->Name << "):" << std::endl;
        multiplyEffect(df, it->TeamBonusID, civ_id, times);
        std::cout << std::endl;
    }

    for (auto it = df->Techs.begin(); it != df->Techs.end(); ++it) {
        if (it->Civ == 0 || it->Civ == -1 || it->EffectID == -1)
            continue;
        if (std::find(effectIds.begin(), effectIds.end(), it->EffectID) != effectIds.end())
            continue;

        effectIds.push_back(it->EffectID);

        std::cout << "Tech " << (it - df->Techs.begin()) << " \"" << it->Name << "\" " << "(" << df->Civs[it->Civ].Name << ")" << std::endl;
        multiplyEffect(df, it->EffectID, it->Civ, times);
        std::cout << std::endl;
    }

	for (auto civ = df->Civs.begin(); civ != df->Civs.end(); ++civ) {
		auto &unit = civ->Units[ID_MADRASAH_MONK];
        for (auto &storage : unit.ResourceStorages) {
            storage.Amount *= capEffectMultiplication(EffectId::SARACEN_MADRASAH, times);
        }
	}
}
