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
    skipShare(346);
    skipShare(442);
    skipShare(444);

    auto team_bonus_id = 488;

    const int AZT_TECH_TREE = 447;
    shareEffect(df, AZT_TECH_TREE, team_bonus_id);
    
    const int AZT_ATLATL = 514;
    shareEffect(df, AZT_ATLATL, team_bonus_id);
    
    const int AZT_GARLAND_WARS = 465;
    shareEffect(df, AZT_GARLAND_WARS, team_bonus_id);
    
    const int AZT_MONK_HP_1 = 470;
    shareEffect(df, AZT_MONK_HP_1, team_bonus_id);
    
    const int AZT_MONK_HP_2 = 471;
    shareEffect(df, AZT_MONK_HP_2, team_bonus_id);
    
    const int AZT_MONK_HP_3 = 472;
    shareEffect(df, AZT_MONK_HP_3, team_bonus_id);
    
    const int AZT_MONK_HP_4 = 473;
    shareEffect(df, AZT_MONK_HP_4, team_bonus_id);
    
    const int AZT_MONK_HP_5 = 474;
    shareEffect(df, AZT_MONK_HP_5, team_bonus_id);
    
    const int AZT_MONK_HP_6 = 475;
    shareEffect(df, AZT_MONK_HP_6, team_bonus_id);
    
    const int AZT_MONK_HP_7 = 476;
    shareEffect(df, AZT_MONK_HP_7, team_bonus_id);
    
    const int AZT_MONK_HP_8 = 477;
    shareEffect(df, AZT_MONK_HP_8, team_bonus_id);
    
    const int AZT_MONK_HP_9 = 478;
    shareEffect(df, AZT_MONK_HP_9, team_bonus_id);
    
    const int AZT_MONK_HP_10 = 512;
    shareEffect(df, AZT_MONK_HP_10, team_bonus_id);

    const int AZT_START_RES = 213;
    shareEffect(df, AZT_START_RES, team_bonus_id);
    
    const int AZT_RELIC = 488;
    shareEffect(df, AZT_RELIC, team_bonus_id);
}

void bengalisCivBonus(genie::DatFile *df) {
    skipShare(846);
    skipShare(847);

    auto team_bonus_id = 841;

    const int BENG_TECH_TREE = 840;
    shareEffect(df, BENG_TECH_TREE, team_bonus_id);

    const int BENG_PAIKS = 852;
    shareEffect(df, BENG_PAIKS, team_bonus_id);

    const int BENG_MAHAYANA = 853;
    shareEffect(df, BENG_MAHAYANA, team_bonus_id);

    const int BENG_RESIST = 865;
    shareEffect(df, BENG_RESIST, team_bonus_id);

    const int BENG_VILS = 866;
    shareEffect(df, BENG_VILS, team_bonus_id);

    const int BENG_CAV_BONUS = 888;
    shareEffect(df, BENG_CAV_BONUS, team_bonus_id);

    const int BENG_SHIPS_REGENERATE = 867;
    shareEffect(df, BENG_SHIPS_REGENERATE, team_bonus_id);

    const int BENG_MONK_DEFENSE = 568;
    shareEffect(df, BENG_MONK_DEFENSE, team_bonus_id);

    const int BENG_TRADE = 841;
    shareEffect(df, BENG_TRADE, team_bonus_id);
}

void berbersCivBonus(genie::DatFile *df) {
    skipShare(594);
    skipShare(595);
    skipShare(624);

    auto team_bonus_id = 38;

    const int BERB_TECH_TREE = 37;
    shareEffect(df, BERB_TECH_TREE, team_bonus_id);

    const int BERB_KASBAH = 607;
    shareEffect(df, BERB_KASBAH, team_bonus_id);

    const int BERB_MAGHREBI_CAMELS = 608;
    shareEffect(df, BERB_MAGHREBI_CAMELS, team_bonus_id);

    const int BERB_VILLAGERS_FASTER_1 = 609;
    shareEffect(df, BERB_VILLAGERS_FASTER_1, team_bonus_id);

    const int BERB_VILLAGERS_FASTER_2 = 626;
    shareEffect(df, BERB_VILLAGERS_FASTER_2, team_bonus_id);

    const int BERB_CAVALRY_DISCOUNT_1 = 610;
    shareEffect(df, BERB_CAVALRY_DISCOUNT_1, team_bonus_id);

    const int BERB_CAVALRY_DISCOUNT_2 = 638;
    shareEffect(df, BERB_CAVALRY_DISCOUNT_2, team_bonus_id);

    const int BERB_SHIPS_FASTER_1 = 34;
    shareEffect(df, BERB_SHIPS_FASTER_1, team_bonus_id);

    const int BERB_SHIPS_FASTER_2 = 611;
    shareEffect(df, BERB_SHIPS_FASTER_2, team_bonus_id);
}

void bohemiansCivBonus(genie::DatFile *df) {
    skipShare(807);
    skipShare(808);
    skipShare(814);

    auto team_bonus_id = 804;

    const int BOH_TECH_TREE = 803;
    shareEffect(df, BOH_TECH_TREE, team_bonus_id);

    const int BOH_WAGENBURG_TACTICS = 811;
    shareEffect(df, BOH_WAGENBURG_TACTICS, team_bonus_id);

    const int BOH_HUSSITE_REFORMS = 812;
    shareEffect(df, BOH_HUSSITE_REFORMS, team_bonus_id);

    const int BOH_BLACKSMITH_DISCOUNT = 830;
    shareEffect(df, BOH_BLACKSMITH_DISCOUNT, team_bonus_id);

    const int BOH_SPEAR_BONUS = 824;
    shareEffect(df, BOH_SPEAR_BONUS, team_bonus_id);

    const int BOH_VIL_SANCTITY = 825;
    shareEffect(df, BOH_VIL_SANCTITY, team_bonus_id);

    const int BOH_VIL_FERVOR = 826;
    shareEffect(df, BOH_VIL_FERVOR, team_bonus_id);

    const int BOH_MARKET_SPEED = 804;
    shareEffect(df, BOH_MARKET_SPEED, team_bonus_id);
}

void britonsCivBonus(genie::DatFile *df) {
    skipShare(251);
    skipShare(358);
    
    auto team_bonus_id = 399;

    const int BRIT_TECH_TREE = 254;
    shareEffect(df, BRIT_TECH_TREE, team_bonus_id);
    
    const int BRIT_YEOMEN = 455;
    shareEffect(df, BRIT_YEOMEN, team_bonus_id);
    
    const int BRIT_WARWOLF = 540;
    shareEffect(df, BRIT_WARWOLF    , team_bonus_id);
    
    const int BRIT_TC_DISCOUNT = 379;
    shareEffect(df, BRIT_TC_DISCOUNT, team_bonus_id);

    const int BRIT_ARCHER_RANGE_1 = 380;
    shareEffect(df, BRIT_ARCHER_RANGE_1, team_bonus_id);

    const int BRIT_ARCHER_RANGE_2 = 415;
    shareEffect(df, BRIT_ARCHER_RANGE_2, team_bonus_id);

    const int BRIT_SHEPARD = 381;
    shareEffect(df, BRIT_SHEPARD, team_bonus_id);

    const int BRIT_RANGE_SPEED = 399;
    shareEffect(df, BRIT_RANGE_SPEED, team_bonus_id);
}

void bulgariansCivBonus(genie::DatFile *df) {
    skipShare(714);
    skipShare(715);
    
    auto team_bonus_id = 707;

    const int BULG_TECH_TREE = 706;
    shareEffect(df, BULG_TECH_TREE, team_bonus_id);

    const int BULG_STIRRUPS = 722;
    shareEffect(df, BULG_STIRRUPS, team_bonus_id);

    const int BULG_BAGAINS = 723;
    shareEffect(df, BULG_BAGAINS, team_bonus_id);

    const int BULG_MILITIA_UPGRADES = 730;
    shareEffect(df, BULG_MILITIA_UPGRADES, team_bonus_id);

    const int BULG_TC_STONE_DISCOUNT_1 = 171;
    shareEffect(df, BULG_TC_STONE_DISCOUNT_1, team_bonus_id);
    
    const int BULG_TC_STONE_DISCOUNT_2 = 731;
    shareEffect(df, BULG_TC_STONE_DISCOUNT_2, team_bonus_id);

    const int BULG_BLACKSMITH_FASTER = 707;
    shareEffect(df, BULG_BLACKSMITH_FASTER, team_bonus_id);

    const int BULD_ENABLE_CREPOST = 732;
    shareEffect(df, BULD_ENABLE_CREPOST, team_bonus_id);
}

