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

#ifndef _PPDOPTION_H_
#define _PPDOPTION_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <utility>
#include <vector>

extern "C" {
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/language.h>
}

#include "config.h"

namespace wml {

        /*!
         * A PPD option.
         */
        class PpdOption
        {
        public:
                PpdOption ();
                PpdOption (const ppd_option_t* o);
                ~PpdOption ();

                /*!
                 * Zero out any stored values.
                 */
                void zero (void);

                /*!
                 * Get the group name.
                 */
                std::string getGroupName (void) const;

                /*!
                 * Set the group name.
                 */
                void setGroupName (const std::string& n);

                /*!
                 * Return the keyword of this option.
                 */
                std::string getKeyword (void) const;

                /*!
                 * Set the keyword to s
                 */
                void setKeyword (const std::string& s);

                /*!
                 * Return the text of this option.
                 */
                std::string getText (void) const;

                /*!
                 * Set the text to s
                 */
                void setText (const std::string& s);

                /*!
                 * Return the value of this option.
                 */
                std::string getChoiceValue (void) const;

                /*!
                 * Set the choice which has the value s
                 */
                void setChoiceValue (const std::string& s);

                /*!
                 * Return the text of the chosen option.
                 */
                std::string getChoiceText (void) const;

                /*!
                 * Set the choice which has the text s
                 */
                void setChoiceText (const std::string& s);

                /*!
                 * Return the default choice of this option.
                 */
                std::string getDefaultChoice (void) const;

                /*!
                 * Set the default choice to s
                 */
                void setDefaultChoice (const std::string& s);

                /*!
                 * Get the UI type of this option.
                 */
                ppd_ui_t getUi (void) const;

                /*!
                 * Set the UI type of this option.
                 */
                void setUi (const ppd_ui_t u);

                /*!
                 * Returns the number of elements in this->choices.
                 */
                int getNumChoices (void) const;

                /*!
                 * Get the next choice.
                 *
                 * Populates choice with the element indicated by
                 * this->curChoice, if available, and increments
                 * this->curChoice.
                 *
                 * \return Indicates whether there was a next choice
                 * to get.
                 */
                bool getNextChoice (ppd_choice_t& choice);

                /*!
                 * Reset this->curChoice to point to the start of
                 * this->choices.
                 */
                void resetCurChoice (void);

        private:
                /*!
                 * The name of the PPD option group to which this
                 * PpdOption belongs.
                 */
                std::string groupName;

                /*!
                 * The keyword of the option. Should be no longer than
                 * PPD_MAX_NAME.
                 */
                std::string keyword;

                /*!
                 * The text of the ppdoption. Should be no longer than
                 * PPD_MAX_TEXT.
                 */
                std::string text;

                /*!
                 * The default option choice. No longer than
                 * PPD_MAX_NAME chars.
                 */
                std::string defaultChoice;

                /*!
                 * The UI type of this option - PPD_UI_BOOLEAN,
                 * PPD_UI_PICKONE or PPD_UI_PICKMANY.
                 */
                ppd_ui_t ui;

                /*!
                 * The available choices for the option.
                 */
                std::vector<ppd_choice_t> choices;

                /*!
                 * The current choice pointed to in choices. Used by
                 * the getNextChoice() function. Distinct from
                 * currentChoice.
                 */
                std::vector<ppd_choice_t>::iterator curChoice;
        };

} // namespace wml

#endif // _PPDOPTION_H_

