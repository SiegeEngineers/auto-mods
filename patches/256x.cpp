#include "256x.h"
#include <iostream>
#include "genie/dat/DatFile.h"
#include "ids.h"
#include <bits/stdc++.h>


void sellAllWood(genie::DatFile *df) {
    auto effectCommand1 = new genie::EffectCommand();
    effectCommand1->Type = 1;  // Resource Modifier
    effectCommand1->A = 3;
    effectCommand1->B = 1;
    effectCommand1->C = 1;
    effectCommand1->D = 0.17;

    auto effectCommand2 = new genie::EffectCommand();
    effectCommand2->Type = 1;  // Resource Modifier
    effectCommand2->A = 1;
    effectCommand2->B = 0;
    effectCommand2->C = -1;
    effectCommand2->D = 0;

    auto effect = new genie::Effect();
    effect->Name = "Sell All Wood (Guilds)";
    effect->EffectCommands.push_back(*effectCommand1);
    effect->EffectCommands.push_back(*effectCommand2);
    int sellAllGuildsEffectId = df->Effects.size();
    df->Effects.push_back(*effect);

    effectCommand1->D = 0.14;

    effect = new genie::Effect();
    effect->Name = "Sell All Wood";
    effect->EffectCommands.push_back(*effectCommand1);
    effect->EffectCommands.push_back(*effectCommand2);
    int sellAllEffectId = df->Effects.size();
    df->Effects.push_back(*effect);

    effectCommand1->D = 0.19;

    effect = new genie::Effect();
    effect->Name = "Sell All Wood (Saracens)";
    effect->EffectCommands.push_back(*effectCommand1);
    effect->EffectCommands.push_back(*effectCommand2);
    int sellAllSaracensEffectId = df->Effects.size();
    df->Effects.push_back(*effect);


    auto tech = new genie::Tech();
    tech->Name = "Sell All Wood";
    tech->RequiredTechs.push_back(GUILDS);
    tech->RequiredTechCount = 1;
    tech->ResearchTime = 1;
    tech->IconID = 3;
    tech->ButtonID = 10;
    tech->EffectID = sellAllGuildsEffectId;
    tech->ResearchLocation = MARKET;
    tech->Repeatable = 1;
    tech->LanguageDLLName = 19109;
    tech->LanguageDLLDescription = 8441;
    std::cout << "Adding Sell All Wood (Guilds) Repeatable Tech with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(*tech);

    tech = new genie::Tech();
    tech->Name = "Sell All Wood";
    tech->RequiredTechCount = 0;
    tech->ResearchTime = 1;
    tech->IconID = 3;
    tech->ButtonID = 10;
    tech->EffectID = sellAllEffectId;
    tech->ResearchLocation = MARKET;
    tech->Repeatable = 1;
    tech->LanguageDLLName = 19109;
    tech->LanguageDLLDescription = 8440;
    std::cout << "Adding Sell All Wood Repeatable Tech with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(*tech);

    tech = new genie::Tech();
    tech->Name = "Sell All Wood";
    tech->RequiredTechCount = 0;
    tech->ResearchTime = 1;
    tech->IconID = 3;
    tech->ButtonID = 10;
    tech->EffectID = sellAllSaracensEffectId;
    tech->ResearchLocation = MARKET;
    tech->Civ = SARACENS;
    tech->Repeatable = 1;
    tech->LanguageDLLName = 19109;
    tech->LanguageDLLDescription = 8442;
    std::cout << "Adding Sell All Wood (Saracens) Repeatable Tech with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(*tech);
    
        
    auto effectCommand3 = new genie::EffectCommand();
    effectCommand3->Type = 102;  // Resource Modifier
    effectCommand3->D = sellAllEffectId;

    df->Effects.at(261).EffectCommands.push_back(*effectCommand3);

}

