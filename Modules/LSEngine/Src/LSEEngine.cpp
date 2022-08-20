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
 * Description: The lowest-level include in the engine.  All engine files include this one.  It defines
 *	constants, settings, and include files used throughout the engine.
 */

#include "LSEEngine.h"
#include "HighLevel/LSFFilesEx.h"
//#include "Models/LSEModelInstance.h"
//#include "Models/LSEDrawableModelInstance.h"
//#include "Texture/LSECoreModelTexture.h"
#ifdef LSE_WINDOWS
#include "Windows/LSEWindow.h"
#include <cfloat>
#elif defined( LSE_OSX )
#include <ApplicationServices/ApplicationServices.h>
//#include <ApplicationServices/CoreGraphics/CGDirectDisplay.h>
#include "OSX/LSEWindow.h"
#endif	// #ifdef LSE_WINDOWS


namespace lse {

	// == Members.
	/**
	 * The main window.
	 */
	CWindow * CEngine::m_pwWindow = NULL;
	
	// == Functions.
	/**
	 * Initialize the engine.
	 *
	 * \param _eiParms Initialization parameters.
	 */
	void LSE_CALL CEngine::InitEngine( const CEngine::LSE_ENGINE_INIT &_eiParms ) {
		CMemLib::Init( _eiParms.ui32HeapSize, _eiParms.bHeapIsGrowable != 0 );
		CMathLib::Init();
		CFilesEx::InitFilesEx();
		CGraphicsLib::InitGraphicsLib();
		/*CModelFile::InitModelLib();
		C2dAnimLib::Init2dAnimLib();
		CTerrain::InitTerrainLib();*/
		CSoundLib::InitSoundLibrary();
		CNetLib::InitNetwork();
		CImageLib::InitImageLibrary();
		CModelLib::InitModelLibrary();


		// Add search directories for textures.
		/*CCoreModelTexture::AddTextureSearchDir( "Data/" );
		CCoreModelTexture::AddTextureSearchDir( "Data/Tex/" );
		CCoreModelTexture::AddTextureSearchDir( "Data/Textures/" );
		CCoreModelTexture::AddTextureSearchDir( "./" );
		CCoreModelTexture::AddTextureSearchDir( "Tex/" );
		CCoreModelTexture::AddTextureSearchDir( "Textures/" );
		CCoreModelTexture::AddTextureSearchDir( "Resources/" );
		CCoreModelTexture::AddTextureSearchDir( "Resources/Tex/" );
		CCoreModelTexture::AddTextureSearchDir( "Resources/Textures/" );
		CCoreModelTexture::AddTextureSearchDir( "Res/" );
		CCoreModelTexture::AddTextureSearchDir( "Res/Tex/" );
		CCoreModelTexture::AddTextureSearchDir( "Res/Textures/" );*/

#ifdef LSE_WINDOWS
		// Set the working directory to here.
		WCHAR szBuffer[LSF_MAX_PATH];
		uint32_t ui32SetDir = ::SetCurrentDirectoryW( CFileLib::GetSelfDir( szBuffer ) );
		if ( ui32SetDir == 0 ) {
			uint32_t ui32Error = ::GetLastError();
			WCHAR * pwcErrorBuffer;
			::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				ui32Error,
				MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
				reinterpret_cast<LPWSTR>(&pwcErrorBuffer),
				0, NULL );

			::OutputDebugStringW( L"Error setting current directory: " );
			::OutputDebugStringW( pwcErrorBuffer );

			::LocalFree( pwcErrorBuffer );
		}

		// This is the main game thread.  It should be set to run on only a single processor.
		// TODO: Be sure to benchmark this.  The theory is that having the main game thread run on multiple
		//	cores could cause problems with QueryPerformanceCounter(), but in practice I have never encountered this.
		// If this is slower and no bugs really exist with QueryPerformanceCounter() on multiple cores, remove this.
		//DWORD_PTR dwptrProc, dwptrSystem;
		//if ( ::GetProcessAffinityMask( ::GetCurrentProcess(), &dwptrProc, &dwptrSystem ) ) {
		//	// Find the lowest bit and set this thread to run on that processor.
		//	for ( uint32_t I = 0; I < sizeof( DWORD_PTR ) << 3; ++I ) {
		//		if ( (dwptrProc & (1 << I)) && (dwptrSystem & (1 << I)) ) {
		//			::SetThreadAffinityMask( ::GetCurrentThread(), (1 << I) );
		//			break;
		//		}
		//	}
		//}
		

