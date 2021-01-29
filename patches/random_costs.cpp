#include "random_costs.h"
#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include "genie/dat/DatFile.h"
#include "ids.h"


typedef genie::ResourceUsage<int16_t, int16_t, int16_t> ResourceCost;
typedef genie::ResourceUsage<int16_t, int16_t, int8_t> ResearchResourceCost;


bool isNaturalResourceCost(const ResourceCost &cost) { return cost.Type != -1 && cost.Type < 4 && cost.Amount > 0; }


bool isNaturalResearchResourceCost(const ResearchResourceCost &cost) {
    return cost.Type != -1 && cost.Type < 4 && cost.Amount > 0;
}


bool hasNaturalResourceCost(const genie::Unit &unit) {
    std::vector<ResourceCost> costs = unit.Creatable.ResourceCosts;
    return std::any_of(costs.begin(), costs.end(), isNaturalResourceCost);
}


bool hasNaturalResearchResourceCost(std::vector<ResearchResourceCost> costs) {
    return std::any_of(costs.begin(), costs.end(), isNaturalResearchResourceCost);
}


int getSumOfNaturalResourceCosts(const std::vector<ResourceCost> &resourceCosts) {
    int16_t sum = 0;
    for (const ResourceCost &cost : resourceCosts) {
        if (cost.Type > -1 && cost.Type < 4) {
            sum += cost.Amount;
        }
    }
    return sum;
}


ResearchResourceCost toResearchResourceCost(const ResourceCost &resourceCost) {
    ResearchResourceCost researchResourceCost;
    researchResourceCost.Type = resourceCost.Type;
    researchResourceCost.Amount = resourceCost.Amount;
    researchResourceCost.Flag = (bool)resourceCost.Flag;
    return researchResourceCost;
}


ResourceCost toResourceCost(const ResearchResourceCost &researchResourceCost) {
    ResourceCost resourceCost;
    resourceCost.Type = researchResourceCost.Type;
    resourceCost.Amount = researchResourceCost.Amount;
    resourceCost.Flag = (bool)researchResourceCost.Flag;
    return resourceCost;
}


std::vector<ResearchResourceCost> toResearchResourceCosts(const std::vector<ResourceCost> &resourceCosts) {
    std::vector<ResearchResourceCost> researchResourceCosts;
    researchResourceCosts.reserve(resourceCosts.size());
    for (const ResourceCost &resourceCost : resourceCosts) {
        researchResourceCosts.push_back(toResearchResourceCost(resourceCost));
    }
    return researchResourceCosts;
}


std::vector<ResourceCost> toResourceCosts(const std::vector<ResearchResourceCost> &researchResourceCosts) {
    std::vector<ResourceCost> resourceCosts;
    resourceCosts.reserve(researchResourceCosts.size());
    for (const ResearchResourceCost &researchResourceCost : researchResourceCosts) {
        resourceCosts.push_back(toResourceCost(researchResourceCost));
    }
    return resourceCosts;
}


std::string costToString(const std::vector<ResourceCost> &costs) {
    std::string s;
    for (const ResourceCost &cost : costs) {
        if (cost.Flag == 1) {
            s += std::to_string(cost.Amount);
            s += " ";
            switch (cost.Type) {
            case TYPE_FOOD:
                s += "Food ";
                break;
            case TYPE_WOOD:
                s += "Wood ";
                break;
            case TYPE_GOLD:
                s += "Gold ";
                break;
            case TYPE_STONE:
                s += "Stone ";
                break;
            case TYPE_POPULATION_HEADROOM:
                s += "Pop ";
                break;
            default:
                s += "vat?";
            }
        }
    }
    return s;
}