void wheelbarrowHelpsFarms(genie::DatFile *df) {
    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 5;  // Resource Modifier
    effectCommand->A = 50;
    effectCommand->B = -1;
    effectCommand->C = 13;
    effectCommand->D = 1.15;
    
    df->Effects.at(200).EffectCommands.push_back(*effectCommand);
    df->Effects.at(238).EffectCommands.push_back(*effectCommand);
    effectCommand->A = 214;
    df->Effects.at(200).EffectCommands.push_back(*effectCommand);
    df->Effects.at(238).EffectCommands.push_back(*effectCommand);
    effectCommand->A = 259;
    df->Effects.at(200).EffectCommands.push_back(*effectCommand);
    df->Effects.at(238).EffectCommands.push_back(*effectCommand);
    std::cout << "Adding 14 copies of Wheelbarrow, and setting it to help farms" << std::endl;

    auto wheelbarrowTech = df->Techs.at(213);
    for (int i = 0; i < 14; i++)
    {
        df->Techs.push_back(wheelbarrowTech);
    }
}

void loomNoLongerGivesArmor(genie::DatFile *df) {
    std::cout << "Changing Loom effect to not give armor" << std::endl;
    df->Effects.at(22).EffectCommands.pop_back();
    df->Effects.at(22).EffectCommands.pop_back();
}

void repeatableTownWatch(genie::DatFile *df) {
    df->Techs.at(8).Repeatable = 1;
}

void sappersOnceForAll(genie::DatFile *df) {
    auto sappersTech = df->Techs.at(321);
    std::cout << "Adding Sappers" << std::endl;
    df->Techs.push_back(sappersTech);
}

void treadmillforAll(genie::DatFile *df) {
    auto treadmillTech = df->Techs.at(54);
    std::cout << "Adding Repeatable Tech Treadmill Crane with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(treadmillTech);
}

void fixCaravan(genie::DatFile *df) {
    std::cout << "Adding 9 copies of Caravan, and setting it to not help speed" << std::endl;

    auto caravanTech = df->Techs.at(48);
    for (int i = 0; i < 9; i++)
    {
        df->Techs.push_back(caravanTech);
    }
    df->Effects.at(482).EffectCommands.erase(df->Effects.at(482).EffectCommands.begin()+6);
    df->Effects.at(482).EffectCommands.erase(df->Effects.at(482).EffectCommands.begin()+4);
    df->Effects.at(482).EffectCommands.erase(df->Effects.at(482).EffectCommands.begin()+2);
    df->Effects.at(482).EffectCommands.erase(df->Effects.at(482).EffectCommands.begin()+0);

}

