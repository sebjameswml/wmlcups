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

string
wml::CupsCtrl::getPrinterAttribute (const char* printerName,
                                    IppAttr& attr)
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

                        if (!strcmp(ipp_attributes->name, attr.getName().c_str()) &&
                            ipp_attributes->value_tag == attr.getType()) {
                                // Set the value, based on what kind
                                // of value this is. We COULD do
                                // attr.setValue(ipp_attributes->values[0]
                                // or somesuch. Then this logic would
                                // go into IppAttr
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
wml::CupsCtrl::setPrinterAttribute (const char* printerName,
                                    wml::IppAttr& attr)
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

        // Add Attribute
        switch (attr.getType()) {
        case IPP_TAG_ENUM:
        case IPP_TAG_INTEGER:
                ippAddInteger (prqst, IPP_TAG_PRINTER,
                               attr.getType(), attr.getName().c_str(),
                               attr.getInt());
                break;
        case IPP_TAG_BOOLEAN:
                ippAddBoolean (prqst, IPP_TAG_PRINTER,
                               attr.getName().c_str(), attr.getInt());
                break;
        default:
                ippAddString(prqst, IPP_TAG_PRINTER,
                             attr.getType(), attr.getName().c_str(),
                             NULL,
                             attr.getString().c_str());
                break;
        }

        rtn = cupsDoRequest (this->connection, prqst, "/admin/");

        if (!rtn) {
                // Handle error
                stringstream eee;
                eee << "CupsCtrl: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete (rtn);
        return;
}

void
wml::CupsCtrl::setPrinterAttributes (const char* printerName,
                                     wml::IppAttr& attr1,
                                     wml::IppAttr& attr2)
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

        // Add Attribute One
        switch (attr1.getType()) {
        case IPP_TAG_ENUM:
        case IPP_TAG_INTEGER:
                ippAddInteger (prqst, IPP_TAG_PRINTER,
                               attr1.getType(), attr1.getName().c_str(),
                               attr1.getInt());
                break;
        case IPP_TAG_BOOLEAN:
                ippAddBoolean (prqst, IPP_TAG_PRINTER,
                               attr1.getName().c_str(), attr1.getInt());
                break;
        default:
                ippAddString(prqst, IPP_TAG_PRINTER,
                             attr1.getType(), attr1.getName().c_str(),
                             NULL,
                             attr1.getString().c_str());
                break;
        }

        // Add Attribute Two
        switch (attr2.getType()) {
        case IPP_TAG_ENUM:
        case IPP_TAG_INTEGER:
                ippAddInteger (prqst, IPP_TAG_PRINTER,
                               attr2.getType(), attr2.getName().c_str(),
                               attr2.getInt());
                break;
        case IPP_TAG_BOOLEAN:
                ippAddBoolean (prqst, IPP_TAG_PRINTER,
                               attr2.getName().c_str(), attr2.getInt());
                break;
        default:
                ippAddString(prqst, IPP_TAG_PRINTER,
                             attr2.getType(), attr2.getName().c_str(),
                             NULL,
                             attr2.getString().c_str());
                break;
        }

        rtn = cupsDoRequest (this->connection, prqst, "/admin/");

        if (!rtn) {
                // Handle error
                stringstream eee;
                eee << "CupsCtrl: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete (rtn);
        return;
}

void
wml::CupsCtrl::sendPrinterCommand (const char* printerName,
                                   string asUser,
                                   string reason,
                                   ipp_op_t command,
                                   string directory)
{
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (command);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          directory.c_str(), printerName);

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
                eee << "CupsCtrl: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete (rtn);
        return;
}

vector<string>
wml::CupsCtrl::getPrinterList(void)
{
        return this->getCupsPrinterList (GET_PRINTERS_ONLY);
}

bool
wml::CupsCtrl::printerExists (string cupsPrinter)
{
        IppAttr attr("printer-name");
        getPrinterAttribute (cupsPrinter.c_str(), attr);
        if (attr.getString() == cupsPrinter) {
                return true;
        }
        return false;
}

void
wml::CupsCtrl::setInfo (string cupsPrinter, string s)
{
        IppAttr attr("printer-info");
        attr.setValue (s);
        this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::CupsCtrl::setLocation (string cupsPrinter, string s)
{
        IppAttr attr("printer-location");
        attr.setValue (s);
        this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::CupsCtrl::getMakeModel (string cupsPrinter)
{
        IppAttr attr("printer-make-and-model");
        return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::CupsCtrl::setMakeModel (string cupsPrinter, string s)
{
        throw runtime_error ("Not user settable");
        IppAttr attr("printer-make-and-model");
        attr.setValue (s);
        this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

string
wml::CupsCtrl::getDeviceURI (string cupsPrinter)
{
        IppAttr attr("device-uri");
        return this->getPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::CupsCtrl::setDeviceURI (string cupsPrinter, string s)
{
        DBG ("Called to set device uri to '" << s << "'");

        /*
         * To ensure that newly created IPP and IPPS queues get the
         * "shared" attribute set for them, we need to set the ppd
         * file when setting the device uri, or somehow ensure that
         * it's set. If not, the CUPS says "This is a remote printer,
         * and there's no filter yet, so I will force sharing off for
         * security reasons".
         */

        string curPpd("");
        try {
                curPpd = this->getPPDNickname (cupsPrinter);
        } catch (const exception& e) {
                curPpd = "Unknown";
        }

        if (curPpd == "Unknown") {
                DBG ("Initialising PPD to default");
                try {
                        setPPDFromFile (cupsPrinter, "/usr/share/cups/model/pdf.ppd");
                } catch (const exception& e) {
                        stringstream ee;
                        ee << "Failed to initialise PPD for Queue " << cupsPrinter
                           << " error: "  << e.what();
                        DBG (ee.str());
                }
        }

        /*
         * Now set the device URI.
         */

        IppAttr attr("device-uri");
        attr.setValue (s);
        this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::CupsCtrl::addPrinter (string cupsPrinter, string devURI)
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
wml::CupsCtrl::addPrinter (string cupsPrinter, string devURI, string sourcePPD)
{
        DBG ("Called");
        try {
                this->getDeviceURI(cupsPrinter);
                // If we get here without exception, printer exists,
                // but we'll set the DeviceURI anyway:
                DBG ("Printer already exists, setting device uri and PPD anyway");
                this->setDeviceURI (cupsPrinter, devURI);
                this->setPPD (cupsPrinter, sourcePPD);

        } catch (const exception& e ) {

                string ee = e.what();
                if (ee == "Unknown") {
                        this->setDeviceURI (cupsPrinter, devURI);
                        this->setPPD (cupsPrinter, sourcePPD);
                } else {
                        // Re-throw other exceptions
                        throw e;
                }
        }
}

void
wml::CupsCtrl::deletePrinter (string cupsPrinter)
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
                eee << "CupsCtrl: cupsDoRequest() failed in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete (rtn);
        return;
}
