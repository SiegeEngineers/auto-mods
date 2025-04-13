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
    VASCO_DA_GAMA, ADMIRAL_YI_SHUN_SHIN, MIHIRA_BHOJA, LEIF_ERIKSON, \
    TYPE_POPULATION_HEADROOM, TYPE_CURRENT_POPULATION, TYPE_TOTAL_UNITS_OWNED, TYPE_FOOD_STORAGE, \
    TYPE_GOLD_STORAGE, TYPE_CASTLE_TRAIN_LOCATION, TYPE_DOCK_TRAIN_LOCATION, \
    TYPE_ENABLE_DISABLE_UNIT, TECH_REQUIREMENT_IMPERIAL_AGE, TYPE_INFLUENCE_ABILITY, \
    TYPE_SPAWN_CAP, TYPE_UNIT_LIMIT_RESOURCE, TYPE_POPULATION_HEADROOM, TYPE_TOTAL_UNITS_OWNED, \
    TYPE_SPAWN_UNIT, TOWN_CENTER, TYPE_TOWN_CENTER_BUILT, SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD, \
    WARSHIP_CLASS, CAVLARY_CLASS, INFANTRY_CLASS, ARCHER_CLASS, CAVALRY_ARCHER_CLASS, MONK_CLASS \

from mods.auras import attackSpeed, movementSpeed, healing

NAME = 'heroes-and-villains'

#NOTE DO NOT REMOVE SPARTANS, WE USE THEIR POLEMARCH MECHANIC AND GIVING THEM EXTRA HEROES WOULD BREAK SHIT.
IGNORED_CIVS = ['Gaia', 'Achaemenids', 'Spartans', 'Athenians', 'Shu', 'Wei', 'Wu', "Khitans", 'Jurchens']

#TODO Maybe allow for multiple heroes, or randomize hero on each build.
#TODO Maybe generate water hero(es) alongside land hero(es)
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
    "Vikings": { "unitId": LEIF_ERIKSON, "unitStatChanges": {} },
    "Aztecs": { "unitId": ITZCOATL, "unitStatChanges": {} },
    "Huns": { "unitId": ATTILA_THE_HUN, "unitStatChanges": {} },
    "Koreans": { "unitId": ADMIRAL_YI_SHUN_SHIN, "unitStatChanges": {} },
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
    "Portuguese": { "unitId": VASCO_DA_GAMA, "unitStatChanges": {}},
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
    #MIHIRA_BHOJA is kinda between bengali / gurjaras and more gurjaras depending on source. Would change if other option
    "Bengalis": { "unitId": MIHIRA_BHOJA, "unitStatChanges": {} }, 
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
        required_techs=(TECH_REQUIREMENT_IMPERIAL_AGE, -1, -1, -1, -1, -1), #imperial age
        resource_costs=(
            ResearchResourceCost(type=-1, amount=0, flag=0),
            ResearchResourceCost(type=-1, amount=0, flag=0),
            ResearchResourceCost(type=-1, amount=0, flag=0)
        ),
        required_tech_count=1,
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

    #Make dead basilius not have a dead graphic
    dead_basilius = data.civs[civ_id].units[SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD]
    #remove basilius dead graphic
    dead_basilius.standing_graphic = (-1,-1)
    dead_basilius.dying_graphic = -1 #TODO maybe give an explosion (petard) a flash of light or more obvious death animation
    #create a dead basilieus at start to give one of needed resource for the unit. This one gives instant resource 501 when it dies instead of delayed
    create_basilius_effect_command = EffectCommand(type=TYPE_SPAWN_UNIT, a=SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD, b=TOWN_CENTER, c=1, d=0.0)
    limit_unit_creatable_effect = Effect(
        name=f'Limit Hero Amount Effect for {data.civs[civ_id].units[unit_id].name}',
        effect_commands=[limit_unit_creatable_effect_command, create_basilius_effect_command]
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

def extendTasks(unit: Unit, tasks) -> Unit:
    #set unit to use aura abilities
    unit.type_50.break_off_combat = TYPE_INFLUENCE_ABILITY
    #extend the tasks of the unit with the new tasks
    for task in tasks:
        #get correct ids
        task.id = len(unit.bird.tasks) #set the id to be the next available id
        unit.bird.tasks.append(task)
    return unit

def giveAura(unit: Unit):
    #TODO handle boat class
    if(unit.class_ in [CAVLARY_CLASS]):
        logging.info("giving cavalry aura to unit")
        extendTasks(unit, movementSpeed)
    if(unit.class_ in [INFANTRY_CLASS]):
        logging.info("giving infantry aura to unit")
        extendTasks(unit, attackSpeed)
        extendTasks(unit, movementSpeed)
    if(unit.class_ in [ARCHER_CLASS]):
        logging.info("giving archer aura to unit")
        extendTasks(unit, attackSpeed)
        extendTasks(unit, movementSpeed)
    if(unit.class_ in [CAVALRY_ARCHER_CLASS]):
        logging.info("giving cavalry archer aura to unit")
        extendTasks(unit, attackSpeed)
    if(unit.class_ in [WARSHIP_CLASS]):
        logging.info("giving cavalry archer aura to unit")
        extendTasks(unit, attackSpeed)
    if(unit.class_ in [MONK_CLASS]):
        logging.info("giving monk aura to unit")
        extendTasks(unit, healing)
    return unit

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

    #give the unit an aura based on its class
    unit = giveAura(unit)
    #print('unit', unit)

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
    if(unit.class_ == WARSHIP_CLASS):
        logging.info(f'chose dock for hero unit {unit.name} for civ {civ.name}')
        unit.creatable.train_location_id = TYPE_DOCK_TRAIN_LOCATION
        unit.creatable.button_id = 30
    else:
        logging.info(f'chose castle for hero unit {unit.name} for civ {civ.name}')
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
