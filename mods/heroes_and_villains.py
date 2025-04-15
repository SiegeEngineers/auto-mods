import logging

from genieutils.civ import Civ
from genieutils.unit import Unit
from genieutils.effect import Effect, EffectCommand
from genieutils.tech import Tech, ResearchResourceCost
from genieutils.datfile import DatFile
from genieutils.unit import ResourceCost, ResourceStorage
from mods.util import clone
from mods.ids import TABINSHWEHTI, TSAR_KONSTANTIN, BELISARIUS, WILLIAM_WALLACE, \
    WANG_TONG, ALARIC_THE_GOTH, SUNDJATA, SHAH_ISHMAIL, SALADIN, SELIM_THE_GRIM, JARL, \
    CUAUHTEMOC, ATTILA_THE_HUN, PACAL_II, EL_CID_CAMPEADOR, GENGHIS_KHAN, FRANCESCO_SFORZA, \
    MIKLOS_TOLDI, ALEXANDER_NEVSKI, TARIQ_IBN_ZIYAD, DAGNAJAN, SUMANGURU, BAYINNAUNG, \
    SURYAVARMAN_I, GAJAH_MADA, LE_LOI, KOTYAN_KHAN, ALGIRDAS, QUTLUGH, OSMAN, \
    JOHN_THE_FEARLESS, ROGER_BOSSO, JAN_ZIZKA, JADWIGA, IBRAHIM_LODI, PRITHVIRAJ, TAMAR, \
    THOROS, JOAN_OF_ARC, NOBUNAGA, ULRICH_VON_JUNGINGEN, PACHACUTI, RAJENDRA_CHOLA, POPE_LEO_I, \
    VASCO_DA_GAMA, ADMIRAL_YI_SHUN_SHIN, MIHIRA_BHOJA, LEIF_ERIKSON, EDWARD_LONGSHANKS, \
    THEMISTOCLES, LEONIDAS, DARIUS, \
    TYPE_POPULATION_HEADROOM, TYPE_CURRENT_POPULATION, TYPE_TOTAL_UNITS_OWNED, TYPE_FOOD_STORAGE, \
    TYPE_GOLD_STORAGE, TYPE_CASTLE_TRAIN_LOCATION, TYPE_DOCK_TRAIN_LOCATION, TYPE_POPULATION_HEADROOM, \
    TYPE_ENABLE_DISABLE_UNIT, TECH_REQUIREMENT_IMPERIAL_AGE, TYPE_INFLUENCE_ABILITY,  TYPE_TOTAL_UNITS_OWNED,\
    TYPE_SPAWN_UNIT, TOWN_CENTER, TYPE_TOWN_CENTER_BUILT, SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD, \
    WARSHIP_CLASS, CAVLARY_CLASS, INFANTRY_CLASS, ARCHER_CLASS, CAVALRY_ARCHER_CLASS, MONK_CLASS, HAND_CANNONEER_CLASS, \
    WAR_ELEPHANT_CLASS, ELEPHANT_ARCHER_CLASS, CONQUISTADOR_CLASS, HEALER_CLASS, \
    CAO_CAO, LIU_BEI, SUN_JIAN, FORTIFIED_CHURCH #auras

#reserve spaces for hidden resouces. Dont use 501 as its used for sparta already.
LAND_BASILIUS_RESOURCE_VALUE = 502
WATER_BASILIUS_RESOURCE_VALUE = 503

NAME = 'heroes-and-villains'

CIVS_WITH_HEROES_ALREADY = ['Shu', 'Wu', 'Wei']

