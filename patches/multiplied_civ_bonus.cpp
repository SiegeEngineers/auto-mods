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
        case 195: // construction rate
        case 237: // folwark collection amount
        case 254: // gurjara mill food
            command.D = multipleAdd(command.D, totalCount);
            break;
        case 78: // trade fee
        case 85: // research cost multiplier
        case 86: // research time multiplier
            command.D = multipleMultiply(command.D, totalCount);
            break;
    }
}

void multipleSpawnUnit(std::vector<genie::EffectCommand>& commands, int id, float totalCount) {
    auto &command = commands.at(id);
    switch (command.A)
    {
        case 59: // forage bush
        case 305: // llama
        case VILLAGER:
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

    int cost_type;
    switch (command.C) {
        case 103:
            cost_type = 0;
            break;
        case 104:
            cost_type = 1;
            break;
        case 105:
            cost_type = 3;
            break;
        case 106:
            cost_type = 2;
            break;
    }


    auto base_cost = 0;
    auto costs_size = discounted_unit.Creatable.ResourceCosts.size();
    for (auto i = 0; i < costs_size; i++) {
        auto &resource_cost = discounted_unit.Creatable.ResourceCosts.at(i);
        if (resource_cost.Type == cost_type) {
            base_cost = resource_cost.Amount;
        }
    }

    if (command.D >= 0) {
        //command.D = multipleChangeFrom(command.D, totalCount, base_cost);
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
                if (command.A == 234 && effect_id == 792) { // spawn cap of first crusade
                    command.D = multipleAdd(command.D, totalCount);
                } else if (command.B == 0) {    
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
                    multipleMultiplyFunc(commands, i, totalCount);
                } else if (command.C == 101) {
                    command.D = max(multipleMultiply(command.D, totalCount), 0.01f);
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
            auto found = false;
            for (int j = 0; j < base_storages_count; j++) {
                if (elite_unit.ResourceStorages.at(i).Type == base_unit.ResourceStorages.at(j).Type) {
                    elite_unit.ResourceStorages.at(i).Amount = calculateDiff(elite_unit.ResourceStorages.at(i).Amount, base_unit.ResourceStorages.at(j).Amount, totalCount);
                    found = true;
                    break;
                }
            }

            if (!found) {
                elite_unit.ResourceStorages.at(i).Amount = calculateDiff(elite_unit.ResourceStorages.at(i).Amount, 0, totalCount);
            }
        }

        auto attacks_count = elite_unit.Type50.Attacks.size();
        auto base_attacks_count = base_unit.Type50.Attacks.size();
        for (int i = 0; i < attacks_count; i++) {
            auto found = false;
            for (int j = 0; j < base_attacks_count; j++) {
                if (elite_unit.Type50.Attacks.at(i).Class == base_unit.Type50.Attacks.at(j).Class) {
                    elite_unit.Type50.Attacks.at(i).Amount = calculateDiff(elite_unit.Type50.Attacks.at(i).Amount, base_unit.Type50.Attacks.at(j).Amount, totalCount);
                    found = true;
                    break;
                }
            }

            if (!found) {
                elite_unit.Type50.Attacks.at(i).Amount = calculateDiff(elite_unit.Type50.Attacks.at(i).Amount, 0, totalCount);
            }
        }

        auto armours_count = elite_unit.Type50.Armours.size();
        auto base_armours_count = base_unit.Type50.Armours.size();
        for (int i = 0; i < armours_count; i++) {
            auto found = false;
            for (int j = 0; j < base_armours_count; j++) {
                if (elite_unit.Type50.Armours.at(i).Class == base_unit.Type50.Armours.at(j).Class) {
                    elite_unit.Type50.Armours.at(i).Amount = calculateDiff(elite_unit.Type50.Armours.at(i).Amount, base_unit.Type50.Armours.at(j).Amount, totalCount);
                    found = true;
                    break;
                }
            }

            if (!found) {
                    elite_unit.Type50.Armours.at(i).Amount = calculateDiff(elite_unit.Type50.Armours.at(i).Amount, 0, totalCount);
            }
        }
    }
}

#pragma endregion Helper_Methods

void aztecsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_JAGUAR_WARRIOR, JAGUAR_WARRIOR, totalCount);

    skipUpdate(346); // AZT_XOLOTL
    skipUpdate(442); // AZT_JAGUAR_MAN
    skipUpdate(444); // AZT_JAGUAR_MAN

    updateEffect(df, 447, totalCount); // AZT_TECH_TREE 
    updateEffect(df, 514, totalCount); // AZT_ATLATL 
    updateEffect(df, 465, totalCount); // AZT_GARLAND_WARS 
    updateEffect(df, 470, totalCount); // AZT_MONK_HP_1 
    updateEffect(df, 471, totalCount); // AZT_MONK_HP_2 
    updateEffect(df, 472, totalCount); // AZT_MONK_HP_3 
    updateEffect(df, 473, totalCount); // AZT_MONK_HP_4 
    updateEffect(df, 474, totalCount); // AZT_MONK_HP_5 
    updateEffect(df, 475, totalCount); // AZT_MONK_HP_6 
    updateEffect(df, 476, totalCount); // AZT_MONK_HP_7 
    updateEffect(df, 477, totalCount); // AZT_MONK_HP_8 
    updateEffect(df, 478, totalCount); // AZT_MONK_HP_9 
    updateEffect(df, 512, totalCount); // AZT_MONK_HP_10 
    updateEffect(df, 213, totalCount); // AZT_START_RES 
    updateEffect(df, 488, totalCount); // AZT_RELIC 
}

void bengalisCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, 1761, 1759, totalCount);
    updateUnitFromBase(df, 1740, 1738, totalCount);

    skipUpdate(846); // BENG_RATHA
    skipUpdate(847); // BENG_RATHA

    updateEffect(df, 840, totalCount); // BENG_TECH_TREE 
    updateEffect(df, 852, totalCount); // BENG_PAIKS 
    updateEffect(df, 853, totalCount); // BENG_MAHAYANA 
    updateEffect(df, 865, totalCount); // BENG_RESIST 
    updateEffect(df, 866, totalCount); // BENG_VILS 
    updateEffect(df, 888, totalCount); // BENG_CAV_BONUS 
    updateEffect(df, 867, totalCount); // BENG_SHIPS_REGENERATE 
    updateEffect(df, 568, totalCount); // BENG_MONK_DEFENSE 
    updateEffect(df, 841, totalCount); // BENG_TRADE 
}

void berbersCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CAMEL_ARCHER, CAMEL_ARCHER, totalCount);
    updateUnitFromBase(df, ELITE_GENITOUR, GENITOUR, totalCount);
    updateUnitFromBase(df, 596, 583, totalCount);

    skipUpdate(594); // BERB_CAMEL_ARCHER
    skipUpdate(595); // BERB_CAMEL_ARCHER
    skipUpdate(440); // BERB_GENITOUR
    skipUpdate(624); // BERB_GENITOUR
    skipUpdate(627); // BERB_GENITOUR

    updateEffect(df, 37, totalCount); // BERB_TECH_TREE 
    updateEffect(df, 607, totalCount); // BERB_KASBAH 
    updateEffect(df, 608, totalCount); // BERB_MAGHREBI_CAMELS 
    updateEffect(df, 609, totalCount); // BERB_VILLAGERS_FASTER_1 
    updateEffect(df, 626, totalCount); // BERB_VILLAGERS_FASTER_2 
    updateEffect(df, 610, totalCount); // BERB_CAVALRY_DISCOUNT_1 
    updateEffect(df, 638, totalCount); // BERB_CAVALRY_DISCOUNT_2 
    updateEffect(df, 34, totalCount); // BERB_SHIPS_FASTER_1 
    updateEffect(df, 611, totalCount); // BERB_SHIPS_FASTER_2 
}

void bohemiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, HOUFNICE, BOMBARD_CANNON, totalCount);
    updateUnitFromBase(df, ELITE_HUSSITE_WAGON, HUSSITE_WAGON, totalCount);

    skipUpdate(807); // BOH_HUSSITE_WAGON
    skipUpdate(808); // BOH_HUSSITE_WAGON
    skipUpdate(814); // BOH_HOUFNICE

    updateEffect(df, 803, totalCount); // BOH_TECH_TREE 
    updateEffect(df, 811, totalCount); // BOH_WAGENBURG_TACTICS 
    updateEffect(df, 812, totalCount); // BOH_HUSSITE_REFORMS 
    updateEffect(df, 830, totalCount); // BOH_BLACKSMITH_DISCOUNT 
    updateEffect(df, 824, totalCount); // BOH_SPEAR_BONUS 
    updateEffect(df, 825, totalCount); // BOH_VIL_SANCTITY 
    updateEffect(df, 826, totalCount); // BOH_VIL_FERVOR 
    updateEffect(df, 804, totalCount); // BOH_MARKET_SPEED 
}

void britonsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_LONGBOWMAN, LONGBOWMAN, totalCount);

    skipUpdate(251); // BRIT_LONGBOWMAN
    skipUpdate(358); // BRIT_LONGBOWMAN

    updateEffect(df, 254, totalCount); // BRIT_TECH_TREE 
    updateEffect(df, 455, totalCount); // BRIT_YEOMEN 
    updateEffect(df, 540, totalCount); // BRIT_WARWOLF 
    updateEffect(df, 379, totalCount); // BRIT_TC_DISCOUNT 
    updateEffect(df, 380, totalCount); // BRIT_ARCHER_RANGE_1 
    updateEffect(df, 415, totalCount); // BRIT_ARCHER_RANGE_2 
    updateEffect(df, 381, totalCount); // BRIT_SHEPARD 
    updateEffect(df, 399, totalCount); // BRIT_RANGE_SPEED 
}

void bulgariansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KONNIK, KONNIK, totalCount);
    updateUnitFromBase(df, ELITE_KONNIK_2, KONNIK_2, totalCount);
    updateUnitFromBase(df, DISMOUNTED_ELITE_KONNIK, DISMOUNTED_KONNIK, totalCount);

    skipUpdate(714); // BULG_KONNIK
    skipUpdate(715); // BULG_KONNIK

    updateEffect(df, 706, totalCount); // BULG_TECH_TREE 
    updateEffect(df, 722, totalCount); // BULG_STIRRUPS 
    updateEffect(df, 723, totalCount); // BULG_BAGAINS 
    updateEffect(df, 730, totalCount); // BULG_MILITIA_UPGRADES 
    updateEffect(df, 171, totalCount); // BULG_TC_STONE_DISCOUNT_1 
    updateEffect(df, 731, totalCount); // BULG_TC_STONE_DISCOUNT_2 
    updateEffect(df, 707, totalCount); // BULG_BLACKSMITH_FASTER 
    updateEffect(df, 732, totalCount); // BULD_ENABLE_CREPOST 
    const int BULG_KREPOST = 1251;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(BULG_KREPOST);
        unit.Creatable.ResourceCosts.at(0).Amount = multipleChangeFrom(unit.Creatable.ResourceCosts.at(0).Amount, totalCount, 650);
    }
}

void burgundiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_COUSTILLIER, COUSTILLIER, totalCount);
    updateUnitFromBase(df, 1663, LONG_SWORDSMAN, totalCount);
    updateUnitFromBase(df, 1697, LONG_SWORDSMAN, totalCount);
    updateUnitFromBase(df, 1699, LONG_SWORDSMAN, totalCount);

    skipUpdate(786); // BURG_COUSTILLIER
    skipUpdate(787); // BURG_COUSTILLIER

    updateEffect(df, 782, totalCount); // BURG_TECH_TREE 
    updateEffect(df, 790, totalCount); // BURG_BURGUNDIAN_VINEYARDS 
    updateEffect(df, 791, totalCount); // BURG_FLEMISH_REVOLUTION 
    updateEffect(df, 794, totalCount); // BURG_GUNPOWDER 
    updateEffect(df, 783, totalCount); // BURG_RELIC_FOOD 
}

void burmeseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_ARAMBAI, ARAMBAI, totalCount);

    skipUpdate(658); // BURM_ARAMBAI
    skipUpdate(659); // BURM_ARAMBAI

    updateEffect(df, 650, totalCount); // BURM_TECH_TREE 
    updateEffect(df, 666, totalCount); // BURM_HOWDAH 
    updateEffect(df, 667, totalCount); // BURM_MANIPUR_CAVALRY 
    updateEffect(df, 685, totalCount); // BURM_LUMBER_UPGRADES 
    updateEffect(df, 686, totalCount); // BURM_INF_ATTACK_1 
    updateEffect(df, 687, totalCount); // BURM_INF_ATTACK_2 
    updateEffect(df, 688, totalCount); // BURM_INF_ATTACK_3 
    updateEffect(df, 699, totalCount); // BURM_INF_ATTACK_4 
    updateEffect(df, 679, totalCount); // BURM_ELEP_ARMOR 
    updateEffect(df, 689, totalCount); // BURM_MONASTERY_DISCOUNT 
}

void byzantinesCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CATAPHRACT, CATAPHRACT, totalCount);

    skipUpdate(264); // BYZ_CATAPHRACT
    skipUpdate(359); // BYZ_CATAPHRACT

    updateEffect(df, 256, totalCount); // BYZ_TECH_TREE 
    updateEffect(df, 518, totalCount); // BYZ_GREEK_FIRE 
    updateEffect(df, 493, totalCount); // BYZ_LOGISTICA 
    updateEffect(df, 413, totalCount); // BYZ_OBSOLETE 
    updateEffect(df, 282, min(totalCount, 3.0f)); // BYZ_BUILDINGS_HP_1 
    updateEffect(df, 429, min(totalCount, 3.0f)); // BYZ_BUILDINGS_HP_2 
    updateEffect(df, 430, min(totalCount, 3.0f)); // BYZ_BUILDINGS_HP_3 
    updateEffect(df, 431, min(totalCount, 3.0f)); // BYZ_BUILDINGS_HP_4 
    updateEffect(df, 283, totalCount); // BYZ_COUNTER_DISCOUNT 
    updateEffect(df, 396, totalCount); // BYZ_SHIP_ATTACKSPEED 
    updateEffect(df, 400, totalCount); // BYZ_HEALSPEED 
}

void celtsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_WOAD_RAIDER, WOAD_RAIDER, totalCount);

    skipUpdate(274); // CELT_WOAD_RIDER
    skipUpdate(368); // CELT_WOAD_RIDER

    updateEffect(df, 275, totalCount); // CELT_TECH_TREE 
    updateEffect(df, 537, totalCount); // CELT_STRONGHOLD 
    updateEffect(df, 239, totalCount); // CELT_FUROR_CELTICA 
    updateEffect(df, 392, totalCount); // CELT_INF_SPEED 
    updateEffect(df, 384, totalCount); // CELT_LUMBERJACK 
    updateEffect(df, 385, totalCount); // CELT_SIEGE_FIRING 
    updateEffect(df, 417, totalCount); // CELT_DOMINANT_LOS 
    updateEffect(df, 401, totalCount); // CELTS_SIEGEWORKSHOP_SPEED 
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

    skipUpdate(265); // CHIN_CHUKONU
    skipUpdate(360); // CHIN_CHUKONU

    updateEffect(df, 257, totalCount); // CHIN_TECH_TREE 
    updateEffect(df, 516, min(totalCount, 3.0f)); // CHIN_GREAT_WALL 
    updateEffect(df, 483, totalCount); // CHIN_ROCKETRY 
    updateEffect(df, 215, totalCount); // CHIN_START_RES 
    updateEffect(df, 302, totalCount); // CHIN_ADDITIONAL_VILS 
    updateEffect(df, 304, totalCount); // CHIN_TECH_DISCOUNT_1 
    updateEffect(df, 349, totalCount); // CHIN_TECH_DISCOUNT_2 
    updateEffect(df, 350, totalCount); // CHIN_TECH_DISCOUNT_3 
    updateEffect(df, 351, totalCount); // CHIN_TECH_DISCOUNT_4 
    updateEffect(df, 437, totalCount); // CHIN_TC_POP 
    updateEffect(df, 395, totalCount); // CHIN_DEMO_HP 
    updateEffect(df, 402, totalCount); // CHIN_FARM_FOOD_1 
    updateEffect(df, 831, totalCount); // CHIN_FARM_FOOD_2 
    updateEffect(df, 832, totalCount); // CHIN_FARM_FOOD_3 
    updateEffect(df, 833, totalCount); // CHIN_FARM_FOOD_4 
    updateEffect(df, 834, totalCount); // CHIN_FARM_FOOD_5 
    updateEffect(df, 835, totalCount); // CHIN_FARM_FOOD_6 
    updateEffect(df, 836, totalCount); // CHIN_FARM_FOOD_7 
    updateEffect(df, 837, totalCount); // CHIN_FARM_FOOD_8 
}

void cumansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KIPCHAK, KIPCHAK, totalCount);
    updateUnitFromBase(df, 1260, KIPCHAK, totalCount);

    skipUpdate(718); // CUMANS_KIPCHAK
    skipUpdate(719); // CUMANS_KIPCHAK
    skipUpdate(759); // CUMANS_TC_BUILD_TIME
    skipUpdate(760); // CUMANS_TC_BUILD_TIME

    updateEffect(df, 710, totalCount); // CUMANS_TECH_TREE 
    updateEffect(df, 726, totalCount); // CUMANS_STEPPE_HUSBANDRY 
    updateEffect(df, 743, totalCount); // CUMANS_CUMAN_MERCENARIES 
    updateEffect(df, 727, totalCount); // CUMANS_CUMAN_MERCENARIES_2 
    updateEffect(df, 761, totalCount); // CUMANS_TC 
    updateEffect(df, 742, totalCount); // CUMANS_SIEGE 
    updateEffect(df, 697, totalCount); // CUMANS_WOOD_DISCOUNT 
    updateEffect(df, 746, totalCount); // CUMANS_CAV_FASTER_1 
    updateEffect(df, 748, totalCount); // CUMANS_CAV_FASTER_2 
    updateEffect(df, 762, totalCount); // CUMANS_CAV_FASTER_3 
    updateEffect(df, 763, totalCount); // CUMANS_CAV_FASTER_4 
    updateEffect(df, 711, min(totalCount, 3.0f)); // CUMANS_PALISADE_HP 
}

void dravidiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, 1737, 1735, totalCount);

    skipUpdate(844); // DRAV_URUMI_SWORDSMAN
    skipUpdate(845); // DRAV_URUMI_SWORDSMAN

    updateEffect(df, 838, totalCount); // DRAV_TECH_TREE 
    updateEffect(df, 850, totalCount); // DRAV_MEDICAL_CORPS 
    updateEffect(df, 851, totalCount); // DRAV_WOOTZ_STEEL 
    updateEffect(df, 860, totalCount); // DRAV_THIRISADAI 
    updateEffect(df, 868, totalCount); // DRAV_BONUS_WOOD 
    updateEffect(df, 863, totalCount); // DRAV_FISHING_CARRY 
    updateEffect(df, 887, totalCount); // DRAV_SIEGE_DISCOUNT 
    updateEffect(df, 864, totalCount); // DRAV_SKIRM_ATTACKFASTER 
    updateEffect(df, 839, totalCount); // DRAV_DOCK_POP 
}

void ethiopiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SHOTEL_WARRIOR, SHOTEL_WARRIOR, totalCount);

    skipUpdate(598); // ETH_SHOTEL_WARRIOR
    skipUpdate(599); // ETH_SHOTEL_WARRIOR

    updateEffect(df, 48, totalCount); // ETH_TECH_TREE 
    updateEffect(df, 603, totalCount); // ETH_ROYAL_HEIRS 
    updateEffect(df, 604, totalCount); // ETH_TORSION_ENGINES 
    updateEffect(df, 634, totalCount); // ETH_TORSION_ENGINES_2 
    updateEffect(df, 612, totalCount); // ETH_ACHER_FIREFASTER 
    updateEffect(df, 613, totalCount); // ETH_BONUS_RES_1 
    updateEffect(df, 614, totalCount); // ETH_BONUS_RES_2 
    updateEffect(df, 615, totalCount); // ETH_BONUS_RES_3 
    updateEffect(df, 616, totalCount); // ETH_PAIK_FREE 
    updateEffect(df, 49, totalCount); // ETH_OUTPOST_LOS 
}

void franksCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_THROWING_AXEMAN, THROWING_AXEMAN, totalCount);

    skipUpdate(272); // FRANKS_THROWING_AXEMAN
    skipUpdate(361); // FRANKS_THROWING_AXEMAN

    updateEffect(df, 258, totalCount); // FRANK_TECH_TREE 
    updateEffect(df, 291, totalCount); // FRANK_BEARDED_AXE 
    updateEffect(df, 548, totalCount); // FRANK_CHIVALRY 
    updateEffect(df, 324, totalCount); // FRANK_CASTLE_DISCOUNT 
    updateEffect(df, 285, totalCount); // FRANK_CAVALRY_HP 
    updateEffect(df, 523, totalCount); // FRANK_FORAGERS 
    updateEffect(df, 403, totalCount); // FRANK_KNIGHT_LOS 
}

void gothsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_HUSKARL, HUSKARL, totalCount);
    updateUnitFromBase(df, 761, 759, totalCount);

    skipUpdate(363); // GOTH_HUSCARL
    skipUpdate(507); // GOTH_HUSCARL

    updateEffect(df, 259, totalCount); // GOTH_TECH_TREE 
    updateEffect(df, 462, totalCount); // GOTH_ANARCHY 
    updateEffect(df, 513, totalCount); // GOTH_PERFUSION 
    updateEffect(df, 342, totalCount); // GOTH_INF_DISCOUNT_1 
    updateEffect(df, 765, totalCount); // GOTH_INF_DISCOUNT_2 
    updateEffect(df, 766, totalCount); // GOTH_INF_DISCOUNT_3 
    updateEffect(df, 767, totalCount); // GOTH_INF_DISCOUNT_4 
    updateEffect(df, 344, totalCount); // GOTH_INF_DISCOUNT_5 
    updateEffect(df, 343, totalCount); // GOTH_INF_DISCOUNT_6 
    updateEffect(df, 326, totalCount); // GOTH_INF_BONUS_1 
    updateEffect(df, 327, totalCount); // GOTH_INF_BONUS_2 
    updateEffect(df, 328, totalCount); // GOTH_INF_BONUS_3 
    updateEffect(df, 414, totalCount); // GOTH_HUNT 
    updateEffect(df, 341, totalCount); // GOTH_LOOM_INSTANT 
    updateEffect(df, 418, totalCount); // GOTH_POP 
    updateEffect(df, 405, totalCount); // GOTH_BARRACKS_WORK 
}

void gurjarasCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SHRIVAMSHA_RIDER, SHRIVAMSHA_RIDER, totalCount);
    updateUnitFromBase(df, 1743, 1741, totalCount);

    skipUpdate(848); // GUR_CHAKRAM_THROWER
    skipUpdate(849); // GUR_CHAKRAM_THROWER
    skipUpdate(861); // GUR_SHRIVAMSHA_RIDER
    skipUpdate(862); // GUR_SHRIVAMSHA_RIDER

    updateEffect(df, 842, totalCount); // GUR_TECH_TREE 
    updateEffect(df, 854, totalCount); // GUR_KSHATRIYAS 
    updateEffect(df, 875, totalCount); // GUR_KSHATRIYAS_SUP 
    updateEffect(df, 855, totalCount); // GUR_FRONTIER_GUARDS 
    updateEffect(df, 872, totalCount); // GUR_FORAGEBUSHES 
    updateEffect(df, 871, totalCount); // GUR_MILL_GARISON 
    updateEffect(df, 869, totalCount); // GUR_CAV_BONUS_1 
    updateEffect(df, 873, totalCount); // GUR_CAV_BONUS_2 
    updateEffect(df, 884, totalCount); // GUR_CAV_BONUS_3
    updateEffect(df, 870, totalCount); // GUR_DOCK_GARRISON 
    updateEffect(df, 843, totalCount); // GUR_CAMEL_TRAIN 
}

void hindustanisCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, IMPERIAL_CAMEL_RIDER, HEAVY_CAMEL_RIDER, totalCount);
    updateUnitFromBase(df, 1749, 1747, totalCount);

    skipUpdate(858); // HIND_GHULAM
    skipUpdate(859); // HIND_GHULAM

    updateEffect(df, 1, totalCount); // HIND_TECH_TREE 
    updateEffect(df, 577, totalCount); // HIND_IMP_CAMEL 
    updateEffect(df, 562, totalCount); // HIND_GRAND_TRUNK_ROAD 
    updateEffect(df, 563, totalCount); // HIND_SHATAGNI 
    updateEffect(df, 551, totalCount); // HIND_VIL_DISCOUNT_1 
    updateEffect(df, 552, totalCount); // HIND_VIL_DISCOUNT_2 
    updateEffect(df, 553, totalCount); // HIND_VIL_DISCOUNT_3 
    updateEffect(df, 585, totalCount); // HIND_VIL_DISCOUNT_4 
    updateEffect(df, 560, totalCount); // HIND_CAMEL_ATTACKSPEED 
    updateEffect(df, 576, totalCount); // HIND_GUNPOWDER_ARMOR 
    updateEffect(df, 574, totalCount); // HIND_CARAVENSEI_AVAIL 
    const int HIND_CARAVENSEI = 1754;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(HIND_CARAVENSEI);

        auto tasks_count = unit.Bird.TaskList.size();
        for (int i = 0; i < tasks_count; i++) {
            unit.Bird.TaskList.at(i).WorkValue1 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue1, totalCount);
            unit.Bird.TaskList.at(i).WorkValue2 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue2, totalCount);
        }
    }

    updateEffect(df, 2, totalCount); // HIND_CAV_ATTACK 
}

void hunsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_TARKAN, TARKAN, totalCount);
    updateUnitFromBase(df, 887, 886, totalCount);

    skipUpdate(453); // HUNS_TARKAN
    skipUpdate(454); // HUNS_TARKAN

    updateEffect(df, 448, totalCount); // HUNS_TECH_TREE 
    skipUpdate(214); // HUNS_START_RES 
    updateEffect(df, 538, totalCount); // HUNS_MARAUERS 
    updateEffect(df, 464, totalCount); // HUNS_ATHEISM 
    updateEffect(df, 497, totalCount); // HUNS_CA_DISCOUNT_1 
    updateEffect(df, 498, totalCount); // HUNS_CA_DISCOUNT_2 
    updateEffect(df, 484, totalCount); // HUNS_FASTER_STABLES 
}

void incasCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KAMAYUK, KAMAYUK, totalCount);

    skipUpdate(40); // INC_EAGLE_START
    skipUpdate(564); // INC_KAMAYUK
    skipUpdate(565); // INC_KAMAYUK
    skipUpdate(582); // INC_SLINGER

    updateEffect(df, 3, totalCount); // INC_TECH_TREE 
    updateEffect(df, 572, totalCount); // INC_ANDEAN_SLING 
    updateEffect(df, 573, totalCount); // INC_FABRIC_SHIELDS 
    updateEffect(df, 156, totalCount); // INC_FOOD_DISCOUNT_1 
    updateEffect(df, 158, totalCount); // INC_FOOD_DISCOUNT_2 
    updateEffect(df, 159, totalCount); // INC_FOOD_DISCOUNT_3 
    updateEffect(df, 160, totalCount); // INC_FOOD_DISCOUNT_4 
    updateEffect(df, 529, totalCount); // INC_VIL_BLACKSMITH_1 
    updateEffect(df, 530, totalCount); // INC_VIL_BLACKSMITH_2 
    updateEffect(df, 531, totalCount); // INC_VIL_BLACKSMITH_3 
    updateEffect(df, 532, totalCount); // INC_VIL_BLACKSMITH_4 
    updateEffect(df, 533, totalCount); // INC_VIL_BLACKSMITH_5 
    updateEffect(df, 534, totalCount); // INC_VIL_BLACKSMITH_6 
    updateEffect(df, 550, totalCount); // INC_HOUSES_POP 
    updateEffect(df, 575, totalCount); // INC_STONE_DISCOUNT 
    updateEffect(df, 678, totalCount); // INC_SPAWN_LAMA 
}

void italiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_GENOESE_CROSSBOWMAN, GENOESE_CROSSBOWMAN, totalCount);
    updateUnitFromBase(df, CONDOTTIERO, LONG_SWORDSMAN, totalCount);

    skipUpdate(519); // ITAL_GENOESE_CROSSBOWMAN
    skipUpdate(520); // ITAL_GENOESE_CROSSBOWMAN

    updateEffect(df, 10, totalCount); // ITAL_TECH_TREE 
    updateEffect(df, 549, totalCount); // ITAL_PAVISE 
    updateEffect(df, 554, totalCount); // ITAL_SILK_ROAD 
    updateEffect(df, 555, totalCount); // ITAL_GUNPOWDER_DISCOUNT 
}

void japaneseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SAMURAI, SAMURAI, totalCount);

    skipUpdate(250); // JAP_SAMURAI
    skipUpdate(364); // JAP_SAMURAI

    updateEffect(df, 255, totalCount); // JAP_TECH_TREE 
    updateEffect(df, 539, totalCount); // JAP_YASAMA 
    updateEffect(df, 59, totalCount); // JAP_KATAPARUTO 
    updateEffect(df, 432, totalCount); // JAP_OBSOLETE 
    updateEffect(df, 433, totalCount); // JAP_OBSOLETE_2 
    updateEffect(df, 306, totalCount); // JAP_FISH_IMPROVE 
    updateEffect(df, 434, totalCount); // JAP_FISH_WORK_1 
    updateEffect(df, 435, totalCount); // JAP_FISH_WORK_2 
    updateEffect(df, 436, totalCount); // JAP_FISH_WORK_3 
    updateEffect(df, 338, totalCount); // JAP_DROP_DISCOUNT 
    updateEffect(df, 339, totalCount); // JAP_ATTACK_SPEED 
    updateEffect(df, 406, totalCount); // JAP_GALLEY_LOS 
}

void khmerCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_BALLISTA_ELEPHANT, BALLISTA_ELEPHANT, totalCount);

    skipUpdate(654); // KHMER_BALLISTA_ELEPHANT
    skipUpdate(655); // KHMER_BALLISTA_ELEPHANT

    updateEffect(df, 646, totalCount); // KHMER_TECH_TREE 
    updateEffect(df, 662, totalCount); // KHMER_TUSK_SWORDS 
    updateEffect(df, 663, totalCount); // KHMER_DOUBLE_CROSSBOW 
    updateEffect(df, 703, totalCount); // KHMER_ELEP_FASTER 
    updateEffect(df, 693, totalCount); // KHMER_HOUSE_GARRISON 
    updateEffect(df, 647, totalCount); // KHMER_SCORP_RANGE 
}

void koreansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_WAR_WAGON, WAR_WAGON, totalCount);
    updateUnitFromBase(df, ELITE_TURTLE_SHIP, TURTLE_SHIP, totalCount);

    skipUpdate(500); // KOREA_TURTLE_SHIP
    skipUpdate(501); // KOREA_TURTLE_SHIP
    skipUpdate(502); // KOREA_WAR_WAGON
    skipUpdate(503); // KOREA_WAR_WAGON

    updateEffect(df, 504, totalCount); // KOREA_TECH_TREE 
    updateEffect(df, 541, totalCount); // KOREA_EUPSEONG 
    updateEffect(df, 506, totalCount); // KOREA_SHINKICHON 
    updateEffect(df, 508, totalCount); // KORA_ARCHER_ARMOR 
    updateEffect(df, 561, totalCount); // KOREA_WOOD_DISCOUNT 
}

void lithuaniansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_LEITIS, LEITIS, totalCount);

    skipUpdate(720); // LITH_LEITIS
    skipUpdate(721); // LITH_LEITIS
    skipUpdate(747); // LITH_HUSSAR
    skipUpdate(817); // LITH_HUSSAR

    updateEffect(df, 712, totalCount); // LITH_TECH_TREE 
    updateEffect(df, 728, totalCount); // LITH_HILL_FORTS 
    updateEffect(df, 729, totalCount); // LITH_TOWER_SHIELDS 
    updateEffect(df, 734, totalCount); // LITH_FOOD_BONUS 
    const int LITH_FOOD_BONUS_UNIT = 888;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(LITH_FOOD_BONUS_UNIT);
        unit.ResourceStorages.at(0).Amount = multipleAdd(unit.ResourceStorages.at(0).Amount, totalCount);
    }

    updateEffect(df, 745, totalCount); // LITH_TRASH_FASTER 
    updateEffect(df, 736, totalCount); // LITH_RELIC_ATTACK_1 
    updateEffect(df, 737, totalCount); // LITH_RELIC_ATTACK_2 
    updateEffect(df, 738, totalCount); // LITH_RELIC_ATTACK_3 
    updateEffect(df, 739, totalCount); // LITH_RELIC_ATTACK_4 
    updateEffect(df, 713, totalCount); // LITH_MONASTERY_SPEED 
}

void magyarsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_MAGYAR_HUSZAR, MAGYAR_HUSZAR, totalCount);

    skipUpdate(525); // MAG_MAGYAR_HUSSAR
    skipUpdate(526); // MAG_MAGYAR_HUSSAR

    updateEffect(df, 5, totalCount); // MAG_TECH_TREE 
    updateEffect(df, 570, totalCount); // MAG_RECURVE_BOW 
    updateEffect(df, 571, totalCount); // MAG_MERCENARIES 
    updateEffect(df, 528, totalCount); // MAG_ANIMAL_BONUS 
    updateEffect(df, 524, totalCount); // MAG_SCOUTS_DISCOUNT 
    updateEffect(df, 6, totalCount); // MAG_ARCHER_LOS 
}

void malayCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KARAMBIT_WARRIOR, KARAMBIT_WARRIOR, totalCount);

    skipUpdate(656); // MALAY_KARAMBIT_WARRIOR
    skipUpdate(657); // MALAY_KARAMBIT_WARRIOR

    updateEffect(df, 648, totalCount); // MALAY_TECH_TREE 
    updateEffect(df, 664, totalCount); // MALAY_THALASSOCRACY 
    updateEffect(df, 665, totalCount); // MALAY_FORCED_LEVY 
    updateEffect(df, 682, totalCount); // MALAY_OBSOLETE 
    updateEffect(df, 683, totalCount); // MALAY_OBSOLETE_2 
    updateEffect(df, 684, totalCount); // MALAY_OBSOLETE_3 
    updateEffect(df, 674, totalCount); // MALAY_FASTER_AGEADVANCE 
    updateEffect(df, 675, totalCount); // MALAY_FISHTRAP_DISCOUNT 
    updateEffect(df, 677, totalCount); // MALAY_FISHTRAP_FOOD 
    updateEffect(df, 695, totalCount); // MALAY_ELE_DISCOUNT_1 
    updateEffect(df, 696, totalCount); // MALAY_ELE_DISCOUNT_2 
    updateEffect(df, 649, totalCount); // MALAY_DOCK_LOS 
}

void maliansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_GBETO, GBETO, totalCount);

    skipUpdate(596); // MALI_GHBETO
    skipUpdate(597); // MALI_GHBETO

    updateEffect(df, 42, totalCount); // MALI_TECH_TREE 
    updateEffect(df, 605, totalCount); // MALI_TIGUI 
    updateEffect(df, 606, totalCount); // MALI_FARIMBA 
    updateEffect(df, 165, totalCount); // MALI_TC_WOOD_COST 
    updateEffect(df, 617, totalCount); // MALI_WOOD_DISCOUNT 
    updateEffect(df, 618, totalCount); // MALI_INF_ARMOR_1 
    updateEffect(df, 619, totalCount); // MALI_INF_ARMOR_2 
    updateEffect(df, 620, totalCount); // MALI_INF_ARMOR_3 
    updateEffect(df, 621, totalCount); // MALI_GOLDMINING 
    updateEffect(df, 43, totalCount); // MALI_UNIVERSITY_WORKRATE 
}

void mayansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_PLUMED_ARCHER, PLUMED_ARCHER, totalCount);

    skipUpdate(468); // MAY_PLUMED_ARCHER
    skipUpdate(469); // MAY_PLUMED_ARCHER

    updateEffect(df, 449, totalCount); // MAY_TECH_TREE 
    updateEffect(df, 515, totalCount); // MAY_HULCHE_JAVELINEERS 
    updateEffect(df, 456, totalCount); // MAY_EL_DORADO 
    updateEffect(df, 216, totalCount); // MAY_START_RES 
    updateEffect(df, 586, totalCount); // MAY_VILS_SPAWN 
    updateEffect(df, 485, totalCount); // MAY_ARCHER_COST_1 
    updateEffect(df, 486, totalCount); // MAY_ARCHER_COST_2 
    updateEffect(df, 771, totalCount); // MAY_RESOURCES_LONGER_1 
    updateEffect(df, 772, totalCount); // MAY_RESOURCES_LONGER_2 
    updateEffect(df, 773, totalCount); // MAY_RESOURCES_LONGER_3 
    updateEffect(df, 774, totalCount); // MAY_RESOURCES_LONGER_4 
    updateEffect(df, 775, totalCount); // MAY_RESOURCES_LONGER_5 
    updateEffect(df, 776, totalCount); // MAY_RESOURCES_LONGER_6 
    updateEffect(df, 777, totalCount); // MAY_RESOURCES_LONGER_7 
    updateEffect(df, 778, totalCount); // MAY_RESOURCES_LONGER_8 
    updateEffect(df, 779, totalCount); // MAY_RESOURCES_LONGER_9 
    updateEffect(df, 780, totalCount); // MAY_RESOURCES_LONGER_10 
    updateEffect(df, 781, totalCount); // MAY_RESOURCES_LONGER_11 
    updateEffect(df, 489, totalCount); // MAY_WALL_DISCOUNT 
}

void mongolsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_MANGUDAI, MANGUDAI, totalCount);

    skipUpdate(270); // MONG_MANGUDAI
    skipUpdate(369); // MONG_MANGUDAI

    updateEffect(df, 277, totalCount); // MONG_TECH_TREE 
    updateEffect(df, 542, totalCount); // MONG_NOMADS 
    updateEffect(df, 681, totalCount); // MONG_NOMADS_2 
    updateEffect(df, 457, totalCount); // MONG_DRILL 
    updateEffect(df, 393, totalCount); // MONG_CA_FIRING 
    updateEffect(df, 387, totalCount); // MONG_LIGHTCAV_HP 
    updateEffect(df, 388, totalCount); // MONG_HUNTERS 
    updateEffect(df, 407, totalCount); // MONG_SCOUT_LOS 
}

void persiansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_WAR_ELEPHANT, WAR_ELEPHANT, totalCount);

    skipUpdate(271); // PERS_WAR_ELEPHANT
    skipUpdate(365); // PERS_WAR_ELEPHANT

    updateEffect(df, 260, totalCount); // PERS_TECH_TREE 
    updateEffect(df, 458, totalCount); // PERS_MAHOUTS 
    updateEffect(df, 543, totalCount); // PERS_KAMANDARAN 
    updateEffect(df, 212, totalCount); // PERS_START_RES 
    updateEffect(df, 340, min(totalCount, 3.0f)); // PERS_TC_HP 
    updateEffect(df, 347, min(totalCount, 3.0f)); // PERS_DOCK_HP 
    updateEffect(df, 421, totalCount); // PERS_TC_SPEED 
    updateEffect(df, 424, totalCount); // PERS_DOCK_SPEED 
    updateEffect(df, 408, totalCount); // PERS_KNIGHT_BONUS 
}

void polesCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_OBUCH, OBUCH, totalCount);
    updateUnitFromBase(df, 1707, LIGHT_CAVALRY, totalCount);
    updateUnitFromBase(df, 1734, 68, totalCount);
    updateUnitFromBase(df, 1711, 129, totalCount);
    updateUnitFromBase(df, 1720, 130, totalCount);

    skipUpdate(805); // POL_OBUCH
    skipUpdate(806); // POL_OBUCH

    updateEffect(df, 801, totalCount); // POL_TECH_TREE 
    updateEffect(df, 809, totalCount); // POL_SZLACHTA_PRIVILEGES 
    updateEffect(df, 810, totalCount); // POL_LECHITIC_LEGACY 
    updateEffect(df, 815, totalCount); // POL_VIL_REGEN 
    updateEffect(df, 816, totalCount); // POL_FOLWARK 
    updateEffect(df, 818, totalCount); // POL_FOLWARK_2 
    updateEffect(df, 819, totalCount); // POL_FOLWARK_3 
    updateEffect(df, 820, totalCount); // POL_FOLWARK_4 
    updateEffect(df, 821, totalCount); // POL_FOLWARKFARM_FOOD_1 
    updateEffect(df, 822, totalCount); // POL_FOLWARKFARM_FOOD_2 
    updateEffect(df, 823, totalCount); // POL_FOLWARKFARM_FOOD_3 
    updateEffect(df, 827, totalCount); // POL_STONE_GOLDGEN_1 
    updateEffect(df, 828, totalCount); // POL_STONE_GOLDGEN_2 
    updateEffect(df, 829, totalCount); // POL_STONE_GOLDGEN_3 
    updateEffect(df, 802, totalCount); // POL_SCOUTS_BONUS 
}

void portugueseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_ORGAN_GUN, ORGAN_GUN, totalCount);
    updateUnitFromBase(df, ELITE_CARAVEL, CARAVEL, totalCount);

    skipUpdate(591); // PORT_ORGAN_GUN
    skipUpdate(592); // PORT_ORGAN_GUN
    skipUpdate(622); // PORT_CARAVEL
    skipUpdate(623); // PORT_CARAVEL

    updateEffect(df, 31, totalCount); // PORT_TECH_TREE 
    updateEffect(df, 601, totalCount); // PORT_CARRACK 
    updateEffect(df, 602, totalCount); // PORT_ARQUEBUS 
    updateEffect(df, 35, totalCount); // PORT_OBSOLETE 
    updateEffect(df, 33, totalCount); // PORT_GOLD_DISCOUNT 
    updateEffect(df, 510, totalCount); // PORT_FORAGERS 
    // Ships + 10% hp

    updateEffect(df, 600, totalCount); // PORT_FEITORIA_AVAIL 
    auto add_res_command = new genie::EffectCommand();
    add_res_command->Type = 1;
    add_res_command->A = 265;
    add_res_command->B = 0;
    add_res_command->C =-1;
    add_res_command->D = 1;
    df->Effects.at(600).EffectCommands.push_back(*add_res_command);

    const int PORT_FEITORIA = 1021;
    for (auto &civ : df->Civs) {
        auto &unit = civ.Units.at(PORT_FEITORIA);

        auto tasks_count = unit.Bird.TaskList.size();
        for (int i = 0; i < tasks_count; i++) {
            unit.Bird.TaskList.at(i).WorkValue1 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue1, totalCount);
            unit.Bird.TaskList.at(i).WorkValue2 = multipleAdd(unit.Bird.TaskList.at(i).WorkValue2, totalCount);
        }

        unit.ResourceStorages[2].Type = 265;
        unit.ResourceStorages[2].Amount = -1;
        unit.ResourceStorages[2].Flag = 2;

        unit.Creatable.ResourceCosts[2].Type = 265;
        unit.Creatable.ResourceCosts[2].Amount = 1;
        unit.Creatable.ResourceCosts[2].Flag = 0;
    }

    updateEffect(df, 32, totalCount); // PORT_RESEARCH_SPEED 
}

void romansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, 1792, 1790, totalCount);
    updateUnitFromBase(df, 1793, LONG_SWORDSMAN, totalCount);

    skipUpdate(892); // ROM_CENTURION
    skipUpdate(893); // ROM_CENTURION
    skipUpdate(896); // ROM_LEGIONARY
    skipUpdate(903); // ROM_LEGIONARY

    updateEffect(df, 890, totalCount); // ROM_TECH_TREE 
    updateEffect(df, 894, totalCount); // ROM_BALLISTAS 
    updateEffect(df, 895, totalCount); // ROM_COMITATENSES 
    updateEffect(df, 901, totalCount); // ROM_SCORP_BALLISTICS 
    updateEffect(df, 898, totalCount); // ROM_VILS_WORK 
    updateEffect(df, 899, totalCount); // ROM_NAVY 
    auto scale_mail_id = duplicateEffect(df, SCALE_MAIL_ARMOR);
    updateEffect(df, scale_mail_id, totalCount);
    df->Techs.at(889).EffectID = scale_mail_id;

    auto chain_mail_id = duplicateEffect(df, CHAIN_MAIL_ARMOR);
    updateEffect(df, chain_mail_id, totalCount);
    df->Techs.at(890).EffectID = chain_mail_id;

    auto plate_mail_id = duplicateEffect(df, PLATE_MAIL_ARMOR);
    updateEffect(df, plate_mail_id, totalCount);
    df->Techs.at(891).EffectID = plate_mail_id;
    
    updateEffect(df, 900, totalCount); // ROM_SCORP_DISCOUT 
}

void saracensCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_MAMELUKE, MAMELUKE, totalCount);

    skipUpdate(266); // SAR_MAMELUKE
    skipUpdate(366); // SAR_MAMELUKE

    updateEffect(df, 261, totalCount); // SAR_TECH_TREE 
    updateEffect(df, 459, totalCount); // SAR_ZEALOTRY 
    updateEffect(df, 480, totalCount); // SAR_COUNTERWEIGHTS 
    updateEffect(df, 545, totalCount); // SAR_OBSOLETE 
    updateEffect(df, 354, totalCount); // SAR_MARKET 
    updateEffect(df, 311, totalCount); // SAR_TRANSPORT 
    updateEffect(df, 416, totalCount); // SAR_GALLEY_ATTACK 
    updateEffect(df, 312, totalCount); // SAR_CAMEL_HP 
    updateEffect(df, 313, totalCount); // SAR_ARCHER_BONUS_1 
    updateEffect(df, 318, totalCount); // SAR_ARCHER_BONUS_2 
    updateEffect(df, 409, totalCount); // SAR_ARCHER_BONUS 
}

void siciliansCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_SERJEANT, SERJEANT, totalCount);
    updateUnitFromBase(df, 1661, 1660, totalCount);
    updateUnitFromBase(df, 1665, 79, totalCount);

    skipUpdate(788); // SIC_SERJEANT
    skipUpdate(789); // SIC_SERJEANT

    updateEffect(df, 784, totalCount); // SIC_TECH_TREE 
    updateEffect(df, 792, totalCount); // SIC_FIRST_CRUSADE 
    updateEffect(df, 793, totalCount); // SIC_HAUBERK 
    updateEffect(df, 217, totalCount); // SIC_START_RES 
    updateEffect(df, 795, totalCount); // SIC_BUILD_SPEED 
    updateEffect(df, 796, totalCount); // SIC_BONUS_RESISTANCE 
    updateEffect(df, 797, totalCount); // SIC_FARM_FOOD_1 
    updateEffect(df, 798, totalCount); // SIC_FARM_FOOD_2 
    updateEffect(df, 799, totalCount); // SIC_FARM_FOOD_3 
    updateEffect(df, 800, totalCount); // SIC_DONJON_AVAIL 
    updateEffect(df, 785, totalCount); // SIC_TRANSPORT 
}

void slavsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_BOYAR, BOYAR, totalCount);

    skipUpdate(556); // SLAVS_BOYAR
    skipUpdate(557); // SLAVS_BOYAR

    updateEffect(df, 7, totalCount); // SLAVS_TECH_TREE 
    updateEffect(df, 481, totalCount); // SLAVS_DETINETS 
    updateEffect(df, 569, totalCount); // SLAVS_DRUZHINA 
    updateEffect(df, 690, totalCount); // SLAVS_FARMS 
    updateEffect(df, 567, totalCount); // SLAVS_SIEGE_DISCOUNT 
    updateEffect(df, 758, totalCount); // SLAVS_MILITARY_POP 
}

void spanishCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_CONQUISTADOR, CONQUISTADOR, totalCount);

    skipUpdate(491); // SPANISH_CONQ
    skipUpdate(492); // SPANISH_CONQ

    updateEffect(df, 446, totalCount); // SPAN_TECH_TREE 
    updateEffect(df, 547, totalCount); // SPAN_INQUISITION 
    updateEffect(df, 495, totalCount); // SPAN_SUPREMACY 
    updateEffect(df, 168, totalCount); // SPAN_TC_BUILD 
    updateEffect(df, 300, totalCount); // SPAN_TECH_REWARD 
    updateEffect(df, 181, totalCount); // SPAN_TECH_REWARD_2 
    updateEffect(df, 237, totalCount); // SPAN_TECH_REWARD_3 
    updateEffect(df, 496, totalCount); // SPAN_MISSIONARY_AVAIL 
    updateEffect(df, 490, totalCount); // SPAN_TRADE 
}

void tatarsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_KESHIK, KESHIK, totalCount);
    updateUnitFromBase(df, FLAMING_CAMEL, PETARD, totalCount);

    skipUpdate(716); // TAT_KESHIK
    skipUpdate(717); // TAT_KESHIK

    updateEffect(df, 708, totalCount); // TAT_TECH_TREE 
    updateEffect(df, 724, totalCount); // TAT_SILK_ARMOR 
    updateEffect(df, 725, totalCount); // TAT_TIMURID 
    updateEffect(df, 741, totalCount); // TAT_SHEEP_BONUS 
    updateEffect(df, 299, totalCount); // TAT_SHEEP_BONUS_2 
    updateEffect(df, 303, totalCount); // TAT_SHEEP_BONUS_3 
    updateEffect(df, 305, totalCount); // TAT_SHEEP_BONUS_4 
    updateEffect(df, 735, totalCount); // TAT_PARTHIAN_TACTICS 
    updateEffect(df, 733, totalCount); // TAT_ELEVATION 
    updateEffect(df, 709, totalCount); // TAT_CA_LOS 
}

void teutonsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_TEUTONIC_KNIGHT, TEUTONIC_KNIGHT, totalCount);

    skipUpdate(273); // TEUT_TEUTONIC_KNIGHT
    skipUpdate(362); // TEUT_TEUTONIC_KNIGHT

    updateEffect(df, 262, totalCount); // TEUT_TECH_TREE 
    updateEffect(df, 544, totalCount); // TEUT_IRONCLAD 
    updateEffect(df, 461, totalCount); // TEUT_CRENELLATIONS 
    updateEffect(df, 332, totalCount); // TEUT_OBSOLETE 
    updateEffect(df, 345, totalCount); // TEUT_HEAL_RANGE 
    updateEffect(df, 352, totalCount); // TEUT_TOWER_GARISON 
    updateEffect(df, 314, totalCount); // TEUT_FARMS_DISCOUNT 
    updateEffect(df, 335, totalCount); // TEUT_TC_IMPROVE 
    updateEffect(df, 333, totalCount); // TEUT_ARMOR_1 
    updateEffect(df, 334, totalCount); // TEUT_ARMOR_2 
    updateEffect(df, 404, totalCount); // TEUT_CONVERSION_RESISTANCE 
}

void turksCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_JANISSARY, JANISSARY, totalCount);

    skipUpdate(268); // TURK_JANNISARY
    skipUpdate(353); // TURK_CASTLE
    skipUpdate(367); // TURK_JANNISARY

    updateEffect(df, 263, totalCount); // TURK_TECH_TREE 
    updateEffect(df, 546, totalCount); // TURK_SIPAHI 
    updateEffect(df, 460, totalCount); // TURK_ARTILLERY 
    updateEffect(df, 527, totalCount); // TURK_HUSSAR_FREE 
    updateEffect(df, 301, totalCount); // TURK_CHEMISTRY_FREE 
    updateEffect(df, 296, totalCount); // TURK_GUNPOWDER_HP 
    updateEffect(df, 284, totalCount); // TURK_GUNPOWDERSHIP_HP 
    updateEffect(df, 295, totalCount); // TURK_GOLD_MINING 
    updateEffect(df, 509, totalCount); // TURK_LIGHTCAV_PIERCE 
    updateEffect(df, 410, totalCount); // TURK_GUNPOWDER_TRAINTIME 
}

void vietnameseCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_RATTAN_ARCHER, RATTAN_ARCHER, totalCount);
    updateUnitFromBase(df, IMPERIAL_SKIRMISHER, ELITE_SKIRMISHER, totalCount);

    skipUpdate(660); // VIET_RATTAN
    skipUpdate(661); // VIET_RATTAN

    updateEffect(df, 652, totalCount); // VIET_TECH_TREE 
    updateEffect(df, 668, totalCount); // VIET_CHATRAS 
    updateEffect(df, 669, totalCount); // VIET_PAPER_MONEY_1 
    updateEffect(df, 881, totalCount); // VIET_PAPER_MONEY_2 
    updateEffect(df, 882, totalCount); // VIET_PAPER_MONEY_3 
    updateEffect(df, 883, totalCount); // VIET_PAPER_MONEY_4 
    updateEffect(df, 672, totalCount); // VIET_ARCHERS_HP 
    updateEffect(df, 698, totalCount); // VIET_VISION 
}

void vikingsCivBonus(genie::DatFile *df, float totalCount) {
    updateUnitFromBase(df, ELITE_BERSERK, BERSERK, totalCount);
    updateUnitFromBase(df, ELITE_LONGBOAT, LONGBOAT, totalCount);

    skipUpdate(251); // VIK_LONGBOAT
    skipUpdate(269); // VIK_LONGBOAT
    skipUpdate(370); // VIK_LONGBOAT
    skipUpdate(397); // VIK_BERSERK
    skipUpdate(398); // VIK_BERSERK

    updateEffect(df, 276, totalCount); // VIK_TECH_TREE 
    updateEffect(df, 517, totalCount); // VIK_CHIEFTAINS 
    updateEffect(df, 467, totalCount); // VIK_BOGSVEIGAR 
    updateEffect(df, 391, totalCount); // VIK_WHEELBARROW_FREE 
    updateEffect(df, 412, totalCount); // VIK_HANDCART_FREE 
    updateEffect(df, 383, totalCount); // VIK_WARSHIP_COST_1 
    updateEffect(df, 386, totalCount); // VIK_WARSHIP_COST_2 
    updateEffect(df, 394, totalCount); // VIK_WARSHIP_COST_3 
    updateEffect(df, 390, totalCount); // VIK_INF_HP_1 
    updateEffect(df, 427, totalCount); // VIK_INF_HP_2 
    updateEffect(df, 428, totalCount); // VIK_INF_HP_3 
    updateEffect(df, 411, totalCount); // VIK_DOCK_COST 
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

    // auto civs_cout = df->Civs.size();
    // for (auto &effect : updated_effects) {
    //     bool effect_contained = false;
    //     for (auto i = 0; i < techs_count; i++) {
    //         auto &tech = df->Techs.at(i);
    //         if (tech.EffectID == effect) {
    //             effect_contained = true;
    //             break;
    //         }
    //     }

    //     for (auto i = 0; i < civs_cout; i++) {
    //         auto &civ = df->Civs.at(i);
    //         if (civ.TechTreeID == effect ||
    //             civ.TeamBonusID == effect) {
    //             effect_contained = true;
    //             break;
    //         }
    //     }

    //     if (!effect_contained) {
    //         cout << "Effect " << effect << ":" << df->Effects.at(effect).Name << " not contained in any tech or civ\n";
    //     }
    // }

    // for (auto i = 0; i < techs_count; i++) {
    //     auto &tech = df->Techs.at(i);
    //     auto required_techs_count = tech.RequiredTechCount;

    //     for (auto j = 0; j < required_techs_count; j++) {
    //         auto &required_tech = tech.RequiredTechs.at(j);
    //         if (required_tech == 725) {
    //             cout << "Tech " << i << ":" << tech.Name << "needs 725\n";
    //         }
    //     }
    // }
}