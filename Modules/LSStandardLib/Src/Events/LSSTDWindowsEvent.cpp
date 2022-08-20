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

#include "LSSTDWindowsEvent.h"

#ifdef LSE_WINDOWS

namespace lsstd	{

	// == Various constructors.
	LSE_CALLCTOR CWindowsEvent::CWindowsEvent() {
		m_hHandle = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	}
	LSE_CALLCTOR CWindowsEvent::~CWindowsEvent() {
		::CloseHandle( m_hHandle );
		m_hHandle = NULL;
	}

	// == Functions.
	/**
	 * Waits for an event to be triggered.  Events are triggered by calling Trigger().
	 */
	void LSE_CALL CWindowsEvent::WaitEvent() {
		switch ( ::WaitForSingleObject( m_hHandle, INFINITE ) ) {
			case WAIT_OBJECT_0 : {
				// Success.
				break;
			}
			case WAIT_FAILED : {
				PrintError( L"WaitEvent()" );
				break;
			}
			// WAIT_ABANDONED and WAIT_TIMEOUT are impossible cases.
		}
	}

	/**
	 * Triggers an event.  Threads waiting on the event (by calling WaitEvent()) will resume.
	 *
	 * \return Returns true if triggering the event succeeded.
	 */
	LSBOOL LSE_CALL CWindowsEvent::Trigger() {
		if ( !::SetEvent( m_hHandle ) ) {
			PrintError( L"Trigger()" );
			return false;
		}
		return true;
	}

	/**
	 * reports GetError() results.
	 *
	 * \param _putfxFuncName The function name to print.
	 */
	void LSE_CALL CWindowsEvent::PrintError( const LSUTFX * _putfxFuncName ) {
		LPVOID lpvMsgBuf;
		LPVOID lpvDisplayBuf;
		DWORD dwError = ::GetLastError(); 

		DWORD dwCreatedLen = ::FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			reinterpret_cast<LPWSTR>(&lpvMsgBuf),
			0, NULL );

		uint32_t ui32LocalLen = dwCreatedLen + 64 + CStd::WStrLen( _putfxFuncName );
		uint32_t ui32Len = ui32LocalLen * sizeof( WCHAR );
		lpvDisplayBuf = static_cast<LPVOID>(::LocalAlloc( LMEM_ZEROINIT, ui32Len ));
		CStd::WSPrintF( static_cast<LSUTFX *>(lpvDisplayBuf),
			ui32LocalLen,
			L"%s failed with error %d: %s.\r\n",
			_putfxFuncName, dwError, lpvMsgBuf ); 
		CStd::DebugPrintW( static_cast<const LSUTFX *>(lpvDisplayBuf) );
		::LocalFree( lpvMsgBuf );
		::LocalFree( lpvDisplayBuf );
	}

}	// namespace lsstd

#endif	// #ifdef LSE_WINDOWS
