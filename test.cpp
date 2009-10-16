#include <iostream>
#include <string>

#include "wmlcups.h"
#include "WmlIppAttr.h"

using namespace std;
using namespace wml;

int main (int argc, char** argv)
{
	if (argc<2) {
		cerr << "Usage " << argv[0] << " queue\n";
		return -1;
	}

	//string addr ("192.168.0.6");
	string addr ("localhost");
	WmlCups c(addr);

	// Test getStatus
	cout << "\nTop level status function tests:\n";
	string printer(argv[1]);
	cout << "getEnabled says " << printer << (c.getEnabled (printer) ? " is " : " isn't ") << "enabled\n";
	cout << "getAccepting says " << printer << (c.getAccepting (printer) ? " is " : " isn't ") << "accepting jobs\n";
	cout << "getState says " << printer << " state is " << c.getState(printer) << endl;
	cout << "getInfo says " << printer << " info is " << c.getInfo(printer) << endl;
	cout << "getLocation says " << printer << " location is " << c.getLocation(printer) << endl;
	cout << "getMakeModel says " << printer << " make and model is " << c.getMakeModel(printer) << endl;
	cout << "Device URI is " << c.getDeviceURI(printer) << endl;
	cout << "Cups URI is " << c.getCupsURI(printer) << endl;

	cout << "\ngetPrinterAttribute() tests:\n";
	WmlIppAttr at("printer-state-message");
	c.getPrinterAttribute (argv[1], at);
	cout << "state message: " << at.getString() << endl;

	WmlIppAttr at2("printer-info");
	cout << "info: " << c.getPrinterAttribute (argv[1], at2) << endl;

	WmlIppAttr at3("printer-location");
	cout << "location: " << c.getPrinterAttribute (argv[1], at3) << endl;

	WmlIppAttr at4("printer-state");
	cout << "state: " << c.getPrinterAttribute (argv[1], at4) << endl;

	cout << "\nNow setting info...\n";
	WmlIppAttr a("printer-info");
	a.setValue ("Blah blah");
	try {
		c.setPrinterAttribute (argv[1], a);
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	a.setValue ("nono");
	cout << "info is now: " << c.getPrinterAttribute (argv[1], a) << endl;

	c.setInfo (argv[1], "Bleurgh bleurgh");
	c.setLocation (argv[1], "The Quadrant");
	return 0;
}
