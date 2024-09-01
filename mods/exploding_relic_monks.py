import logging

from genieutils.civ import Civ
from genieutils.datfile import DatFile

from mods.ids import SABOTEUR, FURIOUS_THE_MONKEY_BOY, MONK_WITH_RELIC
from mods.util import clone, patch_unit_for_explosion, patch_monument_to_keep_it_from_exploding, prevent_hp_increase

NAME = 'exploding-relic-monks'


def clone_units_and_set_relic_monk_dead_unit(civ: Civ, version: str) -> tuple[int, int]:
    last_unit_id = MONK_WITH_RELIC
    for unit_id in [SABOTEUR, FURIOUS_THE_MONKEY_BOY]:
        clone_unit_id = len(civ.units)
        cloned_unit = clone(civ.units[unit_id], version)
        cloned_unit.id = clone_unit_id
        cloned_unit.hit_points = -1
        prevent_hp_increase(cloned_unit)
        logging.info(f'Patched hero unit {clone_unit_id} ({cloned_unit.name}) for civ {civ.name}')
        civ.units.append(cloned_unit)
        civ.units[last_unit_id].dead_unit_id = clone_unit_id
        logging.info(f'Set dying unit for unit {last_unit_id} to {clone_unit_id} for civ {civ.name}')
        last_unit_id = clone_unit_id
    return len(civ.units) - 2, len(civ.units) - 1


def mod(data: DatFile):
    for civ in data.civs:
        patch_monument_to_keep_it_from_exploding(civ)
        saboteur_id, monkey_id = clone_units_and_set_relic_monk_dead_unit(civ, data.version)
        patch_unit_for_explosion(saboteur_id, [0, 1, 2], 17, civ)
        patch_unit_for_explosion(monkey_id, [0, 1], 17, civ)
