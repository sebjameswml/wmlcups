/* -*-c++-*- */
/*!
 *  This file is part of wmlcups - a library containing extentions of
 *  and wrappers around the CUPS API.
 *
 *  wmlcups is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
 *
 *  wmlcups is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  wmlcups is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wmlcups (see the file COPYING).  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
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
		 * Is the printer marked as shared?
		 */
		bool shared;

		/*!
		 * The information about the last job processed.
		 */
		CupsJob lastJob;
	};

} // namespace wml

#endif // _QUEUECUPSSTATUS_H_

