import logging

from genieutils.datfile import DatFile
from genieutils.effect import EffectCommand, Effect

from mods.ids import TYPE_ENABLE_DISABLE_UNIT
from mods.util import is_unit_upgrade, affects_units

NAME = 'matryoshka'



def set_unit_availability(effect: Effect, unit_id: int, available: bool):
    command = EffectCommand(
        type=TYPE_ENABLE_DISABLE_UNIT,
        a=unit_id,
        b=1 if available else 0,
        c=-1,
        d=0,
    )
    effect.effect_commands.append(command)


def disable_unit(effect: Effect, unit_id: int):
    set_unit_availability(effect, unit_id, False)


def enable_unit(effect: Effect, unit_id: int):
    set_unit_availability(effect, unit_id, True)


def remember_pair(mapping: dict[int, set[int]], lower: int, higher: int):
    if higher not in mapping:
        mapping[higher] = set()
    mapping[higher].add(lower)


def set_dead_ids_from_pairs(data: DatFile, mapping: dict[int, set[int]]):
    for i in range(10):
        to_delete = []
        for higher, lowers in mapping.items():
            if len(lowers) == 1:
                lower = list(lowers)[0]
                for civ in data.civs:
                    civ.units[higher].dead_unit_id = lower
                logging.info(f'Setting dead unit id of unit {higher} to {lower}')
                to_delete.append(lower)
        for higher, lowers in mapping.items():
            for item in to_delete:
                if item in lowers:
                    lowers.remove(item)


def apply_matryoshka_logic(data: DatFile):
    mapping = {}
    for effect in data.effects:
        to_disable = set()
        to_enable = set()
        for command in effect.effect_commands:
            if is_unit_upgrade(command) and affects_units(data, command):
                command.c = 1
                to_disable.add(command.a)
                to_enable.add(command.b)
                remember_pair(mapping, lower=command.a, higher=command.b)
        for unit_id in to_disable:
            disable_unit(effect, unit_id)
        for unit_id in to_enable:
            enable_unit(effect, unit_id)
    set_dead_ids_from_pairs(data, mapping)


def mod(data: DatFile):
    apply_matryoshka_logic(data)
