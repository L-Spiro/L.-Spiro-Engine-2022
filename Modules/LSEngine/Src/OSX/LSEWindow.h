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
 * Description: Creates a window into which to render.
 */


#ifndef __LSE_WINDOW_H__
#define __LSE_WINDOW_H__

#include "../LSEEngine.h"

#if defined( LSE_OSX )

#include "../Game/LSEGame.h"
#include "../Input/LSEInputQueue.h"
#include "Windows/LSSTDWindows.h"
#include <CoreGraphics/CGDirectDisplay.h>

namespace lse {

	/**
	 * Class CWindow
	 *
	 * Description: The window into which to render on the Windows platform.
	 */
	class CWindow {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALL					CWindow();


		// == Functions.
		/**
		 * Sets the game pointer.  Can do nothing without this.
		 *
		 * \param _pgGame Pointer to the game class to be used by this window.
		 */
		LSVOID LSE_CALL						SetGamePointer( CGame * _pgGame );

		/**
		 * Sets the starting resolutions of the window.  One set of dimensions is for windowed mode and
		 *	the other for full-screen.
		 *
		 * \param _iWinWidth Windowed default width.
		 * \param _iWinHeight Windowed default height.
		 * \param _iFullWidth Full-screen default width.
		 * \param _iFullHeight Full-screen default height.
		 */
		LSVOID LSE_CALL						SetDefaultWindowDimensions( LSINT32 _iWinWidth, LSINT32 _iWinHeight, LSINT32 _iFullWidth, LSINT32 _iFullHeight );

		/**
		 * Creates a window with the given resolution.
		 *
		 * \param _iWidth Width of the window.
		 * \param _iHeight Height of the window.
		 * \param _bFullScreen If true, the window is full-screen.
		 * \param _dwBits Bit-depth of the window.
		 * \param _pcTitle Optional window title.
		 * \return Returns true if the window was created.
		 */
		LSBOOL LSE_CALL						CreateRenderWindow( LSINT32 _iWidth, LSINT32 _iHeight, LSBOOL _bFullScreen, LSUINT32 _dwBits, const LSCHAR * _pcTitle );

		/**
		 * Run the message pump.
		 *
		 * \return Returns an exit code.
		 */
		LSINT32 LSE_CALL					Run();

		/**
		 * Toggle full-screen mode.
		 */
		LSBOOL LSE_CALL						ToggleFullScreen();


	protected :
		// == Members.
		/** Game pointer. */
		CGame *								m_pgGame;
		
		/** Path to us. */
		wchar_t								m_szPath[LSF_MAX_PATH];

		/** Dimensions of the window. */
		LSINT32								m_iWidth, m_iHeight;

		/** Dimensions of the window in windowed mode. */
		LSINT32								m_iWindowedWidth, m_iWindowedHeight;

		/** Dimensions of the window in full-screen mode. */
		LSINT32								m_iFullWidth, m_iFullHeight;

		/** Bit depth. */
		LSUINT32							m_ui32Depth;

		/** Are we tracking the mouse? */
		LSBOOL								m_bTracking;

		/** Are we toggling full-screen mode? */
		LSBOOL								m_bTogglingFullScreen;

		/** Input queue. */
		CInputQueue							m_iqInputQueue;

		/** Input timer. */
		CTime								m_tInputTime;
		
		/** Window handle. */
		WindowRef							m_wrWnd;

	
		// == Functions.
		/**
		 * Presenting the back buffer to the screen.
		 *
		 * \param _pvParm Present-defined data.
		 */
		static LSVOID LSE_CALL				Present( LSVOID * _pvParm );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALL CWindow::CWindow() :
		m_pgGame( NULL ),
		m_iWidth( 0 ), m_iHeight( 0 ),
		m_iWindowedWidth( 0 ), m_iWindowedHeight( 0 ),
		m_iFullWidth( 0 ), m_iFullHeight( 0 ),
		m_ui32Depth( 0 ),
		m_bTogglingFullScreen( FALSE ) {
		// Get our path.
	}
	
	/**
	 * Sets the game pointer.  Can do nothing without this.
	 *
	 * \param _pgGame Pointer to the game class to be used by this window.
	 */
	LSE_INLINE LSVOID LSE_CALL CWindow::SetGamePointer( CGame * _pgGame ) {
		m_pgGame = _pgGame;
	}

	/**
	 * Sets the starting resolutions of the window.  One set of dimensions is for windowed mode and
	 *	the other for full-screen.
	 *
	 * \param _iWinWidth Windowed default width.
	 * \param _iWinHeight Windowed default height.
	 * \param _iFullWidth Full-screen default width.
	 * \param _iFullHeight Full-screen default height.
	 */
	LSE_INLINE LSVOID LSE_CALL CWindow::SetDefaultWindowDimensions( LSINT32 _iWinWidth, LSINT32 _iWinHeight, LSINT32 _iFullWidth, LSINT32 _iFullHeight ) {
		m_iWindowedWidth = _iWinWidth;
		m_iWindowedHeight = _iWinHeight;
		m_iFullWidth = _iFullWidth;
		m_iFullHeight = _iFullHeight;
	}

}	// namespace lse

#endif	// #if defined( LSE_OSX )

#endif	// __LSE_WINDOW_H__