void burgundiansCivBonus(genie::DatFile *df) {
    skipShare(786);
    skipShare(787);
    
    auto team_bonus_id = 783;

    const int BURG_TECH_TREE = 782;
    shareEffect(df, BURG_TECH_TREE, team_bonus_id);

    const int BURG_BURGUNDIAN_VINEYARDS = 790;
    shareEffect(df, BURG_BURGUNDIAN_VINEYARDS, team_bonus_id);

    const int BURG_FLEMISH_REVOLUTION = 791;
    shareEffect(df, BURG_FLEMISH_REVOLUTION, team_bonus_id);

    const int BURG_GUNPOWDER = 794;
    shareEffect(df, BURG_GUNPOWDER, team_bonus_id);

    const int BURG_RELIC_FOOD = 783;
    shareEffect(df, BURG_RELIC_FOOD, team_bonus_id);
}

void burmeseCivBonus(genie::DatFile *df) {
    skipShare(658);
    skipShare(659);
    
    auto team_bonus_id = 651;

    const int BURM_TECH_TREE = 650;
    shareEffect(df, BURM_TECH_TREE, team_bonus_id);

    const int BURM_HOWDAH = 666;
    shareEffect(df, BURM_HOWDAH, team_bonus_id);

    const int BURM_MANIPUR_CAVALRY = 667;
    shareEffect(df, BURM_MANIPUR_CAVALRY, team_bonus_id);

    const int BURM_LUMBER_UPGRADES = 685;
    shareEffect(df, BURM_LUMBER_UPGRADES, team_bonus_id);

    const int BURM_INF_ATTACK_1 = 686;
    shareEffect(df, BURM_INF_ATTACK_1, team_bonus_id);

    const int BURM_INF_ATTACK_2 = 687;
    shareEffect(df, BURM_INF_ATTACK_2, team_bonus_id);

    const int BURM_INF_ATTACK_3 = 688;
    shareEffect(df, BURM_INF_ATTACK_3, team_bonus_id);

    const int BURM_INF_ATTACK_4 = 699;
    shareEffect(df, BURM_INF_ATTACK_4, team_bonus_id);

    const int BURM_ELEP_ARMOR = 679;
    shareEffect(df, BURM_ELEP_ARMOR, team_bonus_id);

    const int BURM_MONASTERY_DISCOUNT = 689;
    shareEffect(df, BURM_MONASTERY_DISCOUNT, team_bonus_id);
}

void byzantinesCivBonus(genie::DatFile *df) {
    skipShare(264);
    skipShare(359);
    
    auto team_bonus_id = 400;

    const int BYZ_TECH_TREE = 256;
    shareEffect(df, BYZ_TECH_TREE, team_bonus_id);

    const int BYZ_GREEK_FIRE = 518;
    shareEffect(df, BYZ_GREEK_FIRE, team_bonus_id);

    const int BYZ_LOGISTICA = 493;
    shareEffect(df, BYZ_LOGISTICA, team_bonus_id);

    const int BYZ_OBSOLETE = 413;
    shareEffect(df, BYZ_OBSOLETE, team_bonus_id);

    const int BYZ_BUILDINGS_HP_1 = 282;
    shareEffect(df, BYZ_BUILDINGS_HP_1, team_bonus_id);

    const int BYZ_BUILDINGS_HP_2 = 429;
    shareEffect(df, BYZ_BUILDINGS_HP_2, team_bonus_id);

    const int BYZ_BUILDINGS_HP_3 = 430;
    shareEffect(df, BYZ_BUILDINGS_HP_3, team_bonus_id);

    const int BYZ_BUILDINGS_HP_4 = 431;
    shareEffect(df, BYZ_BUILDINGS_HP_4, team_bonus_id);

    const int BYZ_COUNTER_DISCOUNT = 283;
    shareEffect(df, BYZ_COUNTER_DISCOUNT, team_bonus_id);

    const int BYZ_SHIP_ATTACKSPEED = 396;
    shareEffect(df, BYZ_SHIP_ATTACKSPEED, team_bonus_id);

    const int BYZ_HEALSPEED = 400;
    shareEffect(df, BYZ_HEALSPEED, team_bonus_id);
}

void celtsCivBonus(genie::DatFile *df) {
    skipShare(274);
    skipShare(368);
    
    auto team_bonus_id = 401;

    const int CELT_TECH_TREE = 275;
    shareEffect(df, CELT_TECH_TREE, team_bonus_id);

    const int CELT_STRONGHOLD = 537;
    shareEffect(df, CELT_STRONGHOLD, team_bonus_id);

    const int CELT_FUROR_CELTICA = 239;
    shareEffect(df, CELT_FUROR_CELTICA, team_bonus_id);

    const int CELT_INF_SPEED = 392;
    shareEffect(df, CELT_INF_SPEED, team_bonus_id);

    const int CELT_LUMBERJACK = 384;
    shareEffect(df, CELT_LUMBERJACK, team_bonus_id);

    const int CELT_SIEGE_FIRING = 385;
    shareEffect(df, CELT_SIEGE_FIRING, team_bonus_id);

    const int CELT_DOMINANT_LOS = 417;
    shareEffect(df, CELT_DOMINANT_LOS, team_bonus_id);

    const int CELTS_SIEGEWORKSHOP_SPEED = 401;
    shareEffect(df, CELTS_SIEGEWORKSHOP_SPEED, team_bonus_id);
}

void chineseCivBonus(genie::DatFile *df) {
    skipShare(265);
    skipShare(360);
    
    auto team_bonus_id = 402;

    const int CHIN_TECH_TREE = 257;
    shareEffect(df, CHIN_TECH_TREE, team_bonus_id);

    const int CHIN_GREAT_WALL = 516;
    shareEffect(df, CHIN_GREAT_WALL, team_bonus_id);

    const int CHIN_ROCKETRY = 483;
    shareEffect(df, CHIN_ROCKETRY, team_bonus_id);

    const int CHIN_START_RES = 215;
    shareEffect(df, CHIN_START_RES, team_bonus_id);

    const int CHIN_ADDITIONAL_VILS = 302;
    shareEffect(df, CHIN_ADDITIONAL_VILS, team_bonus_id);

    const int CHIN_TECH_DISCOUNT_1 = 304;
    shareEffect(df, CHIN_TECH_DISCOUNT_1, team_bonus_id);

    const int CHIN_TECH_DISCOUNT_2 = 349;
    shareEffect(df, CHIN_TECH_DISCOUNT_2, team_bonus_id);

    const int CHIN_TECH_DISCOUNT_3 = 350;
    shareEffect(df, CHIN_TECH_DISCOUNT_3, team_bonus_id);

    const int CHIN_TECH_DISCOUNT_4 = 351;
    shareEffect(df, CHIN_TECH_DISCOUNT_4, team_bonus_id);

    const int CHIN_TC_POP = 437;
    shareEffect(df, CHIN_TC_POP, team_bonus_id);

    const int CHIN_DEMO_HP = 395;
    shareEffect(df, CHIN_DEMO_HP, team_bonus_id);

    const int CHIN_FARM_FOOD_1 = 402;
    shareEffect(df, CHIN_FARM_FOOD_1, team_bonus_id);

    const int CHIN_FARM_FOOD_2 = 831;
    shareEffect(df, CHIN_FARM_FOOD_2, team_bonus_id);

    const int CHIN_FARM_FOOD_3 = 832;
    shareEffect(df, CHIN_FARM_FOOD_3, team_bonus_id);

    const int CHIN_FARM_FOOD_4 = 833;
    shareEffect(df, CHIN_FARM_FOOD_4, team_bonus_id);

    const int CHIN_FARM_FOOD_5 = 834;
    shareEffect(df, CHIN_FARM_FOOD_5, team_bonus_id);

    const int CHIN_FARM_FOOD_6 = 835;
    shareEffect(df, CHIN_FARM_FOOD_6, team_bonus_id);

    const int CHIN_FARM_FOOD_7 = 836;
    shareEffect(df, CHIN_FARM_FOOD_7, team_bonus_id);

    const int CHIN_FARM_FOOD_8 = 837;
    shareEffect(df, CHIN_FARM_FOOD_8, team_bonus_id);
}

