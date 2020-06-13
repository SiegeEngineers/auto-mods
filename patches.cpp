#include "genie/dat/DatFile.h"
#include "genie/dat/TechageEffect.h"
#include "patches.h"
#include "ids.h"
#include <list>
#include <set>

using namespace std;

void configureCommunityGamesMod(genie::DatFile *df) {
    addPopulationCostToBombardTower(df);
    addGreatHallTech(df);
    addElitePetard(df);
    modifyCaravanCost(df, 800, 200);
    makeTreesContain200Wood(df);
}

void addPopulationCostToBombardTower(genie::DatFile *df) {
    cout << "Adding population cost to bombard towers of all civs" << endl;
    for (genie::Civ &civ : df->Civs) {
        genie::ResourceUsage<int16_t, int16_t, int16_t> &resourceCosts = civ.Units.at(BOMBARD_TOWER)
                .Creatable.ResourceCosts.at(2);
        resourceCosts.Type = TYPE_POPULATION_HEADROOM;
        resourceCosts.Amount = 1;
        resourceCosts.Flag = 0;

        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(0).Type = TYPE_POPULATION_HEADROOM;
        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(0).Amount = -1;
        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(0).Flag = 2;

        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(1).Type = TYPE_CURRENT_POPULATION;
        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(1).Amount = 1;
        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(1).Flag = 2;

        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(2).Type = TYPE_TOTAL_UNITS_OWNED;
        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(2).Amount = 1;
        civ.Units.at(BOMBARD_TOWER).ResourceStorages.at(2).Flag = 1;
    }
}

void addGreatHallTech(genie::DatFile *df) {
    cout << "Adding great hall tech" << endl;
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
        cout << "Adding elite petard for civ " << civ.Name << " with ID " << elitePetardId << endl;
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

    cout << "Adding Elite Petard Tech with id " << df->Techs.size() << endl;
    df->Techs.push_back(*tech);
}

void modifyCaravanCost(genie::DatFile *df, int amountFood, int amountGold) {
    cout << "Setting the cost of Caravan (" << TECH_CARAVAN << ") to " << amountFood << " Food, " << amountGold
         << " Gold" << endl;
    df->Techs.at(TECH_CARAVAN).ResourceCosts.at(0).Type = TYPE_FOOD;
    df->Techs.at(TECH_CARAVAN).ResourceCosts.at(0).Amount = amountFood;
    df->Techs.at(TECH_CARAVAN).ResourceCosts.at(0).Flag = 1;

    df->Techs.at(TECH_CARAVAN).ResourceCosts.at(1).Type = TYPE_GOLD;
    df->Techs.at(TECH_CARAVAN).ResourceCosts.at(1).Amount = amountGold;
    df->Techs.at(TECH_CARAVAN).ResourceCosts.at(1).Flag = 1;
}

void makeTreesContain200Wood(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit: civ.Units) {
            for (auto storage : unit.ResourceStorages) {
                if (storage.Type == TYPE_WOOD && storage.Amount > 50) {
                    cout << "Setting amount of wood in " << unit.Name << " (" << unit.ID << ") to 200" << endl;
                    storage.Amount = 200;
                }
            }
        }
    }
}

