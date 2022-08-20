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
 * Description: Cross-platform threading.
 */


#ifndef __LSH_THREADLIB_H__
#define __LSH_THREADLIB_H__

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

#if defined( LSE_WINDOWS )
// Get threading routines on Windows.
#include "Windows/LSSTDWindows.h"
#define LSH_CALL			WINAPI
#elif defined( LSE_MAC )
// Apple/Macintosh OS X.
#include <pthread.h>

#define LSH_CALL			
#elif defined( __linux__ )
// Linux.
#define LSH_LIN
#endif	// #if defined( LSE_WINDOWS )


namespace lsh {

	// The handle to a thread.
#ifdef LSE_WINDOWS
	typedef HANDLE															LSH_THREAD;
#elif defined( LSE_MAC )
	typedef pthread_t														LSH_THREAD;
#else
#error "Threading not supported on this platform yet."
#endif	// #ifdef LSE_WINDOWS

	typedef uint32_t (LSH_CALL * LSH_THREAD_PROC)( void * _lpParameter );

	/**
	 * Class CThreadLib
	 * \brief Cross-platform threading.
	 *
	 * Description: Cross-platform threading.
	 */
	class CThreadLib {
	public :
		// == Functions.
		/**
		 * Creates a new thread.  _ptReturnHandle can be NULL if the handle for the thread is not needed.
		 *
		 * \param _pfProc Function pointer where the thread begins.
		 * \param _lpParameter Parameter to pass to the function on the new thread.
		 * \param _ptReturnHandle Returned handle of the thread.
		 * \return Returns true if the thread was created.
		 */
		static LSBOOL LSE_CALL												CreateThread( LSH_THREAD_PROC _pfProc, void * _lpParameter, LSH_THREAD * _ptReturnHandle );

		/**
		 * Close the handle to a thread.  Does not cause the thread to stop.  The handle must not be closed more than once.
		 *
		 * \param _tReturnHandle The handle to close.
		 * \return Returns true if the given handle is valid.
		 */
		static LSBOOL LSE_CALL												CloseThread( LSH_THREAD &_tReturnHandle );

		/**
		 * Wait for a thread to finish and get its return value.  The return value of the other thread is given in uint32_t form as the return
		 *	of this function.
		 *
		 * \param _tThread The thread for which to wait.
		 * \param _pui32RetVal Return value from the thread.
		 * \return Returns true if the given thread is valid.
		 */
		static LSBOOL LSE_CALL												WaitForThread( const LSH_THREAD &_tThread, uint32_t * _pui32RetVal );

		/**
		 * Gets a handle to the current thread.
		 *
		 * \return Returns a handle to the current thread.  The handle should not be closed.
		 */
		static LSH_THREAD LSE_CALL											GetCurThread();

		/**
		 * Make a thread handle invalid.
		 *
		 * \param _tHandle The handle to make invalid.
		 */
		static LSE_INLINE void LSE_CALL										MakeInvalidHandle( LSH_THREAD &_tHandle );

		/**
		 * Is the thread handle valid?
		 *
		 * \param _tHandle The handle to check for being valid.
		 * \return Returns true if the given thread handle is valid.
		 */
		static LSE_INLINE LSBOOL LSE_CALL									ThreadHandleIsValid( const LSH_THREAD &_tHandle );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Make a thread handle invalid.
	 *
	 * \param _tHandle The handle to make invalid.
	 */
	LSE_INLINE void LSE_CALL CThreadLib::MakeInvalidHandle( LSH_THREAD &_tHandle ) {
		CStd::MemSet( &_tHandle, 0, sizeof( LSH_THREAD ) );
	}

	/**
	 * Is the thread handle valid?
	 *
	 * \param _tHandle The handle to check for being valid.
	 * \return Returns true if the given thread handle is valid.
	 */
	LSE_INLINE LSBOOL LSE_CALL CThreadLib::ThreadHandleIsValid( const LSH_THREAD &_tHandle ) {
		static const uint8_t ui8Invalid[sizeof( LSH_THREAD )] = { 0 };
		return !CStd::MemCmpF( &_tHandle, ui8Invalid, sizeof( LSH_THREAD ) );
	}

}	// namespace lsh

#endif	// __LSH_THREADLIB_H__
