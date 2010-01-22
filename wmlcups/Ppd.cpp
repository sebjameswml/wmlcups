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
#include <vector>

#include "Ppd.h"

using namespace std;
using namespace wml;

wml::Ppd::Ppd (void) :
	type (0),
	modelNumber(0)
{
}

wml::Ppd::~Ppd (void)
{
}

void
wml::Ppd::setName (std::string s)
{
	this->name = s;
}

void
wml::Ppd::setMakeAndModel (std::string s)
{
	this->makeAndModel = s;
}

void
wml::Ppd::setMake (std::string s)
{
	this->make = s;
}

void
wml::Ppd::setDeviceId (std::string s)
{
	this->deviceId = s;
}

void
wml::Ppd::setProduct (std::string s)
{
	this->product = s;
}

void
wml::Ppd::setPsversion (std::string s)
{
	this->psversion = s;
}

void
wml::Ppd::setType (int i)
{
	this->type = i;
}

void
wml::Ppd::setModelNumber (int i)
{
	this->modelNumber = i;
}

void
wml::Ppd::setNaturalLanguage (string s)
{
	this->naturalLanguage = s;
}

std::string
wml::Ppd::getName (void)
{
	return this->name;
}

std::string
wml::Ppd::getMakeAndModel (void)
{
	return this->makeAndModel;
}

std::string
wml::Ppd::getMake (void)
{
	return this->make;
}

std::string
wml::Ppd::getDeviceId (void)
{
	return this->deviceId;
}

std::string
wml::Ppd::getProduct (void)
{
	return this->product;
}

std::string
wml::Ppd::getPsversion (void)
{
	return this->psversion;
}

int
wml::Ppd::getType (void)
{
	return this->type;
}

int
wml::Ppd::getModelNumber (void)
{
	return this->modelNumber;
}

std::string
wml::Ppd::getNaturalLanguage (void)
{
	return this->naturalLanguage;
}