		// Inform windows not to enter power-saving modes.
		::SetThreadExecutionState( ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED );
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Secondary initialization.  Called after a custom game class has been created.
	 *
	 * \param _esiParms Initialization parameters.
	 */
	void LSE_CALL CEngine::SecondaryInit(
#if defined( LSE_IPHONE ) || defined( LSA_TOOLS )
		const LSE_ENGINE_SECONDARY_INIT &/*_esiParms*/
#else
		const LSE_ENGINE_SECONDARY_INIT &_esiParms
#endif	// #if defined( LSE_MAC )
		) {
#ifndef LSA_TOOLS
#if !defined( LSE_IPHONE )
		if ( m_pwWindow ) {
			CStd::DebugPrintA( "CEngine::SecondaryInit() may only be called once." );
			return;
		}

		m_pwWindow = LSENEW CWindow();
		m_pwWindow->SetGamePointer( _esiParms.pgGame );
		LSE_ENGINE_SECONDARY_INIT esiCopy = _esiParms;
#if defined( LSE_WINDOWS )
		
		RECT rRect;
		::GetClientRect( ::GetDesktopWindow(), &rRect );
		if ( esiCopy.ui32FullWidth == 0 ) {
			esiCopy.ui32FullWidth = rRect.right;
		}
		if ( esiCopy.ui32FullHeight == 0 ) {
			esiCopy.ui32FullHeight = rRect.bottom;
		}
#elif defined( LSE_OSX )
		CGDirectDisplayID ddiMainDisp = CGMainDisplayID();
		if ( esiCopy.ui32FullWidth == 0 ) {
			esiCopy.ui32FullWidth = static_cast<uint32_t>(::CGDisplayPixelsWide( ddiMainDisp ));
		}
		if ( esiCopy.ui32FullHeight == 0 ) {
			esiCopy.ui32FullHeight = static_cast<uint32_t>(::CGDisplayPixelsHigh( ddiMainDisp ));
		}
#endif	// #if defined( LSE_WINDOWS )

		// Default window dimensions are used for switching between full-screen and windowed mode.
		m_pwWindow->SetDefaultWindowDimensions( esiCopy.ui32WindowWidth, esiCopy.ui32WindowHeight, esiCopy.ui32FullWidth, esiCopy.ui32FullHeight );
		// Finally we actually create a window using one set of sizes or the other, depending on whether we want a full-screen window or
		//	a windowed window.
		m_pwWindow->CreateRenderWindow( esiCopy.bFullScreen ? esiCopy.ui32FullWidth : esiCopy.ui32WindowWidth,
			esiCopy.bFullScreen ? esiCopy.ui32FullHeight : esiCopy.ui32WindowHeight,
			esiCopy.bFullScreen, 32,
			esiCopy.pcWindowTitle );
#endif	// #if !defined( LSE_IPHONE )

#ifdef LSG_DX9
		CDirectX9::LSG_DIRECTX9_INIT diInitMe;
		diInitMe.hWnd = m_pwWindow->GetWindow();
		diInitMe.bWindowed = !esiCopy.bFullScreen;
		CGraphicsLib::InitGraphicsLibApi( &diInitMe );
#endif	// #ifdef LSG_DX9

#ifdef LSG_DX10
		CDirectX10::LSG_DIRECTX10_INIT diInitMe;
		diInitMe.hWnd = m_pwWindow->GetWindow();
		diInitMe.bWindowed = !esiCopy.bFullScreen;
		CGraphicsLib::InitGraphicsLibApi( &diInitMe );
#endif	// #ifdef LSG_DX10

#ifdef LSG_DX11
		CDirectX11::LSG_DIRECTX11_INIT diInitMe;
		diInitMe.hWnd = m_pwWindow->GetWindow();
		diInitMe.bWindowed = !esiCopy.bFullScreen;
		CGraphicsLib::InitGraphicsLibApi( &diInitMe );
#endif	// #ifdef LSG_DX11

#ifdef LSG_VKN
		CVulkan::LSG_VULKAN_INIT viInitMe;
#ifdef LSE_WINDOWS
		viInitMe.hWnd = m_pwWindow->GetWindow();
		viInitMe.hInst = ::GetModuleHandleW( NULL );
#endif	// #ifdef LSE_WINDOWS
		viInitMe.pcWindowName = esiCopy.pcWindowTitle;
		viInitMe.ui32AppVersion = esiCopy.ui32Version;
		CGraphicsLib::InitGraphicsLibApi( &viInitMe );
#endif	// #ifdef LSG_VKN

#else	// LSA_TOOLS
		CGraphicsLib::InitGraphicsLibApi( NULL );
#endif	// #ifndef LSA_TOOLS

		/*CModelInstance::m_mmModelMan.SetAnimMan( &CModelInstance::m_amAnimMan );
		CDrawableModelInstance::m_mmDrawableModelMan.SetAnimMan( &CModelInstance::m_amAnimMan );*/

#ifndef LSA_TOOLS
		CGraphicsLib::PostInitGraphicsLib();
#endif	// #ifndef LSA_TOOLS
		// After the window has been created, we initialize the sound library.
		CSoundLib::PostInitSoundLibrary();

#if defined( LSE_WINDOWS ) && !defined( LSE_WIN64 )
		_controlfp( _PC_24, _MCW_PC );
		_controlfp( _RC_NEAR, _MCW_RC );
#endif	// #if defined( LSE_WINDOWS ) && !defined( LSE_WIN64 )
	}

	/**
	 * Begin primary engine execution.  This causes the game class to be ticked etc.
	 *
	 * \return Returns a shut-down code.
	 */
	int32_t LSE_CALL CEngine::Run() {
#ifdef LSE_WINDOWS
		if ( !m_pwWindow ) { return -1; }
		return m_pwWindow->Run();
#else
		return -1;
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Shut down the engine.
	 */
	void LSE_CALL CEngine::DestroyEngine() {
#if defined( LSE_WINDOWS )
		// No longer need to be hold a lock on power-saving mode.
		::SetThreadExecutionState( ES_CONTINUOUS );

		LSEDELETE m_pwWindow;
		m_pwWindow = NULL;
#endif	// #if defined( LSE_WINDOWS )

		CModelLib::ShutDownModelLibrary();
		CImageLib::ShutDownImageLibrary();
		CNetLib::ShutDownNetwork();
		CSoundLib::ShutDownSoundLibrary();
		CFilesEx::ShutDownFilesEx();
		/*CCoreModelTexture::DelTextureSearchDirs();*/
		CGraphicsLib::DestroyGraphicsLib();
		
		// Can't destroy the memory manager on iOS devices because the OS was using it too.
#ifndef LSE_IPHONE
		CMemLib::Destroy();
#endif	// #ifndef LSE_IPHONE
	}
	
}	// namespace lse
