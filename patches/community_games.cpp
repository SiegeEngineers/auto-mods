#include <iostream>
#include "genie/dat/DatFile.h"
#include "ids.h"


void addPopulationCostToBombardTower(genie::DatFile *df) {
	std::cout << "Adding population cost to bombard towers of all civs" << std::endl;
    for (genie::Civ &civ : df->Civs) {
        genie::Unit &bombard_tower = civ.Units.at(BOMBARD_TOWER);
        genie::ResourceUsage<int16_t, int16_t, int16_t> &resourceCosts = bombard_tower.Creatable.ResourceCosts.at(2);
        resourceCosts.Type = TYPE_POPULATION_HEADROOM;
        resourceCosts.Amount = 1;
        resourceCosts.Flag = 0;

        bombard_tower.ResourceStorages.at(0).Type = TYPE_POPULATION_HEADROOM;
        bombard_tower.ResourceStorages.at(0).Amount = -1;
        bombard_tower.ResourceStorages.at(0).Flag = 2;

        bombard_tower.ResourceStorages.at(1).Type = TYPE_CURRENT_POPULATION;
        bombard_tower.ResourceStorages.at(1).Amount = 1;
        bombard_tower.ResourceStorages.at(1).Flag = 2;

        bombard_tower.ResourceStorages.at(2).Type = TYPE_TOTAL_UNITS_OWNED;
        bombard_tower.ResourceStorages.at(2).Amount = 1;
        bombard_tower.ResourceStorages.at(2).Flag = 1;
    }
}

void addGreatHallTech(genie::DatFile *df) {
	std::cout << "Adding great hall tech" << std::endl;
    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 1; // Resource Modifier
    effectCommand->A = 32; // Resource: Bonus Population Cap
    effectCommand->B = 1; // Mode: +/-
    effectCommand->D = 10; // Amount +/-

    auto effect = new genie::Effect();
    effect->Name = "Great Hall";
    effect->EffectCommands.push_back(*effectCommand);

    df->Effects.push_back(*effect);
    size_t greatHallEffectId = df->Effects.size() - 1;

    auto tech = new genie::Tech();
    tech->Name = "The Great Hall";
    tech->RequiredTechs.push_back(BOMBARD_TOWER_TECH);
    tech->ResearchTime = 5;
    tech->IconID = 103;
    tech->ButtonID = 1;
    tech->EffectID = greatHallEffectId;
    tech->ResearchLocation = BOMBARD_TOWER;

    tech->ResourceCosts.at(0).Type = TYPE_WOOD;
    tech->ResourceCosts.at(0).Amount = 400;
    tech->ResourceCosts.at(0).Flag = 1;

    tech->ResourceCosts.at(1).Type = TYPE_GOLD;
    tech->ResourceCosts.at(1).Amount = 600;
    tech->ResourceCosts.at(1).Flag = 1;

    df->Techs.push_back(*tech);
}

void addElitePetard(genie::DatFile *df) {
    int elitePetardId = -1;
    for (genie::Civ &civ : df->Civs) {
        genie::Unit elitePetard = civ.Units.at(PETARD);
        elitePetard.Enabled = 0;
        elitePetard.Name = "Elite Petard";
        elitePetard.HitPoints = 60;
        elitePetard.Speed = 1.05;
        elitePetard.Type50.DisplayedAttack = 45;
        elitePetard.Type50.BlastWidth = 1.5;
        elitePetard.Type50.BlastAttackLevel = 1;
        elitePetard.Type50.Attacks.at(0).Amount = 150;
        elitePetard.Type50.Attacks.at(1).Amount = 650;
        elitePetard.Type50.Attacks.at(2).Amount = 45;
        elitePetard.Type50.Attacks.at(3).Amount = 70;
        elitePetard.Type50.Attacks.at(4).Amount = 1000;
        elitePetard.Type50.Armours.at(1).Amount = 3;
        elitePetard.Creatable.DisplayedPierceArmour = 3;
        elitePetardId = civ.Units.size();
		std::cout << "Adding elite petard for civ " << civ.Name << " with ID " << elitePetardId << std::endl;
        civ.Units.push_back(elitePetard);
        civ.UnitPointers.push_back(1);
    }

    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 3; // Upgrade Unit
    effectCommand->A = PETARD;
    effectCommand->B = elitePetardId;

    auto effect = new genie::Effect();
    effect->Name = "Elite Petard";
    effect->EffectCommands.push_back(*effectCommand);
    int elitePetardEffectId = df->Effects.size();
    df->Effects.push_back(*effect);

    auto tech = new genie::Tech();
    tech->Name = "Elite Petard";
    tech->RequiredTechs.push_back(CHEMISTRY);
    tech->RequiredTechCount = 1;
    tech->ResearchTime = 40;
    tech->IconID = 105;
    tech->ButtonID = 9;
    tech->EffectID = elitePetardEffectId;
    tech->ResearchLocation = CASTLE;

    tech->ResourceCosts.at(0).Type = TYPE_FOOD;
    tech->ResourceCosts.at(0).Amount = 450;
    tech->ResourceCosts.at(0).Flag = 1;

    tech->ResourceCosts.at(1).Type = TYPE_GOLD;
    tech->ResourceCosts.at(1).Amount = 450;
    tech->ResourceCosts.at(1).Flag = 1;

	std::cout << "Adding Elite Petard Tech with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(*tech);
}

void modifyCaravanCost(genie::DatFile *df, int amountFood, int amountGold) {
	std::cout << "Setting the cost of Caravan (" << TECH_CARAVAN << ") to " << amountFood << " Food, " << amountGold
         << " Gold" << std::endl;
    genie::Tech &caravan = df->Techs.at(TECH_CARAVAN);
    caravan.ResourceCosts.at(0).Type = TYPE_FOOD;
    caravan.ResourceCosts.at(0).Amount = amountFood;
    caravan.ResourceCosts.at(0).Flag = 1;

    caravan.ResourceCosts.at(1).Type = TYPE_GOLD;
    caravan.ResourceCosts.at(1).Amount = amountGold;
    caravan.ResourceCosts.at(1).Flag = 1;
}

void makeTreesContain200Wood(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit: civ.Units) {
            for (auto storage : unit.ResourceStorages) {
                if (storage.Type == TYPE_WOOD && storage.Amount > 50) {
					std::cout << "Setting amount of wood in " << unit.Name << " (" << unit.ID << ") to 200" << std::endl;
                    storage.Amount = 200;
                }
            }
        }
    }
}

void configureCommunityGamesMod(genie::DatFile *df) {
    addPopulationCostToBombardTower(df);
    addGreatHallTech(df);
    addElitePetard(df);
    modifyCaravanCost(df, 800, 200);
    makeTreesContain200Wood(df);
}
