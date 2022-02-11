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

void makeFarmTechsBuffSpeed(genie::DatFile *df) {
    std::vector<int16_t> unitIds = {FARMER_F, FARMER_M, FARM};
    std::vector<int16_t> effectIds = {HORSE_COLLAR, HEAVY_PLOW, CROP_ROTATION};

    for (const auto &effectId: effectIds) {
        for (const auto &unitId: unitIds) {
            auto effectCommand = new genie::EffectCommand();
            effectCommand->Type = 5;  // Attribute Modifier (Multiply)
            effectCommand->A = unitId;
            effectCommand->B = -1;
            effectCommand->C = 13;     // Work Rate
            effectCommand->D = 1.1;    // Amount *
            df->Effects.at(effectId).EffectCommands.push_back(*effectCommand);
            std::cout << "Adding EffectCommand to effect " << effectId << " to multiply work rate of unit " << unitId
                      << " by 1.1" << std::endl;
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
            "Huns Tech Tree",
    };
    const std::set<std::string> forbiddenEffectNames = {
            "Walls HP castle age",
            "Palisade Walls HP feudal age",
    };
    if (allowedEffectNames.find(effect.Name) != allowedEffectNames.end()) {
        return false;
    }
    if (forbiddenEffectNames.find(effect.Name) != forbiddenEffectNames.end()) {
        return true;
    }
    for (auto &command: effect.EffectCommands) {
        if (command.Type == ENABLE_DISABLE_UNIT || command.Type == UPGRADE_UNIT) {
            std::cout << "Skipping effect " << effect.Name << std::endl;
            return true;
        }
    }
    return false;
}
