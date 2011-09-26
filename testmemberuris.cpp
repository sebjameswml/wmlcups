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
