#include "exploding_kings.h"
#include <set>
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureExplodingKings(genie::DatFile *df) {
    std::deque<int> dyingHeroesTimer = {
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
        //set the king to the first dying unit, 
        genie::Unit &dyingUnitPointer = civ.Units.at(ID_KING);
        std::cout << "Patched king unit " << ID_KING << " for civ " << civ.Name << "\n";
        for (int nextDyingUnitId : dyingHeroesTimer)
        {
            //and then each DeadUnitId  to the next ID
            dyingUnitPointer.DeadUnitID = nextDyingUnitId;
            std::cout << "Set dying unit for previous patched unit to " << nextDyingUnitId << " for civ " << civ.Name << "\n";

            //set dying unit pointer to the next unit 
            genie::Unit &dyingUnitPointer = civ.Units.at(nextDyingUnitId);

            //make the unit instantly die on spawn (this affects all units but king)
            dyingUnitPointer.HitPoints = -1;
            dyingUnitPointer.Type50.Armours = {};
            std::cout << "Patched a hero unit " << nextDyingUnitId << " for civ " << civ.Name << "\n";
        }

        //set the final dying hero to ID Saboteur (this affects only last unit)
        dyingUnitPointer.DeadUnitID = ID_SABOTEUR;
        std::cout << "Set dying unit for previous patched unit to " << ID_SABOTEUR << " for civ " << civ.Name << "\n";


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
