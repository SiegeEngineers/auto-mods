#include "genie/dat/DatFile.h"
#include "patches.h"
#include <string>

using namespace std;


int main(int argc, char **argv) {
    const string FLYING_DUTCHMAN = "flying-dutchman";

    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <mod-identifier> source.dat target.dat" << endl;
        cout << "Where <mod-identifier> is one of the following:" << endl;
        cout << "    " << FLYING_DUTCHMAN << endl;
        return 1;
    }

    genie::DatFile *df = new genie::DatFile();
    df->setGameVersion(genie::GV_LatestDE2);

    cout << "Loading " << argv[2] << "..." << endl;
    df->load(argv[2]);

    char *modIdentifier = argv[1];
    if (FLYING_DUTCHMAN == modIdentifier) {
        makeTransportShipsFly(df);
    } else {
        cout << "Unknown mod identifier: '" << modIdentifier << "'" << endl;
    }


    cout << "Saving as " << argv[3] << "..." << endl;
    df->saveAs(argv[3]);

    cout << "Done." << endl;
    return 0;
}


