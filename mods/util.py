import logging
from typing import TypeVar

from genieutils.civ import Civ
from genieutils.common import GenieClass, ByteHandler
from genieutils.datfile import DatFile
from genieutils.effect import EffectCommand
from genieutils.unit import Unit
from genieutils.versions import Version

from mods.ids import CLASS_PETARD, MONUMENT, CLASS_HERO, TYPE_UPGRADE_UNIT, TYPE_COMBATANT

GC = TypeVar('GC', bound=GenieClass)

def clone(item: GC, version: str) -> GC:
    packed = item.to_bytes(Version(version))
    byte_handler = ByteHandler(memoryview(packed))
    byte_handler.version = Version(version)
    return item.__class__.from_bytes(byte_handler)

def disable_unit(data: DatFile, unit_id: int):
    logging.info(f'Disabling unit with id {unit_id} ({data.civs[0].units[unit_id].name}) for all civs')
    for civ in data.civs:
        civ.units[unit_id].enabled = 0


def disable_tech_effect(data: DatFile, tech_id: int):
    logging.info(f'Disabling the effect of tech with id {tech_id} ({data.techs[tech_id].name})')
    data.techs[tech_id].effect_id = -1


def disable_tech_research_location(data: DatFile, tech_id: int):
    logging.info(f'Disabling the research location of tech with id {tech_id} ({data.techs[tech_id].name})')
    data.techs[tech_id].research_location = -1

def patch_unit_for_explosion(unit_it: int, attack_indexes: list[int], range_: int, civ: Civ):
    unit = civ.units[unit_it]
    unit.class_ = CLASS_PETARD
    for index in attack_indexes:
        unit.type_50.attacks[index].amount = 5000
    unit.type_50.max_range = range_
    unit.type_50.blast_width = range_
    unit.type_50.blast_attack_level = 0
    unit.wwise_train_sound_id = 0
    logging.info(f'Patched unit {unit.id} ({unit.name}) for civ {civ.name}')


def patch_monument_to_keep_it_from_exploding(civ: Civ):
    monument = civ.units[MONUMENT]
    monument.hit_points = 30000
    monument.type_50.base_armor = 10000


def is_trebuchet(unit: Unit) -> bool:
    return unit.class_ in (51, 54)


def prevent_hp_increase(unit: Unit):
    unit.type_50.armours = []
    if not is_trebuchet(unit):  # Trebuchets don't die right if they have another class
        unit.class_ = CLASS_HERO


def is_unit_upgrade(effect_command: EffectCommand) -> bool:
    return effect_command.type == TYPE_UPGRADE_UNIT and effect_command.c == -1


def is_unit(data: DatFile, unit_id: int) -> bool:
    return data.civs[0].units[unit_id] and data.civs[0].units[unit_id].type == TYPE_COMBATANT


def affects_units(data: DatFile, effect_command: EffectCommand) -> bool:
    return is_unit(data, effect_command.a) and is_unit(data, effect_command.b)