std::string costToString(const std::vector<ResearchResourceCost> &costs) {
    std::string s;
    for (const ResearchResourceCost &cost : costs) {
        if (cost.Flag == 1) {
            s += std::to_string(cost.Amount);
            s += " ";
            switch (cost.Type) {
            case TYPE_FOOD:
                s += "Food ";
                break;
            case TYPE_WOOD:
                s += "Wood ";
                break;
            case TYPE_GOLD:
                s += "Gold ";
                break;
            case TYPE_STONE:
                s += "Stone ";
                break;
            case TYPE_POPULATION_HEADROOM:
                s += "Pop ";
                break;
            default:
                s += "vat?";
            }
        }
    }
    return s;
}


bool bothRequirePopulationHeadroom(int unitId, std::vector<ResourceCost> &resourceCosts, const genie::Civ &civ) {
    return ((civ.Units.at(unitId).Creatable.ResourceCosts.at(2).Type == TYPE_POPULATION_HEADROOM &&
             resourceCosts.at(2).Type == TYPE_POPULATION_HEADROOM) ||
            (civ.Units.at(unitId).Creatable.ResourceCosts.at(2).Type != TYPE_POPULATION_HEADROOM &&
             resourceCosts.at(2).Type != TYPE_POPULATION_HEADROOM));
}


void copyResourceCostAt(int unitId, int index, std::vector<ResourceCost> &target, const genie::Civ &civ) {
    genie::ResourceUsage<int16_t, int16_t, int16_t> source = civ.Units.at(unitId).Creatable.ResourceCosts.at(index);
    target.at(index).Type = source.Type;
    target.at(index).Amount = source.Amount;
    target.at(index).Flag = source.Flag;
}


void jumbleCosts(genie::DatFile *df) {
    std::vector<int> unitIds;
    for (genie::Unit unit : df->Civs.at(0).Units) {
        if (hasNaturalResourceCost(unit)) {
            unitIds.push_back(unit.ID);
        }
    }

    std::vector<int> techIds;
    size_t index = 0;
    for (const genie::Tech &tech : df->Techs) {
        std::vector<ResearchResourceCost> resourceCopy = tech.ResourceCosts;
        if (hasNaturalResearchResourceCost(resourceCopy)) {
            techIds.push_back(index);
        }
        index++;
    }

    std::vector<std::vector<ResourceCost>> allTheCosts;
    for (int unitId : unitIds) {
        std::vector<ResourceCost> resourceCopy = df->Civs.at(0).Units.at(unitId).Creatable.ResourceCosts;
        allTheCosts.push_back(resourceCopy);
    }

    for (int techId : techIds) {
        std::vector<ResourceCost> resourceCopy = toResourceCosts(df->Techs.at(techId).ResourceCosts);
        allTheCosts.push_back(resourceCopy);
    }

    for (int i = 0; i < 10; i++) {
        unsigned int seed = std::random_device()();
        std::cout << "Seed for shuffling unit and tech costs is: " << std::to_string(seed) << std::endl;
        shuffle(allTheCosts.begin(), allTheCosts.end(), std::mt19937(seed));

        size_t costIndex = 0;
        for (int techId : techIds) {
            std::vector<ResearchResourceCost> researchResourceCosts =
                toResearchResourceCosts(allTheCosts.at(costIndex));
            std::cout << "Setting cost of tech with id " << techId << " to " << costToString(researchResourceCosts)
                      << std::endl;
            df->Techs.at(techId).ResourceCosts = researchResourceCosts;
            costIndex++;
        }
        for (int unitId : unitIds) {
            std::vector<ResourceCost> &resourceCosts = allTheCosts.at(costIndex);
            std::cout << "Setting cost of unit with id " << unitId << " to " << costToString(resourceCosts)
                      << std::endl;
            if (!bothRequirePopulationHeadroom(unitId, resourceCosts, df->Civs.at(0))) {
                copyResourceCostAt(unitId, 2, resourceCosts, df->Civs.at(0));
            }
            for (genie::Civ &civ : df->Civs) {
                civ.Units.at(unitId).Creatable.ResourceCosts = resourceCosts;
            }
            costIndex++;
        }

        int maleVillagerCost =
            getSumOfNaturalResourceCosts(df->Civs.at(0).Units.at(ID_VILLAGER_BASE_M).Creatable.ResourceCosts);
        int femaleVillagerCost =
            getSumOfNaturalResourceCosts(df->Civs.at(0).Units.at(ID_VILLAGER_BASE_F).Creatable.ResourceCosts);
        if (maleVillagerCost <= 200 && femaleVillagerCost <= 200) {
            return;
        }
        std::cout << "Villagers are too expensive, reshuffling…" << std::endl;
    }
    std::cout << "Giving up, villagers stay expensive, sorry." << std::endl;
}


