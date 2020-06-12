# aoe2de-data-mods
some data mods, generated programmatically, so they can be recreated automagically for every new patch

## Published Mods

- [Flying Dutchman](https://www.ageofempires.com/mods/details/15644/)
- [Exploding Villagers](https://www.ageofempires.com/mods/details/15682/)
- [Exploding Villagers with Flying Dutchman](https://www.ageofempires.com/mods/details/15681/)

## Cloning and Compiling

You'll need cmake, build tools for C++, boost libraries, … 

Just install stuff until it stops complaining I guess, sorry.

```sh
git clone --recurse-submodules https://github.com/HSZemi/aoe2de-data-mods.git
cd aoe2de-data-mods
mkdir build
cd build
cmake ..
cmake --build .
```

You should now have an executable `create-data-mod` in the `build` folder. Hooray!

## Usage

Execute the executable without parameters to get usage instructions.

```sh
$ ./create-data-mod
Usage: ./create-data-mod <mod-identifier> source.dat target.dat
Where <mod-identifier> is one of the following, or multiple of the following joined by a +:
    exploding-villagers
    flying-dutchman
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

