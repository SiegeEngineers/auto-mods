import logging

from genieutils.datfile import DatFile
from genieutils.effect import Effect, EffectCommand

from mods.ids import TYPE_ENABLE_DISABLE_UNIT, TYPE_UPGRADE_UNIT, FARMER_F, FARMER_M, FARM, \
    HORSE_COLLAR, HEAVY_PLOW, CROP_ROTATION, ATTR_WORK_RATE, ATTR_MODIFIER_MULTIPLY

NAME = 'teamwork'

ALLOWED_EFFECTS = [
    "Anarchy",
    "Marauders",
    "Nomads",
    "Imperial Nomads",
    "Madrasah",
    "Thalassocracy",
    "Tatars UT 2",
    "Chemistry",
    "Huns Tech Tree",
]

FORBIDDEN_EFFECTS = [
    "Walls HP castle age",
    "Palisade Walls HP feudal age",
]


def should_skip(effect: Effect):
    if effect.name in ALLOWED_EFFECTS:
        return False
    if effect.name in FORBIDDEN_EFFECTS:
        return True
    for command in effect.effect_commands:
        if command.type == TYPE_ENABLE_DISABLE_UNIT or command.type == TYPE_UPGRADE_UNIT:
            return True
    return False


def make_tech_effects_shared(data: DatFile):
    for effect in data.effects:
        if not should_skip(effect):
            for command in effect.effect_commands:
                if command.type < 8:
                    new_type = command.type + 10
                    logging.info(f'Modifying command type from {command.type} to {new_type} for effect {effect.name}')
                    command.type = new_type


def make_farm_techs_buff_speed(data: DatFile):
    unit_ids = [FARMER_F, FARMER_M, FARM]
    effect_ids = [HORSE_COLLAR, HEAVY_PLOW, CROP_ROTATION]

    for effect_id in effect_ids:
        for unit_id in unit_ids:
            effect_command = EffectCommand(
                type=ATTR_MODIFIER_MULTIPLY,
                a=unit_id,
                b=-1,
                c=ATTR_WORK_RATE,
                d=1.1,
            )
            data.effects[effect_id].effect_commands.append(effect_command)
            logging.info(f'Adding EffectCommand to effect {effect_id} to multiply work rate of unit {unit_id} by 1.1')


def mod(data: DatFile):
    make_tech_effects_shared(data)
    make_farm_techs_buff_speed(data)
