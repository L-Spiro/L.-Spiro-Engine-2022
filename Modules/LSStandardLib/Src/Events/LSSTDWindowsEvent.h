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
 * Description: A Windows event.  When triggered, a thread will activate from a waiting state.
 */


#ifndef __LSSTD_WINDOWSEVENT_H__
#define __LSSTD_WINDOWSEVENT_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_WINDOWS

#include "../Windows/LSSTDWindows.h"

namespace lsstd {

	/**
	 * Class CWindowsEvent
	 * \brief A Windows event.
	 *
	 * Description: A Windows event.  When triggered, a thread will activate from a waiting state.
	 */
	class CWindowsEvent {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CWindowsEvent();
		LSE_CALLCTOR							~CWindowsEvent();


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
		HANDLE									m_hHandle;


		// == Functions.
		/**
		 * reports GetError() results.
		 *
		 * \param _putfxFuncName The function name to print.
		 */
		static void LSE_CALL					PrintError( const LSUTFX * _putfxFuncName );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	typedef CWindowsEvent						CEvent;

}	// namespace lsstd

#endif	// #ifdef LSE_WINDOWS

#endif	// __LSSTD_WINDOWSEVENT_H__
