# auto-mods
some data mods, generated programmatically, so they can be recreated automagically for every new patch

> We just do everything auto!«
> *\- [MbL, 2020](https://www.youtube.com/watch?v=q879j3ydfw8)*

## Published Mods

- [Community Games Mod](https://www.ageofempires.com/mods/details/15917/)
- [Flying Dutchman](https://www.ageofempires.com/mods/details/15644/)
- [Exploding Villagers](https://www.ageofempires.com/mods/details/15682/)
- [Exploding Villagers Extreme](https://www.ageofempires.com/mods/details/47898/)
- [Exploding Villagers with Flying Dutchman](https://www.ageofempires.com/mods/details/15681/)
- [Kidnap Mod](https://www.ageofempires.com/mods/details/15982/)
- [Matryoshka Mod](https://www.ageofempires.com/mods/details/71435/)
- [No Wall Mod](https://www.ageofempires.com/mods/details/15916/)
- [Pocket Horse Mod](https://www.ageofempires.com/mods/details/71440/)
- [Random Costs](https://www.ageofempires.com/mods/details/17281/)
- [Random Tech Costs](https://www.ageofempires.com/mods/details/17280/)
- [Random Unit Costs](https://www.ageofempires.com/mods/details/17279/)
- [Teamwork Mod](https://www.ageofempires.com/mods/details/43753/)
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
brew install cmake
brew install boost
brew install gcc
brew install g++
brew install gdb
```
Also change the 'cMakeList.txt' comipile CMAKE_CXX_FLAGS from -static-libstdc++ to -std=c++11 for OSX


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
    flying-dutchman
    kidnap
    no-wall
    random-costs
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
