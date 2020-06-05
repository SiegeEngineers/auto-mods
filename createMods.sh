#! /bin/bash

rm -rf mods
mkdir -p mods/flying_dutchman/resources/_common/dat
./build/create-data-mod flying-dutchman ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/flying_dutchman/resources/_common/dat/empires2_x2_p1.dat
cd mods/flying_dutchman
zip -r ../flying_dutchman.zip *
cd ../..
