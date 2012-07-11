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
wml::Ppd::setName (const std::string& s)
{
        this->name = s;
}

void
wml::Ppd::setMakeAndModel (const std::string& s)
{
        this->makeAndModel = s;
}

void
wml::Ppd::setMake (const std::string& s)
{
        this->make = s;
}

void
wml::Ppd::setDeviceId (const std::string& s)
{
        this->deviceId = s;
}

void
wml::Ppd::setProduct (const std::string& s)
{
        this->product = s;
}

void
wml::Ppd::setPsversion (const std::string& s)
{
        this->psversion = s;
}

void
wml::Ppd::setType (const int i)
{
        this->type = i;
}

void
wml::Ppd::setModelNumber (const int i)
{
        this->modelNumber = i;
}

void
wml::Ppd::setNaturalLanguage (const string& s)
{
        this->naturalLanguage = s;
}

std::string
wml::Ppd::getName (void) const
{
        return this->name;
}

std::string
wml::Ppd::getMakeAndModel (void) const
{
        return this->makeAndModel;
}

std::string
wml::Ppd::getMake (void) const
{
        return this->make;
}

std::string
wml::Ppd::getDeviceId (void) const
{
        return this->deviceId;
}

std::string
wml::Ppd::getProduct (void) const
{
        return this->product;
}

std::string
wml::Ppd::getPsversion (void) const
{
        return this->psversion;
}

int
wml::Ppd::getType (void) const
{
        return this->type;
}

int
wml::Ppd::getModelNumber (void) const
{
        return this->modelNumber;
}

std::string
wml::Ppd::getNaturalLanguage (void) const
{
        return this->naturalLanguage;
}
