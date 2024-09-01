import logging
import random
from collections import namedtuple

from genieutils.datfile import DatFile
from genieutils.tech import ResearchResourceCost
from genieutils.unit import ResourceCost

from mods.ids import TYPE_POPULATION_HEADROOM, VILLAGER_BASE_M, VILLAGER_BASE_F, TYPE_FOOD, TYPE_WOOD, TYPE_GOLD, \
    TYPE_STONE

NAME = 'random-costs'
NAME_2 = 'random-costs-light'
NAME_3 = 'random-tech-costs'
NAME_4 = 'random-unit-costs'

GenericCost = namedtuple('GenericCost', ['type', 'amount', 'flag'])


def to_generic(costs: tuple[ResearchResourceCost, ResearchResourceCost, ResearchResourceCost] | tuple[
    ResourceCost, ResourceCost, ResourceCost]) -> tuple[GenericCost, GenericCost, GenericCost]:
    return (
        GenericCost(costs[0].type, costs[0].amount, costs[0].flag),
        GenericCost(costs[1].type, costs[1].amount, costs[1].flag),
        GenericCost(costs[2].type, costs[2].amount, costs[2].flag),
    )


def is_natural_resource_cost(cost: ResourceCost) -> bool:
    return cost.type != -1 and cost.type < 4 and cost.amount > 0 and cost.flag == 1


def has_natural_resource_cost(costs: tuple[ResourceCost, ResourceCost, ResourceCost] | tuple[
    ResearchResourceCost, ResearchResourceCost, ResearchResourceCost]) -> bool:
    for cost in costs:
        if is_natural_resource_cost(cost):
            return True
    return False


def get_sum_of_natural_resource_costs(costs: tuple[ResourceCost, ResourceCost, ResourceCost] | tuple[
    ResearchResourceCost, ResearchResourceCost, ResearchResourceCost]) -> int:
    value = 0
    for cost in costs:
        if -1 < cost.type < 4:
            value += cost.amount
    return value


def get_applicable_unit_ids(data: DatFile, limit: int) -> list[int]:
    applicable_unit_ids = []
    for i, unit in enumerate(data.civs[1].units):
        if unit \
                and unit.creatable \
                and has_natural_resource_cost(unit.creatable.resource_costs) \
                and get_sum_of_natural_resource_costs(unit.creatable.resource_costs) > limit:
            applicable_unit_ids.append(i)
    return applicable_unit_ids


def get_applicable_tech_ids(data: DatFile, limit: int) -> list[int]:
    applicable_tech_ids = []
    for i, tech in enumerate(data.techs):
        if has_natural_resource_cost(tech.resource_costs) and get_sum_of_natural_resource_costs(
                tech.resource_costs) > limit:
            applicable_tech_ids.append(i)
    return applicable_tech_ids


def for_tech(costs: tuple[GenericCost, GenericCost, GenericCost]) -> tuple[
    ResearchResourceCost, ResearchResourceCost, ResearchResourceCost]:
    generic_cost = ResearchResourceCost(type=-1, amount=0, flag=0)
    return (
        ResearchResourceCost(
            type=costs[0].type,
            amount=costs[0].amount,
            flag=costs[0].flag,
        ) if costs[0].type < TYPE_POPULATION_HEADROOM else generic_cost,
        ResearchResourceCost(
            type=costs[1].type,
            amount=costs[1].amount,
            flag=costs[1].flag,
        ) if costs[1].type < TYPE_POPULATION_HEADROOM else generic_cost,
        ResearchResourceCost(
            type=costs[2].type,
            amount=costs[2].amount,
            flag=costs[2].flag,
        ) if costs[2].type < TYPE_POPULATION_HEADROOM else generic_cost,
    )


def for_unit(costs: tuple[GenericCost, GenericCost, GenericCost]) -> tuple[ResourceCost, ResourceCost, ResourceCost]:
    return (
        ResourceCost(type=costs[0].type, amount=costs[0].amount, flag=costs[0].flag),
        ResourceCost(type=costs[1].type, amount=costs[1].amount, flag=costs[1].flag),
        ResourceCost(type=costs[2].type, amount=costs[2].amount, flag=costs[2].flag),
    )


def costs_to_string(costs: tuple[ResourceCost, ResourceCost, ResourceCost] | tuple[
    ResearchResourceCost, ResearchResourceCost, ResearchResourceCost]) -> str:
    value = ''
    for cost in costs:
        if cost.flag == 1:
            value += str(cost.amount)
            if cost.type == TYPE_FOOD:
                value += ' Food '
            elif cost.type == TYPE_WOOD:
                value += ' Wood '
            elif cost.type == TYPE_GOLD:
                value += ' Gold '
            elif cost.type == TYPE_STONE:
                value += ' Stone '
            elif cost.type == TYPE_POPULATION_HEADROOM:
                value += ' Pop '
            elif cost.type == -1:
                pass
            else:
                value += ' vat? '
    return value


def patch_thing(data: DatFile, limit: int, units: bool, techs: bool):
    applicable_units = get_applicable_unit_ids(data, limit)
    applicable_techs = get_applicable_tech_ids(data, limit)
    all_the_costs = []
    if units:
        all_the_costs += [to_generic(data.civs[1].units[unit_id].creatable.resource_costs) for unit_id in
                          applicable_units]
    if techs:
        all_the_costs += [to_generic(data.techs[tech_id].resource_costs) for tech_id in applicable_techs]

    for i in range(10):
        random.seed(None)
        seed = random.randbytes(8)
        logging.info(f'{seed=}')
        random.seed(seed)
        random.shuffle(all_the_costs)
        cost_index = 0
        if units:
            for unit_id in applicable_units:
                costs = for_unit(all_the_costs[cost_index])
                new_costs = (
                    costs[0],
                    costs[1],
                    costs[2] if costs[2].type == TYPE_POPULATION_HEADROOM else
                    data.civs[0].units[unit_id].creatable.resource_costs[2],
                )
                logging.info(f'Setting cost of unit with id {unit_id} to {costs_to_string(new_costs)} for all civs')
                for civ in data.civs:
                    civ.units[unit_id].creatable.resource_costs = new_costs
                cost_index += 1

        if techs:
            for tech_id in applicable_techs:
                data.techs[tech_id].resource_costs = for_tech(all_the_costs[cost_index])
                logging.info(f'Setting cost of tech with id {tech_id} '
                             f'to {costs_to_string(data.techs[tech_id].resource_costs)}')
                cost_index += 1

        male_villager_cost = get_sum_of_natural_resource_costs(
            data.civs[0].units[VILLAGER_BASE_M].creatable.resource_costs)
        female_villager_cost = get_sum_of_natural_resource_costs(
            data.civs[0].units[VILLAGER_BASE_F].creatable.resource_costs)
        if male_villager_cost <= 200 and female_villager_cost <= 200:
            return
        logging.info('Villagers are too expensive, reshuffling…')
    logging.info('Giving up, villagers stay expensive, sorry.')


def mod(data: DatFile):
    patch_thing(data, limit=0, units=True, techs=True)


def mod_2(data: DatFile):
    patch_thing(data, limit=5, units=True, techs=True)


def mod_3(data: DatFile):
    patch_thing(data, limit=0, units=False, techs=True)


def mod_4(data: DatFile):
    patch_thing(data, limit=0, units=True, techs=False)
