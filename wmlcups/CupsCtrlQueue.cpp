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

bool
wml::CupsCtrl::getAccepting (string cupsPrinter)
{
	IppAttr attr("printer-is-accepting-jobs");

	getPrinterAttribute (cupsPrinter.c_str(), attr);

	if (attr.getInt() > 0) {
		return true;
	}

	return false;
}

void
wml::CupsCtrl::setAccepting (string cupsPrinter, bool accept, string directory)
{
	if (accept == true) {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "CupsCtrl",
					  "CupsCtrl::setAccepting(true)",
					  CUPS_ACCEPT_JOBS,
					  directory);
	} else {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "CupsCtrl",
					  "CupsCtrl::setAccepting(false)",
					  CUPS_REJECT_JOBS,
					  directory);
	}
}

bool
wml::CupsCtrl::getEnabled (string cupsPrinter)
{
	IppAttr attr("printer-state");

	getPrinterAttribute (cupsPrinter.c_str(), attr);

	// Printer state is IPP_PRINTER_PROCESSING (4), _IDLE (3) or _STOPPED (5)
	if (attr.getInt() == (int)IPP_PRINTER_STOPPED) {
		return false;
	}

	return true;
}

void
wml::CupsCtrl::setEnabled (string cupsPrinter, bool enable, string directory)
{
	if (enable == true) {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "CupsCtrl",
					  "CupsCtrl::setEnabled(true)",
					  IPP_RESUME_PRINTER,
					  directory);
	} else {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "CupsCtrl",
					  "CupsCtrl::setEnabled(false)",
					  IPP_PAUSE_PRINTER,
					  directory);
	}
}


string
wml::CupsCtrl::getState (string cupsPrinter)
{
	IppAttr attr("printer-state");
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
wml::CupsCtrl::getStateMsg (string cupsPrinter)
{
	IppAttr attr("printer-state-message");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::CupsCtrl::getStateReasons (string cupsPrinter)
{
	IppAttr attr("printer-state-reasons");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

bool
wml::CupsCtrl::getFullStatus (std::string cupsPrinter,
			      wml::QueueCupsStatus& qstat)
{
	DBG ("Called for queue " << cupsPrinter);

	bool gotPrinter = false;
	ipp_t * prqst;
	ipp_t * rtn;
	static const char * printerAttributes[] = {
		"printer-name",
		"printer-state",
		"printer-state-message",
		"printer-is-accepting-jobs"
	};
	int n_attributes = 4;
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

			// State/Enabled
			if (!strcmp(ipp_attributes->name, "printer-state") &&
			    ipp_attributes->value_tag == IPP_TAG_ENUM) {
				switch (ipp_attributes->values[0].integer) {
				case (int)IPP_PRINTER_PROCESSING:
					qstat.state = "processing";
					qstat.enabled = true;
					break;
				case (int)IPP_PRINTER_IDLE:
					qstat.state = "idle";
					qstat.enabled = true;
					break;
				case (int)IPP_PRINTER_STOPPED:
					qstat.state = "stopped";
					qstat.enabled = false;
					break;
				default:
					qstat.state = "error reading state";
					break;
				}
			}

			// State Message
			if (!strcmp(ipp_attributes->name, "printer-state-message") &&
			    ipp_attributes->value_tag == IPP_TAG_TEXT) {
				qstat.stateMsg = ipp_attributes->values[0].string.text;
			}

			// Accepting
			if (!strcmp(ipp_attributes->name, "printer-is-accepting-jobs") &&
			    ipp_attributes->value_tag == IPP_TAG_BOOLEAN) {
				if (ipp_attributes->values[0].boolean) {
					qstat.accepting = true;
				} else {
					qstat.accepting = false;
				}
			}

			// Printer name.
			if (!strcmp(ipp_attributes->name, "printer-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {
				printer = ipp_attributes->values[0].string.text;
			}

			ipp_attributes = ipp_attributes->next;
		}

		if (printer != NULL
		    && !strcmp (printer, cupsPrinter.c_str())) {
			// Then we have a queue of the right name. We
			// should already have configured the features above.
			// so just break now.
			gotPrinter = true;
			break;

		} // end of if printer != NULL...

		if (ipp_attributes == NULL) {
			break;
		}

	} // end of for each ipp response

	ippDelete (rtn);

	if (gotPrinter == true) {
		DBG ("Calling getJobStatus now");
		this->getJobStatus (cupsPrinter, 0, qstat.lastJob);
	}

	return gotPrinter;
}

string
wml::CupsCtrl::getInfo (string cupsPrinter)
{
	IppAttr attr("printer-info");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::CupsCtrl::getLocation (string cupsPrinter)
{
	IppAttr attr("printer-location");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

vector<string>
wml::CupsCtrl::getPrinterClassesList(void)
{

	return this->getCupsPrinterList (GET_BOTH);


}

wml::QTYPE
wml::CupsCtrl::getQueueType (string queuename)
{
	vector<string> classes;
	vector<string> printers;
	classes = this->getClassesList();
	printers = this->getPrinterList();

	vector<string>::iterator iter;
	for (iter = classes.begin(); iter != classes.end(); iter++) {
		if (queuename == *iter) {
			return WMLCUPS_CLASS;
		}
	}

	for (iter = printers.begin(); iter != printers.end(); iter++) {
		if (queuename == *iter) {
			return WMLCUPS_PRINTER;
		}
	}

	return WMLCUPS_UNKNOWNTYPE;
}
