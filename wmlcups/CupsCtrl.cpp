#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"
#include <futil/WmlDbg.h>

extern "C" {
#include <errno.h>
#include <cups/cups.h>
#include <cups/language.h>
}

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <utility>
#include <set>

#include <futil/FoundryUtilities.h>
#include "QueueCupsStatus.h"
#include "IppAttr.h"
#include "CupsCtrl.h"

using namespace std;
using namespace wml;

wml::CupsCtrl::CupsCtrl ()
{
}

wml::CupsCtrl::CupsCtrl (string addr, int port)
{
	if (addr.empty()) {
		addr = cupsServer();
		// This connects to the cupsd on the localhost

	}

	if (port == 0) {
		port = ippPort();
	}

	this->connection = httpConnectEncrypt (addr.c_str(),
					       port,
					       cupsEncryption());

	try {
		if (this->connection == 0) {
			throw runtime_error ("CupsCtrl: Couldn't connect to the cupsd!");
		}
	} catch (runtime_error& e) {
		DBG ("EX:: Found error: " << e.what());
	}

	this->cupsdAddress = addr;
}

wml::CupsCtrl::~CupsCtrl()
{
	httpClose (this->connection);
}

void
wml::CupsCtrl::initialise (void)
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
		throw runtime_error ("CupsCtrl: Couldn't connect to the cupsd!");
	}
}

void
wml::CupsCtrl::setCupsdAddress (string a)
{
	this->cupsdAddress = a;
}

std::string
wml::CupsCtrl::getCupsdAddress (void)
{
	return this->cupsdAddress;
}

string
wml::CupsCtrl::getCupsURI (string cupsPrinter)
{
	IppAttr attr("printer-uri-supported");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

vector<string>
wml::CupsCtrl::getCupsPrinterList (GET_PRINTERS getPrinters)
{
	vector<string> theList;
	ipp_t * prqst;
	ipp_t * rtn;
	ipp_attribute_t * ipp_attributes;


	if (getPrinters != GET_CLASSES) {
		prqst = ippNewRequest (CUPS_GET_PRINTERS);
	} else {
		prqst = ippNewRequest (CUPS_GET_CLASSES);
	}

	static const char * printerAttributes[]= {
		"printer-name"
	};
	int n_attributes = 1;

	ippAddStrings(prqst,
		      IPP_TAG_OPERATION,
		      IPP_TAG_KEYWORD,
		      "requested-attributes",
		      n_attributes,
		      NULL,
		      printerAttributes);

	if (getPrinters == GET_PRINTERS_ONLY) {
		ippAddInteger(prqst,
			      IPP_TAG_OPERATION,
			      IPP_TAG_ENUM,
			      "printer-type-mask", CUPS_PRINTER_CLASS);
	}

	rtn = cupsDoRequest (this->connection, prqst, "/");

	if (!rtn) {
		// Handle error
		throw runtime_error ("CupsCtrl: cupsDoRequest() failed");
	} else {

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


			if (!strcmp(ipp_attributes->name, "printer-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {

				string p(ipp_attributes->values[0].string.text);

				if (!p.empty()) {
					theList.push_back (p);
				}
			} else if (!strcmp(ipp_attributes->name, "member-uris")){

				int numvalues = ipp_attributes->num_values;

				for (int i = 0; i < numvalues; i++) {
					string p(ipp_attributes->values[i].string.text);
					DBG ("Printer: " << p << " is member of this class");
				}


			}

			ipp_attributes = ipp_attributes->next;
		}

		if (ipp_attributes == NULL) {
			break;
		}

	} // end of for each ipp response

	ippDelete (rtn);

	return theList;
}

list<string>
wml::CupsCtrl::getCupsPrinterList2 (void)
{
	list<string> theList;
	ipp_t * prqst;
	ipp_t * rtn;
	ipp_attribute_t * ipp_attributes;
	static const char * printerAttributes[] = {
		"printer-name"
	};
	int n_attributes = 1;

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
		throw runtime_error ("CupsCtrl: cupsDoRequest() failed");
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

			if (!strcmp(ipp_attributes->name, "printer-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {
				string p(ipp_attributes->values[0].string.text);
				if (!p.empty()) {
					theList.push_back (p);
				}
			}

			ipp_attributes = ipp_attributes->next;
		}

		if (ipp_attributes == NULL) {
			break;
		}

	} // end of for each ipp response

	ippDelete (rtn);

	return theList;
}




