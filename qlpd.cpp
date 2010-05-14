/*
 * A simple program to wrap around lpd.c
 */

#include <iostream>
#include <fstream>
#include <string>
#include "config.h"
#include <futil/WmlDbg.h>
#include <futil/Process.h>

using namespace std;
using namespace wml;

ofstream DBGSTREAM;
string finMsg("");

namespace wml {
	class QlpdCallbacks : public wml::ProcessCallbacks
	{
	public:
		QlpdCallbacks () {}
		~QlpdCallbacks () {}
		void startedSignal (std::string) {}
		void errorSignal (int err) { cerr << "Process error: " << err << endl; }
		void processFinishedSignal (std::string msg) { finMsg = msg; }
		void readyReadStandardOutputSignal (void) {}
		void readyReadStandardErrorSignal (void) {}
	private:
	};
}

int main (int argc, char** argv)
{
	if ((argc < 3) || (argc > 4)) {  /* Test for correct number of arguments */
		cerr << "Usage: " << argv[0] << " <Server Address/IP> <File> [<Queue>]\n";
		exit (1);
	}

	string servIP(argv[1]);
	string printFile(argv[2]);
	string theQueue("passthru");
	if (argc == 4) {
		theQueue = argv[3];
	}

	Process lpd;
	wml::QlpdCallbacks cb;
	lpd.setCallbacks (&cb);
	string processPath = "/usr/bin/lpd";
	list<string> args;
	args.push_back (processPath);// lpd
	args.push_back ("1");        // Job ID
	args.push_back ("qlpd");     // User
	args.push_back ("qlpd_job"); // Title
	args.push_back ("1");        // Copies
	args.push_back ("0");        // Options
	args.push_back (servIP);     // Hostname
	args.push_back (theQueue);
	args.push_back (printFile);
	lpd.start (processPath, args);
	while (finMsg == "")  {
		lpd.probeProcess();
	}
	cout << lpd.readAllStandardOutput() << endl;
	cerr << lpd.readAllStandardError() << endl;

	return 0;
}
