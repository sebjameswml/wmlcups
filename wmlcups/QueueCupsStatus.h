/* -*-c++-*- */
/*!
 * Queue status information container class.
 *
 * Author: Sebastian James <sjames@wmltd.co.uk>
 *
 * Copyright: 2009 William Matthew Limited.
 */

#ifndef _QUEUECUPSSTATUS_H_
#define _QUEUECUPSSTATUS_H_

#ifdef __GNUG__
# pragma interface
#endif

#include "config.h"
#include "CupsJob.h"

namespace wml {

	/*!
	 * Collection of status information for Queue.
	 *
	 * A small struct to hold Queue status, and only status. A
	 * data container to pass into the WmlCups function
	 * getStatus(). Holds queue status and info about the last
	 * print job processed.
	 *
	 */
	struct QueueCupsStatus
	{
		/*!
		 * The state of the queue. Stored here as a string for
		 * simplicity, even though this will use a bit more
		 * memory than storing as a number.
		 */
		std::string state;

		/*!
		 * The printer state message.
		 */
		std::string stateMsg;

		/*!
		 * Enabled?
		 */
		bool enabled;

		/*!
		 * Accepting?
		 */
		bool accepting;

		/*!
		 * The information about the last job processed.
		 */
		CupsJob lastJob;
	};

} // namespace wml

#endif // _QUEUECUPSSTATUS_H_

