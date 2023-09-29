# auto-mods
some data mods, generated programmatically, so they can be recreated automagically for every new patch

> We just do everything auto!«
> *\- [MbL, 2020](https://www.youtube.com/watch?v=q879j3ydfw8)*

## Published Mods

- [Community Games Mod](https://www.ageofempires.com/mods/details/93970/)
- [Exploding Kings](https://www.ageofempires.com/mods/details/94022/)
- [Exploding Kings with Rewarding Snipes](https://www.ageofempires.com/mods/details/94278/)
- [Exploding Nomad Kings](https://www.ageofempires.com/mods/details/125764/)
- [Exploding Villagers](https://www.ageofempires.com/mods/details/93961/)
- [Exploding Villagers Extreme](https://www.ageofempires.com/mods/details/93971/)
- [Exploding Villagers with Flying Dutchman](https://www.ageofempires.com/mods/details/93966/)
- [Exploding Villagers with Kidnap Mod](https://www.ageofempires.com/mods/details/93969/)
- [Flying Dutchman](https://www.ageofempires.com/mods/details/93967/)
- [Kidnap Mod](https://www.ageofempires.com/mods/details/93968/)
- [Matryoshka Mod](https://www.ageofempires.com/mods/details/93973/)
- [No Wall Mod](https://www.ageofempires.com/mods/details/93962/)
- [Nomad King](https://www.ageofempires.com/mods/details/123629/)
- [Pocket Horse Mod](https://www.ageofempires.com/mods/details/93974/)
- [Random Costs](https://www.ageofempires.com/mods/details/93975/)
- [Random Costs Light](https://www.ageofempires.com/mods/details/185441/)
- [Random Tech Costs](https://www.ageofempires.com/mods/details/93964/)
- [Random Unit Costs](https://www.ageofempires.com/mods/details/93963/)
- [Relic Monk Boom](https://www.ageofempires.com/mods/details/166337/)
- [Rewarding Snipes](https://www.ageofempires.com/mods/details/94277/)
- [Teamwork Mod](https://www.ageofempires.com/mods/details/93972/)
- [3x Tech Mod](https://www.ageofempires.com/mods/details/93960/)
- [9x Tech Mod](https://www.ageofempires.com/mods/details/93959/)
- ~~256x Tech Mod~~ _discontinued_

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
- `lz4`

#### Dependencies 

**on Ubuntu (e.g. WSL)**
```sh
sudo apt update
sudo apt install --fix-missing gcc g++ cmake \
libboost-iostreams-dev libboost-program-options-dev \
zlib1g-dev liblz4-dev
```

**on OSX**
```sh
xcode-select --install #this is to make sure xcode is installed for access to zlib1g-dev and liblz4-dev
brew install cmake
brew install boost
brew install gcc
brew install gdb
```


### Compiling

Inside the `repository root` use the following commands:

```sh
mkdir build
cd build
cmake -DSTATIC_COMPILE=TRUE ..
cmake --build .
```

Note:  You can also run `./scripts/build.sh` from the project root.

You should now have an executable `create-data-mod` in the `build` folder. Hooray!

### Debugging (GDB on Ubuntu)

1. Ensure gcc tools are installed: `sudo apt-get install build-essential gdb`

Now we have two debug options:

2. Debug via GDB on the terminal...
```
./buildForDebugger.sh  # make sure to run this from the project root
gdb build/create-data-mod
> b main
> run
```

3. OR use the VSCode visual debugger, by going to the debug sidebar on the left, and running the `(gdb) Build and Launch` configuration.  This will automatically make a debug build and run it.  You can set breakpoints in the visual debugger.

*Right now it runs `create-data-mod` with no parameters.  For testing, you can add arguments to `.vscode/launch.json` in the `args` parameter.*

## Usage

Execute the executable without parameters to get usage instructions.

```sh
$ ./create-data-mod
Usage: ./create-data-mod <mod-identifier> source.dat target.dat
Where <mod-identifier> is one of the following, or multiple of the following joined by a +:
    community-games
    exploding-villagers
    exploding-villagers-extreme
    exploding-kings
    exploding-relic-monks
    rewarding-snipes
    flying-dutchman
    kidnap
    no-wall
    random-costs
    random-costs-light
    random-tech-costs
    random-unit-costs
    teamwork
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
