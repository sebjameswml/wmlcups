#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"

extern "C" {
#include <stdlib.h>
}

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>

#include <futil/FoundryUtilities.h>
#include "CupsdCtrl.h"

using namespace std;
using namespace wml;

/*!
 * CupsdDirContainer class
 */
//@{

wml::CupsdDirContainer::CupsdDirContainer ():
	type("root")
{

}

wml::CupsdDirContainer::CupsdDirContainer (string t, string p):
	type(t),
	parameter(p)
{

}

wml::CupsdDirContainer::~CupsdDirContainer ()
{
}

string
wml::CupsdDirContainer::getType()
{
	return this->type;
}

string
wml::CupsdDirContainer::getParameter()
{
	return this->parameter;
}

string
wml::CupsdDirContainer::getId()
{
	stringstream idss;
	idss << this->getType() << " " << this->getParameter();
	return idss.str();
}

void
wml::CupsdDirContainer::setDirective(queue<pair<string, string> > containerId, string key, string value)
{
	bool containerFound = false;
	if (!containerId.empty()){
		pair <string, string> temp = containerId.front();
		containerId.pop();

		list<CupsdDirContainer>::iterator iter;
		for (iter = this->directiveContainers.begin(); iter != this->directiveContainers.end(); iter++) {
			string containerIdString = temp.first + " " + temp.second;
			if (iter->getId() == containerIdString) {
				containerFound = true;
				iter->setDirective(containerId, key, value);
			}
		}
		if (containerFound == false) {
			throw runtime_error("Container not found 4");
		}
	} else {
		this->setDirective(key, value);
	}
}

void
wml::CupsdDirContainer::setDirective(string key, string value)
{
	this->setDirective(this->getId(), key, value);
}

void
wml::CupsdDirContainer::setDirective(string containerId, string key, string value)
{
	//map<string,string>::iterator iter;
	if (containerId == this->getId()) {
		if (directives.find(key) == directives.end()) {
			;
		} else {
			this->directives.erase(key);
		}
		this->directives.insert (make_pair( key, value) );
	}
}

/*void
wml::CupsdDirContainer::getDirective(queue<pair<string, string> > containerId, string key, string& returnStr)
{
	this->getDirective(containerId, key, returnStr, false);
	}*/

void
wml::CupsdDirContainer::getDirective(queue<pair<string, string> > containerId, string key, string& returnStr, bool valueOnly)
{
	bool containerFound = false;
	if (!containerId.empty()) {

		pair <string, string> temp = containerId.front();
		containerId.pop();

		list<CupsdDirContainer>::iterator iter;
		for (iter = this->directiveContainers.begin(); iter != this->directiveContainers.end(); iter++) {
			string containerIdString = temp.first + " " + temp.second;
			if (iter->getId() == containerIdString) {
				containerFound = true;
				iter->getDirective(containerId, key, returnStr);
			}
		}
		if (containerFound == false) {
			throw runtime_error("Container not found 3");
		}

	} else {
		map<string, string>::iterator iter;
		iter = this->directives.find(key);
		if (iter != this->directives.end()) {
			if (valueOnly == false) {
				this->fullDirective(returnStr, iter);
			} else {
				this->directiveValue(returnStr, iter);
			}
		} else {
			returnStr = "Directive not found";
		}
	}
}

void
wml::CupsdDirContainer::directiveValue(string& returnStr, map<string, string>::iterator& iter)
{
	returnStr = iter->second;
}

void
wml::CupsdDirContainer::fullDirective(string& returnStr, map<string, string>::iterator& iter)
{
	returnStr = iter->first + " " + iter->second;
}

void
wml::CupsdDirContainer::addContainer(CupsdDirContainer cont)
{

	this->directiveContainers.push_back(cont);

}

void
wml::CupsdDirContainer::read (ifstream& f, int& recurslevel)
{

	for (int i = 0; i < recurslevel; i++) {
		cout << "   ";
	}
	cout << "<" << this->getType() << " " << this->getParameter() << ">" << endl;
	int pos;
	int endpos;
	string line;

	while (getline(f,line)) {
		FoundryUtilities::stripLeadingSpaces(line);
		if (line[0] == '<') {
			if (line[1] == '/') {
				list<CupsdDirContainer>::iterator iter;

				for (int i = 0; i < recurslevel; i++){
					cout << "   ";
				}

				cout << "</" << this->getType() << ">" << endl;
				recurslevel -= 1;

				return;
			} else {
				pos = line.find(" ");
				endpos = line.find (">");
				CupsdDirContainer c(line.substr(1, pos-1), line.substr(pos+1,endpos - (pos+1)));
				recurslevel += 1;
				c.read (f, recurslevel);

				this->directiveContainers.push_back (c);
			}
		} else if (line[0] == '#') {

		} else {
			if (line.size() > 0){
				pos = line.find (" ");
				string key = line.substr(0, pos);
				string value = line.substr(pos+1);
				for (int i = 0; i < recurslevel; i++) {
					cout << "   ";
				}
				cout << "   " << key << " " << value << endl;
				this->setDirective (key, value);
			}
		}
	}
}

