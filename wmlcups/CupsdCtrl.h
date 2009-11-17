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

/*! \file CupsdCtrl.h
 * \brief Declares the classes CupsdDirContainer and CupsdCtrl
 */

namespace wml {

	/*! \class CupsdDirContainer
	 * \brief A class to hold sets of cupsd directives.
	 */
	class CupsdDirContainer
	{
	public:
                /*! \name Constructor and Destructor */
		//@{

		/*!
		 * \brief Constructor
		 * Create a new CupsdDirContainer object
		 */
		CupsdDirContainer(void);

		/*!
		 * \brief Constructor
		 * Create a new CupsdDirContainer object, with the
		 * provided type and parameter (<Type Parameter>)
		 *
		 * \param t The type of the new container
		 *
		 * \param p The parameter of the new container
		 */
		CupsdDirContainer(std::string t, std::string p);

		/*!
		 * \brief Destructor
		 * Delete this CupsdDirContainer object
		 */
		~CupsdDirContainer(void);
		//@}

		/*! \name Public Class methods */
		//@{

		/*!
		 * \brief Setter for type
		 * Sets the type of the container (<Type Parameter>)
		 *
		 * \param t The new type of the container
		 */
		void setType (std::string t);

		/*!
		 * \brief Getter for type
		 *
		 * \return The type of the container in a string
		 */
		std::string getType (void);

		/*!
		 * \brief Getter for parameter
		 *
		 * \return The parameter of the container in a string
		 */
		std::string getParameter (void);

		/*!
		 * \brief Setter for parameter
		 * Sets the parameter of the container (<Type Parameter>)
		 *
		 * \param p The new parameter of the container
		 */
		void setParameter (std::string p);

		/*!
		 * \brief Getter for complete container ID
		 *
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
		 *
		 * \return The ID of the container in a string
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

		/*!
		 * \brief Set a directive
		 * Takes a queue of container ids, consisting of the path
		 * to the directive. Recursively pops each container in the
		 * path until the bottom container is reached, then adds the
		 * directive of "key value" to that container.
		 *
		 * \param containerId A queue containing each container in the
		 * path to the directive. The first of these is always "root",
		 * or the call will return "container not found".
		 *
		 * \param key The key of the key/value pair of the directive
		 * to be added
		 *
		 * \param value The value of the key/value pair of the directive
		 * to be added
	       	 */
		void setDirective (std::queue<std::pair<std::string, std:: string> > containerId, std::string key, std::string value);

		/*!
		 * \brief Get a directive
		 * Takes a queue of container ids, consisting of the path
		 * to the directive. Recursively pops each container in the
		 * path until the bottom container is reached, then returns
		 * the directive
		 *
		 * \param containerId A queue containing each container in the
		 * path to the directive. The first of these is always "root",
		 * or the call will return "container not found".
		 *
		 * \param key The key of the directive being searched for
		 *
		 * \param returnStr A reference to a string that is used to
		 * contain the directive once it has been found, and pass it
		 * back out.
		 *
		 * \param valueOnly If this is set to true, the function will
		 * return only the value of the directive. If set to false,
		 * returns the complete directive. Set to false by default
		 *
		 */
		void getDirective (std::queue<std::pair<std::string, std::string > > containerId,
				   std::string key, std::string& returnStr, bool valueOnly = false);

		/*!
		 * \brief Add a sub-container to this container
		 * Adds a sub-container this containers set of sub-containers.
		 *
		 * \param cont The container to be added as a sub-container
		 */
		void addContainer(wml::CupsdDirContainer cont);

		/*!
		 * \brief Read in the current level of containers and directives
		 * Reads in the current level of containers and directives
		 * from the cups configuration file specified, and adds them to
		 * the appropriate data structure
		 *
		 * \param f The input file stream being read from
		 *
		 * \param recursLevel The current depth of recursion. Only
		 * used to determine level of tabbing for output to std out
		 */
		void read (std::ifstream& f, int& recurslevel);

		/*!
		 * \brief Write the current level of containers and directives
		 * Writes the current level of containers and directives to
		 * the cups configuration file specified, based on the contents
		 * of the directives and directiveContainers structures
		 *
		 * \param ofs The output file stream object that is used to
		 * write to a file
		 *
		 * \param rlev The current depth of recursion. Only used to
		 * determine level of tabbing for output to the file
		 */
		void write(std::ofstream& ofs, int& rlev);

