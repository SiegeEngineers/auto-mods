#include "rewarding_snipes.h"
#include "genie/dat/DatFile.h"
#include "ids.h"

void configureRewardingSnipes(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        genie::Unit &king = civ.Units.at(ID_KING);
        king.BloodUnitID = ID_RELIC_CART; // use blood unit so it does not conflict with exploding kings
        std::cout << "Patched King blood unit for civ " << civ.Name << "\n";

        genie::Unit &relicCart = civ.Units.at(ID_RELIC_CART);
        relicCart.FogVisibility = 1; //always visible
        relicCart.HitPoints = 30000; //don't die from exploding kings

        //this makes it not need houses if at max pop
        relicCart.ResourceStorages.at(0).Type = TYPE_POPULATION_HEADROOM;
        relicCart.ResourceStorages.at(0).Amount = 50; //reduce current pop by 50, avoids limits
        relicCart.ResourceStorages.at(0).Flag = TYPE_GIVE_AND_TAKE; //change on convert
                                                                    //
        //this makes it give -50 pop
        relicCart.ResourceStorages.at(1).Type = TYPE_BONUS_POPULATION_CAP;
        relicCart.ResourceStorages.at(1).Amount = 50; //reduce current pop by 50, avoids limits
        relicCart.ResourceStorages.at(1).Flag = TYPE_GIVE_AND_TAKE; //change on convert
        std::cout << "Patched Relic Cart for civ " << civ.Name << "\n";
    }
}
