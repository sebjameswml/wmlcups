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
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getInfo (string cupsPrinter)
{
	WmlIppAttr attr("printer-info");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::WmlCups::setInfo (string cupsPrinter, string s)
{
	WmlIppAttr attr("printer-info");
	attr.setValue (s);
	this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getLocation (string cupsPrinter)
{
	WmlIppAttr attr("printer-location");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::WmlCups::setLocation (string cupsPrinter, string s)
{
	WmlIppAttr attr("printer-location");
	attr.setValue (s);
	this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getMakeModel (string cupsPrinter)
{
	WmlIppAttr attr("printer-make-and-model");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::WmlCups::setMakeModel (string cupsPrinter, string s)
{
	WmlIppAttr attr("printer-make-and-model");
	attr.setValue (s);
	this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getDeviceURI (string cupsPrinter)
{
	WmlIppAttr attr("device-uri");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getCupsURI (string cupsPrinter)
{
	WmlIppAttr attr("printer-uri-supported");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::WmlCups::getPrinterAttribute (const char* printerName,
				   WmlIppAttr& attr)
{
	bool gotPrinter = false;
	ipp_t * prqst;
	ipp_t * rtn;
	char * attrTag;
	// NB: Must free attrTag at end!
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

	prqst = ippNewRequest (CUPS_GET_PRINTERS);

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
			gotPrinter = true;
			break;

		} // end of if printer != NULL...

		if (ipp_attributes == NULL) {
			break;
		}

	} // end of for each ipp response

	ippDelete (rtn);
	if (attrTag != (char*)0) {
		free (attrTag);
	}

	if (!gotPrinter) {
		// We didn't find the printer, so zero attr
		attr.zero();
	}

	return attr.getString();
}

void
wml::WmlCups::setPrinterAttribute (const char* printerName,
				   wml::WmlIppAttr& attr)
{
	ipp_t * prqst;
	ipp_t * rtn;
	char uri[HTTP_MAX_URI];

	prqst = ippNewRequest (CUPS_ADD_PRINTER);

	httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
			  NULL, this->cupsdAddress.c_str(), 0,
			  "/printers/%s", printerName);

	ippAddString(prqst,
		     IPP_TAG_OPERATION, IPP_TAG_URI,
		     "printer-uri", NULL, uri);

	ippAddString(prqst, IPP_TAG_PRINTER,
		     attr.getType(), attr.getName().c_str(),
		     NULL,
		     attr.getString().c_str());

	rtn = cupsDoRequest (this->connection, prqst, "/admin/");

	if (!rtn) {
		// Handle error
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() failed -> "
		    << cupsLastErrorString();
		throw runtime_error (eee.str());
	}
	if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
		// Handle conflict
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() failed -> "
		    << cupsLastErrorString();
		ippDelete (rtn);
		throw runtime_error (eee.str());
	}

	ippDelete (rtn);
	return;
}

bool
wml::WmlCups::printerNameIsValid (string s)
{
	string::size_type sz = s.size();
	if (sz > 127) {
		return false;
	}
	for (unsigned int i=0; i<sz; i++) {
		if ((s[i] >= 0 && s[i] <= ' ')
		    || s[i] == 127 || s[i] == '#' || s[i] == '/') {
			return false;
		}
	}
	return true;
}
