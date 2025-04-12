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
    TYPE_ENABLE_DISABLE_UNIT, TECH_REQUIREMENT_IMPERIAL_AGE, TECH_CASTLE_BUILT, \
    TYPE_SPAWN_CAP, TYPE_UNIT_LIMIT_RESOURCE, TYPE_POPULATION_HEADROOM, TYPE_TOTAL_UNITS_OWNED, \
    SPECIAL_UNIT_SPAWN_BASILIEUS, TYPE_SPAWN_UNIT, TOWN_CENTER, TYPE_TOWN_CENTER_BUILT, SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD

NAME = 'heroes-and-villains'

IGNORED_CIVS = ['Gaia', 'Achaemenids', 'Spartans', 'Athenians', 'Shu', 'Wei', 'Wu', "Khitans", 'Jurchens']

#TODO maybe allow for multiple heroes for each civ, 
#TODO setup naval heroes as well (would give koreans and portugese something only leaving out Bengalis)
#TODO setup some better stats for balancing.
#TODO maybe give auras to units
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
    #create unit
    logging.info(f'Making effect for hero unit for {data.civs[civ_id].units[unit_id].name} - {unit_id} for civ {data.civs[civ_id].name} - {civ_id}')
    enable_hero_unit_effect_command = EffectCommand(
        type=TYPE_ENABLE_DISABLE_UNIT,
        a=unit_id,
        b=1,
        c=-1,
        d=0.0,
    )

    #create effect to enable unit
    enable_hero_unit_effect = Effect(
        name=f'Create Hero Unit for {data.civs[civ_id].units[unit_id].name}',
        effect_commands=[enable_hero_unit_effect_command]
    )
    enable_hero_unit_effect_id = len(data.effects)
    data.effects.append(enable_hero_unit_effect)

    #create tech to trigger effect
    logging.info(f'Making tech for hero unit {data.civs[civ_id].units[unit_id].name} for civ {data.civs[civ_id].name} for effect {enable_hero_unit_effect_id}')
    enable_hero_tech = Tech(
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
        effect_id=enable_hero_unit_effect_id,
        type=0,
        icon_id=-1,
        button_id=0,
        language_dll_help=0,
        language_dll_tech_tree=0,
        hot_key=-1,
        name=f'Make Hero Unit Available for {data.civs[civ_id].units[unit_id].name}',
        repeatable=1,
    )
    data.techs.append(enable_hero_tech)


def limitCivToOneHero(civ_id: int, unit_id: int, data: DatFile):
    logging.info(f'Making effect for hero unit limit for {data.civs[civ_id].units[unit_id].name} - {unit_id} for civ {data.civs[civ_id].name} - {civ_id}')
    limit_unit_creatable_effect_command = EffectCommand(
        type=1,
        a=TYPE_SPAWN_CAP,
        b=0,
        c=-1,
        d=1.0,
    )

    basilius_clone_id = len(data.civs[civ_id].units)
    #TODO when working change to basilius
    basilius_clone = clone(data.civs[civ_id].units[SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD], data.version)
    basilius_clone.id = basilius_clone_id
    data.civs[civ_id].units.append(basilius_clone)
    #create a dead basilieus at start to give one of needed resource for the unit. This one gives instant resource 501 when it dies instead of delayed
    create_basilius_effect_command = EffectCommand(type=TYPE_SPAWN_UNIT, a=basilius_clone_id, b=TOWN_CENTER, c=1, d=0.0)

    limit_unit_creatable_effect = Effect(
        name=f'Limit Hero Amount Effect for {data.civs[civ_id].units[unit_id].name}',
        effect_commands=[limit_unit_creatable_effect_command, create_test, create_basilius_effect_command]
    )

    limit_unit_creatable_effect_id = len(data.effects)
    data.effects.append(limit_unit_creatable_effect)

    logging.info(f'Making tech for hero unit limit for {data.civs[civ_id].units[unit_id].name} - {unit_id} for civ {data.civs[civ_id].name} - {civ_id}')
    limit_hero_unit_creatable_effect = Tech(
        required_techs=(TECH_REQUIREMENT_IMPERIAL_AGE, TYPE_TOWN_CENTER_BUILT, -1, -1, -1, -1),
        resource_costs=(
            ResearchResourceCost(type=-1, amount=0, flag=0),
            ResearchResourceCost(type=-1, amount=0, flag=0),
            ResearchResourceCost(type=-1, amount=0, flag=0)
        ),
        required_tech_count=2,
        civ=civ_id,
        full_tech_mode=0,
        research_location=-1,
        language_dll_name=0,
        language_dll_description=0,
        research_time=0,
        effect_id=limit_unit_creatable_effect_id,
        type=0,
        icon_id=-1,
        button_id=0,
        language_dll_help=0,
        language_dll_tech_tree=0,
        hot_key=-1,
        name=f'Limit Hero Unit amount for {data.civs[civ_id].units[unit_id].name}',
        repeatable=0,
    )
    data.techs.append(limit_hero_unit_creatable_effect)


def makeHero(unitData: dict, civ: Civ, data: DatFile) -> int:
    #prevent_hp_increase(cloned_unit)
    logging.info(f'Patching hero unit {unitData["unitId"]}')
    new_unit_id = len(civ.units)
    #clone the unit
    unit = clone(civ.units[unitData["unitId"]], data.version)
    #set id to be end of civ units
    unit.id = new_unit_id

    #this gives back a resource when the unit dies that the unit costs to spawn. This limits us to one.
    unit.dead_unit_id = SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD

    #TODO balance out the units stats compared to new DLC
    unit.hit_points = 500

    #make sure unit take up population space
    unit.resource_storages = (
        ResourceStorage(type=TYPE_POPULATION_HEADROOM, amount=-1, flag=2),
        ResourceStorage(type=TYPE_CURRENT_POPULATION, amount=1, flag=2),
        ResourceStorage(type=TYPE_TOTAL_UNITS_OWNED, amount=1, flag=1),
    )

    #make unit cost resources 
    unit.creatable.resource_costs = (
        #this is how basilius dead works to respawn on intervals
        ResourceCost(type=TYPE_UNIT_LIMIT_RESOURCE, amount=1, flag=1),
        ResourceCost(type=TYPE_FOOD_STORAGE, amount=500, flag=1),
        ResourceCost(type=TYPE_GOLD_STORAGE, amount=500, flag=1),
    )

    #make unit trainable in the castle
    unit.creatable.train_location_id = TYPE_CASTLE_TRAIN_LOCATION
    unit.creatable.button_id = 4
    unit.creatable.hero_mode = 1

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
            limitCivToOneHero(civ_id, new_unit_id, data)
            logging.info(f'Creating hero for civ other civs')
#            for rest_of_civs_civ_id, rest_of_civs_civ in enumerate(data.civs):
#                if rest_of_civs_civ_id != civ_id:
#                    #make the hero for the other civs
#                    makeHero(hero, rest_of_civs_civ, data)
        else:
            logging.info(f'Failed to find hero for civ {civ.name}')
