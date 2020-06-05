#include "genie/dat/DatFile.h"
#include "patches.h"
#include <list>
#include <set>

using namespace std;

static const int ID_TRANSPORT_SHIP = 545;
static const int ID_RAM = 35;
static const int ID_CAPPED_RAM = 422;
static const int ID_SIEGE_RAM = 548;
static const int ID_SIEGE_TOWER = 1105;

static const int ID_FISHING_SHIP = 13;
static const int ID_TRADE_COG = 17;
static const int ID_TRADE_CART_EMPTY = 128;
static const int ID_TRADE_CART_FULL = 204;
static const int ID_VILLAGER_BASE_M = 83;
static const int ID_VILLAGER_BASE_F = 293;
static const int ID_VILLAGER_FARMER_M = 259;
static const int ID_VILLAGER_FARMER_F = 214;
static const int ID_VILLAGER_SHEPHERD_M = 592;
static const int ID_VILLAGER_SHEPHERD_F = 590;
static const int ID_VILLAGER_FORAGER_M = 120;
static const int ID_VILLAGER_FORAGER_F = 354;
static const int ID_VILLAGER_HUNTER_M = 122;
static const int ID_VILLAGER_HUNTER_F = 216;
static const int ID_VILLAGER_FISHER_M = 56;
static const int ID_VILLAGER_FISHER_F = 57;
static const int ID_VILLAGER_WOOD_M = 123;
static const int ID_VILLAGER_WOOD_F = 218;
static const int ID_VILLAGER_GOLD_M = 579;
static const int ID_VILLAGER_GOLD_F = 581;
static const int ID_VILLAGER_STONE_M = 124;
static const int ID_VILLAGER_STONE_F = 220;
static const int ID_VILLAGER_BUILDER_M = 118;
static const int ID_VILLAGER_BUILDER_F = 212;
static const int ID_VILLAGER_REPAIRER_M = 156;
static const int ID_VILLAGER_REPAIRER_F = 222;
static const int ID_SABOTEUR = 706;

static const int CLASS_TRANSPORT_BOAT = 20;
static const int ACTION_TYPE_GARRISON = 3;


void configureExplodingVillagers(genie::DatFile *df) {

    set<int> villagers{
            ID_FISHING_SHIP,
            ID_TRADE_COG,
            ID_TRADE_CART_EMPTY,
            ID_TRADE_CART_FULL,
            ID_VILLAGER_BASE_M,
            ID_VILLAGER_BASE_F,
            ID_VILLAGER_FARMER_M,
            ID_VILLAGER_FARMER_F,
            ID_VILLAGER_SHEPHERD_M,
            ID_VILLAGER_SHEPHERD_F,
            ID_VILLAGER_FORAGER_M,
            ID_VILLAGER_FORAGER_F,
            ID_VILLAGER_HUNTER_M,
            ID_VILLAGER_HUNTER_F,
            ID_VILLAGER_FISHER_M,
            ID_VILLAGER_FISHER_F,
            ID_VILLAGER_WOOD_M,
            ID_VILLAGER_WOOD_F,
            ID_VILLAGER_GOLD_M,
            ID_VILLAGER_GOLD_F,
            ID_VILLAGER_STONE_M,
            ID_VILLAGER_STONE_F,
            ID_VILLAGER_BUILDER_M,
            ID_VILLAGER_BUILDER_F,
            ID_VILLAGER_REPAIRER_M,
            ID_VILLAGER_REPAIRER_F,
    };

    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            if (villagers.find(unit.ID) != villagers.end()) {
                unit.DeadUnitID = ID_SABOTEUR;
                cout << "Patched Villager unit " << unit.ID << " for civ " << civ.Name << "\n";
            } else if (unit.ID == ID_SABOTEUR) {
                unit.HitPoints = 0;
                unit.Type50.Attacks.at(0).Amount = 50;
                unit.Type50.Attacks.at(1).Amount = 90;
                unit.Type50.Attacks.at(2).Amount = 0;
                unit.Type50.MaxRange = 2;
                unit.Type50.BlastAttackLevel = 1; // cut trees
                unit.TrainSound = -1;
                unit.WwiseTrainSoundID = 0; // prevent melee unit train sound from playing
                cout << "Patched Saboteur unit for civ " << civ.Name << "\n";
            }
        }
    }
}

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
