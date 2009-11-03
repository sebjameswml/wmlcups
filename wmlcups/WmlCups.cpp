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

#include "QueueCupsStatus.h"
#include "WmlIppAttr.h"
#include "WmlCups.h"

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

void
wml::WmlCups::setAccepting (string cupsPrinter, bool accept)
{
	if (accept == true) {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "WmlCups",
					  "WmlCups::setAccepting(true)",
					  CUPS_ACCEPT_JOBS);
	} else {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "WmlCups",
					  "WmlCups::setAccepting(false)",
					  CUPS_REJECT_JOBS);
	}
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

void
wml::WmlCups::setEnabled (string cupsPrinter, bool enable)
{
	if (enable == true) {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "WmlCups",
					  "WmlCups::setEnabled(true)",
					  IPP_RESUME_PRINTER);
	} else {
		this->sendPrinterCommand (cupsPrinter.c_str(),
					  "WmlCups",
					  "WmlCups::setEnabled(false)",
					  IPP_PAUSE_PRINTER);
	}
}

bool
wml::WmlCups::printerExists (string cupsPrinter)
{
	WmlIppAttr attr("printer-name");
	getPrinterAttribute (cupsPrinter.c_str(), attr);
	if (attr.getString() == cupsPrinter) {
		return true;
	}
	return false;
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
wml::WmlCups::getStateReasons (string cupsPrinter)
{
	WmlIppAttr attr("printer-state-reasons");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

bool
wml::WmlCups::getFullStatus (std::string cupsPrinter,
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

void
wml::WmlCups::getJobList (string cupsPrinter, vector<CupsJob>& jList, string whichJobs)
{
	// Could be:
	// this->getJobList (cupsPrinter, jList, 0, whichJobs);
	// But better as:
	ipp_t * jrqst;
	ipp_t * rtn;
	ipp_attribute_t * ipp_attributes;
	char uri[HTTP_MAX_URI];

	jrqst = ippNewRequest (IPP_GET_JOBS);

	if (!cupsPrinter.empty()) {
		httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
				  NULL, this->cupsdAddress.c_str(), 0,
				  "/printers/%s", cupsPrinter.c_str());
	} else {
		snprintf (uri, HTTP_MAX_URI, "ipp://%s/", this->cupsdAddress.c_str());
	}

	ippAddString (jrqst,
		      IPP_TAG_OPERATION,
		      IPP_TAG_URI,
		      "printer-uri",
		      NULL,
		      uri);

	if (!whichJobs.empty()) {
		ippAddString (jrqst,
			      IPP_TAG_OPERATION,
			      IPP_TAG_KEYWORD,
			      "which-jobs",
			      NULL,
			      whichJobs.c_str());
	}

	rtn = cupsDoRequest (this->connection, jrqst, "/");

	if (!rtn) {
		// Handle error
		throw runtime_error ("WmlCups: cupsDoRequest() failed");
	}

	if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
		// Handle conflict
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		ippDelete (rtn);
		throw runtime_error (eee.str());
	}

	/*
	 * Would like to work backwards, ideally, but CUPS API doesn't
	 * seem to provide for this, even though struct ipp_s has a
	 * "prev" field since CUPS 1.2.
	 */
	for (ipp_attributes = rtn->attrs;
	     ipp_attributes != NULL;
	     ipp_attributes = ipp_attributes->next) {

		while (ipp_attributes != NULL
		       && ipp_attributes->group_tag != IPP_TAG_JOB) {
			// Move on to the next one.
			ipp_attributes = ipp_attributes->next;
		}

		CupsJob j;
		while (ipp_attributes != NULL &&
		       ipp_attributes->group_tag == IPP_TAG_JOB) {

			if (!strcmp(ipp_attributes->name, "job-id") &&
			    ipp_attributes->value_tag == IPP_TAG_INTEGER) {
				j.setId (ipp_attributes->values[0].integer);
				DBG ("Found job ID " << j.getId());
			}
			if (!strcmp(ipp_attributes->name, "copies") &&
			    ipp_attributes->value_tag == IPP_TAG_INTEGER) {
				j.setCopies (ipp_attributes->values[0].integer);
			}
			if (!strcmp(ipp_attributes->name, "job-k-octets") &&
			    ipp_attributes->value_tag == IPP_TAG_INTEGER) {
				j.setSizeKB (ipp_attributes->values[0].integer);
			}
			if (!strcmp(ipp_attributes->name, "job-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {
				j.setName (ipp_attributes->values[0].string.text);
			}
			if (!strcmp(ipp_attributes->name, "job-orginating-user-name") &&
			    ipp_attributes->value_tag == IPP_TAG_NAME) {
				j.setUser (ipp_attributes->values[0].string.text);
			}
			if (!strcmp(ipp_attributes->name, "job-printer-uri") &&
			    ipp_attributes->value_tag == IPP_TAG_URI) {
				j.setPrinterUri (ipp_attributes->values[0].string.text);
			}
			if (!strcmp(ipp_attributes->name, "job-state") &&
			    ipp_attributes->value_tag == IPP_TAG_ENUM) {
				j.setState ((ipp_jstate_t)ipp_attributes->values[0].integer);
			}
			if (!strcmp(ipp_attributes->name, "time-at-creation") &&
			    ipp_attributes->value_tag == IPP_TAG_INTEGER) {
				j.setTime (ipp_attributes->values[0].integer);
			}

			ipp_attributes = ipp_attributes->next;
		}

		// Add the new job to the list
		if (j.getId() != 0) {
			jList.push_back (j);
		}

		if (ipp_attributes == NULL) {
			break;
		}
	}

}

/*
 * Tried two alternative implementations of getJobList, but couldn't
 * make it run any faster than 0.2 seconds to get the last job info
 * out of about 450 jobs on a queue. CUPS really needs a way to walk
 * backward through the list of jobs.
 */
void
wml::WmlCups::getJobList (string cupsPrinter,
			  vector<CupsJob>& jList,
			  int numJobs,
			  string whichJobs)
{
	DBG ("Called");

	ipp_t * jrqst;
	ipp_t * rtn;
	ipp_attribute_t * ipp_attributes;
	char uri[HTTP_MAX_URI];

	jrqst = ippNewRequest (IPP_GET_JOBS);

	if (!cupsPrinter.empty()) {
		httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
				  NULL, this->cupsdAddress.c_str(), 0,
				  "/printers/%s", cupsPrinter.c_str());
	} else {
		snprintf (uri, HTTP_MAX_URI, "ipp://%s/", this->cupsdAddress.c_str());
	}

	ippAddString (jrqst,
		      IPP_TAG_OPERATION,
		      IPP_TAG_URI,
		      "printer-uri",
		      NULL,
		      uri);

	if (!whichJobs.empty()) {
		ippAddString (jrqst,
			      IPP_TAG_OPERATION,
			      IPP_TAG_KEYWORD,
			      "which-jobs",
			      NULL,
			      whichJobs.c_str());
	}

	rtn = cupsDoRequest (this->connection, jrqst, "/");

	if (!rtn) {
		// Handle error
		throw runtime_error ("WmlCups: cupsDoRequest() failed");
	}

	if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
		// Handle conflict
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		ippDelete (rtn);
		throw runtime_error (eee.str());
	}

	/*
	 * Would like to work backwards, ideally, but CUPS API doesn't
	 * seem to provide for this, even though struct ipp_s has a
	 * "prev" field since CUPS 1.2.
	 *
	 * This is second best - reverse the list, putting it into a
	 * vector.
	 */
	vector<ipp_attribute_t*> attrList;
	for (ipp_attributes = rtn->attrs;
	     ipp_attributes != NULL;
	     ipp_attributes = ipp_attributes->next) {
		attrList.push_back (ipp_attributes);
	}

	if (attrList.empty()) {
		DBG ("Empty list; return.");
		return;
	}

	// Go from the end and backwards
	vector<ipp_attribute_t*>::iterator i = attrList.end();
	i--;
	int count = 0;
	while (i != attrList.begin()) {

		while (i != attrList.begin()
		       && (*i)->group_tag != IPP_TAG_JOB) {
			// Move on to the next one back.
			i--;
		}

		// Now, (*i)->group_tag IS IPP_TAG_JOB.

		count++;
		if (count > numJobs) {
			DBG ("Reached " << count << " jobs, breaking");
			break;
		}

		CupsJob j;
		// Move back through the attributes now
		while (i != attrList.begin() &&
		       (*i)->group_tag == IPP_TAG_JOB) {

			if (!strcmp((*i)->name, "job-id") &&
			    (*i)->value_tag == IPP_TAG_INTEGER) {
				j.setId ((*i)->values[0].integer);
				DBG ("Found job ID " << j.getId());
			}
			if (!strcmp((*i)->name, "copies") &&
			    (*i)->value_tag == IPP_TAG_INTEGER) {
				j.setCopies ((*i)->values[0].integer);
			}
			if (!strcmp((*i)->name, "job-k-octets") &&
			    (*i)->value_tag == IPP_TAG_INTEGER) {
				j.setSizeKB ((*i)->values[0].integer);
			}
			if (!strcmp((*i)->name, "job-name") &&
			    (*i)->value_tag == IPP_TAG_NAME) {
				j.setName ((*i)->values[0].string.text);
			}
			if (!strcmp((*i)->name, "job-orginating-user-name") &&
			    (*i)->value_tag == IPP_TAG_NAME) {
				j.setUser ((*i)->values[0].string.text);
			}
			if (!strcmp((*i)->name, "job-printer-uri") &&
			    (*i)->value_tag == IPP_TAG_URI) {
				j.setPrinterUri ((*i)->values[0].string.text);
			}
			if (!strcmp((*i)->name, "job-state") &&
			    (*i)->value_tag == IPP_TAG_ENUM) {
				j.setState ((ipp_jstate_t)(*i)->values[0].integer);
			}
			if (!strcmp((*i)->name, "time-at-creation") &&
			    (*i)->value_tag == IPP_TAG_INTEGER) {
				j.setTime ((*i)->values[0].integer);
			}

			i--;
		}

		// Add the new job to the list
		if (j.getId() != 0) {
			jList.push_back (j);
		}

		// Don't think we break on (*i) being NULL here.
	}
}

