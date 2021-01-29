#include "no_wall.h"
#include <list>
#include "genie/dat/DatFile.h"
#include "ids.h"


void disableTechEffect(genie::Tech *tech) { tech->EffectID = -1; }


void disableTechResearchLocation(genie::Tech *tech) { tech->ResearchLocation = -1; }


void disableWalls(genie::DatFile *df) {
    const std::list<int> unitsToDisable = {
        PALISADE_WALL,
    };

    const std::list<int> techsToDisable = {
        TECH_PALISADE_GATE,
        TECH_STONE_WALLS,
    };

    const std::list<int> techsToHide = {
        TECH_FORTIFIED_WALL,
    };

    for (int unitId : unitsToDisable) {
        std::cout << "Disabling unit with id " << unitId << " for all civs" << std::endl;
        for (genie::Civ &civ : df->Civs) {
            civ.Units.at(unitId).Enabled = 0;
        }
    }

    for (int techId : techsToDisable) {
        genie::Tech *tech = &df->Techs.at(techId);
        std::cout << "Disabling the effect of tech with id " << techId << " ('" << tech->Name << "')" << std::endl;
        disableTechEffect(tech);
    }

    for (int techId : techsToHide) {
        genie::Tech *tech = &df->Techs.at(techId);
        std::cout << "Disabling the research location of tech with id " << techId << " ('" << tech->Name << "')"
                  << std::endl;
        disableTechResearchLocation(tech);
    }
}
