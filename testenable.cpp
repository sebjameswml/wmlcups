#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsCtrl.h"
#include "IppAttr.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
	if (argc<3) {
		cerr << "Usage " << argv[0] << " queue yes|no\n";
		return -1;
	}

	//string addr ("192.168.0.6");
	string addr ("localhost");
	CupsCtrl c(addr);

	// Test setEnable
	string thequeue(argv[1]);
	string enable(argv[2]);

	if (enable == "yes") {
		c.setEnabled (thequeue, true);
	} else {
		c.setEnabled (thequeue, false);
	}
	return 0;
}