void jumbleUnitCosts(genie::DatFile *df) {
    std::vector<int> unitIds;

    for (genie::Unit unit : df->Civs.at(0).Units) {
        if (hasNaturalResourceCost(unit)) {
            unitIds.push_back(unit.ID);
        }
    }

    std::vector<std::vector<ResourceCost>> allTheCosts;
    for (int unitId : unitIds) {
        std::vector<ResourceCost> resourceCopy = df->Civs.at(0).Units.at(unitId).Creatable.ResourceCosts;
        allTheCosts.push_back(resourceCopy);
    }

    for (int i = 0; i < 10; i++) {
        unsigned int seed = std::random_device()();
        std::cout << "Seed for shuffling unit costs is: " << std::to_string(seed) << std::endl;
        std::shuffle(allTheCosts.begin(), allTheCosts.end(), std::mt19937(seed));

        size_t index = 0;
        for (int unitId : unitIds) {
            std::vector<ResourceCost> &resourceCosts = allTheCosts.at(index);
            std::cout << "Setting cost of unit with id " << unitId << " to " << costToString(resourceCosts)
                      << std::endl;
            if (!bothRequirePopulationHeadroom(unitId, resourceCosts, df->Civs.at(0))) {
                copyResourceCostAt(unitId, 2, resourceCosts, df->Civs.at(0));
            }
            for (genie::Civ &civ : df->Civs) {
                civ.Units.at(unitId).Creatable.ResourceCosts = resourceCosts;
            }
            index++;
        }

        int maleVillagerCost =
            getSumOfNaturalResourceCosts(df->Civs.at(0).Units.at(ID_VILLAGER_BASE_M).Creatable.ResourceCosts);
        int femaleVillagerCost =
            getSumOfNaturalResourceCosts(df->Civs.at(0).Units.at(ID_VILLAGER_BASE_F).Creatable.ResourceCosts);
        if (maleVillagerCost <= 200 && femaleVillagerCost <= 200) {
            return;
        }
        std::cout << "Villagers are too expensive, reshuffling…" << std::endl;
    }
    std::cout << "Giving up, villagers stay expensive, sorry." << std::endl;
}


void jumbleTechCosts(genie::DatFile *df) {
    std::vector<int> techIds;

    std::vector<std::vector<ResearchResourceCost>> allTheCosts;
    size_t index = 0;
    for (const genie::Tech &tech : df->Techs) {
        std::vector<ResearchResourceCost> resourceCopy = tech.ResourceCosts;
        if (hasNaturalResearchResourceCost(resourceCopy)) {
            allTheCosts.push_back(resourceCopy);
            techIds.push_back(index);
        }
        index++;
    }

    unsigned int seed = std::random_device()();
    std::cout << "Seed for shuffling tech costs is: " << std::to_string(seed) << std::endl;
    shuffle(allTheCosts.begin(), allTheCosts.end(), std::mt19937(seed));

    index = 0;
    for (size_t techId : techIds) {
        std::vector<ResearchResourceCost> &resourceCosts = allTheCosts.at(index);
        std::cout << "Setting cost of tech with id " << techId << " to " << costToString(resourceCosts) << std::endl;
        df->Techs.at(techId).ResourceCosts = resourceCosts;
        index++;
    }
}
