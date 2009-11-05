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

	QueueCupsStatus qstat;

	if (c.getFullStatus (thequeue, qstat) == true) {

		cout << "State: " << qstat.state << '\n';
		cout << "State msg: " << qstat.stateMsg << '\n';
		cout << "Enabled?: " << qstat.enabled << '\n';
		cout << "Accepting?: " << qstat.accepting << '\n';
		cout << "Last job state: " << qstat.lastJob.getState() << '\n';

	} else {
		cout << "No such queue " << thequeue << endl;
	}

	//cout << c.getStateReasons (thequeue) << endl;

	return 0;
}
