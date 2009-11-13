#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsCtrl.h"
#include "IppAttr.h"
#include "QueueCupsStatus.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
	if (argc<2) {
		cerr << "Usage " << argv[0] << " queue\n";
		return -1;
	}

	string addr ("localhost");
	CupsCtrl c(addr);

	// Test setAccepting
	string thequeue(argv[1]);

	c.setPPDFromFile (thequeue, "/usr/src/wmlcups/test.ppd");

	// This works (though c.getPPD() still doesn't return anything)
	//c.setPPD (thequeue, "lsb/usr/foomatic-rip/openprinting-gs-builtin/Epson/Epson-ActionPrinter_3250-ap3250.ppd.gz");

	cout << "Cups PPD file is: " << c.getPPD (thequeue) << endl;

	return 0;
}
