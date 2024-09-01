import logging

from genieutils.datfile import DatFile
from genieutils.task import Task

from mods.ids import KING, TYPE_GARRISON, CLASS_BUILDING

NAME = 'nomad-king'


def is_not_garrison_building_task(task: Task) -> bool:
    return not (task.action_type == TYPE_GARRISON and task.class_id == CLASS_BUILDING)


def patch_kings(data: DatFile):
    for civ in data.civs:
        king = civ.units[KING]
        king.bird.tasks = [task for task in king.bird.tasks if is_not_garrison_building_task(task)]
        logging.info(f'Removed building garrison task from King ({KING}) for civ {civ.name}')


def mod(data: DatFile):
    patch_kings(data)
