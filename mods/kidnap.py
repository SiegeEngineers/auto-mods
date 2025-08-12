import logging

from genieutils.datfile import DatFile
from genieutils.task import Task

from mods.ids import EAGLE_SCOUT, CAMEL_SCOUT, SCOUT_CAVALRY, ACTION_KIDNAP_UNIT, CLASS_CIVILIAN, ACTION_LOOT, \
    CLASS_BUILDING

NAME = 'kidnap'

SCOUTS = [
    SCOUT_CAVALRY,
    EAGLE_SCOUT,
    CAMEL_SCOUT
]


def patch_scouts(data: DatFile):
    for civ in data.civs:
        for scout_id in SCOUTS:
            unit = civ.units[scout_id]
            logging.info(f'Setting scout ({scout_id}) properties for civ {civ.name}')
            unit.type_50.attacks = []
            unit.type_50.displayed_attack = 0
            unit.garrison_capacity = 1
            unit.bird.tasks = unit.bird.tasks[1:]
            logging.info(f'Adding kidnap task to scout ({scout_id}) for civ {civ.name}')
            kidnap_task = Task(
                task_type=1,
                id=-1,
                is_default=0,
                action_type=ACTION_KIDNAP_UNIT,
                class_id=CLASS_CIVILIAN,
                unit_id=-1,
                terrain_id=-1,
                resource_in=-1,
                resource_multiplier=-1,
                resource_out=-1,
                unused_resource=-1,
                work_value_1=0.0,
                work_value_2=0.0,
                work_range=0.25,
                auto_search_targets=0,
                search_wait_time=0.0,
                enable_targeting=0,
                combat_level_flag=0,
                gather_type=2,
                work_flag_2=0,
                target_diplomacy=2,
                carry_check=0,
                pick_for_construction=0,
                moving_graphic_id=-1,
                proceeding_graphic_id=1966,  # SCOUT_AN
                working_graphic_id=-1,
                carrying_graphic_id=-1,
                resource_gathering_sound_id=-1,
                resource_deposit_sound_id=-1,
                wwise_resource_gathering_sound_id=0,
                wwise_resource_deposit_sound_id=0,
                enabled=1,
            )
            unit.bird.tasks.append(kidnap_task)
            logging.info(f'Adding loot task to scout ({scout_id}) for civ {civ.name}')
            loot_task = Task(
                task_type=1,
                id=-1,
                is_default=0,
                action_type=ACTION_LOOT,
                class_id=CLASS_BUILDING,
                unit_id=-1,
                terrain_id=-1,
                resource_in=-1,
                resource_multiplier=-1,
                resource_out=-1,
                unused_resource=-1,
                work_value_1=0.0,
                work_value_2=0.0,
                work_range=0.25,
                auto_search_targets=0,
                search_wait_time=0.0,
                enable_targeting=0,
                combat_level_flag=0,
                gather_type=1,
                work_flag_2=0,
                target_diplomacy=2,
                carry_check=0,
                pick_for_construction=0,
                moving_graphic_id=-1,
                proceeding_graphic_id=1966,  # SCOUT_AN
                working_graphic_id=-1,
                carrying_graphic_id=-1,
                resource_gathering_sound_id=-1,
                resource_deposit_sound_id=-1,
                wwise_resource_gathering_sound_id=0,
                wwise_resource_deposit_sound_id=0,
                enabled=1,
            )
            unit.bird.tasks.append(loot_task)


def mod(data: DatFile):
    patch_scouts(data)
