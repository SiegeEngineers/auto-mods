import logging

from genieutils.civ import Civ
from genieutils.unit import Unit
from genieutils.effect import Effect, EffectCommand
from genieutils.tech import Tech
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
    TYPE_GOLD_STORAGE, TYPE_CASTLE_TRAIN_LOCATION, TYPE_CURRENT_AGE, EFFECT_THIRISIDAI_MAKE_AVIALABLE, \
    TYPE_ENABLE_DISABLE_UNIT

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


def makeUnitTrainableInCastle(unit: Unit, civ: Civ):
    #make unit cost money
    unit.creatable.resource_costs = (
        ResourceCost(type=TYPE_FOOD_STORAGE, amount=500, flag=1),
        ResourceCost(type=TYPE_GOLD_STORAGE, amount=500, flag=1),
        #require imperial age
        unit.creatable.resource_costs[2]
        #ResourceCost(type=TYPE_CURRENT_AGE, amount=3, flag=0),
    )
    #make sure unit take up population space
    unit.resource_storages = (
        ResourceStorage(type=TYPE_POPULATION_HEADROOM, amount=-1, flag=2),
        ResourceStorage(type=TYPE_CURRENT_POPULATION, amount=1, flag=2),
        ResourceStorage(type=TYPE_TOTAL_UNITS_OWNED, amount=1, flag=1),
    )
    #make unit trainable in the castle
    #print(unit)
    unit.train_location_id = TYPE_CASTLE_TRAIN_LOCATION
    unit.train_button = 4
    unit.hero_mode = 1
    
    #TODO make unit traininable only once if the unit is alive

    return unit

def addUnitToCiv(civ_id: int, unitId: int, data: DatFile):
    logging.info(f'Making effect for hero unit {data.civs[civ_id].units[unitId].name} for civ {data.civs[civ_id].name}')
    effect_command = EffectCommand(
        type=TYPE_ENABLE_DISABLE_UNIT,
        a=2,
        b=unitId,
        c=-1,
        d=0,
    )

    effect = Effect(
        name=f'Create Hero Unit {data.civs[civ_id].units[unitId].name}',
        effect_commands=[effect_command]
    )
    effect_id = len(data.effects)
    data.effects.append(effect)
    
    logging.info(f'Making tech for hero unit {data.civs[civ_id].units[unitId].name} for civ {data.civs[civ_id].name} for effect {effect_id}')
    #TODO create tech that activates "make available" that was just made require 103 imperial age
    tech = Tech(
        required_techs=(103, -1, -1, -1, -1, -1), #imperial age
        resource_costs=(),
        required_tech_count=1,
        civ=civ_id,
        full_tech_mode=1,
        research_location=-1,
        language_dll_name=0,
        language_dll_description=0,
        research_time=5,
        effect_id=effect_id,
        type=-1,
        icon_id=103,
        button_id=0,
        language_dll_help=0,
        language_dll_tech_tree=0,
        hot_key=0,
        name=f'Make Hero Unit Available {data.civs[civ_id].units[unitId].name}',
        repeatable=1,
    )
    data.techs.append(tech)

def makeHero(unitData: dict, civ: Civ, data: DatFile) -> int:
    #prevent_hp_increase(cloned_unit)
    logging.info(f'Patching hero unit {unitData["unitId"]}')
    clone_unit_id = len(civ.units)
    #clone the unit
    cloned_unit = clone(civ.units[unitData["unitId"]], data.version)
    #set id to be end of civ units
    cloned_unit.id = clone_unit_id


    #TODO balance out the units stats compared to new DLC
    cloned_unit.hit_points = 300

    #TODO make the unit buildable in imperial age at the castle
    trainable_unit = makeUnitTrainableInCastle(cloned_unit, civ)

    #add the new unit to the civ
    civ.units.append(trainable_unit)

    logging.info(f'Patched hero unit {cloned_unit.name} for civ {civ.name}')
    return clone_unit_id


def mod(data: DatFile):
    for civ_id, civ in enumerate(data.civs):
        if civ.name not in IGNORED_CIVS and HERO_FOR_CIV[civ.name]["unitId"] is not None:
            logging.info(f'Creating hero for civ {civ.name}')
            new_unit_id = makeHero(HERO_FOR_CIV[civ.name], civ, data)
            addUnitToCiv(civ_id, new_unit_id, data)
        else:
            logging.info(f'Failed to find hero for civ {civ.name}')
