#include "exploding_kings.h"
#include "genie/dat/DatFile.h"
#include "ids.h"


void patchSaboteurForFirstExplosion(genie::Civ &civ) {
    genie::Unit &saboteur = civ.Units.at(ID_SABOTEUR);
    saboteur.HitPoints = -1;
    saboteur.DeadUnitID = ID_FURIOUS_THE_MONKEY_BOY;
    saboteur.Type50.Attacks.at(0).Amount = 5000;
    saboteur.Type50.Attacks.at(1).Amount = 5000;
    saboteur.Type50.Attacks.at(2).Amount = 5000;
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
    furious.Type50.Attacks.at(0).Amount = 5000;
    furious.Type50.Attacks.at(1).Amount = 5000;
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

void lowKeyMakeKingsAbleToGarrisonSiege(genie::Civ &civ) {
    genie::Unit &king = civ.Units.at(ID_KING);
    auto garrisonSiegeTask = new genie::Task();
    garrisonSiegeTask->ActionType = ACTION_TYPE_GARRISON; //3
    garrisonSiegeTask->ClassID = CLASS_SIEGE_WEAPON; //13
    garrisonSiegeTask->WorkRange = 1;
    garrisonSiegeTask->TargetDiplomacy = 1;
    king.Bird.TaskList.push_back(*garrisonSiegeTask);
    std::cout << "Patched King garrison for civ " << civ.Name << "\n";
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

void patchMonumentToKeepItFromExploding(genie::Civ &civ) {
    genie::Unit &monument = civ.Units.at(ID_MONUMENT);
    monument.HitPoints = 30000; //Max HP allowed in engine with some buffer so not to risk overflow
    monument.Type50.BaseArmor = 10000;  //buff armors to keep it from booming
}

void configureExplodingKings(genie::DatFile *df) {
    std::vector<int16_t> COUNTDOWN_UNITS = {
        ID_TRISTAN,                    // Self Plug
        ID_BAYINNAUNG,                 // elephant
        ID_DAGNAJAN,                   // elephaunt
        ID_SURYAVARMAN_I,              // elephant
        ID_TABINSHWEHTI,               // elephant
        ID_ABRAHA_ELEPHANT,            // elephant
        ID_KHOSRAU,                    // elephant
        ID_TSAR_KONSTANTIN,            // Chariot
        ID_LEIF_ERIKSON,               // longboat
        ID_VASCO_DA_GAMA,              // caravel
        ID_THE_MIDDLEBROOK,            // longboat
        ID_ADMIRAL_YI_SHUN_SHIN,       // turtle ship
        ID_BAD_NEIGHBOR,               // treb 
        ID_GODS_OWN_SLING,             // treb
        ID_BAD_NEIGHBOR_PACKED,        // treb
        ID_GODS_OWN_SLING_PACKED,      // treb
        ID_EMPEROR_IN_A_BARREL,        // unique
    };

    for (genie::Civ &civ : df->Civs) {
        lowKeyMakeKingsAbleToGarrisonSiege(civ);
        buildCountdownToSaboteur(COUNTDOWN_UNITS, civ);
        patchSaboteurForFirstExplosion(civ);
        patchMonkeyForSecondExplosion(civ);
        patchMonumentToKeepItFromExploding(civ);
    }
}
