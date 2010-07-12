/* -*-c++-*- */
/*!
 *  This file is part of WML CUPS - a library containing extentions of
 *  and wrappers around the CUPS API.
 *
 *  WML CUPS is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  WML CUPS is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  WML CUPS is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WML CUPS (see the file COPYING).  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _CUPSCTRL_H_
#define _CUPSCTRL_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <utility>
#include <vector>
#include <list>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"
#include "IppAttr.h"
#include "Ppd.h"
#include "PpdOption.h"
#include "QueueCupsStatus.h"

/*!
 * Location of the system-standard PPD files.
 */
#define CUPS_PPD_DIR "/usr/share/ppd"

namespace wml {

	enum GET_PRINTERS {
		GET_PRINTERS_ONLY,
		GET_CLASSES,
		GET_BOTH,
		GET_PRINTERS_N
	};

	enum QTYPE {
		WMLCUPS_UNKNOWNTYPE,
		WMLCUPS_PRINTER,
		WMLCUPS_CLASS,
		QTYPE_N
	};

	/*!
	 * A C++ wrapper around the CUPS API, with functions to get
	 * status information from and send commands to the cupsd.
	 *
	 * Intended always to be run in try/catch, as CUPS errors are
	 * passed back as exceptions. (Or at least that was the
	 * initial plan).
	 */
	class CupsCtrl
	{
	public:
		CupsCtrl ();
		CupsCtrl (std::string addr, int port = 0);
		~CupsCtrl ();

		/*!
		 * After setting address, this makes a connection to
		 * the cupsd.
		 */
		void initialise (void);

		/*!
		 * Sets the cupsd address.
		 */
		void setCupsdAddress (std::string s);

		/*!
		 * Get the cupsd address
		 */
		std::string getCupsdAddress (void);

		/*!
		 * Query cupsd for whether the given cupsPrinter is
		 * accepting
		 */
		bool getAccepting (std::string cupsPrinter);

		/*!
		 * Enable printer cupsPrinter if enable is true,
		 * disable it otherwise.
		 */
		void setAccepting (std::string cupsPrinter,
				   bool accept,
				   std::string directory = "/printers/%s");

		/*!
		 * Query cupsd for whether the given cupsPrinter is
		 * enabled
		 */
		bool getEnabled (std::string cupsPrinter);

		/*!
		 * Enable printer cupsPrinter if enable is true,
		 * disable it otherwise.
		 */
		void setEnabled (std::string cupsPrinter,
				 bool enable,
				 std::string directory = "/printers/%s");
		/*!
		 * Query cupsd for whether the given cupsPrinter is
		 * shared
		 */
		bool getShared (std::string cupsPrinter);

		/*!
		 * Enable sharing for printer cupsPrinter if enable is
		 * true, disable sharing otherwise.
		 */
		void setShared (std::string cupsPrinter,
				bool enable,
				std::string directory = "/printers/%s");

		/*!
		 * Is there a printer with the given name set up on
		 * the cupsd?
		 */
		bool printerExists (std::string cupsPrinter);

		/*!
		 * Return a string (in English) of the printer state.
		 */
		std::string getState (std::string cupsPrinter);

		/*!
		 * Return a string (in English) of the printer state
		 * message, which is not the same as the printer
		 * state.
		 */
		std::string getStateMsg (std::string cupsPrinter);

		std::string getStateReasons (std::string cupsPrinter);

		/*!
		 * Get all status info for the queue cupsPrinter in
		 * the minimum number of cupsd connections, storing
		 * the results in qstat. Return true if the printer
		 * was found and status was set, false otherwise.
		 */
		bool getFullStatus (std::string cupsPrinter,
				    wml::QueueCupsStatus& qstat);


		/*!
		 * Get a list of the jobs for the printer
		 * cupsPrinter. Can pass in whichJobs as "all" or
		 * "completed" to show all jobs or only completed
		 * jobs. If whichJobs is left empty, only currently
		 * processing jobs are listed.
		 */
		void getJobList (std::string cupsPrinter,
				 std::vector<wml::CupsJob>& jList,
				 std::string whichJobs = "");