void fixMasonryArchitecture(genie::DatFile *df) {
    std::cout << "Fixing Masonry and Architecture" << std::endl;

    auto masonryTech = df->Techs.at(50);
    auto architectureTech = df->Techs.at(51);
    for (int i = 0; i < 16; i++)
    {
        df->Techs.push_back(masonryTech);
    }
    std::cout << "Adding Repeatable Tech Architecture with id " << df->Techs.size() << std::endl;
    int architectureId = df->Techs.size();
    df->Techs.push_back(architectureTech);
    df->Techs.at(50).RequiredTechs.at(0) = 50;
    df->Techs.at(51).RequiredTechs.at(0) = 51;

    auto tech = new genie::Tech();
    tech->Name = "Architecture requirement 1";
    tech->RequiredTechs.push_back(architectureId - 16);
    tech->RequiredTechs.push_back(architectureId - 15);
    tech->RequiredTechs.push_back(architectureId - 14);
    tech->RequiredTechs.push_back(architectureId - 13);
    tech->RequiredTechCount = 4;
    tech->ResearchTime = 0;
    df->Techs.push_back(*tech);

    tech = new genie::Tech();
    tech->Name = "Architecture requirement 2";
    tech->RequiredTechs.push_back(architectureId - 12);
    tech->RequiredTechs.push_back(architectureId - 11);
    tech->RequiredTechs.push_back(architectureId - 10);
    tech->RequiredTechs.push_back(architectureId - 9);
    tech->RequiredTechCount = 4;
    tech->ResearchTime = 0;
    df->Techs.push_back(*tech);

    tech = new genie::Tech();
    tech->Name = "Architecture requirement 3";
    tech->RequiredTechs.push_back(architectureId - 8);
    tech->RequiredTechs.push_back(architectureId - 7);
    tech->RequiredTechs.push_back(architectureId - 6);
    tech->RequiredTechs.push_back(architectureId - 5);
    tech->RequiredTechCount = 4;
    tech->ResearchTime = 0;
    df->Techs.push_back(*tech);

    tech = new genie::Tech();
    tech->Name = "Architecture requirement 4";
    tech->RequiredTechs.push_back(architectureId - 4);
    tech->RequiredTechs.push_back(architectureId - 3);
    tech->RequiredTechs.push_back(architectureId - 2);
    tech->RequiredTechs.push_back(architectureId - 1);
    tech->RequiredTechCount = 4;
    tech->ResearchTime = 0;
    df->Techs.push_back(*tech);



    df->Techs.at(architectureId).RequiredTechCount = 5;
    df->Techs.at(architectureId).RequiredTechs.at(1) = architectureId + 1;
    df->Techs.at(architectureId).RequiredTechs.at(2) = architectureId + 2;
    df->Techs.at(architectureId).RequiredTechs.at(3) = architectureId + 3;
    df->Techs.at(architectureId).RequiredTechs.at(4) = architectureId + 4;


    df->Effects.at(51).EffectCommands.erase(df->Effects.at(51).EffectCommands.begin()+4);
    df->Effects.at(51).EffectCommands.erase(df->Effects.at(51).EffectCommands.begin()+0);

    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 0;  // Attribute Modifier
    effectCommand->A = -1;
    effectCommand->B = 3;
    effectCommand->C = 0;
    effectCommand->D = 32767;

    df->Effects.at(51).EffectCommands.push_back(*effectCommand);
    effectCommand->B = 52;
    df->Effects.at(51).EffectCommands.push_back(*effectCommand);

    df->Techs.at(379).RequiredTechs.at(0) = 379;
}

void fixChemistry(genie::DatFile *df) {
    auto chemistryTech = df->Techs.at(47);
    std::cout << "Adding Repeatable Tech Chemistry with id " << df->Techs.size() << std::endl;
    int chemistryId = df->Techs.size();
    df->Techs.push_back(chemistryTech);

    auto chemistryEffect = df->Effects.at(47);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+69);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+61);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+60);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+57);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+43);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+36);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+35);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+34);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+32);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+27);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+26);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+25);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+24);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+23);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+22);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+21);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+20);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+19);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+18);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+17);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+16);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+14);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+13);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+12);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+11);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+10);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+9);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+7);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+6);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+2);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+1);
    chemistryEffect.EffectCommands.erase(chemistryEffect.EffectCommands.begin()+0);

    int chemistryEffectId = df->Effects.size();
    df->Effects.push_back(chemistryEffect);
    df->Techs.at(chemistryId).EffectID = chemistryEffectId;


}

