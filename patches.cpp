#include "genie/dat/DatFile.h"
#include "patches.h"
#include <list>

using namespace std;

static const int ID_TRANSPORT_SHIP = 545;
static const int ID_RAM = 35;
static const int ID_CAPPED_RAM = 422;
static const int ID_SIEGE_RAM = 548;
static const int ID_SIEGE_TOWER = 1105;

static const int CLASS_TRANSPORT_BOAT = 20;
static const int ACTION_TYPE_GARRISON = 3;


void makeTransportShipsFly(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            if (unit.ID == ID_TRANSPORT_SHIP) {
                unit.FlyMode = 1; // true
                unit.TerrainRestriction = 0; // All
                unit.CollisionSize = {0, 0, 0}; // walk through everything, like flying
                cout << "Patched Transport Ship unit for civ " << civ.Name << "\n";
            }
        }
    }
}

void preventRamsAndSiegeTowersFromBoardingTransportShips(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            if (unit.ID == ID_RAM || unit.ID == ID_CAPPED_RAM || unit.ID == ID_SIEGE_RAM || unit.ID == ID_SIEGE_TOWER) {
                list<int> toDelete;
                for (int i = 0; i < unit.Bird.TaskList.size(); i++) {
                    genie::Task task = unit.Bird.TaskList.at(i);
                    if (task.ActionType == ACTION_TYPE_GARRISON && task.ClassID == CLASS_TRANSPORT_BOAT) {
                        toDelete.push_front(i);
                    }
                }

                for (int i:toDelete) {
                    unit.Bird.TaskList.erase(unit.Bird.TaskList.begin() + i);
                }
                cout << "Patched unit " << unit.Name << " (" << unit.ID << ") unit for civ " << civ.Name << "\n";
            }
        }
    }
}
