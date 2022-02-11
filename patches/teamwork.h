#ifndef TEAMWORK_H
#define TEAMWORK_H

#include "genie/dat/DatFile.h"

static const int ENABLE_DISABLE_UNIT = 2;
static const int UPGRADE_UNIT = 3;
static const int FARMER_F = 214;
static const int FARMER_M = 259;
static const int FARM = 50;
static const int HORSE_COLLAR = 14;
static const int HEAVY_PLOW = 13;
static const int CROP_ROTATION = 12;


typedef genie::ResourceUsage<int16_t, int16_t, uint8_t> ResearchResourceCost;

void makeTechEffectsShared(genie::DatFile *df);

void makeFarmTechsBuffSpeed(genie::DatFile *df);

bool shouldSkipEffect(genie::Effect &effect);

#endif  // TEAMWORK_H
