#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "WmlCups.h"
#include "WmlIppAttr.h"

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
	WmlCups c(addr);

	// Test setAccepting
	string thequeue(argv[1]);
	string accept(argv[2]);

	if (accept == "yes") {
		c.setAccepting (thequeue, true);
	} else {
		c.setAccepting (thequeue, false);
	}
	return 0;
}