		/*!
		 * Get a list of the jobs for the printer
		 * cupsPrinter. Can pass in whichJobs as "all" or
		 * "completed" to show all jobs or only completed
		 * jobs. If whichJobs is left empty, only currently
		 * processing jobs are listed. Fills jList with up to
		 * numJobs jobs from the cupsd, most recent first. If
		 * numJobs is 0, then there is no limit on the number
		 * of jobs returned in jList.
		 *
		 * This should be faster if you only want to get the
		 * information of the last one (or few) jobs.
		 */
		void getJobList (std::string cupsPrinter,
				 std::vector<wml::CupsJob>& jList,
				 int numJobs = 0,
				 std::string whichJobs = "");

		/*!
		 * Get the Job information for the given Job ID,
		 * placing it in CupsJob j. If id == 0, then get the
		 * job information for the last job processed or
		 * in-process.
		 */
		void getJobStatus (std::string cupsPrinter,
				   int id,
				   wml::CupsJob& j);

		/*!
		 * Retrieves the job with the given id from the cups
		 * daemon and returns a CupsJob representation of that
		 * job
		 */
		wml::CupsJob getJob (std::string id);

		/*!
		 * Return a string (in English) of the printer info.
		 */
		std::string getInfo (std::string cupsPrinter);

		/*!
		 * Set the info Cups parameter to s.
		 */
		void setInfo (std::string cupsPrinter, std::string s);

		/*!
		 * Same as above, but works for a class rather than a printer
		 */
		void setClassInfo (std::string cupsPrinter, std::string s);

		/*!
		 * Return a string (in English) of the printer
		 * location.
		 */
		std::string getLocation (std::string cupsPrinter);

		/*!
		 * Set the location Cups parameter to s.
		 */
		void setLocation (std::string cupsPrinter, std::string s);

		/*!
		 * Same as above, but works for a class rather than a printer
		 */
		void setClassLocation (std::string cupsClass, std::string s);

		/*!
		 * Return a string (in English) of the printer make
		 * and model (i.e. the PPD "make and model").
		 */
		std::string getMakeModel (std::string cupsPrinter);

		/*!
		 * Set the make and model string
		 */
		void setMakeModel (std::string cupsPrinter, std::string s);

		/*!
		 * Return the printer destination URI. Something like:
		 * socket://hp3005.wml:9100
		 */
		std::string getDeviceURI (std::string cupsPrinter);

		/*!
		 * Similar to getDeviceURI, but returns a vector of URIs.
		 * Useful for getting the members of a class
		 */
		std::vector<std::string> getMemberUris (std::string cupsPrinter);

		/*!
		 * Set the printer destination URI to s. This is the
		 * "create a cups printer" command - if the printer
		 * doesn't exist a new one is added with the given
		 * device uri.
		 */
		void setDeviceURI (std::string cupsPrinter, std::string s);

		/*!
		 * Set the PPD for the given cupsPrinter. ppdTag is
		 * the identifier for the PPD, which must be available
		 * on the cupsd system.
		 */
		void setPPD (std::string cupsPrinter, std::string ppdTag);

		/*!
		 * Get the PPD for the queue cupsPrinter to the given
		 * file.
		 */
		void getPPDToFile (std::string cupsPrinter, std::string file);

		/*!
		 * Return a PPD object containing some PPD info for
		 * the given cupsPrinter. Does not return the
		 * ppd-name, which is generated internally in the
		 * cupsd, and which is not returned by this method.
		 */
		wml::Ppd getPPD (std::string cupsPrinter);

		/*!
		 * Set the provided PPD file for the given cupsPrinter.
		 */
		void setPPDFromFile (std::string cupsPrinter, std::string sourcePPDFile);

		/*!
		 * Get the PPD file stored in ppd-name on the cupsd
		 * for this queue and then obtain its nickname.
		 */
		std::string getPPDNickname (std::string cupsPrinter);

