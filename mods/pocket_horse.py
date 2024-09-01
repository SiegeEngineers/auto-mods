import logging

from genieutils.datfile import DatFile

from mods.ids import LONG_SWORDSMAN, TWO_HANDED_SWORDSMAN, CHAMPION, KNIGHT, CAVALIER, PALADIN, \
    SPEARMAN, PIKEMAN, HALBERDIER, CAMEL_SCOUT, CAMEL_RIDER, HEAVY_CAMEL_RIDER, IMPERIAL_CAMEL_RIDER, CROSSBOWMAN, \
    ARBALESTER, CAVALRY_ARCHER, HEAVY_CAV_ARCHER, ELITE_SKIRMISHER, IMPERIAL_SKIRMISHER, GENITOUR, ELITE_GENITOUR, \
    SCOUT_CAVALRY, LIGHT_CAVALRY, HUSSAR, WINGED_HUSSAR, EAGLE_SCOUT, EAGLE_WARRIOR, ELITE_EAGLE_WARRIOR, KAMAYUK, \
    ELITE_KAMAYUK, STEPPE_LANCER, ELITE_STEPPE_LANCER, SAMURAI, ELITE_SAMURAI, MOUNTED_SAMURAI, ELEPHANT_ARCHER, \
    ELITE_ELEPHANT_ARCHER, BATTLE_ELEPHANT, ELITE_BATTLE_ELEPHANT, HAND_CANNONEER, CONQUISTADOR, ELITE_CONQUISTADOR, \
    SCORPION, HEAVY_SCORPION, BALLISTA_ELEPHANT, ELITE_BALLISTA_ELEPHANT, GBETO, ELITE_GBETO, MAMELUKE, ELITE_MAMELUKE, \
    PETARD, FLAMING_CAMEL, BOMBARD_CANNON, HOUFNICE, CANNON_GALLEON, ELITE_CANNON_GALLEON

NAME = 'pocket-horse'


def configure_switch(data: DatFile, source: int, target: int):
    for civ in data.civs:
        civ.units[source].nothing = target
        civ.units[source].trait |= 8
    logging.info(f'Added Ratha Ability {source} ({data.civs[0].units[source].name}) '
                 f'→ {target} ({data.civs[0].units[target].name}) for all civs')


