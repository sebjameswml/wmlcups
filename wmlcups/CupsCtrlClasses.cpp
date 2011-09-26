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


void
wml::CupsCtrl::setClassInfo (string cupsClass, string s)
{
        IppAttr attr("printer-info");
        attr.setValue (s);
        this->setClassAttribute (cupsClass.c_str(), attr);
}


void
wml::CupsCtrl::setClassLocation (string cupsClass, string s)
{
        IppAttr attr("printer-location");
        attr.setValue (s);
        this->setClassAttribute (cupsClass.c_str(), attr);
}


vector<string>
wml::CupsCtrl::getMemberUris (string cupsPrinter)
{
        DBG ("Called");
        IppAttr attr("member-uris");
        return this->getQueueAttribute (cupsPrinter.c_str(), attr);
}


vector<string>
wml::CupsCtrl::getClassesList(void)
{
        return this->getCupsPrinterList (GET_CLASSES);
}


vector<string>
wml::CupsCtrl::getQueueAttribute (const char* printerName,
                                  IppAttr& attr)
{
        bool gotPrinter = false;
        ipp_t * prqst;
        ipp_t * rtn;
        char * attrTag;

        vector<string> results;

        string uri = printerName;

        uri = "ipp://" + this->getCupsdAddress() + "/classes/" + uri;

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

        prqst = ippNewRequest (IPP_GET_PRINTER_ATTRIBUTES);

        ippAddString(prqst,
                     IPP_TAG_OPERATION,
                     IPP_TAG_URI,
                     "printer-uri",
                     NULL,
                     uri.c_str());

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
                                printer = ipp_attributes->values[0].string.text;
                        }

                        if (!strcmp(ipp_attributes->name, attr.getName().c_str())) {
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

                                        for (int i = 0; i < ipp_attributes->num_values; i++) {
                                                attr.setValue (ipp_attributes->values[i].string.text);
                                                results.push_back(attr.getString());
                                        }
                                        break;
                                case IPP_TAG_ENUM:
                                case IPP_TAG_INTEGER:
                                        for (int i = 0; i < ipp_attributes->num_values; i++) {
                                                attr.setValue (ipp_attributes->values[i].integer);
                                                results.push_back(attr.getString());
                                        }
                                        break;
                                case IPP_TAG_BOOLEAN:
                                        for (int i = 0; i < ipp_attributes->num_values; i++) {
                                                attr.setValue (ipp_attributes->values[i].boolean);
                                                results.push_back(attr.getString());
                                        }
                                        break;
                                default:
                                        // Can't set value from this type
                                        break;
                                }
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
                //free (*attr);
        }

        if (!gotPrinter) {
                // We didn't find the printer, so zero attr
                attr.zero();
        }

        return results;
}

void
wml::CupsCtrl::setClassAttribute (const char* printerName,
                                  wml::IppAttr& attr)
{
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (CUPS_ADD_CLASS);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/classes/%s", printerName);

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
}

void
wml::CupsCtrl::setClassMembers (string className,
                                vector<string> members)
{
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        ipp_attribute_t * ipp_attributes;

        prqst = ippNewRequest (CUPS_ADD_CLASS);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/classes/%s", className.c_str());

        ippAddString (prqst, IPP_TAG_OPERATION, IPP_TAG_URI,
                      "printer-uri", NULL, uri);

        vector<string>::iterator iter;

        ipp_attributes = ippAddStrings(prqst, IPP_TAG_PRINTER, IPP_TAG_URI, "member-uris",
                                       members.size(), NULL, NULL);

        int i = 0;

        for (iter = members.begin(); iter != members.end(); iter++) {
                ipp_attributes->values[i].string.text = strdup(iter->c_str());
                i++;
        }

        rtn = cupsDoRequest (this->connection, prqst, "/admin/");

        if (rtn == (ipp_t*)0) {
                stringstream ee;
                ee << "cupsDoRequest failed to set " << members.size()
                   << " members for the class " << className;
                throw runtime_error (ee.str());
        }

        ippDelete (rtn);
}