		/*!
		 * Obtain a list of the printer "makes" for which
		 * there are available PPD files stored on the
		 * system. This makes a CUPS_GET_PPDS IPP request to
		 * the cupsd to get the information.
		 */
		std::vector<std::string> getPPDListOfMakes (void);

		/*!
		 * Get the list of PPD drivers (models) for the given
		 * make. This makes a CUPS_GET_PPDS IPP request to the
		 * cupsd to get the information.
		 */
		std::vector<wml::Ppd> getPPDListOfModels (std::string make);

		/*!
		 * Obtain the PPD options for the given printer.
		 */
		std::vector<wml::PpdOption> getPPDOptions (std::string cupsPrinter);

		/*!
		 * Set the PPD option identified by keyword to value.
		 */
		void setPPDOption (std::string cupsPrinter,
				   std::string keyword,
				   std::string value);

		/*!
		 * This checks that the printer doesn't exist, then
		 * calls setDeviceURI() to create it.
		 */
		void addPrinter (std::string cupsPrinter, std::string devURI);

		/*!
		 * add printer, with device URI, and at the same time,
		 * set the PPD file.
		 */
		void addPrinter (std::string cupsPrinter, std::string s, std::string sourcePPD);

		/*!
		 * Delete the printer.
		 */
		void deletePrinter (std::string cupsPrinter);

		/*!
		 * Return the "cups" URI for the queue. Something
		 * like: ipp://circle.wml:631/printers/hp3005
		 */
		std::string getCupsURI (std::string cupsPrinter);

		/*!
		 * Get a list of printer on the cupsd. Return as vector.
		 */
		std::vector<std::string> getCupsPrinterList (wml::GET_PRINTERS getPrinters);

		/*!
		 * Wrapper around getCupsPrinterList. Returns a vector
		 * containing all classes and printers
		 */
		std::vector<std::string> getPrinterClassesList(void);

		/*!
		 * Wrapper around getCupsPrinterList. Returns a vector
		 * containing only printers
		 */
		std::vector<std::string> getPrinterList(void);

		/*!
		 * Wrapper around getCupsPrinterList. Returns a vector
		 * containing only classes
		 */
		std::vector<std::string> getClassesList(void);

		/*!
		 * Determines whether the queue is a printer or a class
		 * and returns the result as a string.
		 */
		wml::QTYPE getQueueType (std::string queuename);

		/*!
		 * Request that a job is created on the cupsd. Return
		 * the job id.
		 */
		int createJob (std::string cupsQueue,
			       std::string title="",
			       std::string asUser="",
			       std::string docName="");

		/*!
		 * As the user asUser, this IPP_HOLD_JOBs jobId
		 */
		void holdJob (int jobId, std::string asUser);

		/*!
		 * As the user asUser, this IPP_RELEASE_JOBs jobId
		 */
		void releaseJob (int jobId, std::string asUser);

		/*!
		 * As the user asUser, this IPP_CANCEL_JOBs jobId
		 */
		void cancelJob (int jobId, std::string asUser);

		/*!
		 * As the user asUser, this IPP_RESTART_JOBs jobId
		 */
		void restartJob (int jobId, std::string asUser);

		/*!
		 * Send a file specified by filePath to the cupsd,
		 * adding it to the job with id jobId. Optionally set
		 * the document name and the format string. If
		 * lastInSet is false, the doc will not be printed
		 * immediately.
		 */
		void sendDocument (int jobId,
				   std::string filePath,
				   std::string asUser,
				   std::string docName = "",
				   std::string format = "",
				   bool lastInSet = true);
		/*!
		 * Print the file at filePath to the CUPS queue
		 * cupsQueue. Throw exceptions if the file does not
		 * exist, or if the cupsd refuses to print it. Return
		 * the job id.
		 */
		int printFile (std::string filePath,
			       std::string jobTitle,
			       std::string cupsQueue);

		/*!
		 * Like getCupsPrinterList but returns a list rather
		 * than a vector.
		 */
		std::list<std::string> getCupsPrinterList2 (void);