def configure_switches(data: DatFile):
    # Long Swordsman, Two-Handed Swordsman, Champion + ←→ Knight, Cavalier, Paladin
    configure_switch(data, LONG_SWORDSMAN, KNIGHT)
    configure_switch(data, TWO_HANDED_SWORDSMAN, KNIGHT)
    configure_switch(data, CHAMPION, KNIGHT)
    configure_switch(data, KNIGHT, LONG_SWORDSMAN)
    configure_switch(data, CAVALIER, LONG_SWORDSMAN)
    configure_switch(data, PALADIN, LONG_SWORDSMAN)

    # Spearman, Pikeman, Halberdier ←→ Camel Scout, Camel Rider, Heavy Camel Rider, Imperial Camel Rider
    configure_switch(data, SPEARMAN, CAMEL_SCOUT)
    configure_switch(data, PIKEMAN, CAMEL_SCOUT)
    configure_switch(data, HALBERDIER, CAMEL_SCOUT)
    configure_switch(data, CAMEL_SCOUT, SPEARMAN)
    configure_switch(data, CAMEL_RIDER, SPEARMAN)
    configure_switch(data, HEAVY_CAMEL_RIDER, SPEARMAN)
    configure_switch(data, IMPERIAL_CAMEL_RIDER, SPEARMAN)

    # Crossbowman, Arbalester ←→ Cavalry Archer, Heavy Cavalry Archer
    configure_switch(data, CROSSBOWMAN, CAVALRY_ARCHER)
    configure_switch(data, ARBALESTER, CAVALRY_ARCHER)
    configure_switch(data, CAVALRY_ARCHER, CROSSBOWMAN)
    configure_switch(data, HEAVY_CAV_ARCHER, CROSSBOWMAN)

    # Elite Skirmisher, Imperial Skirmisher ←→ Genitour, Elite Genitour
    configure_switch(data, ELITE_SKIRMISHER, GENITOUR)
    configure_switch(data, IMPERIAL_SKIRMISHER, GENITOUR)
    configure_switch(data, GENITOUR, ELITE_SKIRMISHER)
    configure_switch(data, ELITE_GENITOUR, ELITE_SKIRMISHER)

    # Scout Cavalry, Light Cavalry, Hussar, Winged Hussar ←→ Eagle Scout, Eagle Warrior, Elite Eagle Warrior
    configure_switch(data, SCOUT_CAVALRY, EAGLE_SCOUT)
    configure_switch(data, LIGHT_CAVALRY, EAGLE_SCOUT)
    configure_switch(data, HUSSAR, EAGLE_SCOUT)
    configure_switch(data, WINGED_HUSSAR, EAGLE_SCOUT)
    configure_switch(data, EAGLE_SCOUT, SCOUT_CAVALRY)
    configure_switch(data, EAGLE_WARRIOR, SCOUT_CAVALRY)
    configure_switch(data, ELITE_EAGLE_WARRIOR, SCOUT_CAVALRY)

    # Kamayuk, Elite Kamayuk ←→ Steppe Lancer, Elite Steppe Lancer
    configure_switch(data, KAMAYUK, STEPPE_LANCER)
    configure_switch(data, ELITE_KAMAYUK, STEPPE_LANCER)
    configure_switch(data, STEPPE_LANCER, KAMAYUK)
    configure_switch(data, ELITE_STEPPE_LANCER, KAMAYUK)

    # Samurai, Elite Samurai -> Mounted Samurai
    configure_switch(data, SAMURAI, MOUNTED_SAMURAI)
    configure_switch(data, ELITE_SAMURAI, MOUNTED_SAMURAI)
    configure_switch(data, MOUNTED_SAMURAI, SAMURAI)

    # Elephant Archer, Elite Elephant Archer ←→ Battle Elephant, Elite Battle
    configure_switch(data, ELEPHANT_ARCHER, BATTLE_ELEPHANT)
    configure_switch(data, ELITE_ELEPHANT_ARCHER, BATTLE_ELEPHANT)
    configure_switch(data, BATTLE_ELEPHANT, ELEPHANT_ARCHER)
    configure_switch(data, ELITE_BATTLE_ELEPHANT, ELEPHANT_ARCHER)

    # Hand Cannoneer ←→ Conquistador, Elite Conquistador
    configure_switch(data, HAND_CANNONEER, CONQUISTADOR)
    configure_switch(data, CONQUISTADOR, HAND_CANNONEER)
    configure_switch(data, ELITE_CONQUISTADOR, HAND_CANNONEER)

    # Scorpion, Heavy Scorpion ←→ Ballista Elephant, Elite Ballista Elephant
    configure_switch(data, SCORPION, BALLISTA_ELEPHANT)
    configure_switch(data, HEAVY_SCORPION, BALLISTA_ELEPHANT)
    configure_switch(data, BALLISTA_ELEPHANT, SCORPION)
    configure_switch(data, ELITE_BALLISTA_ELEPHANT, SCORPION)

    # Gbeto, Elite Gbeto ←→ Mameluke, Elite Mameluke
    configure_switch(data, GBETO, MAMELUKE)
    configure_switch(data, ELITE_GBETO, MAMELUKE)
    configure_switch(data, MAMELUKE, GBETO)
    configure_switch(data, ELITE_MAMELUKE, GBETO)

    # Petard ←→ Flaming Camel
    configure_switch(data, PETARD, FLAMING_CAMEL)
    configure_switch(data, FLAMING_CAMEL, PETARD)

    # Bombard Cannon, Houfnice ←→ Cannon Galleon, Elite Cannon Galleon # lol?
    configure_switch(data, BOMBARD_CANNON, CANNON_GALLEON)
    configure_switch(data, HOUFNICE, CANNON_GALLEON)
    configure_switch(data, CANNON_GALLEON, BOMBARD_CANNON)
    configure_switch(data, ELITE_CANNON_GALLEON, BOMBARD_CANNON)


def mod(data: DatFile):
    configure_switches(data)