void
wml::WmlCups::getJobStatus (string cupsPrinter, int id, CupsJob& j)
{
	if (j.getId()>0) {
		j.reset();
	}
	vector<CupsJob> jList;
	if (id == 0) {
		// Although we had to get all the blinking job information
		// from cupsd, just copy the information from the last one:
		DBG ("Call getJobList():");
		this->getJobList (cupsPrinter, jList, 1, "all");
		if (!jList.empty()) {
			j = jList.front();
		}
	} else {
		this->getJobList (cupsPrinter, jList, "all");
		vector<CupsJob>::iterator i = jList.begin();
		while (i != jList.end()) {
			if (i->getId() == id) {
				// Match!
				j = *i;
				return;
			}
			i++;
		}
	}
	return;
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
	DBG ("Called");
	WmlIppAttr attr("device-uri");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::WmlCups::setDeviceURI (string cupsPrinter, string s)
{
	DBG ("Called to set device uri to '" << s << "'");
	WmlIppAttr attr("device-uri");
	attr.setValue (s);
	DBG ("Calling setPrinterAttribute for '" << cupsPrinter << "'...");
	this->setPrinterAttribute (cupsPrinter.c_str(), attr);
	DBG ("Called setPrinterAttribute...");
}

void
wml::WmlCups::addPrinter (string cupsPrinter, string devURI)
{
	DBG ("Called");
	try {
		this->getDeviceURI(cupsPrinter);
		// If we get here without exception, printer exists,
		// but we'll set the DeviceURI anyway:
		DBG ("Printer already exists, setting device uri anyway");
		this->setDeviceURI (cupsPrinter, devURI);

	} catch (const exception& e ) {

		string ee = e.what();
		if (ee == "Unknown") {
			this->setDeviceURI (cupsPrinter, devURI);
		} else {
			// Re-throw other exceptions
			throw e;
		}
	}
}

void
wml::WmlCups::deletePrinter (string cupsPrinter)
{
	DBG ("Called");
	ipp_t * prqst;
	ipp_t * rtn;
	char uri[HTTP_MAX_URI];

	prqst = ippNewRequest (CUPS_DELETE_PRINTER);

	httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
			  NULL, this->cupsdAddress.c_str(), 0,
			  "/printers/%s", cupsPrinter.c_str());

	ippAddString(prqst,
		     IPP_TAG_OPERATION, IPP_TAG_URI,
		     "printer-uri", NULL, uri);

	rtn = cupsDoRequest (this->connection, prqst, "/admin/");

	if (!rtn) {
		// Handle error
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		throw runtime_error (eee.str());
	}
	if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
		// Handle conflict
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		ippDelete (rtn);
		throw runtime_error (eee.str());
	}

	ippDelete (rtn);
	return;
}

