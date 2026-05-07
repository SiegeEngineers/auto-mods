import logging

from genieutils.civ import Civ
from genieutils.datfile import DatFile

from mods.ids import KING, RELIC_CART, TYPE_POPULATION_HEADROOM, TYPE_GIVE_AND_TAKE, \
    TYPE_BONUS_POPULATION_CAP, TYPE_UNIT_VISION_GRANT_EFFECT, TYPE_RESOURCE_MODIFIER

from genieutils.effect import EffectCommand, Effect

from genieutils.tech import Tech, ResearchResourceCost, ResearchLocation

from mods.util import clone

NAME = 'rewarding-snipes'


def add_tech_for_relic_cart_vision_to_each_civ(data: DatFile, relic_cart_unit_id: int, version: str):
    effect_command = EffectCommand(
        type=TYPE_RESOURCE_MODIFIER,
        a=TYPE_UNIT_VISION_GRANT_EFFECT,
        b=0,  # mode
        c=-1,
        d=-(relic_cart_unit_id)  # negative unit ID to grant vision always and not just on creation.
    )

    effect = Effect(
        name='Give Relic Cart Vision to Civ',
        effect_commands=[effect_command]
    )
    effect_id = len(data.effects)
    data.effects.append(effect)

    for civ_id, civ in enumerate(data.civs):
        tech = Tech(
            required_techs=(-1, -1, -1, -1, -1, -1),
            resource_costs=(
                ResearchResourceCost(type=-1, amount=0, flag=0),
                ResearchResourceCost(type=-1, amount=0, flag=0),
                ResearchResourceCost(type=-1, amount=0, flag=0),
            ),
            required_tech_count=0,
            civ=civ_id,
            full_tech_mode=0,
            research_location=-1,  # no specific location
            language_dll_name=0,
            language_dll_description=0,
            research_time=0,
            effect_id=effect_id,
            type=0,
            icon_id=-1,
            button_id=1,
            language_dll_help=0,
            language_dll_tech_tree=0,
            hot_key=0,
            name='Grant Civ Relic Cart Vision',
            repeatable=1,
            research_locations=[
                ResearchLocation(
                    location_id=-1,
                    research_time=0,
                    button_id=0,
                    hot_key_id=-1,
                )
            ]
        )
        logging.info(f'Adding relic cart vision tech for civ {civ.name} with id {len(data.techs)}')
        data.techs.append(tech)

def clone_and_patch_relic_cart(civ: Civ, version: str):
    clone_unit_id = len(civ.units)
    cloned_unit = clone(civ.units[RELIC_CART], version)
    cloned_unit.id = clone_unit_id
    cloned_unit.fog_visibility = 1  # always visible
    cloned_unit.hit_points = 30_000  # don't die from exploding kings

    # act like a house that gives (and takes) 50 pop space
    cloned_unit.resource_storages[0].type = TYPE_POPULATION_HEADROOM
    cloned_unit.resource_storages[0].amount = 50
    cloned_unit.resource_storages[0].flag = TYPE_GIVE_AND_TAKE

    # grant 50 bonus pop cap while controlled
    cloned_unit.resource_storages[1].type = TYPE_BONUS_POPULATION_CAP
    cloned_unit.resource_storages[1].amount = 50
    cloned_unit.resource_storages[1].flag = TYPE_GIVE_AND_TAKE

    logging.info(f'Cloned unit {clone_unit_id} ({cloned_unit.name}) for civ {civ.name}')
    civ.units.append(cloned_unit)
    return clone_unit_id


def patch_kings(data: DatFile):
    for civ in data.civs:
        relic_cart_id = clone_and_patch_relic_cart(civ, data.version)
        king = civ.units[KING]
        king.blood_unit_id = relic_cart_id  # use blood unit so it does not conflict with exploding kings
        logging.info(f'Patched king blood unit for civ {civ.name}')

    add_tech_for_relic_cart_vision_to_each_civ(data, relic_cart_id, data.version)


def mod(data: DatFile):
    patch_kings(data)
