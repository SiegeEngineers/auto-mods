#include "exploding_kings.h"
#include "genie/dat/DatFile.h"
#include "ids.h"


void patchSaboteurForFirstExplosion(genie::Civ &civ) {
    genie::Unit &saboteur = civ.Units.at(ID_SABOTEUR);
    saboteur.HitPoints = -1;
    saboteur.DeadUnitID = ID_FURIOUS_THE_MONKEY_BOY;
    saboteur.Type50.Attacks.at(0).Amount = 9999;
    saboteur.Type50.Attacks.at(1).Amount = 9999;
    saboteur.Type50.Attacks.at(2).Amount = 9999;
    saboteur.Type50.MaxRange = 25;
    saboteur.Type50.BlastWidth = 25;
    saboteur.Type50.BlastAttackLevel = 0;
    saboteur.WwiseTrainSoundID = 0;
    std::cout << "Patched Saboteur unit for civ " << civ.Name << "\n";
}

void patchMonkeyForSecondExplosion(genie::Civ &civ) {
    genie::Unit &furious = civ.Units.at(ID_FURIOUS_THE_MONKEY_BOY);
    furious.HitPoints = -1;
    furious.Class = CLASS_PETARD;
    furious.Type50.Attacks.at(0).Amount = 9999;
    furious.Type50.Attacks.at(1).Amount = 9999;
    furious.Type50.MaxRange = 25;
    furious.Type50.BlastWidth = 25;
    furious.Type50.BlastAttackLevel = 0;
    std::cout << "Patched Furious the Monkey Boy unit for civ " << civ.Name << "\n";
}

bool isTrebuchet(const genie::Unit *unit) { return unit->Class == 51 || unit->Class == 54; }

void preventHPIncrease(genie::Unit *unit) {
    unit->Type50.Armours = {};
    if (!isTrebuchet(unit)) {  // Trebuchets don't die right if they have another class
        unit->Class = CLASS_HERO;
    }
}

void buildCountdownToSaboteur(std::vector<int16_t> &COUNTDOWN_UNITS, genie::Civ &civ) {
    genie::Unit *unit = &civ.Units.at(ID_KING);
    for (int16_t nextUnitId : COUNTDOWN_UNITS) {
        unit->DeadUnitID = nextUnitId;
        std::cout << "Set dying unit for unit " << unit->ID << " to " << nextUnitId << " for civ " << civ.Name
                  << std::endl;

        unit = &civ.Units.at(nextUnitId);
        unit->HitPoints = -1;
        preventHPIncrease(unit);
        std::cout << "Patched hero unit " << unit->ID << " for civ " << civ.Name << std::endl;
    }

    unit->DeadUnitID = ID_SABOTEUR;
    std::cout << "Set dying unit for unit " << unit->ID << " to " << ID_SABOTEUR << " for civ " << civ.Name
              << std::endl;
}

void configureExplodingKings(genie::DatFile *df) {
    std::vector<int16_t> COUNTDOWN_UNITS = {
        ID_TRISTAN,                    // Self Plug
        ID_GUY_JOSSELYNE,              // paladin
        ID_GUY_LANCELOT,               // paladin
        ID_GUY_MORDRED,                // paladin
        ID_GUY_RICHARD_THE_LIONHEART,  // paladin
        ID_HENRY_V,                    // paladin
        ID_DINH_LE,                    // unique paladin
        ID_BABUR,                      // camel
        ID_TABINSHWEHTI,               // elephant
        ID_SURYAVARMAN_I,              // elephant
        ID_BAYINNAUNG,                 // elephant
        ID_KHOSRAU,                    // elephant
        ID_ABRAHA_ELEPHANT,            // elephant
        ID_BAD_NEIGHBOR_PACKED,        // treb
        ID_GODS_OWN_SLING_PACKED,      // treb
        ID_BAD_NEIGHBOR,               // treb
        ID_GODS_OWN_SLING,             // treb
        ID_EMPEROR_IN_A_BARREL,        // unique
    };

    for (genie::Civ &civ : df->Civs) {
        buildCountdownToSaboteur(COUNTDOWN_UNITS, civ);
        patchSaboteurForFirstExplosion(civ);
        patchMonkeyForSecondExplosion(civ);
    }
}
