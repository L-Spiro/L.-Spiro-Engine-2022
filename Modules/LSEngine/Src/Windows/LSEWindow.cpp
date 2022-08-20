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

#if defined( LSE_WINDOWS )
#include "Gfx/LSGGfx.h"
#include <shellapi.h>

#pragma warning( push )

// warning C4244: 'argument' : conversion from 'LONG_PTR' to 'LONG', possible loss of data
#pragma warning( disable : 4244 )


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
	BOOL LSE_CALL CWindow::CreateRenderWindow( INT _iWidth, INT _iHeight, BOOL _bFullScreen, DWORD _dwBits, const char * _pcTitle ) {
		assert( m_pgGame );
		LPWSTR lpwsTitle = NULL;
		if ( !m_hWnd ) {
			WNDCLASSW wndClass		= { 0 };
			wndClass.style			= CS_VREDRAW | CS_HREDRAW;//CS_DBLCLKS;
			wndClass.lpfnWndProc	= WndProc;
			wndClass.cbClsExtra		= 0;
			wndClass.cbWndExtra		= 0;
			wndClass.hInstance		= ::GetModuleHandleW( NULL );
			wndClass.hIcon			= ::ExtractIconW( wndClass.hInstance, m_szPath, 0 );
			wndClass.hCursor		= ::LoadCursorW( NULL, IDC_ARROW );
			wndClass.hbrBackground	= NULL;
			wndClass.lpszMenuName	= NULL;
			wndClass.lpszClassName	= L"LSEWindowClass";

			if ( !::RegisterClassW( &wndClass ) ) { return FALSE; }
		}
		else {
			// Retain the window title.
			uint32_t ui32Len = ::GetWindowTextLengthW( m_hWnd ) + 1;
			lpwsTitle = LSENEW WCHAR[ui32Len];
			::GetWindowTextW( m_hWnd, lpwsTitle, ui32Len );

			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}

		m_bTracking = FALSE;
		m_dwDepth = _dwBits;

		DWORD dwStyle, dwStyleEx;
		if ( _bFullScreen ) {
			DEVMODE dmMode = { 0 };
			dmMode.dmSize = sizeof( dmMode );
			dmMode.dmPelsWidth = _iWidth;
			dmMode.dmPelsHeight = _iHeight;
			dmMode.dmBitsPerPel = _dwBits;
			dmMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

			if ( ::ChangeDisplaySettings( &dmMode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL ) {
				// Try again in 16-bit mode if we did not already.
				if ( _dwBits > 16 ) {
					dmMode.dmBitsPerPel = 16;
					if ( ::ChangeDisplaySettings( &dmMode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL ) {
						_bFullScreen = FALSE;
					}
					m_dwDepth = 16;
				}
				else {
					_bFullScreen = FALSE;
				}
			}
		}

		// If we entered fullscreen mode, change the styles.
		if ( _bFullScreen ) {
			dwStyle = WS_POPUP;
			dwStyleEx = WS_EX_APPWINDOW;
		}
		else {
			dwStyle = WS_OVERLAPPEDWINDOW;
			dwStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		}

		RECT rRect;
		::SetRect( &rRect, 0, 0, _iWidth, _iHeight );        
		::AdjustWindowRectEx( &rRect, dwStyle, FALSE, dwStyleEx );

		
		if ( _pcTitle ) {
			LSEDELETE [] lpwsTitle;
			lpwsTitle = NULL;
			uint32_t ui32Len = CStd::Utf8StrLenAsUtf16( reinterpret_cast<const LSUTF8 *>(_pcTitle) );
			lpwsTitle = LSENEW WCHAR[ui32Len];
			CStd::Utf8ToUtf16( reinterpret_cast<LSUTF16 *>(lpwsTitle), reinterpret_cast<const LSUTF8 *>(_pcTitle), ui32Len );
		}
		m_hWnd = ::CreateWindowExW( dwStyleEx,
			L"LSEWindowClass", lpwsTitle ? lpwsTitle : L"L. Spiro",
			dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, (rRect.right - rRect.left), (rRect.bottom - rRect.top), 0,
			NULL, ::GetModuleHandleW( NULL ), reinterpret_cast<LPVOID>(this) );
		LSEDELETE [] lpwsTitle;
		if ( !m_hWnd ) { return FALSE; }

		/*RECT rFinalRect;
		::GetClientRect( m_hWnd, &rFinalRect );*/

		m_iWidth = _iWidth;
		m_iHeight = _iHeight;



		// Get the mouse positions.
		::GetCursorPos( &m_pInitialMousePos );
		if ( m_pgGame ) {
			// Set the input queue and synchronize the input clock to the game clock.
			//	When the game requests inputs up to a maximum time stamp it will only
			//	get the correct inputs if the clocks are synchronized.
			m_pgGame->SetInputQueue( &m_iqInputQueue );
			m_tInputTime = m_pgGame->Time();

			m_pgGame->SetInitialMousePosition( m_pInitialMousePos.x, m_pInitialMousePos.y );
			m_pgGame->SetFullScreen( _bFullScreen );
			//m_pgGame->SetPresentFunc( Present, reinterpret_cast<void *>(m_hDc) );
			
		}

		::ShowCursor( FALSE );
		::ShowWindow( m_hWnd, SW_SHOWNORMAL );
		::UpdateWindow( m_hWnd );
		::SetForegroundWindow( m_hWnd );
		::SetActiveWindow( m_hWnd );

		return TRUE;
	}

	/**
	 * Run the message pump.
	 *
	 * \return Returns an exit code.
	 */
	INT LSE_CALL CWindow::Run() {
		assert( m_pgGame );
		MSG		mMsg;
		mMsg.message = WM_NULL;
		::PeekMessage( &mMsg, NULL, 0U, 0U, PM_NOREMOVE );

		while ( mMsg.message != WM_QUIT ) {
			// Use ::PeekMessage() so we can use idle time to render the scene.

			while ( (::PeekMessage( &mMsg, NULL, 0U, 0U, PM_REMOVE ) != 0) ) {
				// Translate and dispatch the message.
				if ( ::TranslateAccelerator( m_hWnd, NULL, &mMsg ) == 0 ) {
					::TranslateMessage( &mMsg );
					::DispatchMessage( &mMsg );
				}
			}
			if ( mMsg.message != WM_QUIT && !m_pgGame->IsClosing() ) {
				// Reduce the CPU load when not active.
				if ( !m_pgGame->Active() ) {
					::Sleep( 100 );
				}
				if ( !m_pgGame->Tick() ) {
					// Remove the current state so it can free its RAM.
					m_pgGame->SetNextState( static_cast<uint32_t>(lse::LSE_GS_INVALID), 0, true );
					m_pgGame->Close();
					while ( m_pgGame->CurStateId() != static_cast<uint32_t>(lse::LSE_GS_INVALID) ) {
						m_pgGame->Tick();
					}
					return -1;
				}
				CGfx::Present();
			}
			if ( m_pgGame->IsClosing() ) {
				break;
			}
		}
		// Remove the current state so it can free its RAM.
		m_pgGame->SetNextState( static_cast<uint32_t>(lse::LSE_GS_INVALID), 0, true );
		m_pgGame->Close();
		while ( m_pgGame->CurStateId() != static_cast<uint32_t>(lse::LSE_GS_INVALID) ) {
			m_pgGame->Tick();
		}
		return static_cast<INT>(mMsg.wParam);
	}

	/**
	 * Toggle full-screen mode.
	 */
	BOOL LSE_CALL CWindow::ToggleFullScreen() {
		++m_bTogglingFullScreen;

#if defined( LSG_DX9 ) || defined( LSG_DX11 )
		CGfx::TogglingFullScreen( true );
#endif	// #if defined( LSG_DX9 ) || defined( LSG_DX11 )

		CreateRenderWindow( !m_pgGame->IsFullScreen() ? m_iFullWidth : m_iWindowedWidth,
			!m_pgGame->IsFullScreen() ? m_iFullHeight : m_iWindowedHeight,
			!m_pgGame->IsFullScreen(), m_dwDepth,
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
	 * The window procedure.
	 *
	 * \param _hWnd A handle to the window.
	 * \param _uiMsg The message.
	 * \param _wParam Additional message information.  The contents of this parameter depend on the value of the _uiMsg parameter.
	 * \param _lParam Additional message information.  The contents of this parameter depend on the value of the _uiMsg parameter.
	 * \return The return value is the result of the message processing and depends on the message sent.
	 */
	LRESULT CALLBACK CWindow::WndProc( HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWindow * pwThis = reinterpret_cast<CWindow *>(static_cast<uintptr_t>(::GetWindowLongPtr( _hWnd, GWLP_USERDATA )));
		if ( pwThis ) { pwThis->m_tInputTime.Update( false ); }
		static BOOL bMovingMouse = FALSE;

		switch ( _uiMsg ) {
			// ============================
			// Create and destroy.
			// ============================
			case WM_NCCREATE : {
				LPCREATESTRUCT lpcsCreateParms = reinterpret_cast<LPCREATESTRUCT>(_lParam);
				pwThis = reinterpret_cast<CWindow *>(lpcsCreateParms->lpCreateParams);

				::SetWindowLongPtr( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pwThis) );
				break;
			}
			case WM_CREATE : {
				pwThis->SetHdc( ::GetDC( _hWnd ) );
#ifdef LSG_OGL
				// Attach OpenGL.
				pwThis->CreatePixelFormat();
				CGraphicsLib::InitGraphicsLibApi( pwThis->GetHdc() );
#endif	// #ifdef LSG_OGL
				CGraphicsLib::MakeCurrent();

				RECT rcClient;
				::GetClientRect( _hWnd, &rcClient );
				pwThis->m_iWidth = rcClient.right - rcClient.left;
				pwThis->m_iHeight = rcClient.bottom - rcClient.top;
				pwThis->m_pgGame->SetScreenDimensions( pwThis->m_iWidth, pwThis->m_iHeight );


				RAWINPUTDEVICE ridInput;
				ridInput.usUsagePage = 1;
				ridInput.usUsage = 2;
				ridInput.dwFlags = RIDEV_INPUTSINK;
				ridInput.hwndTarget = _hWnd;
				::RegisterRawInputDevices( &ridInput, 1, sizeof( ridInput ) );
				break;
			}
			case WM_NCDESTROY : {}
			case WM_DESTROY : {}
			case WM_CLOSE : {}
			case WM_QUIT : {
				if ( pwThis ) {
					if ( pwThis->m_pgGame ) {
						pwThis->m_pgGame->SetInputQueue( NULL );
					}
				}


				if ( !pwThis->m_bTogglingFullScreen ) {
					if ( pwThis->m_pgGame ) {
						pwThis->m_pgGame->SetNextState( static_cast<uint32_t>(lse::LSE_GS_INVALID), 0 );
						pwThis->m_pgGame->Close();
						while ( pwThis->m_pgGame->CurStateId() != static_cast<uint32_t>(lse::LSE_GS_INVALID) ) {
							pwThis->m_pgGame->Tick();
						}
					}
					CGraphicsLib::DestroyGraphicsLibWindow();
					pwThis->m_hGrlc = NULL;
				}

				if ( pwThis->GetHdc() ) {
					::ReleaseDC( _hWnd, pwThis->GetHdc() );
					pwThis->SetHdc( NULL );
				}

				if ( !pwThis->m_bTogglingFullScreen ) {
					::PostQuitMessage( 0 );
				}
				
				break;
			}
			// ============================
			// ============================


			// ============================
			// States.
			// ============================
			//case WM_WINDOWPOSCHANGED : {}
			case WM_EXITSIZEMOVE : {
				RECT rcClient;
				::GetClientRect( _hWnd, &rcClient );
				pwThis->m_iWidth = rcClient.right - rcClient.left;
				pwThis->m_iHeight = rcClient.bottom - rcClient.top;
				pwThis->m_pgGame->SetScreenDimensions( pwThis->m_iWidth, pwThis->m_iHeight );
				break;
			}
			case WM_SIZE : {
				if ( _wParam != SIZE_MINIMIZED ) {
					pwThis->m_iWidth = LOWORD( _lParam );
					pwThis->m_iHeight = HIWORD( _lParam );
					pwThis->m_pgGame->SetScreenDimensions( pwThis->m_iWidth, pwThis->m_iHeight );
				}
				break;
			}
			case WM_ACTIVATE : {
				if ( LOWORD( _wParam ) == WA_INACTIVE ) {
					pwThis->m_pgGame->SetActive( FALSE );
				}
				else {
					pwThis->m_pgGame->SetActive( TRUE );
				}
				break;
			}
			// ============================
			// ============================


			// ============================
			// Input.
			// ============================
			case WM_INPUT : {
				if ( !pwThis->m_pgGame->Active() ) { break; }
				if ( pwThis->m_bTogglingFullScreen ) { break; }
				UINT dwSize = 40;
				
				::GetRawInputData( reinterpret_cast<HRAWINPUT>(_lParam), RID_INPUT, 
					NULL, &dwSize, sizeof( RAWINPUTHEADER ) );

				PBYTE pbData = LSENEW BYTE[dwSize];
				if ( !pbData ) { break; }
				if ( ::GetRawInputData( reinterpret_cast<HRAWINPUT>(_lParam), RID_INPUT, 
					pbData, &dwSize, sizeof( RAWINPUTHEADER ) ) == static_cast<UINT>(-1) ) {
					LSEDELETE [] pbData;
					break;
				}

				LPRAWINPUT lprwInput = reinterpret_cast<LPRAWINPUT>(pbData);

				if ( lprwInput->header.dwType == RIM_TYPEMOUSE ) {
					if ( (lprwInput->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) ) {
						//pwThis->m_pgGame->Mouse().SetPos( static_cast<LSREAL>(lprwInput->data.mouse.lLastX), static_cast<LSREAL>(lprwInput->data.mouse.lLastY) );
						pwThis->m_iqInputQueue.OnMouseMove( static_cast<LSREAL>(lprwInput->data.mouse.lLastX), static_cast<LSREAL>(lprwInput->data.mouse.lLastY),
							true, pwThis->m_tInputTime.GetCurMicros() );
					}
					else if ( (lprwInput->data.mouse.usFlags == 0) ) {
						//pwThis->m_pgGame->Mouse().AddPos( static_cast<LSREAL>(lprwInput->data.mouse.lLastX), static_cast<LSREAL>(lprwInput->data.mouse.lLastY) );
						pwThis->m_iqInputQueue.OnMouseMove( static_cast<LSREAL>(lprwInput->data.mouse.lLastX), static_cast<LSREAL>(lprwInput->data.mouse.lLastY),
							false, pwThis->m_tInputTime.GetCurMicros() );
					}
					

					// Check for mouse-down events.
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) ) {
						pwThis->m_iqInputQueue.OnMouseDown( LSE_MBC_LBUTTON, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) ) {
						pwThis->m_iqInputQueue.OnMouseDown( LSE_MBC_RBUTTON, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) ) {
						pwThis->m_iqInputQueue.OnMouseDown( LSE_MBC_MBUTTON, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) ) {
						pwThis->m_iqInputQueue.OnMouseDown( LSE_MBC_XBUTTON1, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) ) {
						pwThis->m_iqInputQueue.OnMouseDown( LSE_MBC_XBUTTON2, pwThis->m_tInputTime.GetCurMicros() );
					}

					// Check for mouse-up events.
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) ) {
						pwThis->m_iqInputQueue.OnMouseUp( LSE_MBC_LBUTTON, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) ) {
						pwThis->m_iqInputQueue.OnMouseUp( LSE_MBC_RBUTTON, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) ) {
						pwThis->m_iqInputQueue.OnMouseUp( LSE_MBC_MBUTTON, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) ) {
						pwThis->m_iqInputQueue.OnMouseUp( LSE_MBC_XBUTTON1, pwThis->m_tInputTime.GetCurMicros() );
					}
					if ( (lprwInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) ) {
						pwThis->m_iqInputQueue.OnMouseUp( LSE_MBC_XBUTTON2, pwThis->m_tInputTime.GetCurMicros() );
					}
				}
				
				LSEDELETE [] pbData;
				break;
			}
			case WM_MOUSEMOVE : {
				if ( pwThis->m_bTogglingFullScreen ) { break; }
				// Mouse move + not tracking = start tracking.
				if ( !pwThis->m_bTracking ) {
					TRACKMOUSEEVENT tmeEvent = {
						sizeof( TRACKMOUSEEVENT ),
						TME_LEAVE,
						_hWnd,
						HOVER_DEFAULT,
					};
					if ( ::TrackMouseEvent( &tmeEvent ) ) {
						pwThis->m_bTracking = TRUE;
					}
				}

				if ( !bMovingMouse ) {
					if ( pwThis->m_pgGame->Active() ) {
						bMovingMouse++;
						RECT rcWindow;
						::GetWindowRect( _hWnd, &rcWindow );
						LONG lWidth = rcWindow.right - rcWindow.left;
						LONG lHeight = rcWindow.bottom - rcWindow.top;

						LONG lTargetX = rcWindow.left + (lWidth >> 1);
						LONG lTargetY = rcWindow.top + (lHeight >> 1);

						POINT pCurPos;
						::GetCursorPos( &pCurPos );
						if ( pCurPos.x != lTargetX || pCurPos.y != lTargetY ) {
							/*if ( !pwThis->m_pgGame->SendInputToHiDefMouse() ) {
								pwThis->m_pgGame->GetLoDefMouse().AddPos( static_cast<LSREAL>(pCurPos.x - lTargetX), static_cast<LSREAL>(pCurPos.y - lTargetY) );
							}*/
							::SetCursorPos( lTargetX,
								lTargetY );
						}
						bMovingMouse--;
					}
				}
				break;
			}
#if 0
			case WM_LBUTTONDOWN : {}
			case WM_RBUTTONDOWN : {}
			case WM_MBUTTONDOWN : {}
			case WM_LBUTTONUP : {}
			case WM_RBUTTONUP : {}
			case WM_MBUTTONUP : {
				if ( pwThis->m_bTogglingFullScreen ) { break; }
				if ( pwThis->m_pgGame->Active() ) {
					if ( !pwThis->m_pgGame->SendInputToHiDefMouse() ) {
						switch ( _uiMsg ) {
							case WM_LBUTTONDOWN : {
								pwThis->m_pgGame->GetLoDefMouse().Click( pwThis->m_pgGame->GetLoDefMouse().GetIndexMap()[0] );
								break;
							}
							case WM_RBUTTONDOWN : {
								pwThis->m_pgGame->GetLoDefMouse().Click( pwThis->m_pgGame->GetLoDefMouse().GetIndexMap()[2] );
								break;
							}
							case WM_MBUTTONDOWN : {
								pwThis->m_pgGame->GetLoDefMouse().Click( pwThis->m_pgGame->GetLoDefMouse().GetIndexMap()[1] );
								break;
							}
							case WM_LBUTTONUP : {
								pwThis->m_pgGame->GetLoDefMouse().UnClick( pwThis->m_pgGame->GetLoDefMouse().GetIndexMap()[0] );
								break;
							}
							case WM_RBUTTONUP : {
								pwThis->m_pgGame->GetLoDefMouse().UnClick( pwThis->m_pgGame->GetLoDefMouse().GetIndexMap()[2] );
								break;
							}
							case WM_MBUTTONUP : {
								pwThis->m_pgGame->GetLoDefMouse().UnClick( pwThis->m_pgGame->GetLoDefMouse().GetIndexMap()[1] );
								break;
							}
						}
					}
				}
				break;
			}
#endif	// #if 0
			case WM_MOUSELEAVE : {
				if ( pwThis->m_bTogglingFullScreen ) { break; }
				pwThis->m_bTracking = FALSE;
				if ( pwThis->m_pgGame->Active() ) {
					if ( pwThis->m_pgGame->MouseCapture() ) {
						// We are not on a menu.  Put the mouse back at the center.
						bMovingMouse++;
						RECT rcWindow;
						::GetWindowRect( _hWnd, &rcWindow );
						LONG lWidth = rcWindow.right - rcWindow.left;
						LONG lHeight = rcWindow.bottom - rcWindow.top;

						LONG lTargetX = rcWindow.left + (lWidth >> 1);
						LONG lTargetY = rcWindow.top + (lHeight >> 1);

						POINT pCurPos;
						::GetCursorPos( &pCurPos );
						if ( pCurPos.x != lTargetX || pCurPos.y != lTargetY ) {
							::SetCursorPos( lTargetX,
								lTargetY );
						}
						bMovingMouse--;
					}
				}
				break;
			}
			case WM_KEYDOWN : {
				struct LSE_KEY {
					uintptr_t				uiptrRepCnt		: 16;
					uintptr_t				uiptrScanCode	: 8;
					uintptr_t				uiptrEx			: 1;
					uintptr_t				uiptrRes		: 4;
					uintptr_t				uiptrContext	: 1;
					uintptr_t				uiptrPrev		: 1;
					uintptr_t				uiptrTrans		: 1;
				} kKey = (*reinterpret_cast<LSE_KEY *>(&_lParam));
				if ( kKey.uiptrPrev == 0 ) {
					// Key just pressed.
					pwThis->m_iqInputQueue.OnKeyDown( static_cast<LSE_KEYCODES>(_wParam), pwThis->m_tInputTime.GetCurMicros() );
				}
				return 0;
			}
			case WM_KEYUP : {
				struct LSE_KEY {
					uintptr_t				uiptrRepCnt		: 16;
					uintptr_t				uiptrScanCode	: 8;
					uintptr_t				uiptrEx			: 1;
					uintptr_t				uiptrRes		: 4;
					uintptr_t				uiptrContext	: 1;
					uintptr_t				uiptrPrev		: 1;
					uintptr_t				uiptrTrans		: 1;
				} kKey = (*reinterpret_cast<LSE_KEY *>(&_lParam));
				if ( kKey.uiptrPrev == 1 ) {
					// Key just released.
					pwThis->m_iqInputQueue.OnKeyUp( static_cast<LSE_KEYCODES>(_wParam), pwThis->m_tInputTime.GetCurMicros() );
				}
				return 0;
			}
			case WM_SYSKEYDOWN : {
				struct LSE_KEY {
					uintptr_t				uiptrRepCnt		: 16;
					uintptr_t				uiptrScanCode	: 8;
					uintptr_t				uiptrEx			: 1;
					uintptr_t				uiptrRes		: 4;
					uintptr_t				uiptrContext	: 1;
					uintptr_t				uiptrPrev		: 1;
					uintptr_t				uiptrTrans		: 1;
				} kKey = (*reinterpret_cast<LSE_KEY *>(&_lParam));
				if ( kKey.uiptrPrev == 0 ) {
					// Key just pressed.
					pwThis->m_iqInputQueue.OnKeyDown( static_cast<LSE_KEYCODES>(_wParam), pwThis->m_tInputTime.GetCurMicros() );
				}
				// Handle special keys.
				if ( _wParam == VK_F4 ) {
					// Alt-F4 pressed.  Close the game.
					::DestroyWindow( _hWnd );
				}
				else if ( _wParam == VK_RETURN ) {
					pwThis->m_iqInputQueue.RemoveAllInputs();
					pwThis->ToggleFullScreen();
				}
				return 0;
			}
			case WM_SYSKEYUP : {
				struct LSE_KEY {
					uintptr_t				uiptrRepCnt		: 16;
					uintptr_t				uiptrScanCode	: 8;
					uintptr_t				uiptrEx			: 1;
					uintptr_t				uiptrRes		: 4;
					uintptr_t				uiptrContext	: 1;
					uintptr_t				uiptrPrev		: 1;
					uintptr_t				uiptrTrans		: 1;
				} kKey = (*reinterpret_cast<LSE_KEY *>(&_lParam));
				if ( kKey.uiptrPrev == 1 ) {
					// Key just released.
					pwThis->m_iqInputQueue.OnKeyUp( static_cast<LSE_KEYCODES>(_wParam), pwThis->m_tInputTime.GetCurMicros() );
				}
				return 0;
			}
			// ============================
			// ============================


			// ============================
			// Power management.
			// ============================
			case WM_POWERBROADCAST : {
				switch ( _wParam ) {
					case PBT_APMQUERYSUSPEND : { return BROADCAST_QUERY_DENY; }
				}
				break;
			}
			// ============================
			// ============================

		}
		return ::DefWindowProc( _hWnd, _uiMsg, _wParam, _lParam );
	}
	
	/**
	 * Presenting the back buffer to the screen.
	 *
	 * \param _pvParm Present-defined data.
	 */
	void LSE_CALL CWindow::Present( void * _pvParm ) {
		HDC hDc = reinterpret_cast<HDC>(_pvParm);
		::SwapBuffers( hDc );
	}

	/**
	 * Creates a compatible pixel format.
	 */
	VOID LSE_CALL CWindow::CreatePixelFormat() {
		assert( m_hDc );
		INT iPixForm;
		PIXELFORMATDESCRIPTOR pfdFormat = {
			sizeof( PIXELFORMATDESCRIPTOR ),
			1,
			PFD_SUPPORT_OPENGL |
			PFD_DRAW_TO_WINDOW |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			static_cast<BYTE>(m_dwDepth)
		};
		pfdFormat.cDepthBits = 24;
		pfdFormat.cStencilBits = 8;
		pfdFormat.iLayerType = PFD_MAIN_PLANE;
		pfdFormat.cAccumBits = 32;

		iPixForm = ::ChoosePixelFormat( m_hDc, &pfdFormat );
		::SetPixelFormat( m_hDc, iPixForm, &pfdFormat );
	}

}	// namespace lse

#pragma warning( pop )

#endif	// #if defined( LSE_WINDOWS )
