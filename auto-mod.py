#! /usr/bin/env python3
import argparse
import hashlib
import logging
import pickle
import time
from pathlib import Path
from typing import Callable

from genieutils.datfile import DatFile

from mods import community_games, duplicate_techs, exploding_kings, exploding_relic_monks, exploding_villagers, \
    flying_dutchman, kidnap, matryoshka, no_wall, nomadking, pocket_horse, random_cost, \
    rewarding_snipes, teamwork, heroes_and_villains

AVAILABLE_MODS: dict[str, Callable[[DatFile], None]] = {
    community_games.NAME: community_games.mod,
    duplicate_techs.NAME: duplicate_techs.mod,
    duplicate_techs.NAME_2: duplicate_techs.mod_2,
    exploding_kings.NAME: exploding_kings.mod,
    exploding_relic_monks.NAME: exploding_relic_monks.mod,
    exploding_villagers.NAME: exploding_villagers.mod,
    exploding_villagers.NAME_2: exploding_villagers.mod_2,
    flying_dutchman.NAME: flying_dutchman.mod,
    kidnap.NAME: kidnap.mod,
    matryoshka.NAME: matryoshka.mod,
    no_wall.NAME: no_wall.mod,
    nomadking.NAME: nomadking.mod,
    pocket_horse.NAME: pocket_horse.mod,
    random_cost.NAME: random_cost.mod,
    random_cost.NAME_2: random_cost.mod_2,
    random_cost.NAME_3: random_cost.mod_3,
    random_cost.NAME_4: random_cost.mod_4,
    rewarding_snipes.NAME: rewarding_snipes.mod,
    teamwork.NAME: teamwork.mod,
    heroes_and_villains.NAME: heroes_and_villains.mod,
}


def main():
    start = time.time()
    parser = argparse.ArgumentParser(
        prog='auto-mod',
        description='Programmatically mod Genie engine dat files '
                    '(mainly for Age of Empires II Definitive Edition)')
    parser.add_argument('input_filename', type=Path,
                        help='The dat file to modify')
    parser.add_argument('output_filename', type=Path,
                        help='Where the modified dat file shall be written to')
    parser.add_argument('--mods', choices=AVAILABLE_MODS.keys(), required=True, nargs='+')
    parser.add_argument('--logfile', type=Path, required=False)
    parser.add_argument('--cache', action='store_true', required=False,
                        help='Cache the parsed dat file, and load a cached version of the dat file if available')
    args = parser.parse_args()

    handlers = []
    if args.logfile:
        handler = logging.FileHandler(args.logfile)
        handler.setLevel(logging.INFO)
        handlers.append(handler)
    logging.basicConfig(level=logging.INFO, handlers=handlers)

    logging.info(f'Loading dat file {args.input_filename}')
    cache_file = None
    data = None
    if args.cache:
        cache_file, data = load_cache(args.input_filename)
    if not data:
        data = DatFile.parse(args.input_filename)
        if args.cache and cache_file:
            write_cache(data, cache_file)

    logging.info(f'Applying mods: {args.mods}')
    for mod in args.mods:
        logging.info(f'Applying {mod}:')
        AVAILABLE_MODS[mod](data)

    logging.info(f'Saving dat file {args.output_filename}')
    data.save(args.output_filename)

    end = time.time()
    logging.info(f'Took {end - start:.2f} seconds')


def load_cache(input_file: Path) -> tuple[Path, DatFile | None]:
    file_hash = get_file_hash(input_file)
    cache_file = Path('/tmp/auto-mod') / f'{file_hash}.pickle'
    logging.info(f'Checking if {cache_file=} exists')
    data = None
    if cache_file.is_file():
        logging.info(f'Loading {cache_file=}')
        data = pickle.loads(cache_file.read_bytes())
    else:
        logging.info('Cache file does not exist')
    return cache_file, data


def write_cache(data: DatFile, cache_file: Path):
    logging.info(f'Writing data to {cache_file=}')
    cache_file.parent.mkdir(exist_ok=True, parents=True)
    cache_file.write_bytes(pickle.dumps(data))


def get_file_hash(input_file: Path) -> str:
    with input_file.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


if __name__ == '__main__':
    main()
