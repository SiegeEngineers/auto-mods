#include "kidnap.h"
#include <list>
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureKidnap(genie::DatFile *df) {
    const std::list<int> scoutIds = {
        ID_SCOUT,
        ID_EAGLE_SCOUT,
        ID_CAMEL_SCOUT,
    };
    for (genie::Civ &civ : df->Civs) {
        for (int scoutId : scoutIds) {
            genie::Unit &scout = civ.Units.at(scoutId);
            std::cout << "Setting Scout (" << scoutId << ") properties for civ " << civ.Name << std::endl;
            scout.Type50.Attacks.clear();
            scout.Type50.DisplayedAttack = 0;
            scout.GarrisonCapacity = 1;
            scout.Bird.TaskList.erase(scout.Bird.TaskList.begin());

            std::cout << "Adding Kidnap Task to scout (" << scoutId << ") for civ " << civ.Name << std::endl;
            auto kidnapTask = new genie::Task();
            kidnapTask->ActionType = ACTION_KIDNAP_UNIT;
            kidnapTask->ClassID = CLASS_CIVILIAN;
            kidnapTask->WorkRange = 0.25;
            kidnapTask->ProceedingGraphicID = 1966;  // SCOUT_AN
            kidnapTask->TargetDiplomacy = 2;
            kidnapTask->GatherType = 2;
            scout.Bird.TaskList.push_back(*kidnapTask);

            std::cout << "Adding Loot Task to scout (" << scoutId << ") for civ " << civ.Name << std::endl;
            auto lootTask = new genie::Task();
            lootTask->ActionType = ACTION_LOOT;
            lootTask->ClassID = CLASS_BUILDING;
            lootTask->WorkRange = 0.25;
            lootTask->ProceedingGraphicID = 1966;  // SCOUT_AN
            lootTask->TargetDiplomacy = 2;
            lootTask->GatherType = 1;
            scout.Bird.TaskList.push_back(*lootTask);
        }
    }
}