		/*!
		 * \brief Adds a new directive
		 * Adds a new directive to this containers directives structure
		 *
		 * \param key The key of the key/value pair of the directive
		 * to be added
		 *
		 * \param value The value of the key/value pair of the directive
		 * to be added
		 */
		void addDir(std::string key, std::string value);

		//@}

	private:
		/*! \name Private class attributes */
		//@{

		/*!
		 * \brief The type of container - Limit, Location, Root etc
		 * <Type Parameter>
		 */
		std::string type;

		/*!
		 * \brief The parameter of the container.
		 * <Type Parameter>
		 */
		std::string parameter;

		/*!
		 * \brief A map of key-value pairs
		 * The directives held in this directive container.
		 */
		std::map<std::string, std::string> directives;

		/*!
		 * \brief Sub-containers held in this container
		 * containers can be nested.
		 */
		std::list<wml::CupsdDirContainer> directiveContainers;

		/*!
		 * \brief Returns just the directive value
		 * This function simply returns the value of the directive in
		 * question, specified by iter
		 *
		 * \param returnStr The string containing the directive value,
		 * returned to the calling function
		 *
		 * \param iter A pointer to the directive object
		 */
		void directiveValue (std::string& returnStr, std::map<std::string, std::string>::iterator& iter);

		/*!
		 * \brief Returns a full directive
		 * This function returns a full directive, specificed by iter
		 *
		 * \param returnStr The string containing the directive,
		 * returned to the calling function
		 *
		 * \param iter A pointer to the directive object
		 */
		void fullDirective (std::string& returnStr, std::map<std::string, std::string>::iterator& iter);

		//@}
	};

	/*! \class CupsdCtrl
	 * \brief A class that controls cupsd.conf settings
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
                /*! \name Constructor and Destructor */
		//@{

		/*!
		 * \brief Constructor
		 * Create a new CupsdCtrl object
		 * Default constructor sets file path to /etc/cups/cupsd.conf
		 */
		CupsdCtrl();

		/*!
		 * \brief Constructor
		 * Create a new CupsdCtrl object, setting the file path to p
		 */
		CupsdCtrl(std::string p);

		/*!
		 * \brief Deconstructor
		 * Delete this CupsdCtrl object
		 */
		~CupsdCtrl();

		/*!
		 * \brief Set a directive
		 * Sets a directive based on the path stored in containerId,
		 * and the key/value pair specified. This does not write
		 * the directive to the file specified
		 *
		 * \param containerId The path to the directive to be set
		 *
		 * \param key The key of the key/value pair of the directive
		 * to be set
		 *
		 * \param value The value of the key/value pair of the directive
		 * to be set
		 */
		void setDirective (std::queue<std::pair<std::string, std::string> > containerId,
				   std::string key, std::string value);

		/*!
		 * \brief Get a directive
		 * Returns a directive based on the path stored in containerId,
		 * and the key/value pair specified. This does not write the
		 * directive to the file specified.
		 *
		 * \param containerId The path to the directive to be returned
		 *
		 * \param key The key of the directive to be returned
		 *
		 * \param valueOnly specifies if the function should return
		 * the full directive or the value only
		 *
		 * \return Returns a string containing either the full directive,
		 * or the directives value
		 */
		std::string getDirective (std::queue<std::pair<std::string, std::string> > containerId,
					  std::string key, bool valueOnly = false);

		/*!
		 * \brief Read in the file specified
		 * Reads in the file specified, and populates the directiveContainers
		 * and directives data structures
		 */
		void read (void);

		/*!
		 * \brief Write to the file specified
		 * Writes the data stored in the directiveContainers and directives
		 * data structures to the file specified
		 */
		void write (void);

		/*!
		 * Empty this->directives etc.
		 */
		void reset (void);

		/*!
		 * \brief Restart the cupsd program.
		 */
		void restartCups (void);

	private:
		/*!
		 * \brief Contains the full path to the file to be edited
		 * Set to /etc/cups/cupsd.conf by default
		 */
		std::string cupsdPath;

		/*!
		 * \brief map for storing root directives.
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
