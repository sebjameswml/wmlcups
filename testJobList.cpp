#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsJob.h"
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

	DBGOPEN ("./testJobList.dbg");

	string addr ("localhost");
	CupsCtrl c(addr);

	// Test setAccepting
	string thequeue(argv[1]);

/*
	vector<CupsJob> li;
	c.getJobList (thequeue, li, "all");

	vector<CupsJob>::iterator i = li.end();
	int count = 0;
	while (i != li.begin() && count < 5) {
		i--;
		count++;
		cout << "Job ID " << i->getId() << " has state: " << i->getState()
		     << " time: " << i->getFormattedTime() << endl;

	}
*/
	CupsJob j;
	c.getJobStatus (thequeue, 0, j);
	cout << "Last Job ID " << j.getId() << " has state: " << j.getState()
	     << " time: " << j.getFormattedTime() << endl;

/*
	c.getJobStatus (thequeue, 140210, j);
	cout << "Job ID " << j.getId() << " has state: " << j.getState()
	     << " time: " << j.getFormattedTime() << endl;
*/

	DBGCLOSE();
	return 0;
}
