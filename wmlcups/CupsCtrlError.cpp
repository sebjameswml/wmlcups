#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"
#include <futil/WmlDbg.h>

extern "C" {
#include <errno.h>
#include <cups/cups.h>
#include <cups/language.h>
}

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <utility>
#include <set>

#include <futil/FoundryUtilities.h>
#include "QueueCupsStatus.h"
#include "IppAttr.h"
#include "CupsCtrl.h"

using namespace std;
using namespace wml;

bool
wml::CupsCtrl::printerNameIsValid (const string& s)
{
        if (s.size() > 127) {
                return false;
        }
        try {
                string tmp (s);
                FoundryUtilities::sanitize (tmp, WMLCUPS_QUEUENAME_SAFE_CHARS);
        } catch (const exception& e) {
                return false;
        }
        return true;
}

bool
wml::CupsCtrl::addrIsValid (const string& s)
{
        if (s.size() > 127) {
                return false;
        }
        try {
                string tmp (s);
                FoundryUtilities::sanitize (tmp, CUPS_ADDRESS_SAFE_CHARS);
        } catch (const exception& e) {
                return false;
        }
        return true;
}

bool
wml::CupsCtrl::lpdqIsValid (const string& s)
{
        if (s.size() > 127) {
                return false;
        }
        try {
                // Allow same characters in LPD queue name as allowed
                // in IPP printer name. That means
                // CHARS_NUMERIC_ALPHA, '_' and
                // '-'. WMLCUPS_QUEUENAME_SAFE_CHARS is defined in
                // futil/FoundryUtilities.h.
                string tmp (s);
                FoundryUtilities::sanitize (tmp, WMLCUPS_QUEUENAME_SAFE_CHARS);
        } catch (const exception& e) {
                return false;
        }
        return true;
}

bool
wml::CupsCtrl::portIsValid (const string& s)
{
        if (s.size() > 127) {
                return false;
        }
        try {
                string tmp (s);
                FoundryUtilities::sanitize (tmp, CHARS_NUMERIC);
        } catch (const exception& e) {
                return false;
        }
        return true;
}

bool
wml::CupsCtrl::titleIsValid (const string& s)
{
        if (s.size() > 127) {
                return false;
        }
        try {
                string tmp (s);
                FoundryUtilities::sanitize (tmp, WMLCUPS_TITLE_SAFE_CHARS);
        } catch (const exception& e) {
                return false;
        }
        return true;
}

string
wml::CupsCtrl::errorString (const http_status_t err) const
{
        string errStr("Unknown");
        switch (err) {
        case HTTP_ERROR:
                errStr = "HTTP_ERROR";
                break;
        case HTTP_CONTINUE:
                errStr = "ok, continue";
                break;
        case HTTP_SWITCHING_PROTOCOLS:
                errStr = "Upgrading to TLS/SSL connection";
                break;
        case HTTP_OK:
                errStr = "successful-ok";
                break;
        case HTTP_CREATED:
                errStr = "put cmd successful";
                break;
        case HTTP_ACCEPTED:
                errStr = "delete cmd successful";
                break;
        case HTTP_NOT_AUTHORITATIVE:
                errStr = "Information isn't authoritative";
                break;
        case HTTP_NO_CONTENT:
                errStr = "Successful command, no new data";
                break;
        case HTTP_RESET_CONTENT:
                errStr = "Content was reset/recreated";
                break;
        case HTTP_SERVICE_UNAVAILABLE:
                errStr = "Service unavailable";
                break;
        case HTTP_UNAUTHORIZED:
                errStr = "Unauthorized to access the host";
                break;
#if CUPS_VERSION_MINOR > 3
        case HTTP_AUTHORIZATION_CANCELED:
                errStr = "User cancelled authorization";
                break;
#endif
        case HTTP_UPGRADE_REQUIRED:
                errStr = "Upgrade to SSL/TLS required";
                break;

                // Cases I've not yet typed out:
        case HTTP_PARTIAL_CONTENT:                        /* Only a partial file was recieved/sent */
        case HTTP_MULTIPLE_CHOICES:                /* Multiple files match request */
        case HTTP_MOVED_PERMANENTLY:                /* Document has moved permanently */
        case HTTP_MOVED_TEMPORARILY:                /* Document has moved temporarily */
        case HTTP_SEE_OTHER:                        /* See this other link... */
        case HTTP_NOT_MODIFIED:                        /* File not modified */
        case HTTP_USE_PROXY:                        /* Must use a proxy to access this URI */
        case HTTP_BAD_REQUEST:                /* Bad request */
        case HTTP_PAYMENT_REQUIRED:                /* Payment required */
        case HTTP_FORBIDDEN:                        /* Forbidden to access this URI */
        case HTTP_NOT_FOUND:                        /* URI was not found */
        case HTTP_METHOD_NOT_ALLOWED:                /* Method is not allowed */
        case HTTP_NOT_ACCEPTABLE:                        /* Not Acceptable */
        case HTTP_PROXY_AUTHENTICATION:                /* Proxy Authentication is Required */
        case HTTP_REQUEST_TIMEOUT:                        /* Request timed out */
        case HTTP_CONFLICT:                        /* Request is self-conflicting */
        case HTTP_GONE:                                /* Server has gone away */
        case HTTP_LENGTH_REQUIRED:                        /* A content length or encoding is required */
        case HTTP_PRECONDITION:                        /* Precondition failed */
        case HTTP_REQUEST_TOO_LARGE:                /* Request entity too large */
        case HTTP_URI_TOO_LONG:                        /* URI too long */
        case HTTP_UNSUPPORTED_MEDIATYPE:                /* The requested media type is unsupported */
        case HTTP_REQUESTED_RANGE:                        /* The requested range is not satisfiable */
        case HTTP_EXPECTATION_FAILED:                /* The expectation given in an Expect header field was not met */
        case HTTP_SERVER_ERROR:                /* Internal server error */
        case HTTP_NOT_IMPLEMENTED:                        /* Feature not implemented */
        case HTTP_BAD_GATEWAY:                        /* Bad gateway */
        case HTTP_GATEWAY_TIMEOUT:                        /* Gateway connection timed out */
        case HTTP_NOT_SUPPORTED:                        /* HTTP version not supported */
        default:
                break;
        }

        return errStr;
}

