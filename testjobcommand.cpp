#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsJob.h"
#include "CupsCtrl.h"
#include "IppAttr.h"
#include "QueueCupsStatus.h"
//#include <wmlqueue/Queue.h>

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
	if (argc<2) {
		cerr << "Usage " << argv[0] << " queue\n";
		return -1;
	}

	DBGOPEN ("./testjobcommand.dbg");

	string addr ("localhost");
	CupsCtrl c(addr);

	// Test setAccepting
	string thejob(argv[1]);

	int jobnum;
	stringstream temp;
	temp << thejob;
	temp >> jobnum;

	DBG (thejob);
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
string user (argv[2]);
	//CupsJob j;
//	Queue q ("hp3005");

	try {
//	q.reprintJob (jobnum, user);
		cout << c.getQueueType (thejob) << endl;
	} catch (runtime_error& e) {
		DBG ("Error occured: " << e.what());
	}
	//cout << "Last Job ID " << j.getId() << " has state: " << j.getState()
	//  << " time: " << j.getFormattedTime() << endl;

/*
	c.getJobStatus (thequeue, 140210, j);
	cout << "Job ID " << j.getId() << " has state: " << j.getState()
	     << " time: " << j.getFormattedTime() << endl;
*/

	DBGCLOSE();
	return 0;
}
