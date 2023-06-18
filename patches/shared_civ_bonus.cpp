#include <string>
#include <iostream>
#include <functional>
#include <list>
#include <vector>
#include <set>
#include <math.h>
#include "shared_civ_bonus.h"
#include "genie/dat/DatFile.h"
#include "ids.h"

using namespace std;

set<int> shared_effects;

#pragma region Helper_Methods

void skipShare(int effect_id) {
    auto effect_updated = shared_effects.find(effect_id) != shared_effects.end();
    if (effect_updated) {
        cout << "Effect " << effect_id << " was already updated\n";
        return;
    } else {
        shared_effects.insert(effect_id);
    }
}

int duplicateForShare(
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

void shareEffect(
    genie::DatFile *df,
    int effect_id,
    int team_bonus_id) {
    auto &effect = df->Effects.at(effect_id);

    if (effect_id == team_bonus_id) {
        return;
    }

    auto effect_updated = shared_effects.find(effect_id) != shared_effects.end();
    if (effect_updated) {
        cout << "Effect " << effect_id << " was already updated\n";
        return;
    } else {
        shared_effects.insert(effect_id);
    }

    bool is_behind_cost = false;
    auto techs_count = df->Techs.size();
    for (auto j = 0; j < techs_count; j++) {
        auto &tech = df->Techs.at(j);
        if (tech.EffectID == effect_id) {
            is_behind_cost = tech.ResearchTime > 0;
            break;
        }
    }

    for (int i = 0; i < effect.EffectCommands.size(); i++) {
        auto &command = effect.EffectCommands.at(i);
        switch (command.Type)
        {
            case 0: // attribute set
                command.Type = 10;
                break;
            case 2: // enable/disable unit
                if (command.B == 1) {
                    command.Type = 12;
                }
                break;
            case 3: // upgrade units
                command.Type = 13;
                break;
            case 7: // spawn units
                command.Type = 17;
                break;
            case 4: // attribute +-
                command.Type = 14;
                break;
            case 1: // resource +-
                command.Type = 11;
                break;
            case 5: // attribute */
                command.Type = 15;
                break;
            case 6: // resource modifier */
                command.Type = 16;
                break;
            case 101: // tech cost modifier
            case 103: // tech cost modifier
                if (is_behind_cost)
                    break;

                auto moved_command = new genie::EffectCommand();
                moved_command->Type = command.Type;
                moved_command->A = command.A;
                moved_command->B = command.B;
                moved_command->C = command.C;
                moved_command->D = command.D;

                effect.EffectCommands.erase(effect.EffectCommands.begin() + i);
                df->Effects.at(team_bonus_id).EffectCommands.push_back(*moved_command);
                i--;
                break;
        }
    }
}

void shareTech(
    genie::DatFile *df,
    int tech_id) {
    auto &tech = df->Techs.at(tech_id);
    auto &civ = df->Civs.at(tech.Civ);
    auto &civ_bonus = df->Effects.at(civ.TeamBonusID);

    tech.Civ = -1;
    tech.ResourceCosts.at(0).Amount = 1;
    tech.ResourceCosts.at(0).Type = 0;
    tech.ResourceCosts.at(0).Flag = 1;
    tech.ResearchTime = 1;

    auto remove_food_cost_command = new genie::EffectCommand();
    remove_food_cost_command->Type = 101;
    remove_food_cost_command->A = tech_id;
    remove_food_cost_command->B = 0;
    remove_food_cost_command->C = 0;
    remove_food_cost_command->D = 0;
    civ_bonus.EffectCommands.push_back(*remove_food_cost_command);

    auto remove_research_time_command = new genie::EffectCommand();
    remove_research_time_command->Type = 103;
    remove_research_time_command->A = tech_id;
    remove_research_time_command->B = 0;
    remove_research_time_command->C = 0;
    remove_research_time_command->D = 0;
    civ_bonus.EffectCommands.push_back(*remove_research_time_command);
}

#pragma endregion Helper_Methods

void aztecsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 488;

    skipShare(346); // AZT_XOLOTL
    skipShare(442); // AZT_JAGUAR_MAN
    skipShare(444); // AZT_JAGUAR_MAN

    shareEffect(df, 447, team_bonus_id); // AZT_TECH_TREE
    shareEffect(df, 514, team_bonus_id); // AZT_ATLATL
    shareEffect(df, 465, team_bonus_id); // AZT_GARLAND_WARS
    shareEffect(df, 470, team_bonus_id); // AZT_MONK_HP_1
    shareEffect(df, 471, team_bonus_id); // AZT_MONK_HP_2
    shareEffect(df, 472, team_bonus_id); // AZT_MONK_HP_3
    shareEffect(df, 473, team_bonus_id); // AZT_MONK_HP_4
    shareEffect(df, 474, team_bonus_id); // AZT_MONK_HP_5
    shareEffect(df, 475, team_bonus_id); // AZT_MONK_HP_6
    shareEffect(df, 476, team_bonus_id); // AZT_MONK_HP_7
    shareEffect(df, 477, team_bonus_id); // AZT_MONK_HP_8
    shareEffect(df, 478, team_bonus_id); // AZT_MONK_HP_9
    shareEffect(df, 512, team_bonus_id); // AZT_MONK_HP_10
    shareEffect(df, 213, team_bonus_id); // AZT_START_RES
    shareEffect(df, 488, team_bonus_id); // AZT_RELIC
}

void bengalisCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 841;

    skipShare(846); // BENG_RATHA
    skipShare(847); // BENG_RATHA

    shareEffect(df, 840, team_bonus_id); // BENG_TECH_TREE
    shareEffect(df, 852, team_bonus_id); // BENG_PAIKS
    shareEffect(df, 853, team_bonus_id); // BENG_MAHAYANA
    shareEffect(df, 865, team_bonus_id); // BENG_RESIST
    shareEffect(df, 866, team_bonus_id); // BENG_VILS
    shareEffect(df, 888, team_bonus_id); // BENG_CAV_BONUS
    shareEffect(df, 867, team_bonus_id); // BENG_SHIPS_REGENERATE
    shareEffect(df, 568, team_bonus_id); // BENG_MONK_DEFENSE
    shareEffect(df, 841, team_bonus_id); // BENG_TRADE
}

void berbersCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 38;

    skipShare(594); // BERB_CAMEL_ARCHER
    skipShare(595); // BERB_CAMEL_ARCHER
    skipShare(440); // BERB_GENITOUR
    skipShare(624); // BERB_GENITOUR
    skipShare(627); // BERB_GENITOUR

    shareEffect(df, 37, team_bonus_id); // BERB_TECH_TREE
    shareEffect(df, 607, team_bonus_id); // BERB_KASBAH
    shareEffect(df, 608, team_bonus_id); // BERB_MAGHREBI_CAMELS
    shareEffect(df, 609, team_bonus_id); // BERB_VILLAGERS_FASTER_1
    shareEffect(df, 626, team_bonus_id); // BERB_VILLAGERS_FASTER_2
    shareEffect(df, 610, team_bonus_id); // BERB_CAVALRY_DISCOUNT_1
    shareEffect(df, 638, team_bonus_id); // BERB_CAVALRY_DISCOUNT_2
    shareEffect(df, 34, team_bonus_id); // BERB_SHIPS_FASTER_1
    shareEffect(df, 611, team_bonus_id); // BERB_SHIPS_FASTER_2
}

void bohemiansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 804;

    skipShare(807); // BOH_HUSSITE_WAGON
    skipShare(808); // BOH_HUSSITE_WAGON
    shareEffect(df, 814, team_bonus_id); // BOH_HOUFNICE

    shareEffect(df, 803, team_bonus_id); // BOH_TECH_TREE
    shareEffect(df, 811, team_bonus_id); // BOH_WAGENBURG_TACTICS
    shareEffect(df, 812, team_bonus_id); // BOH_HUSSITE_REFORMS
    shareEffect(df, 830, team_bonus_id); // BOH_BLACKSMITH_DISCOUNT
    shareEffect(df, 824, team_bonus_id); // BOH_SPEAR_BONUS
    shareEffect(df, 825, team_bonus_id); // BOH_VIL_SANCTITY
    shareEffect(df, 826, team_bonus_id); // BOH_VIL_FERVOR
    shareEffect(df, 804, team_bonus_id); // BOH_MARKET_SPEED
}

void britonsCivBonus(genie::DatFile *df) {

    skipShare(251); // BRIT_LONGBOWMAN
    skipShare(358); // BRIT_LONGBOWMAN
    
    auto team_bonus_id = 399;
    shareEffect(df, 254, team_bonus_id); // BRIT_TECH_TREE
    shareEffect(df, 455, team_bonus_id); // BRIT_YEOMEN
    shareEffect(df, 540, team_bonus_id); // BRIT_WARWOLF
    shareEffect(df, 379, team_bonus_id); // BRIT_TC_DISCOUNT
    shareEffect(df, 380, team_bonus_id); // BRIT_ARCHER_RANGE_1
    shareEffect(df, 415, team_bonus_id); // BRIT_ARCHER_RANGE_2
    shareEffect(df, 381, team_bonus_id); // BRIT_SHEPARD
    shareEffect(df, 399, team_bonus_id); // BRIT_RANGE_SPEED
}

void bulgariansCivBonus(genie::DatFile *df) {    
    auto team_bonus_id = 707;

    skipShare(714); // BULG_KONNIK
    skipShare(715); // BULG_KONNIK

    shareEffect(df, 706, team_bonus_id); // BULG_TECH_TREE
    shareEffect(df, 722, team_bonus_id); // BULG_STIRRUPS
    shareEffect(df, 723, team_bonus_id); // BULG_BAGAINS
    shareEffect(df, 730, team_bonus_id); // BULG_MILITIA_UPGRADES
    shareEffect(df, 171, team_bonus_id); // BULG_TC_STONE_DISCOUNT_1
    shareEffect(df, 731, team_bonus_id); // BULG_TC_STONE_DISCOUNT_2
    shareEffect(df, 707, team_bonus_id); // BULG_BLACKSMITH_FASTER
    shareEffect(df, 732, team_bonus_id); // BULD_ENABLE_CREPOST
}

void burgundiansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 783;

    skipShare(786); // BURG_COUSTILLIER
    skipShare(787); // BURG_COUSTILLIER

    shareEffect(df, 782, team_bonus_id); // BURG_TECH_TREE
    shareEffect(df, 790, team_bonus_id); // BURG_BURGUNDIAN_VINEYARDS
    shareEffect(df, 791, team_bonus_id); // BURG_FLEMISH_REVOLUTION
    shareEffect(df, 794, team_bonus_id); // BURG_GUNPOWDER
    shareEffect(df, 783, team_bonus_id); // BURG_RELIC_FOOD
}

void burmeseCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 651;

    skipShare(658); // BURM_ARAMBAI
    skipShare(659); // BURM_ARAMBAI

    shareEffect(df, 650, team_bonus_id); // BURM_TECH_TREE
    shareEffect(df, 666, team_bonus_id); // BURM_HOWDAH
    shareEffect(df, 667, team_bonus_id); // BURM_MANIPUR_CAVALRY
    shareEffect(df, 685, team_bonus_id); // BURM_LUMBER_UPGRADES
    shareEffect(df, 686, team_bonus_id); // BURM_INF_ATTACK_1
    shareEffect(df, 687, team_bonus_id); // BURM_INF_ATTACK_2
    shareEffect(df, 688, team_bonus_id); // BURM_INF_ATTACK_3
    shareEffect(df, 699, team_bonus_id); // BURM_INF_ATTACK_4
    shareEffect(df, 679, team_bonus_id); // BURM_ELEP_ARMOR
    shareEffect(df, 689, team_bonus_id); // BURM_MONASTERY_DISCOUNT
}

void byzantinesCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 400;

    skipShare(264); // BYZ_CATAPHRACT
    skipShare(359); // BYZ_CATAPHRACT

    shareEffect(df, 256, team_bonus_id); // BYZ_TECH_TREE
    shareEffect(df, 518, team_bonus_id); // BYZ_GREEK_FIRE
    shareEffect(df, 493, team_bonus_id); // BYZ_LOGISTICA
    shareEffect(df, 413, team_bonus_id); // BYZ_OBSOLETE
    shareEffect(df, 282, team_bonus_id); // BYZ_BUILDINGS_HP_1
    shareEffect(df, 429, team_bonus_id); // BYZ_BUILDINGS_HP_2
    shareEffect(df, 430, team_bonus_id); // BYZ_BUILDINGS_HP_3
    shareEffect(df, 431, team_bonus_id); // BYZ_BUILDINGS_HP_4
    shareEffect(df, 283, team_bonus_id); // BYZ_COUNTER_DISCOUNT
    shareEffect(df, 396, team_bonus_id); // BYZ_SHIP_ATTACKSPEED
    shareEffect(df, 400, team_bonus_id); // BYZ_HEALSPEED
}

void celtsCivBonus(genie::DatFile *df) {    
    auto team_bonus_id = 401;

    skipShare(274); // CELT_WOAD_RIDER
    skipShare(368); // CELT_WOAD_RIDER

    shareEffect(df, 275, team_bonus_id); // CELT_TECH_TREE
    shareEffect(df, 537, team_bonus_id); // CELT_STRONGHOLD
    shareEffect(df, 239, team_bonus_id); // CELT_FUROR_CELTICA
    shareEffect(df, 392, team_bonus_id); // CELT_INF_SPEED
    shareEffect(df, 384, team_bonus_id); // CELT_LUMBERJACK
    shareEffect(df, 385, team_bonus_id); // CELT_SIEGE_FIRING
    shareEffect(df, 417, team_bonus_id); // CELT_DOMINANT_LOS
    shareEffect(df, 401, team_bonus_id); // CELTS_SIEGEWORKSHOP_SPEED
}

void chineseCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 402;

    skipShare(265); // CHIN_CHUKONU
    skipShare(360); // CHIN_CHUKONU

    shareEffect(df, 257, team_bonus_id); // CHIN_TECH_TREE
    shareEffect(df, 516, team_bonus_id); // CHIN_GREAT_WALL
    shareEffect(df, 483, team_bonus_id); // CHIN_ROCKETRY
    shareEffect(df, 215, team_bonus_id); // CHIN_START_RES
    shareEffect(df, 302, team_bonus_id); // CHIN_ADDITIONAL_VILS
    shareEffect(df, 304, team_bonus_id); // CHIN_TECH_DISCOUNT_1
    shareEffect(df, 349, team_bonus_id); // CHIN_TECH_DISCOUNT_2
    shareEffect(df, 350, team_bonus_id); // CHIN_TECH_DISCOUNT_3
    shareEffect(df, 351, team_bonus_id); // CHIN_TECH_DISCOUNT_4
    shareEffect(df, 437, team_bonus_id); // CHIN_TC_POP
    shareEffect(df, 395, team_bonus_id); // CHIN_DEMO_HP
    shareEffect(df, 402, team_bonus_id); // CHIN_FARM_FOOD_1
    shareEffect(df, 831, team_bonus_id); // CHIN_FARM_FOOD_2
    shareEffect(df, 832, team_bonus_id); // CHIN_FARM_FOOD_3
    shareEffect(df, 833, team_bonus_id); // CHIN_FARM_FOOD_4
    shareEffect(df, 834, team_bonus_id); // CHIN_FARM_FOOD_5
    shareEffect(df, 835, team_bonus_id); // CHIN_FARM_FOOD_6
    shareEffect(df, 836, team_bonus_id); // CHIN_FARM_FOOD_7
    shareEffect(df, 837, team_bonus_id); // CHIN_FARM_FOOD_8
}

void cumansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 711;

    skipShare(718); // CUMANS_KIPCHAK
    skipShare(719); // CUMANS_KIPCHAK
    skipShare(759); // CUMANS_TC_BUILD_TIME
    skipShare(760); // CUMANS_TC_BUILD_TIME

    shareEffect(df, 710, team_bonus_id); // CUMANS_TECH_TREE
    shareEffect(df, 726, team_bonus_id); // CUMANS_STEPPE_HUSBANDRY
    shareEffect(df, 743, team_bonus_id); // CUMANS_CUMAN_MERCENARIES
    shareEffect(df, 727, team_bonus_id); // CUMANS_CUMAN_MERCENARIES_2
    shareEffect(df, 761, team_bonus_id); // CUMANS_TC
    shareEffect(df, 742, team_bonus_id); // CUMANS_SIEGE
    shareEffect(df, 697, team_bonus_id); // CUMANS_WOOD_DISCOUNT
    shareEffect(df, 746, team_bonus_id); // CUMANS_CAV_FASTER_1
    shareEffect(df, 748, team_bonus_id); // CUMANS_CAV_FASTER_2
    shareEffect(df, 762, team_bonus_id); // CUMANS_CAV_FASTER_3
    shareEffect(df, 763, team_bonus_id); // CUMANS_CAV_FASTER_4
    shareEffect(df, 711, team_bonus_id); // CUMANS_PALISADE_HP
}

void dravidiansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 839;

    skipShare(844); // DRAV_URUMI_SWORDSMAN
    skipShare(845); // DRAV_URUMI_SWORDSMAN

    shareEffect(df, 838, team_bonus_id); // DRAV_TECH_TREE
    shareEffect(df, 850, team_bonus_id); // DRAV_MEDICAL_CORPS
    shareEffect(df, 851, team_bonus_id); // DRAV_WOOTZ_STEEL
    shareEffect(df, 860, team_bonus_id); // DRAV_THIRISADAI
    shareEffect(df, 868, team_bonus_id); // DRAV_BONUS_WOOD
    shareEffect(df, 863, team_bonus_id); // DRAV_FISHING_CARRY
    shareEffect(df, 887, team_bonus_id); // DRAV_SIEGE_DISCOUNT
    shareEffect(df, 864, team_bonus_id); // DRAV_SKIRM_ATTACKFASTER
    shareEffect(df, 839, team_bonus_id); // DRAV_DOCK_POP
}

void ethiopiansCivBonus(genie::DatFile *df) {    
    auto team_bonus_id = 49;

    skipShare(598); // ETH_SHOTEL_WARRIOR
    skipShare(599); // ETH_SHOTEL_WARRIOR

    shareEffect(df, 48, team_bonus_id); // ETH_TECH_TREE
    shareEffect(df, 603, team_bonus_id); // ETH_ROYAL_HEIRS
    shareEffect(df, 604, team_bonus_id); // ETH_TORSION_ENGINES
    shareEffect(df, 634, team_bonus_id); // ETH_TORSION_ENGINES_2
    shareEffect(df, 612, team_bonus_id); // ETH_ACHER_FIREFASTER
    shareEffect(df, 613, team_bonus_id); // ETH_BONUS_RES_1
    shareEffect(df, 614, team_bonus_id); // ETH_BONUS_RES_2
    shareEffect(df, 615, team_bonus_id); // ETH_BONUS_RES_3
    shareEffect(df, 616, team_bonus_id); // ETH_PAIK_FREE
    shareEffect(df, 49, team_bonus_id); // ETH_OUTPOST_LOS
}

void franksCivBonus(genie::DatFile *df) {    
    auto team_bonus_id = 403;

    skipShare(272); // FRANKS_THROWING_AXEMAN
    skipShare(361); // FRANKS_THROWING_AXEMAN

    shareEffect(df, 258, team_bonus_id); // FRANK_TECH_TREE
    shareEffect(df, 291, team_bonus_id); // FRANK_BEARDED_AXE
    shareEffect(df, 548, team_bonus_id); // FRANK_CHIVALRY
    shareEffect(df, 324, team_bonus_id); // FRANK_CASTLE_DISCOUNT
    shareEffect(df, 285, team_bonus_id); // FRANK_CAVALRY_HP
    shareEffect(df, 523, team_bonus_id); // FRANK_FORAGERS
    shareEffect(df, 403, team_bonus_id); // FRANK_KNIGHT_LOS
}

void gothsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 405;

    skipShare(363); // GOTH_HUSCARL
    skipShare(507); // GOTH_HUSCARL

    shareEffect(df, 259, team_bonus_id); // GOTH_TECH_TREE
    shareEffect(df, 462, team_bonus_id); // GOTH_ANARCHY
    shareEffect(df, 513, team_bonus_id); // GOTH_PERFUSION
    shareEffect(df, 342, team_bonus_id); // GOTH_INF_DISCOUNT_1
    shareEffect(df, 765, team_bonus_id); // GOTH_INF_DISCOUNT_2
    shareEffect(df, 766, team_bonus_id); // GOTH_INF_DISCOUNT_3
    shareEffect(df, 767, team_bonus_id); // GOTH_INF_DISCOUNT_4
    shareEffect(df, 344, team_bonus_id); // GOTH_INF_DISCOUNT_5
    shareEffect(df, 343, team_bonus_id); // GOTH_INF_DISCOUNT_6
    shareEffect(df, 326, team_bonus_id); // GOTH_INF_BONUS_1
    shareEffect(df, 327, team_bonus_id); // GOTH_INF_BONUS_2
    shareEffect(df, 328, team_bonus_id); // GOTH_INF_BONUS_3
    shareEffect(df, 414, team_bonus_id); // GOTH_HUNT
    shareEffect(df, 341, team_bonus_id); // GOTH_LOOM_INSTANT
    shareEffect(df, 418, team_bonus_id); // GOTH_POP
    shareEffect(df, 405, team_bonus_id); // GOTH_BARRACKS_WORK
}

void gurjarasCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 843;

    skipShare(848); // GUR_CHAKRAM_THROWER
    skipShare(849); // GUR_CHAKRAM_THROWER
    shareEffect(df, 861, team_bonus_id); // GUR_SHRIVAMSHA_RIDER
    shareEffect(df, 862, team_bonus_id); // GUR_SHRIVAMSHA_RIDER

    shareEffect(df, 842, team_bonus_id); // GUR_TECH_TREE
    shareEffect(df, 854, team_bonus_id); // GUR_KSHATRIYAS
    shareEffect(df, 875, team_bonus_id); // GUR_KSHATRIYAS_SUP
    shareEffect(df, 855, team_bonus_id); // GUR_FRONTIER_GUARDS
    shareEffect(df, 872, team_bonus_id); // GUR_FORAGEBUSHES
    shareEffect(df, 871, team_bonus_id); // GUR_MILL_GARISON
    shareEffect(df, 869, team_bonus_id); // GUR_CAV_BONUS_1
    shareEffect(df, 873, team_bonus_id); // GUR_CAV_BONUS_2
    shareEffect(df, 874, team_bonus_id); // GUR_CAV_BONUS_3
    shareEffect(df, 884, team_bonus_id); // GUR_CAV_BONUS_4
    shareEffect(df, 870, team_bonus_id); // GUR_DOCK_GARRISON
    shareEffect(df, 843, team_bonus_id); // GUR_CAMEL_TRAIN
}

void hindustanisCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 2;

    skipShare(858); // HIND_GHULAM
    skipShare(859); // HIND_GHULAM

    shareEffect(df, 1, team_bonus_id); // HIND_TECH_TREE
    shareEffect(df, 577, team_bonus_id); // HIND_IMP_CAMEL
    shareEffect(df, 562, team_bonus_id); // HIND_GRAND_TRUNK_ROAD
    shareEffect(df, 563, team_bonus_id); // HIND_SHATAGNI
    shareEffect(df, 551, team_bonus_id); // HIND_VIL_DISCOUNT_1
    shareEffect(df, 552, team_bonus_id); // HIND_VIL_DISCOUNT_2
    shareEffect(df, 553, team_bonus_id); // HIND_VIL_DISCOUNT_3
    shareEffect(df, 585, team_bonus_id); // HIND_VIL_DISCOUNT_4
    shareEffect(df, 560, team_bonus_id); // HIND_CAMEL_ATTACKSPEED
    shareEffect(df, 576, team_bonus_id); // HIND_GUNPOWDER_ARMOR
    shareEffect(df, 574, team_bonus_id); // HIND_CARAVENSEI_AVAIL
    shareEffect(df, 2, team_bonus_id); // HIND_CAV_ATTACK
}

void hunsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 484;

    skipShare(453); // HUNS_TARKAN
    skipShare(454); // HUNS_TARKAN

    shareEffect(df, 448, team_bonus_id); // HUNS_TECH_TREE
    shareEffect(df, 538, team_bonus_id); // HUNS_MARAUERS
    shareEffect(df, 464, team_bonus_id); // HUNS_ATHEISM
    shareEffect(df, 214, team_bonus_id); // HUNS_START_RES
    shareEffect(df, 497, team_bonus_id); // HUNS_CA_DISCOUNT_1
    shareEffect(df, 498, team_bonus_id); // HUNS_CA_DISCOUNT_2
    shareEffect(df, 484, team_bonus_id); // HUNS_FASTER_STABLES
}

void incasCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 4;

    skipShare(40); // INC_EAGLE_START
    skipShare(564); // INC_KAMAYUK
    skipShare(565); // INC_KAMAYUK
    shareEffect(df, 582, team_bonus_id); // INC_SLINGER

    shareEffect(df, 3, team_bonus_id); // INC_TECH_TREE
    shareEffect(df, 572, team_bonus_id); // INC_ANDEAN_SLING
    shareEffect(df, 573, team_bonus_id); // INC_FABRIC_SHIELDS
    shareEffect(df, 156, team_bonus_id); // INC_FOOD_DISCOUNT_1
    shareEffect(df, 158, team_bonus_id); // INC_FOOD_DISCOUNT_2
    shareEffect(df, 159, team_bonus_id); // INC_FOOD_DISCOUNT_3
    shareEffect(df, 160, team_bonus_id); // INC_FOOD_DISCOUNT_4
    shareEffect(df, 529, team_bonus_id); // INC_VIL_BLACKSMITH_1
    shareEffect(df, 530, team_bonus_id); // INC_VIL_BLACKSMITH_2
    shareEffect(df, 531, team_bonus_id); // INC_VIL_BLACKSMITH_3
    shareEffect(df, 532, team_bonus_id); // INC_VIL_BLACKSMITH_4
    shareEffect(df, 533, team_bonus_id); // INC_VIL_BLACKSMITH_5
    shareEffect(df, 534, team_bonus_id); // INC_VIL_BLACKSMITH_6
    shareEffect(df, 550, team_bonus_id); // INC_HOUSES_POP
    shareEffect(df, 575, team_bonus_id); // INC_STONE_DISCOUNT
    shareEffect(df, 678, team_bonus_id); // INC_SPAWN_LAMA
}

void italiansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 11;

    skipShare(519); // ITAL_GENOESE_CROSSBOWMAN
    skipShare(520); // ITAL_GENOESE_CROSSBOWMAN

    shareEffect(df, 10, team_bonus_id); // ITAL_TECH_TREE
    shareEffect(df, 549, team_bonus_id); // ITAL_PAVISE
    shareEffect(df, 554, team_bonus_id); // ITAL_SILK_ROAD
    shareEffect(df, 555, team_bonus_id); // ITAL_GUNPOWDER_DISCOUNT
}

void japaneseCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 406;

    skipShare(250); // JAP_SAMURAI
    skipShare(364); // JAP_SAMURAI

    shareEffect(df, 255, team_bonus_id); // JAP_TECH_TREE
    shareEffect(df, 539, team_bonus_id); // JAP_YASAMA
    shareEffect(df, 59, team_bonus_id); // JAP_KATAPARUTO
    shareEffect(df, 432, team_bonus_id); // JAP_OBSOLETE
    shareEffect(df, 433, team_bonus_id); // JAP_OBSOLETE_2
    shareEffect(df, 306, team_bonus_id); // JAP_FISH_IMPROVE
    shareEffect(df, 434, team_bonus_id); // JAP_FISH_WORK_1
    shareEffect(df, 435, team_bonus_id); // JAP_FISH_WORK_2
    shareEffect(df, 436, team_bonus_id); // JAP_FISH_WORK_3
    shareEffect(df, 338, team_bonus_id); // JAP_DROP_DISCOUNT
    shareEffect(df, 339, team_bonus_id); // JAP_ATTACK_SPEED
    shareEffect(df, 406, team_bonus_id); // JAP_GALLEY_LOS
}

void khmerCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 647;

    skipShare(654); // KHMER_BALLISTA_ELEPHANT
    skipShare(655); // KHMER_BALLISTA_ELEPHANT

    shareEffect(df, 646, team_bonus_id); // KHMER_TECH_TREE
    shareEffect(df, 662, team_bonus_id); // KHMER_TUSK_SWORDS
    shareEffect(df, 663, team_bonus_id); // KHMER_DOUBLE_CROSSBOW
    shareEffect(df, 703, team_bonus_id); // KHMER_ELEP_FASTER
    shareEffect(df, 693, team_bonus_id); // KHMER_HOUSE_GARRISON
    shareEffect(df, 647, team_bonus_id); // KHMER_SCORP_RANGE
}

void koreansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 505;

    shareEffect(df, 500, team_bonus_id); // KOREA_TURTLE_SHIP
    shareEffect(df, 501, team_bonus_id); // KOREA_TURTLE_SHIP
    skipShare(502); // KOREA_WAR_WAGON
    skipShare(503); // KOREA_WAR_WAGON

    shareEffect(df, 504, team_bonus_id); // KOREA_TECH_TREE
    shareEffect(df, 541, team_bonus_id); // KOREA_EUPSEONG
    shareEffect(df, 506, team_bonus_id); // KOREA_SHINKICHON
    shareEffect(df, 508, team_bonus_id); // KORA_ARCHER_ARMOR
    shareEffect(df, 561, team_bonus_id); // KOREA_WOOD_DISCOUNT
}

void lithuaniansCivBonus(genie::DatFile *df) {    
    auto team_bonus_id = 713;

    skipShare(720); // LITH_LEITIS
    skipShare(721); // LITH_LEITIS
    skipShare(747); // LITH_HUSSAR
    skipShare(817); // LITH_HUSSAR
    shareEffect(df, 813, team_bonus_id); // LITH_HUSSAR

    shareEffect(df, 712, team_bonus_id); // LITH_TECH_TREE
    shareEffect(df, 728, team_bonus_id); // LITH_HILL_FORTS
    shareEffect(df, 729, team_bonus_id); // LITH_TOWER_SHIELDS
    shareEffect(df, 734, team_bonus_id); // LITH_FOOD_BONUS
    shareEffect(df, 745, team_bonus_id); // LITH_TRASH_FASTER
    shareEffect(df, 736, team_bonus_id); // LITH_RELIC_ATTACK_1
    shareEffect(df, 737, team_bonus_id); // LITH_RELIC_ATTACK_2
    shareEffect(df, 738, team_bonus_id); // LITH_RELIC_ATTACK_3
    shareEffect(df, 739, team_bonus_id); // LITH_RELIC_ATTACK_4
    shareEffect(df, 713, team_bonus_id); // LITH_MONASTERY_SPEED
}

void magyarsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 6;

    skipShare(525); // MAG_MAGYAR_HUSSAR
    skipShare(526); // MAG_MAGYAR_HUSSAR

    shareEffect(df, 5, team_bonus_id); // MAG_TECH_TREE
    shareEffect(df, 570, team_bonus_id); // MAG_RECURVE_BOW
    shareEffect(df, 571, team_bonus_id); // MAG_MERCENARIES
    shareEffect(df, 528, team_bonus_id); // MAG_ANIMAL_BONUS
    shareEffect(df, 524, team_bonus_id); // MAG_SCOUTS_DISCOUNT
    shareEffect(df, 6, team_bonus_id); // MAG_ARCHER_LOS
}

void malayCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 649;

    skipShare(656); // MALAY_KARAMBIT_WARRIOR
    skipShare(657); // MALAY_KARAMBIT_WARRIOR

    shareEffect(df, 648, team_bonus_id); // MALAY_TECH_TREE
    shareEffect(df, 664, team_bonus_id); // MALAY_THALASSOCRACY
    shareEffect(df, 665, team_bonus_id); // MALAY_FORCED_LEVY
    shareEffect(df, 682, team_bonus_id); // MALAY_OBSOLETE
    shareEffect(df, 683, team_bonus_id); // MALAY_OBSOLETE_2
    shareEffect(df, 684, team_bonus_id); // MALAY_OBSOLETE_3
    shareEffect(df, 674, team_bonus_id); // MALAY_FASTER_AGEADVANCE
    shareEffect(df, 675, team_bonus_id); // MALAY_FISHTRAP_DISCOUNT
    shareEffect(df, 677, team_bonus_id); // MALAY_FISHTRAP_FOOD
    shareEffect(df, 695, team_bonus_id); // MALAY_ELE_DISCOUNT_1
    shareEffect(df, 696, team_bonus_id); // MALAY_ELE_DISCOUNT_2
    shareEffect(df, 649, team_bonus_id); // MALAY_DOCK_LOS
}

void maliansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 43;

    skipShare(596); // MALI_GHBETO
    skipShare(597); // MALI_GHBETO

    shareEffect(df, 42, team_bonus_id); // MALI_TECH_TREE
    shareEffect(df, 605, team_bonus_id); // MALI_TIGUI
    shareEffect(df, 606, team_bonus_id); // MALI_FARIMBA
    shareEffect(df, 165, team_bonus_id); // MALI_TC_WOOD_COST
    shareEffect(df, 617, team_bonus_id); // MALI_WOOD_DISCOUNT
    shareEffect(df, 618, team_bonus_id); // MALI_INF_ARMOR_1
    shareEffect(df, 619, team_bonus_id); // MALI_INF_ARMOR_2
    shareEffect(df, 620, team_bonus_id); // MALI_INF_ARMOR_3
    shareEffect(df, 621, team_bonus_id); // MALI_GOLDMINING
    shareEffect(df, 43, team_bonus_id); // MALI_UNIVERSITY_WORKRATE
}

void mayansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 489;

    skipShare(468); // MAY_PLUMED_ARCHER
    skipShare(469); // MAY_PLUMED_ARCHER

    shareEffect(df, 449, team_bonus_id); // MAY_TECH_TREE
    shareEffect(df, 515, team_bonus_id); // MAY_HULCHE_JAVELINEERS
    shareEffect(df, 456, team_bonus_id); // MAY_EL_DORADO
    shareEffect(df, 216, team_bonus_id); // MAY_START_RES
    shareEffect(df, 586, team_bonus_id); // MAY_VILS_SPAWN
    shareEffect(df, 485, team_bonus_id); // MAY_ARCHER_COST_1
    shareEffect(df, 486, team_bonus_id); // MAY_ARCHER_COST_2
    shareEffect(df, 771, team_bonus_id); // MAY_RESOURCES_LONGER_1
    shareEffect(df, 772, team_bonus_id); // MAY_RESOURCES_LONGER_2
    shareEffect(df, 773, team_bonus_id); // MAY_RESOURCES_LONGER_3
    shareEffect(df, 774, team_bonus_id); // MAY_RESOURCES_LONGER_4
    shareEffect(df, 775, team_bonus_id); // MAY_RESOURCES_LONGER_5
    shareEffect(df, 776, team_bonus_id); // MAY_RESOURCES_LONGER_6
    shareEffect(df, 777, team_bonus_id); // MAY_RESOURCES_LONGER_7
    shareEffect(df, 778, team_bonus_id); // MAY_RESOURCES_LONGER_8
    shareEffect(df, 779, team_bonus_id); // MAY_RESOURCES_LONGER_9
    shareEffect(df, 780, team_bonus_id); // MAY_RESOURCES_LONGER_10
    shareEffect(df, 781, team_bonus_id); // MAY_RESOURCES_LONGER_11
    shareEffect(df, 489, team_bonus_id); // MAY_WALL_DISCOUNT
}

void mongolsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 407;

    skipShare(270); // MONG_MANGUDAI
    skipShare(369); // MONG_MANGUDAI

    shareEffect(df, 277, team_bonus_id); // MONG_TECH_TREE
    shareEffect(df, 542, team_bonus_id); // MONG_NOMADS
    shareEffect(df, 681, team_bonus_id); // MONG_NOMADS_2
    shareEffect(df, 457, team_bonus_id); // MONG_DRILL
    shareEffect(df, 393, team_bonus_id); // MONG_CA_FIRING
    shareEffect(df, 387, team_bonus_id); // MONG_LIGHTCAV_HP
    shareEffect(df, 388, team_bonus_id); // MONG_HUNTERS
    shareEffect(df, 407, team_bonus_id); // MONG_SCOUT_LOS
}

void persiansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 408;

    skipShare(271); // PERS_WAR_ELEPHANT
    skipShare(365); // PERS_WAR_ELEPHANT

    shareEffect(df, 260, team_bonus_id); // PERS_TECH_TREE
    shareEffect(df, 458, team_bonus_id); // PERS_MAHOUTS
    shareEffect(df, 543, team_bonus_id); // PERS_KAMANDARAN
    shareEffect(df, 212, team_bonus_id); // PERS_START_RES
    shareEffect(df, 340, team_bonus_id); // PERS_TC_HP
    shareEffect(df, 347, team_bonus_id); // PERS_DOCK_HP
    shareEffect(df, 421, team_bonus_id); // PERS_TC_SPEED
    shareEffect(df, 424, team_bonus_id); // PERS_DOCK_SPEED
    shareEffect(df, 408, team_bonus_id); // PERS_KNIGHT_BONUS
}

void polesCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 802;

    skipShare(805); // POL_OBUCH
    skipShare(806); // POL_OBUCH

    shareEffect(df, 801, team_bonus_id); // POL_TECH_TREE
    shareEffect(df, 809, team_bonus_id); // POL_SZLACHTA_PRIVILEGES
    shareEffect(df, 810, team_bonus_id); // POL_LECHITIC_LEGACY
    shareEffect(df, 815, team_bonus_id); // POL_VIL_REGEN
    shareEffect(df, 816, team_bonus_id); // POL_FOLWARK
    shareEffect(df, 818, team_bonus_id); // POL_FOLWARK_2
    shareEffect(df, 819, team_bonus_id); // POL_FOLWARK_3
    shareEffect(df, 820, team_bonus_id); // POL_FOLWARK_4
    shareEffect(df, 821, team_bonus_id); // POL_FOLWARKFARM_FOOD_1
    shareEffect(df, 822, team_bonus_id); // POL_FOLWARKFARM_FOOD_2
    shareEffect(df, 823, team_bonus_id); // POL_FOLWARKFARM_FOOD_3
    shareEffect(df, 827, team_bonus_id); // POL_STONE_GOLDGEN_1
    shareEffect(df, 828, team_bonus_id); // POL_STONE_GOLDGEN_2
    shareEffect(df, 829, team_bonus_id); // POL_STONE_GOLDGEN_3
    shareEffect(df, 802, team_bonus_id); // POL_SCOUTS_BONUS
}

void portugueseCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 32;

    skipShare(591); // PORT_ORGAN_GUN
    skipShare(592); // PORT_ORGAN_GUN
    shareEffect(df, 622, team_bonus_id); // PORT_CARAVEL
    shareEffect(df, 623, team_bonus_id); // PORT_CARAVEL

    shareEffect(df, 31, team_bonus_id); // PORT_TECH_TREE
    shareEffect(df, 601, team_bonus_id); // PORT_CARRACK
    shareEffect(df, 602, team_bonus_id); // PORT_ARQUEBUS
    shareEffect(df, 35, team_bonus_id); // PORT_OBSOLETE
    shareEffect(df, 33, team_bonus_id); // PORT_GOLD_DISCOUNT
    shareEffect(df, 510, team_bonus_id); // PORT_FORAGERS
    shareEffect(df, 600, team_bonus_id); // PORT_FEITORIA_AVAIL
    shareEffect(df, 32, team_bonus_id); // PORT_RESEARCH_SPEED
}

void romansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 891;

    skipShare(892); // ROM_CENTURION
    skipShare(893); // ROM_CENTURION
    skipShare(903); // ROM_LEGIONARY
    shareEffect(df, 896, team_bonus_id); // ROM_LEGIONARY

    shareEffect(df, 890, team_bonus_id); // ROM_TECH_TREE
    shareEffect(df, 894, team_bonus_id); // ROM_BALLISTAS
    shareEffect(df, 895, team_bonus_id); // ROM_COMITATENSES
    shareEffect(df, 901, team_bonus_id); // ROM_SCORP_BALLISTICS
    shareEffect(df, 898, team_bonus_id); // ROM_VILS_WORK
    shareEffect(df, 899, team_bonus_id); // ROM_NAVY

    if (df->Techs.at(889).EffectID == SCALE_MAIL_ARMOR) {
        auto scale_mail_id = duplicateForShare(df, SCALE_MAIL_ARMOR);
        df->Techs.at(889).EffectID = scale_mail_id;
    }
    shareEffect(df, df->Techs.at(889).EffectID, team_bonus_id); // ROM_SCALE_MAIL_ARMOR

    if (df->Techs.at(890).EffectID == CHAIN_MAIL_ARMOR) {
        auto chain_mail_id = duplicateForShare(df, CHAIN_MAIL_ARMOR);
        df->Techs.at(890).EffectID = chain_mail_id;
    }
    shareEffect(df, df->Techs.at(890).EffectID, team_bonus_id); // ROM_CHAIN_MAIL_ARMOR

    if (df->Techs.at(891).EffectID == PLATE_MAIL_ARMOR) {
        auto plate_mail_id = duplicateForShare(df, PLATE_MAIL_ARMOR);
        df->Techs.at(891).EffectID = plate_mail_id;
    }
    shareEffect(df, df->Techs.at(891).EffectID, team_bonus_id); // ROM_PLATE_MAIL_ARMOR
    
    shareEffect(df, 900, team_bonus_id); // ROM_SCORP_DISCOUT
}

void saracensCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 409;

    skipShare(266); // SAR_MAMELUKE
    skipShare(366); // SAR_MAMELUKE
    
    shareEffect(df, 261, team_bonus_id); // SAR_TECH_TREE
    shareEffect(df, 459, team_bonus_id); // SAR_ZEALOTRY
    shareEffect(df, 480, team_bonus_id); // SAR_COUNTERWEIGHTS
    shareEffect(df, 545, team_bonus_id); // SAR_OBSOLETE
    shareEffect(df, 354, team_bonus_id); // SAR_MARKET
    shareEffect(df, 311, team_bonus_id); // SAR_TRANSPORT
    shareEffect(df, 416, team_bonus_id); // SAR_GALLEY_ATTACK
    shareEffect(df, 312, team_bonus_id); // SAR_CAMEL_HP
    shareEffect(df, 313, team_bonus_id); // SAR_ARCHER_BONUS_1
    shareEffect(df, 318, team_bonus_id); // SAR_ARCHER_BONUS_2
    shareEffect(df, 409, team_bonus_id); // SAR_ARCHER_BONUS
}

void siciliansCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 785;

    skipShare(788); // SIC_SERJEANT
    skipShare(789); // SIC_SERJEANT

    shareEffect(df, 784, team_bonus_id); // SIC_TECH_TREE
    shareEffect(df, 792, team_bonus_id); // SIC_FIRST_CRUSADE
    shareEffect(df, 793, team_bonus_id); // SIC_HAUBERK
    shareEffect(df, 217, team_bonus_id); // SIC_START_RES
    shareEffect(df, 795, team_bonus_id); // SIC_BUILD_SPEED
    shareEffect(df, 796, team_bonus_id); // SIC_BONUS_RESISTANCE
    shareEffect(df, 797, team_bonus_id); // SIC_FARM_FOOD_1
    shareEffect(df, 798, team_bonus_id); // SIC_FARM_FOOD_2
    shareEffect(df, 799, team_bonus_id); // SIC_FARM_FOOD_3
    shareEffect(df, 800, team_bonus_id); // SIC_DONJON_AVAIL
    shareEffect(df, 785, team_bonus_id); // SIC_TRANSPORT
}

void slavsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 9;

    skipShare(556); // SLAVS_BOYAR
    skipShare(557); // SLAVS_BOYAR

    shareEffect(df, 7, team_bonus_id); // SLAVS_TECH_TREE
    shareEffect(df, 481, team_bonus_id); // SLAVS_DETINETS
    shareEffect(df, 569, team_bonus_id); // SLAVS_DRUZHINA
    shareEffect(df, 690, team_bonus_id); // SLAVS_FARMS
    shareEffect(df, 567, team_bonus_id); // SLAVS_SIEGE_DISCOUNT
    shareEffect(df, 758, team_bonus_id); // SLAVS_MILITARY_POP
}

void spanishCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 490;

    skipShare(491); // SPANISH_CONQ
    skipShare(492); // SPANISH_CONQ

    shareEffect(df, 446, team_bonus_id); // SPAN_TECH_TREE
    shareEffect(df, 547, team_bonus_id); // SPAN_INQUISITION
    shareEffect(df, 495, team_bonus_id); // SPAN_SUPREMACY
    shareEffect(df, 168, team_bonus_id); // SPAN_TC_BUILD
    shareEffect(df, 300, team_bonus_id); // SPAN_TECH_REWARD
    shareEffect(df, 181, team_bonus_id); // SPAN_TECH_REWARD_2
    shareEffect(df, 237, team_bonus_id); // SPAN_TECH_REWARD_3
    shareEffect(df, 496, team_bonus_id); // SPAN_MISSIONARY_AVAIL
    shareEffect(df, 490, team_bonus_id); // SPAN_TRADE
}

void tatarsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 709;

    skipShare(716); // TAT_KESHIK
    skipShare(717); // TAT_KESHIK

    shareEffect(df, 708, team_bonus_id); // TAT_TECH_TREE
    shareEffect(df, 724, team_bonus_id); // TAT_SILK_ARMOR
    shareEffect(df, 725, team_bonus_id); // TAT_TIMURID
    shareEffect(df, 741, team_bonus_id); // TAT_SHEEP_BONUS
    shareEffect(df, 299, team_bonus_id); // TAT_SHEEP_BONUS_2
    shareEffect(df, 303, team_bonus_id); // TAT_SHEEP_BONUS_3
    shareEffect(df, 305, team_bonus_id); // TAT_SHEEP_BONUS_4
    shareEffect(df, 735, team_bonus_id); // TAT_PARTHIAN_TACTICS
    shareEffect(df, 733, team_bonus_id); // TAT_ELEVATION
    shareEffect(df, 709, team_bonus_id); // TAT_CA_LOS
}

void teutonsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 404;

    skipShare(273); // TEUT_TEUTONIC_KNIGHT
    skipShare(362); // TEUT_TEUTONIC_KNIGHT

    shareEffect(df, 262, team_bonus_id); // TEUT_TECH_TREE
    shareEffect(df, 544, team_bonus_id); // TEUT_IRONCLAD
    shareEffect(df, 461, team_bonus_id); // TEUT_CRENELLATIONS
    shareEffect(df, 332, team_bonus_id); // TEUT_OBSOLETE
    shareEffect(df, 345, team_bonus_id); // TEUT_HEAL_RANGE
    shareEffect(df, 352, team_bonus_id); // TEUT_TOWER_GARISON
    shareEffect(df, 314, team_bonus_id); // TEUT_FARMS_DISCOUNT
    shareEffect(df, 335, team_bonus_id); // TEUT_TC_IMPROVE
    shareEffect(df, 333, team_bonus_id); // TEUT_ARMOR_1
    shareEffect(df, 334, team_bonus_id); // TEUT_ARMOR_2
    shareEffect(df, 404, team_bonus_id); // TEUT_CONVERSION_RESISTANCE
}

void turksCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 410;

    skipShare(268); // TURK_JANNISARY
    skipShare(353); // TURK_CASTLE
    skipShare(367); // TURK_JANNISARY

    shareEffect(df, 263, team_bonus_id); // TURK_TECH_TREE
    shareEffect(df, 546, team_bonus_id); // TURK_SIPAHI
    shareEffect(df, 460, team_bonus_id); // TURK_ARTILLERY
    shareEffect(df, 527, team_bonus_id); // TURK_HUSSAR_FREE
    shareEffect(df, 301, team_bonus_id); // TURK_CHEMISTRY_FREE
    shareEffect(df, 296, team_bonus_id); // TURK_GUNPOWDER_HP
    shareEffect(df, 284, team_bonus_id); // TURK_GUNPOWDERSHIP_HP
    shareEffect(df, 295, team_bonus_id); // TURK_GOLD_MINING
    shareEffect(df, 509, team_bonus_id); // TURK_LIGHTCAV_PIERCE
    shareEffect(df, 410, team_bonus_id); // TURK_GUNPOWDER_TRAINTIME
}

void vietnameseCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 653;

    skipShare(660); // VIET_RATTAN
    skipShare(661); // VIET_RATTAN

    shareEffect(df, 652, team_bonus_id); // VIET_TECH_TREE
    shareEffect(df, 668, team_bonus_id); // VIET_CHATRAS
    shareEffect(df, 669, team_bonus_id); // VIET_PAPER_MONEY_1
    shareEffect(df, 881, team_bonus_id); // VIET_PAPER_MONEY_2
    shareEffect(df, 882, team_bonus_id); // VIET_PAPER_MONEY_3
    shareEffect(df, 883, team_bonus_id); // VIET_PAPER_MONEY_4
    shareEffect(df, 672, team_bonus_id); // VIET_ARCHERS_HP
    shareEffect(df, 698, team_bonus_id); // VIET_VISION
}

void vikingsCivBonus(genie::DatFile *df) {
    auto team_bonus_id = 411;

    shareEffect(df, 269, team_bonus_id); // VIK_LONGBOAT
    shareEffect(df, 370, team_bonus_id); // VIK_LONGBOAT
    skipShare(397); // VIK_BERSERK
    skipShare(398); // VIK_BERSERK

    shareEffect(df, 276, team_bonus_id); // VIK_TECH_TREE
    shareEffect(df, 517, team_bonus_id); // VIK_CHIEFTAINS
    shareEffect(df, 467, team_bonus_id); // VIK_BOGSVEIGAR
    shareEffect(df, 391, team_bonus_id); // VIK_WHEELBARROW_FREE
    shareEffect(df, 412, team_bonus_id); // VIK_HANDCART_FREE
    shareEffect(df, 383, team_bonus_id); // VIK_WARSHIP_COST_1
    shareEffect(df, 386, team_bonus_id); // VIK_WARSHIP_COST_2
    shareEffect(df, 394, team_bonus_id); // VIK_WARSHIP_COST_3
    shareEffect(df, 390, team_bonus_id); // VIK_INF_HP_1
    shareEffect(df, 427, team_bonus_id); // VIK_INF_HP_2
    shareEffect(df, 428, team_bonus_id); // VIK_INF_HP_3
    shareEffect(df, 411, team_bonus_id); // VIK_DOCK_COST
}

void configureSharedCivBonus(genie::DatFile *df) {
    aztecsCivBonus(df);
    bengalisCivBonus(df);
    berbersCivBonus(df);
    bohemiansCivBonus(df);
    britonsCivBonus(df);
    bulgariansCivBonus(df);
    burgundiansCivBonus(df);
    burmeseCivBonus(df);
    byzantinesCivBonus(df);
    celtsCivBonus(df);
    chineseCivBonus(df);
    cumansCivBonus(df);
    dravidiansCivBonus(df);
    ethiopiansCivBonus(df);
    franksCivBonus(df);
    gothsCivBonus(df);
    gurjarasCivBonus(df);
    hunsCivBonus(df);
    hindustanisCivBonus(df);
    incasCivBonus(df);
    italiansCivBonus(df);
    japaneseCivBonus(df);
    khmerCivBonus(df);
    koreansCivBonus(df);
    lithuaniansCivBonus(df);
    magyarsCivBonus(df);
    malayCivBonus(df);
    maliansCivBonus(df);
    mayansCivBonus(df);
    mongolsCivBonus(df);
    persiansCivBonus(df);
    polesCivBonus(df);
    portugueseCivBonus(df);
    romansCivBonus(df);
    saracensCivBonus(df);
    siciliansCivBonus(df);
    slavsCivBonus(df);
    spanishCivBonus(df);
    tatarsCivBonus(df);
    teutonsCivBonus(df);
    turksCivBonus(df);
    vietnameseCivBonus(df);
    vikingsCivBonus(df);

    auto techs_count = df->Techs.size();
    for (auto i = 0; i < techs_count; i++) {
        auto &tech = df->Techs.at(i);
        auto was_effect_updated = shared_effects.find(tech.EffectID) != shared_effects.end();
        
        if (tech.Civ > 0 && !was_effect_updated && tech.EffectID != -1) {
            cout << "Effect " << tech.EffectID << ":" << df->Effects.at(tech.EffectID).Name << " should be shared for civ: " << df->Civs.at(tech.Civ).Name << " tech: " << tech.Name << " but was not.\n";
        } else if (tech.Civ == 0 && was_effect_updated && tech.EffectID != -1) {
            cout << "Effect " << tech.EffectID << ":" << df->Effects.at(tech.EffectID).Name << " should not be shared for civ: " << df->Civs.at(tech.Civ).Name << " tech: " << tech.Name << " but was.\n";
        }

        if (tech.Civ > 0 && tech.EffectID == -1) {
            shareTech(df, i);
        }
    }
}