string
wml::CupsCtrl::errorString (const ipp_status_t err) const
{
        string errStr("Unknown");
        switch (err) {

        case IPP_OK:
                errStr = "successful-ok";
                break;

        case IPP_OK_SUBST:
                errStr = "successful-ok-ignored-or-substituted-attributes";
                break;

        case IPP_OK_CONFLICT:
                errStr = "successful-ok-conflicting-attributes";
                break;

        case IPP_OK_IGNORED_SUBSCRIPTIONS:
                errStr = "successful-ok-ignored-subscriptions";
                break;

        case IPP_OK_IGNORED_NOTIFICATIONS:
                errStr = "successful-ok-ignored-notifications";
                break;

        case IPP_OK_TOO_MANY_EVENTS:
                errStr = "successful-ok-too-many-events";
                break;

        case IPP_OK_BUT_CANCEL_SUBSCRIPTION:
                errStr = "successful-ok-but-cancel-subscription";
                break;

        case IPP_OK_EVENTS_COMPLETE:
                errStr = "successful-ok-events-complete";
                break;

        case IPP_REDIRECTION_OTHER_SITE:
                errStr = "redirection-other-site @private@";
                break;

        case CUPS_SEE_OTHER:
                errStr = "cups-see-other";
                break;

        case IPP_BAD_REQUEST:
                errStr = "client-error-bad-request";
                break;

        case IPP_FORBIDDEN:
                errStr = "client-error-forbidden";
                break;

        case IPP_NOT_AUTHENTICATED:
                errStr = "client-error-not-authenticated";
                break;

        case IPP_NOT_AUTHORIZED:
                errStr = "client-error-not-authorized";
                break;

        case IPP_NOT_POSSIBLE:
                errStr = "client-error-not-possible";
                break;

        case IPP_TIMEOUT:
                errStr = "client-error-timeout";
                break;

        case IPP_NOT_FOUND:
                errStr = "client-error-not-found";
                break;

        case IPP_GONE:
                errStr = "client-error-gone";
                break;

        case IPP_REQUEST_ENTITY:
                errStr = "client-error-request-entity-too-large";
                break;

        case IPP_REQUEST_VALUE:
                errStr = "client-error-request-value-too-long";
                break;

        case IPP_DOCUMENT_FORMAT:
                errStr = "client-error-document-format-not-supported";
                break;

        case IPP_ATTRIBUTES:
                errStr = "client-error-attributes-or-values-not-supported";
                break;

        case IPP_URI_SCHEME:
                errStr = "client-error-uri-scheme-not-supported";
                break;

        case IPP_CHARSET:
                errStr = "client-error-charset-not-supported";
                break;

        case IPP_CONFLICT:
                errStr = "client-error-conflicting-attributes";
                break;

        case IPP_COMPRESSION_NOT_SUPPORTED:
                errStr = "client-error-compression-not-supported";
                break;

        case IPP_COMPRESSION_ERROR:
                errStr = "client-error-compression-error";
                break;

        case IPP_DOCUMENT_FORMAT_ERROR:
                errStr = "client-error-document-format-error";
                break;

        case IPP_DOCUMENT_ACCESS_ERROR:
                errStr = "client-error-document-access-error";
                break;

        case IPP_ATTRIBUTES_NOT_SETTABLE:
                errStr = "client-error-attributes-not-settable";
                break;

        case IPP_IGNORED_ALL_SUBSCRIPTIONS:
                errStr = "client-error-ignored-all-subscriptions";
                break;

        case IPP_TOO_MANY_SUBSCRIPTIONS:
                errStr = "client-error-too-many-subscriptions";
                break;

        case IPP_IGNORED_ALL_NOTIFICATIONS:
                errStr = "client-error-ignored-all-notifications";
                break;

        case IPP_PRINT_SUPPORT_FILE_NOT_FOUND:
                errStr = "client-error-print-support-file-not-found";
                break;

        case IPP_INTERNAL_ERROR:
                errStr = "server-error-internal-error";
                break;

        case IPP_OPERATION_NOT_SUPPORTED:
                errStr = "server-error-operation-not-supported";
                break;

        case IPP_SERVICE_UNAVAILABLE:
                errStr = "server-error-service-unavailable";
                break;

        case IPP_VERSION_NOT_SUPPORTED:
                errStr = "server-error-version-not-supported";
                break;

        case IPP_DEVICE_ERROR:
                errStr = "server-error-device-error";
                break;

        case IPP_TEMPORARY_ERROR:
                errStr = "server-error-temporary-error";
                break;

        case IPP_NOT_ACCEPTING:
                errStr = "server-error-not-accepting-jobs";
                break;

        case IPP_PRINTER_BUSY:
                errStr = "server-error-busy";
                break;

        case IPP_ERROR_JOB_CANCELED:
                errStr = "server-error-job-canceled";
                break;

        case IPP_MULTIPLE_JOBS_NOT_SUPPORTED:
                errStr = "server-error-multiple-document-jobs-not-supported";
                break;

        case IPP_PRINTER_IS_DEACTIVATED:
                errStr = "server-error-printer-is-deactivated";
                break;

        default:
                break;
        }

        return errStr;
}
