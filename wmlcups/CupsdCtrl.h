/* -*-c++-*- */
/*!
 * A class to read and write the cupsd.conf file.
 *
 * Authors: Seb James <sjames@wmltd.co.uk>,
 *          Mark Richardson <mrichardson@wmltd.co.uk>
 *
 * Copyright: 2009 William Matthew Limited.
 */

#ifndef _CUPSDCTRL_H_
#define _CUPSDCTRL_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <map>
#include <list>
#include <queue>

#include "config.h"
#include <futil/WmlDbg.h>


namespace wml {

	/*!
	 * A class to hold sets of cupsd directives.
	 */
	class CupsdDirContainer
	{
	public:
		/*!
		 * Default constructor.
		 */
		CupsdDirContainer();
		/*!
		 * Constructor for creating container passing in type
		 * and parameter.
		 */
		CupsdDirContainer(std::string t, std::string p);
		~CupsdDirContainer();

		/*!
		 * Setter for type
		 */
		void setType (std::string t);

		/*!
		 * Getter for type
		 */
		std::string getType (void);

		/*!
		 * Getter for parameter
		 */
		std::string getParameter (void);

		/*!
		 * Setter for parameter
		 */
		void setParameter (std::string p);

		/*!
		 * Get the unique identifier for the container - this
		 * is built from type and parameter: If type is "Limit"
		 * and parameter is "/path", then the Id is:
		 *
		 * <Limit /path>
		 *
		 * which is the start tag of the container as it is to
		 * be written out in the cupsd.conf file.
		 *
		 * The exception is the Root container, which has the
		 * Id "Root" but which has no tags around it in the
		 * cupsd.conf file.
		 */
		std::string getId(void);

		/*!
		 * Set a directive in the container specified by
		 * containerId. If this->getId() == containerId, then
		 * the directive is placed in this->directives.
		 */
		void setDirective (std::string containerId,
				   std::string key, std::string value);

		/*!
		 * Wrapper around setDirective function when containerId
		 * is not required
		 */
		void setDirective (std::string key, std::string value);


		void setDirective (std::queue<std::pair<std::string, std:: string> > containerId, std::string key, std::string value);

		/*!
		 * Get a directive from the container specified by
		 * containerId. If this->getId() == containerId, then
		 * the directive is obtained from this->directives.
		 */
		void getDirective (std::queue<std::pair<std::string, std::string > > containerId,
					  std::string key, std::string& returnStr);

		void addContainer(wml::CupsdDirContainer cont);


		void read (std::ifstream& f, int& recurslevel);


		void write(std::ofstream& ofs, int& rlev);


		void addDir(std::string key, std::string value);

		/*std::map<std::string, std::string> getDirectiveMap();*/

	private:
		/*!
		 * The type of container - Limit, Location, Root etc
		 */
		std::string type;

		/*!
		 * The parameter of the container.
		 */
		std::string parameter;

		/*!
		 * A map of key-value pairs - the directives held in
		 * this directive container.
		 */
		std::map<std::string, std::string> directives;

		/*!
		 * Sub-containers held in this container - containers
		 * can be nested.
		 */
		std::list<wml::CupsdDirContainer> directiveContainers;
	};

	/*!
	 * A class to read values from and write to cupsd.conf. Also
	 * provides a method to cause the cupsd to reload the
	 * configuration.
	 *
	 * As well as managing cupsd.conf, this class may also manage
	 * mime.convs and mime.types (functionality to be added later).
	 */
	class CupsdCtrl
	{
	public:
		CupsdCtrl();

		CupsdCtrl(std::string p);

		~CupsdCtrl();

		/*!
		 * Set a directive in the container specified by
		 * containerId. Does not write value to the cupsd.conf
		 * file.
		 */
		void setDirective (std::queue<std::pair<std::string, std::string> > containerId,
				   std::string key, std::string value);

		/*!
		 * Get a directive from a container specified by
		 * containerId. The cupsd.conf file needs to have been
		 * read first using the read() method.
		 */
		std::string getDirective (std::queue<std::pair<std::string, std::string> > containerId,
					  std::string key);

		/*!
		 * Read the cupsd.conf file, populating
		 * this->directives.
		 */
		void read (void);

		/*!
		 * Write out the directives into cupsd.conf.
		 */
		void write (void);

		/*!
		 * Empty this->directives etc.
		 */
		void reset (void);

		/*!
		 * Restart the cupsd program.
		 */
		void recupsd (void);

	private:
		/*!
		 * The absolute path to cupsd.conf. Usually
		 * /etc/cups/cupsd.conf, but can be different
		 * esp. when developing.
		 */
		std::string cupsdPath;

		/*!
		 * \brief map for storing uncontained directives.
		 */
		std::map<std::string, std::string> directives;

		/*!
		 * \brief vector for storing containerId.
		 */
		std::queue<std::pair<std::string, std::string> > containerId;

		/*!
		 * \brief Containers to hold the cupsd directives.
		 *
		 * The cupsd.conf file has these directive containers:
		 *
		 * <Limit operations> ... </Limit>
		 *    Specifies the IPP operations that are being limited
		 *    inside a policy.
		 *
		 * <Limit methods> ... </Limit>
		 *    (Probably - not documented): Specifies the IPP
		 *    methods that are being limited inside a policy.
		 *
		 * <LimitExcept methods> ... </LimitExcept>
		 *    Specifies the HTTP methods that are being limited
		 *    inside a location.
		 *
		 * <Location /path> ... </Location>
		 *    Specifies access control for the named location.
		 *
		 * <Policy name> ... </Policy>
		 *    Specifies access control for the named policy.
		 *
		 * Plus the special container "Root" which has no
		 * parameter.
		 *
		 * Here, the container type is "Limit", "LimitExcept",
		 * "Location", "Policy" and "Root".
		 *
		 * The container also has a parameter; "operations",
		 * "methods", "/path" etc.
		 *
		 * The directives which are listed directly in the
		 * cupsd.conf are held in the "Root" container.
		 *
		 * To generate the cupsd.conf file, you run through
		 * each container, calling its output method into the
		 * file.
		 */
		std::list<wml::CupsdDirContainer> directiveContainers;
	};

} // namespace wml

#endif // _CUPSDCTRL_H_
