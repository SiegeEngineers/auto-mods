import logging

from genieutils.datfile import DatFile

from mods.ids import TRANSPORT_SHIP
from mods.util import is_unit_upgrade, affects_units

NAME = 'x3'
NAME_2 = 'x9'


def has_unit_upgrade_command(data: DatFile, tech_id: int) -> bool:
    for command in data.effects[data.techs[tech_id].effect_id].effect_commands:
        if is_unit_upgrade(command) and affects_units(data, command):
            return True
    return False

def duplicate_techs(data: DatFile, count: int):
    techs_to_duplicate = []
    for i in range(len(data.techs)):
        if data.techs[i].repeatable:
            if not has_unit_upgrade_command(data, i):
                techs_to_duplicate.append(i)

    for i in techs_to_duplicate:
        tech = data.techs[i]
        for _ in range(count):
            data.techs.append(tech)
        logging.info(f'Added Tech {tech.name} for a total of {count} instances')


def mod(data: DatFile):
    duplicate_techs(data, count=2)


def mod_2(data: DatFile):
    duplicate_techs(data, count=8)
