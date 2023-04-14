#include "nomadking.h"
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureNomadKing(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        genie::Unit &king = civ.Units.at(ID_KING);
        std::cout << "Removing building garrison task from King (" << ID_KING << ") for civ " << civ.Name << std::endl;
        king.Bird.TaskList.erase(king.Bird.TaskList.end() - 1);
    }
}
