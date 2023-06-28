#include "duplicate_techs.h"
#include <list>
#include <vector>
#include "genie/dat/DatFile.h"
#include "ids.h"


void duplicateTech(genie::DatFile *df, const genie::Tech &tech, int totalCount) {
    for (int i = 0; i < (totalCount - 1); ++i) {
        df->Techs.push_back(tech);
    }
    std::cout << "Added Tech '" << tech.Name << "' for a total of " << totalCount << " instances" << std::endl;
}


void duplicateTechs(genie::DatFile *df, int totalCount) {
    const std::list<int> techsToDuplicate = {
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
        FIRST_CRUSADE,
        HAUBERK,
        BURGUNDIAN_VINEYARDS,
        FLEMISH_REVOLUTION,
        WAGENBURG_TACTICS,
        SZLACHTA_PRIVILEGES,
        BALLISTAS,
        COMITATENSES,
    };

    for (int techId : techsToDuplicate) {
        genie::Tech tech = df->Techs.at(techId);
        duplicateTech(df, tech, totalCount);
    }
}
