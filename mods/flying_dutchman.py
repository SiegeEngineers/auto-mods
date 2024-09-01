import logging

from genieutils.datfile import DatFile

from mods.ids import TRANSPORT_SHIP

NAME = 'flying-dutchman'


def patch_transport_ships(data: DatFile):
    for civ in data.civs:
        transport_ship = civ.units[TRANSPORT_SHIP]
        transport_ship.fly_mode = 1
        transport_ship.terrain_restriction = 0  # ALL
        transport_ship.collision_size_x = 0
        transport_ship.collision_size_y = 0
        transport_ship.collision_size_z = 0
        logging.info(f'Patched Transport Ship unit for civ {civ.name}')


def mod(data: DatFile):
    patch_transport_ships(data)
