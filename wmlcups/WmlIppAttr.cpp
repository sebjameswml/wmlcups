#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <utility>

#include "WmlIppAttr.h"

using namespace std;
using namespace wml;

wml::WmlIppAttr::WmlIppAttr () :
	name(""),
	type (IPP_TAG_UNKNOWN),
	stringValue(""),
	intValue(0)
{
}

wml::WmlIppAttr::WmlIppAttr (const char* attributeName) :
	name(attributeName),
	type (IPP_TAG_UNKNOWN),
	stringValue(""),
	intValue(0)
{
	// We have an attribute name to work with, so get the type.
	this->determineType();
}

wml::WmlIppAttr::~WmlIppAttr ()
{
}

string
wml::WmlIppAttr::getName (void)
{
	return this->name;
}

ipp_tag_t
wml::WmlIppAttr::getType (void)
{
	return this->type;
}

string
wml::WmlIppAttr::getString (void)
{
	// Hmm - should work on type, rather than whether or not
	// string is empty.
	string s;
	if (this->stringValue.empty()) {
		stringstream ss;
		ss << this->intValue;
		s = ss.str();
	} else {
		s = this->stringValue;
	}
	return s;
}

int
wml::WmlIppAttr::getInt (void)
{
	return this->intValue;
}

void
wml::WmlIppAttr::setValue (string s)
{
	this->stringValue = s;
}

void
wml::WmlIppAttr::setValue (int i)
{
	this->intValue = i;
}

void
wml::WmlIppAttr::determineType (void)
{
	// Figure out the IPP_TAG_XXXX from  name.

	string start = this->name.substr(0, 7);
	if (start == "printer") {
		// printer-something attributes
		switch (this->name[8]) {
		case 'n':
			if (this->name == "printer-name") {
				this->type = IPP_TAG_NAME;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}
			break;
		case 's':
			if (this->name == "printer-state") {
				this->type = IPP_TAG_ENUM;
			} else if (this->name == "printer-state-message") {
				this->type = IPP_TAG_TEXT;
			} else if (this->name == "printer-state-time") {
				this->type = IPP_TAG_INTEGER;
			} else if (this->name == "printer-state-reasons") {
				this->type = IPP_TAG_KEYWORD;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}
			break;
		case 't':
			if (this->name == "printer-type") {
				this->type = IPP_TAG_ENUM;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}
			break;
		case 'i':
			if (this->name == "printer-info") {
				this->type = IPP_TAG_TEXT;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}
			break;
		case 'l':
			if (this->name == "printer-location") {
				this->type = IPP_TAG_TEXT;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}
			break;
		case 'm':
			if (this->name == "printer-make-and-model") {
				this->type = IPP_TAG_TEXT;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}

			break;
		case 'u':
			if (this->name == "printer-uri-supported") {
				this->type = IPP_TAG_URI;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}
			break;
		default:
			this->type = IPP_TAG_UNKNOWN;
			break;
		}
	} else if (start == "request") {
		// requesting-something attributes
		switch (this->name[11]) {
		case 'u':
			if (this->name == "requesting-user-name-allowed") {
				this->type = IPP_TAG_NAME;
			} else if (this->name == "requesting-user-name-denied") {
				this->type = IPP_TAG_NAME;
			} else {
				this->type = IPP_TAG_UNKNOWN;
			}

			break;
		default:
			this->type = IPP_TAG_UNKNOWN;
			break;
		}
	} else {
		this->type = IPP_TAG_UNKNOWN;
	}
}
