#include <iostream>
#include <fstream>
#include <string>
#include <queue>

#include "config.h"
#include <futil/WmlDbg.h>


#include "CupsdCtrl.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{

/*	if (argc<2) {
		cerr << "Usage " << argv[0] << " queue\n";
		return -1;
		}*/

	DBGOPEN ("cupsdCtrl.dbg");

	CupsdCtrl c("/etc/cups/cupsd.conf");

	//string s(argv[1]);
	//c.setDirective();is not re
	//cout << s << endl;
	c.read();

	queue<pair<string, string> > containerId;
	containerId.push(make_pair("root",""));
	containerId.push(make_pair("Location","/admin/conf"));
	//containerId.push(make_pair("Limit", "All"));
	c.setDirective(containerId, "AuthType", "notDefault");
	cout << "Directive is: " << c.getDirective(containerId, "AuthType") << endl;
	c.write();

	DBGCLOSE();

	return 0;
}
