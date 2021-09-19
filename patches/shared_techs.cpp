#include <set>
#include "shared_techs.h"
#include "genie/dat/DatFile.h"


void makeTechEffectsShared(genie::DatFile *df) {
    for (const auto &tech: df->Techs) {
        if (tech.EffectID < 0 || isProbablyCivBonus(tech)) {
            std::cout << "Skipping tech " << tech.Name << std::endl;
            continue;
        }
        auto effect = df->Effects.at(tech.EffectID);
        if (shouldSkipEffect(effect)) {
            continue;
        }
        for (auto &command: effect.EffectCommands) {
            if (command.Type < 8) {
                uint8_t newType = command.Type + 10;
                std::cout << "Modifying command type from " << std::to_string(command.Type) << " to "
                          << std::to_string(newType) << " for effect " << effect.Name << std::endl;
                command.Type = newType;
            }
        }
    }
}

bool isProbablyCivBonus(const genie::Tech &tech) {
    return tech.Civ != -1
           && tech.ResearchLocation != -1
           && tech.ResearchTime == 0
           && techIsFree(tech.ResourceCosts);
}

bool techIsFree(const std::vector<ResearchResourceCost> &costs) {
    for (const auto &cost: costs) {
        if (cost.Amount != 0) {
            return false;
        }
    }
    return true;
}

bool shouldSkipEffect(genie::Effect &effect) {
    const std::set<std::string> allowedEffectNames = {
            "Anarchy",
            "Marauders",
            "Nomads",
            "Imperial Nomads",
            "Madrasah",
            "Thalassocracy",
            "Tatars UT 2",
            "Chemistry",
    };
    if (allowedEffectNames.find(effect.Name) != allowedEffectNames.end()) {
        return false;
    }
    for (auto &command: effect.EffectCommands) {
        if (command.Type == ENABLE_DISABLE_UNIT || command.Type == UPGRADE_UNIT) {
            std::cout << "Skipping effect " << effect.Name << std::endl;
            return true;
        }
    }
    return false;
}
