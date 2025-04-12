import logging

from genieutils.civ import Civ
from genieutils.unit import Unit
from genieutils.effect import Effect, EffectCommand
from genieutils.tech import Tech, ResearchResourceCost
from genieutils.datfile import DatFile
from genieutils.unit import ResourceCost, ResourceStorage
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
    TYPE_ENABLE_DISABLE_UNIT, TECH_REQUIREMENT_IMPERIAL_AGE, TECH_CASTLE_BUILT

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

def addUnitToCiv(civ_id: int, unit_id: int, data: DatFile):
    logging.info(f'Making effect for hero unit for {data.civs[civ_id].units[unit_id].name} - {unit_id} for civ {data.civs[civ_id].name} - {civ_id}')
    effect_command = EffectCommand(
        type=TYPE_ENABLE_DISABLE_UNIT,
        a=unit_id,
        b=1,
        c=-1,
        d=0.0,
    )
    effect = Effect(
        name=f'Create Hero Unit for {data.civs[civ_id].units[unit_id].name}',
        effect_commands=[effect_command]
    )
    effect_id = len(data.effects)
    data.effects.append(effect)

    logging.info(f'Making tech for hero unit {data.civs[civ_id].units[unit_id].name} for civ {data.civs[civ_id].name} for effect {effect_id}')
    tech = Tech(
        required_techs=(TECH_REQUIREMENT_IMPERIAL_AGE, TECH_CASTLE_BUILT, -1, -1, -1, -1), #imperial age
        resource_costs=(
            ResearchResourceCost(type=-1, amount=0, flag=0),
            ResearchResourceCost(type=-1, amount=0, flag=0),
            ResearchResourceCost(type=-1, amount=0, flag=0)
        ),
        required_tech_count=2,
        civ=civ_id,
        full_tech_mode=1,
        research_location=-1,
        language_dll_name=0,
        language_dll_description=0,
        research_time=0,
        effect_id=effect_id,
        type=0,
        icon_id=-1,
        button_id=0,
        language_dll_help=0,
        language_dll_tech_tree=0,
        hot_key=-1,
        name=f'Make Hero Unit Available for {data.civs[civ_id].units[unit_id].name}',
        repeatable=1,
    )
    data.techs.append(tech)

def makeHero(unitData: dict, civ: Civ, data: DatFile) -> int:
    #prevent_hp_increase(cloned_unit)
    logging.info(f'Patching hero unit {unitData["unitId"]}')
    new_unit_id = len(civ.units)
    #clone the unit
    unit = clone(civ.units[unitData["unitId"]], data.version)
    #set id to be end of civ units
    unit.id = new_unit_id

    #TODO balance out the units stats compared to new DLC
    unit.hit_points = 500
    #make unit cost resources
    unit.creatable.resource_costs = (
        ResourceCost(type=TYPE_FOOD_STORAGE, amount=500, flag=1),
        ResourceCost(type=TYPE_GOLD_STORAGE, amount=500, flag=1),
        unit.creatable.resource_costs[2]
    )
    #make sure unit take up population space
    unit.resource_storages = (
        ResourceStorage(type=TYPE_POPULATION_HEADROOM, amount=-1, flag=2),
        ResourceStorage(type=TYPE_CURRENT_POPULATION, amount=1, flag=2),
        ResourceStorage(type=TYPE_TOTAL_UNITS_OWNED, amount=1, flag=1),
    )
    #make unit trainable in the castle
    unit.creatable.train_location_id = TYPE_CASTLE_TRAIN_LOCATION
    unit.creatable.button_id = 4
    unit.creatable.hero_mode = 1

    #TODO make unit trainable only one at a time with resources, kinda like praetorian

    #add the new unit to the civ
    civ.units.append(unit)
    logging.info(f'Patched hero unit {unit.name} for civ {civ.name}')
    return new_unit_id


def mod(data: DatFile):
    for civ_id, civ in enumerate(data.civs):
        if civ.name not in IGNORED_CIVS and HERO_FOR_CIV[civ.name]["unitId"] is not None:
            logging.info(f'Creating hero for civ {civ.name}')
            hero = HERO_FOR_CIV[civ.name]
            new_unit_id = makeHero(hero, civ, data)
            addUnitToCiv(civ_id, new_unit_id, data)
            logging.info(f'Creating hero for civ other civs')
#            for rest_of_civs_civ_id, rest_of_civs_civ in enumerate(data.civs):
#                if rest_of_civs_civ_id != civ_id:
#                    #make the hero for the other civs
#                    makeHero(hero, rest_of_civs_civ, data)
        else:
            logging.info(f'Failed to find hero for civ {civ.name}')
