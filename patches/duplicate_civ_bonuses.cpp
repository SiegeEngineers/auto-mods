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

std::string getUnitName(genie::DatFile *df, int unitId) {
    return unitId > 0 ? df->Civs[0].Units[unitId].Name : "-1";
}

std::string getCommandTypeName(genie::DatFile *df, genie::EffectCommand &command) {
    switch (command.Type) {
        case COMMAND_RESOURCE_MODIFIER:
            return "Resource Modifier (Set/+/-)";
        case COMMAND_UPGRADE_UNIT:
            return "Upgrade Unit " + getUnitName(df, command.A) + " -> " + getUnitName(df, command.B);
        case COMMAND_ATTRIBUTE_MODIFIER:
            return "Attribute Modifier (+/-)";
        case COMMAND_ATTRIBUTE_MULTIPLIER:
            return "Attribute Modifier (Mult)";
        case COMMAND_TEAM_ATTRIBUTE_MODIFIER:
            return "Team Attribute Modifier (Set)";
        case COMMAND_TECH_COST_MODIFIER:
            return "Tech Cost Modifier (Set/+/-) " + getTechName(df, command.A);
        case COMMAND_DISABLE_TECH:
            return "Disable tech " + getTechName(df, command.D);
        case COMMAND_TECH_TIME_MODIFIER:
            return "Tech Time Modifier (Set/+/-) " + getTechName(df, command.A);
    }
    return "UNKNOWN";
}

int capEffectMultiplication(uint16_t effectId, int times) {
    // Cap some effects multiplication so it doesnt break the gameplay too much
    // especially for higher multiplication values
	switch(effectId) {
        case EFFECT_ID_HUNS_100_WOOD:
        case EFFECT_ID_PERSIANS_KAMANDARAN:
            return std::min(1, times);
        case EFFECT_ID_BYZANTINE_BUILDING_BONUS_HP_DARK:
        case EFFECT_ID_BYZANTINE_BUILDING_BONUS_HP_FEUDAL:
        case EFFECT_ID_BYZANTINE_BUILDING_BONUS_HP_CASTLE:
        case EFFECT_ID_BYZANTINE_BUILDING_BONUS_HP_IMPERIAL:
        case EFFECT_ID_MAYAN_TECH_TREE:
        case EFFECT_ID_SARACEN_MARKET_BONUS:
            return std::min(3, times);
        case EFFECT_ID_PERSIANS_TC_HITPOINTS:
            return std::min(4, times);
        case EFFECT_ID_PERSIANS_DOCK_HITPOINTS:
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
    for (auto& civ : df->Civs) {
        civ.Units.push_back(dummyAnnexUnit);
        civ.UnitPointers.push_back(1);
    }
}

void multiplyResourceModifierCommand(genie::DatFile *df, genie::EffectCommand& command, int civ_id, int times) {
	if (command.B == 1) { // `add` or `sub` mode
		command.D *= times;
	}
	else { // `set` mode
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
			// productivity and "negative" bonuses stack multiplicatively
			command.D = std::pow(command.D / baseValue, times) * baseValue;
		}
		else {
            // the rest of the bonuses stack additively
			command.D = (command.D - baseValue) * times + baseValue;
		}
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
    if (command.C == 8 || command.C == 9) { // Armor/Atack
        int armor_type = int(command.D) / 256;
        int amount = int(command.D) % 256;
        command.D = float(armor_type * 256 + std::min(amount * times, 255));
    }
    else {
        command.D *= times;
    }
}

void multiplyAttributeMultiplierCommand(genie::DatFile *df, genie::EffectCommand &command, int civ_id, int times) {
    command.D = std::pow(command.D, times);
    // make sure units wont have too many hitpoints
    if (command.A != -1 && command.C == 0) {
        const int unitHitPoints = df->Civs[civ_id].Units[command.A].HitPoints;
        if (unitHitPoints * command.D >= (1 << 16)) {
            std::cout << "[WARNING] unit with too many hitpoints: " << df->Civs[civ_id].Units[command.A].Name << " (" << df->Civs[civ_id].Name << ")" << std::endl;
        }
    }
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
    const genie::Tech& tech = df->Techs[command.A];
    if (command.C == 0) { // `set` mode

    }
    else { // `add` or `sub` mode
        command.D *= times;
        if (command.D <= -tech.ResearchTime) {
            command.D = -tech.ResearchTime + 1;
        }
    }
}

void multiplyEffectCommand(genie::DatFile *df, genie::EffectCommand& command, uint16_t effectId, uint16_t civ_id, int times) {
	std::string commandTypeName = getCommandTypeName(df, command);
    float oldValue = command.D;
    switch (command.Type) {
        case COMMAND_RESOURCE_MODIFIER:
            multiplyResourceModifierCommand(df, command, civ_id, times);
            break;
        case COMMAND_UPGRADE_UNIT:
            if (command.A == ID_EMPTY_TC_ANNEX) {
                multiplyTcAnnexCommand(df, command, times);
            }
            break;
        case COMMAND_ATTRIBUTE_MODIFIER:
            multiplyAttributeModifierCommand(df, command, times);
            break;
        case COMMAND_ATTRIBUTE_MULTIPLIER:
            multiplyAttributeMultiplierCommand(df, command, civ_id, times);
            break;
        case COMMAND_TEAM_ATTRIBUTE_MODIFIER:
            if (command.A != -1) {
                float delta = command.D - df->Civs[civ_id].Units[command.A].ResourceStorages[0].Amount;
                command.D = delta * times;
            }
            break;
        case COMMAND_TECH_COST_MODIFIER:
            multiplyTechCostModifierCommand(df, command, times);
            break;
        case COMMAND_TECH_TIME_MODIFIER:
            multiplyTechTimeModifierCommand(df, command, times);
            break;
    }
    std::cout << "  command type: " << int(command.Type) << " - " << commandTypeName << " - " << "(A=" << command.A << " B=" << command.B << " C=" << command.C << " D=" << oldValue << ") => " << command.D << std::endl;
}

void multiplyEffect(genie::DatFile *df, uint16_t effectId, uint16_t civ_id, int times) {
    times = capEffectMultiplication(effectId, times);

    auto& effect = df->Effects.at(effectId);
    std::cout << "Effect " << effectId << " \"" << effect.Name << "\"" << std::endl;
    for (auto& command: effect.EffectCommands) {
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
		auto& madarashMonk = civ->Units[ID_MADARASH_MONK];
		for (auto storage = madarashMonk.ResourceStorages.begin(); storage != madarashMonk.ResourceStorages.end(); ++storage) {
			if (storage->Type == TYPE_GOLD) {
				storage->Amount *= capEffectMultiplication(EFFECT_ID_SARACEN_MADARASH, times);
			}
		}
	}
}
