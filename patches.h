#ifndef CREATE_DATA_MOD_PATCHES_H
#define CREATE_DATA_MOD_PATCHES_H

void configureCommunityGamesMod(genie::DatFile *df);
void configureExplodingVillagers(genie::DatFile *df);
void makeTransportShipsFly(genie::DatFile *df);
void duplicateTechs(genie::DatFile *df, int totalCount);
void configureKidnap(genie::DatFile *df);
void disableWalls(genie::DatFile *df);
void preventRamsAndSiegeTowersFromBoardingTransportShips(genie::DatFile *df);
void duplicateTech(genie::DatFile *df, const genie::Tech &tech, int totalCount);
void disableTechEffect(genie::Tech *tech);
void disableTechResearchLocation(genie::Tech *tech);
void addPopulationCostToBombardTower(genie::DatFile *df);
void addGreatHallTech(genie::DatFile *df);
void addElitePetard(genie::DatFile *df);
void modifyCaravanCost(genie::DatFile *df, int amountFood, int amountGold);
void makeTreesContain200Wood(genie::DatFile *df);

#endif //CREATE_DATA_MOD_PATCHES_H
