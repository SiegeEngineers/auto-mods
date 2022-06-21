#include "rewarding_snipes.h"
#include <set>
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureRewardingSnipes(genie::DatFile *df) {

    for (genie::Civ &civ : df->Civs) {
        //set the king to create a relic cart on death. 
        //note: blood unit id does not conflict with dead unit id
        genie::Unit &king = civ.Units.at(ID_KING);
        king.BloodUnitID = ID_RELIC_CART;
        std::cout << "Patched King unit for civ " << civ.Name << "\n";

        //set the relic cart to not die from exploding kings if doing both
        genie::Unit &relicCart = civ.Units.at(ID_RELIC_CART);
        relicCart.HitPoints = 30000; //don't die from exploding king
        relicCart.ResourceStorages.at(0).Type = TYPE_POPULATION_HEADROOM;
        relicCart.ResourceStorages.at(0).Amount = 50; //give 50 bonus pop
        relicCart.ResourceStorages.at(0).Flag = TYPE_GIVE_AND_TAKE;
        std::cout << "Patched Relic Cart for civ " << civ.Name << "\n";
    }
}
