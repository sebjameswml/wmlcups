/* -*-c++-*- */
/*!
 * A C++ wrapper for some features of the CUPS API.
 *
 * This is the IPP attribute class.
 *
 * Author: Sebastian James <sjames@wmltd.co.uk>
 *
 * Copyright: 2009 William Matthew Limited.
 */

#ifndef _WMLIPPATTR_H_
#define _WMLIPPATTR_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <utility>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"

namespace wml {

	/*!
	 * An IPP Attribute. Has name, type, value.
	 *
	 * Note: I'm not planning on including a method which knows
	 * how to query the cupsd to fill the value. This is because
	 * we may write methods in WmlCups which fill 10 WmlIppAttr
	 * objects with a single connection to the cupsd.
	 */
	class WmlIppAttr
	{
	public:
		WmlIppAttr ();
		WmlIppAttr (const char* attributeName);
		~WmlIppAttr ();

		/*!
		 * Return the name of this attribute.
		 */
		std::string getName (void);

		/*!
		 * Return the type tag of this attribute.
		 */
		ipp_tag_t getType (void);

		/*!
		 * Get the string value of this IPP attribute, if
		 * applicable.
		 */
		std::string getString (void);

		/*!
		 * Get the integer value of this IPP attribute, if
		 * applicable.
		 */
		int getInt (void);

		/*!
		 * Set the stringValue to s.
		 */
		void setValue (std::string s);

		/*!
		 * Set the integer value to i.
		 */
		void setValue (int i);

	private:
		/*!
		 * Determine type from name.
		 */
		void determineType (void);

		/*!
		 * The attribute name, such as printer-name,
		 * printer-type, printer-info and so on.
		 */
		std::string name;

		/*!
		 * The attribute type, which can be determined by
		 * comparing name with a lookup-table. Values for
		 * this found in cups-1.x/cups/ipp.h
		 */
		ipp_tag_t type;

		/*!
		 * The string value of the attribute, if
		 * applicable. This is filled for IPP_TAG_TEXT,
		 * IPP_TAG_NAME etc types. If the type is
		 * IPP_TAG_INTEGER or IPP_TAG_ENUM, then the integer
		 * is formatted as a decimal and returned as a string.
		 */
		std::string stringValue;

		/*!
		 * Integer value, if applicable. Filled for
		 * IPP_TAG_INTEGER or IPP_TAG_ENUM types (not a
		 * complete list). 0 returned if type is
		 * IPP_TAG_TEXT or some other string value.
		 */
		int intValue;
	};

} // namespace wml

#endif // _WMLIPPATTR_H_

