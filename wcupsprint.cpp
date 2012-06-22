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

#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>

#include "config.h"
#include <futil/WmlDbg.h>

#include "CupsJob.h"
#include "CupsCtrl.h"

using namespace std;
using namespace wml;

ofstream DBGSTREAM;

int main (int argc, char** argv)
{
        if (argc<3) {
                cerr << argv[0] << ": Print a file to a queue\n";
                cerr << "Usage " << argv[0] << " queue filepath\n";
                return -1;
        }

        DBGOPEN ("wcupsprint.log");

        string addr ("localhost");
        CupsCtrl c(addr);

        // Test setAccepting
        string thequeue(argv[1]);
        string fpath (argv[2]);

        int id = c.printFile (fpath, "testprint", thequeue);
        cout << "File printed on job with id " << id << endl;

        DBGCLOSE();
        return 0;
}
