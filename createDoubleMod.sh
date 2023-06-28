#! /bin/bash

rm -rf mods
mkdir -p mods/double-costs/resources/_common/dat

./build/create-data-mod double-costs ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./mods/double-costs/resources/_common/dat/empires2_x2_p1.dat

cd mods/double-costs
zip -r ../double-costs.zip *
cd ../..