string
wml::WmlCups::getCupsURI (string cupsPrinter)
{
	WmlIppAttr attr("printer-uri-supported");
	return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

vector<string>
wml::WmlCups::getCupsPrinterList (void)
{
	vector<string> theList;
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
				case IPP_TAG_KEYWORD:
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
		eee << "WmlCups: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		throw runtime_error (eee.str());
	}
	if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
		// Handle conflict
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		ippDelete (rtn);
		throw runtime_error (eee.str());
	}

	ippDelete (rtn);
	return;
}

void
wml::WmlCups::sendPrinterCommand (const char* printerName,
				  string asUser,
				  string reason,
				  ipp_op_t command)
{
	ipp_t * prqst;
	ipp_t * rtn;
	char uri[HTTP_MAX_URI];

	prqst = ippNewRequest (command);

	httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
			  NULL, this->cupsdAddress.c_str(), 0,
			  "/printers/%s", printerName);

	ippAddString(prqst,
		     IPP_TAG_OPERATION, IPP_TAG_URI,
		     "printer-uri", NULL, uri);

	ippAddString(prqst,
		     IPP_TAG_OPERATION, IPP_TAG_NAME,
		     "requesting-user-name", NULL, asUser.c_str());

	// Can also add a reason string, if useful:
	ippAddString(prqst,
		     IPP_TAG_OPERATION, IPP_TAG_TEXT,
		     "printer-state-message", NULL, reason.c_str());

	rtn = cupsDoRequest (this->connection, prqst, "/admin/");

	if (!rtn) {
		// Handle error
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
		throw runtime_error (eee.str());
	}
	if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
		// Handle conflict
		stringstream eee;
		eee << "WmlCups: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
		    << hex << cupsLastError() << " ("
		    << this->errorString (cupsLastError()) << ")";
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