void cumansCivBonus(genie::DatFile *df) {
    skipShare(718);
    skipShare(719);
    skipShare(759);
    skipShare(760); 
    
    auto team_bonus_id = 711;

    const int CUMANS_TECH_TREE = 710;
    shareEffect(df, CUMANS_TECH_TREE, team_bonus_id);

    const int CUMANS_STEPPE_HUSBANDRY = 726;
    shareEffect(df, CUMANS_STEPPE_HUSBANDRY, team_bonus_id);

    const int CUMANS_CUMAN_MERCENARIES = 743;
    shareEffect(df, CUMANS_CUMAN_MERCENARIES, team_bonus_id);

    const int CUMANS_CUMAN_MERCENARIES_2 = 727;
    shareEffect(df, CUMANS_CUMAN_MERCENARIES_2, team_bonus_id);

    const int CUMANS_TC = 761;
    shareEffect(df, CUMANS_TC, team_bonus_id);

    const int CUMANS_SIEGE = 742;
    shareEffect(df, CUMANS_SIEGE, team_bonus_id);

    const int CUMANS_WOOD_DISCOUNT = 697;
    shareEffect(df, CUMANS_WOOD_DISCOUNT, team_bonus_id);

    const int CUMANS_CAV_FASTER_1 = 746;
    shareEffect(df, CUMANS_CAV_FASTER_1, team_bonus_id);

    const int CUMANS_CAV_FASTER_2 = 748;
    shareEffect(df, CUMANS_CAV_FASTER_2, team_bonus_id);

    const int CUMANS_CAV_FASTER_3 = 762;
    shareEffect(df, CUMANS_CAV_FASTER_3, team_bonus_id);

    const int CUMANS_CAV_FASTER_4 = 763;
    shareEffect(df, CUMANS_CAV_FASTER_4, team_bonus_id);

    const int CUMANS_PALISADE_HP = 711;
    shareEffect(df, CUMANS_PALISADE_HP, team_bonus_id);
}

void dravidiansCivBonus(genie::DatFile *df) {
    skipShare(844);
    skipShare(845);
    
    auto team_bonus_id = 839;

    const int DRAV_TECH_TREE = 838;
    shareEffect(df, DRAV_TECH_TREE, team_bonus_id);

    const int DRAV_MEDICAL_CORPS = 850;
    shareEffect(df, DRAV_MEDICAL_CORPS, team_bonus_id);

    const int DRAV_WOOTZ_STEEL = 851;
    shareEffect(df, DRAV_WOOTZ_STEEL, team_bonus_id);

    const int DRAV_THIRISADAI = 860;
    shareEffect(df, DRAV_THIRISADAI, team_bonus_id);

    const int DRAV_BONUS_WOOD = 868;
    shareEffect(df, DRAV_BONUS_WOOD, team_bonus_id);

    const int DRAV_FISHING_CARRY = 863;
    shareEffect(df, DRAV_FISHING_CARRY, team_bonus_id);

    const int DRAV_SIEGE_DISCOUNT = 887;
    shareEffect(df, DRAV_SIEGE_DISCOUNT, team_bonus_id);

    const int DRAV_SKIRM_ATTACKFASTER = 864;
    shareEffect(df, DRAV_SKIRM_ATTACKFASTER, team_bonus_id);

    const int DRAV_DOCK_POP = 839;
    shareEffect(df, DRAV_DOCK_POP, team_bonus_id);
}

void ethiopiansCivBonus(genie::DatFile *df) {
    skipShare(598);
    skipShare(599);
    
    auto team_bonus_id = 49;

    const int ETH_TECH_TREE = 48;
    shareEffect(df, ETH_TECH_TREE, team_bonus_id);

    const int ETH_ROYAL_HEIRS = 603;
    shareEffect(df, ETH_ROYAL_HEIRS, team_bonus_id);

    const int ETH_TORSION_ENGINES = 604;
    shareEffect(df, ETH_TORSION_ENGINES, team_bonus_id);

    const int ETH_TORSION_ENGINES_2 = 634;
    shareEffect(df, ETH_TORSION_ENGINES_2, team_bonus_id);

    const int ETH_ACHER_FIREFASTER = 612;
    shareEffect(df, ETH_ACHER_FIREFASTER, team_bonus_id);

    const int ETH_BONUS_RES_1 = 613;
    shareEffect(df, ETH_BONUS_RES_1, team_bonus_id);

    const int ETH_BONUS_RES_2 = 614;
    shareEffect(df, ETH_BONUS_RES_2, team_bonus_id);

    const int ETH_BONUS_RES_3 = 615;
    shareEffect(df, ETH_BONUS_RES_3, team_bonus_id);

    const int ETH_PAIK_FREE = 616;
    shareEffect(df, ETH_PAIK_FREE, team_bonus_id);

    const int ETH_OUTPOST_LOS = 49;
    shareEffect(df, ETH_OUTPOST_LOS, team_bonus_id);
}

void franksCivBonus(genie::DatFile *df) {
    skipShare(272);
    skipShare(361);
    
    auto team_bonus_id = 403;

    const int FRANK_TECH_TREE = 258;
    shareEffect(df, FRANK_TECH_TREE, team_bonus_id);

    const int FRANK_BEARDED_AXE = 291;
    shareEffect(df, FRANK_BEARDED_AXE, team_bonus_id);

    const int FRANK_CHIVALRY = 548;
    shareEffect(df, FRANK_CHIVALRY, team_bonus_id);

    const int FRANK_CASTLE_DISCOUNT = 324;
    shareEffect(df, FRANK_CASTLE_DISCOUNT, team_bonus_id);

    const int FRANK_CAVALRY_HP = 285;
    shareEffect(df, FRANK_CAVALRY_HP, team_bonus_id);

    const int FRANK_FORAGERS = 523;
    shareEffect(df, FRANK_FORAGERS, team_bonus_id);

    const int FRANK_KNIGHT_LOS = 403;
    shareEffect(df, FRANK_KNIGHT_LOS, team_bonus_id);
}

void gothsCivBonus(genie::DatFile *df) {
    skipShare(363);
    skipShare(507);
    
    auto team_bonus_id = 405;

    const int GOTH_TECH_TREE = 259;
    shareEffect(df, GOTH_TECH_TREE, team_bonus_id);

    const int GOTH_ANARCHY = 462;
    shareEffect(df, GOTH_ANARCHY, team_bonus_id);

    const int GOTH_PERFUSION = 513;
    shareEffect(df, GOTH_PERFUSION, team_bonus_id);
    
    const int GOTH_INF_DISCOUNT_1 = 342;
    shareEffect(df, GOTH_INF_DISCOUNT_1, team_bonus_id);
    
    const int GOTH_INF_DISCOUNT_2 = 765;
    shareEffect(df, GOTH_INF_DISCOUNT_2, team_bonus_id);
    
    const int GOTH_INF_DISCOUNT_3 = 766;
    shareEffect(df, GOTH_INF_DISCOUNT_3, team_bonus_id);
    
    const int GOTH_INF_DISCOUNT_4 = 767;
    shareEffect(df, GOTH_INF_DISCOUNT_4, team_bonus_id);
    
    const int GOTH_INF_DISCOUNT_5 = 344;
    shareEffect(df, GOTH_INF_DISCOUNT_5, team_bonus_id);
    
    const int GOTH_INF_DISCOUNT_6 = 343;
    shareEffect(df, GOTH_INF_DISCOUNT_6, team_bonus_id);

    const int GOTH_INF_BONUS_1 = 326;
    shareEffect(df, GOTH_INF_BONUS_1, team_bonus_id);

    const int GOTH_INF_BONUS_2 = 327;
    shareEffect(df, GOTH_INF_BONUS_2, team_bonus_id);
    
    const int GOTH_INF_BONUS_3 = 328;
    shareEffect(df, GOTH_INF_BONUS_3, team_bonus_id);

    const int GOTH_HUNT = 414;
    shareEffect(df, GOTH_HUNT, team_bonus_id);

    const int GOTH_LOOM_INSTANT = 341;
    shareEffect(df, GOTH_LOOM_INSTANT, team_bonus_id);
    
    const int GOTH_POP = 418;
    shareEffect(df, GOTH_POP, team_bonus_id);
    
    const int GOTH_BARRACKS_WORK = 405;
    shareEffect(df, GOTH_BARRACKS_WORK, team_bonus_id);
}

