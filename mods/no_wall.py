from genieutils.datfile import DatFile

from mods.ids import PALISADE_WALL, TECH_PALISADE_GATE, TECH_STONE_WALLS, TECH_FORTIFIED_WALL
from mods.util import disable_unit, disable_tech_effect, disable_tech_research_location

NAME = 'no-wall'


def mod(data: DatFile):
    disable_unit(data, PALISADE_WALL)
    disable_tech_effect(data, TECH_PALISADE_GATE)
    disable_tech_effect(data, TECH_STONE_WALLS)
    disable_tech_research_location(data, TECH_FORTIFIED_WALL)