bool
wml::WmlCups::addrIsValid (string s)
{
	// For now, this is the same as printerNameIsValid(). Needs
	// checking really.
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

bool
wml::WmlCups::lpdqIsValid (string s)
{
	// Allow alphanumerics only
	string::size_type sz = s.size();
	if (sz > 127) {
		return false;
	}
	for (unsigned int i=0; i<sz; i++) {
		if (s[i] < '0'
		    || (s[i] > '9' && s[i] < 'A')
		    || (s[i] > 'Z' && s[i] < 'a')
		    || s[i] > 'z') {
			return false;
		}
	}
	return true;
}

string
wml::WmlCups::errorString (ipp_status_t err)
{
	string errStr("Unknown");
	switch (err) {


	case IPP_OK:
		errStr = "successful-ok";
		break;

	case IPP_OK_SUBST:
		errStr = "successful-ok-ignored-or-substituted-attributes";
		break;

	case IPP_OK_CONFLICT:
		errStr = "successful-ok-conflicting-attributes";
		break;

	case IPP_OK_IGNORED_SUBSCRIPTIONS:
		errStr = "successful-ok-ignored-subscriptions";
		break;

	case IPP_OK_IGNORED_NOTIFICATIONS:
		errStr = "successful-ok-ignored-notifications";
		break;

	case IPP_OK_TOO_MANY_EVENTS:
		errStr = "successful-ok-too-many-events";
		break;

	case IPP_OK_BUT_CANCEL_SUBSCRIPTION:
		errStr = "successful-ok-but-cancel-subscription";
		break;

	case IPP_OK_EVENTS_COMPLETE:
		errStr = "successful-ok-events-complete";
		break;

	case IPP_REDIRECTION_OTHER_SITE:
		errStr = "redirection-other-site @private@";
		break;

	case CUPS_SEE_OTHER:
		errStr = "cups-see-other";
		break;

	case IPP_BAD_REQUEST:
		errStr = "client-error-bad-request";
		break;

	case IPP_FORBIDDEN:
		errStr = "client-error-forbidden";
		break;

	case IPP_NOT_AUTHENTICATED:
		errStr = "client-error-not-authenticated";
		break;

	case IPP_NOT_AUTHORIZED:
		errStr = "client-error-not-authorized";
		break;

	case IPP_NOT_POSSIBLE:
		errStr = "client-error-not-possible";
		break;

	case IPP_TIMEOUT:
		errStr = "client-error-timeout";
		break;

	case IPP_NOT_FOUND:
		errStr = "client-error-not-found";
		break;

	case IPP_GONE:
		errStr = "client-error-gone";
		break;

	case IPP_REQUEST_ENTITY:
		errStr = "client-error-request-entity-too-large";
		break;

	case IPP_REQUEST_VALUE:
		errStr = "client-error-request-value-too-long";
		break;

	case IPP_DOCUMENT_FORMAT:
		errStr = "client-error-document-format-not-supported";
		break;

	case IPP_ATTRIBUTES:
		errStr = "client-error-attributes-or-values-not-supported";
		break;

	case IPP_URI_SCHEME:
		errStr = "client-error-uri-scheme-not-supported";
		break;

	case IPP_CHARSET:
		errStr = "client-error-charset-not-supported";
		break;

	case IPP_CONFLICT:
		errStr = "client-error-conflicting-attributes";
		break;

	case IPP_COMPRESSION_NOT_SUPPORTED:
		errStr = "client-error-compression-not-supported";
		break;

	case IPP_COMPRESSION_ERROR:
		errStr = "client-error-compression-error";
		break;

	case IPP_DOCUMENT_FORMAT_ERROR:
		errStr = "client-error-document-format-error";
		break;

	case IPP_DOCUMENT_ACCESS_ERROR:
		errStr = "client-error-document-access-error";
		break;

	case IPP_ATTRIBUTES_NOT_SETTABLE:
		errStr = "client-error-attributes-not-settable";
		break;

	case IPP_IGNORED_ALL_SUBSCRIPTIONS:
		errStr = "client-error-ignored-all-subscriptions";
		break;

	case IPP_TOO_MANY_SUBSCRIPTIONS:
		errStr = "client-error-too-many-subscriptions";
		break;

	case IPP_IGNORED_ALL_NOTIFICATIONS:
		errStr = "client-error-ignored-all-notifications";
		break;

	case IPP_PRINT_SUPPORT_FILE_NOT_FOUND:
		errStr = "client-error-print-support-file-not-found";
		break;

	case IPP_INTERNAL_ERROR:
		errStr = "server-error-internal-error";
		break;

	case IPP_OPERATION_NOT_SUPPORTED:
		errStr = "server-error-operation-not-supported";
		break;

	case IPP_SERVICE_UNAVAILABLE:
		errStr = "server-error-service-unavailable";
		break;

	case IPP_VERSION_NOT_SUPPORTED:
		errStr = "server-error-version-not-supported";
		break;

	case IPP_DEVICE_ERROR:
		errStr = "server-error-device-error";
		break;

	case IPP_TEMPORARY_ERROR:
		errStr = "server-error-temporary-error";
		break;

	case IPP_NOT_ACCEPTING:
		errStr = "server-error-not-accepting-jobs";
		break;

	case IPP_PRINTER_BUSY:
		errStr = "server-error-busy";
		break;

	case IPP_ERROR_JOB_CANCELED:
		errStr = "server-error-job-canceled";
		break;

	case IPP_MULTIPLE_JOBS_NOT_SUPPORTED:
		errStr = "server-error-multiple-document-jobs-not-supported";
		break;

	case IPP_PRINTER_IS_DEACTIVATED:
		errStr = "server-error-printer-is-deactivated";
		break;

	default:
		break;
	}

	return errStr;
}
