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
wml::CupsCtrl::sendJobCommand (int jobId,
			       string asUser,
			       ipp_op_t command)
{
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
wml::CupsCtrl::cancelJobs (string printerName)
{
	vector<CupsJob> jobs;
	vector<CupsJob>::iterator iter;

	this->getJobList(printerName, jobs, "");

	for (iter = jobs.begin(); iter != jobs.end(); iter++) {
		this->sendJobCommand (iter->getId(), "default", IPP_CANCEL_JOB);
	}

	return;
}

void
wml::CupsCtrl::pauseJobs (string printerName)
{
	vector<CupsJob> jobs;
	vector<CupsJob>::iterator iter;

	this->getJobList(printerName, jobs, "");

	for (iter = jobs.begin(); iter != jobs.end(); iter++) {
		this->sendJobCommand (iter->getId(), "default", IPP_HOLD_JOB);
	}

	return;
}

void
wml::CupsCtrl::releaseJobs (string printerName)
{
	vector<CupsJob> jobs;
	vector<CupsJob>::iterator iter;

	this->getJobList(printerName, jobs, "");

	for (iter = jobs.begin(); iter != jobs.end(); iter++) {
		this->sendJobCommand (iter->getId(), "default", IPP_RELEASE_JOB);
	}

	return;
}

void
wml::CupsCtrl::getJobStatus (string cupsPrinter, int id, CupsJob& j)
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

CupsJob
wml::CupsCtrl::getJob (string id)
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
			j.setTime ((*i)->values[0].integer);
		}

		i++;
	}

	return j;
}

void
wml::CupsCtrl::getJobList (string cupsPrinter,
			   vector<CupsJob>& jList,
			   string whichJobs)
{
	DBG ("Called");
	this->getJobList (cupsPrinter, jList, 0, whichJobs);
}

/*
 * Tried two alternative implementations of getJobList, but couldn't
 * make it run any faster than 0.2 seconds to get the last job info
 * out of about 450 jobs on a queue. CUPS really needs a way to walk
 * backward through the list of jobs.
 */
void
wml::CupsCtrl::getJobList (string cupsPrinter,
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

int
wml::CupsCtrl::createJob (string cupsQueue,
			  string title,
			  string asUser,
			  string docName)
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

	string dest = "/printers/"+cupsQueue;
	rtn = cupsDoRequest (this->connection, prqst, dest.c_str());

	ipp_attribute_t * attr;		/* IPP job-id attribute */
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
wml::CupsCtrl::holdJob (int jobId, string asUser)
{
	this->sendJobCommand (jobId, asUser, IPP_HOLD_JOB);
}

void
wml::CupsCtrl::releaseJob (int jobId, string asUser)
{
	this->sendJobCommand (jobId, asUser, IPP_RELEASE_JOB);
}

void
wml::CupsCtrl::cancelJob (int jobId, string asUser)
{
	this->sendJobCommand (jobId, asUser, IPP_CANCEL_JOB);
}

void
wml::CupsCtrl::restartJob (int jobId, string asUser)
{
	this->sendJobCommand (jobId, asUser, IPP_RESTART_JOB);
}

void
wml::CupsCtrl::sendDocument (int jobId,
			     string filePath,
			     string asUser,
			     string docName,
			     string format,
			     bool lastInSet)
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