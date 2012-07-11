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
wml::CupsCtrl::sendJobCommand (const int jobId,
                               const string& user,
                               const ipp_op_t command) const
{
        DBG ("Called to send command for job id " << jobId
             << " as user '" << user << "'");
        string asUser (user);

        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (command);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/jobs/%d", jobId);

        ippAddString(prqst,
                     IPP_TAG_OPERATION, IPP_TAG_URI,
                     "job-uri", NULL, uri);

        if (asUser.empty()) {
                DBG ("Setting user to \"guest\"");
                asUser = "guest";
        }
        ippAddString(prqst,
                     IPP_TAG_OPERATION, IPP_TAG_NAME,
                     "requesting-user-name", NULL, asUser.c_str());

        rtn = cupsDoRequest (this->connection, prqst, "/jobs");

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
                if (cupsLastError() == IPP_NOT_AUTHORIZED) {
                        eee << "CupsCtrl: Job action not authorized for user " << asUser
                            << ". (Error was " << this->errorString (cupsLastError()) << ")";
                } else {
                        eee << "CupsCtrl: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
                            << hex << cupsLastError() << " ("
                            << this->errorString (cupsLastError()) << " - "
                            << cupsLastErrorString() << ")";
                }
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete (rtn);
        return;
}

void
wml::CupsCtrl::moveJob (const int jobId,
                        const string& user,
                        const string& destQueue) const
{
        DBG ("WARNING, UNTESTED! Called to move job id " << jobId
             << " as user '" << user
             << "' to queue '" << destQueue << "'");
        string asUser (user);

        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];
        char desturi[HTTP_MAX_URI];

        prqst = ippNewRequest (CUPS_MOVE_JOB);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/jobs/%d", jobId);

        ippAddString (prqst,
                      IPP_TAG_OPERATION, IPP_TAG_URI,
                      "job-uri", NULL, uri);

        if (asUser.empty()) {
                DBG ("Setting user to \"guest\"");
                asUser = "guest";
        }
        ippAddString (prqst,
                      IPP_TAG_OPERATION, IPP_TAG_NAME,
                      "requesting-user-name", NULL, asUser.c_str());

        // Build the destination uri for the job to be moved to
        httpAssembleURIf (HTTP_URI_CODING_ALL, desturi, sizeof(desturi), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/printers/%s", destQueue.c_str());
        ippAddString (prqst,
                      IPP_TAG_JOB, IPP_TAG_URI,
                      "job-printer-uri", NULL, desturi);

        rtn = cupsDoRequest (this->connection, prqst, "/jobs");

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
                if (cupsLastError() == IPP_NOT_AUTHORIZED) {
                        eee << "CupsCtrl: Job action not authorized for user " << asUser
                            << ". (Error was " << this->errorString (cupsLastError()) << ")";
                } else {
                        eee << "CupsCtrl: cupsDoRequest() conflict in " << __FUNCTION__ << ". Error 0x"
                            << hex << cupsLastError() << " ("
                            << this->errorString (cupsLastError()) << " - "
                            << cupsLastErrorString() << ")";
                }
                ippDelete (rtn);
                throw runtime_error (eee.str());
        }

        ippDelete (rtn);
        return;
}

void
wml::CupsCtrl::cancelJobs (const string& printerName) const
{
        vector<CupsJob> jobs;
        vector<CupsJob>::iterator iter;

        this->getJobList(printerName, jobs, "");

        for (iter = jobs.begin(); iter != jobs.end(); iter++) {
                // User here may need to be "lp"
                this->sendJobCommand (iter->getId(), "default", IPP_CANCEL_JOB);
        }

        return;
}

void
wml::CupsCtrl::pauseJobs (const string& printerName) const
{
        vector<CupsJob> jobs;
        vector<CupsJob>::iterator iter;

        this->getJobList(printerName, jobs, "");

        for (iter = jobs.begin(); iter != jobs.end(); iter++) {
                // User here may need to be "lp"
                this->sendJobCommand (iter->getId(), "default", IPP_HOLD_JOB);
        }

        return;
}

void
wml::CupsCtrl::releaseJobs (const string& printerName) const
{
        vector<CupsJob> jobs;
        vector<CupsJob>::iterator iter;

        this->getJobList(printerName, jobs, "");

        for (iter = jobs.begin(); iter != jobs.end(); iter++) {
                // User here may need to be "lp"
                this->sendJobCommand (iter->getId(), "default", IPP_RELEASE_JOB);
        }

        return;
}

