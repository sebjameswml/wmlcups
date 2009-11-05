#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsdCtrl.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
/*
	if (argc<2) {
		cerr << "Usage " << argv[0] << " queue\n";
		return -1;
	}
*/
	DBGOPEN ("cupsdCtrl.dbg");

	CupsdCtrl c("/etc/cups/cupsd.conf");

	//c.setDirective();

	c.read();

	DBGCLOSE();

	return 0;
}
