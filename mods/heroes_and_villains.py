import logging

from genieutils.civ import Civ
from genieutils.unit import Unit
from genieutils.datfile import DatFile
from genieutils.unit import ResourceCost, ResourceStorage
from genieutils.task import Task
from mods.util import clone
from mods.ids import RICHARD_THE_LIONHEART, TSAR_KONSTANTIN, BELISARIUS, WILLIAM_WALLACE, \
    WANG_TONG, THEODORIC_THE_GOTH, KUSHLUK, SHAH_ISHMAIL, SALADIN, SELIM_THE_GRIM, JARL, \
    ITZCOATL, ATTILA_THE_HUN, PACAL_II, EL_CID_CAMPEADOR, GENGHIS_KHAN, FRANCESCO_SFORZA, \
    MIKLOS_TOLDI, ALEXANDER_NEVSKI, TARIQ_IBN_ZIYAD, DAGNAJAN, SUMANGURU, BAYINNAUNG, \
    SURYAVARMAN_I, GAJAH_MADA, DINH_LE, KOTYAN_KHAN, VYTAUTAS_THE_GREAT, QUTLUGH, \
    JOHN_THE_FEARLESS, ROGER_BOSSO, JAN_ZIZKA, JADWIGA, IBRAHIM_LODI, PRITHVIRAJ, TAMAR, \
    THOROS, JOAN_OF_ARC, MINAMOTO, ULRICH_VON_JUNGINGEN, PACHACUTI, RAJENDRA_CHOLA, POPE_LEO_I, \
    TYPE_POPULATION_HEADROOM, TYPE_CURRENT_POPULATION, TYPE_TOTAL_UNITS_OWNED, TYPE_FOOD_STORAGE, \
    TYPE_GOLD_STORAGE, TYPE_CASTLE_TRAIN_LOCATION, TYPE_CURRENT_AGE

NAME = 'heroes-and-villains'

IGNORED_CIVS = ['Gaia', 'Achaemenids', 'Spartans', 'Athenians', 'Shu', 'Wei', 'Wu', "Khitans", 'Jurchens']

HERO_FOR_CIV = {
    "British": { "unitId": RICHARD_THE_LIONHEART, "unitStatChanges": {} },
    "Byzantine": { "unitId": BELISARIUS, "unitStatChanges": {} },
    "Celts": { "unitId": WILLIAM_WALLACE, "unitStatChanges": {} },
    "Chinese": { "unitId": WANG_TONG, "unitStatChanges": {} },
    "French": { "unitId": JOAN_OF_ARC, "unitStatChanges": {} },
    "Goths": { "unitId": THEODORIC_THE_GOTH, "unitStatChanges": {} },
    "Japanese": { "unitId": MINAMOTO, "unitStatChanges": {} },
    "Mongols": { "unitId": KUSHLUK, "unitStatChanges": {} },
                    #GENGHIS_KHAN is another option but not unique skin
    "Persians": { "unitId": SHAH_ISHMAIL, "unitStatChanges": {} },
    "Saracens": { "unitId": SALADIN, "unitStatChanges": {} },
    "Teutons": { "unitId": ULRICH_VON_JUNGINGEN, "unitStatChanges": {} },
    "Turks": { "unitId": SELIM_THE_GRIM, "unitStatChanges": {} },
    "Vikings": { "unitId": JARL, "unitStatChanges": {} },
    "Aztecs": { "unitId": ITZCOATL, "unitStatChanges": {} },
    "Huns": { "unitId": ATTILA_THE_HUN, "unitStatChanges": {} },
    "Koreans": { "unitId": None, "unitStatChanges": {} },
    "Mayan": { "unitId": PACAL_II, "unitStatChanges": {} },
    "Spanish": { "unitId": EL_CID_CAMPEADOR, "unitStatChanges": {} },
    "Incas": { "unitId": PACHACUTI, "unitStatChanges": {} },
     #"Indians": { "unitId": "unit_indians", "unitStatChanges": {} },
    "Italians": { "unitId": FRANCESCO_SFORZA, "unitStatChanges": {} },
    "Magyars": { "unitId": MIKLOS_TOLDI, "unitStatChanges": {} },
    "Slavs": { "unitId": ALEXANDER_NEVSKI, "unitStatChanges": {} },
    "Berbers": { "unitId": TARIQ_IBN_ZIYAD, "unitStatChanges": {} },
    "Ethiopians": { "unitId": DAGNAJAN, "unitStatChanges": {} },
    "Malians": { "unitId": SUMANGURU, "unitStatChanges": {} },
    "Portuguese": { "unitId": None, "unitStatChanges": {} },
    "Burmese": { "unitId": BAYINNAUNG, "unitStatChanges": {} },
    "Khmer": { "unitId": SURYAVARMAN_I, "unitStatChanges": {} },
    "Malay": { "unitId": GAJAH_MADA, "unitStatChanges": {} },
    "Vietnamese": { "unitId": DINH_LE, "unitStatChanges": {} },
    "Bulgarians": { "unitId": TSAR_KONSTANTIN, "unitStatChanges": {} },
    "Cumans": { "unitId": KOTYAN_KHAN, "unitStatChanges": {} },
    "Lithuanians": { "unitId": VYTAUTAS_THE_GREAT, "unitStatChanges": {} },
    "Tatars": { "unitId": QUTLUGH, "unitStatChanges": {} },
    "Burgundians": { "unitId": JOHN_THE_FEARLESS, "unitStatChanges": {} },
    "Sicilians": { "unitId": ROGER_BOSSO, "unitStatChanges": {} },
    "Bohemians": { "unitId": JAN_ZIZKA, "unitStatChanges": {} },
    "Poles": { "unitId": JADWIGA, "unitStatChanges": {} },
    "Hindustanis": { "unitId": IBRAHIM_LODI, "unitStatChanges": {} },
    "Bengalis": { "unitId": None, "unitStatChanges": {} },
    "Gurjaras": { "unitId": PRITHVIRAJ, "unitStatChanges": {} },
    "Dravidians": { "unitId": RAJENDRA_CHOLA, "unitStatChanges": {} },
    "Romans": { "unitId": POPE_LEO_I, "unitStatChanges": {} },
    "Armenians": { "unitId": THOROS, "unitStatChanges": {} },
    "Georgians": { "unitId": TAMAR, "unitStatChanges": {} }
}


