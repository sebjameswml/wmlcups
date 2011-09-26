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
//        Queue q ("hp3005");

        try {
//        q.reprintJob (jobnum, user);
                cout << c.getQueueType (thejob) << endl;
        } catch (runtime_error& e) {
                DBG ("Error occurred: " << e.what());
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
