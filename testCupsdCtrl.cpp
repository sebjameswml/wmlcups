/*!
 *  This test program is part of WML CUPS - a library containing
 *  extentions of and wrappers around the CUPS API.
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
