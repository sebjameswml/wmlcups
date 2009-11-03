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

#ifndef _IPPATTR_H_
#define _IPPATTR_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <utility>

extern "C" {
#include <cups/cups.h>
#include <cups/language.h>
}

#include "config.h"

#ifdef A_COPY_OF_IPP_TAGS
typedef enum ipp_tag_e			/**** Format tags for attributes... ****/
{
  IPP_TAG_ZERO = 0x00,			/* Zero tag - used for separators */
  IPP_TAG_OPERATION,			/* Operation group */
  IPP_TAG_JOB,				/* Job group */
  IPP_TAG_END,				/* End-of-attributes */
  IPP_TAG_PRINTER,			/* Printer group */
  IPP_TAG_UNSUPPORTED_GROUP,		/* Unsupported attributes group */
  IPP_TAG_SUBSCRIPTION,			/* Subscription group */
  IPP_TAG_EVENT_NOTIFICATION,		/* Event group */
  IPP_TAG_UNSUPPORTED_VALUE = 0x10,	/* Unsupported value */
  IPP_TAG_DEFAULT,			/* Default value */
  IPP_TAG_UNKNOWN,			/* Unknown value */
  IPP_TAG_NOVALUE,			/* No-value value */
  IPP_TAG_NOTSETTABLE = 0x15,		/* Not-settable value */
  IPP_TAG_DELETEATTR,			/* Delete-attribute value */
  IPP_TAG_ADMINDEFINE,			/* Admin-defined value */
  IPP_TAG_INTEGER = 0x21,		/* Integer value */
  IPP_TAG_BOOLEAN,			/* Boolean value */
  IPP_TAG_ENUM,				/* Enumeration value */
  IPP_TAG_STRING = 0x30,		/* Octet string value */
  IPP_TAG_DATE,				/* Date/time value */
  IPP_TAG_RESOLUTION,			/* Resolution value */
  IPP_TAG_RANGE,			/* Range value */
  IPP_TAG_BEGIN_COLLECTION,		/* Beginning of collection value */
  IPP_TAG_TEXTLANG,			/* Text-with-language value */
  IPP_TAG_NAMELANG,			/* Name-with-language value */
  IPP_TAG_END_COLLECTION,		/* End of collection value */
  IPP_TAG_TEXT = 0x41,			/* Text value */
  IPP_TAG_NAME,				/* Name value */
  IPP_TAG_KEYWORD = 0x44,		/* Keyword value */
  IPP_TAG_URI,				/* URI value */
  IPP_TAG_URISCHEME,			/* URI scheme value */
  IPP_TAG_CHARSET,			/* Character set value */
  IPP_TAG_LANGUAGE,			/* Language value */
  IPP_TAG_MIMETYPE,			/* MIME media type value */
  IPP_TAG_MEMBERNAME,			/* Collection member name value */
  IPP_TAG_MASK = 0x7fffffff,		/* Mask for copied attribute values */
  IPP_TAG_COPY = -0x7fffffff-1		/* Bitflag for copied attribute values */
} ipp_tag_t;
#endif

namespace wml {

	/*!
	 * An IPP Attribute. Has name, type, value.
	 *
	 * Note: I'm not planning on including a method which knows
	 * how to query the cupsd to fill the value. This is because
	 * we may write methods in WmlCups which fill 10 IppAttr
	 * objects with a single connection to the cupsd.
	 */
	class IppAttr
	{
	public:
		IppAttr ();
		IppAttr (const char* attributeName);
		~IppAttr ();

		/*!
		 * Zero out any stored values.
		 */
		void zero (void);

		/*!
		 * Return the name of this attribute.
		 */
		std::string getName (void);

		/*!
		 * Set the attribute name to s.
		 */
		void setName (std::string s);

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
		 * Set stringValue to string pointed at by c.
		 */
		void setValue (const char* c);

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

#endif // _IPPATTR_H_

