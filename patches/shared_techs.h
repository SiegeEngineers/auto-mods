#ifndef SHARED_TECHS_H
#define SHARED_TECHS_H

#include "genie/dat/DatFile.h"

static const int ENABLE_DISABLE_UNIT = 2;
static const int UPGRADE_UNIT = 3;

typedef genie::ResourceUsage<int16_t, int16_t, uint8_t> ResearchResourceCost;

void makeTechEffectsShared(genie::DatFile *df);

bool isProbablyCivBonus(const genie::Tech &tech);

bool techIsFree(const std::vector<ResearchResourceCost> &costs);

bool shouldSkipEffect(genie::Effect &effect);

#endif  // SHARED_TECHS_h
