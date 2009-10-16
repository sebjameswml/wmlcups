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
#include "wmlcups.h"

using namespace std;
using namespace wml;

wml::WmlCups::WmlCups ()
{
}

wml::WmlCups::WmlCups (string addr)
{
	if (addr.empty()) {
		// This connects to the cupsd on the localhost
		this->connection = httpConnectEncrypt (cupsServer(),
						       ippPort(),
						       cupsEncryption());
	} else {
		// Connect to indicated cupsdaddr
		this->connection = httpConnectEncrypt (addr.c_str(),
						       ippPort(),
						       cupsEncryption());
	}

	if (this->connection == 0) {
		throw runtime_error ("WmlCups: Couldn't connect to the cupsd!");
	}

	this->cupsdAddress = addr;
}

wml::WmlCups::~WmlCups()
{
	httpClose (this->connection);
}

void
wml::WmlCups::initialise (void)
{
	if (this->cupsdAddress.empty()) {
		// This connects to the cupsd on the localhost
		this->connection = httpConnectEncrypt (cupsServer(),
						       ippPort(),
						       cupsEncryption());
	} else {
		// Connect to indicated cupsdaddr
		this->connection = httpConnectEncrypt (this->cupsdAddress.c_str(),
						       ippPort(),
						       cupsEncryption());
	}

	if (this->connection == 0) {
		throw runtime_error ("WmlCups: Couldn't connect to the cupsd!");
	}
}

void
wml::WmlCups::setCupsdAddress (string a)
{
	this->cupsdAddress = a;
}

bool
wml::WmlCups::getAccepting (string cupsPrinter)
{
	WmlIppAttr attr("printer-is-accepting-jobs");

	getPrinterAttribute (cupsPrinter.c_str(), attr);

	if (attr.getInt() > 0) {
		return true;
	}

	return false;
}

bool
wml::WmlCups::getEnabled (string cupsPrinter)
{
	WmlIppAttr attr("printer-state");

	getPrinterAttribute (cupsPrinter.c_str(), attr);

	// Printer state is IPP_PRINTER_PROCESSING (4), _IDLE (3) or _STOPPED (5)
	if (attr.getInt() == (int)IPP_PRINTER_STOPPED) {
		return false;
	}

	return true;
}

string
wml::WmlCups::getState (string cupsPrinter)
{
	WmlIppAttr attr("printer-state");
	getPrinterAttribute (cupsPrinter.c_str(), attr);
	string state("");
	switch (attr.getInt()) {
	case (int)IPP_PRINTER_PROCESSING:
		state = "processing";
		break;
	case (int)IPP_PRINTER_IDLE:
		state = "idle";
		break;
	case (int)IPP_PRINTER_STOPPED:
		state = "stopped";
		break;
	default:
		state = "error reading state";
		break;
	}

	return state;
}

string
wml::WmlCups::getStateMsg (string cupsPrinter)
{
	WmlIppAttr attr("printer-state-message");
	return getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getInfo (string cupsPrinter)
{
	WmlIppAttr attr("printer-info");
	return getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getLocation (string cupsPrinter)
{
	WmlIppAttr attr("printer-location");
	return getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getMakeModel (string cupsPrinter)
{
	WmlIppAttr attr("printer-make-and-model");
	return getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getDeviceURI (string cupsPrinter)
{
	WmlIppAttr attr("device-uri");
	return getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getCupsURI (string cupsPrinter)
{
	WmlIppAttr attr("printer-uri-supported");
	return getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getPrinterAttribute (const char* printerName,
				   WmlIppAttr& attr)
{
	ipp_t * prqst;
	ipp_t * rtn;
	cups_lang_t * lang;

	char * attrTag;
	// NB: Must free at end!
	attrTag = (char*)calloc (1+attr.getName().size(), sizeof(char));
	strcpy (attrTag, attr.getName().c_str());
	char printerNameTag[] = "printer-name";
	const char * printerAttributes[2];
	printerAttributes[0] = printerNameTag;
	printerAttributes[1] = attrTag;
	// Doesn't work:
	//printerAttributes[1] = attr.getName().c_str();
	int n_attributes = 2;

	ipp_attribute_t * ipp_attributes;
	char * printer = NULL;

	prqst = ippNew();
	prqst->request.op.operation_id = CUPS_GET_PRINTERS; // May need to pass this in?
	prqst->request.op.request_id   = 1;

	lang = cupsLangDefault();

	ippAddString(prqst,
		     IPP_TAG_OPERATION,
		     IPP_TAG_CHARSET,
		     "attributes-charset",
		     NULL,
		     cupsLangEncoding(lang));

	ippAddString(prqst,
		     IPP_TAG_OPERATION,
		     IPP_TAG_LANGUAGE,
		     "attributes-natural-language",
		     NULL,
		     lang->language);

	ippAddStrings(prqst,
		      IPP_TAG_OPERATION,
		      IPP_TAG_KEYWORD,
		      "requested-attributes",
		      n_attributes,
		      NULL,
		      printerAttributes);

	rtn = cupsDoRequest (this->connection, prqst, "/");

	if (!rtn) {
		// Handle error
		cupsLangFree (lang);
		throw runtime_error ("WmlCups: cupsDoRequest() failed");
	}

	for (ipp_attributes = rtn->attrs;
	     ipp_attributes != NULL;
	     ipp_attributes = ipp_attributes->next) {

		while (ipp_attributes != NULL
		       && ipp_attributes->group_tag != IPP_TAG_PRINTER) {
			// Move on to the next one.
			ipp_attributes = ipp_attributes->next;
		}

		while (ipp_attributes != NULL &&
		       ipp_attributes->group_tag == IPP_TAG_PRINTER) {

			if (!strcmp(ipp_attributes->name, attr.getName().c_str()) &&
			    ipp_attributes->value_tag == attr.getType()) {
				// Set the value, based on what kind
				// of value this is. We COULD do
				// attr.setValue(ipp_attributes->values[0]
				// or somesuch. Then this logic would
				// go into WmlIppAttr
				switch (attr.getType()) {
				case IPP_TAG_TEXT:
				case IPP_TAG_NAME:
				case IPP_TAG_URI:
					attr.setValue (ipp_attributes->values[0].string.text);
					break;
				case IPP_TAG_ENUM:
				case IPP_TAG_INTEGER:
					attr.setValue (ipp_attributes->values[0].integer);
					break;
				case IPP_TAG_BOOLEAN:
					attr.setValue (ipp_attributes->values[0].boolean);
					break;
				default:
					// Can't set value from this type
					break;
				}
			}
			if (!strcmp(ipp_attributes->name, "printer-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {
				printer = ipp_attributes->values[0].string.text;
			}

			ipp_attributes = ipp_attributes->next;
		}

		if (printer != NULL
		    && !strcmp (printer, printerName)) {
			// Then we have a queue of the right name. We
			// should already have configured attr, above,
			// so just break now.
			break;

		} // end of if printer != NULL...

		if (ipp_attributes == NULL) {
			break;
		}

	} // end of for each ipp response

	ippDelete (rtn);
	cupsLangFree (lang);

	if (attrTag != (char*)0) {
		free (attrTag);
	}

	return attr.getString();
}