void gurjarasCivBonus(genie::DatFile *df) {
    skipShare(848);
    skipShare(849);
    skipShare(861);
    skipShare(862);
    
    auto team_bonus_id = 843;

    const int GUR_TECH_TREE = 842;
    shareEffect(df, GUR_TECH_TREE, team_bonus_id);

    const int GUR_KSHATRIYAS = 854;
    shareEffect(df, GUR_KSHATRIYAS, team_bonus_id);
    
    const int GUR_KSHATRIYAS_SUP = 875;
    shareEffect(df, GUR_KSHATRIYAS_SUP, team_bonus_id);

    const int GUR_FRONTIER_GUARDS = 855;
    shareEffect(df, GUR_FRONTIER_GUARDS, team_bonus_id);

    const int GUR_FORAGEBUSHES = 872;
    shareEffect(df, GUR_FORAGEBUSHES, team_bonus_id);

    const int GUR_MILL_GARISON = 871;
    shareEffect(df, GUR_MILL_GARISON, team_bonus_id);
    
    const int GUR_CAV_BONUS_1 = 869;
    shareEffect(df, GUR_CAV_BONUS_1, team_bonus_id);
    
    const int GUR_CAV_BONUS_2 = 873;
    shareEffect(df, GUR_CAV_BONUS_2, team_bonus_id);
    
    const int GUR_CAV_BONUS_3 = 874;
    shareEffect(df, GUR_CAV_BONUS_3, team_bonus_id);
    
    const int GUR_CAV_BONUS_4 = 884;
    shareEffect(df, GUR_CAV_BONUS_4, team_bonus_id);
    
    const int GUR_DOCK_GARRISON = 870;
    shareEffect(df, GUR_DOCK_GARRISON, team_bonus_id);

    const int GUR_CAMEL_TRAIN = 843;
    shareEffect(df, GUR_CAMEL_TRAIN, team_bonus_id);
}

void hindustanisCivBonus(genie::DatFile *df) {
    skipShare(858);
    skipShare(859);
    
    auto team_bonus_id = 2;

    const int HIND_TECH_TREE = 1;
    shareEffect(df, HIND_TECH_TREE, team_bonus_id);

    const int HIND_IMP_CAMEL = 577;
    shareEffect(df, HIND_IMP_CAMEL, team_bonus_id);

    const int HIND_GRAND_TRUNK_ROAD = 562;
    shareEffect(df, HIND_GRAND_TRUNK_ROAD, team_bonus_id);

    const int HIND_SHATAGNI = 563;
    shareEffect(df, HIND_SHATAGNI, team_bonus_id);
    
    const int HIND_VIL_DISCOUNT_1 = 551;
    shareEffect(df, HIND_VIL_DISCOUNT_1, team_bonus_id);
    
    const int HIND_VIL_DISCOUNT_2 = 552;
    shareEffect(df, HIND_VIL_DISCOUNT_2, team_bonus_id);
    
    const int HIND_VIL_DISCOUNT_3 = 553;
    shareEffect(df, HIND_VIL_DISCOUNT_3, team_bonus_id);
    
    const int HIND_VIL_DISCOUNT_4 = 585;
    shareEffect(df, HIND_VIL_DISCOUNT_4, team_bonus_id);

    const int HIND_CAMEL_ATTACKSPEED = 560;
    shareEffect(df, HIND_CAMEL_ATTACKSPEED, team_bonus_id);

    const int HIND_GUNPOWDER_ARMOR = 576;
    shareEffect(df, HIND_GUNPOWDER_ARMOR, team_bonus_id);

    const int HIND_CARAVENSEI_AVAIL = 574;
    shareEffect(df, HIND_CARAVENSEI_AVAIL, team_bonus_id);

    const int HIND_CAV_ATTACK = 2;
    shareEffect(df, HIND_CAV_ATTACK, team_bonus_id);
}

void hunsCivBonus(genie::DatFile *df) {
    skipShare(453);
    skipShare(454);
    
    auto team_bonus_id = 484;

    const int HUNS_TECH_TREE = 448;
    shareEffect(df, HUNS_TECH_TREE, team_bonus_id);

    const int HUNS_MARAUERS = 538;
    shareEffect(df, HUNS_MARAUERS, team_bonus_id);

    const int HUNS_ATHEISM = 464;
    shareEffect(df, HUNS_ATHEISM, team_bonus_id);

    const int HUNS_START_RES = 214;
    shareEffect(df, HUNS_START_RES, team_bonus_id);

    const int HUNS_CA_DISCOUNT_1 = 497;
    shareEffect(df, HUNS_CA_DISCOUNT_1, team_bonus_id);

    const int HUNS_CA_DISCOUNT_2 = 498;
    shareEffect(df, HUNS_CA_DISCOUNT_2, team_bonus_id);

    const int HUNS_FASTER_STABLES = 484;
    shareEffect(df, HUNS_FASTER_STABLES, team_bonus_id);
}

void incasCivBonus(genie::DatFile *df) {
    skipShare(40);
    skipShare(564);
    skipShare(565);
    skipShare(582);
    
    auto team_bonus_id = 4;

    const int INC_TECH_TREE = 3;
    shareEffect(df, INC_TECH_TREE, team_bonus_id);

    const int INC_ANDEAN_SLING = 572;
    shareEffect(df, INC_ANDEAN_SLING, team_bonus_id);

    const int INC_FABRIC_SHIELDS = 573;
    shareEffect(df, INC_FABRIC_SHIELDS, team_bonus_id);

    const int INC_FOOD_DISCOUNT_1 = 156;
    shareEffect(df, INC_FOOD_DISCOUNT_1, team_bonus_id);
    
    const int INC_FOOD_DISCOUNT_2 = 158;
    shareEffect(df, INC_FOOD_DISCOUNT_2, team_bonus_id);
    
    const int INC_FOOD_DISCOUNT_3 = 159;
    shareEffect(df, INC_FOOD_DISCOUNT_3, team_bonus_id);
    
    const int INC_FOOD_DISCOUNT_4 = 160;
    shareEffect(df, INC_FOOD_DISCOUNT_4, team_bonus_id);
    
    const int INC_VIL_BLACKSMITH_1 = 529;
    shareEffect(df, INC_VIL_BLACKSMITH_1, team_bonus_id);
    
    const int INC_VIL_BLACKSMITH_2 = 530;
    shareEffect(df, INC_VIL_BLACKSMITH_2, team_bonus_id);
    
    const int INC_VIL_BLACKSMITH_3 = 531;
    shareEffect(df, INC_VIL_BLACKSMITH_3, team_bonus_id);
    
    const int INC_VIL_BLACKSMITH_4 = 532;
    shareEffect(df, INC_VIL_BLACKSMITH_4, team_bonus_id);
    
    const int INC_VIL_BLACKSMITH_5 = 533;
    shareEffect(df, INC_VIL_BLACKSMITH_5, team_bonus_id);
    
    const int INC_VIL_BLACKSMITH_6 = 534;
    shareEffect(df, INC_VIL_BLACKSMITH_6, team_bonus_id);
    
    const int INC_HOUSES_POP = 550;
    shareEffect(df, INC_HOUSES_POP, team_bonus_id);
    
    const int INC_STONE_DISCOUNT = 575;
    shareEffect(df, INC_STONE_DISCOUNT, team_bonus_id);
    
    const int INC_SPAWN_LAMA = 678;
    shareEffect(df, INC_SPAWN_LAMA    , team_bonus_id);
}

void italiansCivBonus(genie::DatFile *df) {
    skipShare(519);
    skipShare(520);
    
    auto team_bonus_id = 11;

    const int ITAL_TECH_TREE = 10;
    shareEffect(df, ITAL_TECH_TREE, team_bonus_id);

    const int ITAL_PAVISE = 549;
    shareEffect(df, ITAL_PAVISE, team_bonus_id);
    
    const int ITAL_SILK_ROAD = 554;
    shareEffect(df, ITAL_SILK_ROAD, team_bonus_id);

    const int ITAL_GUNPOWDER_DISCOUNT = 555;
    shareEffect(df, ITAL_GUNPOWDER_DISCOUNT, team_bonus_id);
}

