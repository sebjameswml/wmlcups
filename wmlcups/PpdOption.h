/* -*-c++-*- */
/*!
 * A C++ wrapper for some features of the CUPS API.
 *
 * This is the PPD option class.
 *
 * Author: Sebastian James <sjames@wmltd.co.uk>
 *
 * Copyright: 2009 William Matthew Limited.
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
		PpdOption (ppd_option_t* o);
		~PpdOption ();

		/*!
		 * Zero out any stored values.
		 */
		void zero (void);

		/*!
		 * Get the group name.
		 */
		std::string getGroupName (void);

		/*!
		 * Set the group name.
		 */
		void setGroupName (std::string n);

		/*!
		 * Return the keyword of this option.
		 */
		std::string getKeyword (void);

		/*!
		 * Set the keyword to s
		 */
		void setKeyword (std::string s);

		/*!
		 * Return the text of this option.
		 */
		std::string getText (void);

		/*!
		 * Set the text to s
		 */
		void setText (std::string s);

		/*!
		 * Return the value of this option.
		 */
		std::string getChoiceValue (void);

		/*!
		 * Set the choice which has the value s
		 */
		void setChoiceValue (std::string s);

		/*!
		 * Return the text of the chosen option.
		 */
		std::string getChoiceText (void);

		/*!
		 * Set the choice which has the text s
		 */
		void setChoiceText (std::string s);

		/*!
		 * Return the default choice of this option.
		 */
		std::string getDefaultChoice (void);

		/*!
		 * Set the default choice to s
		 */
		void setDefaultChoice (std::string s);

		ppd_ui_t getUi (void);

		void setUi (ppd_ui_t u);

		/*!
		 * Returns the number of elements in this->choices.
		 */
		int getNumChoices (void);

		/*!
		 * Get the next choice.
		 */
		bool getNextChoice (ppd_choice_t& choice);

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

