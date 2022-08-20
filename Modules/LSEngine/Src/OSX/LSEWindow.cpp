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

#include "LSEWindow.h"

#if defined( LSE_OSX )
#include "Gfx/LSGGfx.h"


namespace lse {

	// == Functions.
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
	LSBOOL LSE_CALL CWindow::CreateRenderWindow( LSINT32 _iWidth, LSINT32 _iHeight, LSBOOL _bFullScreen, LSUINT32 _dwBits, const LSCHAR * _pcTitle ) {
		assert( m_pgGame );
		CGDirectDisplayID ddiId = ::CGMainDisplayID();
		GDHandle hDisplay;


		return TRUE;
	}

	/**
	 * Toggle full-screen mode.
	 */
	LSBOOL LSE_CALL CWindow::ToggleFullScreen() {
		++m_bTogglingFullScreen;

#if defined( LSG_DX9 ) || defined( LSG_DX11 )
		CGfx::TogglingFullScreen( true );
#endif	// #if defined( LSG_DX9 ) || defined( LSG_DX11 )

		CreateRenderWindow( !m_pgGame->IsFullScreen() ? m_iFullWidth : m_iWindowedWidth,
			!m_pgGame->IsFullScreen() ? m_iFullHeight : m_iWindowedHeight,
			!m_pgGame->IsFullScreen(), m_ui32Depth,
			NULL );

#ifdef LSG_DX9
		CDirectX9::SetWindowParms( !m_pgGame->IsFullScreen(), GetWindow() );
		CDirectX9::ResetDevice();
		CDirectX9::TogglingFullScreen( false );
#endif	// #ifdef LSG_DX9
#ifdef LSG_DX11
		CDirectX11::SetWindowParms( !m_pgGame->IsFullScreen(), GetWindow() );
		CDirectX11::TogglingFullScreen( false );
#endif	// #ifdef LSG_DX11
#ifdef LSG_OGL
		COpenGl::PostInitOpenGl();
#endif	// #ifdef LSG_OGL
		--m_bTogglingFullScreen;

		return true;
	}
	
	/**
	 * Presenting the back buffer to the screen.
	 *
	 * \param _pvParm Present-defined data.
	 */
	LSVOID LSE_CALL CWindow::Present( LSVOID * _pvParm ) {

	}

}	// namespace lse

#endif	// #if defined( LSE_OSX )
