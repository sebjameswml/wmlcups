#include <utility>
#include <vector>
#include <list>
#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
#include <unistd.h>
}

#include "CupsCtrl.h"
#include "config.h"
#include "IppAttr.h"
#include "PpdOption.h"
#include "QueueCupsStatus.h"
#include <futil/WmlDbg.h>

std::ofstream DBGSTREAM;

using namespace std;
using namespace wml;

int main(int argc, char **argv)
{
	DBGOPEN ("/tmp/memberuritest.log");

	try {

		CupsCtrl c ("127.0.0.1");

		stringstream test;

		vector<string> test2;

		string queuename = "newclass3";
		vector<string>::iterator iter;

		test << c.getQueueType (queuename);

		cout << "Attempting to output member uris for class " << queuename << "..." << endl;

		if (test.str() == "1") {
			cout << "printer" << endl;
		} else if (test.str() == "2") {
			cout << "class" << endl;
		} else {
			cout << "unknown" << endl;
		}
		test2 = c.getMemberUris (queuename);

		for (iter = test2.begin(); iter != test2.end(); iter++) {
			cout << *iter << endl;
		}



		/*test2.push_back ("ipp://" + c.getCupsdAddress() + "/printers/kyo1300");
		test2.push_back ("ipp://" + c.getCupsdAddress() + "/printers/hp1312-1");
		test2.push_back ("ipp://" + c.getCupsdAddress() + "/printers/blackhole");

		IppAttr attr;



		c.setInfo ("testclass",
			   "testinfo");

		c.setLocation ("testclass",
			       "theQuadrant");

		//c.setClassAttribute

		c.setClassMembers ("testclass",

		test2);*/



	} catch (const exception& e) {

		cout << "Exception: '" << e.what() << "'\n";
		DBGSTREAM << "Exception: '" << e.what() << "'\n";
		DBGSTREAM.flush();
		cout << "Content-Type: text/plain\n\n";
		cout << 0 << endl;


	}

	DBGCLOSE();

	_exit(0);

}
