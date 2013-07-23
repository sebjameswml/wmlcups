/* -*-c++-*- */
/*!
 * Queue status information container class.
 *
 * Author: Sebastian James <sjames@wmltd.co.uk>
 *
 * Copyright: 2009 William Matthew Limited.
 */

#ifndef _CUPSJOB_H_
#define _CUPSJOB_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <string>
#include <cups/ipp.h>
#include "config.h"
#include "CupsJob.h"

namespace wml {

        /*!
         * Collection of status information for a Job, plus some job
         * control code.
         */
        class CupsJob
        {
        public:
                CupsJob (void);
                CupsJob (const std::string& jobName);
                CupsJob (const int jobId);
                CupsJob (const int jobId, const std::string& jobName);
                ~CupsJob (void);

                /*!
                 * Reset all the attributes of this CupsJob object to
                 * defaults.
                 */
                void reset (void);

                /*!
                 * Simple accessors
                 */
                //@{
                void setId (const int i);
                int getId (void) const;

                void setCopies (const int i);
                int getCopies (void) const;

                void setName (const std::string& s);
                std::string getName (void) const;

                void setUser (const std::string& s);
                std::string getUser (void) const;

                void setSizeKB (const unsigned int i);
                unsigned int getSizeKB (void) const;

                std::string getSizeString (void) const;

                void setCupsPages (const unsigned int i);
                unsigned int getCupsPages (void) const;

                void setPrinterUri (const std::string& s);
                std::string getPrinterUri (void) const;

                void setState (const ipp_jstate_t jstate);
                std::string getState (void) const;

                void setCreationTime (const int t);
                int getCreationTime (void) const;
                void setProcessingTime (const int t);
                int getProcessingTime (void) const;
                void setCompletedTime (const int t);
                int getCompletedTime (void) const;

                void setCurrentFilter (std::string s);
                std::string getCurrentFilter (void) const;

                int getTime (void) const;
                std::string getFormattedTime (void) const;

                void setSourceIP (const std::string& ip);
                std::string getSourceIP (void) const;

                void setDestIP (const std::string& ip);
                std::string getDestIP (void) const;
                //@}

        private:

                /*!
                 * The Job ID number
                 */
                int id;

                /*!
                 * Number of copies.
                 */
                int copies;

                /*!
                 * The user-supplied job name
                 */
                std::string name;

                /*!
                 * The printing user.
                 */
                std::string user;

                /*!
                 * Job size in kBytes
                 */
                unsigned int sizeKB;

                /*!
                 * Job number of pages, according to CUPS
                 */
                unsigned int cupsPages;

                /*!
                 * The queue on which the job was/is to be printed.
                 */
                std::string printerUri;

                /*!
                 * Job state
                 */
                ipp_jstate_t state;

                /*!
                 * CUPS time-at-creation
                 */
                int creation_time;

                /*!
                 * CUPS time-at-processing
                 */
                int processing_time;

                /*!
                 * CUPS time-at-completed
                 */
                int completed_time;

                /*!
                 * WML addition. The current working filter
                 * program. This is used when building status or error
                 * PDF output pages.
                 */
                std::string currentFilter;

                /*!
                 * The IP address of the sending device - the IP
                 * address of the computer which sent data to this
                 * device.
                 */
                std::string sourceIP;

                /*!
                 * The IP address to which the print job was sent. If
                 * the WMLPP system has aliased IP addresses, this is
                 * the IP address selected.
                 */
                std::string destIP;
        };

} // namespace wml

#endif // _CUPSJOB_H_