void fixFreeTechs(genie::DatFile *df) {
    //Bohemians mining
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+36);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+35);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+34);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+33);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+32);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+31);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+30);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+29);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+28);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+27);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+26);
    df->Effects.at(803).EffectCommands.erase(df->Effects.at(803).EffectCommands.begin()+25);

    auto tech = df->Techs.at(55);
    tech.ResearchTime = 0;
    tech.Civ = 39;
    df->Techs.push_back(tech);

    tech = df->Techs.at(182);
    tech.ResearchTime = 0;
    tech.Civ = 39;
    df->Techs.push_back(tech);

    tech = df->Techs.at(278);
    tech.ResearchTime = 0;
    tech.Civ = 39;
    df->Techs.push_back(tech);

    tech = df->Techs.at(279);
    tech.ResearchTime = 0;
    tech.Civ = 39;
    df->Techs.push_back(tech);

    //Burmese wood
    df->Techs.at(645).EffectID = -1;

    tech = df->Techs.at(202);
    tech.ResearchTime = 0;
    tech.Civ = 30;
    df->Techs.push_back(tech);

    tech = df->Techs.at(203);
    tech.ResearchTime = 0;
    tech.Civ = 30;
    df->Techs.push_back(tech);

    tech = df->Techs.at(221);
    tech.ResearchTime = 0;
    tech.Civ = 30;
    df->Techs.push_back(tech);

    //Koreans archer armor
    df->Techs.at(451).EffectID = -1;
    tech = df->Techs.at(211);
    tech.ResearchTime = 0;
    tech.Civ = 18;
    df->Techs.push_back(tech);

    tech = df->Techs.at(212);
    tech.ResearchTime = 0;
    tech.Civ = 18;
    df->Techs.push_back(tech);

    tech = df->Techs.at(219);
    tech.ResearchTime = 0;
    tech.Civ = 18;
    df->Techs.push_back(tech);

    //Magyars infantry attack
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+33);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+32);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+31);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+30);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+29);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+28);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+27);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+26);
    df->Effects.at(5).EffectCommands.erase(df->Effects.at(5).EffectCommands.begin()+25);

    tech = df->Techs.at(67);
    tech.ResearchTime = 0;
    tech.Civ = 22;
    df->Techs.push_back(tech);

    tech = df->Techs.at(68);
    tech.ResearchTime = 0;
    tech.Civ = 22;
    df->Techs.push_back(tech);

    tech = df->Techs.at(75);
    tech.ResearchTime = 0;
    tech.Civ = 22;
    df->Techs.push_back(tech);


    //Tatars thumb ring
    df->Techs.at(698).EffectID = -1;

    tech = df->Techs.at(436);
    tech.ResearchTime = 0;
    tech.Civ = 33;
    df->Techs.push_back(tech);

    tech = df->Techs.at(437);
    tech.ResearchTime = 0;
    tech.Civ = 33;
    df->Techs.push_back(tech);
}

void incaCivBonus(genie::DatFile *df) {

    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 102;  // Disable Tech
    effectCommand->D = 67;
    df->Effects.at(3).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 68;
    df->Effects.at(3).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 75;
    df->Effects.at(3).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 74;
    df->Effects.at(3).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 76;
    df->Effects.at(3).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 77;
    df->Effects.at(3).EffectCommands.push_back(*effectCommand);


    effectCommand->Type = 4;
    effectCommand->A = -1;
    effectCommand->B = 4;
    effectCommand->C = 9;
    effectCommand->D = 4*256 + 1;

    auto effectToModify = df->Effects.at(67);
    effectToModify.EffectCommands.push_back(*effectCommand);
    auto effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    auto tech = df->Techs.at(67);
    tech.Civ = 21;
    tech.EffectID = effectId;

    auto previousId = df->Techs.size();
    std::cout << "Adding Repeatable Tech Inca Forging with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);


    effectToModify = df->Effects.at(68);
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(68);
    tech.Civ = 21;
    tech.EffectID = effectId;
    tech.RequiredTechs.at(1) = previousId;

    previousId = df->Techs.size();
    std::cout << "Adding Repeatable Tech Inca Iron Casting with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);


    effectToModify = df->Effects.at(75);
    effectCommand->D = 4*256 + 2;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(75);
    tech.Civ = 21;
    tech.EffectID = effectId;
    tech.RequiredTechs.at(1) = previousId;

    std::cout << "Adding Repeatable Tech Inca Blast Furnace with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);

    effectCommand->C = 8;


    effectToModify = df->Effects.at(74);
    effectCommand->D = 4*256 + 1;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->D = 3*256 + 1;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(74);
    tech.Civ = 21;
    tech.EffectID = effectId;

    previousId = df->Techs.size();
    std::cout << "Adding Repeatable Tech Inca Scale Armor with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);



    effectToModify = df->Effects.at(76);
    effectCommand->D = 4*256 + 1;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->D = 3*256 + 1;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(76);
    tech.Civ = 21;
    tech.EffectID = effectId;
    tech.RequiredTechs.at(1) = previousId;

    std::cout << "Adding Repeatable Tech Inca Chain Armor with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);



    effectToModify = df->Effects.at(77);
    effectCommand->D = 4*256 + 1;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->D = 3*256 + 2;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(77);
    tech.Civ = 21;
    tech.EffectID = effectId;
    tech.RequiredTechs.at(1) = previousId;

    std::cout << "Adding Repeatable Tech Inca Plate Armor with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);
}

