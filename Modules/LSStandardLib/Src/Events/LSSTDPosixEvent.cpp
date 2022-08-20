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

#include "LSSTDPosixEvent.h"

#ifdef LSE_POSIX

namespace lsstd	{

	// == Various constructors.
	LSE_CALLCTOR CPosixEvent::CPosixEvent() {
		::pthread_cond_init( &m_cHandle, NULL );
		::pthread_mutex_init( &m_mLock, NULL );
	}
	LSE_CALLCTOR CPosixEvent::~CPosixEvent() {
		::pthread_cond_destroy( &m_cHandle );
		::pthread_mutex_destroy( &m_mLock );
	}

	// == Functions.
	/**
	 * Waits for an event to be triggered.  Events are triggered by calling Trigger().
	 */
	void LSE_CALL CPosixEvent::WaitEvent() {
		if ( ::pthread_mutex_lock( &m_mLock ) != 0 ) {
			CStd::DebugPrintA( "CPosixEvent::WaitEvent(): Failed to acquire mutex lock." );
			return;
		}
		while ( ::pthread_cond_wait( &m_cHandle, &m_mLock ) != 0 ) {}
		::pthread_mutex_unlock( &m_mLock );
	}

	/**
	 * Triggers an event.  Threads waiting on the event (by calling WaitEvent()) will resume.
	 *
	 * \return Returns true if triggering the event succeeded.
	 */
	LSBOOL LSE_CALL CPosixEvent::Trigger() {
		if ( ::pthread_mutex_lock( &m_mLock ) != 0 ) {
			CStd::DebugPrintA( "CPosixEvent::Trigger(): Failed to acquire mutex lock." );
			return false;
		}
		::pthread_cond_signal( &m_cHandle );
		::pthread_mutex_unlock( &m_mLock );
		return true;
	}

}	// namespace lsstd

#endif	// #ifdef LSE_POSIX