void
wml::CupsCtrl::getJobStatus (const string& cupsPrinter, const int id, CupsJob& j) const
{
        if (j.getId()>0) {
                j.reset();
        }
        vector<CupsJob> jList;
        if (id == 0) {
                // Although we had to get all the blinking job information
                // from cupsd, just copy the information from the last one:
                DBG ("Requested ID is 0, so call getJobList() to get last job information:");
                this->getJobList (cupsPrinter, jList, 1, "all");
                if (!jList.empty()) {
                        j = jList.front();
                }
        } else {
                DBG ("Call getJobList() to get all job information and compare with id:");
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

CupsJob
wml::CupsCtrl::getJob (const string& id) const
{
        DBG ("Called");

        ipp_t * jrqst;
        ipp_t * rtn;
        ipp_attribute_t * ipp_attributes;
        char uri[HTTP_MAX_URI];

        jrqst = ippNewRequest (IPP_GET_JOB_ATTRIBUTES);

        if (!id.empty()) {

                stringstream jobss;
                int job;

                jobss << id;
                jobss >> job;

                httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                                  NULL, this->cupsdAddress.c_str(), 0,
                                  "/jobs/%s", id.c_str());

                ippAddString (jrqst,
                              IPP_TAG_OPERATION,
                              IPP_TAG_URI,
                              "job-uri",
                              NULL,
                              uri);
        }

        rtn = cupsDoRequest (this->connection, jrqst, "/");

        if (!rtn) {
                throw runtime_error ("Request failed");
        }

        vector<ipp_attribute_t*> attrList;
        for (ipp_attributes = rtn->attrs;
             ipp_attributes != NULL;
             ipp_attributes = ipp_attributes->next) {
                attrList.push_back (ipp_attributes);
        }

        vector<ipp_attribute_t*>::iterator i = attrList.begin();

        CupsJob j;

        while (i != attrList.end()) {

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
                        j.setCreationTime ((*i)->values[0].integer);
                }
                if (!strcmp((*i)->name, "time-at-processing") &&
                    (*i)->value_tag == IPP_TAG_INTEGER) {
                        j.setProcessingTime ((*i)->values[0].integer);
                }
                if (!strcmp((*i)->name, "time-at-completed") &&
                    (*i)->value_tag == IPP_TAG_INTEGER) {
                        j.setCompletedTime ((*i)->values[0].integer);
                }

                i++;
        }

        return j;
}

void
wml::CupsCtrl::getJobList (const string& cupsPrinter,
                           vector<CupsJob>& jList,
                           const string& whichJobs) const
{
        this->getJobList (cupsPrinter, jList, 0, whichJobs);
}

/*
 * Tried two alternative implementations of getJobList, but couldn't
 * make it run any faster than 0.2 seconds to get the last job info
 * out of about 450 jobs on a queue. CUPS really needs a way to walk
 * backward through the list of jobs.
 */