void aztecCivBonus(genie::DatFile *df) {

    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 102;  // Disable Tech
    effectCommand->D = 441;
    df->Effects.at(447).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 231;
    df->Effects.at(447).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 252;
    df->Effects.at(447).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 45;
    df->Effects.at(447).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 233;
    df->Effects.at(447).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 230;
    df->Effects.at(447).EffectCommands.push_back(*effectCommand);


    effectCommand->Type = 4;
    effectCommand->A = -1;
    effectCommand->B = 18;
    effectCommand->C = 0;
    effectCommand->D = 5;

    auto effectToModify = df->Effects.at(41);
    effectCommand->B = 18;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->B = 43;
    effectToModify.EffectCommands.push_back(*effectCommand);
    auto effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    auto tech = df->Techs.at(441);
    tech.Civ = 15;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Aztec Herbal Medicine with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);



    effectToModify = df->Effects.at(221);
    effectCommand->B = 18;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->B = 43;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(231);
    tech.Civ = 15;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Aztec Sanctity with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);


    effectToModify = df->Effects.at(241);
    effectCommand->B = 18;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->B = 43;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(252);
    tech.Civ = 15;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Aztec Fervor with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);



    effectToModify = df->Effects.at(45);
    effectCommand->B = 18;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->B = 43;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(45);
    tech.Civ = 15;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Aztec Faith with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);



    effectToModify = df->Effects.at(219);
    effectCommand->B = 18;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->B = 43;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(233);
    tech.Civ = 15;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Aztec Illumination with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);



    effectToModify = df->Effects.at(220);
    effectCommand->B = 18;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectCommand->B = 43;
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(230);
    tech.Civ = 15;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Aztec Block Printing with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);
}

void bohemianCivBonus(genie::DatFile *df) {

    auto effectCommand = new genie::EffectCommand();
    effectCommand->Type = 102;  // Disable Tech
    effectCommand->D = 231;
    df->Effects.at(803).EffectCommands.push_back(*effectCommand);
    effectCommand->D = 252;
    df->Effects.at(803).EffectCommands.push_back(*effectCommand);


    effectCommand->Type = 4;
    effectCommand->A = -1;
    effectCommand->B = 4;
    effectCommand->C = 0;
    effectCommand->D = 15;

    auto effectToModify = df->Effects.at(221);
    effectToModify.EffectCommands.push_back(*effectCommand);
    auto effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    auto tech = df->Techs.at(231);
    tech.Civ = 39;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Bohemian Sanctity with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);

    effectCommand->Type = 5;
    effectCommand->C = 5;
    effectCommand->D = 1.15;

    effectToModify = df->Effects.at(241);
    effectToModify.EffectCommands.push_back(*effectCommand);
    effectId = df->Effects.size();
    df->Effects.push_back(effectToModify);

    tech = df->Techs.at(252);
    tech.Civ = 39;
    tech.EffectID = effectId;

    std::cout << "Adding Repeatable Tech Bohemian Fervor with id " << df->Techs.size() << std::endl;
    df->Techs.push_back(tech);
}


void configure256x(genie::DatFile *df) {
    sellAllWood(df);
    wheelbarrowHelpsFarms(df);
    loomNoLongerGivesArmor(df);
    repeatableTownWatch(df);
    fixCaravan(df);
    fixMasonryArchitecture(df);
    sappersOnceForAll(df);
    treadmillforAll(df);
    fixChemistry(df);
    fixFreeTechs(df);
    incaCivBonus(df);
    aztecCivBonus(df);
    bohemianCivBonus(df);

}
