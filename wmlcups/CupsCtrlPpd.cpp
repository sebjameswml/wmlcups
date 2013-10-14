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

#include <futil/futil.h>
#include "QueueCupsStatus.h"
#include "IppAttr.h"
#include "CupsCtrl.h"
#include "Ppd.h"

using namespace std;
using namespace wml;

void
wml::CupsCtrl::setPPD (const string& cupsPrinter, const string& ppdTag)
{
        // This is one way, if sourcePPD specifies a "ppd descriptor".
        IppAttr attr("ppd-name");
        attr.setValue (ppdTag);
        this->setPrinterAttribute (cupsPrinter.c_str(), attr);
}

void
wml::CupsCtrl::setPPDFromFile (const string& cupsPrinter, const string& sourcePPD)
{
        if (!futil::fileExists(sourcePPD)) {
                stringstream ee;
                ee << "The file " << sourcePPD << " doesn't exist";
                throw runtime_error (ee.str());
        }

        // If we provide a filename, then do it like this:
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (CUPS_ADD_PRINTER);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/printers/%s", cupsPrinter.c_str());

        ippAddString(prqst,
                     IPP_TAG_OPERATION, IPP_TAG_URI,
                     "printer-uri", NULL, uri);

        rtn = cupsDoFileRequest (this->connection,
                                 prqst,
                                 "/admin/",
                                 sourcePPD.c_str());
        if (!rtn) {
                // Handle error
                stringstream eee;
                eee << "CupsCtrl: cupsDoFileRequest() failed in "
                    << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoFileRequest() conflict in "
                    << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete(rtn);
}

void
wml::CupsCtrl::getPPDToFile (const string& cupsPrinter, const string& file) const
{
        if (cupsPrinter.empty()) {
                throw runtime_error ("Must specify printer.");
        }

        ofstream f (file.c_str(), ios::out|ios::trunc);
        if (!f.is_open()) {
                throw runtime_error ("Couldn't open file");
        } else {
                f.close();
        }

        string uri = "/printers/" + cupsPrinter + ".ppd";
        http_status_t rtn = cupsGetFile (this->connection,
                                         uri.c_str(),
                                         file.c_str());

        if (rtn != HTTP_OK) {
                stringstream ee;
                int theErr = errno;
                ee << "Error '" << theErr << "' getting '" << uri << "' into file '"
                   << file << "': " << this->errorString (rtn);
                throw runtime_error (ee.str());
        }

        return;
}

wml::Ppd
wml::CupsCtrl::getPPD (const string& cupsPrinter) const
{
        // This gets an INCOMPLETE set of info about the PPD.

        Ppd p; // For return.

        if (cupsPrinter.empty()) {
                throw runtime_error ("Must specify printer.");
        }

        string ppdFile = cupsPrinter + ".ppd";
        string tmpFilePath("/tmp/ppd_");
        tmpFilePath += ppdFile;
        string uri("/printers/");
        uri += ppdFile;

        // Have to create tmpFilePath first:
        ofstream f (tmpFilePath.c_str(), ios::out|ios::trunc);
        if (!f.is_open()) {
                throw runtime_error ("Couldn't open file");
        } else {
                f.close();
        }

        http_status_t rtn = cupsGetFile (this->connection,
                                         uri.c_str(),
                                         tmpFilePath.c_str());

        if (rtn != HTTP_OK) {
                stringstream ee;
                int theErr = errno;
                ee << "Error '" << theErr << "' getting '" << uri << "' into file '"
                   << tmpFilePath << "': " << this->errorString (rtn);
                throw runtime_error (ee.str());
        }

        ppd_file_t* ppd = ppdOpenFile (tmpFilePath.c_str());
        if (ppd == (ppd_file_t*)0) {
                throw runtime_error ("Error opening tmpFilePath...");
        }

        stringstream ss;
        if (ppd->nickname != (char*)0) {
                p.setMakeAndModel (ppd->nickname); // Right
        }
        if (ppd->modelname != (char*)0) {
                //p.setName (ppd->modelname); // Wrong - this is nickname, but without the ", version"
        }
        if (ppd->manufacturer != (char*)0) {
                p.setMake (ppd->manufacturer); // Right
        }
        if (ppd->product != (char*)0) {
                p.setProduct (ppd->product); // Right
        }
        ppdClose (ppd);

        unlink (tmpFilePath.c_str());

        return p;
}

string
wml::CupsCtrl::getPPDNickname (const string& cupsPrinter) const
{
        if (cupsPrinter.empty()) {
                throw runtime_error ("Must specify printer.");
        }

        string ppdFile = cupsPrinter + ".ppd";
        string tmpFilePath("/tmp/ppd_");
        tmpFilePath += ppdFile;
        string uri("/printers/");
        uri += ppdFile;

        // Have to create tmpFilePath first:
        ofstream f (tmpFilePath.c_str(), ios::out|ios::trunc);
        if (!f.is_open()) {
                throw runtime_error ("Couldn't open file");
        } else {
                f.close();
        }

        http_status_t rtn = cupsGetFile (this->connection,
                                         uri.c_str(),
                                         tmpFilePath.c_str());

        if (rtn != HTTP_OK) {
                stringstream ee;
                int theErr = errno;
                ee << "Error '" << theErr << "' getting '" << uri << "' into file '"
                   << tmpFilePath << "': " << this->errorString (rtn);
                throw runtime_error (ee.str());
        }

        ppd_file_t* ppd = ppdOpenFile (tmpFilePath.c_str());
        if (ppd == (ppd_file_t*)0) {
                throw runtime_error ("Error opening tmpFilePath...");
        }

        stringstream ss;
        if (ppd->nickname != (char*)0) {
                ss << ppd->nickname;
        } else {
                ss << "Unknown";
        }
        ppdClose (ppd);

        unlink (tmpFilePath.c_str());

        return ss.str();
}

vector<string>
wml::CupsCtrl::getPPDListOfMakes (void) const
{
        vector<string> theList;

        ipp_attribute_t * ipp_attributes;
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (CUPS_GET_PPDS);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/printers/");

        ippAddString (prqst,
                      IPP_TAG_OPERATION,
                      IPP_TAG_URI,
                      "printer-uri",
                      NULL,
                      uri);

        ippAddString (prqst,
                      IPP_TAG_OPERATION,
                      IPP_TAG_KEYWORD,
                      "requested-attributes",
                      NULL,
                      "ppd-make");

        rtn = cupsDoRequest (this->connection, prqst, "/");

        if (!rtn) {
                // Handle error
                stringstream eee;
                eee << "CupsCtrl: cupsDoFileRequest() failed in "
                    << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoFileRequest() conflict in "
                    << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
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

                        if (!strcmp(ipp_attributes->name, "ppd-make") &&
                            ipp_attributes->value_tag == IPP_TAG_TEXT) {
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

        return theList;
}

vector<Ppd>
wml::CupsCtrl::getPPDListOfModels (const string& make) const
{
        vector<Ppd> theList;

        ipp_attribute_t * ipp_attributes;
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (CUPS_GET_PPDS);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/printers/");

        ippAddString (prqst,
                      IPP_TAG_OPERATION,
                      IPP_TAG_URI,
                      "printer-uri",
                      NULL,
                      uri);

        ippAddString (prqst,
                      IPP_TAG_OPERATION,
                      IPP_TAG_TEXT,
                      "ppd-make",
                      NULL,
                      make.c_str());

        rtn = cupsDoRequest (this->connection, prqst, "/");

        if (!rtn) {
                // Handle error
                stringstream eee;
                eee << "CupsCtrl: cupsDoFileRequest() failed in "
                    << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                throw runtime_error (eee.str());
        }
        if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                // Handle conflict
                stringstream eee;
                eee << "CupsCtrl: cupsDoFileRequest() conflict in "
                    << __FUNCTION__ << ". Error 0x"
                    << hex << cupsLastError() << " ("
                    << this->errorString (cupsLastError()) << ")";
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        for (ipp_attributes = rtn->attrs;
             ipp_attributes != NULL;
             ipp_attributes = ipp_attributes->next) {

                while (ipp_attributes != NULL
                       && ipp_attributes->group_tag != IPP_TAG_PRINTER) {
                        // Move on to the next one.
                        ipp_attributes = ipp_attributes->next;
                }

                Ppd ppd;

                while (ipp_attributes != NULL &&
                       ipp_attributes->group_tag == IPP_TAG_PRINTER) {

                        /*
                          Example text fields which this returns:

                          'ppd-name'           lsb/usr/HP/hp-psc_2400_series-hpijs.ppd.gz
                          'ppd-make'           HP
                          'ppd-make-and-model' HP PSC 2400 Series hpijs, 3.9.10.72
                          'ppd-device-id'      MFG:HP;MDL:psc 2400 series;DES:psc 2400 series;
                          'ppd-product'        HP PSC 2405 Photosmart All-in-one Printer
                          'ppd-psversion'      (3010.000) 705

                          Non-TEXT/NAME fields returned are: ppd-type,
                          ppd-model-number, ppd-natural-language

                        */

                        if (!strcmp(ipp_attributes->name, "ppd-make-and-model") &&
                            ipp_attributes->value_tag == IPP_TAG_TEXT) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setMakeAndModel (p);
                                }
                        }

                        // For selecting the printers, we need the ppd-name.
                        else if (!strcmp(ipp_attributes->name, "ppd-name") &&
                                 ipp_attributes->value_tag == IPP_TAG_NAME) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setName (p);
                                }
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-make") &&
                                 ipp_attributes->value_tag == IPP_TAG_TEXT) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setMake (p);
                                }
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-device-id") &&
                                 ipp_attributes->value_tag == IPP_TAG_TEXT) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setDeviceId (p);
                                }
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-product") &&
                                 ipp_attributes->value_tag == IPP_TAG_TEXT) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setProduct (p);
                                }
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-psversion") &&
                                 ipp_attributes->value_tag == IPP_TAG_TEXT) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setPsversion (p);
                                }
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-type") &&
                                 ipp_attributes->value_tag == IPP_TAG_INTEGER) {
                                ppd.setType (ipp_attributes->values[0].integer);
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-model-number") &&
                                 ipp_attributes->value_tag == IPP_TAG_INTEGER) {
                                ppd.setModelNumber (ipp_attributes->values[0].integer);
                        }

                        else if (!strcmp(ipp_attributes->name, "ppd-natural-language") &&
                                 ipp_attributes->value_tag == IPP_TAG_LANGUAGE) {
                                string p(ipp_attributes->values[0].string.text);
                                if (!p.empty()) {
                                        ppd.setNaturalLanguage (p);
                                }
                        }

                        ipp_attributes = ipp_attributes->next;
                }

                if (!ppd.getMakeAndModel().empty()) {
                        theList.push_back (ppd);
                }

                if (ipp_attributes == NULL) {
                        break;
                }

        } // end of for each ipp response

        return theList;
}