HERO_FOR_CIV = {
    "British": [EDWARD_LONGSHANKS],
    "Byzantine": [BELISARIUS],
    "Celts": [WILLIAM_WALLACE],
    "Chinese": [WANG_TONG],
    "French": [JOAN_OF_ARC],
    "Goths": [ALARIC_THE_GOTH],
    "Japanese": [NOBUNAGA],
    "Mongols": [GENGHIS_KHAN],
    "Persians": [SHAH_ISHMAIL],
    "Saracens": [SALADIN],
    "Teutons": [ULRICH_VON_JUNGINGEN],
    "Turks": [OSMAN],
    "Vikings": [JARL, LEIF_ERIKSON],
    "Aztecs": [CUAUHTEMOC],
    "Huns": [ATTILA_THE_HUN],
    "Koreans": [ADMIRAL_YI_SHUN_SHIN],
    "Mayan": [PACAL_II],
    "Spanish": [EL_CID_CAMPEADOR],
    "Incas": [PACHACUTI],
    "Italians": [FRANCESCO_SFORZA],
    "Magyars": [MIKLOS_TOLDI],
    "Slavs": [ALEXANDER_NEVSKI],
    "Berbers": [TARIQ_IBN_ZIYAD],
    "Ethiopians": [DAGNAJAN],
    "Malians": [SUNDJATA],
    "Portuguese": [VASCO_DA_GAMA],
    "Burmese": [TABINSHWEHTI],
    "Khmer": [SURYAVARMAN_I],
    "Malay": [GAJAH_MADA],
    "Vietnamese": [LE_LOI],
    "Bulgarians": [TSAR_KONSTANTIN],
    "Cumans": [KOTYAN_KHAN],
    "Lithuanians": [ALGIRDAS],
    "Tatars": [QUTLUGH], #Tamerlane 
    "Burgundians": [JOHN_THE_FEARLESS],
    "Sicilians": [ROGER_BOSSO],
    "Bohemians": [JAN_ZIZKA],
    "Poles": [JADWIGA], #Jogaila
    "Hindustanis": [IBRAHIM_LODI],
    "Bengalis": [MIHIRA_BHOJA], #maybe not good
    "Gurjaras": [PRITHVIRAJ],
    "Dravidians": [RAJENDRA_CHOLA],
    "Romans": [POPE_LEO_I],
    "Armenians": [THOROS],
    "Georgians": [TAMAR],
    "Spartans": [LEONIDAS],
    "Achaemenids": [DARIUS],
    "Athenians": [THEMISTOCLES]
}

class auraClass:
      def __init__(self, data: DatFile):
        def getAuraFromUnit(unit_id: int, data: DatFile):
            auraTasks = [x for x in data.civs[0].units[unit_id].bird.tasks if x.action_type == 155]
            return auraTasks

        #rip from the units with avilities so we kinda can balance.
        self.workRate = getAuraFromUnit(FORTIFIED_CHURCH, data)
        self.healing = getAuraFromUnit(LIU_BEI, data)
        self.attackSpeed = getAuraFromUnit(CAO_CAO, data)
        self.movementSpeed = getAuraFromUnit(SUN_JIAN, data)


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


def limitHeroesForCiv(civ_id: int, data: DatFile, hidden_resource_id: int) -> int:
    logging.info(f'Making effect for hero unit limit for civ {data.civs[civ_id].name} - {civ_id}')

    #copy dead basilius to give one of hiden resource at start of game.
    dead_basilius = clone(data.civs[civ_id].units[SPECIAL_UNIT_SPAWN_BASILIEUS_DEAD], data.version)
    dead_basilius_id = len(data.civs[civ_id].units)
    dead_basilius.id = dead_basilius_id
    dead_basilius.resource_storages = (
        ResourceStorage(type=12, amount=300.0, flag=0), 
        ResourceStorage(type=hidden_resource_id, amount=1.0, flag=1), 
        ResourceStorage(type=-1, amount=0.0, flag=0)  
    )
    #remove basilius graphic so the dead unit assignment isnt funky.
    dead_basilius.standing_graphic = (-1,-1)
    dead_basilius.dying_graphic = -1
    #add basilius to the civs unit list so it can be used in the effect.
    data.civs[civ_id].units.append(dead_basilius)
    print('dead_basilius', dead_basilius)

    #create a dead basilieus at start to give one of needed resource for the unit. This one gives instant resource 501 when it dies instead of delayed
    give_resource_at_first_tc_effect_command = EffectCommand(type=TYPE_SPAWN_UNIT, a=dead_basilius_id, b=TOWN_CENTER, c=1, d=0.0)
    limit_unit_creatable_effect = Effect(
        name=f'Limit Hero Amount Effect for {data.civs[civ_id].name}',
        effect_commands=[give_resource_at_first_tc_effect_command]
    )
    limit_unit_creatable_effect_id = len(data.effects)
    data.effects.append(limit_unit_creatable_effect)
    #add the tech with the effect to the civ to give 1 hero value.
    logging.info(f'Making tech for hero unit limit for {data.civs[civ_id].name} - {civ_id}')
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
        name=f'Limit Hero Unit amount for civ {data.civs[civ_id].name}',
        repeatable=0,
    )
    data.techs.append(limit_hero_unit_creatable_effect)
    return dead_basilius_id

def extendTasks(unit: Unit, tasks) -> Unit:
    #set unit to use aura abilities
    unit.type_50.break_off_combat = TYPE_INFLUENCE_ABILITY
    #extend the tasks of the unit with the new tasks
    for task in tasks:
        #get correct ids
        task.id = len(unit.bird.tasks) #set the id to be the next available id
        unit.bird.tasks.append(task)
    return unit

