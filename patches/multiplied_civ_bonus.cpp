#include <string>
#include <iostream>
#include <functional>
#include <list>
#include <vector>
#include <set>
#include <math.h>
#include "multiplied_civ_bonus.h"
#include "genie/dat/DatFile.h"
#include "ids.h"

using namespace std;

set<int> updated_effects;

#pragma region Helper_Methods

float multipleAdd(float value, int count) {
    return value * count;
}

float multipleMultiply(float value, int count) {
    if (value <= 1) {
        return pow(abs(value), count);
    }
    else {
        return 1 + (abs(value) - 1) * count;
    }
}

float multipleChangeFrom(float value, int count, float base) {
    if (abs(base) <= 1e-6f){
        return value;
    }
    else if (value < 0) {
        return multipleMultiply(value / base, count) * base - base;
    }
    else {
        return multipleMultiply(value / base, count) * base;
    }
}

float calculateDiff(
    uint8_t to,
    uint8_t from,
    float totalCount
) {
    return from + (to - from) * totalCount;
}

float calculateDiff(
    int16_t to,
    int16_t from,
    float totalCount
) {
    return from + (to - from) * totalCount;
}

float calculateDiff(
    float to,
    float from,
    float totalCount
) {
    return from + (to - from) * totalCount;
}

float calculateReduction(
    float to,
    float from,
    float totalCount
) {
    if (abs(to) <= 1e-6f || abs(to - from) <= 1e-6f)
        return to;
    return pow(to/from, totalCount) * from;
}

float calculatePercentIncrease(
    float to,
    float from,
    float totalCount
) {
    if (abs(to - 100) <= 1e-6f || abs(to - from) <= 1e-6f)
        return to;
    return 100 - 100 * multipleMultiply((1-to/100)/(1-from/100), totalCount);
}

void multipleAttributeSet(genie::DatFile *df, std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    switch (command.C)
    {
        case 2: // garrison capacity
        case 21: // pop space
            command.D = multipleAdd(command.D, totalCount);
            break;
        case 11: // accuracy percent
            {
                auto base_accuracy = df->Civs.at(0).Units.at(command.A).Type50.AccuracyPercent;
                command.D = calculatePercentIncrease(command.D, base_accuracy, totalCount);
                break;
            }
        case 24: // hidden damage resistance
            command.D = 1 - multipleMultiply(1 - command.D, totalCount);
            break;
        case 110: // pop cost
            command.D = -multipleMultiply(-command.D, totalCount);
            break;
    }
}

void multipleResourceSet(std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    switch (command.A)
    {
        case 0: // food
        case 1: // wood
        case 2: // stone
        case 3: // gold
        case 91: // starting food
        case 92: // starting wood
        case 93: // starting stone
        case 94: // starting gold
        case 88: // fish trap food
        case 214: // mercenary kipchak
        case 234: // unit spawn cap
        case 237: // folwark collection amount
        case 251: // bengali trade food
        case 254: // gurjara mill food
            command.D = multipleAdd(command.D, totalCount);
            break;
        case 78: // trade fee
        case 85: // research cost multiplier
            command.D = multipleMultiply(command.D, totalCount);
            break;
    }
}

void multipleSpawnUnit(std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    switch (command.A)
    {
        case 59: // forrage bush
        case 83: // villager
            command.C = multipleAdd(command.C, totalCount);
            break;
        default: // other do not spawn more
            break;
    }
}

void multipleTechCostModifier(genie::DatFile *df, std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    auto &discounted_tech = df->Techs.at(command.A);
    auto cost_type = command.B;

    auto base_cost = 0;
    auto costs_size = discounted_tech.ResourceCosts.size();
    for (auto i = 0; i < costs_size; i++) {
        auto &resource_cost = discounted_tech.ResourceCosts.at(i);
        if (resource_cost.Type == cost_type) {
            base_cost = resource_cost.Amount;
        }
    }

    if (command.C == 0) {
        command.D = multipleChangeFrom(command.D, totalCount, base_cost);
    }
    else if (command.D <= 0) {
        command.D = multipleChangeFrom(base_cost + command.D, totalCount, base_cost) - base_cost;
    }
}

void multipleTechTimeModifier(genie::DatFile *df, std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    auto &discounted_tech = df->Techs.at(command.A);
    auto cost_type = command.B;
    auto base_time = discounted_tech.ResearchTime;

    if (command.C == 0) {
        command.D = multipleChangeFrom(command.D, totalCount, base_time);
    }
    else {
        command.D = multipleChangeFrom(base_time+command.D, totalCount, base_time) - base_time;
    }
}

void multipleDecreaseUnitCost(genie::DatFile *df, std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    auto &discounted_unit = df->Civs.at(0).Units.at(command.A);
    auto cost_type = command.C - 103;

    auto base_cost = 0;
    auto costs_size = discounted_unit.Creatable.ResourceCosts.size();
    for (auto i = 0; i < costs_size; i++) {
        auto &resource_cost = discounted_unit.Creatable.ResourceCosts.at(i);
        if (resource_cost.Type == cost_type) {
            base_cost = resource_cost.Amount;
        }
    }

    if (command.D >= 0) {
        command.D = multipleChangeFrom(command.D, totalCount, base_cost);
    }
    else {
        command.D = multipleChangeFrom(base_cost + command.D, totalCount, base_cost) - base_cost;
    }
}

void multipleAddFunc(std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    if (command.C != 8 && command.C != 9) {
        command.D = multipleAdd(command.D, totalCount);
    } else {
        auto commandValue = ((int)command.D % 256);
        auto valueType = command.D - commandValue;
        auto value = multipleAdd(commandValue, totalCount);

        while (value > 255) {
            auto addedCommand = new genie::EffectCommand(command);
            addedCommand->Type = command.Type;
            addedCommand->A = command.A;
            addedCommand->B = command.B;
            addedCommand->C = command.C;
            addedCommand->D = 255 + valueType;
            commands.push_back(*addedCommand);
            value -= 255;
        }

        command.D = value + valueType;
    }
}

void multipleMultiplyWorkRate(std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    if (command.D < 1.0f) {
        command.D = 1.0f / multipleMultiply(1.0f / command.D, totalCount);
    }
    else {
        command.D = multipleMultiply(command.D, totalCount);
    }
}

void multipleMultiplyFunc(std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    if (command.C == 0) { // hp is allowed to apply only 5x
        totalCount = totalCount;
    }

    if (command.C != 8 && command.C != 9) {
        command.D = multipleMultiply(command.D, totalCount);
    } else {
        auto commandValue = ((int)command.D % 256);
        auto valueType = command.D - commandValue;
        auto value = multipleMultiply((float)commandValue / 100, totalCount);

        while (value > 2.55) {
            auto addedCommand = new genie::EffectCommand(command);
            addedCommand->Type = command.Type;
            addedCommand->A = command.A;
            addedCommand->B = command.B;
            addedCommand->C = command.C;
            addedCommand->D = 255 + valueType;
            commands.push_back(*addedCommand);
            value /= 2.55;
        }

        command.D = (int)(value * 100) + valueType;
    }
}

void skipUpdate(int effect_id) {
    auto effect_updated = updated_effects.find(effect_id) != updated_effects.end();
    if (effect_updated) {
        cout << "Effect " << effect_id << " was already updated\n";
        return;
    } else {
        updated_effects.insert(effect_id);
    }
}

int duplicateEffect(
    genie::DatFile *df,
    int effect_id
) {
    auto &source_effect = df->Effects.at(effect_id);
    auto target_effect = new genie::Effect();
    target_effect->Name = source_effect.Name + " clone";

    auto &source_commands = source_effect.EffectCommands;
    auto source_commands_length = source_commands.size();
    for (auto i = 0; i < source_commands_length; i++) {
        auto &source_command = source_commands.at(i);
        auto target_command = new genie::EffectCommand();
        target_command->Type = source_command.Type;
        target_command->A = source_command.A;
        target_command->B = source_command.B;
        target_command->C = source_command.C;
        target_command->D = source_command.D;
        target_effect->EffectCommands.push_back(*target_command);
    }

    df->Effects.push_back(*target_effect);
    return df->Effects.size() - 1;
}

void updateEffect(
    genie::DatFile *df,
    int effect_id, 
    float totalCount) {
    auto &effect = df->Effects.at(effect_id);
    auto &commands = effect.EffectCommands;
    auto commands_count = commands.size();

    auto effect_updated = updated_effects.find(effect_id) != updated_effects.end();
    if (effect_updated) {
        cout << "Effect " << effect_id << " was already updated\n";
        return;
    } else {
        updated_effects.insert(effect_id);
    }

    for (int i = 0; i < commands_count; i++) {
        auto &command = commands.at(i);
        switch (command.Type)
        {
            case 0: // attribute set
            case 10: // team attribute set
            case 20: // enemy attribute set
                multipleAttributeSet(df, commands, i, totalCount);
                break;
            case 7: // spawn units
            case 17: // team spawn units
            case 27: // enemy spawn units
                multipleSpawnUnit(commands, i, totalCount);
                break;
            case 4: // attribute +-
            case 14: // team attribute +-
            case 24: // team attribute +-
                if (command.C >= 103 && command.C <= 106) {
                    multipleDecreaseUnitCost(df, commands, i, totalCount);
                } else if (command.C == 11) {
                    auto base_accuracy = df->Civs.at(0).Units.at(command.A).Type50.AccuracyPercent;
                    command.D = calculatePercentIncrease(command.D, base_accuracy, totalCount) - base_accuracy;
                } else {
                    multipleAddFunc(commands, i, totalCount);
                }
                break;
            case 1: // resource +-
            case 11: // team resource +-
            case 21: // enemy resource +-
                if (command.C == -1) {
                    multipleResourceSet(commands, i, totalCount);
                } else {
                    multipleAddFunc(commands, i, totalCount);
                }
                break;
            case 5: // attribute */
            case 15: // team attribute */
            case 25: // enemy attribute */
                if (command.C == 13) {
                    multipleMultiplyWorkRate(commands, i, totalCount);
                } else if (command.C == 0) {
                    multipleMultiplyFunc(commands, i, min(totalCount, 3.0f));
                } else {
                    multipleMultiplyFunc(commands, i, totalCount);
                }
                break;
            case 6: // resource modifier */
            case 16: // team resource modifier */
            case 26: // enemy resource modifier */
                multipleMultiplyFunc(commands, i, totalCount);
                break;
            case 101: // tech cost modifier
                multipleTechCostModifier(df, commands, i, totalCount);
                break;
            case 103: // tech cost modifier
                multipleTechTimeModifier(df, commands, i, totalCount);
                break;
        }
    }
}