void configureExplodingVillagers(genie::DatFile *df) {
    set<int> villagers = {
            ID_FISHING_SHIP,
            ID_TRADE_COG,
            ID_TRADE_CART_EMPTY,
            ID_TRADE_CART_FULL,
            ID_VILLAGER_BASE_M,
            ID_VILLAGER_BASE_F,
            ID_VILLAGER_FARMER_M,
            ID_VILLAGER_FARMER_F,
            ID_VILLAGER_SHEPHERD_M,
            ID_VILLAGER_SHEPHERD_F,
            ID_VILLAGER_FORAGER_M,
            ID_VILLAGER_FORAGER_F,
            ID_VILLAGER_HUNTER_M,
            ID_VILLAGER_HUNTER_F,
            ID_VILLAGER_FISHER_M,
            ID_VILLAGER_FISHER_F,
            ID_VILLAGER_WOOD_M,
            ID_VILLAGER_WOOD_F,
            ID_VILLAGER_GOLD_M,
            ID_VILLAGER_GOLD_F,
            ID_VILLAGER_STONE_M,
            ID_VILLAGER_STONE_F,
            ID_VILLAGER_BUILDER_M,
            ID_VILLAGER_BUILDER_F,
            ID_VILLAGER_REPAIRER_M,
            ID_VILLAGER_REPAIRER_F,
    };

    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            if (villagers.find(unit.ID) != villagers.end()) {
                unit.DeadUnitID = ID_SABOTEUR;
                cout << "Patched Villager unit " << unit.ID << " for civ " << civ.Name << "\n";
            } else if (unit.ID == ID_SABOTEUR) {
                unit.HitPoints = 0;
                unit.Type50.Attacks.at(0).Amount = 50;
                unit.Type50.Attacks.at(1).Amount = 90;
                unit.Type50.Attacks.at(2).Amount = 0;
                unit.Type50.MaxRange = 2;
                unit.Type50.BlastAttackLevel = 1; // cut trees
                unit.TrainSound = -1;
                unit.WwiseTrainSoundID = 0; // prevent melee unit train sound from playing
                cout << "Patched Saboteur unit for civ " << civ.Name << "\n";
            }
        }
    }
}

void duplicateTechs(genie::DatFile *df, int totalCount) {

    const list<int> techsToDuplicate = {
            YEOMEN,
            EL_DORADO,
            FUROR_CELTICA,
            DRILL,
            MAHOUTS,
            TOWN_WATCH,
            ZEALOTRY,
            ARTILLERY,
            CRENELLATIONS,
            CROP_ROTATION,
            HEAVY_PLOW,
            HORSE_COLLAR,
            GUILDS,
            BANKING,
            ATHEISM,
            LOOM,
            GARLAND_WARS,
            HUSBANDRY,
            FAITH,
            CHEMISTRY,
            CARAVAN,
            BERSERKERGANG,
            MASONRY,
            ARCHITECTURE,
            ROCKETRY,
            TREADMILL_CRANE,
            GOLD_MINING,
            KATAPARUTO,
            LOGISTICA,
            GILLNETS,
            FORGING,
            IRON_CASTING,
            SCALE_MAIL_ARMOR,
            BLAST_FURNACE,
            CHAIN_MAIL_ARMOR,
            PLATE_MAIL_ARMOR,
            PLATE_BARDING_ARMOR,
            SCALE_BARDING_ARMOR,
            CHAIN_BARDING_ARMOR,
            BEARDED_AXE,
            GOLD_SHAFT_MINING,
            FLETCHING,
            BODKIN_ARROW,
            BRACER,
            DOUBLE_BIT_AXE,
            BOW_SAW,
            PADDED_ARCHER_ARMOR,
            LEATHER_ARCHER_ARMOR,
            WHEELBARROW,
            SQUIRES,
            RING_ARCHER_ARMOR,
            TWO_MAN_SAW,
            BLOCK_PRINTING,
            SANCTITY,
            ILLUMINATION,
            HAND_CART,
            FERVOR,
            STONE_MINING,
            STONE_SHAFT_MINING,
            TOWN_PATROL,
            CONSCRIPTION,
            SAPPERS,
            SHIPWRIGHT,
            CAREENING,
            DRY_DOCK,
            SIEGE_ENGINEERS,
            HOARDINGS,
            HEATED_SHOT,
            BLOODLINES,
            PARTHIAN_TACTICS,
            THUMB_RING,
            SUPREMACY,
            HERBAL_MEDICINE,
            SHINKICHON,
            PERFUSION,
            ATLATL,
            WARWOLF,
            GREAT_WALL,
            CHIEFTAINS,
            GREEK_FIRE,
            STRONGHOLD,
            YASAMA,
            OBSIDIAN_ARROWS,
            PANOKSEON,
            KAMANDARAN,
            IRONCLAD,
            SIPAHI,
            INQUISITION,
            CHIVALRY,
            PAVISE,
            SILK_ROAD,
            SULTANS,
            SHATAGNI,
            ORTHODOXY,
            DRUZHINA,
            RECURVE_BOW,
            FABRIC_SHIELDS,
            CARRACK,
            ARQUEBUS,
            ROYAL_HEIRS,
            TORSION_ENGINES,
            TIGUI,
            FARIMBA,
            KASBAH,
            MAGHRABI_CAMELS,
            ARSON,
            ARROWSLITS,
            TUSK_SWORDS,
            DOUBLE_CROSSBOW,
            FORCED_LEVY,
            HOWDAH,
            MANIPUR_CAVALRY,
            CHATRAS,
            PAPER_MONEY,
            STIRRUPS,
            BAGAINS,
            SILK_ARMOR,
            TIMURID_SIEGECRAFT,
            STEPPE_HUSBANDRY,
            CUMAN_MERCENARIES,
            HILL_FORTS,
            TOWER_SHIELDS,
            SUPPLIES,
    };

    for (int techId: techsToDuplicate) {
        genie::Tech tech = df->Techs.at(techId);
        duplicateTech(df, tech, totalCount);
    }

}

