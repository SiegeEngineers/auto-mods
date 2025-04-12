#! /bin/bash

rm -rf build
mkdir -p build/community_games/resources/_common/dat
mkdir -p build/flying_dutchman/resources/_common/dat
mkdir -p build/exploding_villagers/resources/_common/dat
mkdir -p build/exploding_villagers_extreme/resources/_common/dat
mkdir -p build/exploding_villagers_flying_dutchman/resources/_common/dat
mkdir -p build/exploding_kings/resources/_common/dat
mkdir -p build/exploding_relic_monks/resources/_common/dat
mkdir -p build/exploding_nomad_kings/resources/_common/dat
mkdir -p build/rewarding_snipes/resources/_common/dat
mkdir -p build/exploding_kings_rewarding_snipes/resources/_common/dat
mkdir -p build/kidnap/resources/_common/dat
mkdir -p build/exploding_villagers_kidnap/resources/_common/dat
mkdir -p build/matryoshka/resources/_common/dat
mkdir -p build/no_wall/resources/_common/dat
mkdir -p build/nomad_king/resources/_common/dat
mkdir -p build/pocket_horse/resources/_common/dat
mkdir -p build/random-costs/resources/_common/dat
mkdir -p build/random-costs-light/resources/_common/dat
mkdir -p build/random-tech-costs/resources/_common/dat
mkdir -p build/random-unit-costs/resources/_common/dat
mkdir -p build/teamwork/resources/_common/dat
mkdir -p build/x3/resources/_common/dat
mkdir -p build/x9/resources/_common/dat
mkdir -p build/heroes_and_villains/resources/_common/dat
mkdir -p build/heroes_and_villains_king_boom_rewarding_snipes/resources/_common/dat


./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/community_games/resources/_common/dat/empires2_x2_p1.dat --mods community-games 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/flying_dutchman/resources/_common/dat/empires2_x2_p1.dat --mods flying-dutchman 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_villagers/resources/_common/dat/empires2_x2_p1.dat --mods exploding-villagers 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_villagers_extreme/resources/_common/dat/empires2_x2_p1.dat --mods exploding-villagers-extreme 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_relic_monks/resources/_common/dat/empires2_x2_p1.dat --mods exploding-relic-monks 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_kings/resources/_common/dat/empires2_x2_p1.dat --mods exploding-kings 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/rewarding_snipes/resources/_common/dat/empires2_x2_p1.dat --mods rewarding-snipes 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_kings_rewarding_snipes/resources/_common/dat/empires2_x2_p1.dat --mods exploding-kings rewarding-snipes 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_villagers_flying_dutchman/resources/_common/dat/empires2_x2_p1.dat --mods exploding-villagers flying-dutchman 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/kidnap/resources/_common/dat/empires2_x2_p1.dat --mods kidnap 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_villagers_kidnap/resources/_common/dat/empires2_x2_p1.dat --mods exploding-villagers kidnap 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/matryoshka/resources/_common/dat/empires2_x2_p1.dat --mods matryoshka 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/no_wall/resources/_common/dat/empires2_x2_p1.dat --mods no-wall 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/nomad_king/resources/_common/dat/empires2_x2_p1.dat --mods nomad-king 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/exploding_nomad_kings/resources/_common/dat/empires2_x2_p1.dat --mods nomad-king exploding-kings 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/pocket_horse/resources/_common/dat/empires2_x2_p1.dat --mods pocket-horse 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/random-costs/resources/_common/dat/empires2_x2_p1.dat --mods random-costs 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/random-costs-light/resources/_common/dat/empires2_x2_p1.dat --mods random-costs-light 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/random-tech-costs/resources/_common/dat/empires2_x2_p1.dat --mods random-tech-costs 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/random-unit-costs/resources/_common/dat/empires2_x2_p1.dat --mods random-unit-costs 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/teamwork/resources/_common/dat/empires2_x2_p1.dat --mods teamwork 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/x3/resources/_common/dat/empires2_x2_p1.dat --mods x3 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/x9/resources/_common/dat/empires2_x2_p1.dat --mods x9 
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/heroes_and_villains/resources/_common/dat/empires2_x2_p1.dat --mods heroes-and-villains
./auto-mod.py --cache ~/aoe/Aoe2DE\ proton/resources/_common/dat/empires2_x2_p1.dat ./build/heroes_and_villains_king_boom_rewarding_snipes/resources/_common/dat/empires2_x2_p1.dat --mods exploding-kings rewarding-snipes heroes-and-villains


cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/community_games/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/flying_dutchman/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_villagers/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_villagers_extreme/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_relic_monks/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_kings/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/rewarding_snipes/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_kings_rewarding_snipes/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_villagers_flying_dutchman/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/kidnap/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_villagers_kidnap/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/matryoshka/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/no_wall/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/nomad_king/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/exploding_nomad_kings/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/pocket_horse/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/random-costs/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/random-costs-light/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/random-tech-costs/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/random-unit-costs/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/teamwork/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/x3/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/x9/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/heroes_and_villains/resources/_common/dat/civilizations.json
cp ~/aoe/Aoe2DE\ proton/resources/_common/dat/civilizations.json ./build/heroes_and_villains_king_boom_rewarding_snipes/resources/_common/dat/civilizations.json


cd build/community_games
zip -r ../community_games.zip *
cd ../..

cd build/flying_dutchman
zip -r ../flying_dutchman.zip *
cd ../..

cd build/exploding_villagers
zip -r ../exploding_villagers.zip *
cd ../..

cd build/exploding_villagers_extreme
zip -r ../exploding_villagers_extreme.zip *
cd ../..

cd build/exploding_kings
zip -r ../exploding_kings.zip *
cd ../..

cd build/exploding_relic_monks
zip -r ../exploding_relic_monks.zip *
cd ../..

cd build/rewarding_snipes
zip -r ../rewarding_snipes.zip *
cd ../..

cd build/exploding_kings_rewarding_snipes
zip -r ../exploding_kings_rewarding_snipes.zip *
cd ../..

cd build/exploding_villagers_flying_dutchman
zip -r ../exploding_villagers_flying_dutchman.zip *
cd ../..

cd build/kidnap
zip -r ../kidnap.zip *
cd ../..

cd build/exploding_villagers_kidnap
zip -r ../exploding_villagers_kidnap.zip *
cd ../..

cd build/matryoshka
zip -r ../matryoshka.zip *
cd ../..

cd build/no_wall
zip -r ../no_wall.zip *
cd ../..

cd build/nomad_king
zip -r ../nomad_king.zip *
cd ../..

cd build/exploding_nomad_kings
zip -r ../exploding_nomad_kings.zip *
cd ../..

cd build/pocket_horse
zip -r ../pocket_horse.zip *
cd ../..

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

cd build/teamwork
zip -r ../teamwork.zip *
cd ../..

cd build/x3
zip -r ../x3.zip *
cd ../..

cd build/x9
zip -r ../x9.zip *
cd ../..

cd build/heroes_and_villains
zip -r ../heroes_and_villains.zip *
cd ../..

cd build/heroes_and_villains_king_boom_rewarding_snipes
zip -r ../heroes_and_villains_king_boom_rewarding_snipes.zip *
cd ../..
