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

        DBGOPEN ("teststatus.dbg");

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

        cout << "Cups PPD file is: " << c.getPPDNickname (thequeue) << endl;

        DBGCLOSE();

        return 0;
}
