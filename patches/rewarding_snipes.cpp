#include "rewarding_snipes.h"
#include <set>
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureRewardingSnipes(genie::DatFile *df) {

    for (genie::Civ &civ : df->Civs) {
        //set the king to create a relic cart on death. 
        //note: blood unit id does not conflict with dead unit id
        genie::Unit &king = civ.Units.at(ID_KING);
        king.BloodUnitId = ID_RELIC_CART;
        std::cout << "Patched King unit for civ " << civ.Name << "\n";

        //set the relic cart to not die from exploding kings if doing both
        genie::Unit &relicCart = civ.Units.at(ID_RELIC_CART);
        relicCart.HitPoints = 30000; //don't die from exploding king
        relicCart.DeadUnitID = ID_FURIOUS_THE_MONKEY_BOY;
        relicCart.ResourceStorages.at(0).Type = 4; //set resource to population headroom
        relicCart.ResourceStorages.at(0).Amount = 50; //give 50 population
        relicCart.ResourceStorages.at(0).StoreMode = 2; //have population come go with control
        std::cout << "Patched Relic Cart for civ " << civ.Name << "\n";
    }
}
