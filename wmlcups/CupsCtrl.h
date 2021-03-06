/* -*-c++-*- */
/*!
 *  This file is part of wmlcups - a library containing extentions of
 *  and wrappers around the CUPS API.
 *
 *  wmlcups is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  wmlcups is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  wmlcups is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wmlcups (see the file COPYING).  If not, see
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

        /*!
         * The CUPS "type" of the queue. This helps us distinguish
         * between printers and classes, which have some features in
         * common, but not all features in common. In our wml::Queue
         * class, the Queue has a QTYPE, to save us having the need
         * for wml::Queue and wml::Class classes.
         */
        enum QTYPE {
                WMLCUPS_UNKNOWNTYPE,
                WMLCUPS_PRINTER,
                WMLCUPS_CLASS,
                QTYPE_N
        };

        /*!
         * Used to define which of printers, classes or both we want
         * to get in a request to the cupsd.
         */
        enum GET_PRINTERS {
                GET_PRINTERS_ONLY,
                GET_CLASSES,
                GET_BOTH,
                GET_PRINTERS_N
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
                void setCupsdAddress (const std::string& s);

                /*!
                 * Get the cupsd address
                 */
                std::string getCupsdAddress (void) const;

                /*!
                 * Query cupsd for whether the given cupsPrinter is
                 * accepting
                 */
                bool getAccepting (const std::string& cupsPrinter) const;

                /*!
                 * Enable printer cupsPrinter if enable is true,
                 * disable it otherwise.
                 */
                void setAccepting (const std::string& cupsPrinter,
                                   const bool accept,
                                   const std::string& directory = "/printers/%s");

                /*!
                 * Query cupsd for whether the given cupsPrinter is
                 * enabled
                 */
                bool getEnabled (const std::string& cupsPrinter) const;

                /*!
                 * Enable printer cupsPrinter if enable is true,
                 * disable it otherwise.
                 */
                void setEnabled (const std::string& cupsPrinter,
                                 const bool enable,
                                 const std::string& directory = "/printers/%s");
                /*!
                 * Query cupsd for whether the given cupsPrinter is
                 * shared
                 */
                bool getShared (const std::string& cupsPrinter) const;

                /*!
                 * Enable sharing for printer cupsPrinter if enable is
                 * true, disable sharing otherwise.
                 */
                void setShared (const std::string& cupsPrinter,
                                const bool enable,
                                const std::string& directory = "/printers/%s");

                /*!
                 * Is there a printer with the given name set up on
                 * the cupsd?
                 */
                bool printerExists (const std::string& cupsPrinter) const;

                /*!
                 * Return a string (in English) of the printer state.
                 */
                std::string getState (const std::string& cupsPrinter) const;

                /*!
                 * Return a string (in English) of the printer state
                 * message, which is not the same as the printer
                 * state.
                 */
                std::string getStateMsg (const std::string& cupsPrinter) const;

                std::string getStateReasons (const std::string& cupsPrinter) const;

                /*!
                 * Get all status info for the queue cupsPrinter in
                 * the minimum number of cupsd connections, storing
                 * the results in qstat. Return true if the printer
                 * was found and status was set, false otherwise.
                 */
                bool getFullStatus (const std::string& cupsPrinter,
                                    wml::QueueCupsStatus& qstat,
                                    const bool includeJobStatus = true) const;

                /*!
                 * Get a list of the jobs for the printer
                 * cupsPrinter. Can pass in whichJobs as "all" or
                 * "completed" to show all jobs or only completed
                 * jobs. If whichJobs is left empty, only currently
                 * processing jobs are listed.
                 */
                void getJobList (const std::string& cupsPrinter,
                                 std::vector<wml::CupsJob>& jList,
                                 const std::string& whichJobs = "") const;

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
                void getJobList (const std::string& cupsPrinter,
                                 std::vector<wml::CupsJob>& jList,
                                 const int numJobs = 0,
                                 const std::string& whichJobs = "") const;

                /*!
                 * Get the Job information for the given Job ID,
                 * placing it in CupsJob j. If id == 0, then get the
                 * job information for the last job processed or
                 * in-process.
                 */
                void getJobStatus (const std::string& cupsPrinter,
                                   const int id,
                                   wml::CupsJob& j) const;

                /*!
                 * Retrieves the job with the given id from the cups
                 * daemon and returns a CupsJob representation of that
                 * job
                 */
                wml::CupsJob getJob (const std::string& id) const;

                /*!
                 * Return a string (in English) of the printer info.
                 */
                std::string getInfo (const std::string& cupsPrinter) const;

                /*!
                 * Set the info Cups parameter to s.
                 */
                void setInfo (const std::string& cupsPrinter, const std::string& s);

                /*!
                 * Same as above, but works for a class rather than a printer
                 */
                void setClassInfo (const std::string& cupsPrinter, const std::string& s);

                /*!
                 * Return a string (in English) of the printer
                 * location.
                 */
                std::string getLocation (const std::string& cupsPrinter) const;

                /*!
                 * Set the location Cups parameter to s.
                 */
                void setLocation (const std::string& cupsPrinter, const std::string& s);

                /*!
                 * Same as above, but works for a class rather than a printer
                 */
                void setClassLocation (const std::string& cupsClass, const std::string& s);

                /*!
                 * Return a string (in English) of the printer make
                 * and model (i.e. the PPD "make and model").
                 */
                std::string getMakeModel (const std::string& cupsPrinter) const;

                /*!
                 * Set the make and model string
                 */
                void setMakeModel (const std::string& cupsPrinter, const std::string& s);

                /*!
                 * Return the printer destination URI. Something like:
                 * socket://hp3005.wml:9100
                 */
                std::string getDeviceURI (const std::string& cupsPrinter) const;

                /*!
                 * Similar to getDeviceURI, but returns a vector of URIs.
                 * Useful for getting the members of a class
                 */
                std::vector<std::string> getMemberUris (const std::string& cupsPrinter) const;

                /*!
                 * Set the printer destination URI to s. This is the
                 * "create a cups printer" command - if the printer
                 * doesn't exist a new one is added with the given
                 * device uri.
                 */
                void setDeviceURI (const std::string& cupsPrinter, const std::string& s);

                /*!
                 * Set the PPD for the given cupsPrinter. ppdTag is
                 * the identifier for the PPD, which must be available
                 * on the cupsd system.
                 */
                void setPPD (const std::string& cupsPrinter, const std::string& ppdTag);

                /*!
                 * Get the PPD for the queue cupsPrinter to the given
                 * file.
                 */
                void getPPDToFile (const std::string& cupsPrinter, const std::string& file) const;

                /*!
                 * Return a PPD object containing some PPD info for
                 * the given cupsPrinter. Does not return the
                 * ppd-name, which is generated internally in the
                 * cupsd, and which is not returned by this method.
                 */
                wml::Ppd getPPD (const std::string& cupsPrinter) const;

                /*!
                 * Set the provided PPD file for the given cupsPrinter.
                 */
                void setPPDFromFile (const std::string& cupsPrinter, const std::string& sourcePPDFile);

                /*!
                 * Get the PPD file stored in ppd-name on the cupsd
                 * for this queue and then obtain its nickname.
                 */
                std::string getPPDNickname (const std::string& cupsPrinter) const;

                /*!
                 * Obtain a list of the printer "makes" for which
                 * there are available PPD files stored on the
                 * system. This makes a CUPS_GET_PPDS IPP request to
                 * the cupsd to get the information.
                 */
                std::vector<std::string> getPPDListOfMakes (void) const;

                /*!
                 * Get the list of PPD drivers (models) for the given
                 * make. This makes a CUPS_GET_PPDS IPP request to the
                 * cupsd to get the information.
                 */
                std::vector<wml::Ppd> getPPDListOfModels (const std::string& make) const;

                /*!
                 * Obtain the PPD options for the given printer.
                 */
                std::vector<wml::PpdOption> getPPDOptions (const std::string& cupsPrinter) const;

                /*!
                 * Set the PPD option identified by keyword to value.
                 */
                void setPPDOption (const std::string& cupsPrinter,
                                   const std::string& keyword,
                                   const std::string& value);

                /*!
                 * This checks that the printer doesn't exist, then
                 * calls setDeviceURI() to create it.
                 */
                void addPrinter (const std::string& cupsPrinter, const std::string& devURI);

                /*!
                 * add printer, with device URI, and at the same time,
                 * set the PPD file.
                 */
                void addPrinter (const std::string& cupsPrinter, const std::string& s, const std::string& sourcePPD);

                /*!
                 * Delete the printer.
                 */
                void deletePrinter (const std::string& cupsPrinter);

                /*!
                 * Return the "cups" URI for the queue. Something
                 * like: ipp://circle.wml:631/printers/hp3005
                 */
                std::string getCupsURI (const std::string& cupsPrinter) const;

                /*!
                 * Get a list of printer on the cupsd. Return as vector.
                 */
                std::vector<std::string> getCupsPrinterList (wml::GET_PRINTERS getPrinters) const;

                /*!
                 * Wrapper around getCupsPrinterList. Returns a vector
                 * containing all classes and printers
                 */
                std::vector<std::string> getPrinterClassesList (void) const;

                /*!
                 * Wrapper around getCupsPrinterList. Returns a vector
                 * containing only printers
                 */
                std::vector<std::string> getPrinterList (void) const;

                /*!
                 * Wrapper around getCupsPrinterList. Returns a vector
                 * containing only classes
                 */
                std::vector<std::string> getClassesList (void) const;

                /*!
                 * Determines whether the queue is a printer or a class
                 * and returns the result as a string.
                 */
                wml::QTYPE getQueueType (const std::string& queuename) const;

                /*!
                 * Request that a job is created on the cupsd. Return
                 * the job id.
                 */
                int createJob (const std::string& cupsQueue,
                               const std::string& title="",
                               const std::string& asUser="",
                               const std::string& docName="",
                               const int numOptions = 0,
                               cups_option_t* options = (cups_option_t*)0) const;

                /*!
                 * As the user asUser, this IPP_HOLD_JOBs jobId
                 */
                void holdJob (const int jobId, const std::string& asUser) const;

                /*!
                 * As the user asUser, this IPP_RELEASE_JOBs jobId
                 */
                void releaseJob (const int jobId, const std::string& asUser) const;

                /*!
                 * As the user asUser, this IPP_CANCEL_JOBs jobId
                 */
                void cancelJob (const int jobId, const std::string& asUser) const;

                /*!
                 * As the user asUser, this IPP_RESTART_JOBs jobId
                 */
                void restartJob (const int jobId, const std::string& asUser) const;

                /*!
                 * As the user asUser, move the job jobId to the queue
                 * destQueue.
                 */
                void moveJob (const int jobId, const std::string& asUser, const std::string& destQueue) const;

                /*!
                 * Send a file specified by filePath to the cupsd,
                 * adding it to the job with id jobId. Optionally set
                 * the document name and the format string. If
                 * lastInSet is false, the doc will not be printed
                 * immediately.
                 */
                void sendDocument (const int jobId,
                                   const std::string& filePath,
                                   const std::string& asUser,
                                   const std::string& docName = "",
                                   const std::string& format = "",
                                   const bool lastInSet = true) const;
                /*!
                 * Print the file at filePath to the CUPS queue
                 * cupsQueue. Throw exceptions if the file does not
                 * exist, or if the cupsd refuses to print it. Return
                 * the job id.
                 */
                int printFile (const std::string& filePath,
                               const std::string& jobTitle,
                               const std::string& cupsQueue,
                               const std::string& asUser = "") const;

                /*!
                 * Like getCupsPrinterList but returns a list rather
                 * than a vector.
                 */
                std::list<std::string> getCupsPrinterList2 (void) const;

                /*!
                 * Get the value of the attribute attr for the printer
                 * printerName. attr should have been configured with
                 * name before being passed to this function.
                 *
                 * attr has its value set, and when done,
                 * attr.getString() is returned by this function.
                 */
                std::string getPrinterAttribute (const char* printerName,
                                                 wml::IppAttr& attr) const;

                /*!
                 * Similar to getPrinterAttribute, but returns a vector
                 * of strings, instead of a single string. Useful when
                 * an attribute contains more than one value, such as
                 * the member-uris attribute of a class.
                 */
                std::vector<std::string> getQueueAttribute (const char* printerName,
                                                            wml::IppAttr& attr) const;

                /*!
                 * Set an IPP attribute for printerName. attr should
                 * have been set up with the attribute name and the
                 * attribute value.
                 */
                void setPrinterAttribute (const char* printerName,
                                          const wml::IppAttr& attr);

                /*!
                 * Set two IPP attributes for printerName. attrs
                 * should have been set up with the attribute name and
                 * the attribute value.
                 */
                void setPrinterAttributes (const char* printerName,
                                           const wml::IppAttr& attr1,
                                           const wml::IppAttr& attr2);

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
                void setClassMembers (const std::string& className,
                                      std::vector<std::string> members);

                /*!
                 * Send a command for the queue, such as
                 * CUPS_ACCEPT_JOBS, CUPS_REJECT_JOBS,
                 * IPP_PAUSE_PRINTER or IPP_RESUME_PRINTER.
                 */
                void sendPrinterCommand (const char* printerName,
                                         const std::string& asUser,
                                         const std::string& reason,
                                         const ipp_op_t command,
                                         const std::string& directory = "/printers/%s") const;

                /*!
                 * Return true if the string s is a name which is
                 * valid to be a CUPS printer name. Note that this is
                 * more restrictive about which characters are
                 * permitted in the printer name than is CUPS itself,
                 * limiting the allowed characters to numerals,
                 * letters of the alphabet (upper and lower cases),
                 * the underscore character and the dash character.
                 */
                static bool printerNameIsValid (const std::string& s);

                /*!
                 * Return true if the string s is a name which is
                 * valid to be a CUPS device address.
                 */
                static bool addrIsValid (const std::string& s);

                /*!
                 * Return true if the string s is a name which is
                 * valid to be an LPD destination queue. As with
                 * printerNameIsValid, this is more retrictive than
                 * the LPD protocol itself (as described in RFC
                 * 1179). The same set of characters are allowed as
                 * for CupsCtrl::printerNameIsValid.
                 */
                static bool lpdqIsValid (const std::string& s);

                /*!
                 * Return true if the string s describes a port which
                 * is valid to be a raw port for TCP/IP data
                 * transfer. Currently, this simply ensures that the
                 * string is a set of numbers. Range checking of the
                 * port value is left to client code.
                 */
                static bool portIsValid (const std::string& s);

                /*!
                 * Return true if the string s is a valid to be a CUPS
                 * job title.
                 */
                static bool titleIsValid (const std::string& s);

                /*!
                 * Same effect as cupsLastErrorString(), except that
                 * cupsLastErrorString doesn't seem to work.
                 */
                std::string errorString (const ipp_status_t err) const;

                /*!
                 * Gets the string equivalent of the http_status_t
                 * status/error.
                 */
                std::string errorString (const http_status_t err) const;

                /*!
                 * General function for sending job commands (such
                 * as cancel, etc) to the Cups daemon, to be applied
                 * to the job with the id provided
                 */
                void sendJobCommand (const int jobId,
                                     const std::string& asUser,
                                     const ipp_op_t command) const;

                /*!
                 * Cancels all jobs on the queue with the given
                 * printer name
                 */
                void cancelJobs (const std::string& printerName) const;

                /*!
                 * Pauses all jobs on the queue with the given
                 * printer name
                 */
                void pauseJobs (const std::string& printerName) const;

                /*!
                 * Resumes all jobs on the queue with the given
                 * printer name
                 */
                void releaseJobs (const std::string& printerName) const;

        private:
                /*!
                 * Used by setPPDOption. Intended to do the same job
                 * as cupsSetDests2(), but it doesn't bother to try
                 * writing options to $HOME/.cups/lpoptions - this is
                 * for setting system-wide options only.
                 */
                void writeLpOptions (const int ndests, cups_dest_t * dests) const;

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