vector<PpdOption>
wml::CupsCtrl::getPPDOptions (const std::string& cupsPrinter) const
{
        if (cupsPrinter.empty()) {
                throw runtime_error ("Must specify printer.");
        }

        /*!
         * First job, get the user-specified options - lpoptions,
         * which come via cups api calls from /etc/cups/lpoptions.
         */
        cups_dest_t * dests; // All destinations
        cups_dest_t * d;     // One destination

        int ndests = cupsGetDests (&dests);
        DBG ("After first cupsGetDests(), we have " << ndests << " destinations");

        d = cupsGetDest (cupsPrinter.c_str(), (const char *)0, ndests, dests);
        if (d == (cups_dest_t*)0) {
                DBG ("Adding dest...");
                ndests = cupsAddDest (cupsPrinter.c_str(), (const char *)0, 0, &dests);
                d = cupsGetDest (cupsPrinter.c_str(), (const char *)0, ndests, dests);
        }

        if (d == (cups_dest_t*)0) {
                throw runtime_error ("Queue does not exist (dest is NULL).");
        }

        int k = 0;
        set<string> destOptions;
        while (k < d->num_options) {
                //string key(d->options[i].name);
                //destOptions.insert (key);
                destOptions.insert (d->options[k].name);
                k++;
        }

        /*!
         * Now we have the user options, we get all the options from
         * the PPD, overriding the default option selection with the
         * user option selection where necessary.
         */
        vector<PpdOption> theOptions;

        string ppdFile = cupsPrinter + ".ppd";
        string tmpFilePath("/tmp/ppd_");
        tmpFilePath += ppdFile;
        string uri("/printers/");
        uri += ppdFile;

        // Have to create tmpFilePath first:
        ofstream f (tmpFilePath.c_str(), ios::out|ios::trunc);
        if (!f.is_open()) {
                throw runtime_error ("Couldn't open file");
        } else {
                f.close();
        }

        http_status_t rtn = cupsGetFile (this->connection,
                                         uri.c_str(),
                                         tmpFilePath.c_str());

        if (rtn != HTTP_OK) {
                stringstream ee;
                int theErr = errno;
                ee << "Error '" << theErr << "' getting '" << uri << "' into file '"
                   << tmpFilePath << "': " << this->errorString (rtn);
                throw runtime_error (ee.str());
        }
        // ippDelete (rtn); ??

        ppd_file_t* ppd = ppdOpenFile (tmpFilePath.c_str());

        ppdMarkDefaults (ppd);

        if (ppd == NULL) {
                // No options.
                return theOptions;
        }

        unsigned int i, j;
        ppd_group_t *group;
        ppd_option_t *option;
        string optionList;
        string valueList;

        // Loop through option groups
        for (i=ppd->num_groups, group=ppd->groups; i>0; i--, group++) {
                // Loop through options in current group
                for (j=group->num_options, option=group->options; j>0; j--, option++) {
                        PpdOption o (option);
                        o.setGroupName (group->text);
                        // Check if user has over-ridden the PPD:
                        if (destOptions.count (o.getKeyword()) > 0) {
                                try {
                                        DBG ("Setting user-set option " << o.getKeyword());
                                        o.setChoiceValue (cupsGetOption (o.getKeyword().c_str(),
                                                                         d->num_options,
                                                                         d->options));
                                        DBG ("After setting, the option value is "
                                             << o.getChoiceValue());

                                } catch (const exception& e) {
                                        DBG ("Failed to set user choice in ppd.");
                                }
                        }
                        theOptions.push_back (o);
                }
        }

        // Close the ppd file
        ppdClose (ppd);
        // Done with dests now
        cupsFreeDests (ndests, dests);

        return theOptions;
}

