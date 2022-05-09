#include <string>
#include "genie/dat/DatFile.h"
#include "patches/community_games.h"
#include "patches/duplicate_techs.h"
#include "patches/exploding_villagers.h"
#include "patches/flying_dutchman.h"
#include "patches/kidnap.h"
#include "patches/no_wall.h"
#include "patches/random_costs.h"
#include "patches/teamwork.h"
#include "patches/256x.h"


using namespace std;
const char *const COMMUNITY_GAMES = "community-games";
const char *const EXPLODING_VILLAGERS = "exploding-villagers";
const char *const EXPLODING_VILLAGERS_EXTREME = "exploding-villagers-extreme";
const char *const FLYING_DUTCHMAN = "flying-dutchman";
const char *const KIDNAP = "kidnap";
const char *const NO_WALL = "no-wall";
const char *const RANDOM_COSTS = "random-costs";
const char *const RANDOM_TECH_COSTS = "random-tech-costs";
const char *const RANDOM_UNIT_COSTS = "random-unit-costs";
const char *const TEAMWORK = "teamwork";
const char *const X_256_TECH = "x256";
const char *const X_3_TECH = "x3";
const char *const X_9_TECH = "x9";

vector<string> getModIdentifiers(char *const *argv);

void printModIdentifiers(const vector<string> &modIdentifiers);

void applyModifications(genie::DatFile *df, const string &modIdentifier);


int main(int argc, char **argv) {

    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <mod-identifier> source.dat target.dat" << endl;
        cout << "Where <mod-identifier> is one of the following, or multiple of the following joined by a +:" << endl;
        cout << "    " << COMMUNITY_GAMES << endl;
        cout << "    " << EXPLODING_VILLAGERS << endl;
        cout << "    " << EXPLODING_VILLAGERS_EXTREME << endl;
        cout << "    " << FLYING_DUTCHMAN << endl;
        cout << "    " << KIDNAP << endl;
        cout << "    " << NO_WALL << endl;
        cout << "    " << RANDOM_COSTS << endl;
        cout << "    " << RANDOM_TECH_COSTS << endl;
        cout << "    " << RANDOM_UNIT_COSTS << endl;
        cout << "    " << TEAMWORK << endl;
        cout << "    " << X_3_TECH << endl;
        cout << "    " << X_9_TECH << endl;
        cout << "    " << X_256_TECH << endl;
        return 1;
    }

    auto *df = new genie::DatFile();
    df->setGameVersion(genie::GV_LatestDE2);

    cout << "Loading " << argv[2] << "..." << endl;
    df->load(argv[2]);

    vector<string> modIdentifiers = getModIdentifiers(argv);

    printModIdentifiers(modIdentifiers);

    for (const string &modIdentifier : modIdentifiers) {
        applyModifications(df, modIdentifier);
    }


    cout << "Saving as " << argv[3] << "..." << endl;
    df->saveAs(argv[3]);

    cout << "Done." << endl;
    return 0;
}


void applyModifications(genie::DatFile *df, const string &modIdentifier) {
    if (COMMUNITY_GAMES == modIdentifier) {
        configureCommunityGamesMod(df);
    } else if (EXPLODING_VILLAGERS == modIdentifier) {
        configureExplodingVillagers(df, true);
    } else if (EXPLODING_VILLAGERS_EXTREME == modIdentifier) {
        configureExplodingVillagers(df, false);
    } else if (FLYING_DUTCHMAN == modIdentifier) {
        makeTransportShipsFly(df);
    } else if (KIDNAP == modIdentifier) {
        configureKidnap(df);
    } else if (NO_WALL == modIdentifier) {
        disableWalls(df);
    } else if (RANDOM_COSTS == modIdentifier) {
        jumbleCosts(df);
        enableStablesForMesoCivs(df);
    } else if (RANDOM_TECH_COSTS == modIdentifier) {
        jumbleTechCosts(df);
        enableStablesForMesoCivs(df);
    } else if (RANDOM_UNIT_COSTS == modIdentifier) {
        jumbleUnitCosts(df);
        enableStablesForMesoCivs(df);
    } else if (TEAMWORK == modIdentifier) {
        makeTechEffectsShared(df);
        makeFarmTechsBuffSpeed(df);
    } else if (X_3_TECH == modIdentifier) {
        duplicateTechs(df, 3);
    } else if (X_9_TECH == modIdentifier) {
        duplicateTechs(df, 9);
    } else if (X_256_TECH == modIdentifier) {
        configure256x(df);
    } else {
        cout << "Unknown mod identifier: '" << modIdentifier << "'" << endl;
    }
}


void printModIdentifiers(const vector<string> &modIdentifiers) {
    cout << "Applying the following modifications in order:" << endl;
    for (const string &modIdentifier : modIdentifiers) {
        cout << "\t" << modIdentifier << endl;
    }
}


vector<string> getModIdentifiers(char *const *argv) {
    string s = argv[1];
    string delimiter = "+";
    vector<string> modIdentifiers;

    unsigned int start = 0U;
    size_t end = s.find(delimiter);
    while (end != string::npos) {
        modIdentifiers.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    modIdentifiers.push_back(s.substr(start, end));
    return modIdentifiers;
}
