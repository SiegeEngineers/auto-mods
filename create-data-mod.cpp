#include "genie/dat/DatFile.h"
#include "patches.h"
#include <string>

using namespace std;


int main(int argc, char **argv) {
    const string FLYING_DUTCHMAN = "flying-dutchman";
    const string FLYING_DUTCHMAN_WITH_RAM_FIX = "flying-dutchman-with-ram-fix";
    const string RAM_FIX = "ram-fix";

    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <mod-identifier> source.dat target.dat" << endl;
        cout << "Where <mod-identifier> is one of the following:" << endl;
        cout << "    " << FLYING_DUTCHMAN << endl;
        cout << "    " << FLYING_DUTCHMAN_WITH_RAM_FIX << endl;
        cout << "    " << RAM_FIX << endl;
        return 1;
    }

    genie::DatFile *df = new genie::DatFile();
    df->setGameVersion(genie::GV_LatestDE2);

    cout << "Loading " << argv[2] << "..." << endl;
    df->load(argv[2]);

    char *modIdentifier = argv[1];
    if (FLYING_DUTCHMAN == modIdentifier) {
        makeTransportShipsFly(df);
    } else if (FLYING_DUTCHMAN_WITH_RAM_FIX == modIdentifier) {
        makeTransportShipsFly(df);
        preventRamsAndSiegeTowersFromBoardingTransportShips(df);
    } else if (RAM_FIX == modIdentifier) {
        preventRamsAndSiegeTowersFromBoardingTransportShips(df);
    } else {
        cout << "Unknown mod identifier: '" << modIdentifier << "'" << endl;
    }


    cout << "Saving as " << argv[3] << "..." << endl;
    df->saveAs(argv[3]);

    cout << "Done." << endl;
    return 0;
}