void duplicateTech(genie::DatFile *df, const genie::Tech &tech, int totalCount) {
    for (int i = 0; i < (totalCount - 1); ++i) {
        df->Techs.push_back(tech);
    }
    cout << "Added Tech '" << tech.Name << "' for a total of " << totalCount << " instances" << endl;
}

void makeTransportShipsFly(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            if (unit.ID == ID_TRANSPORT_SHIP) {
                unit.FlyMode = 1; // true
                unit.TerrainRestriction = 0; // All
                unit.CollisionSize = {0, 0, 0}; // walk through everything, like flying
                cout << "Patched Transport Ship unit for civ " << civ.Name << "\n";
            }
        }
    }
}

void disableWalls(genie::DatFile *df) {
    const list<int> unitsToDisable = {
            PALISADE_WALL,
    };

    const list<int> techsToDisable = {
            TECH_PALISADE_GATE,
            TECH_STONE_WALLS,
    };

    const list<int> techsToHide = {
            TECH_FORTIFIED_WALL,
    };

    for (int unitId: unitsToDisable) {
        cout << "Disabling unit with id " << unitId << " for all civs" << endl;
        for (genie::Civ &civ : df->Civs) {
            civ.Units.at(unitId).Enabled = 0;
        }
    }

    for (int techId: techsToDisable) {
        genie::Tech *tech = &df->Techs.at(techId);
        cout << "Disabling the effect of tech with id " << techId << " ('" << tech->Name << "')" << endl;
        disableTechEffect(tech);
    }

    for (int techId: techsToHide) {
        genie::Tech *tech = &df->Techs.at(techId);
        cout << "Disabling the research location of tech with id " << techId << " ('" << tech->Name << "')" << endl;
        disableTechResearchLocation(tech);
    }
}

void disableTechEffect(genie::Tech *tech) {
    tech->EffectID = -1;
}

void disableTechResearchLocation(genie::Tech *tech) {
    tech->ResearchLocation = -1;
}

void preventRamsAndSiegeTowersFromBoardingTransportShips(genie::DatFile *df) {
    for (genie::Civ &civ : df->Civs) {
        for (genie::Unit &unit : civ.Units) {
            if (unit.ID == ID_RAM || unit.ID == ID_CAPPED_RAM || unit.ID == ID_SIEGE_RAM || unit.ID == ID_SIEGE_TOWER) {
                list<int> toDelete;
                for (int i = 0; i < unit.Bird.TaskList.size(); i++) {
                    genie::Task task = unit.Bird.TaskList.at(i);
                    if (task.ActionType == ACTION_TYPE_GARRISON && task.ClassID == CLASS_TRANSPORT_BOAT) {
                        toDelete.push_front(i);
                    }
                }

                for (int i:toDelete) {
                    unit.Bird.TaskList.erase(unit.Bird.TaskList.begin() + i);
                }
                cout << "Patched unit " << unit.Name << " (" << unit.ID << ") unit for civ " << civ.Name << "\n";
            }
        }
    }
}
