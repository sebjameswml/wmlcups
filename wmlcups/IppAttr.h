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

#ifndef _IPPATTR_H_
#define _IPPATTR_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <utility>
#include <string>

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

// A copy of _ipp_option_t from cups-1.4.x/cups/ipp-private.h
#define A_COPY_OF_IPP_OPTION_T 1 // required
#ifdef A_COPY_OF_IPP_OPTION_T
typedef struct				/**** Attribute mapping data ****/
{
  int		multivalue;		/* Option has multiple values? */
  const char	*name;			/* Option/attribute name */
  ipp_tag_t	value_tag;		/* Value tag for this attribute */
  ipp_tag_t	group_tag;		/* Group tag for this attribute */
} _ipp_option_t;
#endif

// This is a copy of ipp_options from cups-1.4.x/cups/encode.c. It
// shows how different attribute names map to value_tags and
// group_tags.
#define A_COPY_OF_IPP_OPTIONS  1 // required
#ifdef A_COPY_OF_IPP_OPTIONS
/*
 * Local list of option names and the value tags they should use...
 *
 * **** THIS LIST MUST BE SORTED ****
 */
static const _ipp_option_t ipp_options[] =
{
  { 1, "auth-info",             IPP_TAG_TEXT,           IPP_TAG_JOB },
  { 1, "auth-info-required",    IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 0, "blackplot",             IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 0, "blackplot-default",     IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "brightness",            IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "brightness-default",    IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "columns",               IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "columns-default",       IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "compression",           IPP_TAG_KEYWORD,        IPP_TAG_OPERATION },
  { 0, "copies",                IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "copies-default",        IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "device-uri",            IPP_TAG_URI,            IPP_TAG_PRINTER },
  { 0, "document-format",       IPP_TAG_MIMETYPE,       IPP_TAG_OPERATION },
  { 0, "document-format-default", IPP_TAG_MIMETYPE,     IPP_TAG_PRINTER },
  { 1, "exclude-schemes",       IPP_TAG_NAME,           IPP_TAG_OPERATION },
  { 1, "finishings",            IPP_TAG_ENUM,           IPP_TAG_JOB },
  { 1, "finishings-default",    IPP_TAG_ENUM,           IPP_TAG_PRINTER },
  { 0, "fit-to-page",           IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 0, "fit-to-page-default",   IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "fitplot",               IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 0, "fitplot-default",       IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "gamma",                 IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "gamma-default",         IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "hue",                   IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "hue-default",           IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "include-schemes",       IPP_TAG_NAME,           IPP_TAG_OPERATION },
  // job-hold-until added by WML as this is a job attribute tag
  { 0, "job-hold-until",        IPP_TAG_NAME,           IPP_TAG_JOB },
  { 0, "job-impressions",       IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "job-k-limit",           IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "job-page-limit",        IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "job-priority",          IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "job-quota-period",      IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "job-sheets",            IPP_TAG_NAME,           IPP_TAG_JOB },
  { 1, "job-sheets-default",    IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 0, "job-uuid",              IPP_TAG_URI,            IPP_TAG_JOB },
  { 0, "landscape",             IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 1, "marker-change-time",    IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "marker-colors",         IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 1, "marker-high-levels",    IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "marker-levels",         IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "marker-low-levels",     IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "marker-message",        IPP_TAG_TEXT,           IPP_TAG_PRINTER },
  { 1, "marker-names",          IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 1, "marker-types",          IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 1, "media",                 IPP_TAG_KEYWORD,        IPP_TAG_JOB },
  { 0, "media-col",             IPP_TAG_BEGIN_COLLECTION, IPP_TAG_JOB },
  { 0, "media-col-default",     IPP_TAG_BEGIN_COLLECTION, IPP_TAG_PRINTER },
  { 0, "media-color",           IPP_TAG_KEYWORD,        IPP_TAG_JOB },
  { 1, "media-default",         IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 0, "media-key",             IPP_TAG_KEYWORD,        IPP_TAG_JOB },
  { 0, "media-size",            IPP_TAG_BEGIN_COLLECTION, IPP_TAG_JOB },
  { 0, "media-type",            IPP_TAG_KEYWORD,        IPP_TAG_JOB },
  // member-uris added by WML
  { 0, "member-uris",           IPP_TAG_URI,            IPP_TAG_PRINTER },
  { 0, "mirror",                IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 0, "mirror-default",        IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "natural-scaling",       IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "natural-scaling-default", IPP_TAG_INTEGER,      IPP_TAG_PRINTER },
  { 0, "notify-charset",        IPP_TAG_CHARSET,        IPP_TAG_SUBSCRIPTION },
  { 1, "notify-events",         IPP_TAG_KEYWORD,        IPP_TAG_SUBSCRIPTION },
  { 1, "notify-events-default", IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 0, "notify-lease-duration", IPP_TAG_INTEGER,        IPP_TAG_SUBSCRIPTION },
  { 0, "notify-lease-duration-default", IPP_TAG_INTEGER, IPP_TAG_PRINTER },
  { 0, "notify-natural-language", IPP_TAG_LANGUAGE,     IPP_TAG_SUBSCRIPTION },
  { 0, "notify-pull-method",    IPP_TAG_KEYWORD,        IPP_TAG_SUBSCRIPTION },
  { 0, "notify-recipient-uri",  IPP_TAG_URI,            IPP_TAG_SUBSCRIPTION },
  { 0, "notify-time-interval",  IPP_TAG_INTEGER,        IPP_TAG_SUBSCRIPTION },
  { 0, "notify-user-data",      IPP_TAG_STRING,         IPP_TAG_SUBSCRIPTION },
  { 0, "number-up",             IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "number-up-default",     IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "orientation-requested", IPP_TAG_ENUM,           IPP_TAG_JOB },
  { 0, "orientation-requested-default", IPP_TAG_ENUM,   IPP_TAG_PRINTER },
  { 0, "page-bottom",           IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "page-bottom-default",   IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "page-left",             IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "page-left-default",     IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "page-ranges",           IPP_TAG_RANGE,          IPP_TAG_JOB },
  { 1, "page-ranges-default",   IPP_TAG_RANGE,          IPP_TAG_PRINTER },
  { 0, "page-right",            IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "page-right-default",    IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "page-top",              IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "page-top-default",      IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "penwidth",              IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "penwidth-default",      IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "port-monitor",          IPP_TAG_NAME,           IPP_TAG_PRINTER },
  // ppd-name added by WML
  { 0, "ppd-name",              IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 0, "ppi",                   IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "ppi-default",           IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "prettyprint",           IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 0, "prettyprint-default",   IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "print-quality",         IPP_TAG_ENUM,           IPP_TAG_JOB },
  { 0, "print-quality-default", IPP_TAG_ENUM,           IPP_TAG_PRINTER },
  { 1, "printer-commands",      IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 0, "printer-error-policy",  IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 0, "printer-info",          IPP_TAG_TEXT,           IPP_TAG_PRINTER },
  { 0, "printer-is-accepting-jobs", IPP_TAG_BOOLEAN,    IPP_TAG_PRINTER },
  { 0, "printer-is-shared",     IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "printer-location",      IPP_TAG_TEXT,           IPP_TAG_PRINTER },
  { 0, "printer-make-and-model",IPP_TAG_TEXT,           IPP_TAG_PRINTER },
  { 0, "printer-more-info",     IPP_TAG_URI,            IPP_TAG_PRINTER },
  // printer-name added by WML as this is a printer attribute tag
  { 0, "printer-name",          IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 0, "printer-op-policy",     IPP_TAG_NAME,           IPP_TAG_PRINTER },
  { 0, "printer-resolution",    IPP_TAG_RESOLUTION,     IPP_TAG_JOB },
  { 0, "printer-state",         IPP_TAG_ENUM,           IPP_TAG_PRINTER },
  { 0, "printer-state-change-time", IPP_TAG_INTEGER,    IPP_TAG_PRINTER },
  // printer-state-time added by WML
  { 0, "printer-state-time",    IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 1, "printer-state-reasons", IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 0, "printer-type",          IPP_TAG_ENUM,           IPP_TAG_PRINTER },
  { 0, "printer-uri",           IPP_TAG_URI,            IPP_TAG_OPERATION },
  { 1, "printer-uri-supported", IPP_TAG_URI,            IPP_TAG_PRINTER },
  { 0, "queued-job-count",      IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "raw",                   IPP_TAG_MIMETYPE,       IPP_TAG_OPERATION },
  { 1, "requested-attributes",  IPP_TAG_NAME,           IPP_TAG_OPERATION },
  { 1, "requesting-user-name-allowed", IPP_TAG_NAME,    IPP_TAG_PRINTER },
  { 1, "requesting-user-name-denied", IPP_TAG_NAME,     IPP_TAG_PRINTER },
  { 0, "resolution",            IPP_TAG_RESOLUTION,     IPP_TAG_JOB },
  { 0, "resolution-default",    IPP_TAG_RESOLUTION,     IPP_TAG_PRINTER },
  { 0, "saturation",            IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "saturation-default",    IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "scaling",               IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "scaling-default",       IPP_TAG_INTEGER,        IPP_TAG_PRINTER },
  { 0, "sides",                 IPP_TAG_KEYWORD,        IPP_TAG_JOB },
  { 0, "sides-default",         IPP_TAG_KEYWORD,        IPP_TAG_PRINTER },
  { 0, "wrap",                  IPP_TAG_BOOLEAN,        IPP_TAG_JOB },
  { 0, "wrap-default",          IPP_TAG_BOOLEAN,        IPP_TAG_PRINTER },
  { 0, "x-dimension",           IPP_TAG_INTEGER,        IPP_TAG_JOB },
  { 0, "y-dimension",           IPP_TAG_INTEGER,        IPP_TAG_JOB }
};
#endif

namespace wml {

        /*!
         * An IPP Attribute. Has name, type, value.
         *
         * Contains an IPP attribute used in transferring print data
         * over a network
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
                 * Return the type tag of this attribute. In cups (i.e
                 * in _ipp_option_t) this is really called the
                 * "value_tag"
                 */
                ipp_tag_t getType (void);
                //ipp_tag_t getValueTag (void);

                /*!
                 * Return the group tag of this attribute.
                 */
                ipp_tag_t getGroup (void);

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
                 * The attribute group, which can be determined by
                 * comparing name with a lookup-table. Values for
                 * this found in cups-1.x/cups/ipp.h or cups-1.x/cups/encode.c
                 */
                ipp_tag_t group;

                /*!
                 * If non zero, then this attribute may have multiple
                 * values (though this is not yet implemented - we
                 * don't have a vector or list of stringValue or a
                 * vector or list of intValue).
                 */
                int multivalue;

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

