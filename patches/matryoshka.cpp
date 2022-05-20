#include "matryoshka.h"
#include <algorithm>
#include <map>
#include <set>
#include "genie/dat/DatFile.h"

bool isUnitUpgrade(const genie::EffectCommand &effectCommand) {
    return effectCommand.Type == 3 && effectCommand.C == -1;
}

bool isUnit(const genie::DatFile *df, int16_t unitId) { return df->Civs.at(0).Units.at(unitId).Type == 70; }

bool affectsUnits(const genie::DatFile *df, const genie::EffectCommand &effectCommand) {
    return isUnit(df, effectCommand.A) && isUnit(df, effectCommand.B);
}

void setUnitAvailability(genie::Effect &effect, int16_t unitId, bool available) {
    auto command = genie::EffectCommand();
    command.Type = 2;
    command.A = unitId;
    command.B = available ? 1 : 0;
    effect.EffectCommands.push_back(command);
}

void disableUnit(genie::Effect &effect, int16_t unitId) { setUnitAvailability(effect, unitId, false); }

void enableUnit(genie::Effect &effect, int16_t unitId) { setUnitAvailability(effect, unitId, true); }

void rememberPair(std::map<int16_t, std::set<int16_t>> &mapping, int16_t lower, int16_t higher) {
    mapping[higher].insert(lower);
    // higher: [lower]
}

void setDeadIdsFromPairs(genie::DatFile *df, std::map<int16_t, std::set<int16_t>> mapping) {
    for (int i = 0; i < 10; i++) {
        std::vector<int16_t> toDelete;
        for (auto const &[higher, lowers] : mapping) {
            if (lowers.size() == 1) {
                int16_t lower = *lowers.begin();
                for (auto &civ : df->Civs) {
                    civ.Units.at(higher).DeadUnitID = lower;
                }
                std::cout << "Setting DeadUnitID of unit " << higher << " to " << lower << std::endl;
                toDelete.push_back(lower);
            }
        }
        for (auto &[higher, lowers] : mapping) {
            for (auto &item : toDelete) {
                lowers.erase(item);
            }
        }
    }
}

void applyMatryoshkaLogic(genie::DatFile *df) {
    std::map<int16_t, std::set<int16_t>> mapping;
    for (auto &effect : df->Effects) {
        std::set<int16_t> toDisable;
        std::set<int16_t> toEnable;
        for (auto &command : effect.EffectCommands) {
            if (isUnitUpgrade(command) && affectsUnits(df, command)) {
                command.C = 1;
                toDisable.insert(command.A);
                toEnable.insert(command.B);
                rememberPair(mapping, command.A, command.B);
            }
        }
        for (const auto &unitId : toDisable) {
            disableUnit(effect, unitId);
        }
        for (const auto &unitId : toEnable) {
            enableUnit(effect, unitId);
        }
    }
    setDeadIdsFromPairs(df, mapping);
}
