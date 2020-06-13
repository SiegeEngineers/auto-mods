#include "genie/dat/DatFile.h"
#include "genie/dat/TechageEffect.h"
#include "patches.h"
#include "ids.h"
#include <list>
#include <set>

using namespace std;

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
