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

#if defined( LSE_WINDOWS )

#include "../Game/LSEGame.h"
#include "../Input/LSEInputQueue.h"
#include "Windows/LSSTDWindows.h"

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
		VOID LSE_CALL						SetGamePointer( CGame * _pgGame );

		/**
		 * Sets the starting resolutions of the window.  One set of dimensions is for windowed mode and
		 *	the other for full-screen.
		 *
		 * \param _iWinWidth Windowed default width.
		 * \param _iWinHeight Windowed default height.
		 * \param _iFullWidth Full-screen default width.
		 * \param _iFullHeight Full-screen default height.
		 */
		VOID LSE_CALL						SetDefaultWindowDimensions( INT _iWinWidth, INT _iWinHeight, INT _iFullWidth, INT _iFullHeight );

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
		BOOL LSE_CALL						CreateRenderWindow( INT _iWidth, INT _iHeight, BOOL _bFullScreen, DWORD _dwBits, const char * _pcTitle );

		/**
		 * Sets the HDC.  Can only be called once.
		 *
		 * \param _hDc The device context for this window.
		 */
		VOID LSE_CALL						SetHdc( HDC _hDc );

		/**
		 * Gets the HDC.
		 *
		 * \return Returns the HDC.
		 */
		HDC LSE_CALL						GetHdc();

		/**
		 * Run the message pump.
		 *
		 * \return Returns an exit code.
		 */
		INT LSE_CALL						Run();

		/**
		 * Gets the initial mouse position.
		 *
		 * \return Returns the initial mouse position.
		 */
		POINT LSE_CALL						GetInitialMousePos() const;

		/**
		 * Gets the handle to the window.
		 *
		 * \return Returns the handle to the main window.
		 */
		HWND LSE_CALL						GetWindow();

		/**
		 * Toggle full-screen mode.
		 */
		BOOL LSE_CALL						ToggleFullScreen();


		// == Members.
		// Rendering context for OpenGL.
		HGLRC								m_hGrlc;


	protected :
		// == Members.
		/** Game pointer. */
		CGame *								m_pgGame;

		/** Handle to the window. */
		HWND								m_hWnd;

		/** Window device context. */
		HDC									m_hDc;
		
		/** Path to us. */
		wchar_t								m_szPath[MAX_PATH];

		/** Dimensions of the window. */
		INT									m_iWidth, m_iHeight;

		/** Dimensions of the window in windowed mode. */
		INT									m_iWindowedWidth, m_iWindowedHeight;

		/** Dimensions of the window in full-screen mode. */
		INT									m_iFullWidth, m_iFullHeight;

		/** Bit depth. */
		DWORD								m_dwDepth;

		/** Initial mouse position. */
		POINT								m_pInitialMousePos;

		/** Are we tracking the mouse? */
		BOOL								m_bTracking;

		/** Are we toggling full-screen mode? */
		BOOL								m_bTogglingFullScreen;

		/** Input queue. */
		CInputQueue							m_iqInputQueue;

		/** Input timer. */
		CTime								m_tInputTime;

	
		// == Functions.
		/**
		 * The window procedure.
		 *
		 * \param _hWnd A handle to the window.
		 * \param _uiMsg The message.
		 * \param _wParam Additional message information.  The contents of this parameter depend on the value of the _uiMsg parameter.
		 * \param _lParam Additional message information.  The contents of this parameter depend on the value of the _uiMsg parameter.
		 * \return The return value is the result of the message processing and depends on the message sent.
		 */
		static LRESULT CALLBACK				WndProc( HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam );
		
		/**
		 * Presenting the back buffer to the screen.
		 *
		 * \param _pvParm Present-defined data.
		 */
		static void LSE_CALL				Present( void * _pvParm );

		/**
		 * Creates a compatible pixel format.
		 */
		VOID LSE_CALL						CreatePixelFormat();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALL CWindow::CWindow() :
		m_pgGame( NULL ),
		m_hWnd( NULL ),
		m_hDc( NULL ),
		m_hGrlc( NULL ),
		m_iWidth( 0 ), m_iHeight( 0 ),
		m_iWindowedWidth( 0 ), m_iWindowedHeight( 0 ),
		m_iFullWidth( 0 ), m_iFullHeight( 0 ),
		m_dwDepth( 0 ),
		m_bTogglingFullScreen( FALSE ) {
		// Get our path.
		::GetModuleFileNameW( NULL, m_szPath, MAX_PATH );
	}

	/**
	 * Sets the game pointer.  Can do nothing without this.
	 *
	 * \param _pgGame Pointer to the game class to be used by this window.
	 */
	LSE_INLINE VOID LSE_CALL CWindow::SetGamePointer( CGame * _pgGame ) {
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
	LSE_INLINE VOID LSE_CALL CWindow::SetDefaultWindowDimensions( INT _iWinWidth, INT _iWinHeight, INT _iFullWidth, INT _iFullHeight ) {
		m_iWindowedWidth = _iWinWidth;
		m_iWindowedHeight = _iWinHeight;
		m_iFullWidth = _iFullWidth;
		m_iFullHeight = _iFullHeight;
	}

	/**
	 * Sets the HDC.  Can only be called once.
	 *
	 * \param _hDc The device context for this window.
	 */
	LSE_INLINE VOID LSE_CALL CWindow::SetHdc( HDC _hDc ) {
		m_hDc = _hDc;
	}

	/**
	 * Gets the HDC.
	 *
	 * \return Returns the HDC.
	 */
	LSE_INLINE HDC LSE_CALL CWindow::GetHdc() {
		return m_hDc;
	}

	/**
	 * Gets the initial mouse position.
	 *
	 * \return Returns the initial mouse position.
	 */
	LSE_INLINE POINT LSE_CALL CWindow::GetInitialMousePos() const {
		return m_pInitialMousePos;
	}

	/**
	 * Gets the handle to the window.
	 *
	 * \return Returns the handle to the main window.
	 */
	LSE_INLINE HWND LSE_CALL CWindow::GetWindow() {
		return m_hWnd;
	}

}	// namespace lse

#endif	// #if defined( LSE_WINDOWS )

#endif	// __LSE_WINDOW_H__
