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

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"
#include "WmlIppAttr.h"

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
		 * Query cupsd for whether the given cupsPrinter is
		 * enabled
		 */
		bool getEnabled (std::string cupsPrinter);

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

		/*!
		 * Return a string (in English) of the printer info.
		 */
		std::string getInfo (std::string cupsPrinter);

		/*!
		 * Set the info Cups parameter to s.
		 */
		void setInfo (std::string cupsPrinter, std::string s);

		/*!
		 * Set the location Cups parameter to s.
		 */
		void setLocation (std::string cupsPrinter, std::string s);

		/*!
		 * Return a string (in English) of the printer
		 * location.
		 */
		std::string getLocation (std::string cupsPrinter);

		/*!
		 * Return a string (in English) of the printer make
		 * and model.
		 */
		std::string getMakeModel (std::string cupsPrinter);

		/*!
		 * Return the printer destination URI. Something like:
		 * socket://hp3005.wml:9100
		 */
		std::string getDeviceURI (std::string cupsPrinter);

		/*!
		 * Return the "cups" URI for the queue. Something
		 * like: ipp://circle.wml:631/printers/hp3005
		 */
		std::string getCupsURI (std::string cupsPrinter);

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
		 * Return true if the string s is a name which is
		 * valid to be a CUPS printer name.
		 */
		bool printerNameIsValid (std::string s);

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
