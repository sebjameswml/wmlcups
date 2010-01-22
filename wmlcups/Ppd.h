/* -*-c++-*- */
/*!
 * A C++ wrapper for some features of the CUPS API.
 *
 * This is the PPD class.
 *
 * Author: Sebastian James <sjames@wmltd.co.uk>
 *
 * Copyright: 2010 William Matthew Limited.
 */

#ifndef _PPD_H_
#define _PPD_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <string>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"

namespace wml {

	/*!
	 * \class Ppd A PPD class to hold information about a PPD file.
	 *
	 * Example text fields which CUPS returns about a PPD file include:
	 *
	 * 'ppd-name'           lsb/usr/HP/hp-psc_2400_series-hpijs.ppd.gz
	 * 'ppd-make'           HP
	 * 'ppd-make-and-model' HP PSC 2400 Series hpijs, 3.9.10.72
	 * 'ppd-device-id'      MFG:HP;MDL:psc 2400 series;DES:psc 2400 series;
	 * 'ppd-product'        HP PSC 2405 Photosmart All-in-one Printer
	 * 'ppd-psversion'      (3010.000) 705
	 *
	 * Non-TEXT/NAME fields returned are:
	 * ppd-type,
	 * ppd-model-number,
	 * ppd-natural-language
	 */
	class Ppd
	{
	public:
		Ppd (void);
		~Ppd (void);

		/*!
		 * Setters
		 */
		//@{
		void setName (std::string s);
		void setMakeAndModel (std::string s);
		void setMake (std::string s);
		void setDeviceId (std::string s);
		void setProduct (std::string s);
		void setPsversion (std::string s);
		void setType (int i);
		void setModelNumber (int i);
		void setNaturalLanguage (std::string s);
		//@}

		/*!
		 * Getters
		 */
		//@{
		std::string getName (void);
		std::string getMakeAndModel (void);
		std::string getMake (void);
		std::string getDeviceId (void);
		std::string getProduct (void);
		std::string getPsversion (void);
		int getType (void);
		int getModelNumber (void);
		std::string getNaturalLanguage (void);
		//@}

	private:
		/*!
		 * The ppd-name, as returned by CUPS.
		 */
		std::string name;

		/*!
		 * The ppd-make, as returned by CUPS.
		 */
		std::string make;

		/*!
		 * The ppd-make-and-model, as returned by CUPS.
		 */
		std::string makeAndModel;

		/*!
		 * The ppd-device-id, as returned by CUPS.
		 */
		std::string deviceId;

		/*!
		 * The ppd-product, as returned by CUPS.
		 */
		std::string product;

		/*!
		 * The ppd-psversion, as returned by CUPS.
		 */
		std::string psversion;

		/*!
		 * The ppd-type, as returned by CUPS.
		 */
		int type;

		/*!
		 * The ppd-model-number, as returned by CUPS.
		 */
		int modelNumber;

		/*!
		 * The ppd-natural-language, as returned by CUPS.
		 */
		std::string naturalLanguage;
	};

} // namespace wml

#endif // _PPD_H_
