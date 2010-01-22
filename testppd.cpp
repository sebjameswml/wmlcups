#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsCtrl.h"
#include "IppAttr.h"
#include "PpdOption.h"
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

	DBGOPEN ("testppd.log");

	string addr ("localhost");
	CupsCtrl c(addr);

	// Test setAccepting
	string thequeue(argv[1]);

	vector<string> theL = c.getPPDListOfMakes();
	vector<string>::iterator i = theL.begin();
	cout << "Makes:" << endl;
	while (i != theL.end()) {
		cout << *i << endl;
		i++;
	}

	string make = "Oki";
	vector<Ppd> fullList = c.getPPDListOfModels (make);
	vector<Ppd>::iterator p = fullList.begin();
	cout << "Models for " << make << ":" << endl;
	while (p != fullList.end()) {
		cout << p->getMakeAndModel() << "\n";
		cout << "  ppd-name:             " << p->getName() << "\n";
		cout << "  ppd-make:             " << p->getMake() << "\n";
		cout << "  ppd-device-id:        " << p->getDeviceId() << "\n";
		cout << "  ppd-product:          " << p->getProduct() << "\n";
		cout << "  ppd-psversion:        " << p->getPsversion() << "\n";
		cout << "  ppd-type:             " << p->getType() << "\n";
		cout << "  ppd-model-number:     " << p->getModelNumber() << "\n";
		cout << "  ppd-natural-language: " << p->getNaturalLanguage() << "\n";
		p++;
	}

	//c.setPPDFromFile (thequeue, "/usr/src/wmlcups/test.ppd");
	// Or:
	c.setPPD (thequeue, fullList.begin()->getName());

	cout << "Cups PPD nickname is: " << c.getPPDNickname (thequeue)
	     << " or (makeandmodel): " << c.getMakeModel (thequeue) << endl;

	vector<PpdOption> vopts = c.getPPDOptions (thequeue);
	vector<PpdOption>::iterator j = vopts.begin();
	while (j != vopts.end()) {
		cout << "Option: '" << j->getKeyword() << "'   Group: '" << j->getGroupName() << "'\n";
		j++;
	}

	Ppd thePPD = c.getPPD (thequeue);
	cout << "We get BACK the following: ";
	cout << thePPD.getMakeAndModel() << "\n";
	cout << "  ppd-name:             " << thePPD.getName() << "\n";
	cout << "  ppd-make:             " << thePPD.getMake() << "\n";
	cout << "  ppd-device-id:        " << thePPD.getDeviceId() << "\n";
	cout << "  ppd-product:          " << thePPD.getProduct() << "\n";
	cout << "  ppd-psversion:        " << thePPD.getPsversion() << "\n";
	cout << "  ppd-type:             " << thePPD.getType() << "\n";
	cout << "  ppd-model-number:     " << thePPD.getModelNumber() << "\n";
	cout << "  ppd-natural-language: " << thePPD.getNaturalLanguage() << "\n";

	// Now set an option.
#ifdef TESTING_SETTING_OPTIONS
	c.setPPDOption (thequeue,
			"HPJobRetentionOption",
			"HPJobRetentionProof");
#endif

	DBGCLOSE();
	return 0;
}