void updateUnitFromBase(
    genie::DatFile *df,
    int eliteId,
    int baseId,
    float totalCount) {
    for (auto &civ : df->Civs) {
        auto &elite_unit = civ.Units.at(eliteId);
        auto &base_unit = civ.Units.at(baseId);
        
        elite_unit.HitPoints = calculateDiff(elite_unit.HitPoints, base_unit.HitPoints, totalCount);
        elite_unit.Speed = calculateDiff(elite_unit.Speed, base_unit.Speed, totalCount);
        elite_unit.LineOfSight = calculateDiff(elite_unit.LineOfSight, base_unit.LineOfSight, totalCount);
        elite_unit.Type50.MaxRange = calculateDiff(elite_unit.Type50.MaxRange, base_unit.Type50.MaxRange, totalCount);
        elite_unit.Type50.DisplayedRange = calculateDiff(elite_unit.Type50.DisplayedRange, base_unit.Type50.DisplayedRange, totalCount);
        elite_unit.Type50.DisplayedAttack = calculateDiff(elite_unit.Type50.DisplayedAttack, base_unit.Type50.DisplayedAttack, totalCount);
        elite_unit.Type50.DisplayedMeleeArmour = calculateDiff(elite_unit.Type50.DisplayedMeleeArmour, base_unit.Type50.DisplayedMeleeArmour, totalCount);
        elite_unit.Type50.DisplayedReloadTime = calculateDiff(elite_unit.Type50.DisplayedReloadTime, base_unit.Type50.DisplayedReloadTime, totalCount);
        elite_unit.Type50.AccuracyDispersion = calculateReduction(elite_unit.Type50.AccuracyDispersion, base_unit.Type50.AccuracyDispersion, totalCount);
        elite_unit.Type50.AccuracyPercent = (int16_t)calculatePercentIncrease((float)elite_unit.Type50.AccuracyPercent, (float)base_unit.Type50.AccuracyPercent, totalCount);
        elite_unit.Type50.BlastDamage = calculateDiff(elite_unit.Type50.BlastDamage, base_unit.Type50.BlastDamage, totalCount);
        elite_unit.Type50.BlastWidth = calculateDiff(elite_unit.Type50.BlastWidth, base_unit.Type50.BlastWidth, totalCount);
        elite_unit.Type50.ReloadTime = calculateReduction(elite_unit.Type50.ReloadTime, base_unit.Type50.ReloadTime, totalCount);
        elite_unit.Type50.FrameDelay = (int16_t)calculateReduction((float)elite_unit.Type50.FrameDelay, (float)base_unit.Type50.FrameDelay, totalCount);
        elite_unit.Creatable.MaxCharge = calculateDiff(elite_unit.Creatable.MaxCharge, base_unit.Creatable.MaxCharge, totalCount);
        elite_unit.Creatable.RechargeRate = calculateDiff(elite_unit.Creatable.RechargeRate, base_unit.Creatable.RechargeRate, totalCount);
        elite_unit.Creatable.MaxTotalProjectiles = calculateDiff(elite_unit.Creatable.MaxTotalProjectiles, base_unit.Creatable.MaxTotalProjectiles, totalCount);
        elite_unit.Creatable.TotalProjectiles = calculateDiff(elite_unit.Creatable.TotalProjectiles, base_unit.Creatable.TotalProjectiles, totalCount);
        elite_unit.Creatable.DisplayedPierceArmour = calculateDiff(elite_unit.Creatable.DisplayedPierceArmour, base_unit.Creatable.DisplayedPierceArmour, totalCount);
        elite_unit.Creatable.TrainTime = (int16_t)calculateReduction((float)elite_unit.Creatable.TrainTime, (float)base_unit.Creatable.TrainTime, totalCount);
        elite_unit.Bird.SearchRadius = calculateDiff(elite_unit.Bird.SearchRadius, base_unit.Bird.SearchRadius, totalCount);
        elite_unit.Bird.WorkRate = calculateDiff(elite_unit.Bird.WorkRate, base_unit.Bird.WorkRate, totalCount);

        auto elite_tasks_count = elite_unit.Bird.TaskList.size();
        auto base_tasks_count = base_unit.Bird.TaskList.size();
        for (int i = 0; i < elite_tasks_count; i++) {
            for (int j = 0; j < base_tasks_count; j++) {
                if (elite_unit.Bird.TaskList.at(i).ActionType == base_unit.Bird.TaskList.at(j).ActionType && 
                    elite_unit.Bird.TaskList.at(i).UnitID == base_unit.Bird.TaskList.at(j).UnitID && 
                    elite_unit.Bird.TaskList.at(i).ResourceIn == base_unit.Bird.TaskList.at(j).ResourceIn && 
                    elite_unit.Bird.TaskList.at(i).ResourceOut == base_unit.Bird.TaskList.at(j).ResourceOut && 
                    elite_unit.Bird.TaskList.at(i).ID == base_unit.Bird.TaskList.at(j).ID) {
                    elite_unit.Bird.TaskList.at(i).WorkValue1 = calculateDiff(elite_unit.Bird.TaskList.at(i).WorkValue1, base_unit.Bird.TaskList.at(j).WorkValue1, totalCount);
                    elite_unit.Bird.TaskList.at(i).WorkValue2 = calculateDiff(elite_unit.Bird.TaskList.at(i).WorkValue2, base_unit.Bird.TaskList.at(j).WorkValue2, totalCount);
                    break;
                }
            }
        }

        auto elite_storages_count = elite_unit.ResourceStorages.size();
        auto base_storages_count = base_unit.ResourceStorages.size();
        for (int i = 0; i < elite_storages_count; i++) {
            for (int j = 0; j < base_storages_count; j++) {
                if (elite_unit.ResourceStorages.at(i).Type == base_unit.ResourceStorages.at(j).Type) {
                    elite_unit.ResourceStorages.at(i).Amount = calculateDiff(elite_unit.ResourceStorages.at(i).Amount, base_unit.ResourceStorages.at(j).Amount, totalCount);
                    break;
                }
            }
        }

        auto attacks_count = elite_unit.Type50.Attacks.size();
        auto base_attacks_count = base_unit.Type50.Attacks.size();
        for (int i = 0; i < attacks_count; i++) {
            for (int j = 0; j < base_attacks_count; j++) {
                if (elite_unit.Type50.Attacks.at(i).Class == base_unit.Type50.Attacks.at(j).Class) {
                    elite_unit.Type50.Attacks.at(i).Amount = calculateDiff(elite_unit.Type50.Attacks.at(i).Amount, base_unit.Type50.Attacks.at(j).Amount, totalCount);
                    break;
                }
            }
        }

        auto armours_count = elite_unit.Type50.Armours.size();
        auto base_armours_count = base_unit.Type50.Armours.size();
        for (int i = 0; i < armours_count; i++) {
            for (int j = 0; j < base_armours_count; j++) {
                if (elite_unit.Type50.Armours.at(i).Class == base_unit.Type50.Armours.at(j).Class) {
                    elite_unit.Type50.Armours.at(i).Amount = calculateDiff(elite_unit.Type50.Armours.at(i).Amount, base_unit.Type50.Armours.at(j).Amount, totalCount);
                    break;
                }
            }
        }
    }
}

#pragma endregion Helper_Methods

void aztecsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_JAGUAR_WARRIOR, JAGUAR_WARRIOR, totalCount);

    skipUpdate(346);
    skipUpdate(442);
    skipUpdate(444);

    const int AZT_TECH_TREE = 447;
    updateEffect(df, AZT_TECH_TREE, totalCount);
    
    const int AZT_ATLATL = 514;
    updateEffect(df, AZT_ATLATL, totalCount);
    
    const int AZT_GARLAND_WARS = 465;
    updateEffect(df, AZT_GARLAND_WARS, totalCount);
    
    const int AZT_MONK_HP_1 = 470;
    updateEffect(df, AZT_MONK_HP_1, totalCount);
    
    const int AZT_MONK_HP_2 = 471;
    updateEffect(df, AZT_MONK_HP_2, totalCount);
    
    const int AZT_MONK_HP_3 = 472;
    updateEffect(df, AZT_MONK_HP_3, totalCount);
    
    const int AZT_MONK_HP_4 = 473;
    updateEffect(df, AZT_MONK_HP_4, totalCount);
    
    const int AZT_MONK_HP_5 = 474;
    updateEffect(df, AZT_MONK_HP_5, totalCount);
    
    const int AZT_MONK_HP_6 = 475;
    updateEffect(df, AZT_MONK_HP_6, totalCount);
    
    const int AZT_MONK_HP_7 = 476;
    updateEffect(df, AZT_MONK_HP_7, totalCount);
    
    const int AZT_MONK_HP_8 = 477;
    updateEffect(df, AZT_MONK_HP_8, totalCount);
    
    const int AZT_MONK_HP_9 = 478;
    updateEffect(df, AZT_MONK_HP_9, totalCount);
    
    const int AZT_MONK_HP_10 = 512;
    updateEffect(df, AZT_MONK_HP_10, totalCount);

    const int AZT_START_RES = 213;
    updateEffect(df, AZT_START_RES, totalCount);
    
    const int AZT_RELIC = 488;
    updateEffect(df, AZT_RELIC, totalCount);
}

void bengalisCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, 1761, 1759, totalCount);
    updateUnitFromBase(df, 1740, 1738, totalCount);

    skipUpdate(846);
    skipUpdate(847);

    const int BENG_TECH_TREE = 840;
    updateEffect(df, BENG_TECH_TREE, totalCount);

    const int BENG_PAIKS = 852;
    updateEffect(df, BENG_PAIKS, totalCount);

    const int BENG_MAHAYANA = 853;
    updateEffect(df, BENG_MAHAYANA, totalCount);

    const int BENG_RESIST = 865;
    updateEffect(df, BENG_RESIST, totalCount);

    const int BENG_VILS = 866;
    updateEffect(df, BENG_VILS, totalCount);

    const int BENG_CAV_BONUS = 888;
    updateEffect(df, BENG_CAV_BONUS, totalCount);

    const int BENG_SHIPS_REGENERATE = 867;
    updateEffect(df, BENG_SHIPS_REGENERATE, totalCount);

    const int BENG_MONK_DEFENSE = 568;
    updateEffect(df, BENG_MONK_DEFENSE, totalCount);

    const int BENG_TRADE = 841;
    updateEffect(df, BENG_TRADE, totalCount);
}

void berbersCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CAMEL_ARCHER, CAMEL_ARCHER, totalCount);
    updateUnitFromBase(df, ELITE_GENITOUR, GENITOUR, totalCount);
    updateUnitFromBase(df, 596, 583, totalCount);

    skipUpdate(594);
    skipUpdate(595);
    skipUpdate(624);

    const int BERB_TECH_TREE = 37;
    updateEffect(df, BERB_TECH_TREE, totalCount);

    const int BERB_KASBAH = 607;
    updateEffect(df, BERB_KASBAH, totalCount);

    const int BERB_MAGHREBI_CAMELS = 608;
    updateEffect(df, BERB_MAGHREBI_CAMELS, totalCount);

    const int BERB_VILLAGERS_FASTER_1 = 609;
    updateEffect(df, BERB_VILLAGERS_FASTER_1, totalCount);

    const int BERB_VILLAGERS_FASTER_2 = 626;
    updateEffect(df, BERB_VILLAGERS_FASTER_2, totalCount);

    const int BERB_CAVALRY_DISCOUNT_1 = 610;
    updateEffect(df, BERB_CAVALRY_DISCOUNT_1, totalCount);

    const int BERB_CAVALRY_DISCOUNT_2 = 638;
    updateEffect(df, BERB_CAVALRY_DISCOUNT_2, totalCount);

    const int BERB_SHIPS_FASTER_1 = 34;
    updateEffect(df, BERB_SHIPS_FASTER_1, totalCount);

    const int BERB_SHIPS_FASTER_2 = 611;
    updateEffect(df, BERB_SHIPS_FASTER_2, totalCount);
}

void bohemiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, HOUFNICE, BOMBARD_CANNON, totalCount);
    updateUnitFromBase(df, ELITE_HUSSITE_WAGON, HUSSITE_WAGON, totalCount);

    skipUpdate(807);
    skipUpdate(808);
    skipUpdate(814);

    const int BOH_TECH_TREE = 803;
    updateEffect(df, BOH_TECH_TREE, totalCount);

    const int BOH_WAGENBURG_TACTICS = 811;
    updateEffect(df, BOH_WAGENBURG_TACTICS, totalCount);

    const int BOH_HUSSITE_REFORMS = 812;
    updateEffect(df, BOH_HUSSITE_REFORMS, totalCount);

    const int BOH_BLACKSMITH_DISCOUNT = 830;
    updateEffect(df, BOH_BLACKSMITH_DISCOUNT, totalCount);

    const int BOH_SPEAR_BONUS = 824;
    updateEffect(df, BOH_SPEAR_BONUS, totalCount);

    const int BOH_VIL_SANCTITY = 825;
    updateEffect(df, BOH_VIL_SANCTITY, totalCount);

    const int BOH_VIL_FERVOR = 826;
    updateEffect(df, BOH_VIL_FERVOR, totalCount);

    const int BOH_MARKET_SPEED = 804;
    updateEffect(df, BOH_MARKET_SPEED, totalCount);
}

void britonsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_LONGBOWMAN, LONGBOWMAN, totalCount);

    skipUpdate(251);
    skipUpdate(358);

    const int BRIT_TECH_TREE = 254;
    updateEffect(df, BRIT_TECH_TREE, totalCount);
    
    const int BRIT_YEOMEN = 455;
    updateEffect(df, BRIT_YEOMEN, totalCount);
    
    const int BRIT_WARWOLF = 540;
    updateEffect(df, BRIT_WARWOLF, totalCount    );
    
    const int BRIT_TC_DISCOUNT = 379;
    updateEffect(df, BRIT_TC_DISCOUNT, totalCount);

    const int BRIT_ARCHER_RANGE_1 = 380;
    updateEffect(df, BRIT_ARCHER_RANGE_1, totalCount);

    const int BRIT_ARCHER_RANGE_2 = 415;
    updateEffect(df, BRIT_ARCHER_RANGE_2, totalCount);

    const int BRIT_SHEPARD = 381;
    updateEffect(df, BRIT_SHEPARD, totalCount);

    const int BRIT_RANGE_SPEED = 399;
    updateEffect(df, BRIT_RANGE_SPEED, totalCount);
}

void bulgariansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KONNIK, KONNIK, totalCount);
    updateUnitFromBase(df, ELITE_KONNIK_2, KONNIK_2, totalCount);
    updateUnitFromBase(df, DISMOUNTED_ELITE_KONNIK, DISMOUNTED_KONNIK, totalCount);

    skipUpdate(714);
    skipUpdate(715);

    const int BULG_TECH_TREE = 706;
    updateEffect(df, BULG_TECH_TREE, totalCount);

    const int BULG_STIRRUPS = 722;
    updateEffect(df, BULG_STIRRUPS, totalCount);

    const int BULG_BAGAINS = 723;
    updateEffect(df, BULG_BAGAINS, totalCount);

    const int BULG_MILITIA_UPGRADES = 730;
    updateEffect(df, BULG_MILITIA_UPGRADES, totalCount);

    const int BULG_TC_STONE_DISCOUNT_1 = 171;
    updateEffect(df, BULG_TC_STONE_DISCOUNT_1, totalCount);
    
    const int BULG_TC_STONE_DISCOUNT_2 = 731;
    updateEffect(df, BULG_TC_STONE_DISCOUNT_2, totalCount);

    const int BULG_BLACKSMITH_FASTER = 707;
    updateEffect(df, BULG_BLACKSMITH_FASTER, totalCount);

    const int BULD_ENABLE_CREPOST = 732;
    updateEffect(df, BULD_ENABLE_CREPOST, totalCount);
    
    const int BULG_KREPOST = 1251;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(BULG_KREPOST);
        unit.Creatable.ResourceCosts.at(0).Amount = multipleChangeFrom(unit.Creatable.ResourceCosts.at(0).Amount, totalCount, 650);
    }
}

void burgundiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_COUSTILLIER, COUSTILLIER, totalCount);

    skipUpdate(786);
    skipUpdate(787);

    const int BURG_TECH_TREE = 782;
    updateEffect(df, BURG_TECH_TREE, totalCount);

    const int BURG_BURGUNDIAN_VINEYARDS = 790;
    updateEffect(df, BURG_BURGUNDIAN_VINEYARDS, totalCount);

    const int BURG_FLEMISH_REVOLUTION = 791;
    updateEffect(df, BURG_FLEMISH_REVOLUTION, totalCount);

    const int BURG_GUNPOWDER = 794;
    updateEffect(df, BURG_GUNPOWDER, totalCount);

    const int BURG_RELIC_FOOD = 783;
    updateEffect(df, BURG_RELIC_FOOD, totalCount);
}

void burmeseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_ARAMBAI, ARAMBAI, totalCount);

    skipUpdate(658);
    skipUpdate(659);

    const int BURM_TECH_TREE = 650;
    updateEffect(df, BURM_TECH_TREE, totalCount);

    const int BURM_HOWDAH = 666;
    updateEffect(df, BURM_HOWDAH, totalCount);

    const int BURM_MANIPUR_CAVALRY = 667;
    updateEffect(df, BURM_MANIPUR_CAVALRY, totalCount);

    const int BURM_LUMBER_UPGRADES = 685;
    updateEffect(df, BURM_LUMBER_UPGRADES, totalCount);

    const int BURM_INF_ATTACK_1 = 686;
    updateEffect(df, BURM_INF_ATTACK_1, totalCount);

    const int BURM_INF_ATTACK_2 = 687;
    updateEffect(df, BURM_INF_ATTACK_2, totalCount);

    const int BURM_INF_ATTACK_3 = 688;
    updateEffect(df, BURM_INF_ATTACK_3, totalCount);

    const int BURM_INF_ATTACK_4 = 699;
    updateEffect(df, BURM_INF_ATTACK_4, totalCount);

    const int BURM_ELEP_ARMOR = 679;
    updateEffect(df, BURM_ELEP_ARMOR, totalCount);

    const int BURM_MONASTERY_DISCOUNT = 689;
    updateEffect(df, BURM_MONASTERY_DISCOUNT, totalCount);
}

void byzantinesCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CATAPHRACT, CATAPHRACT, totalCount);

    skipUpdate(264);
    skipUpdate(359);

    const int BYZ_TECH_TREE = 256;
    updateEffect(df, BYZ_TECH_TREE, totalCount);

    const int BYZ_GREEK_FIRE = 518;
    updateEffect(df, BYZ_GREEK_FIRE, totalCount);

    const int BYZ_LOGISTICA = 493;
    updateEffect(df, BYZ_LOGISTICA, totalCount);

    const int BYZ_OBSOLETE = 413;
    updateEffect(df, BYZ_OBSOLETE, totalCount);

    const int BYZ_BUILDINGS_HP_1 = 282;
    updateEffect(df, BYZ_BUILDINGS_HP_1, totalCount);

    const int BYZ_BUILDINGS_HP_2 = 429;
    updateEffect(df, BYZ_BUILDINGS_HP_2, totalCount);

    const int BYZ_BUILDINGS_HP_3 = 430;
    updateEffect(df, BYZ_BUILDINGS_HP_3, totalCount);

    const int BYZ_BUILDINGS_HP_4 = 431;
    updateEffect(df, BYZ_BUILDINGS_HP_4, totalCount);

    const int BYZ_COUNTER_DISCOUNT = 283;
    updateEffect(df, BYZ_COUNTER_DISCOUNT, totalCount);

    const int BYZ_SHIP_ATTACKSPEED = 396;
    updateEffect(df, BYZ_SHIP_ATTACKSPEED, totalCount);

    const int BYZ_HEALSPEED = 400;
    updateEffect(df, BYZ_HEALSPEED, totalCount);
}

void celtsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_WOAD_RAIDER, WOAD_RAIDER, totalCount);

    skipUpdate(274);
    skipUpdate(368);

    const int CELT_TECH_TREE = 275;
    updateEffect(df, CELT_TECH_TREE, totalCount);

    const int CELT_STRONGHOLD = 537;
    updateEffect(df, CELT_STRONGHOLD, totalCount);

    const int CELT_FUROR_CELTICA = 239;
    updateEffect(df, CELT_FUROR_CELTICA, totalCount);

    const int CELT_INF_SPEED = 392;
    updateEffect(df, CELT_INF_SPEED, totalCount);

    const int CELT_LUMBERJACK = 384;
    updateEffect(df, CELT_LUMBERJACK, totalCount);

    const int CELT_SIEGE_FIRING = 385;
    updateEffect(df, CELT_SIEGE_FIRING, totalCount);

    const int CELT_DOMINANT_LOS = 417;
    updateEffect(df, CELT_DOMINANT_LOS, totalCount);

    const int CELTS_SIEGEWORKSHOP_SPEED = 401;
    updateEffect(df, CELTS_SIEGEWORKSHOP_SPEED, totalCount);

    const int CELTS_STRONGHOLD_CASTLE = 82;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(CELTS_STRONGHOLD_CASTLE);

        auto tasks_count = unit.Bird.TaskList.size();
        for (int i = 0; i < tasks_count; i++) {
            unit.Bird.TaskList.at(i).WorkValue1 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue1, totalCount);
            unit.Bird.TaskList.at(i).WorkValue2 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue2, totalCount);
        }
    }
}

void chineseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CHU_KO_NU, CHU_KO_NU, totalCount);

    skipUpdate(265);
    skipUpdate(360);

    const int CHIN_TECH_TREE = 257;
    updateEffect(df, CHIN_TECH_TREE, totalCount);

    const int CHIN_GREAT_WALL = 516;
    updateEffect(df, CHIN_GREAT_WALL, totalCount);

    const int CHIN_ROCKETRY = 483;
    updateEffect(df, CHIN_ROCKETRY, totalCount);

    const int CHIN_START_RES = 215;
    updateEffect(df, CHIN_START_RES, totalCount);

    const int CHIN_ADDITIONAL_VILS = 302;
    updateEffect(df, CHIN_ADDITIONAL_VILS, totalCount);

    const int CHIN_TECH_DISCOUNT_1 = 304;
    updateEffect(df, CHIN_TECH_DISCOUNT_1, totalCount);

    const int CHIN_TECH_DISCOUNT_2 = 349;
    updateEffect(df, CHIN_TECH_DISCOUNT_2, totalCount);

    const int CHIN_TECH_DISCOUNT_3 = 350;
    updateEffect(df, CHIN_TECH_DISCOUNT_3, totalCount);

    const int CHIN_TECH_DISCOUNT_4 = 351;
    updateEffect(df, CHIN_TECH_DISCOUNT_4, totalCount);

    const int CHIN_TC_POP = 437;
    updateEffect(df, CHIN_TC_POP, totalCount);

    const int CHIN_DEMO_HP = 395;
    updateEffect(df, CHIN_DEMO_HP, totalCount);

    const int CHIN_FARM_FOOD_1 = 402;
    updateEffect(df, CHIN_FARM_FOOD_1, totalCount);

    const int CHIN_FARM_FOOD_2 = 831;
    updateEffect(df, CHIN_FARM_FOOD_2, totalCount);

    const int CHIN_FARM_FOOD_3 = 832;
    updateEffect(df, CHIN_FARM_FOOD_3, totalCount);

    const int CHIN_FARM_FOOD_4 = 833;
    updateEffect(df, CHIN_FARM_FOOD_4, totalCount);

    const int CHIN_FARM_FOOD_5 = 834;
    updateEffect(df, CHIN_FARM_FOOD_5, totalCount);

    const int CHIN_FARM_FOOD_6 = 835;
    updateEffect(df, CHIN_FARM_FOOD_6, totalCount);

    const int CHIN_FARM_FOOD_7 = 836;
    updateEffect(df, CHIN_FARM_FOOD_7, totalCount);

    const int CHIN_FARM_FOOD_8 = 837;
    updateEffect(df, CHIN_FARM_FOOD_8, totalCount);
}

void cumansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KIPCHAK, KIPCHAK, totalCount);
    updateUnitFromBase(df, 1260, KIPCHAK, totalCount);

    skipUpdate(718);
    skipUpdate(719);
    skipUpdate(759);
    skipUpdate(760); 

    const int CUMANS_TECH_TREE = 710;
    updateEffect(df, CUMANS_TECH_TREE, totalCount);

    const int CUMANS_STEPPE_HUSBANDRY = 726;
    updateEffect(df, CUMANS_STEPPE_HUSBANDRY, totalCount);

    const int CUMANS_CUMAN_MERCENARIES = 743;
    updateEffect(df, CUMANS_CUMAN_MERCENARIES, totalCount);

    const int CUMANS_CUMAN_MERCENARIES_2 = 727;
    updateEffect(df, CUMANS_CUMAN_MERCENARIES_2, totalCount);

    const int CUMANS_TC = 761;
    updateEffect(df, CUMANS_TC, totalCount);

    const int CUMANS_SIEGE = 742;
    updateEffect(df, CUMANS_SIEGE, totalCount);

    const int CUMANS_WOOD_DISCOUNT = 697;
    updateEffect(df, CUMANS_WOOD_DISCOUNT, totalCount);

    const int CUMANS_CAV_FASTER_1 = 746;
    updateEffect(df, CUMANS_CAV_FASTER_1, totalCount);

    const int CUMANS_CAV_FASTER_2 = 748;
    updateEffect(df, CUMANS_CAV_FASTER_2, totalCount);

    const int CUMANS_CAV_FASTER_3 = 762;
    updateEffect(df, CUMANS_CAV_FASTER_3, totalCount);

    const int CUMANS_CAV_FASTER_4 = 763;
    updateEffect(df, CUMANS_CAV_FASTER_4, totalCount);

    const int CUMANS_PALISADE_HP = 711;
    updateEffect(df, CUMANS_PALISADE_HP, totalCount);
}

void dravidiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, 1737, 1735, totalCount);

    skipUpdate(844);
    skipUpdate(845);

    const int DRAV_TECH_TREE = 838;
    updateEffect(df, DRAV_TECH_TREE, totalCount);

    const int DRAV_MEDICAL_CORPS = 850;
    updateEffect(df, DRAV_MEDICAL_CORPS, totalCount);

    const int DRAV_WOOTZ_STEEL = 851;
    updateEffect(df, DRAV_WOOTZ_STEEL, totalCount);

    const int DRAV_THIRISADAI = 860;
    updateEffect(df, DRAV_THIRISADAI, totalCount);

    const int DRAV_BONUS_WOOD = 868;
    updateEffect(df, DRAV_BONUS_WOOD, totalCount);

    const int DRAV_FISHING_CARRY = 863;
    updateEffect(df, DRAV_FISHING_CARRY, totalCount);

    const int DRAV_SIEGE_DISCOUNT = 887;
    updateEffect(df, DRAV_SIEGE_DISCOUNT, totalCount);

    const int DRAV_SKIRM_ATTACKFASTER = 864;
    updateEffect(df, DRAV_SKIRM_ATTACKFASTER, totalCount);

    const int DRAV_DOCK_POP = 839;
    updateEffect(df, DRAV_DOCK_POP, totalCount);
}

void ethiopiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SHOTEL_WARRIOR, SHOTEL_WARRIOR, totalCount);

    skipUpdate(598);
    skipUpdate(599);

    const int ETH_TECH_TREE = 48;
    updateEffect(df, ETH_TECH_TREE, totalCount);

    const int ETH_ROYAL_HEIRS = 603;
    updateEffect(df, ETH_ROYAL_HEIRS, totalCount);

    const int ETH_TORSION_ENGINES = 604;
    updateEffect(df, ETH_TORSION_ENGINES, totalCount);

    const int ETH_TORSION_ENGINES_2 = 634;
    updateEffect(df, ETH_TORSION_ENGINES_2, totalCount);

    const int ETH_ACHER_FIREFASTER = 612;
    updateEffect(df, ETH_ACHER_FIREFASTER, totalCount);

    const int ETH_BONUS_RES_1 = 613;
    updateEffect(df, ETH_BONUS_RES_1, totalCount);

    const int ETH_BONUS_RES_2 = 614;
    updateEffect(df, ETH_BONUS_RES_2, totalCount);

    const int ETH_BONUS_RES_3 = 615;
    updateEffect(df, ETH_BONUS_RES_3, totalCount);

    const int ETH_PAIK_FREE = 616;
    updateEffect(df, ETH_PAIK_FREE, totalCount);

    const int ETH_OUTPOST_LOS = 49;
    updateEffect(df, ETH_OUTPOST_LOS, totalCount);
}

void franksCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_THROWING_AXEMAN, THROWING_AXEMAN, totalCount);

    skipUpdate(272);
    skipUpdate(361);

    const int FRANK_TECH_TREE = 258;
    updateEffect(df, FRANK_TECH_TREE, totalCount);

    const int FRANK_BEARDED_AXE = 291;
    updateEffect(df, FRANK_BEARDED_AXE, totalCount);

    const int FRANK_CHIVALRY = 548;
    updateEffect(df, FRANK_CHIVALRY, totalCount);

    const int FRANK_CASTLE_DISCOUNT = 324;
    updateEffect(df, FRANK_CASTLE_DISCOUNT, totalCount);

    const int FRANK_CAVALRY_HP = 285;
    updateEffect(df, FRANK_CAVALRY_HP, totalCount);

    const int FRANK_FORAGERS = 523;
    updateEffect(df, FRANK_FORAGERS, totalCount);

    const int FRANK_KNIGHT_LOS = 403;
    updateEffect(df, FRANK_KNIGHT_LOS, totalCount);
}

void gothsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_HUSKARL, HUSKARL, totalCount);
    updateUnitFromBase(df, 761, 759, totalCount);

    skipUpdate(363);
    skipUpdate(507);

    const int GOTH_TECH_TREE = 259;
    updateEffect(df, GOTH_TECH_TREE, totalCount);

    const int GOTH_ANARCHY = 462;
    updateEffect(df, GOTH_ANARCHY, totalCount);

    const int GOTH_PERFUSION = 513;
    updateEffect(df, GOTH_PERFUSION, totalCount);
    
    const int GOTH_INF_DISCOUNT_1 = 342;
    updateEffect(df, GOTH_INF_DISCOUNT_1, totalCount);
    
    const int GOTH_INF_DISCOUNT_2 = 765;
    updateEffect(df, GOTH_INF_DISCOUNT_2, totalCount);
    
    const int GOTH_INF_DISCOUNT_3 = 766;
    updateEffect(df, GOTH_INF_DISCOUNT_3, totalCount);
    
    const int GOTH_INF_DISCOUNT_4 = 767;
    updateEffect(df, GOTH_INF_DISCOUNT_4, totalCount);
    
    const int GOTH_INF_DISCOUNT_5 = 344;
    updateEffect(df, GOTH_INF_DISCOUNT_5, totalCount);
    
    const int GOTH_INF_DISCOUNT_6 = 343;
    updateEffect(df, GOTH_INF_DISCOUNT_6, totalCount);

    const int GOTH_INF_BONUS_1 = 326;
    updateEffect(df, GOTH_INF_BONUS_1, totalCount);

    const int GOTH_INF_BONUS_2 = 327;
    updateEffect(df, GOTH_INF_BONUS_2, totalCount);
    
    const int GOTH_INF_BONUS_3 = 328;
    updateEffect(df, GOTH_INF_BONUS_3, totalCount);

    const int GOTH_HUNT = 414;
    updateEffect(df, GOTH_HUNT, totalCount);

    const int GOTH_LOOM_INSTANT = 341;
    updateEffect(df, GOTH_LOOM_INSTANT, totalCount);
    
    const int GOTH_POP = 418;
    updateEffect(df, GOTH_POP, totalCount);
    
    const int GOTH_BARRACKS_WORK = 405;
    updateEffect(df, GOTH_BARRACKS_WORK, totalCount);
}

void gurjarasCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SHRIVAMSHA_RIDER, SHRIVAMSHA_RIDER, totalCount);
    updateUnitFromBase(df, 1743, 1741, totalCount);

    skipUpdate(848);
    skipUpdate(849);
    skipUpdate(861);
    skipUpdate(862);

    const int GUR_TECH_TREE = 842;
    updateEffect(df, GUR_TECH_TREE, totalCount);

    const int GUR_KSHATRIYAS = 854;
    updateEffect(df, GUR_KSHATRIYAS, totalCount);
    
    const int GUR_KSHATRIYAS_SUP = 875;
    updateEffect(df, GUR_KSHATRIYAS_SUP, totalCount);

    const int GUR_FRONTIER_GUARDS = 855;
    updateEffect(df, GUR_FRONTIER_GUARDS, totalCount);

    const int GUR_FORAGEBUSHES = 872;
    updateEffect(df, GUR_FORAGEBUSHES, totalCount);

    const int GUR_MILL_GARISON = 871;
    updateEffect(df, GUR_MILL_GARISON, totalCount);
    
    const int GUR_CAV_BONUS_1 = 869;
    updateEffect(df, GUR_CAV_BONUS_1, totalCount);
    
    const int GUR_CAV_BONUS_2 = 873;
    updateEffect(df, GUR_CAV_BONUS_2, totalCount);
    
    const int GUR_CAV_BONUS_3 = 874;
    updateEffect(df, GUR_CAV_BONUS_3, totalCount);
    
    const int GUR_CAV_BONUS_4 = 884;
    updateEffect(df, GUR_CAV_BONUS_4, totalCount);
    
    const int GUR_DOCK_GARRISON = 870;
    updateEffect(df, GUR_DOCK_GARRISON, totalCount);

    const int GUR_CAMEL_TRAIN = 843;
    updateEffect(df, GUR_CAMEL_TRAIN, totalCount);
}

void hindustanisCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, IMPERIAL_CAMEL_RIDER, HEAVY_CAMEL_RIDER, totalCount);
    updateUnitFromBase(df, 1749, 1747, totalCount);

    skipUpdate(858);
    skipUpdate(859);

    const int HIND_TECH_TREE = 1;
    updateEffect(df, HIND_TECH_TREE, totalCount);

    const int HIND_IMP_CAMEL = 577;
    updateEffect(df, HIND_IMP_CAMEL, totalCount);

    const int HIND_GRAND_TRUNK_ROAD = 562;
    updateEffect(df, HIND_GRAND_TRUNK_ROAD, totalCount);

    const int HIND_SHATAGNI = 563;
    updateEffect(df, HIND_SHATAGNI, totalCount);
    
    const int HIND_VIL_DISCOUNT_1 = 551;
    updateEffect(df, HIND_VIL_DISCOUNT_1, totalCount);
    
    const int HIND_VIL_DISCOUNT_2 = 552;
    updateEffect(df, HIND_VIL_DISCOUNT_2, totalCount);
    
    const int HIND_VIL_DISCOUNT_3 = 553;
    updateEffect(df, HIND_VIL_DISCOUNT_3, totalCount);
    
    const int HIND_VIL_DISCOUNT_4 = 585;
    updateEffect(df, HIND_VIL_DISCOUNT_4, totalCount);

    const int HIND_CAMEL_ATTACKSPEED = 560;
    updateEffect(df, HIND_CAMEL_ATTACKSPEED, totalCount);

    const int HIND_GUNPOWDER_ARMOR = 576;
    updateEffect(df, HIND_GUNPOWDER_ARMOR, totalCount);

    const int HIND_CARAVENSEI_AVAIL = 574;
    updateEffect(df, HIND_CARAVENSEI_AVAIL, totalCount);

    const int HIND_CARAVENSEI = 1754;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(HIND_CARAVENSEI);

        auto tasks_count = unit.Bird.TaskList.size();
        for (int i = 0; i < tasks_count; i++) {
            unit.Bird.TaskList.at(i).WorkValue1 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue1, totalCount);
            unit.Bird.TaskList.at(i).WorkValue2 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue2, totalCount);
        }
    }

    const int HIND_CAV_ATTACK = 2;
    updateEffect(df, HIND_CAV_ATTACK, totalCount);
}

void hunsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_TARKAN, TARKAN, totalCount);
    updateUnitFromBase(df, 887, 886, totalCount);

    skipUpdate(453);
    skipUpdate(454);

    const int HUNS_TECH_TREE = 448;
    updateEffect(df, HUNS_TECH_TREE, totalCount);

    const int HUNS_MARAUERS = 538;
    updateEffect(df, HUNS_MARAUERS, totalCount);

    const int HUNS_ATHEISM = 464;
    updateEffect(df, HUNS_ATHEISM, totalCount);

    const int HUNS_START_RES = 214;
    updateEffect(df, HUNS_START_RES, totalCount);

    const int HUNS_CA_DISCOUNT_1 = 497;
    updateEffect(df, HUNS_CA_DISCOUNT_1, totalCount);

    const int HUNS_CA_DISCOUNT_2 = 498;
    updateEffect(df, HUNS_CA_DISCOUNT_2, totalCount);

    const int HUNS_FASTER_STABLES = 484;
    updateEffect(df, HUNS_FASTER_STABLES, totalCount);
}

void incasCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KAMAYUK, KAMAYUK, totalCount);

    skipUpdate(40);
    skipUpdate(564);
    skipUpdate(565);
    skipUpdate(582);

    const int INC_TECH_TREE = 3;
    updateEffect(df, INC_TECH_TREE, totalCount);

    const int INC_ANDEAN_SLING = 572;
    updateEffect(df, INC_ANDEAN_SLING, totalCount);

    const int INC_FABRIC_SHIELDS = 573;
    updateEffect(df, INC_FABRIC_SHIELDS, totalCount);

    const int INC_FOOD_DISCOUNT_1 = 156;
    updateEffect(df, INC_FOOD_DISCOUNT_1, totalCount);
    
    const int INC_FOOD_DISCOUNT_2 = 158;
    updateEffect(df, INC_FOOD_DISCOUNT_2, totalCount);
    
    const int INC_FOOD_DISCOUNT_3 = 159;
    updateEffect(df, INC_FOOD_DISCOUNT_3, totalCount);
    
    const int INC_FOOD_DISCOUNT_4 = 160;
    updateEffect(df, INC_FOOD_DISCOUNT_4, totalCount);
    
    const int INC_VIL_BLACKSMITH_1 = 529;
    updateEffect(df, INC_VIL_BLACKSMITH_1, totalCount);
    
    const int INC_VIL_BLACKSMITH_2 = 530;
    updateEffect(df, INC_VIL_BLACKSMITH_2, totalCount);
    
    const int INC_VIL_BLACKSMITH_3 = 531;
    updateEffect(df, INC_VIL_BLACKSMITH_3, totalCount);
    
    const int INC_VIL_BLACKSMITH_4 = 532;
    updateEffect(df, INC_VIL_BLACKSMITH_4, totalCount);
    
    const int INC_VIL_BLACKSMITH_5 = 533;
    updateEffect(df, INC_VIL_BLACKSMITH_5, totalCount);
    
    const int INC_VIL_BLACKSMITH_6 = 534;
    updateEffect(df, INC_VIL_BLACKSMITH_6, totalCount);
    
    const int INC_HOUSES_POP = 550;
    updateEffect(df, INC_HOUSES_POP, totalCount);
    
    const int INC_STONE_DISCOUNT = 575;
    updateEffect(df, INC_STONE_DISCOUNT, totalCount);
    
    const int INC_SPAWN_LAMA = 678;
    updateEffect(df, INC_SPAWN_LAMA, totalCount    );
}

void italiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_GENOESE_CROSSBOWMAN, GENOESE_CROSSBOWMAN, totalCount);

    skipUpdate(519);
    skipUpdate(520);

    const int ITAL_TECH_TREE = 10;
    updateEffect(df, ITAL_TECH_TREE, totalCount);

    const int ITAL_PAVISE = 549;
    updateEffect(df, ITAL_PAVISE, totalCount);
    
    const int ITAL_SILK_ROAD = 554;
    updateEffect(df, ITAL_SILK_ROAD, totalCount);

    const int ITAL_GUNPOWDER_DISCOUNT = 555;
    updateEffect(df, ITAL_GUNPOWDER_DISCOUNT, totalCount);
}

void japaneseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SAMURAI, SAMURAI, totalCount);

    skipUpdate(250);
    skipUpdate(364);

    const int JAP_TECH_TREE = 255;
    updateEffect(df, JAP_TECH_TREE, totalCount);

    const int JAP_YASAMA = 539;
    updateEffect(df, JAP_YASAMA, totalCount);

    const int JAP_KATAPARUTO = 59;
    updateEffect(df, JAP_KATAPARUTO, totalCount);

    const int JAP_OBSOLETE = 432;
    updateEffect(df, JAP_OBSOLETE, totalCount);

    const int JAP_OBSOLETE_2 = 433;
    updateEffect(df, JAP_OBSOLETE_2, totalCount);

    const int JAP_FISH_IMPROVE = 306;
    updateEffect(df, JAP_FISH_IMPROVE, totalCount);

    const int JAP_FISH_WORK_1 = 434;
    updateEffect(df, JAP_FISH_WORK_1, totalCount);

    const int JAP_FISH_WORK_2 = 435;
    updateEffect(df, JAP_FISH_WORK_2, totalCount);

    const int JAP_FISH_WORK_3 = 436;
    updateEffect(df, JAP_FISH_WORK_3, totalCount);

    const int JAP_DROP_DISCOUNT = 338;
    updateEffect(df, JAP_DROP_DISCOUNT, totalCount);

    const int JAP_ATTACK_SPEED = 339;
    updateEffect(df, JAP_ATTACK_SPEED, totalCount);

    const int JAP_GALLEY_LOS = 406;
    updateEffect(df, JAP_GALLEY_LOS, totalCount);
}

void khmerCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_BALLISTA_ELEPHANT, BALLISTA_ELEPHANT, totalCount);

    skipUpdate(654);
    skipUpdate(655);

    const int KHMER_TECH_TREE = 646;
    updateEffect(df, KHMER_TECH_TREE, totalCount);

    const int KHMER_TUSK_SWORDS = 662;
    updateEffect(df, KHMER_TUSK_SWORDS, totalCount);    

    const int KHMER_DOUBLE_CROSSBOW = 663;
    updateEffect(df, KHMER_DOUBLE_CROSSBOW, totalCount);   

    const int KHMER_ELEP_FASTER = 703;
    updateEffect(df, KHMER_ELEP_FASTER, totalCount);

    const int KHMER_HOUSE_GARRISON = 693;
    updateEffect(df, KHMER_HOUSE_GARRISON, totalCount);

    const int KHMER_SCORP_RANGE = 647;
    updateEffect(df, KHMER_SCORP_RANGE, totalCount);
}

void koreansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_WAR_WAGON, WAR_WAGON, totalCount);
    updateUnitFromBase(df, ELITE_TURTLE_SHIP, TURTLE_SHIP, totalCount);

    skipUpdate(500);
    skipUpdate(501);
    skipUpdate(502);
    skipUpdate(503);

    const int KOREA_TECH_TREE = 504;
    updateEffect(df, KOREA_TECH_TREE, totalCount);

    const int KOREA_EUPSEONG = 541;
    updateEffect(df, KOREA_EUPSEONG, totalCount);

    const int KOREA_SHINKICHON = 506;
    updateEffect(df, KOREA_SHINKICHON, totalCount);

    const int KORA_ARCHER_ARMOR = 508;
    updateEffect(df, KORA_ARCHER_ARMOR, totalCount);

    const int KOREA_WOOD_DISCOUNT = 561;
    updateEffect(df, KOREA_WOOD_DISCOUNT, totalCount);
}

void lithuaniansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_LEITIS, LEITIS, totalCount);

    skipUpdate(720);
    skipUpdate(721);
    skipUpdate(747);
    skipUpdate(817);

    const int LITH_TECH_TREE = 712;
    updateEffect(df, LITH_TECH_TREE, totalCount);

    const int LITH_HILL_FORTS = 728;
    updateEffect(df, LITH_HILL_FORTS, totalCount);

    const int LITH_TOWER_SHIELDS = 729;
    updateEffect(df, LITH_TOWER_SHIELDS, totalCount);

    const int LITH_FOOD_BONUS = 734;
    updateEffect(df, LITH_FOOD_BONUS, totalCount);

    const int LITH_FOOD_BONUS_UNIT = 888;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(LITH_FOOD_BONUS_UNIT);
        unit.ResourceStorages.at(0).Amount = multipleAdd(unit.ResourceStorages.at(0).Amount, totalCount);
    }

    const int LITH_TRASH_FASTER = 745;
    updateEffect(df, LITH_TRASH_FASTER, totalCount);

    const int LITH_RELIC_ATTACK_1 = 736;
    updateEffect(df, LITH_RELIC_ATTACK_1, totalCount);

    const int LITH_RELIC_ATTACK_2 = 737;
    updateEffect(df, LITH_RELIC_ATTACK_2, totalCount);

    const int LITH_RELIC_ATTACK_3 = 738;
    updateEffect(df, LITH_RELIC_ATTACK_3, totalCount);

    const int LITH_RELIC_ATTACK_4 = 739;
    updateEffect(df, LITH_RELIC_ATTACK_4, totalCount);

    const int LITH_MONASTERY_SPEED = 713;
    updateEffect(df, LITH_MONASTERY_SPEED, totalCount);
}

void magyarsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_MAGYAR_HUSZAR, MAGYAR_HUSZAR, totalCount);

    skipUpdate(525);
    skipUpdate(526);

    const int MAG_TECH_TREE = 5;
    updateEffect(df, MAG_TECH_TREE, totalCount);

    const int MAG_RECURVE_BOW = 570;
    updateEffect(df, MAG_RECURVE_BOW, totalCount);

    const int MAG_MERCENARIES = 571;
    updateEffect(df, MAG_MERCENARIES, totalCount);

    const int MAG_ANIMAL_BONUS = 528;
    updateEffect(df, MAG_ANIMAL_BONUS, totalCount);
    
    const int MAG_SCOUTS_DISCOUNT = 524;
    updateEffect(df, MAG_SCOUTS_DISCOUNT, totalCount);

    const int MAG_ARCHER_LOS = 6;
    updateEffect(df, MAG_ARCHER_LOS, totalCount);
}

void malayCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KARAMBIT_WARRIOR, KARAMBIT_WARRIOR, totalCount);

    skipUpdate(656);
    skipUpdate(657);

    const int MALAY_TECH_TREE = 648;
    updateEffect(df, MALAY_TECH_TREE, totalCount);

    const int MALAY_THALASSOCRACY = 664;
    updateEffect(df, MALAY_THALASSOCRACY, totalCount);

    const int MALAY_FORCED_LEVY = 665;
    updateEffect(df, MALAY_FORCED_LEVY, min(totalCount, 3.0f));

    const int MALAY_OBSOLETE = 682;
    updateEffect(df, MALAY_OBSOLETE, totalCount);

    const int MALAY_OBSOLETE_2 = 683;
    updateEffect(df, MALAY_OBSOLETE_2, totalCount);

    const int MALAY_OBSOLETE_3 = 684;
    updateEffect(df, MALAY_OBSOLETE_3, totalCount);

    const int MALAY_FASTER_AGEADVANCE = 674;
    updateEffect(df, MALAY_FASTER_AGEADVANCE, totalCount);
    
    const int MALAY_FISHTRAP_DISCOUNT = 675;
    updateEffect(df, MALAY_FISHTRAP_DISCOUNT, totalCount);
    
    const int MALAY_FISHTRAP_FOOD = 677;
    updateEffect(df, MALAY_FISHTRAP_FOOD, totalCount);
    
    const int MALAY_ELE_DISCOUNT_1 = 695;
    updateEffect(df, MALAY_ELE_DISCOUNT_1, totalCount);
    
    const int MALAY_ELE_DISCOUNT_2 = 696;
    updateEffect(df, MALAY_ELE_DISCOUNT_2, totalCount);

    const int MALAY_DOCK_LOS = 649;
    updateEffect(df, MALAY_DOCK_LOS, totalCount);
}

void maliansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_GBETO, GBETO, totalCount);

    skipUpdate(596);
    skipUpdate(597);

    const int MALI_TECH_TREE = 42;
    updateEffect(df, MALI_TECH_TREE, totalCount);

    const int MALI_TIGUI = 605;
    updateEffect(df, MALI_TIGUI, totalCount);

    const int MALI_FARIMBA = 606;
    updateEffect(df, MALI_FARIMBA, totalCount);
    
    const int MALI_TC_WOOD_COST = 165;
    updateEffect(df, MALI_TC_WOOD_COST, totalCount);
    
    const int MALI_WOOD_DISCOUNT = 617;
    updateEffect(df, MALI_WOOD_DISCOUNT, totalCount);
    
    const int MALI_INF_ARMOR_1 = 618;
    updateEffect(df, MALI_INF_ARMOR_1, totalCount);
    
    const int MALI_INF_ARMOR_2 = 619;
    updateEffect(df, MALI_INF_ARMOR_2, totalCount);
    
    const int MALI_INF_ARMOR_3 = 620;
    updateEffect(df, MALI_INF_ARMOR_3, totalCount);
    
    const int MALI_GOLDMINING = 621;
    updateEffect(df, MALI_GOLDMINING, totalCount);

    const int MALI_UNIVERSITY_WORKRATE = 43;
    updateEffect(df, MALI_UNIVERSITY_WORKRATE, totalCount);
}

void mayansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_PLUMED_ARCHER, PLUMED_ARCHER, totalCount);

    skipUpdate(468);
    skipUpdate(469);

    const int MAY_TECH_TREE = 449;
    updateEffect(df, MAY_TECH_TREE, totalCount);

    const int MAY_HULCHE_JAVELINEERS = 515;
    updateEffect(df, MAY_HULCHE_JAVELINEERS, totalCount);

    const int MAY_EL_DORADO = 456;
    updateEffect(df, MAY_EL_DORADO, totalCount);

    const int MAY_START_RES = 216;
    updateEffect(df, MAY_START_RES, totalCount);

    const int MAY_VILS_SPAWN = 586;
    updateEffect(df, MAY_VILS_SPAWN, totalCount);

    const int MAY_ARCHER_COST_1 = 485;
    updateEffect(df, MAY_ARCHER_COST_1, totalCount);

    const int MAY_ARCHER_COST_2 = 486;
    updateEffect(df, MAY_ARCHER_COST_2, totalCount);

    const int MAY_RESOURCES_LONGER_1 = 771;
    updateEffect(df, MAY_RESOURCES_LONGER_1, totalCount);

    const int MAY_RESOURCES_LONGER_2 = 772;
    updateEffect(df, MAY_RESOURCES_LONGER_2, totalCount);

    const int MAY_RESOURCES_LONGER_3 = 773;
    updateEffect(df, MAY_RESOURCES_LONGER_3, totalCount);

    const int MAY_RESOURCES_LONGER_4 = 774;
    updateEffect(df, MAY_RESOURCES_LONGER_4, totalCount);

    const int MAY_RESOURCES_LONGER_5 = 775;
    updateEffect(df, MAY_RESOURCES_LONGER_5, totalCount);

    const int MAY_RESOURCES_LONGER_6 = 776;
    updateEffect(df, MAY_RESOURCES_LONGER_6, totalCount);

    const int MAY_RESOURCES_LONGER_7 = 777;
    updateEffect(df, MAY_RESOURCES_LONGER_7, totalCount);

    const int MAY_RESOURCES_LONGER_8 = 778;
    updateEffect(df, MAY_RESOURCES_LONGER_8, totalCount);

    const int MAY_RESOURCES_LONGER_9 = 779;
    updateEffect(df, MAY_RESOURCES_LONGER_9, totalCount);

    const int MAY_RESOURCES_LONGER_10 = 780;
    updateEffect(df, MAY_RESOURCES_LONGER_10, totalCount);

    const int MAY_RESOURCES_LONGER_11 = 781;
    updateEffect(df, MAY_RESOURCES_LONGER_11, totalCount);

    const int MAY_WALL_DISCOUNT = 489;
    updateEffect(df, MAY_WALL_DISCOUNT, totalCount);
}

void mongolsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_MANGUDAI, MANGUDAI, totalCount);

    skipUpdate(270);
    skipUpdate(369);

    const int MONG_TECH_TREE = 277;
    updateEffect(df, MONG_TECH_TREE, totalCount);

    const int MONG_NOMADS = 542;
    updateEffect(df, MONG_NOMADS, totalCount);

    const int MONG_NOMADS_2 = 681;
    updateEffect(df, MONG_NOMADS_2, totalCount);

    const int MONG_DRILL = 457;
    updateEffect(df, MONG_DRILL, totalCount);

    const int MONG_CA_FIRING = 393;
    updateEffect(df, MONG_CA_FIRING, totalCount);

    const int MONG_LIGHTCAV_HP = 387;
    updateEffect(df, MONG_LIGHTCAV_HP, totalCount);

    const int MONG_HUNTERS = 388;
    updateEffect(df, MONG_HUNTERS, totalCount);

    const int MONG_SCOUT_LOS = 407;
    updateEffect(df, MONG_SCOUT_LOS, totalCount);
}

void persiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_WAR_ELEPHANT, WAR_ELEPHANT, totalCount);

    skipUpdate(271);
    skipUpdate(365);

    const int PERS_TECH_TREE = 260;
    updateEffect(df, PERS_TECH_TREE, totalCount);

    const int PERS_MAHOUTS = 458;
    updateEffect(df, PERS_MAHOUTS, totalCount);

    const int PERS_KAMANDARAN = 543;
    skipUpdate(PERS_KAMANDARAN);

    const int PERS_START_RES = 212;
    updateEffect(df, PERS_START_RES, totalCount);

    const int PERS_TC_HP = 340;
    updateEffect(df, PERS_TC_HP, totalCount);

    const int PERS_DOCK_HP = 347;
    updateEffect(df, PERS_DOCK_HP, totalCount);

    const int PERS_TC_SPEED = 421;
    updateEffect(df, PERS_TC_SPEED, totalCount);

    const int PERS_DOCK_SPEED = 424;
    updateEffect(df, PERS_DOCK_SPEED, totalCount);

    const int PERS_KNIGHT_BONUS = 408;
    updateEffect(df, PERS_KNIGHT_BONUS, totalCount);
}

void polesCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_OBUCH, OBUCH, totalCount);
    updateUnitFromBase(df, 1707, LIGHT_CAVALRY, totalCount);
    updateUnitFromBase(df, 1734, 68, totalCount);
    updateUnitFromBase(df, 1711, 129, totalCount);
    updateUnitFromBase(df, 1720, 130, totalCount);

    skipUpdate(805);
    skipUpdate(806);

    const int POL_TECH_TREE = 801;
    updateEffect(df, POL_TECH_TREE, totalCount);

    const int POL_SZLACHTA_PRIVILEGES = 809;
    updateEffect(df, POL_SZLACHTA_PRIVILEGES, totalCount);

    const int POL_LECHITIC_LEGACY = 810;
    updateEffect(df, POL_LECHITIC_LEGACY, totalCount);
    
    const int POL_VIL_REGEN = 815;
    updateEffect(df, POL_VIL_REGEN, totalCount);
    
    const int POL_FOLWARK = 816;
    updateEffect(df, POL_FOLWARK, totalCount);
    
    const int POL_FOLWARK_2 = 818;
    updateEffect(df, POL_FOLWARK_2, totalCount);
    
    const int POL_FOLWARK_3 = 819;
    updateEffect(df, POL_FOLWARK_3, totalCount);
    
    const int POL_FOLWARK_4 = 820;
    updateEffect(df, POL_FOLWARK_4, totalCount);
    
    const int POL_FOLWARKFARM_FOOD_1 = 821;
    updateEffect(df, POL_FOLWARKFARM_FOOD_1, totalCount);
    
    const int POL_FOLWARKFARM_FOOD_2 = 822;
    updateEffect(df, POL_FOLWARKFARM_FOOD_2, totalCount);
    
    const int POL_FOLWARKFARM_FOOD_3 = 823;
    updateEffect(df, POL_FOLWARKFARM_FOOD_3, totalCount);
    
    const int POL_STONE_GOLDGEN_1 = 827;
    updateEffect(df, POL_STONE_GOLDGEN_1, totalCount);
    
    const int POL_STONE_GOLDGEN_2 = 828;
    updateEffect(df, POL_STONE_GOLDGEN_2, totalCount);
    
    const int POL_STONE_GOLDGEN_3 = 829;
    updateEffect(df, POL_STONE_GOLDGEN_3, totalCount);

    const int POL_SCOUTS_BONUS = 802;
    updateEffect(df, POL_SCOUTS_BONUS, totalCount);
}

void portugueseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_ORGAN_GUN, ORGAN_GUN, totalCount);
    updateUnitFromBase(df, ELITE_CARAVEL, CARAVEL, totalCount);

    skipUpdate(591);
    skipUpdate(592);
    skipUpdate(622);
    skipUpdate(623);

    const int PORT_TECH_TREE = 31;
    updateEffect(df, PORT_TECH_TREE, totalCount);

    const int PORT_CARRACK = 601;
    updateEffect(df, PORT_CARRACK, totalCount);

    const int PORT_ARQUEBUS = 602;
    updateEffect(df, PORT_ARQUEBUS, totalCount);

    const int PORT_OBSOLETE = 35;
    updateEffect(df, PORT_OBSOLETE, totalCount);

    const int PORT_GOLD_DISCOUNT = 33;
    updateEffect(df, PORT_GOLD_DISCOUNT, totalCount);

    const int PORT_FORAGERS = 510;
    updateEffect(df, PORT_FORAGERS, totalCount);

    // Ships + 10% hp

    const int PORT_FEITORIA_AVAIL = 600;
    updateEffect(df, PORT_FEITORIA_AVAIL, totalCount);

    const int PORT_FEITORIA = 1021;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(PORT_FEITORIA);

        auto tasks_count = unit.Bird.TaskList.size();
        for (int i = 0; i < tasks_count; i++) {
            unit.Bird.TaskList.at(i).WorkValue1 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue1, totalCount);
            unit.Bird.TaskList.at(i).WorkValue2 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue2, totalCount);
        }
    }

    const int PORT_RESEARCH_SPEED = 32;
    updateEffect(df, PORT_RESEARCH_SPEED, totalCount);
}

void romansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, 1792, 1790, totalCount);
    updateUnitFromBase(df, 1793, LONG_SWORDSMAN, totalCount);

    skipUpdate(892);
    skipUpdate(893);
    skipUpdate(896);
    skipUpdate(903);

    const int ROM_TECH_TREE = 890;
    updateEffect(df, ROM_TECH_TREE, totalCount);

    const int ROM_BALLISTAS = 894;
    updateEffect(df, ROM_BALLISTAS, totalCount);

    const int ROM_COMITATENSES = 895;
    updateEffect(df, ROM_COMITATENSES, totalCount);

    const int ROM_SCORP_BALLISTICS = 901;
    updateEffect(df, ROM_SCORP_BALLISTICS, totalCount);

    const int ROM_VILS_WORK = 898;
    updateEffect(df, ROM_VILS_WORK, totalCount);
    
    const int ROM_NAVY = 899;
    updateEffect(df, ROM_NAVY, totalCount);

    auto scale_mail_id = duplicateEffect(df, SCALE_MAIL_ARMOR);
    updateEffect(df, scale_mail_id, totalCount);
    df->Techs.at(889).EffectID = scale_mail_id;

    auto chain_mail_id = duplicateEffect(df, CHAIN_MAIL_ARMOR);
    updateEffect(df, chain_mail_id, totalCount);
    df->Techs.at(890).EffectID = chain_mail_id;

    auto plate_mail_id = duplicateEffect(df, PLATE_MAIL_ARMOR);
    updateEffect(df, plate_mail_id, totalCount);
    df->Techs.at(891).EffectID = plate_mail_id;
    
    const int ROM_SCORP_DISCOUT = 900;
    updateEffect(df, ROM_SCORP_DISCOUT, totalCount);
}

void saracensCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_MAMELUKE, MAMELUKE, totalCount);

    skipUpdate(266);
    skipUpdate(366);

    const int SAR_TECH_TREE = 261;
    updateEffect(df, SAR_TECH_TREE, totalCount);

    const int SAR_ZEALOTRY = 459;
    updateEffect(df, SAR_ZEALOTRY, totalCount);

    const int SAR_COUNTERWEIGHTS = 480;
    updateEffect(df, SAR_COUNTERWEIGHTS, totalCount);

    const int SAR_OBSOLETE = 545;
    updateEffect(df, SAR_OBSOLETE, totalCount);

    const int SAR_MARKET = 354;
    updateEffect(df, SAR_MARKET, totalCount);

    const int SAR_TRANSPORT = 311;
    updateEffect(df, SAR_TRANSPORT, totalCount);

    const int SAR_GALLEY_ATTACK = 416;
    updateEffect(df, SAR_GALLEY_ATTACK, totalCount);

    const int SAR_CAMEL_HP = 312;
    updateEffect(df, SAR_CAMEL_HP, totalCount);

    const int SAR_ARCHER_BONUS_1 = 313;
    updateEffect(df, SAR_ARCHER_BONUS_1, totalCount);

    const int SAR_ARCHER_BONUS_2 = 318;
    updateEffect(df, SAR_ARCHER_BONUS_2, totalCount);

    const int SAR_ARCHER_BONUS = 409;
    updateEffect(df, SAR_ARCHER_BONUS, totalCount);
}

void siciliansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SERJEANT, SERJEANT, totalCount);
    updateUnitFromBase(df, 1661, 1660, totalCount);
    updateUnitFromBase(df, 1665, 79, totalCount);

    skipUpdate(788);
    skipUpdate(789);

    const int SIC_TECH_TREE = 784;
    updateEffect(df, SIC_TECH_TREE, totalCount);

    const int SIC_FIRST_CRUSADE = 792;
    updateEffect(df, SIC_FIRST_CRUSADE, totalCount);

    const int SIC_HAUBERK = 793;
    updateEffect(df, SIC_HAUBERK, totalCount);

    const int SIC_START_RES = 217;
    updateEffect(df, SIC_START_RES, totalCount);

    const int SIC_BUILD_SPEED = 795;
    updateEffect(df, SIC_BUILD_SPEED, totalCount);

    const int SIC_BONUS_RESISTANCE = 796;
    updateEffect(df, SIC_BONUS_RESISTANCE, totalCount);

    const int SIC_FARM_FOOD_1 = 797;
    updateEffect(df, SIC_FARM_FOOD_1, totalCount);

    const int SIC_FARM_FOOD_2 = 798;
    updateEffect(df, SIC_FARM_FOOD_2, totalCount);

    const int SIC_FARM_FOOD_3 = 799;
    updateEffect(df, SIC_FARM_FOOD_3, totalCount);

    const int SIC_DONJON_AVAIL = 800;
    updateEffect(df, SIC_DONJON_AVAIL, totalCount);

    const int SIC_TRANSPORT = 785;
    updateEffect(df, SIC_TRANSPORT, totalCount);
}

void slavsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_BOYAR, BOYAR, totalCount);

    skipUpdate(556);
    skipUpdate(557);

    const int SLAVS_TECH_TREE = 7;
    updateEffect(df, SLAVS_TECH_TREE, totalCount);

    const int SLAVS_DETINETS = 481;
    updateEffect(df, SLAVS_DETINETS, min((float)totalCount, 2.5f));

    const int SLAVS_DRUZHINA = 569;
    updateEffect(df, SLAVS_DRUZHINA, totalCount);

    const int SLAVS_FARMS = 690;
    updateEffect(df, SLAVS_FARMS, totalCount);

    const int SLAVS_SIEGE_DISCOUNT = 567;
    updateEffect(df, SLAVS_SIEGE_DISCOUNT, totalCount);

    const int SLAVS_MILITARY_POP = 758;
    updateEffect(df, SLAVS_MILITARY_POP, totalCount);
}

void spanishCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CONQUISTADOR, CONQUISTADOR, totalCount);

    skipUpdate(491);
    skipUpdate(492);

    const int SPAN_TECH_TREE = 446;
    updateEffect(df, SPAN_TECH_TREE, totalCount);

    const int SPAN_INQUISITION = 547;
    updateEffect(df, SPAN_INQUISITION, totalCount);

    const int SPAN_SUPREMACY = 495;
    updateEffect(df, SPAN_SUPREMACY, totalCount);

    const int SPAN_TC_BUILD = 168;
    updateEffect(df, SPAN_TC_BUILD, totalCount);

    const int SPAN_TECH_REWARD = 300;
    updateEffect(df, SPAN_TECH_REWARD, totalCount);

    const int SPAN_TECH_REWARD_2 = 181;
    updateEffect(df, SPAN_TECH_REWARD_2, totalCount);

    const int SPAN_TECH_REWARD_3 = 237;
    updateEffect(df, SPAN_TECH_REWARD_3, totalCount);

    const int SPAN_MISSIONARY_AVAIL = 496;
    updateEffect(df, SPAN_MISSIONARY_AVAIL, totalCount);

    const int SPAN_TRADE = 490;
    updateEffect(df, SPAN_TRADE, totalCount);
}

void tatarsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KESHIK, KESHIK, totalCount);

    skipUpdate(716);
    skipUpdate(717);

    const int TAT_TECH_TREE = 708;
    updateEffect(df, TAT_TECH_TREE, totalCount);

    const int TAT_SILK_ARMOR = 724;
    updateEffect(df, TAT_SILK_ARMOR, totalCount);

    const int TAT_TIMURID = 725;
    updateEffect(df, TAT_TIMURID, totalCount);
    
    const int TAT_SHEEP_BONUS = 741;
    updateEffect(df, TAT_SHEEP_BONUS, totalCount);
    
    const int TAT_SHEEP_BONUS_2 = 299;
    updateEffect(df, TAT_SHEEP_BONUS_2, totalCount);
    
    const int TAT_SHEEP_BONUS_3 = 303;
    updateEffect(df, TAT_SHEEP_BONUS_3, totalCount);
    
    const int TAT_SHEEP_BONUS_4 = 305;
    updateEffect(df, TAT_SHEEP_BONUS_4, totalCount);
    
    const int TAT_PARTHIAN_TACTICS = 735;
    updateEffect(df, TAT_PARTHIAN_TACTICS, totalCount);
    
    const int TAT_ELEVATION = 733;
    updateEffect(df, TAT_ELEVATION, totalCount);

    const int TAT_CA_LOS = 709;
    updateEffect(df, TAT_CA_LOS, totalCount);
}

void teutonsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_TEUTONIC_KNIGHT, TEUTONIC_KNIGHT, totalCount);

    skipUpdate(273);
    skipUpdate(362);

    const int TEUT_TECH_TREE = 262;
    updateEffect(df, TEUT_TECH_TREE, totalCount);

    const int TEUT_IRONCLAD = 544;
    updateEffect(df, TEUT_IRONCLAD, totalCount);

    const int TEUT_CRENELLATIONS = 461;
    updateEffect(df, TEUT_CRENELLATIONS, totalCount);
    
    const int TEUT_OBSOLETE = 332;
    updateEffect(df, TEUT_OBSOLETE, totalCount);
    
    const int TEUT_HEAL_RANGE = 345;
    updateEffect(df, TEUT_HEAL_RANGE, totalCount);
    
    const int TEUT_TOWER_GARISON = 352;
    updateEffect(df, TEUT_TOWER_GARISON, totalCount);

    const int TEUT_FARMS_DISCOUNT = 314;
    updateEffect(df, TEUT_FARMS_DISCOUNT, totalCount);
    
    const int TEUT_TC_IMPROVE = 335;
    updateEffect(df, TEUT_TC_IMPROVE, totalCount);
    
    const int TEUT_ARMOR_1 = 333;
    updateEffect(df, TEUT_ARMOR_1, totalCount);
    
    const int TEUT_ARMOR_2 = 334;
    updateEffect(df, TEUT_ARMOR_2, totalCount);
    
    const int TEUT_CONVERSION_RESISTANCE = 404;
    updateEffect(df, TEUT_CONVERSION_RESISTANCE, totalCount);
}

void turksCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_JANISSARY, JANISSARY, totalCount);

    skipUpdate(268);
    skipUpdate(353);
    skipUpdate(367);

    const int TURK_TECH_TREE = 263;
    updateEffect(df, TURK_TECH_TREE, totalCount);

    const int TURK_SIPAHI = 546;
    updateEffect(df, TURK_SIPAHI, totalCount);

    const int TURK_ARTILLERY = 460;
    updateEffect(df, TURK_ARTILLERY, totalCount);

    const int TURK_HUSSAR_FREE = 527;
    updateEffect(df, TURK_HUSSAR_FREE, totalCount);

    const int TURK_CHEMISTRY_FREE = 301;
    updateEffect(df, TURK_CHEMISTRY_FREE, totalCount);

    const int TURK_GUNPOWDER_HP = 296;
    updateEffect(df, TURK_GUNPOWDER_HP, totalCount);

    const int TURK_GUNPOWDERSHIP_HP = 284;
    updateEffect(df, TURK_GUNPOWDERSHIP_HP, totalCount);

    const int TURK_GOLD_MINING = 295;
    updateEffect(df, TURK_GOLD_MINING, totalCount);

    const int TURK_LIGHTCAV_PIERCE = 509;
    updateEffect(df, TURK_LIGHTCAV_PIERCE, totalCount);

    const int TURK_GUNPOWDER_TRAINTIME = 410;
    updateEffect(df, TURK_GUNPOWDER_TRAINTIME, totalCount);
}

void vietnameseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_RATTAN_ARCHER, RATTAN_ARCHER, totalCount);
    updateUnitFromBase(df, IMPERIAL_SKIRMISHER, ELITE_SKIRMISHER, totalCount);

    skipUpdate(660);
    skipUpdate(661);

    const int VIET_TECH_TREE = 652;
    updateEffect(df, VIET_TECH_TREE, totalCount);

    const int VIET_CHATRAS = 668;
    updateEffect(df, VIET_CHATRAS, totalCount);

    const int VIET_PAPER_MONEY_1 = 669;
    updateEffect(df, VIET_PAPER_MONEY_1, totalCount);

    const int VIET_PAPER_MONEY_2 = 881;
    updateEffect(df, VIET_PAPER_MONEY_2, totalCount);

    const int VIET_PAPER_MONEY_3 = 882;
    updateEffect(df, VIET_PAPER_MONEY_3, totalCount);

    const int VIET_PAPER_MONEY_4 = 883;
    updateEffect(df, VIET_PAPER_MONEY_4, totalCount);

    const int VIET_ARCHERS_HP = 672;
    updateEffect(df, VIET_ARCHERS_HP, totalCount);

    const int VIET_VISION = 698;
    updateEffect(df, VIET_VISION, totalCount);
}

void vikingsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_BERSERK, BERSERK, totalCount);
    updateUnitFromBase(df, ELITE_LONGBOAT, LONGBOAT, totalCount);

    skipUpdate(269);
    skipUpdate(370);
    skipUpdate(397);
    skipUpdate(398);

    const int VIK_TECH_TREE = 276;
    updateEffect(df, VIK_TECH_TREE, totalCount);

    const int VIK_CHIEFTAINS = 517;
    updateEffect(df, VIK_CHIEFTAINS, totalCount);

    const int VIK_BOGSVEIGAR = 467;
    updateEffect(df, VIK_BOGSVEIGAR, totalCount);

    const int VIK_WHEELBARROW_FREE = 391;
    updateEffect(df, VIK_WHEELBARROW_FREE, totalCount);

    const int VIK_HANDCART_FREE = 412;
    updateEffect(df, VIK_HANDCART_FREE, totalCount);

    const int VIK_WARSHIP_COST_1 = 383;
    updateEffect(df, VIK_WARSHIP_COST_1, totalCount);
    
    const int VIK_WARSHIP_COST_2 = 386;
    updateEffect(df, VIK_WARSHIP_COST_2, totalCount);
    
    const int VIK_WARSHIP_COST_3 = 394;
    updateEffect(df, VIK_WARSHIP_COST_3, totalCount);
    
    const int VIK_INF_HP_1 = 390;
    updateEffect(df, VIK_INF_HP_1, totalCount);
    
    const int VIK_INF_HP_2 = 427;
    updateEffect(df, VIK_INF_HP_2, totalCount);
    
    const int VIK_INF_HP_3 = 428;
    updateEffect(df, VIK_INF_HP_3, totalCount);
    
    const int VIK_DOCK_COST = 411;
    updateEffect(df, VIK_DOCK_COST, totalCount);
}

void configureMultipliedCivBonus(genie::DatFile *df, int totalCount) {
    aztecsCivBonus(df, totalCount);
    bengalisCivBonus(df, totalCount);
    berbersCivBonus(df, totalCount);
    bohemiansCivBonus(df, totalCount);
    britonsCivBonus(df, totalCount);
    bulgariansCivBonus(df, totalCount);
    burgundiansCivBonus(df, totalCount);
    burmeseCivBonus(df, totalCount);
    byzantinesCivBonus(df, totalCount);
    celtsCivBonus(df, totalCount);
    chineseCivBonus(df, totalCount);
    cumansCivBonus(df, totalCount);
    dravidiansCivBonus(df, totalCount);
    ethiopiansCivBonus(df, totalCount);
    franksCivBonus(df, totalCount);
    gothsCivBonus(df, totalCount);
    gurjarasCivBonus(df, totalCount);
    hunsCivBonus(df, totalCount);
    hindustanisCivBonus(df, totalCount);
    incasCivBonus(df, totalCount);
    italiansCivBonus(df, totalCount);
    japaneseCivBonus(df, totalCount);
    khmerCivBonus(df, totalCount);
    koreansCivBonus(df, totalCount);
    lithuaniansCivBonus(df, totalCount);
    magyarsCivBonus(df, totalCount);
    malayCivBonus(df, totalCount);
    maliansCivBonus(df, totalCount);
    mayansCivBonus(df, totalCount);
    mongolsCivBonus(df, totalCount);
    persiansCivBonus(df, totalCount);
    polesCivBonus(df, totalCount);
    portugueseCivBonus(df, totalCount);
    romansCivBonus(df, totalCount);
    saracensCivBonus(df, totalCount);
    siciliansCivBonus(df, totalCount);
    slavsCivBonus(df, totalCount);
    spanishCivBonus(df, totalCount);
    tatarsCivBonus(df, totalCount);
    teutonsCivBonus(df, totalCount);
    turksCivBonus(df, totalCount);
    vietnameseCivBonus(df, totalCount);
    vikingsCivBonus(df, totalCount);

    auto techs_count = df->Techs.size();
    for (auto i = 0; i < techs_count; i++) {
        auto &tech = df->Techs.at(i);
        auto was_effect_updated = updated_effects.find(tech.EffectID) != updated_effects.end();
        if (tech.Civ > 0 && !was_effect_updated && tech.EffectID != -1) {
            cout << "Effect " << tech.EffectID << ":" << df->Effects.at(tech.EffectID).Name << " should be updated for civ: " << df->Civs.at(tech.Civ).Name << " tech: " << tech.Name << " but was not.\n";
        } else if (tech.Civ == 0 && was_effect_updated && tech.EffectID != -1) {
            cout << "Effect " << tech.EffectID << ":" << df->Effects.at(tech.EffectID).Name << " should not be updated for civ: " << df->Civs.at(tech.Civ).Name << " tech: " << tech.Name << " but was.\n";
        }
    }
}