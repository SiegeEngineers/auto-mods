#include "genie/dat/DatFile.h"
#include "patches.h"
#include <string>

using namespace std;
const char *const EXPLODING_VILLAGERS = "exploding-villagers";
const char *const FLYING_DUTCHMAN = "flying-dutchman";

vector<string> getModIdentifiers(char *const *argv);

void printModIdentifiers(const vector<string> &modIdentifiers);

void applyModifications(genie::DatFile *df, const string &modIdentifier);


int main(int argc, char **argv) {

    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <mod-identifier> source.dat target.dat" << endl;
        cout << "Where <mod-identifier> is one of the following, or multiple of the following joined by a +:" << endl;
        cout << "    " << EXPLODING_VILLAGERS << endl;
        cout << "    " << FLYING_DUTCHMAN << endl;
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
    if (EXPLODING_VILLAGERS == modIdentifier) {
        configureExplodingVillagers(df);
    } else if (FLYING_DUTCHMAN == modIdentifier) {
        makeTransportShipsFly(df);
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