void japaneseCivBonus(genie::DatFile *df) {
    skipShare(250);
    skipShare(364);
    
    auto team_bonus_id = 406;

    const int JAP_TECH_TREE = 255;
    shareEffect(df, JAP_TECH_TREE, team_bonus_id);

    const int JAP_YASAMA = 539;
    shareEffect(df, JAP_YASAMA, team_bonus_id);

    const int JAP_KATAPARUTO = 59;
    shareEffect(df, JAP_KATAPARUTO, team_bonus_id);

    const int JAP_OBSOLETE = 432;
    shareEffect(df, JAP_OBSOLETE, team_bonus_id);

    const int JAP_OBSOLETE_2 = 433;
    shareEffect(df, JAP_OBSOLETE_2, team_bonus_id);

    const int JAP_FISH_IMPROVE = 306;
    shareEffect(df, JAP_FISH_IMPROVE, team_bonus_id);

    const int JAP_FISH_WORK_1 = 434;
    shareEffect(df, JAP_FISH_WORK_1, team_bonus_id);

    const int JAP_FISH_WORK_2 = 435;
    shareEffect(df, JAP_FISH_WORK_2, team_bonus_id);

    const int JAP_FISH_WORK_3 = 436;
    shareEffect(df, JAP_FISH_WORK_3, team_bonus_id);

    const int JAP_DROP_DISCOUNT = 338;
    shareEffect(df, JAP_DROP_DISCOUNT, team_bonus_id);

    const int JAP_ATTACK_SPEED = 339;
    shareEffect(df, JAP_ATTACK_SPEED, team_bonus_id);

    const int JAP_GALLEY_LOS = 406;
    shareEffect(df, JAP_GALLEY_LOS, team_bonus_id);
}

void khmerCivBonus(genie::DatFile *df) {
    skipShare(654);
    skipShare(655);
    
    auto team_bonus_id = 647;

    const int KHMER_TECH_TREE = 646;
    shareEffect(df, KHMER_TECH_TREE, team_bonus_id);

    const int KHMER_TUSK_SWORDS = 662;
    shareEffect(df, KHMER_TUSK_SWORDS, team_bonus_id);    

    const int KHMER_DOUBLE_CROSSBOW = 663;
    shareEffect(df, KHMER_DOUBLE_CROSSBOW, team_bonus_id);   

    const int KHMER_ELEP_FASTER = 703;
    shareEffect(df, KHMER_ELEP_FASTER, team_bonus_id);

    const int KHMER_HOUSE_GARRISON = 693;
    shareEffect(df, KHMER_HOUSE_GARRISON, team_bonus_id);

    const int KHMER_SCORP_RANGE = 647;
    shareEffect(df, KHMER_SCORP_RANGE, team_bonus_id);
}

void koreansCivBonus(genie::DatFile *df) {
    skipShare(500);
    skipShare(501);
    skipShare(502);
    skipShare(503);
    
    auto team_bonus_id = 505;

    const int KOREA_TECH_TREE = 504;
    shareEffect(df, KOREA_TECH_TREE, team_bonus_id);

    const int KOREA_EUPSEONG = 541;
    shareEffect(df, KOREA_EUPSEONG, team_bonus_id);

    const int KOREA_SHINKICHON = 506;
    shareEffect(df, KOREA_SHINKICHON, team_bonus_id);

    const int KORA_ARCHER_ARMOR = 508;
    shareEffect(df, KORA_ARCHER_ARMOR, team_bonus_id);

    const int KOREA_WOOD_DISCOUNT = 561;
    shareEffect(df, KOREA_WOOD_DISCOUNT, team_bonus_id);
}

void lithuaniansCivBonus(genie::DatFile *df) {
    skipShare(720);
    skipShare(721);
    skipShare(747);
    skipShare(817);
    
    auto team_bonus_id = 713;

    const int LITH_TECH_TREE = 712;
    shareEffect(df, LITH_TECH_TREE, team_bonus_id);

    const int LITH_HILL_FORTS = 728;
    shareEffect(df, LITH_HILL_FORTS, team_bonus_id);

    const int LITH_TOWER_SHIELDS = 729;
    shareEffect(df, LITH_TOWER_SHIELDS, team_bonus_id);

    const int LITH_FOOD_BONUS = 734;
    shareEffect(df, LITH_FOOD_BONUS, team_bonus_id);

    const int LITH_TRASH_FASTER = 745;
    shareEffect(df, LITH_TRASH_FASTER, team_bonus_id);

    const int LITH_RELIC_ATTACK_1 = 736;
    shareEffect(df, LITH_RELIC_ATTACK_1, team_bonus_id);

    const int LITH_RELIC_ATTACK_2 = 737;
    shareEffect(df, LITH_RELIC_ATTACK_2, team_bonus_id);

    const int LITH_RELIC_ATTACK_3 = 738;
    shareEffect(df, LITH_RELIC_ATTACK_3, team_bonus_id);

    const int LITH_RELIC_ATTACK_4 = 739;
    shareEffect(df, LITH_RELIC_ATTACK_4, team_bonus_id);

    const int LITH_MONASTERY_SPEED = 713;
    shareEffect(df, LITH_MONASTERY_SPEED, team_bonus_id);
}

void magyarsCivBonus(genie::DatFile *df) {
    skipShare(525);
    skipShare(526);
    
    auto team_bonus_id = 6;

    const int MAG_TECH_TREE = 5;
    shareEffect(df, MAG_TECH_TREE, team_bonus_id);

    const int MAG_RECURVE_BOW = 570;
    shareEffect(df, MAG_RECURVE_BOW, team_bonus_id);

    const int MAG_MERCENARIES = 571;
    shareEffect(df, MAG_MERCENARIES, team_bonus_id);

    const int MAG_ANIMAL_BONUS = 528;
    shareEffect(df, MAG_ANIMAL_BONUS, team_bonus_id);
    
    const int MAG_SCOUTS_DISCOUNT = 524;
    shareEffect(df, MAG_SCOUTS_DISCOUNT, team_bonus_id);

    const int MAG_ARCHER_LOS = 6;
    shareEffect(df, MAG_ARCHER_LOS, team_bonus_id);
}

void malayCivBonus(genie::DatFile *df) {
    skipShare(656);
    skipShare(657);
    
    auto team_bonus_id = 649;

    const int MALAY_TECH_TREE = 648;
    shareEffect(df, MALAY_TECH_TREE, team_bonus_id);

    const int MALAY_THALASSOCRACY = 664;
    shareEffect(df, MALAY_THALASSOCRACY, team_bonus_id);

    const int MALAY_FORCED_LEVY = 665;
    shareEffect(df, MALAY_FORCED_LEVY, team_bonus_id);

    const int MALAY_OBSOLETE = 682;
    shareEffect(df, MALAY_OBSOLETE, team_bonus_id);

    const int MALAY_OBSOLETE_2 = 683;
    shareEffect(df, MALAY_OBSOLETE_2, team_bonus_id);

    const int MALAY_OBSOLETE_3 = 684;
    shareEffect(df, MALAY_OBSOLETE_3, team_bonus_id);

    const int MALAY_FASTER_AGEADVANCE = 674;
    shareEffect(df, MALAY_FASTER_AGEADVANCE, team_bonus_id);
    
    const int MALAY_FISHTRAP_DISCOUNT = 675;
    shareEffect(df, MALAY_FISHTRAP_DISCOUNT, team_bonus_id);
    
    const int MALAY_FISHTRAP_FOOD = 677;
    shareEffect(df, MALAY_FISHTRAP_FOOD, team_bonus_id);
    
    const int MALAY_ELE_DISCOUNT_1 = 695;
    shareEffect(df, MALAY_ELE_DISCOUNT_1, team_bonus_id);
    
    const int MALAY_ELE_DISCOUNT_2 = 696;
    shareEffect(df, MALAY_ELE_DISCOUNT_2, team_bonus_id);

    const int MALAY_DOCK_LOS = 649;
    shareEffect(df, MALAY_DOCK_LOS, team_bonus_id);
}

