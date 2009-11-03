/* -*-c++-*- */
/*!
 * A C++ wrapper for some features of the CUPS API.
 *
 * Author: Sebastian James <sjames@wmltd.co.uk>
 *
 * Copyright: 2009 William Matthew Limited.
 */

#ifndef _WMLCUPS_H_
#define _WMLCUPS_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <utility>
#include <vector>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"
#include "WmlIppAttr.h"
#include "QueueCupsStatus.h"

namespace wml {

	/*!
	 * A C++ wrapper around the CUPS API, with functions to get
	 * status information from and send commands to the cupsd.
	 *
	 * Intended always to be run in try/catch, as CUPS errors are
	 * passed back as exceptions. (Or at least that was the
	 * initial plan).
	 */
	class WmlCups
	{
	public:
		WmlCups ();
		WmlCups (std::string addr);
		~WmlCups ();

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
		 * Query cupsd for whether the given cupsPrinter is
		 * accepting
		 */
		bool getAccepting (std::string cupsPrinter);

		/*!
		 * Enable printer cupsPrinter if enable is true,
		 * disable it otherwise.
		 */
		void setAccepting (std::string cupsPrinter, bool accept);

		/*!
		 * Query cupsd for whether the given cupsPrinter is
		 * enabled
		 */
		bool getEnabled (std::string cupsPrinter);

		/*!
		 * Enable printer cupsPrinter if enable is true,
		 * disable it otherwise.
		 */
		void setEnabled (std::string cupsPrinter, bool enable);

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
				 int numJobs,
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
		 * Return a string (in English) of the printer info.
		 */
		std::string getInfo (std::string cupsPrinter);

		/*!
		 * Set the info Cups parameter to s.
		 */
		void setInfo (std::string cupsPrinter, std::string s);

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
		 * Return a string (in English) of the printer make
		 * and model.
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
		 * Set the printer destination URI to s. This is the
		 * "create a cups printer" command - if the printer
		 * doesn't exist a new one is added with the given
		 * device uri.
		 */
		void setDeviceURI (std::string cupsPrinter, std::string s);

		/*!
		 * This checks that the printer doesn't exist, then
		 * calls setDeviceURI() to create it.
		 */
		void addPrinter (std::string cupsPrinter, std::string devURI);

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
		 * Get a list of printer on the cupsd.
		 */
		std::vector<std::string> getCupsPrinterList (void);

		/*!
		 * Get the value of the attribute attr for the printer
		 * printerName. attr should have been configured with
		 * name before being passed to this function.
		 *
		 * attr has its value set, and when done,
		 * attr.getString() is returned by this function.
		 */
		std::string getPrinterAttribute (const char* printerName,
						 wml::WmlIppAttr& attr);

		/*!
		 * Set an IPP attribute for printerName. attr should
		 * have been set up with the attribute name and the
		 * attribute value.
		 */
		void setPrinterAttribute (const char* printerName,
					  wml::WmlIppAttr& attr);

		/*!
		 * Send a command for the queue, such as
		 * CUPS_ACCEPT_JOBS, CUPS_REJECT_JOBS,
		 * IPP_PAUSE_PRINTER or IPP_RESUME_PRINTER.
		 */
		void sendPrinterCommand (const char* printerName,
					 std::string asUser,
					 std::string reason,
					 ipp_op_t command);
		/*!
		 * Return true if the string s is a name which is
		 * valid to be a CUPS printer name.
		 */
		bool printerNameIsValid (std::string s);

		/*!
		 * Return true if the string s is a name which is
		 * valid to be a CUPS device address.
		 */
		bool addrIsValid (std::string s);

		/*!
		 * Return true if the string s is a name which is
		 * valid to be a CUPS queue or port.
		 */
		bool lpdqIsValid (std::string s);

		/*!
		 * Same effect as cupsLastErrorString(), except that
		 * cupsLastErrorString doesn't seem to work.
		 */
		std::string errorString (ipp_status_t err);

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

#endif // _WMLCUPS_H_
