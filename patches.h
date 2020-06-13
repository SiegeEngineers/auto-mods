#ifndef CREATE_DATA_MOD_PATCHES_H
#define CREATE_DATA_MOD_PATCHES_H

void configureExplodingVillagers(genie::DatFile *df);
void makeTransportShipsFly(genie::DatFile *df);
void duplicateTechs(genie::DatFile *df, int totalCount);
void disableWalls(genie::DatFile *df);
void preventRamsAndSiegeTowersFromBoardingTransportShips(genie::DatFile *df);
void duplicateTech(genie::DatFile *df, const genie::Tech &tech, int totalCount);
void disableTechEffect(genie::Tech *tech);
void disableTechResearchLocation(genie::Tech *tech);

#endif //CREATE_DATA_MOD_PATCHES_H
