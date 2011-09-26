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
                CupsJob (std::string jobName);
                CupsJob (int jobId);
                CupsJob (int jobId, std::string jobName);
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
                void setId (int i);
                int getId (void);

                void setCopies (int i);
                int getCopies (void);

                void setName (std::string s);
                std::string getName (void);

                void setUser (std::string s);
                std::string getUser (void);

                void setSizeKB (unsigned int i);
                unsigned int getSizeKB (void);

                std::string getSizeString (void);

                void setCupsPages (unsigned int i);
                unsigned int getCupsPages (void);

                void setPrinterUri (std::string s);
                std::string getPrinterUri (void);

                void setState (ipp_jstate_t jstate);
                std::string getState (void);

                void setCreationTime (int t);
                int getCreationTime (void);
                void setProcessingTime (int t);
                int getProcessingTime (void);
                void setCompletedTime (int t);
                int getCompletedTime (void);

                int getTime (void);
                std::string getFormattedTime (void);

                void setSourceIP (std::string ip);
                std::string getSourceIP (void);

                void setDestIP (std::string ip);
                std::string getDestIP (void);
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