void
wml::CupsCtrl::getJobList (const string& cupsPrinter,
                           vector<CupsJob>& jList,
                           const int numJobs,
                           const string& whichJobs) const
{
        DBG ("Called to list up to " << numJobs << " jobs");

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
                DBG ("Adding which-jobs as '" << whichJobs << "'");
                ippAddString (jrqst,
                              IPP_TAG_OPERATION,
                              IPP_TAG_KEYWORD,
                              "which-jobs",
                              NULL,
                              whichJobs.c_str());
        }

        // We have to specify the job attributes we are interested in
        const char * jobAttributes[10];
        jobAttributes[0] = "job-id";
        jobAttributes[1] = "copies";
        jobAttributes[2] = "job-k-octets";
        jobAttributes[3] = "job-name";
        jobAttributes[4] = "job-originating-user-name";
        jobAttributes[5] = "job-printer-uri";
        jobAttributes[6] = "job-state";
        jobAttributes[7] = "time-at-creation";
        jobAttributes[8] = "time-at-processing";
        jobAttributes[9] = "time-at-completed";
        int n_attributes = 10;
        ippAddStrings(jrqst,
                      IPP_TAG_OPERATION,
                      IPP_TAG_KEYWORD,
                      "requested-attributes",
                      n_attributes,
                      NULL,
                      jobAttributes);

        rtn = cupsDoRequest (this->connection, jrqst, "/");

        if (!rtn) {
                // Handle error
                throw runtime_error ("CupsCtrl: cupsDoRequest() failed");
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
                if (numJobs != 0 && count > numJobs) {
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
                                DBG ("time-at-creation is " << (*i)->values[0].integer);
                                j.setCreationTime ((*i)->values[0].integer);
                        }
                        if (!strcmp((*i)->name, "time-at-processing") &&
                            (*i)->value_tag == IPP_TAG_INTEGER) {
                                DBG ("time-at-processing is " << (*i)->values[0].integer);
                                j.setProcessingTime ((*i)->values[0].integer);
                        }
                        if (!strcmp((*i)->name, "time-at-completed") &&
                            (*i)->value_tag == IPP_TAG_INTEGER) {
                                DBG ("time-at-completed is " << (*i)->values[0].integer);
                                j.setCompletedTime ((*i)->values[0].integer);
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

int
wml::CupsCtrl::createJob (const string& cupsQueue,
                          const string& title,
                          const string& asUser,
                          const string& docName,
                          const int numOptions,
                          cups_option_t* options) const
{
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (IPP_CREATE_JOB);

        if (!asUser.empty()) {
                ippAddString(prqst,
                             IPP_TAG_OPERATION, IPP_TAG_NAME,
                             "requesting-user-name", NULL, asUser.c_str());
        }

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/printers/%s", cupsQueue.c_str());
        ippAddString (prqst, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri",
                      NULL, uri);

        if (!title.empty()) {
                ippAddString (prqst, IPP_TAG_OPERATION, IPP_TAG_NAME,
                              "job-name", NULL, title.c_str());
        }
        if (!docName.empty()) {
                ippAddString (prqst, IPP_TAG_OPERATION, IPP_TAG_NAME,
                              "document-name", NULL, docName.c_str());
        }

        cupsEncodeOptions (prqst, numOptions, options);

        string dest = "/printers/"+cupsQueue;
        rtn = cupsDoRequest (this->connection, prqst, dest.c_str());

        ipp_attribute_t * attr;                /* IPP job-id attribute */
        int jobid = 0;
        if (rtn == NULL) {
                stringstream ee;
                ee << "Unable to create job (queue " << cupsQueue << "), null "
                   << "response to cupsDoRequest, cupsLastError() returns "
                   << this->errorString (cupsLastError());
                throw runtime_error (ee.str());
                jobid = 0;

        } else if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                stringstream ee;
                ee << "Unable to create job (queue " << cupsQueue << "), "
                   << "rtn->request.status.status_code = "
                   << this->errorString (rtn->request.status.status_code);
                throw runtime_error (ee.str());
                jobid = 0;

        } else if ((attr = ippFindAttribute(rtn, "job-id", IPP_TAG_INTEGER)) == NULL) {
                throw runtime_error ("No job-id attribute found in response from server!");
                jobid = 0;

        } else {
                // cupsd successfully allocated a job
                jobid = attr->values[0].integer;
        }

        if (rtn != NULL) {
                ippDelete (rtn);
        }

        return jobid;
}

void
wml::CupsCtrl::holdJob (const int jobId, const string& asUser) const
{
        this->sendJobCommand (jobId, asUser, IPP_HOLD_JOB);
}

void
wml::CupsCtrl::releaseJob (const int jobId, const string& asUser) const
{
        this->sendJobCommand (jobId, asUser, IPP_RELEASE_JOB);
}

void
wml::CupsCtrl::cancelJob (const int jobId, const string& asUser) const
{
        this->sendJobCommand (jobId, asUser, IPP_CANCEL_JOB);
}

void
wml::CupsCtrl::restartJob (const int jobId, const string& asUser) const
{
        this->sendJobCommand (jobId, asUser, IPP_RESTART_JOB);
}

void
wml::CupsCtrl::sendDocument (const int jobId,
                             const string& filePath,
                             const string& asUser,
                             const string& docName,
                             const string& format,
                             const bool lastInSet) const
{
        ipp_t * prqst;
        ipp_t * rtn;
        char uri[HTTP_MAX_URI];

        prqst = ippNewRequest (IPP_SEND_DOCUMENT);

        httpAssembleURIf (HTTP_URI_CODING_ALL, uri, sizeof(uri), "ipp",
                          NULL, this->cupsdAddress.c_str(), 0,
                          "/jobs/%d", jobId);
        ippAddString(prqst,
                     IPP_TAG_OPERATION, IPP_TAG_URI,
                     "job-uri", NULL, uri);

        ippAddString(prqst,
                     IPP_TAG_OPERATION, IPP_TAG_NAME,
                     "requesting-user-name", NULL, asUser.c_str());

        if (!docName.empty()) {
                ippAddString (prqst, IPP_TAG_OPERATION, IPP_TAG_NAME,
                              "document-name", NULL, docName.c_str());
        }

        if (!format.empty()) {
                ippAddString (prqst, IPP_TAG_OPERATION, IPP_TAG_MIMETYPE,
                              "document-format", NULL, format.c_str());
        }

        if (lastInSet) {
                ippAddBoolean (prqst, IPP_TAG_OPERATION, "last-document", 1);
        }

        stringstream dest;
        dest << "/jobs/" << jobId;
        rtn = cupsDoFileRequest (this->connection, prqst,
                                 dest.str().c_str(), filePath.c_str());

        if (rtn == NULL) {
                stringstream ee;
                ee << "Unable to print file (Job " << jobId << "), null "
                   << "response to cupsDoFileRequest, cupsLastError() returns "
                   << this->errorString (cupsLastError());
                throw runtime_error (ee.str());

        } else if (rtn->request.status.status_code > IPP_OK_CONFLICT) {
                stringstream ee;
                ee << "Unable to print file (Job " << jobId << "), "
                   << "rtn->request.status.status_code = "
                   << this->errorString (rtn->request.status.status_code);
                ippDelete (rtn);
                throw runtime_error (ee.str());

        } else {
                DBG ("cupsd accepted file for Job" << jobId);
        }

        ippDelete (rtn);
        return;
}
