/* -*-c++-*- */
/*!
 *  This file is part of wmlcups - a library containing extentions of
 *  and wrappers around the CUPS API.
 *
 *  wmlcups is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  wmlcups is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  wmlcups is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wmlcups (see the file COPYING).  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _PPD_H_
#define _PPD_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <string>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"

namespace wml {

        /*!
         * \class Ppd A PPD class to hold information about a PPD file.
         *
         * Example text fields which CUPS returns about a PPD file include:
         *
         * 'ppd-name'           lsb/usr/HP/hp-psc_2400_series-hpijs.ppd.gz
         * 'ppd-make'           HP
         * 'ppd-make-and-model' HP PSC 2400 Series hpijs, 3.9.10.72
         * 'ppd-device-id'      MFG:HP;MDL:psc 2400 series;DES:psc 2400 series;
         * 'ppd-product'        HP PSC 2405 Photosmart All-in-one Printer
         * 'ppd-psversion'      (3010.000) 705
         *
         * Non-TEXT/NAME fields returned are:
         * ppd-type,
         * ppd-model-number,
         * ppd-natural-language
         */
        class Ppd
        {
        public:
                Ppd (void);
                ~Ppd (void);

                /*!
                 * Setters
                 */
                //@{
                void setName (const std::string& s);
                void setMakeAndModel (const std::string& s);
                void setMake (const std::string& s);
                void setDeviceId (const std::string& s);
                void setProduct (const std::string& s);
                void setPsversion (const std::string& s);
                void setType (const int i);
                void setModelNumber (const int i);
                void setNaturalLanguage (const std::string& s);
                //@}

                /*!
                 * Getters
                 */
                //@{
                std::string getName (void) const;
                std::string getMakeAndModel (void) const;
                std::string getMake (void) const;
                std::string getDeviceId (void) const;
                std::string getProduct (void) const;
                std::string getPsversion (void) const;
                int getType (void) const;
                int getModelNumber (void) const;
                std::string getNaturalLanguage (void) const;
                //@}

        private:
                /*!
                 * The ppd-name, as returned by CUPS.
                 */
                std::string name;

                /*!
                 * The ppd-make, as returned by CUPS.
                 */
                std::string make;

                /*!
                 * The ppd-make-and-model, as returned by CUPS.
                 */
                std::string makeAndModel;

                /*!
                 * The ppd-device-id, as returned by CUPS.
                 */
                std::string deviceId;

                /*!
                 * The ppd-product, as returned by CUPS.
                 */
                std::string product;

                /*!
                 * The ppd-psversion, as returned by CUPS.
                 */
                std::string psversion;

                /*!
                 * The ppd-type, as returned by CUPS.
                 */
                int type;

                /*!
                 * The ppd-model-number, as returned by CUPS.
                 */
                int modelNumber;

                /*!
                 * The ppd-natural-language, as returned by CUPS.
                 */
                std::string naturalLanguage;
        };

} // namespace wml

#endif // _PPD_H_
