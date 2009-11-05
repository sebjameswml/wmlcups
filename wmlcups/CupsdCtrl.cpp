#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"

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
	type("Root")
{
	//this->type = "Root";
}

wml::CupsdDirContainer::CupsdDirContainer (string t, string p):
	type(t),
	parameter(p)
{

	//this->type = t;
	//this->parameter = p;
// writeme
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
	//string id;
	idss << this->getType() << " " << this->getParameter();
	return idss.str();
}

void
wml::CupsdDirContainer::setDirective(string containerId, string key, string value)
{
	if (containerId == this->getId()) {

		this->directives.insert (make_pair( key, value) );

	}

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
				//cout << key << " " << value << endl;
				//	cout << "Line type is directive    : " << line << endl;
				for (int i = 0; i < recurslevel; i++) {
					cout << "   ";
				}
				cout << "   " << key << " " << value << endl;
				this->setDirective (this->getId(), key, value);
			}
		}
	}

}

/*map<string, string>
wml::CupsdDirContainer::getDirectiveMap()
{
	return this->directives;
	}*/
// Add CupsdDirContainer methods....

//@}

/*!
 * CupsdCtrl class
 */
//@{

wml::CupsdCtrl::CupsdCtrl ():
	cupsdPath("/etc/cups/cupsd.conf")

{
	//CupsdDirContainer cdirc();
	//this->cupsdPath = "/etc/cups/cupsd.conf";
}

wml::CupsdCtrl::CupsdCtrl (string p):
	cupsdPath(p)
{
	//this->cupsdPath = p;
}

wml::CupsdCtrl::~CupsdCtrl ()
{
}

void
wml::CupsdCtrl::read()
{
	ifstream f;
	string line;
	int pos;
	int endpos;
	int recurslevel = 0;
	CupsdDirContainer currentC;
	f.open(this->cupsdPath.c_str(), ios::in);
	while (getline(f, line)){
		FoundryUtilities::stripLeadingSpaces(line);
		if (line[0] == '<') {
			if (line[1] == '/') {
				//	cout << "Line type is container end: " << line << endl;

			} else {
				pos = line.find(" ");
				endpos = line.find (">");
				CupsdDirContainer c(line.substr(1, pos-1), line.substr(pos+1,endpos - (pos+1)));
				//cout << "recursion level is: " << recurslevel << endl;
				recurslevel += 1;
				c.read (f, recurslevel);

				this->directiveContainers.push_back (c);

#ifdef OLD
				if (c != currentC) {

				}
				currentC = c;
				this->directiveContainers.push_back (c);
				//	cout << "Line type is container    : " << line << endl;
#endif
			}

		} else if (line[0] == '#'){
			//cout << "Line type is comment      : " << line << endl;
		} else {
			pos = line.find (" ");
			//cout << pos;
			if (line.size() > 0){
				string key = line.substr(0, pos);
				string value = line.substr(pos+1);
				cout << key << " " << value << endl;
				//	cout << "Line type is directive    : " << line << endl;
				currentC.setDirective (currentC.getId(), key, value);
			}
		}
		DBG(line);
		//	cout << this->directiveContainers[i].type;
		//i++;
	}

	list<CupsdDirContainer>::iterator listiter;

	for (listiter = this->directiveContainers.begin(); listiter != this->directiveContainers.end(); listiter++) {
		//cout << listiter->getType() << "," << listiter->getParameter() << endl;
	}

	f.close();
}


// Add CupsdCtrl methods....

//@}
