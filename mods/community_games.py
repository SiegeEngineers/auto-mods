import logging

from genieutils.datfile import DatFile
from genieutils.effect import EffectCommand, Effect
from genieutils.tech import Tech, ResearchResourceCost
from genieutils.unit import ResourceCost, ResourceStorage

from mods.ids import BOMBARD_TOWER, TYPE_POPULATION_HEADROOM, TYPE_CURRENT_POPULATION, TYPE_TOTAL_UNITS_OWNED, \
    TECH_CARAVAN, TYPE_FOOD, TYPE_GOLD, TYPE_WOOD, TYPE_RESOURCE_MODIFIER, RESOURCE_BONUS_POPULATION_CAP, MODE_ADD, \
    BOMBARD_TOWER_TECH, PETARD, TYPE_UPGRADE_UNIT, CHEMISTRY, CASTLE
from mods.util import clone

NAME = 'community-games'


def add_population_cost_to_bombard_tower(data: DatFile):
    logging.info('Adding 1 population cost to bombard towers of all civs')
    for civ in data.civs:
        bbt = civ.units[BOMBARD_TOWER]
        bbt.creatable.resource_costs = (
            bbt.creatable.resource_costs[0],
            bbt.creatable.resource_costs[1],
            ResourceCost(type=TYPE_POPULATION_HEADROOM, amount=1, flag=0),
        )
        bbt.resource_storages = (
            ResourceStorage(type=TYPE_POPULATION_HEADROOM, amount=-1, flag=2),
            ResourceStorage(type=TYPE_CURRENT_POPULATION, amount=1, flag=2),
            ResourceStorage(type=TYPE_TOTAL_UNITS_OWNED, amount=1, flag=1),
        )


def add_great_hall_tech(data: DatFile):
    effect_command = EffectCommand(
        type=TYPE_RESOURCE_MODIFIER,
        a=RESOURCE_BONUS_POPULATION_CAP,
        b=MODE_ADD,
        c=-1,
        d=10,  # amount
    )

    effect = Effect(
        name='Great Hall',
        effect_commands=[effect_command]
    )
    effect_id = len(data.effects)
    data.effects.append(effect)

    tech = Tech(
        required_techs=(BOMBARD_TOWER_TECH, -1, -1, -1, -1, -1),
        resource_costs=(
            ResearchResourceCost(type=TYPE_WOOD, amount=400, flag=1),
            ResearchResourceCost(type=TYPE_GOLD, amount=600, flag=1),
            ResearchResourceCost(type=-1, amount=-1, flag=0),
        ),
        required_tech_count=1,
        civ=-1,
        full_tech_mode=1,
        research_location=BOMBARD_TOWER,
        language_dll_name=0,
        language_dll_description=0,
        research_time=5,
        effect_id=effect_id,
        type=-1,
        icon_id=103,
        button_id=1,
        language_dll_help=0,
        language_dll_tech_tree=0,
        hot_key=0,
        name='The Great Hall',
        repeatable=1,
    )
    logging.info(f'Adding great hall tech with id {len(data.techs)}')
    data.techs.append(tech)


def add_elite_petard(data: DatFile):
    elite_petard_id = len(data.civs[0].units)
    epetard = clone(data.civs[0].units[PETARD], version=data.version)
    epetard.id = elite_petard_id
    epetard.enabled = 1
    epetard.name = 'Elite Petard'
    epetard.hit_points = 60
    epetard.speed = 1.05
    epetard.type_50.displayed_attack = 45
    epetard.type_50.blast_width = 1.5
    epetard.type_50.blast_attack_level = 1
    epetard.type_50.attacks[0].amount = 150
    epetard.type_50.attacks[1].amount = 650
    epetard.type_50.attacks[2].amount = 45
    epetard.type_50.attacks[3].amount = 70
    epetard.type_50.attacks[4].amount = 1000
    epetard.type_50.armours[1].amount = 3
    epetard.creatable.displayed_pierce_armor = 3
    for civ in data.civs:
        logging.info(f'Adding elite petard for civ {civ.name} with ID {elite_petard_id}')
        civ.units.append(epetard)
        civ.unit_pointers.append(1)

    effect_command = EffectCommand(
        type=TYPE_UPGRADE_UNIT,
        a=PETARD,
        b=elite_petard_id,
        c=-1,
        d=-1,
    )

    effect = Effect(
        name='Elite Petard',
        effect_commands=[effect_command]
    )
    effect_id = len(data.effects)
    data.effects.append(effect)

    tech = Tech(
        required_techs=(CHEMISTRY, -1, -1, -1, -1, -1),
        resource_costs=(
            ResearchResourceCost(type=TYPE_FOOD, amount=450, flag=1),
            ResearchResourceCost(type=TYPE_GOLD, amount=450, flag=1),
            ResearchResourceCost(type=-1, amount=-1, flag=0),
        ),
        required_tech_count=1,
        civ=-1,
        full_tech_mode=1,
        research_location=CASTLE,
        language_dll_name=0,
        language_dll_description=0,
        research_time=40,
        effect_id=effect_id,
        type=-1,
        icon_id=105,
        button_id=9,
        language_dll_help=0,
        language_dll_tech_tree=0,
        hot_key=0,
        name='Elite Petard',
        repeatable=1,
    )
    logging.info(f'Adding Elite Petard Tech with id {len(data.techs)}')
    data.techs.append(tech)

def modify_caravan_cost(data: DatFile, food: int, gold: int):
    caravan = data.techs[TECH_CARAVAN]
    caravan.resource_costs = (
        ResearchResourceCost(type=TYPE_FOOD, amount=food, flag=1),
        ResearchResourceCost(type=TYPE_GOLD, amount=gold, flag=1),
        caravan.resource_costs[2],
    )


def make_trees_contain_200_wood(data: DatFile):
    for civ in data.civs:
        for unit in civ.units:
            if unit:
                for storage in unit.resource_storages:
                    if storage.type == TYPE_WOOD and storage.amount > 50:
                        logging.info(f'Setting amount of wood in unit {unit.id} ({unit.name}) to 200')
                        storage.amount = 200


def mod(data: DatFile):
    add_population_cost_to_bombard_tower(data)
    add_great_hall_tech(data)
    add_elite_petard(data)
    modify_caravan_cost(data, 800, 200)
    make_trees_contain_200_wood(data)
