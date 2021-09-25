#include <set>
#include "teamwork.h"
#include "genie/dat/DatFile.h"


void makeTechEffectsShared(genie::DatFile *df) {
    for (auto &effect: df->Effects) {
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
