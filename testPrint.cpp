#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsJob.h"
#include "CupsCtrl.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
	if (argc<3) {
		cerr << "Usage " << argv[0] << " queue filepath\n";
		return -1;
	}

	DBGOPEN ("./testPrint.dbg");

	string addr ("localhost");
	CupsCtrl c(addr);

	// Test setAccepting
	string thequeue(argv[1]);
	string fpath (argv[2]);

	int id = c.createJob (thequeue, "testtitle", "", "");
	cout << "createJob created job with id " << id << endl;
	c.sendDocument (id, fpath, "CupsCtrl");
	cout << "Sent file to the job with id " << id << endl;

	//cout << "Sleep 15 seconds...\n";
	//usleep (15000000);

	// Or, the above two in a single call:
	//id = c.printFile (fpath, "testprint", thequeue);
	//cout << "File printed on job with id " << id << endl;

	DBGCLOSE();
	return 0;
}
