# auto-mods
some data mods, generated programmatically, so they can be recreated automagically for every new patch

_If you are looking for the legacy C++ version of auto-mods, check out the [`master` branch](https://github.com/SiegeEngineers/auto-mods/tree/master/)._

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

## Setup

Create a Python3 virtualenv and install the requirements:

```sh
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

## Usage

Execute the executable without parameters to get usage instructions.

```sh
$ ./auto-mod.py --help
usage: auto-mod [-h] --mods
                {community-games,x3,x9,exploding-kings,exploding-relic-monks,exploding-villagers,exploding-villagers-extreme,flying-dutchman,kidnap,matryoshka,no-wall,nomad-king,pocket-horse,random-costs,random-costs-light,random-tech-costs,random-unit-costs,rewarding-snipes,teamwork}
                [{community-games,x3,x9,exploding-kings,exploding-relic-monks,exploding-villagers,exploding-villagers-extreme,flying-dutchman,kidnap,matryoshka,no-wall,nomad-king,pocket-horse,random-costs,random-costs-light,random-tech-costs,random-unit-costs,rewarding-snipes,teamwork} ...]
                [--logfile LOGFILE]
                input_filename output_filename

Programmatically mod Genie engine dat files (mainly for Age of Empires II Definitive Edition)

positional arguments:
  input_filename        The dat file to modify
  output_filename       Where the modified dat file shall be written to

options:
  -h, --help            show this help message and exit
  --mods {community-games,x3,x9,exploding-kings,exploding-relic-monks,exploding-villagers,exploding-villagers-extreme,flying-dutchman,kidnap,matryoshka,no-wall,nomad-king,pocket-horse,random-costs,random-costs-light,random-tech-costs,random-unit-costs,rewarding-snipes,teamwork} [{community-games,x3,x9,exploding-kings,exploding-relic-monks,exploding-villagers,exploding-villagers-extreme,flying-dutchman,kidnap,matryoshka,no-wall,nomad-king,pocket-horse,random-costs,random-costs-light,random-tech-costs,random-unit-costs,rewarding-snipes,teamwork} ...]
  --logfile LOGFILE
```

For example, in order to patch the current dat file with the Flying Dutchman modifications, one might execute
```
./auto-mod.py ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./empires2_x2_p1.dat --mods flying-dutchman
```

And then use the resulting `empires2_x2_p1.dat` in the current directory for whatever.


In order to patch the current dat file with the Flying Dutchman modifications AND the Exploding Villagers, execute
```
./auto-mod.py ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./empires2_x2_p1.dat --mods exploding-villagers flying-dutchman
```

And then use the resulting `empires2_x2_p1.dat` in the current directory for whatever.

To build all available auto-mods, activate the virtualenv and run `./create-mods.sh`.
Your base dat file must be available at `~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat`.
If it is not, replace the path in the .sh file beforehand.
The mods will be zipped and placed in the `build` folder.

## Troubleshooting

If your output looks somewhat like this:

```sh
$ ./auto-mod.py 
Traceback (most recent call last):
  File "/home/user/git/auto-mods/./auto-mod.py", line 8, in <module>
    from genieutils.datfile import DatFile
ModuleNotFoundError: No module named 'genieutils'
```

Then you forgot to activate the virtualenv.

Fix this by running `source venv/bin/activate`, then run your command again.