void
wml::CupsdDirContainer::write(ofstream& ofs, int& rlev)
{
	list<CupsdDirContainer>::iterator iter;
	//ofs << "#Rlev = " << rlev << endl;
	for (int i = 0; i < rlev; i++)
	{
		ofs << "   ";
	}

	if (this->getType() != "root") {
		ofs << "<" << this->getType() << " " << this->getParameter() << ">" << endl;
	}

	//ofs << endl;

	map< string, string >::iterator mapiter;
	for (mapiter = this->directives.begin(); mapiter != this->directives.end(); mapiter++) {
		for (int i = 0; i < rlev; i++)
		{
			ofs << "   ";
		}
		ofs << mapiter->first << " " << mapiter->second << endl;
	}

	if (this->getType() == "root") {
		ofs << endl;
	}

	for (iter = this->directiveContainers.begin(); iter != this->directiveContainers.end(); iter++) {
		rlev += 1;
		iter->write(ofs,rlev);
	}



	for (int i = 0; i < rlev; i++)
	{
		ofs << "   ";
	}
	if (this->getType() != "root") {
		ofs << "</" << this->getType() << ">" << endl << endl;
	}
	rlev -= 1;
}

// Add CupsdDirContainer methods....

//@}

/*!
 * CupsdCtrl class
 */
//@{

wml::CupsdCtrl::CupsdCtrl ():
	cupsdPath("/etc/cups/cupsd.conf")
{

}

wml::CupsdCtrl::CupsdCtrl (string p):
	cupsdPath(p)
{

}

wml::CupsdCtrl::~CupsdCtrl ()
{
}

void
wml::CupsdCtrl::read(void)
{
	ifstream f;
	int recurslevel = -1;
	CupsdDirContainer rootCont;

	f.open(this->cupsdPath.c_str(), ios::in);

	rootCont.read (f, recurslevel);

	this->directiveContainers.push_back(rootCont);

	f.close();
}

void
wml::CupsdCtrl::write(void)
{

	ofstream ofs;
	int rlev = -1;


	ofs.open(this->cupsdPath.c_str(), ios::out);

	list<CupsdDirContainer>::iterator iter;
	for (iter = this->directiveContainers.begin(); iter != this->directiveContainers.end(); iter++) {
		//ofs << "#Adding 1 to rlev" << endl;
		//rlev += 1;
		iter->write(ofs, rlev);
	}

	ofs.close();

}
// Add CupsdCtrl methods....

//@}

void
wml::CupsdCtrl::setDirective(queue<pair<string, string> > containerId, string key, string value)
{
	bool containerFound = false;
	pair <string, string> temp = containerId.front();
	containerId.pop();

	list<CupsdDirContainer>::iterator iter;
	for (iter = this->directiveContainers.begin(); iter != this->directiveContainers.end(); iter++) {
		string containerIdString = temp.first + " " + temp.second;
		if (iter->getId() == containerIdString) {
			containerFound = true;
			iter->setDirective(containerId, key, value);
		}
	}

	if (containerFound == false) {
		throw runtime_error("Container not found 2");
	}

}

string
wml::CupsdCtrl::getDirective(queue<pair<string, string> > containerId, string key, bool valueOnly)
{
	string returnStr;
	bool containerFound = false;

	pair <string, string> temp = containerId.front();
	containerId.pop();
	list<CupsdDirContainer>::iterator iter;
	for (iter = this->directiveContainers.begin(); iter != this->directiveContainers.end(); iter++) {
		string containerIdString = temp.first + " " + temp.second;
		if (iter->getId() == containerIdString) {
			containerFound = true;
			iter->getDirective(containerId, key, returnStr, valueOnly);
		}
	}

	if (containerFound == false) {
		throw runtime_error("Container not found 1");
	}

	return returnStr;
}

void
wml::CupsdCtrl::restartCups(void)
{
	if (FoundryUtilities::fileExists("/usr/sbin/recupsd")) {
		system("/usr/sbin/recupsd");
	} else {
		DBG ("In the real world, I'd now restart cups. "
		     "But, alas, this is an imaginary world");
	}
}
