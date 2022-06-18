#include "exploding_kings.h"
#include <set>
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureExplodingKings(genie::DatFile *df) {
    std::set<int> dyingHeroesTimer = {
        ID_GUY_JOSSELYNE, //paladin
        ID_GUY_LANCELOT, //paladin
        ID_GUY_MORDRED, //paladin
        ID_GUY_RICHARD_THE_LIONHEART, //paladin
        ID_HENRY_V, //paladin
        ID_DINH_LE, //unique paladin
        ID_BABUR, //camel
        ID_TABINSHWEHTI, //elephant
        ID_SURYAVARMAN_I, //elephant
        ID_BAYINNAUNG, //elephant
        ID_ABRAHA_ELEPHANT, //elephant
        ID_KHOSRAU, //elephant
        ID_BAD_NEIGHBOR_PACKED, //treb
        ID_BAD_NEIGHBOR, //treb
        ID_GODS_OWN_SLING, //treb
        ID_EMPEROR_IN_A_BARREL, //unique
    };


    for (genie::Civ &civ : df->Civs) {
        //set the king to become the first dying unit in the chain when it dies
        std::cout << "Patched king unit " << ID_KING << " for civ " << civ.Name << "\n";
        genie::Unit &dyingUnit = civ.Units.at(ID_KING);
        for (int dyingUnitId : dyingHeroesTimer)
        {
            //set the king to the first dying unit, 
            //and then each subsequen dying unit to the next ID
            dyingUnit.DeadUnitID = dyingUnitId;
            std::cout << "Set dying unit for previous patched unit to " << dyingUnitId << " for civ " << civ.Name << "\n";

            //set dying unit pointer to the next unit 
            genie::Unit &dyingUnit = civ.Units.at(dyingUnitId);

            //make the unit instantly die on spawn (this affects all units but king)
            dyingUnit.HitPoints = -1;
            dyingUnit.Type50.Armours = {};
            std::cout << "Patched a hero unit " << dyingUnitId << " for civ " << civ.Name << "\n";
        }

        //set the final dying hero to ID Saboteur (this affects only last unit)
        dyingUnit.DeadUnitID = ID_SABOTEUR;


        //modify to make king explode eventually
        genie::Unit &saboteur = civ.Units.at(ID_SABOTEUR);
        saboteur.HitPoints = -1;
        saboteur.DeadUnitID = ID_FURIOUS_THE_MONKEY_BOY;
        saboteur.Type50.Attacks.at(0).Amount = 9999;
        saboteur.Type50.Attacks.at(1).Amount = 9999;
        saboteur.Type50.Attacks.at(2).Amount = 9999;
        saboteur.Type50.MaxRange = 25;
        saboteur.Type50.BlastWidth = 25;
        saboteur.Type50.BlastAttackLevel = 0;  // destroy all the things
        saboteur.WwiseTrainSoundID = 0;  // prevent melee unit train sound from playing
        std::cout << "Patched Saboteur unit for civ " << civ.Name << "\n";

        //modify furious to create a second explotion so that everything is truly flattened
        genie::Unit &furious = civ.Units.at(ID_FURIOUS_THE_MONKEY_BOY);
        furious.HitPoints = -1;
        furious.Type50.Attacks.at(0).Amount = 9999;
        furious.Type50.Attacks.at(1).Amount = 9999;
        furious.Type50.MaxRange = 25;
        furious.Type50.BlastWidth = 25;
        furious.Type50.BlastAttackLevel = 0;  // destroy all the things
        std::cout << "Patched Furious the Monkey Boy unit for civ " << civ.Name << "\n";
    }
}
