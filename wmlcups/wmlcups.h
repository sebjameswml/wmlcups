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
	 * passed back as exceptions.
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
		 * Return a string (in English) of the printer info.
		 */
		std::string getInfo (std::string cupsPrinter);

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
		 * Get the value of the attribute attr for the printer
		 * printerName. attr should have been configured with
		 * name before being passed to this function.
		 *
		 * attr has its value set, and when done,
		 * attr.getString() is returned by this function.
		 */
		std::string getPrinterAttribute (const char* printerName,
						 wml::WmlIppAttr& attr);
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
