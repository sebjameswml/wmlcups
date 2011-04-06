/*!
 *  This program is part of WML CUPS - a library containing
 *  extentions of and wrappers around the CUPS API.
 *
 *  This is a simple program to wrap around CUPS's lpd.c (a version of
 *  which is compiled here) providing a utility to send files using
 *  the LPD protocol.
 *
 *  WML CUPS is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  WML CUPS is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  WML CUPS is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WML CUPS (see the file COPYING).  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
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
