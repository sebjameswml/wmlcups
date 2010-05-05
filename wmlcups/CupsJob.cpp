#ifdef __GNUG__
#  pragma implementation
#endif

extern "C" {
#include <time.h>
}
#include <string>
#include <sstream>
#include <cups/ipp.h>

#include "config.h"
#include <futil/WmlDbg.h>
#include <futil/FoundryUtilities.h>
#include "CupsJob.h"

using namespace std;
using namespace wml;

wml::CupsJob::CupsJob (void) :
	id(0),
	copies(1),
	state(IPP_JOB_PENDING),
	creation_time (0),
	processing_time (0),
	completed_time (0)
{
}

wml::CupsJob::CupsJob (string jobName) :
	id(0),
	copies(1),
	name (jobName),
	user (""),
	sizeKB (0),
	cupsPages(0),
	state(IPP_JOB_PENDING),
	creation_time (0),
	processing_time (0),
	completed_time (0),
	sourceIP(""),
	destIP("")
{
}

wml::CupsJob::CupsJob (int jobId) :
	id(jobId),
	copies(1),
	name(""),
	user (""),
	sizeKB (0),
	cupsPages(0),
	state(IPP_JOB_PENDING),
	creation_time (0),
	processing_time (0),
	completed_time (0),
	sourceIP(""),
	destIP("")
{
}

wml::CupsJob::CupsJob (int jobId, string jobName) :
	id(jobId),
	copies(1),
	name (jobName),
	user (""),
	sizeKB (0),
	cupsPages(0),
	state(IPP_JOB_PENDING),
	creation_time (0),
	processing_time (0),
	completed_time (0),
	sourceIP(""),
	destIP("")
{
}

wml::CupsJob::~CupsJob (void)
{
}

void
wml::CupsJob::reset (void)
{
	this->id = 0;
	this->copies = 1;
	this->name = "";
	this->user = "";
	this->sizeKB = 0;
	this->cupsPages = 0;
	this->printerUri = "";
	this->state = IPP_JOB_PENDING;
	this->creation_time = 0;
	this->processing_time = 0;
	this->completed_time = 0;
	this->sourceIP = "";
	this->destIP = "";
}

void
wml::CupsJob::setId (int i)
{
	this->id = i;
}

int
wml::CupsJob::getId (void)
{
	return this->id;
}

void
wml::CupsJob::setCopies (int i)
{
	this->copies = i;
}

int
wml::CupsJob::getCopies (void)
{
	return this->copies;
}

void
wml::CupsJob::setName (std::string s)
{
	this->name = s;
}

std::string
wml::CupsJob::getName (void)
{
	return this->name;
}

void
wml::CupsJob::setUser (std::string s)
{
	this->user = s;
}

std::string
wml::CupsJob::getUser (void)
{
	return this->user;
}

void
wml::CupsJob::setSizeKB (unsigned int i)
{
	this->sizeKB = i;
}

unsigned int
wml::CupsJob::getSizeKB (void)
{
	return this->sizeKB;
}

string
wml::CupsJob::getSizeString (void)
{
	stringstream sizeString;
	sizeString.precision (3);
	float sizeNum;
	if (this->sizeKB >= 1048576) {
		sizeNum = (float) this->sizeKB / 1048576.0f;
		sizeString << sizeNum << " GB";
	} else if (this->sizeKB >= 1024) {
		sizeNum = (float) this->sizeKB / 1024.0f;
		sizeString << sizeNum << " MB";
	} else {
		sizeString << this->sizeKB << " KB";
	}

	return sizeString.str();
}

void
wml::CupsJob::setCupsPages (unsigned int i)
{
	this->cupsPages = i;
}

unsigned int
wml::CupsJob::getCupsPages (void)
{
	return this->cupsPages;
}

void
wml::CupsJob::setPrinterUri (std::string s)
{
	this->printerUri = s;
}

std::string
wml::CupsJob::getPrinterUri (void)
{
	return this->printerUri;
}

void
wml::CupsJob::setState (ipp_jstate_t jstate)
{
	this->state = jstate;
}

std::string
wml::CupsJob::getState (void)
{
	string s;

	switch (this->state) {
	case IPP_JOB_PENDING:
		s = "pending";
		break;
	case IPP_JOB_HELD:
		s = "held";
		break;
	case IPP_JOB_PROCESSING:
		s = "printing";
		break;
	case IPP_JOB_STOPPED:
		s = "stopped";
		break;
	case IPP_JOB_CANCELED:
		s = "cancelled";
		break;
	case IPP_JOB_ABORTED:
		s = "aborted due to error";
		break;
	case IPP_JOB_COMPLETED:
		s = "completed";
		break;
	default:
		s = "unknown";
		break;
	}

	return s;
}

void
wml::CupsJob::setCreationTime (int t)
{
	this->creation_time = t;
}

int
wml::CupsJob::getCreationTime (void)
{
	return this->creation_time;
}

void
wml::CupsJob::setProcessingTime (int t)
{
	this->processing_time = t;
}

int
wml::CupsJob::getProcessingTime (void)
{
	return this->processing_time;
}

void
wml::CupsJob::setCompletedTime (int t)
{
	this->completed_time = t;
}

int
wml::CupsJob::getCompletedTime (void)
{
	return this->completed_time;
}

int
wml::CupsJob::getTime (void)
{
	switch (this->state) {
	case IPP_JOB_PENDING:
		DBG ("state: IPP_JOB_PENDING, return creation_time: " << this->creation_time);
		return this->creation_time; // may be processing_time
		break;
	case IPP_JOB_HELD:
		DBG ("state: IPP_JOB_HELD, return creation_time: " << this->creation_time);
		return this->creation_time; // may be processing_time
		break;
	case IPP_JOB_PROCESSING:
		DBG ("state: IPP_JOB_PROCESSING, return processing_time: " << this->processing_time);
		return this->processing_time;
		break;
	case IPP_JOB_STOPPED:
		DBG ("state: IPP_JOB_STOPPED, return creation_time: " << this->creation_time);
		return this->creation_time; // may be processing_time
		break;
	case IPP_JOB_CANCELED:
		DBG ("state: IPP_JOB_CANCELLED, return completed_time: " << this->completed_time);
		return this->completed_time;
		break;
	case IPP_JOB_ABORTED:
		DBG ("state: IPP_JOB_ABORTED, return completed_time: " << this->completed_time);
		return this->completed_time;
		break;
	case IPP_JOB_COMPLETED:
		DBG ("state: IPP_JOB_COMPLETED, return completed_time: " << this->completed_time);
		return this->completed_time;
		break;
	default:
		DBG ("state: unknown (" << this->state << "), return creation_time: " << this->creation_time);
		return this->creation_time;
		break;
	}
}

string
wml::CupsJob::getFormattedTime (void)
{
	return FoundryUtilities::numToDateTime ((time_t)this->getTime(), '/', ':');
}

void
wml::CupsJob::setSourceIP (std::string ip)
{
	this->sourceIP = ip;
}

std::string
wml::CupsJob::getSourceIP (void)
{
	return this->sourceIP;
}

void
wml::CupsJob::setDestIP (std::string ip)
{
	this->destIP = ip;
}

std::string
wml::CupsJob::getDestIP (void)
{
	return this->destIP;
}