void maliansCivBonus(genie::DatFile *df) {
    skipShare(596);
    skipShare(597);
    
    auto team_bonus_id = 43;

    const int MALI_TECH_TREE = 42;
    shareEffect(df, MALI_TECH_TREE, team_bonus_id);

    const int MALI_TIGUI = 605;
    shareEffect(df, MALI_TIGUI, team_bonus_id);

    const int MALI_FARIMBA = 606;
    shareEffect(df, MALI_FARIMBA, team_bonus_id);
    
    const int MALI_TC_WOOD_COST = 165;
    shareEffect(df, MALI_TC_WOOD_COST, team_bonus_id);
    
    const int MALI_WOOD_DISCOUNT = 617;
    shareEffect(df, MALI_WOOD_DISCOUNT, team_bonus_id);
    
    const int MALI_INF_ARMOR_1 = 618;
    shareEffect(df, MALI_INF_ARMOR_1, team_bonus_id);
    
    const int MALI_INF_ARMOR_2 = 619;
    shareEffect(df, MALI_INF_ARMOR_2, team_bonus_id);
    
    const int MALI_INF_ARMOR_3 = 620;
    shareEffect(df, MALI_INF_ARMOR_3, team_bonus_id);
    
    const int MALI_GOLDMINING = 621;
    shareEffect(df, MALI_GOLDMINING, team_bonus_id);

    const int MALI_UNIVERSITY_WORKRATE = 43;
    shareEffect(df, MALI_UNIVERSITY_WORKRATE, team_bonus_id);
}

void mayansCivBonus(genie::DatFile *df) {
    skipShare(468);
    skipShare(469);
    
    auto team_bonus_id = 489;

    const int MAY_TECH_TREE = 449;
    shareEffect(df, MAY_TECH_TREE, team_bonus_id);

    const int MAY_HULCHE_JAVELINEERS = 515;
    shareEffect(df, MAY_HULCHE_JAVELINEERS, team_bonus_id);

    const int MAY_EL_DORADO = 456;
    shareEffect(df, MAY_EL_DORADO, team_bonus_id);

    const int MAY_START_RES = 216;
    shareEffect(df, MAY_START_RES, team_bonus_id);

    const int MAY_VILS_SPAWN = 586;
    shareEffect(df, MAY_VILS_SPAWN, team_bonus_id);

    const int MAY_ARCHER_COST_1 = 485;
    shareEffect(df, MAY_ARCHER_COST_1, team_bonus_id);

    const int MAY_ARCHER_COST_2 = 486;
    shareEffect(df, MAY_ARCHER_COST_2, team_bonus_id);

    const int MAY_RESOURCES_LONGER_1 = 771;
    shareEffect(df, MAY_RESOURCES_LONGER_1, team_bonus_id);

    const int MAY_RESOURCES_LONGER_2 = 772;
    shareEffect(df, MAY_RESOURCES_LONGER_2, team_bonus_id);

    const int MAY_RESOURCES_LONGER_3 = 773;
    shareEffect(df, MAY_RESOURCES_LONGER_3, team_bonus_id);

    const int MAY_RESOURCES_LONGER_4 = 774;
    shareEffect(df, MAY_RESOURCES_LONGER_4, team_bonus_id);

    const int MAY_RESOURCES_LONGER_5 = 775;
    shareEffect(df, MAY_RESOURCES_LONGER_5, team_bonus_id);

    const int MAY_RESOURCES_LONGER_6 = 776;
    shareEffect(df, MAY_RESOURCES_LONGER_6, team_bonus_id);

    const int MAY_RESOURCES_LONGER_7 = 777;
    shareEffect(df, MAY_RESOURCES_LONGER_7, team_bonus_id);

    const int MAY_RESOURCES_LONGER_8 = 778;
    shareEffect(df, MAY_RESOURCES_LONGER_8, team_bonus_id);

    const int MAY_RESOURCES_LONGER_9 = 779;
    shareEffect(df, MAY_RESOURCES_LONGER_9, team_bonus_id);

    const int MAY_RESOURCES_LONGER_10 = 780;
    shareEffect(df, MAY_RESOURCES_LONGER_10, team_bonus_id);

    const int MAY_RESOURCES_LONGER_11 = 781;
    shareEffect(df, MAY_RESOURCES_LONGER_11, team_bonus_id);

    const int MAY_WALL_DISCOUNT = 489;
    shareEffect(df, MAY_WALL_DISCOUNT, team_bonus_id);
}

void mongolsCivBonus(genie::DatFile *df) {
    skipShare(270);
    skipShare(369);
    
    auto team_bonus_id = 407;

    const int MONG_TECH_TREE = 277;
    shareEffect(df, MONG_TECH_TREE, team_bonus_id);

    const int MONG_NOMADS = 542;
    shareEffect(df, MONG_NOMADS, team_bonus_id);

    const int MONG_NOMADS_2 = 681;
    shareEffect(df, MONG_NOMADS_2, team_bonus_id);

    const int MONG_DRILL = 457;
    shareEffect(df, MONG_DRILL, team_bonus_id);

    const int MONG_CA_FIRING = 393;
    shareEffect(df, MONG_CA_FIRING, team_bonus_id);

    const int MONG_LIGHTCAV_HP = 387;
    shareEffect(df, MONG_LIGHTCAV_HP, team_bonus_id);

    const int MONG_HUNTERS = 388;
    shareEffect(df, MONG_HUNTERS, team_bonus_id);

    const int MONG_SCOUT_LOS = 407;
    shareEffect(df, MONG_SCOUT_LOS, team_bonus_id);
}

void persiansCivBonus(genie::DatFile *df) {
    skipShare(271);
    skipShare(365);
    
    auto team_bonus_id = 408;

    const int PERS_TECH_TREE = 260;
    shareEffect(df, PERS_TECH_TREE, team_bonus_id);

    const int PERS_MAHOUTS = 458;
    shareEffect(df, PERS_MAHOUTS, team_bonus_id);

    const int PERS_KAMANDARAN = 543;
    skipShare(PERS_KAMANDARAN);

    const int PERS_START_RES = 212;
    shareEffect(df, PERS_START_RES, team_bonus_id);

    const int PERS_TC_HP = 340;
    shareEffect(df, PERS_TC_HP, team_bonus_id);

    const int PERS_DOCK_HP = 347;
    shareEffect(df, PERS_DOCK_HP, team_bonus_id);

    const int PERS_TC_SPEED = 421;
    shareEffect(df, PERS_TC_SPEED, team_bonus_id);

    const int PERS_DOCK_SPEED = 424;
    shareEffect(df, PERS_DOCK_SPEED, team_bonus_id);

    const int PERS_KNIGHT_BONUS = 408;
    shareEffect(df, PERS_KNIGHT_BONUS, team_bonus_id);
}

void polesCivBonus(genie::DatFile *df) {
    skipShare(805);
    skipShare(806);
    
    auto team_bonus_id = 802;

    const int POL_TECH_TREE = 801;
    shareEffect(df, POL_TECH_TREE, team_bonus_id);

    const int POL_SZLACHTA_PRIVILEGES = 809;
    shareEffect(df, POL_SZLACHTA_PRIVILEGES, team_bonus_id);

    const int POL_LECHITIC_LEGACY = 810;
    shareEffect(df, POL_LECHITIC_LEGACY, team_bonus_id);
    
    const int POL_VIL_REGEN = 815;
    shareEffect(df, POL_VIL_REGEN, team_bonus_id);
    
    const int POL_FOLWARK = 816;
    shareEffect(df, POL_FOLWARK, team_bonus_id);
    
    const int POL_FOLWARK_2 = 818;
    shareEffect(df, POL_FOLWARK_2, team_bonus_id);
    
    const int POL_FOLWARK_3 = 819;
    shareEffect(df, POL_FOLWARK_3, team_bonus_id);
    
    const int POL_FOLWARK_4 = 820;
    shareEffect(df, POL_FOLWARK_4, team_bonus_id);
    
    const int POL_FOLWARKFARM_FOOD_1 = 821;
    shareEffect(df, POL_FOLWARKFARM_FOOD_1, team_bonus_id);
    
    const int POL_FOLWARKFARM_FOOD_2 = 822;
    shareEffect(df, POL_FOLWARKFARM_FOOD_2, team_bonus_id);
    
    const int POL_FOLWARKFARM_FOOD_3 = 823;
    shareEffect(df, POL_FOLWARKFARM_FOOD_3, team_bonus_id);
    
    const int POL_STONE_GOLDGEN_1 = 827;
    shareEffect(df, POL_STONE_GOLDGEN_1, team_bonus_id);
    
    const int POL_STONE_GOLDGEN_2 = 828;
    shareEffect(df, POL_STONE_GOLDGEN_2, team_bonus_id);
    
    const int POL_STONE_GOLDGEN_3 = 829;
    shareEffect(df, POL_STONE_GOLDGEN_3, team_bonus_id);

    const int POL_SCOUTS_BONUS = 802;
    shareEffect(df, POL_SCOUTS_BONUS, team_bonus_id);
}

