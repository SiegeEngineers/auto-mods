#ifndef TEAMWORK_H
#define TEAMWORK_H

#include "genie/dat/DatFile.h"

static const int ENABLE_DISABLE_UNIT = 2;
static const int UPGRADE_UNIT = 3;

typedef genie::ResourceUsage<int16_t, int16_t, uint8_t> ResearchResourceCost;

void makeTechEffectsShared(genie::DatFile *df);

bool shouldSkipEffect(genie::Effect &effect);

#endif  // TEAMWORK_H
