import logging

from genieutils.civ import Civ
from genieutils.datfile import DatFile
from genieutils.unit import Unit

from mods.ids import SABOTEUR, CLASS_TRADE_BOAT, CLASS_CIVILIAN, CLASS_TRADE_CART, CLASS_FISHING_BOAT
from mods.util import clone

NAME = 'exploding-villagers'
NAME_2 = 'exploding-villagers-extreme'

EXPLODING_CLASSES = [
    CLASS_TRADE_BOAT,
    CLASS_CIVILIAN,
    CLASS_TRADE_CART,
    CLASS_FISHING_BOAT,
]


def clone_saboteur(civ: Civ, version: str) -> int:
    clone_unit_id = len(civ.units)
    cloned_unit = clone(civ.units[SABOTEUR], version)
    cloned_unit.id = clone_unit_id
    cloned_unit.hit_points = -1
    cloned_unit.type_50.max_range = 2
    cloned_unit.type_50.blast_attack_level = 1
    cloned_unit.train_sound = -1
    cloned_unit.wwise_train_sound_id = 0
    logging.info(f'Cloned unit {clone_unit_id} ({cloned_unit.name}) for civ {civ.name}')
    civ.units.append(cloned_unit)
    return clone_unit_id


def should_explode(unit: Unit) -> bool:
    return unit.class_ in EXPLODING_CLASSES if unit else False


def patch_villager_units(data: DatFile, nerf_saboteur: bool):
    for civ in data.civs:
        saboteur_id = clone_saboteur(civ, data.version)
        if nerf_saboteur:
            type_50 = civ.units[saboteur_id].type_50
            type_50.attacks[0].amount = 50
            type_50.attacks[1].amount = 90
            type_50.attacks[2].amount = 0
        for unit in civ.units:
            if should_explode(unit):
                unit.dead_unit_id = saboteur_id
                logging.info(f'Patched unit with id {unit.id} ({unit.name}) for civ {civ.name}')


def mod(data: DatFile):
    patch_villager_units(data, nerf_saboteur=True)


def mod_2(data: DatFile):
    patch_villager_units(data, nerf_saboteur=False)
