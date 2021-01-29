#include <set>
#include "genie/dat/DatFile.h"
#include "ids.h"


void makeTransportShipsFly(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        genie::Unit &transport_ship = civ.Units.at(ID_TRANSPORT_SHIP);
        transport_ship.FlyMode = 1; // true
        transport_ship.TerrainRestriction = 0; // All
        transport_ship.CollisionSize = {0, 0, 0}; // walk through everything, like flying
        std::cout << "Patched Transport Ship unit for civ " << civ.Name << "\n";
    }
}
