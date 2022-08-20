/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A POSIX event.  When triggered, a thread will activate from a waiting state.
 */


#ifndef __LSSTD_POSIXEVENT_H__
#define __LSSTD_POSIXEVENT_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_POSIX

#include <pthread.h>

namespace lsstd {

	/**
	 * Class CPosixEvent
	 * \brief A POSIX event.
	 *
	 * Description: A POSIX event.  When triggered, a thread will activate from a waiting state.
	 */
	class CPosixEvent {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CPosixEvent();
		LSE_CALLCTOR							~CPosixEvent();


		// == Functions.
		/**
		 * Waits for an event to be triggered.  Events are triggered by calling Trigger().
		 */
		void LSE_CALL							WaitEvent();

		/**
		 * Triggers an event.  Threads waiting on the event (by calling WaitEvent()) will resume.
		 *
		 * \return Returns true if triggering the event succeeded.
		 */
		LSBOOL LSE_CALL							Trigger();


	protected :
		// == Members.
		/** The event handle. */
		::pthread_cond_t						m_cHandle;
		
		/** The mutex. */
		::pthread_mutex_t						m_mLock;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	typedef CPosixEvent							CEvent;

}	// namespace lsstd

#endif	// #ifdef LSE_POSIX

#endif	// __LSSTD_POSIXEVENT_H__
