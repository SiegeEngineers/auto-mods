#! /bin/bash
set -ex

cd "$(dirname "$(realpath "$0")")";

rm -rf build
rm -rf automod.log

mkdir -p build/random-costs/resources/_common/dat
mkdir -p build/random-costs-light/resources/_common/dat
mkdir -p build/random-tech-costs/resources/_common/dat
mkdir -p build/random-unit-costs/resources/_common/dat

./auto-mod.py --cache ./dat/empires2_x2_p1.dat ./build/random-costs/resources/_common/dat/empires2_x2_p1.dat --mods random-costs --logfile ./automod.log
./auto-mod.py --cache ./dat/empires2_x2_p1.dat ./build/random-costs-light/resources/_common/dat/empires2_x2_p1.dat --mods random-costs-light --logfile ./automod.log
./auto-mod.py --cache ./dat/empires2_x2_p1.dat ./build/random-tech-costs/resources/_common/dat/empires2_x2_p1.dat --mods random-tech-costs --logfile ./automod.log
./auto-mod.py --cache ./dat/empires2_x2_p1.dat ./build/random-unit-costs/resources/_common/dat/empires2_x2_p1.dat --mods random-unit-costs --logfile ./automod.log

cp ./dat/civilizations.json ./build/random-costs/resources/_common/dat/civilizations.json
cp ./dat/civilizations.json ./build/random-costs-light/resources/_common/dat/civilizations.json
cp ./dat/civilizations.json ./build/random-tech-costs/resources/_common/dat/civilizations.json
cp ./dat/civilizations.json ./build/random-unit-costs/resources/_common/dat/civilizations.json

cd build/random-costs
zip -r ../random-costs.zip *
cd ../..

cd build/random-costs-light
zip -r ../random-costs-light.zip *
cd ../..

cd build/random-tech-costs
zip -r ../random-tech-costs.zip *
cd ../..

cd build/random-unit-costs
zip -r ../random-unit-costs.zip *
cd ../..

cp build/*.zip html/
cp automod.log html/

BUILD_DATE=$(date --rfc-3339 seconds --utc)
sed -i s"/BUILD_DATE/$BUILD_DATE/g" ./html/index.html