void
wml::CupsCtrl::setPPDOption (const std::string& cupsPrinter,
                             const std::string& keyword,
                             const std::string& value)
{
        if (this->cupsdAddress != "localhost"
            && this->cupsdAddress != "127.0.0.1") {
                throw runtime_error ("You can't set PPD options on a different "
                                     "server, unfortunately.");
        }

        // For getting dests from cupsPrinter.
        cups_dest_t * dests = (cups_dest_t*)0;     // All destinations
        cups_dest_t * d = (cups_dest_t*)0;         // One destination
        // For getting options from cupsPrinter.
        int nOptions = 0;
        cups_option_t * options = (cups_option_t*)0;

        int ndests = cupsGetDests (&dests);

#ifdef DEBUG
        if (ndests > 0) {
                DBG ("number of options in dests[0] is " << dests[0].num_options);
        }
#endif

        d = cupsGetDest (cupsPrinter.c_str(), (const char *)0, ndests, dests);
        if (d == (cups_dest_t*)0) {
                ndests = cupsAddDest (cupsPrinter.c_str(), (const char *)0, 0, &dests);
                d = cupsGetDest (cupsPrinter.c_str(), (const char *)0, ndests, dests);
        }

        if (d == (cups_dest_t*)0) {
                throw runtime_error ("Queue does not exist (dest is NULL).");
        }

        int i = 0;
#ifdef DEBUG
        // Just debugging really
        while (i < d->num_options) {
                string key(d->options[i].name);
                if (key == keyword) {
                        DBG ("the destination has a matching key: " << key);
                }
                i++;
        }
#endif

        // Add the options for the destination to the options list:
        for (i = 0; i < d->num_options; i++) {
                if (cupsGetOption(d->options[i].name, nOptions, options) == NULL) {
                        nOptions = cupsAddOption (d->options[i].name,
                                                  d->options[i].value,
                                                  nOptions, &options);
                }
        }

        // Add the new one (replaces existing ppd option if necessary)
        string newOpt = keyword + "=" + value;
        nOptions = cupsParseOptions (newOpt.c_str(), nOptions, &options);

        // Replace the options in the dest with the new options
        cupsFreeOptions (d->num_options, d->options);
        d->num_options = nOptions;
        d->options = options;
        cupsSetDests (ndests, dests);

        // save the dest with this->writeLpOptions, which is similar
        // to cupsSetDests2(). Important - you have to save all the
        // options here, as this function empties /etc/cups/lpoptions
        // and then writes it out afresh.
        this->writeLpOptions (ndests, dests);

        cupsFreeDests (ndests, dests);
}

