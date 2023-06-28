#include "double_costs.h"
#include <vector>
#include "genie/dat/DatFile.h"


typedef genie::ResourceUsage<int16_t, int16_t, int16_t> ResourceCost;
typedef genie::ResourceUsage<int16_t, int16_t, uint8_t> ResearchResourceCost;


bool isNaturalResourceCostX(const ResourceCost &cost) {
    return cost.Type != -1 && cost.Type < 4 && cost.Amount > 0 && cost.Flag == 1;
}


bool isNaturalResearchResourceCostX(const ResearchResourceCost &cost) {
    return cost.Type != -1 && cost.Type < 4 && cost.Amount > 0 && cost.Flag == 1;
}

void doubleCosts(genie::DatFile *df) {

    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            for (auto &item : unit.Creatable.ResourceCosts) {
                if (isNaturalResourceCostX(item)) {
                    item.Amount = (item.Amount + item.Amount);
                }
            }
        }
    }

    for (auto &tech : df->Techs) {
        for (auto &item : tech.ResourceCosts) {
            if (isNaturalResearchResourceCostX(item)) {
                item.Amount = (item.Amount + item.Amount);
            }
        }
    }
    std::cout << "Doubled all resource costs\n";
}