		/*!
		 * Get the value of the attribute attr for the printer
		 * printerName. attr should have been configured with
		 * name before being passed to this function.
		 *
		 * attr has its value set, and when done,
		 * attr.getString() is returned by this function.
		 */
		std::string getPrinterAttribute (const char* printerName,
						 wml::IppAttr& attr);


		/*!
		 * Similar to getPrinterAttribute, but returns a vector
		 * of strings, instead of a single string. Useful when
		 * an attribute contains more than one value, such as
		 * the member-uris attribute of a class.
		 */
		std::vector<std::string> getQueueAttribute (const char* printerName,
							    wml::IppAttr& attr);

		/*!
		 * Set an IPP attribute for printerName. attr should
		 * have been set up with the attribute name and the
		 * attribute value.
		 */
		void setPrinterAttribute (const char* printerName,
					  wml::IppAttr& attr);

		/*!
		 * Set two IPP attributes for printerName. attrs
		 * should have been set up with the attribute name and
		 * the attribute value.
		 */
		void setPrinterAttributes (const char* printerName,
					   wml::IppAttr& attr1,
					   wml::IppAttr& attr2);

		/*!
		 * Same as setPrinterAttribute, but used for classes
		 * rather than printers.
		 */
		void setClassAttribute (const char* printerName,
					wml::IppAttr& attr);

		/*!
		 * Sets up the members of a class. The vector members
		 * contains the names of the members to be added to
		 * the class
		 */
		void setClassMembers (std::string className,
				      std::vector<std::string> members);

		/*!
		 * Send a command for the queue, such as
		 * CUPS_ACCEPT_JOBS, CUPS_REJECT_JOBS,
		 * IPP_PAUSE_PRINTER or IPP_RESUME_PRINTER.
		 */
		void sendPrinterCommand (const char* printerName,
					 std::string asUser,
					 std::string reason,
					 ipp_op_t command,
					 std::string directory = "/printers/%s");
		/*!
		 * Return true if the string s is a name which is
		 * valid to be a CUPS printer name.
		 */
		static bool printerNameIsValid (std::string s);

		/*!
		 * Return true if the string s is a name which is
		 * valid to be a CUPS device address.
		 */
		static bool addrIsValid (std::string s);

		/*!
		 * Return true if the string s is a name which is
		 * valid to be a destination queue.
		 */
		static bool lpdqIsValid (std::string s);

		/*!
		 * Return true if the string s is a name which is
		 * valid to be a CUPS port.
		 */
		static bool portIsValid (std::string s);

		/*!
		 * Return true if the string s is a valid to be a CUPS
		 * job title.
		 */
		static bool titleIsValid (std::string s);

		/*!
		 * Same effect as cupsLastErrorString(), except that
		 * cupsLastErrorString doesn't seem to work.
		 */
		std::string errorString (ipp_status_t err);

		/*!
		 * Gets the string equivalent of the http_status_t
		 * status/error.
		 */
		std::string errorString (http_status_t err);

		/*!
		 * General function for sending job commands (such
		 * as cancel, etc) to the Cups daemon, to be applied
		 * to the job with the id provided
		 */
		void sendJobCommand (int jobId,
				     std::string asUser,
				     ipp_op_t command);

		/*!
		 * Cancels all jobs on the queue with the given
		 * printer name
		 */
		void cancelJobs (std::string printerName);

		/*!
		 * Pauses all jobs on the queue with the given
		 * printer name
		 */
		void pauseJobs (std::string printerName);

		/*!
		 * Resumes all jobs on the queue with the given
		 * printer name
		 */
		void releaseJobs (std::string printerName);


	private:
		/*!
		 * The IP or address of the CUPS server to query.
		 */
		std::string cupsdAddress;

		/*!
		 * The connection to the cupsd. Initialised in
		 * constructor.
		 */
		http_t * connection;
	};

} // namespace wml

#endif // _CUPSCTRL_H_