void
wml::CupsCtrl::writeLpOptions (const int ndests, cups_dest_t * dests) const
{
        DBG ("Called");

        // Re-write /etc/cups/lpoptions
        ofstream f;
        f.open ("/etc/cups/lpoptions", ios::out|ios::trunc);
        if (!f.is_open()) {
                DBG ("Failed to open lpoptions for write!");
                cupsFreeDests (ndests, dests);
                throw runtime_error ("writeLpOptions: Failed to open /etc/cups/lpoptions for write");
        }

        cups_dest_t* dest;
        cups_option_t * option;
        const char        *val;

        int i, j, didHeading = 0;

        DBG ("Main loop");
        for (i = ndests, dest = dests; i > 0; i--, dest++) {

                DBG ("Doing dest " << dest->name);
                if (dest->instance != (char*)0 || dest->num_options != 0 || dest->is_default) {
                        DBG ("Stil doing dest " << dest->name);
                        if (dest->is_default) {
                                f << "Default " <<  dest->name;
                                if (dest->instance != (char*)0) {
                                        f << "/" << dest->instance;
                                }
                                didHeading = 1;
                        } else {
                                didHeading = 0;
                        }

                        DBG ("Options loop");
                        for (j = dest->num_options, option = dest->options; j > 0; j--, option++) {

                                DBG ("Attribute test...");

                                // Skip printer and job attributes
                                IppAttr at(option->name);
                                ipp_tag_t tg = at.getGroup();
                                if (tg == IPP_TAG_PRINTER || tg == IPP_TAG_JOB) {
                                        DBG (option->name << " is a printer or job attribute, skip it.");
                                        continue;
                                }

                                DBG ("Got past attribute test");

                                // Write this option to the file:
                                if (!didHeading) {
                                        f << "Dest " << dest->name;
                                        if (dest->instance != (char*)0) {
                                                f << "/" << dest->instance;
                                        }
                                        didHeading = 1;
                                }

                                if (option->value[0] != '\0') {
                                        if (strchr(option->value, ' ') ||
                                            strchr(option->value, '\\') ||
                                            strchr(option->value, '\"') ||
                                            strchr(option->value, '\''))
                                        {
                                                // Add quotes to value
                                                f << " " << option->name << "=\"";
                                                for (val = option->value; *val; val++) {
                                                        if (strchr("\"\'\\", *val)) {
                                                                f << '\\';
                                                        }
                                                        f << *val;
                                                }
                                                f << '\"';

                                        } else {
                                                // The value without quotes
                                                f << " " << option->name << "=" << option->value;
                                        }
                                } else {
                                        f << " " << option->name;
                                }
                        }

                        if (didHeading) { f << endl; }
                }
        }

        f.close();

        DBG ("Returning");
}