void portugueseCivBonus(genie::DatFile *df) {
    skipShare(591);
    skipShare(592);
    skipShare(622);
    skipShare(623);
    
    auto team_bonus_id = 32;

    const int PORT_TECH_TREE = 31;
    shareEffect(df, PORT_TECH_TREE, team_bonus_id);

    const int PORT_CARRACK = 601;
    shareEffect(df, PORT_CARRACK, team_bonus_id);

    const int PORT_ARQUEBUS = 602;
    shareEffect(df, PORT_ARQUEBUS, team_bonus_id);

    const int PORT_OBSOLETE = 35;
    shareEffect(df, PORT_OBSOLETE, team_bonus_id);

    const int PORT_GOLD_DISCOUNT = 33;
    shareEffect(df, PORT_GOLD_DISCOUNT, team_bonus_id);

    const int PORT_FORAGERS = 510;
    shareEffect(df, PORT_FORAGERS, team_bonus_id);

    // Ships + 10% hp

    const int PORT_FEITORIA_AVAIL = 600;
    shareEffect(df, PORT_FEITORIA_AVAIL, team_bonus_id);

    const int PORT_RESEARCH_SPEED = 32;
    shareEffect(df, PORT_RESEARCH_SPEED, team_bonus_id);
}

void romansCivBonus(genie::DatFile *df) {
    skipShare(892);
    skipShare(893);
    skipShare(896);
    skipShare(903);
    
    auto team_bonus_id = 891;

    const int ROM_TECH_TREE = 890;
    shareEffect(df, ROM_TECH_TREE, team_bonus_id);

    const int ROM_BALLISTAS = 894;
    shareEffect(df, ROM_BALLISTAS, team_bonus_id);

    const int ROM_COMITATENSES = 895;
    shareEffect(df, ROM_COMITATENSES, team_bonus_id);

    const int ROM_SCORP_BALLISTICS = 901;
    shareEffect(df, ROM_SCORP_BALLISTICS, team_bonus_id);

    const int ROM_VILS_WORK = 898;
    shareEffect(df, ROM_VILS_WORK, team_bonus_id);
    
    const int ROM_NAVY = 899;
    shareEffect(df, ROM_NAVY, team_bonus_id);

    if (df->Techs.at(889).EffectID == SCALE_MAIL_ARMOR) {
        auto scale_mail_id = duplicateForShare(df, SCALE_MAIL_ARMOR);
        df->Techs.at(889).EffectID = scale_mail_id;
    }
    shareEffect(df, df->Techs.at(889).EffectID, team_bonus_id);

    if (df->Techs.at(890).EffectID == CHAIN_MAIL_ARMOR) {
        auto chain_mail_id = duplicateForShare(df, CHAIN_MAIL_ARMOR);
        df->Techs.at(890).EffectID = chain_mail_id;
    }
    shareEffect(df, df->Techs.at(890).EffectID, team_bonus_id);

    if (df->Techs.at(891).EffectID == PLATE_MAIL_ARMOR) {
        auto plate_mail_id = duplicateForShare(df, PLATE_MAIL_ARMOR);
        df->Techs.at(891).EffectID = plate_mail_id;
    }
    shareEffect(df, df->Techs.at(891).EffectID, team_bonus_id);
    
    const int ROM_SCORP_DISCOUT = 900;
    shareEffect(df, ROM_SCORP_DISCOUT, team_bonus_id);
}

void saracensCivBonus(genie::DatFile *df) {
    skipShare(266);
    skipShare(366);
    
    auto team_bonus_id = 409;

    const int SAR_TECH_TREE = 261;
    shareEffect(df, SAR_TECH_TREE, team_bonus_id);

    const int SAR_ZEALOTRY = 459;
    shareEffect(df, SAR_ZEALOTRY, team_bonus_id);

    const int SAR_COUNTERWEIGHTS = 480;
    shareEffect(df, SAR_COUNTERWEIGHTS, team_bonus_id);

    const int SAR_OBSOLETE = 545;
    shareEffect(df, SAR_OBSOLETE, team_bonus_id);

    const int SAR_MARKET = 354;
    shareEffect(df, SAR_MARKET, team_bonus_id);

    const int SAR_TRANSPORT = 311;
    shareEffect(df, SAR_TRANSPORT, team_bonus_id);

    const int SAR_GALLEY_ATTACK = 416;
    shareEffect(df, SAR_GALLEY_ATTACK, team_bonus_id);

    const int SAR_CAMEL_HP = 312;
    shareEffect(df, SAR_CAMEL_HP, team_bonus_id);

    const int SAR_ARCHER_BONUS_1 = 313;
    shareEffect(df, SAR_ARCHER_BONUS_1, team_bonus_id);

    const int SAR_ARCHER_BONUS_2 = 318;
    shareEffect(df, SAR_ARCHER_BONUS_2, team_bonus_id);

    const int SAR_ARCHER_BONUS = 409;
    shareEffect(df, SAR_ARCHER_BONUS, team_bonus_id);
}

void siciliansCivBonus(genie::DatFile *df) {
    skipShare(788);
    skipShare(789);
    
    auto team_bonus_id = 785;

    const int SIC_TECH_TREE = 784;
    shareEffect(df, SIC_TECH_TREE, team_bonus_id);

    const int SIC_FIRST_CRUSADE = 792;
    shareEffect(df, SIC_FIRST_CRUSADE, team_bonus_id);

    const int SIC_HAUBERK = 793;
    shareEffect(df, SIC_HAUBERK, team_bonus_id);

    const int SIC_START_RES = 217;
    shareEffect(df, SIC_START_RES, team_bonus_id);

    const int SIC_BUILD_SPEED = 795;
    shareEffect(df, SIC_BUILD_SPEED, team_bonus_id);

    const int SIC_BONUS_RESISTANCE = 796;
    shareEffect(df, SIC_BONUS_RESISTANCE, team_bonus_id);

    const int SIC_FARM_FOOD_1 = 797;
    shareEffect(df, SIC_FARM_FOOD_1, team_bonus_id);

    const int SIC_FARM_FOOD_2 = 798;
    shareEffect(df, SIC_FARM_FOOD_2, team_bonus_id);

    const int SIC_FARM_FOOD_3 = 799;
    shareEffect(df, SIC_FARM_FOOD_3, team_bonus_id);

    const int SIC_DONJON_AVAIL = 800;
    shareEffect(df, SIC_DONJON_AVAIL, team_bonus_id);

    const int SIC_TRANSPORT = 785;
    shareEffect(df, SIC_TRANSPORT, team_bonus_id);
}

void slavsCivBonus(genie::DatFile *df) {
    skipShare(556);
    skipShare(557);
    
    auto team_bonus_id = 9;

    const int SLAVS_TECH_TREE = 7;
    shareEffect(df, SLAVS_TECH_TREE, team_bonus_id);

    const int SLAVS_DETINETS = 481;
    shareEffect(df, SLAVS_DETINETS, team_bonus_id);

    const int SLAVS_DRUZHINA = 569;
    shareEffect(df, SLAVS_DRUZHINA, team_bonus_id);

    const int SLAVS_FARMS = 690;
    shareEffect(df, SLAVS_FARMS, team_bonus_id);

    const int SLAVS_SIEGE_DISCOUNT = 567;
    shareEffect(df, SLAVS_SIEGE_DISCOUNT, team_bonus_id);

    const int SLAVS_MILITARY_POP = 758;
    shareEffect(df, SLAVS_MILITARY_POP, team_bonus_id);
}

void spanishCivBonus(genie::DatFile *df) {
    skipShare(491);
    skipShare(492);
    
    auto team_bonus_id = 490;

    const int SPAN_TECH_TREE = 446;
    shareEffect(df, SPAN_TECH_TREE, team_bonus_id);

    const int SPAN_INQUISITION = 547;
    shareEffect(df, SPAN_INQUISITION, team_bonus_id);

    const int SPAN_SUPREMACY = 495;
    shareEffect(df, SPAN_SUPREMACY, team_bonus_id);

    const int SPAN_TC_BUILD = 168;
    shareEffect(df, SPAN_TC_BUILD, team_bonus_id);

    const int SPAN_TECH_REWARD = 300;
    shareEffect(df, SPAN_TECH_REWARD, team_bonus_id);

    const int SPAN_TECH_REWARD_2 = 181;
    shareEffect(df, SPAN_TECH_REWARD_2, team_bonus_id);

    const int SPAN_TECH_REWARD_3 = 237;
    shareEffect(df, SPAN_TECH_REWARD_3, team_bonus_id);

    const int SPAN_MISSIONARY_AVAIL = 496;
    shareEffect(df, SPAN_MISSIONARY_AVAIL, team_bonus_id);

    const int SPAN_TRADE = 490;
    shareEffect(df, SPAN_TRADE, team_bonus_id);
}