def giveAura(unit: Unit, data: DatFile) -> Unit:
    auras = auraClass(data)
    if(unit.class_ in [CAVLARY_CLASS, WARSHIP_CLASS]):
        logging.info("giving move speed aura to unit")
        extendTasks(unit, auras.movementSpeed)
    elif(unit.class_ in [CAVALRY_ARCHER_CLASS, CONQUISTADOR_CLASS]):
        logging.info("giving attack speed aura to unit")
        extendTasks(unit, auras.attackSpeed)
    elif(unit.class_ in [INFANTRY_CLASS, ARCHER_CLASS, HAND_CANNONEER_CLASS]):
        logging.info("giving move and attack speed aura to unit")
        extendTasks(unit, auras.attackSpeed)
        extendTasks(unit, auras.movementSpeed)
    elif(unit.class_ in [MONK_CLASS, HEALER_CLASS]):
        logging.info("giving healing aura to unit")
        extendTasks(unit, auras.healing)
    else:
        logging.error(f"Unit {unit.name} not given an aura")
    return unit

def makeHero(unitId: int, civ: Civ, data: DatFile, land_basilius_unit_id: int, water_basilius_unit_id: int) -> int:
    #prevent_hp_increase(cloned_unit)
    logging.info(f'Patching hero unit {unitId}')
    new_unit_id = len(civ.units)
    #clone the unit
    unit = clone(civ.units[unitId], data.version)
    #set id to be end of civ units
    unit.id = new_unit_id

    #give the unit an aura based on its class
    unit = giveAura(unit, data)

    #make sure unit take up population space
    unit.resource_storages = (
        ResourceStorage(type=TYPE_POPULATION_HEADROOM, amount=-1, flag=2),
        ResourceStorage(type=TYPE_CURRENT_POPULATION, amount=1, flag=2),
        ResourceStorage(type=TYPE_TOTAL_UNITS_OWNED, amount=1, flag=1),
    )

    #make unit trainable in the dock if waship and limit is with water basilius
    if(unit.class_ == WARSHIP_CLASS):
        logging.info(f'chose dock for hero unit {unit.name} for civ {civ.name}')
        unit.creatable.train_location_id = TYPE_DOCK_TRAIN_LOCATION
        unit.creatable.button_id = 30
        #this gives back a resource when the unit dies that the unit costs to spawn. This limits us to one.
        unit.dead_unit_id = water_basilius_unit_id
        #make unit cost resources 
        unit.creatable.resource_costs = (
            #lock it into a water cost
            ResourceCost(type=WATER_BASILIUS_RESOURCE_VALUE, amount=1, flag=1),
            ResourceCost(type=TYPE_FOOD_STORAGE, amount=500, flag=1),
            ResourceCost(type=TYPE_GOLD_STORAGE, amount=500, flag=1),
        )
    #make unit trainable in the castle if other unit type and limit is with land basilius
    else:
        logging.info(f'chose castle for hero unit {unit.name} for civ {civ.name}')
        unit.creatable.train_location_id = TYPE_CASTLE_TRAIN_LOCATION
        unit.creatable.button_id = 4
        #this gives back a resource when the unit dies that the unit costs to spawn. This limits us to one.
        unit.dead_unit_id = land_basilius_unit_id
        #make unit cost resources 
        unit.creatable.resource_costs = (
            #lock it into a water cost
            ResourceCost(type=LAND_BASILIUS_RESOURCE_VALUE, amount=1, flag=1),
            ResourceCost(type=TYPE_FOOD_STORAGE, amount=500, flag=1),
            ResourceCost(type=TYPE_GOLD_STORAGE, amount=500, flag=1),
        )
        
    unit.creatable.hero_mode = 1

    #add the new unit to the civ
    civ.units.append(unit)
    logging.info(f'Patched hero unit {unit.name} for civ {civ.name}')
    return new_unit_id



def mod(data: DatFile):
    civs_missing_hero = []
    for civ_id, civ in enumerate(data.civs):
        if civ.name in HERO_FOR_CIV and HERO_FOR_CIV[civ.name] is not None:
            logging.info(f'Creating hero for civ {civ.name}')
            land_dead_basilius_id = limitHeroesForCiv(civ_id, data, LAND_BASILIUS_RESOURCE_VALUE)
            water_dead_basilius_id = limitHeroesForCiv(civ_id, data, WATER_BASILIUS_RESOURCE_VALUE)
            unit_id = HERO_FOR_CIV[civ.name][0]
            for unit_id in HERO_FOR_CIV[civ.name]:
                hero_unit_id = makeHero(unit_id, civ, data, land_dead_basilius_id, water_dead_basilius_id)
                addUnitToCiv(civ_id, hero_unit_id, data)
            logging.info(f'Creating hero for civ other civs')
        else:
            if(civ.name not in CIVS_WITH_HEROES_ALREADY):
                # If the civ is not in the list, log an error message
                civs_missing_hero.append(civ.name)
    
    for civ in civs_missing_hero:
        logging.error(f'No hero for civ {civ}')
