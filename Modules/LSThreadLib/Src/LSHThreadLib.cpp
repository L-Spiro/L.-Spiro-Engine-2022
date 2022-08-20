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

#include "LSHThreadLib.h"


namespace lsh {

	/**
	 * Creates a new thread.  _ptReturnHandle can be NULL if the handle for the thread is not needed.
	 *
	 * \param _pfProc Function pointer where the thread begins.
	 * \param _lpParameter Parameter to pass to the function on the new thread.
	 * \param _ptReturnHandle Returned handle of the thread.
	 * \return Returns true if the thread was created.
	 */
	LSBOOL LSE_CALL CThreadLib::CreateThread( LSH_THREAD_PROC _pfProc, void * _lpParameter, LSH_THREAD * _ptReturnHandle ) {
#ifdef LSE_WINDOWS
		LSH_THREAD hRet = ::CreateThread( NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(_pfProc), _lpParameter, 0, NULL );
		if ( !hRet ) { return false; }
#elif defined( LSE_MAC )
		LSH_THREAD hRet;
		if ( ::pthread_create( &hRet, NULL, reinterpret_cast<void * (*)( void * )>(_pfProc), _lpParameter ) != 0 ) {
			return false;
		}
#endif	// #ifdef LSE_WINDOWS


		// Thread created.
		if ( _ptReturnHandle ) {
			CStd::MemCpy( _ptReturnHandle, &hRet, sizeof( LSH_THREAD ) );
		}
		else {
			// Close the handle.
			CloseThread( hRet );
		}
		return true;
	}

	/**
	 * Close the handle to a thread.  Does not cause the thread to stop.  The handle must not be closed more than once.
	 *
	 * \param _tReturnHandle The handle to close.
	 * \return Returns true if the given handle is valid.
	 */
	LSBOOL LSE_CALL CThreadLib::CloseThread( LSH_THREAD &_tReturnHandle ) {
#ifdef LSE_WINDOWS
		if ( ::CloseHandle( _tReturnHandle ) ) {
			MakeInvalidHandle( _tReturnHandle );
			return true;
		}
		return false;
#elif defined( LSE_MAC )
		if ( ::pthread_detach( _tReturnHandle ) == 0 ) {
			// Handle is no longer in use.  Destroy it so that multiple calls on
			//	it will result in error.
			MakeInvalidHandle( _tReturnHandle );
			return true;
		}
		return false;
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Wait for a thread to finish and get its return value.  The return value of the other thread is given in uint32_t form as the return
	 *	of this function.
	 *
	 * \param _tThread The thread for which to wait.
	 * \param _pui32RetVal Return value from the thread.
	 * \return Returns true if the given thread is valid.
	 */
	LSBOOL LSE_CALL CThreadLib::WaitForThread( const LSH_THREAD &_tThread, uint32_t * _pui32RetVal ) {
#ifdef LSE_WINDOWS
		if ( ::WaitForSingleObject( _tThread, INFINITE ) != WAIT_OBJECT_0 ) { return false; }
		DWORD dwRet;
		if ( !::GetExitCodeThread( _tThread, &dwRet ) ) { return false; }
		if ( _pui32RetVal ) {
			(*_pui32RetVal) = static_cast<uint32_t>(dwRet);
		}
		return true;
#elif defined( LSE_MAC )
		void * pvRetVal;
		if ( ::pthread_join( _tThread, &pvRetVal ) != 0 ) { return false; }
		if ( _pui32RetVal ) {
			(*_pui32RetVal) = (*reinterpret_cast<uint32_t *>(&pvRetVal));
		}
		return true;
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Gets a handle to the current thread.
	 *
	 * \return Returns a handle to the current thread.  The handle should not be closed.
	 */
	LSH_THREAD LSE_CALL CThreadLib::GetCurThread() {
#ifdef LSE_WINDOWS
		return ::GetCurrentThread();
#elif defined( LSE_MAC )
		return ::pthread_self();
#endif	// #ifdef LSE_WINDOWS
	}

}	// namespace lsh
