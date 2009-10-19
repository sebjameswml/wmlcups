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
