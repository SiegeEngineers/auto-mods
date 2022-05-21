#include "pocket_horse.h"
#include "genie/dat/DatFile.h"
#include "ids.h"


void configureSwitch(genie::DatFile *df, int16_t source, int16_t target) {
    for (genie::Civ &civ : df->Civs) {
        civ.Units.at(source).Nothing = target;
        civ.Units.at(source).Trait = civ.Units.at(source).Trait | 8;
    }
}

void setPocketHorseIds(genie::DatFile *df) {
    // Long Swordsman, Two-Handed Swordsman, Champion + ←→ Knight, Cavalier, Paladin
    configureSwitch(df, LONG_SWORDSMAN, KNIGHT);
    configureSwitch(df, TWO_HANDED_SWORDSMAN, KNIGHT);
    configureSwitch(df, CHAMPION, KNIGHT);
    configureSwitch(df, KNIGHT, LONG_SWORDSMAN);
    configureSwitch(df, CAVALIER, LONG_SWORDSMAN);
    configureSwitch(df, PALADIN, LONG_SWORDSMAN);

    // Spearman, Pikeman, Halberdier ←→ Camel Scout, Camel Rider, Heavy Camel Rider, Imperial Camel Rider
    configureSwitch(df, SPEARMAN, CAMEL_SCOUT);
    configureSwitch(df, PIKEMAN, CAMEL_SCOUT);
    configureSwitch(df, HALBERDIER, CAMEL_SCOUT);
    configureSwitch(df, CAMEL_SCOUT, SPEARMAN);
    configureSwitch(df, CAMEL_RIDER, SPEARMAN);
    configureSwitch(df, HEAVY_CAMEL_RIDER, SPEARMAN);
    configureSwitch(df, IMPERIAL_CAMEL_RIDER, SPEARMAN);

    // Crossbowman, Arbalester ←→ Cavalry Archer, Heavy Cavalry Archer
    configureSwitch(df, CROSSBOWMAN, CAVALRY_ARCHER);
    configureSwitch(df, ARBALESTER, CAVALRY_ARCHER);
    configureSwitch(df, CAVALRY_ARCHER, CROSSBOWMAN);
    configureSwitch(df, HEAVY_CAV_ARCHER, CROSSBOWMAN);

    // Elite Skirmisher, Imperial Skirmisher ←→ Genitour, Elite Genitour
    configureSwitch(df, ELITE_SKIRMISHER, GENITOUR);
    configureSwitch(df, IMPERIAL_SKIRMISHER, GENITOUR);
    configureSwitch(df, GENITOUR, ELITE_SKIRMISHER);
    configureSwitch(df, ELITE_GENITOUR, ELITE_SKIRMISHER);

    // Scout Cavalry, Light Cavalry, Hussar, Winged Hussar ←→ Eagle Scout, Eagle Warrior, Elite Eagle Warrior
    configureSwitch(df, SCOUT_CAVALRY, EAGLE_SCOUT);
    configureSwitch(df, LIGHT_CAVALRY, EAGLE_SCOUT);
    configureSwitch(df, HUSSAR, EAGLE_SCOUT);
    configureSwitch(df, WINGED_HUSSAR, EAGLE_SCOUT);
    configureSwitch(df, EAGLE_SCOUT, SCOUT_CAVALRY);
    configureSwitch(df, EAGLE_WARRIOR, SCOUT_CAVALRY);
    configureSwitch(df, ELITE_EAGLE_WARRIOR, SCOUT_CAVALRY);

    // Kamayuk, Elite Kamayuk ←→ Steppe Lancer, Elite Steppe Lancer
    configureSwitch(df, KAMAYUK, STEPPE_LANCER);
    configureSwitch(df, ELITE_KAMAYUK, STEPPE_LANCER);
    configureSwitch(df, STEPPE_LANCER, KAMAYUK);
    configureSwitch(df, ELITE_STEPPE_LANCER, KAMAYUK);

    // Elephant Archer, Elite Elephant Archer ←→ Battle Elephant, Elite Battle
    configureSwitch(df, ELEPHANT_ARCHER, BATTLE_ELEPHANT);
    configureSwitch(df, ELITE_ELEPHANT_ARCHER, BATTLE_ELEPHANT);
    configureSwitch(df, BATTLE_ELEPHANT, ELEPHANT_ARCHER);
    configureSwitch(df, ELITE_BATTLE_ELEPHANT, ELEPHANT_ARCHER);

    // Hand Cannoneer ←→ Conquistador, Elite Conquistador
    configureSwitch(df, HAND_CANNONEER, CONQUISTADOR);
    configureSwitch(df, CONQUISTADOR, HAND_CANNONEER);
    configureSwitch(df, ELITE_CONQUISTADOR, HAND_CANNONEER);

    // Scorpion, Heavy Scorpion ←→ Ballista Elephant, Elite Ballista Elephant
    configureSwitch(df, SCORPION, BALLISTA_ELEPHANT);
    configureSwitch(df, HEAVY_SCORPION, BALLISTA_ELEPHANT);
    configureSwitch(df, BALLISTA_ELEPHANT, SCORPION);
    configureSwitch(df, ELITE_BALLISTA_ELEPHANT, SCORPION);

    // Gbeto, Elite Gbeto ←→ Mameluke, Elite Mameluke
    configureSwitch(df, GBETO, MAMELUKE);
    configureSwitch(df, ELITE_GBETO, MAMELUKE);
    configureSwitch(df, MAMELUKE, GBETO);
    configureSwitch(df, ELITE_MAMELUKE, GBETO);

    // Petard ←→ Flaming Camel
    configureSwitch(df, PETARD, FLAMING_CAMEL);
    configureSwitch(df, FLAMING_CAMEL, PETARD);

    // Bombard Cannon, Houfnice ←→ Cannon Galleon, Elite Cannon Galleon // lol?
    configureSwitch(df, BOMBARD_CANNON, CANNON_GALLEON);
    configureSwitch(df, HOUFNICE, CANNON_GALLEON);
    configureSwitch(df, CANNON_GALLEON, BOMBARD_CANNON);
    configureSwitch(df, ELITE_CANNON_GALLEON, BOMBARD_CANNON);
}
