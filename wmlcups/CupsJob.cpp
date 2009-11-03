#ifdef __GNUG__
#  pragma implementation
#endif

extern "C" {
#include <time.h>
}
#include <string>
#include <cups/ipp.h>

#include "config.h"
#include <futil/FoundryUtilities.h>
#include "CupsJob.h"

using namespace std;
using namespace wml;

wml::CupsJob::CupsJob () :
	id(0),
	copies(1),
	state(IPP_JOB_PENDING)
{
}

wml::CupsJob::~CupsJob ()
{
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
wml::CupsJob::setTime (int t)
{
	this->time = t;
}

int
wml::CupsJob::getTime (void)
{
	return this->time;
}

string
wml::CupsJob::getFormattedTime (void)
{
	return FoundryUtilities::numToDateTime ((time_t)this->time, '/', ':');
}
