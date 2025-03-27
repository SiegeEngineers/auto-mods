import logging

from genieutils.civ import Civ
from genieutils.datfile import DatFile
from genieutils.task import Task

from mods.ids import TRISTAN, BAYINNAUNG, DAGNAJAN, SURYAVARMAN_I, \
    TABINSHWEHTI, ABRAHA_ELEPHANT, KHOSRAU, TSAR_KONSTANTIN, LEIF_ERIKSON, VASCO_DA_GAMA, THE_MIDDLEBROOK, \
    ADMIRAL_YI_SHUN_SHIN, BAD_NEIGHBOR, GODS_OWN_SLING, BAD_NEIGHBOR_PACKED, GODS_OWN_SLING_PACKED, EMPEROR_IN_A_BARREL, \
    KING, ACTION_TYPE_GARRISON, CLASS_SIEGE_WEAPON, SABOTEUR, FURIOUS_THE_MONKEY_BOY
from mods.util import clone, patch_unit_for_explosion, patch_monument_to_keep_it_from_exploding, prevent_hp_increase

NAME = 'exploding-kings'

COUNTDOWN_UNITS = [
    TRISTAN,  # Self Plug
    BAYINNAUNG,  # elephant
    DAGNAJAN,  # elephaunt
    SURYAVARMAN_I,  # elephant
    TABINSHWEHTI,  # elephant
    ABRAHA_ELEPHANT,  # elephant
    KHOSRAU,  # elephant
    TSAR_KONSTANTIN,  # Chariot
    LEIF_ERIKSON,  # longboat
    VASCO_DA_GAMA,  # caravel
    THE_MIDDLEBROOK,  # longboat
    ADMIRAL_YI_SHUN_SHIN,  # turtle ship
    BAD_NEIGHBOR,  # treb
    GODS_OWN_SLING,  # treb
    BAD_NEIGHBOR_PACKED,  # treb
    GODS_OWN_SLING_PACKED,  # treb
    EMPEROR_IN_A_BARREL,  # unique
]

NO_CONVERSION_BIT = 2


def lowkey_make_kings_able_to_garrison_siege(civ: Civ):
    king = civ.units[KING]
    task_id = len(king.bird.tasks)
    garrison_siege_task = Task(
        task_type=1,
        id=task_id,
        is_default=0,
        action_type=ACTION_TYPE_GARRISON,
        class_id=CLASS_SIEGE_WEAPON,
        unit_id=-1,
        terrain_id=-1,
        resource_in=-1,
        resource_multiplier=-1,
        resource_out=-1,
        unused_resource=-1,
        work_value_1=0,
        work_value_2=0,
        work_range=1,
        auto_search_targets=0,
        search_wait_time=0,
        enable_targeting=0,
        combat_level_flag=0,
        gather_type=0,
        work_flag_2=0,
        target_diplomacy=1,
        carry_check=0,
        pick_for_construction=0,
        moving_graphic_id=-1,
        proceeding_graphic_id=-1,
        working_graphic_id=-1,
        carrying_graphic_id=-1,
        resource_gathering_sound_id=-1,
        resource_deposit_sound_id=-1,
        wwise_resource_gathering_sound_id=-1,
        wwise_resource_deposit_sound_id=-1,
    )
    king.bird.tasks.append(garrison_siege_task)
    logging.info(f'Patched King garrison for civ {civ.name}')

def lowkey_make_kings_able_to_be_converted(civ: Civ):
    king = civ.units[KING]
    king.creatable.hero_mode &= (255 - NO_CONVERSION_BIT)
    logging.info(f'Patched King conversion for civ {civ.name}')


def build_countdown_to_saboteur(unit_ids: list[int], civ: Civ, version: str) -> tuple[int, int]:
    last_unit_id = KING
    all_unit_ids = unit_ids + [SABOTEUR, FURIOUS_THE_MONKEY_BOY]
    for unit_id in all_unit_ids:
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
        lowkey_make_kings_able_to_garrison_siege(civ)
        lowkey_make_kings_able_to_be_converted(civ)
        patch_monument_to_keep_it_from_exploding(civ)
        saboteur_id, monkey_id = build_countdown_to_saboteur(COUNTDOWN_UNITS, civ, data.version)
        patch_unit_for_explosion(saboteur_id, [0, 1, 2], 25, civ)
        patch_unit_for_explosion(monkey_id, [0, 1], 25, civ)
