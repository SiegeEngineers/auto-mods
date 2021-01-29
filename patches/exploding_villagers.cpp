#include "exploding_villagers.h"
#include <set>
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureExplodingVillagers(genie::DatFile *df) {
    std::set<int> villagers = {
        ID_FISHING_SHIP,        ID_TRADE_COG,           ID_TRADE_CART_EMPTY,   ID_TRADE_CART_FULL,
        ID_VILLAGER_BASE_M,     ID_VILLAGER_BASE_F,     ID_VILLAGER_FARMER_M,  ID_VILLAGER_FARMER_F,
        ID_VILLAGER_SHEPHERD_M, ID_VILLAGER_SHEPHERD_F, ID_VILLAGER_FORAGER_M, ID_VILLAGER_FORAGER_F,
        ID_VILLAGER_HUNTER_M,   ID_VILLAGER_HUNTER_F,   ID_VILLAGER_FISHER_M,  ID_VILLAGER_FISHER_F,
        ID_VILLAGER_WOOD_M,     ID_VILLAGER_WOOD_F,     ID_VILLAGER_GOLD_M,    ID_VILLAGER_GOLD_F,
        ID_VILLAGER_STONE_M,    ID_VILLAGER_STONE_F,    ID_VILLAGER_BUILDER_M, ID_VILLAGER_BUILDER_F,
        ID_VILLAGER_REPAIRER_M, ID_VILLAGER_REPAIRER_F,
    };

    for (genie::Civ &civ : df->Civs) {
        for (int villager_id : villagers) {
            civ.Units.at(villager_id).DeadUnitID = ID_SABOTEUR;
            std::cout << "Patched Villager unit " << villager_id << " for civ " << civ.Name << "\n";
        }
        genie::Unit &saboteur = civ.Units.at(ID_SABOTEUR);
        saboteur.HitPoints = 0;
        saboteur.Type50.Attacks.at(0).Amount = 50;
        saboteur.Type50.Attacks.at(1).Amount = 90;
        saboteur.Type50.Attacks.at(2).Amount = 0;
        saboteur.Type50.MaxRange = 2;
        saboteur.Type50.BlastAttackLevel = 1;  // cut trees
        saboteur.TrainSound = -1;
        saboteur.WwiseTrainSoundID = 0;  // prevent melee unit train sound from playing
        std::cout << "Patched Saboteur unit for civ " << civ.Name << "\n";
    }
}
