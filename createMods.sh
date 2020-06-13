#! /bin/bash

rm -rf mods
mkdir -p mods/flying_dutchman/resources/_common/dat
mkdir -p mods/exploding_villagers/resources/_common/dat
mkdir -p mods/exploding_villagers_flying_dutchman/resources/_common/dat
mkdir -p mods/no_wall/resources/_common/dat
mkdir -p mods/x3/resources/_common/dat
mkdir -p mods/x9/resources/_common/dat
mkdir -p mods/x256/resources/_common/dat

./build/create-data-mod flying-dutchman ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/flying_dutchman/resources/_common/dat/empires2_x2_p1.dat
./build/create-data-mod exploding-villagers ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/exploding_villagers/resources/_common/dat/empires2_x2_p1.dat
./build/create-data-mod exploding-villagers+flying-dutchman ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/exploding_villagers_flying_dutchman/resources/_common/dat/empires2_x2_p1.dat
./build/create-data-mod no-wall ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/no_wall/resources/_common/dat/empires2_x2_p1.dat
./build/create-data-mod x3 ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/x3/resources/_common/dat/empires2_x2_p1.dat
./build/create-data-mod x9 ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/x9/resources/_common/dat/empires2_x2_p1.dat
./build/create-data-mod x256 ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/x256/resources/_common/dat/empires2_x2_p1.dat

cd mods/flying_dutchman
zip -r ../flying_dutchman.zip *
cd ../..

cd mods/exploding_villagers
zip -r ../exploding_villagers.zip *
cd ../..

cd mods/exploding_villagers_flying_dutchman
zip -r ../exploding_villagers_flying_dutchman.zip *
cd ../..

cd mods/no_wall
zip -r ../no_wall.zip *
cd ../..

cd mods/x3
zip -r ../x3.zip *
cd ../..

cd mods/x9
zip -r ../x9.zip *
cd ../..

cd mods/x256
zip -r ../x256.zip *
cd ../..