void tatarsCivBonus(genie::DatFile *df) {
    skipShare(716);
    skipShare(717);
    
    auto team_bonus_id = 709;

    const int TAT_TECH_TREE = 708;
    shareEffect(df, TAT_TECH_TREE, team_bonus_id);

    const int TAT_SILK_ARMOR = 724;
    shareEffect(df, TAT_SILK_ARMOR, team_bonus_id);

    const int TAT_TIMURID = 725;
    shareEffect(df, TAT_TIMURID, team_bonus_id);
    
    const int TAT_SHEEP_BONUS = 741;
    shareEffect(df, TAT_SHEEP_BONUS, team_bonus_id);
    
    const int TAT_SHEEP_BONUS_2 = 299;
    shareEffect(df, TAT_SHEEP_BONUS_2, team_bonus_id);
    
    const int TAT_SHEEP_BONUS_3 = 303;
    shareEffect(df, TAT_SHEEP_BONUS_3, team_bonus_id);
    
    const int TAT_SHEEP_BONUS_4 = 305;
    shareEffect(df, TAT_SHEEP_BONUS_4, team_bonus_id);
    
    const int TAT_PARTHIAN_TACTICS = 735;
    shareEffect(df, TAT_PARTHIAN_TACTICS, team_bonus_id);
    
    const int TAT_ELEVATION = 733;
    shareEffect(df, TAT_ELEVATION, team_bonus_id);

    const int TAT_CA_LOS = 709;
    shareEffect(df, TAT_CA_LOS, team_bonus_id);
}

void teutonsCivBonus(genie::DatFile *df) {
    skipShare(273);
    skipShare(362);
    
    auto team_bonus_id = 404;

    const int TEUT_TECH_TREE = 262;
    shareEffect(df, TEUT_TECH_TREE, team_bonus_id);

    const int TEUT_IRONCLAD = 544;
    shareEffect(df, TEUT_IRONCLAD, team_bonus_id);

    const int TEUT_CRENELLATIONS = 461;
    shareEffect(df, TEUT_CRENELLATIONS, team_bonus_id);
    
    const int TEUT_OBSOLETE = 332;
    shareEffect(df, TEUT_OBSOLETE, team_bonus_id);
    
    const int TEUT_HEAL_RANGE = 345;
    shareEffect(df, TEUT_HEAL_RANGE, team_bonus_id);
    
    const int TEUT_TOWER_GARISON = 352;
    shareEffect(df, TEUT_TOWER_GARISON, team_bonus_id);

    const int TEUT_FARMS_DISCOUNT = 314;
    shareEffect(df, TEUT_FARMS_DISCOUNT, team_bonus_id);
    
    const int TEUT_TC_IMPROVE = 335;
    shareEffect(df, TEUT_TC_IMPROVE, team_bonus_id);
    
    const int TEUT_ARMOR_1 = 333;
    shareEffect(df, TEUT_ARMOR_1, team_bonus_id);
    
    const int TEUT_ARMOR_2 = 334;
    shareEffect(df, TEUT_ARMOR_2, team_bonus_id);
    
    const int TEUT_CONVERSION_RESISTANCE = 404;
    shareEffect(df, TEUT_CONVERSION_RESISTANCE, team_bonus_id);
}

void turksCivBonus(genie::DatFile *df) {
    skipShare(268);
    skipShare(353);
    skipShare(367);
    
    auto team_bonus_id = 410;

    const int TURK_TECH_TREE = 263;
    shareEffect(df, TURK_TECH_TREE, team_bonus_id);

    const int TURK_SIPAHI = 546;
    shareEffect(df, TURK_SIPAHI, team_bonus_id);

    const int TURK_ARTILLERY = 460;
    shareEffect(df, TURK_ARTILLERY, team_bonus_id);

    const int TURK_HUSSAR_FREE = 527;
    shareEffect(df, TURK_HUSSAR_FREE, team_bonus_id);

    const int TURK_CHEMISTRY_FREE = 301;
    shareEffect(df, TURK_CHEMISTRY_FREE, team_bonus_id);

    const int TURK_GUNPOWDER_HP = 296;
    shareEffect(df, TURK_GUNPOWDER_HP, team_bonus_id);

    const int TURK_GUNPOWDERSHIP_HP = 284;
    shareEffect(df, TURK_GUNPOWDERSHIP_HP, team_bonus_id);

    const int TURK_GOLD_MINING = 295;
    shareEffect(df, TURK_GOLD_MINING, team_bonus_id);

    const int TURK_LIGHTCAV_PIERCE = 509;
    shareEffect(df, TURK_LIGHTCAV_PIERCE, team_bonus_id);

    const int TURK_GUNPOWDER_TRAINTIME = 410;
    shareEffect(df, TURK_GUNPOWDER_TRAINTIME, team_bonus_id);
}

void vietnameseCivBonus(genie::DatFile *df) {
    skipShare(660);
    skipShare(661);
    
    auto team_bonus_id = 653;

    const int VIET_TECH_TREE = 652;
    shareEffect(df, VIET_TECH_TREE, team_bonus_id);

    const int VIET_CHATRAS = 668;
    shareEffect(df, VIET_CHATRAS, team_bonus_id);

    const int VIET_PAPER_MONEY_1 = 669;
    shareEffect(df, VIET_PAPER_MONEY_1, team_bonus_id);

    const int VIET_PAPER_MONEY_2 = 881;
    shareEffect(df, VIET_PAPER_MONEY_2, team_bonus_id);

    const int VIET_PAPER_MONEY_3 = 882;
    shareEffect(df, VIET_PAPER_MONEY_3, team_bonus_id);

    const int VIET_PAPER_MONEY_4 = 883;
    shareEffect(df, VIET_PAPER_MONEY_4, team_bonus_id);

    const int VIET_ARCHERS_HP = 672;
    shareEffect(df, VIET_ARCHERS_HP, team_bonus_id);

    const int VIET_VISION = 698;
    shareEffect(df, VIET_VISION, team_bonus_id);
}

void vikingsCivBonus(genie::DatFile *df) {
    skipShare(269);
    skipShare(370);
    skipShare(397);
    skipShare(398);
    
    auto team_bonus_id = 411;

    const int VIK_TECH_TREE = 276;
    shareEffect(df, VIK_TECH_TREE, team_bonus_id);

    const int VIK_CHIEFTAINS = 517;
    shareEffect(df, VIK_CHIEFTAINS, team_bonus_id);

    const int VIK_BOGSVEIGAR = 467;
    shareEffect(df, VIK_BOGSVEIGAR, team_bonus_id);

    const int VIK_WHEELBARROW_FREE = 391;
    shareEffect(df, VIK_WHEELBARROW_FREE, team_bonus_id);

    const int VIK_HANDCART_FREE = 412;
    shareEffect(df, VIK_HANDCART_FREE, team_bonus_id);

    const int VIK_WARSHIP_COST_1 = 383;
    shareEffect(df, VIK_WARSHIP_COST_1, team_bonus_id);
    
    const int VIK_WARSHIP_COST_2 = 386;
    shareEffect(df, VIK_WARSHIP_COST_2, team_bonus_id);
    
    const int VIK_WARSHIP_COST_3 = 394;
    shareEffect(df, VIK_WARSHIP_COST_3, team_bonus_id);
    
    const int VIK_INF_HP_1 = 390;
    shareEffect(df, VIK_INF_HP_1, team_bonus_id);
    
    const int VIK_INF_HP_2 = 427;
    shareEffect(df, VIK_INF_HP_2, team_bonus_id);
    
    const int VIK_INF_HP_3 = 428;
    shareEffect(df, VIK_INF_HP_3, team_bonus_id);
    
    const int VIK_DOCK_COST = 411;
    shareEffect(df, VIK_DOCK_COST, team_bonus_id);
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