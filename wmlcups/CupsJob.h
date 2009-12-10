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
	 * Collection of status information for a Job
	 */
	class CupsJob
	{
	public:
		CupsJob();
		~CupsJob();

		void reset (void);

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

		void setCupsPages (unsigned int i);
		unsigned int getCupsPages (void);

		void setPrinterUri (std::string s);
		std::string getPrinterUri (void);

		void setState (ipp_jstate_t jstate);
		std::string getState (void);

		void setTime (int t);
		int getTime (void);
		std::string getFormattedTime (void);

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
		 * Job time - Time of completion/abortion/send of
		 * print.
		 */
		int time;
	};

} // namespace wml

#endif // _CUPSJOB_H_

