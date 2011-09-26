#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"

#include <futil/WmlDbg.h>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "PpdOption.h"

using namespace std;
using namespace wml;

wml::PpdOption::PpdOption () :
        groupName(""),
        keyword(""),
        text(""),
        defaultChoice(""),
        ui(PPD_UI_BOOLEAN)
{
        this->curChoice = this->choices.begin();
}

wml::PpdOption::PpdOption (ppd_option_t* o) :
        groupName(""),
        keyword(o->keyword),
        text(o->text),
        defaultChoice(o->defchoice),
        ui(o->ui)

{
        // Now get the choices.
        for (int i = 0; i < o->num_choices; i++) {
                this->choices.push_back (o->choices[i]);
        }
        this->curChoice = this->choices.begin();
}

wml::PpdOption::~PpdOption ()
{
}

void
wml::PpdOption::zero (void)
{
        this->groupName = "";
        this->keyword = "";
        this->text = "";
        this->defaultChoice = "";
        this->ui = PPD_UI_BOOLEAN;
        this->choices.clear();
        this->curChoice = this->choices.begin();
}

string
wml::PpdOption::getGroupName (void)
{
        return this->groupName;
}

void
wml::PpdOption::setGroupName (string s)
{
        this->groupName = s;
}

string
wml::PpdOption::getKeyword (void)
{
        return this->keyword;
}

void
wml::PpdOption::setKeyword (string s)
{
        this->keyword = s;
}

string
wml::PpdOption::getText (void)
{
        return this->text;
}

void
wml::PpdOption::setText (string s)
{
        this->text = s;
}

string
wml::PpdOption::getDefaultChoice (void)
{
        return this->defaultChoice;
}

void
wml::PpdOption::setDefaultChoice (string s)
{
        this->defaultChoice = s;
}

ppd_ui_t
wml::PpdOption::getUi (void)
{
        return this->ui;
}

void
wml::PpdOption::setUi (ppd_ui_t u)
{
        this->ui = u;;
}

int
wml::PpdOption::getNumChoices (void)
{
        return this->choices.size();
}

void
wml::PpdOption::resetCurChoice (void)
{
        this->curChoice = this->choices.begin();
}

bool
wml::PpdOption::getNextChoice (ppd_choice_t& choice)
{
        if (this->curChoice == this->choices.end()) {
                return false;
        }

        string cchoice (curChoice->choice);

        DBG ("Next choice is " + cchoice);

        choice = *curChoice;
        curChoice++;
        return true;
}

string
wml::PpdOption::getChoiceValue (void)
{
        string s("");
        std::vector<ppd_choice_t>::iterator currentChoice;
        currentChoice = this->choices.begin();
        while (currentChoice != this->choices.end()) {
                if (currentChoice->marked) {
                        s = currentChoice->choice;
                        break;
                }
                currentChoice++;
        }
        return s;
}

void
wml::PpdOption::setChoiceValue (string s)
{
        std::vector<ppd_choice_t>::iterator currentChoice;
        // This really sets currentChoice, based on a match of the choice value.
        currentChoice = this->choices.begin();
        while (currentChoice != this->choices.end()) {
                if (currentChoice->choice == s) {
                        // Match. Mark it.
                        currentChoice->marked = 1;
                } else {
                        currentChoice->marked = 0;
                }
                currentChoice++;
        }
}

string
wml::PpdOption::getChoiceText (void)
{
        string s("");
        std::vector<ppd_choice_t>::iterator currentChoice;
        currentChoice = this->choices.begin();
        while (currentChoice != this->choices.end()) {
                if (currentChoice->marked) {
                        s = currentChoice->text;
                        break;
                }
                currentChoice++;
        }
        return s;
}

void
wml::PpdOption::setChoiceText (string s)
{
        std::vector<ppd_choice_t>::iterator currentChoice;
        // This really sets currentChoice, based on a match of the text.
        currentChoice = this->choices.begin();
        while (currentChoice != this->choices.end()) {
                if (currentChoice->text == s) {
                        currentChoice->marked = 1;
                } else {
                        currentChoice->marked = 0;
                }
                currentChoice++;
        }
}

