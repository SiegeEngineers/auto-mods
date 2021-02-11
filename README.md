# auto-mods
some data mods, generated programmatically, so they can be recreated automagically for every new patch

> We just do everything auto!«
> *\- [MbL, 2020](https://clips.twitch.tv/AttractiveAntediluvianAmazonHassaanChop)*

## Published Mods

- [Community Games Mod](https://www.ageofempires.com/mods/details/15917/)
- [Flying Dutchman](https://www.ageofempires.com/mods/details/15644/)
- [Exploding Villagers](https://www.ageofempires.com/mods/details/15682/)
- [Exploding Villagers with Flying Dutchman](https://www.ageofempires.com/mods/details/15681/)
- [Kidnap Mod](https://www.ageofempires.com/mods/details/15982/)
- [No Wall Mod](https://www.ageofempires.com/mods/details/15916/)
- [Random Costs](https://www.ageofempires.com/mods/details/17281/)
- [Random Tech Costs](https://www.ageofempires.com/mods/details/17280/)
- [Random Unit Costs](https://www.ageofempires.com/mods/details/17279/)
- [3x Tech Mod](https://www.ageofempires.com/mods/details/15918/)
- [9x Tech Mod](https://www.ageofempires.com/mods/details/15919/)
- [256x Tech Mod](https://www.ageofempires.com/mods/details/15920/)

## Build instructions
### Cloning

_NOTE:_ Remember that you need to clone this repository with its submodules!

```sh
git clone --recurse-submodules https://github.com/SiegeEngineers/auto-mods.git
cd auto-mods
```

### Installing dependencies

You'll need:
- `cmake`
- `gcc`, `g++`
- `libboost-iostreams-dev`
- `libboost-program-options`
- `zlib`

#### Dependencies on Ubuntu (e.g. WSL)

```sh
sudo apt update
sudo apt install --fix-missing gcc g++ cmake \
libboost-iostreams-dev libboost-program-options-dev \
zlib1g-dev
```

### Compiling

Inside the `repository root` use the following commands:

```sh
mkdir build
cd build
cmake -DSTATIC_COMPILE=TRUE ..
cmake --build .
```

You should now have an executable `create-data-mod` in the `build` folder. Hooray!

## Usage

Execute the executable without parameters to get usage instructions.

```sh
$ ./create-data-mod
Usage: ./create-data-mod <mod-identifier> source.dat target.dat
Where <mod-identifier> is one of the following, or multiple of the following joined by a +:
    community-games
    exploding-villagers
    flying-dutchman
    kidnap
    no-wall
    random-costs
    random-tech-costs
    random-unit-costs
    x3
    x9
    x256
```

For example, in order to patch the current dat file with the Flying Dutchman modifications, one might execute
```
./create-data-mod flying-dutchman ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./empires2_x2_p1.dat
```

And then use the resulting `empires2_x2_p1.dat` in the current directory for whatever.


In order to patch the current dat file with the Flying Dutchman modifications AND the Exploding Villagers, execute
```
./create-data-mod exploding-villagers+flying-dutchman ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./empires2_x2_p1.dat
```

And then use the resulting `empires2_x2_p1.dat` in the current directory for whatever.
