import logging

from genieutils.civ import Civ
from genieutils.datfile import DatFile

from mods.ids import FISHING_SHIP, TRADE_COG, TRADE_CART_EMPTY, TRADE_CART_FULL, VILLAGER_FARMER_M, VILLAGER_FARMER_F, \
    VILLAGER_FORAGER_F, VILLAGER_FISHER_F, VILLAGER_GOLD_F, VILLAGER_BUILDER_F, VILLAGER_BASE_M, VILLAGER_SHEPHERD_M, \
    VILLAGER_HUNTER_M, VILLAGER_WOOD_M, VILLAGER_STONE_M, VILLAGER_REPAIRER_M, VILLAGER_BASE_F, VILLAGER_SHEPHERD_F, \
    VILLAGER_HUNTER_F, VILLAGER_WOOD_F, VILLAGER_STONE_F, VILLAGER_REPAIRER_F, VILLAGER_FORAGER_M, VILLAGER_FISHER_M, \
    VILLAGER_GOLD_M, VILLAGER_BUILDER_M, SABOTEUR, ANTIQUITY_MODE_FISHING_SHIP
from mods.util import clone

NAME = 'exploding-villagers'
NAME_2 = 'exploding-villagers-extreme'

UNITS = [
    FISHING_SHIP,
    ANTIQUITY_MODE_FISHING_SHIP,
    TRADE_COG,
    TRADE_CART_EMPTY,
    TRADE_CART_FULL,
    VILLAGER_BASE_M,
    VILLAGER_BASE_F,
    VILLAGER_FARMER_M,
    VILLAGER_FARMER_F,
    VILLAGER_SHEPHERD_M,
    VILLAGER_SHEPHERD_F,
    VILLAGER_FORAGER_M,
    VILLAGER_FORAGER_F,
    VILLAGER_HUNTER_M,
    VILLAGER_HUNTER_F,
    VILLAGER_FISHER_M,
    VILLAGER_FISHER_F,
    VILLAGER_WOOD_M,
    VILLAGER_WOOD_F,
    VILLAGER_GOLD_M,
    VILLAGER_GOLD_F,
    VILLAGER_STONE_M,
    VILLAGER_STONE_F,
    VILLAGER_BUILDER_M,
    VILLAGER_BUILDER_F,
    VILLAGER_REPAIRER_M,
    VILLAGER_REPAIRER_F,
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


def patch_villager_units(data: DatFile, nerf_saboteur: bool):
    for civ in data.civs:
        saboteur_id = clone_saboteur(civ, data.version)
        if nerf_saboteur:
            type_50 = civ.units[saboteur_id].type_50
            type_50.attacks[0].amount = 50
            type_50.attacks[1].amount = 90
            type_50.attacks[2].amount = 0
        for unit_id in UNITS:
            civ.units[unit_id].dead_unit_id = saboteur_id
            logging.info(f'Patched villager unit with id {unit_id} ({civ.units[unit_id].name}) for civ {civ.name}')


def mod(data: DatFile):
    patch_villager_units(data, nerf_saboteur=True)

def mod_2(data: DatFile):
    patch_villager_units(data, nerf_saboteur=False)