def make_unit_trainable_in_castle_in_imperial_age(unit: Unit, civ: Civ):
    #make unit cost money
    unit.creatable.resource_costs = (
        ResourceCost(type=TYPE_FOOD_STORAGE, amount=500, flag=1),
        ResourceCost(type=TYPE_GOLD_STORAGE, amount=500, flag=1),
        #require imperial age
        ResourceCost(type=TYPE_CURRENT_AGE, amount=3, flag=0),
    )
    #make sure unit take up population space
    unit.resource_storages = (
        ResourceStorage(type=TYPE_POPULATION_HEADROOM, amount=-1, flag=2),
        ResourceStorage(type=TYPE_CURRENT_POPULATION, amount=1, flag=2),
        ResourceStorage(type=TYPE_TOTAL_UNITS_OWNED, amount=1, flag=1),
    )
    #make unit trainable in the castle
    unit.train_location = TYPE_CASTLE_TRAIN_LOCATION
    unit.train_button = 12
    unit.train_time = 30
    unit.hero_mode = 1
    
    #TODO make unit traininable only once if the unit is alive

    return unit


def makeHero(unitData: dict, civ: Civ, version: str) -> tuple[int, int]:
    #prevent_hp_increase(cloned_unit)
    logging.info(f'Patching hero unit {unitData["unitId"]}')
    clone_unit_id = len(civ.units)
    #clone the unit
    cloned_unit = clone(civ.units[unitData["unitId"]], version)
    #set id to be end of civ units
    cloned_unit.id = clone_unit_id


    #TODO balance out the units stats compared to new DLC
    cloned_unit.hit_points = 300

    #TODO make the unit buildable in imperial age at the castle
    trainable_unit = make_unit_trainable_in_castle_in_imperial_age(cloned_unit, civ)

    #add the new unit to the civ
    civ.units.append(trainable_unit)

    logging.info(f'Patched hero unit {cloned_unit.name} for civ {civ.name}')
    return len(civ.units) - 2, len(civ.units) - 1


def mod(data: DatFile):
    for civ in data.civs:
        if civ.name not in IGNORED_CIVS and HERO_FOR_CIV[civ.name]["unitId"] is not None:
            logging.info(f'Creating hero for civ {civ.name}')
            makeHero(HERO_FOR_CIV[civ.name], civ, data.version)
        else:
            logging.info(f'Failed to find hero for civ {civ.name}')
