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
 * Description: Core of the graphics library.
 */

#include "LSGGraphicsLib.h"
#include "Gfx/LSGGfx.h"
/*#include "Sprites/LSGSprite.h"*/


namespace lsg {

	// == Functions.
	/**
	 * Initialize the graphics library.
	 */
	void LSE_CALL CGraphicsLib::InitGraphicsLib() {
	}

	/**
	 * Secondary initialization of the graphics library.  Called after the window is created.
	 */
	void LSE_CALL CGraphicsLib::PostInitGraphicsLib() {
#ifdef LSG_OGL
		COpenGl::PostInitOpenGl();
#endif	// #ifdef LSG_OGL

#ifdef LSG_OGLES
		COpenGlEs::PostInitOpenGlEs();
#endif	// #ifdef LSG_OGLES

#ifdef LSG_METAL
		CMetal::PostInitMetal();
#endif	// #ifdef LSG_METAL

#ifdef LSG_DX9
		CDirectX9::PostInitDirectX9();
#endif	// #ifdef LSG_DX9

#ifdef LSG_DX11
		CDirectX11::PostInitDirectX11();
#endif	// #ifdef LSG_DX11
		//CGfx::ApplyDefaultStates();
		//CGfxBase::InitFnd();
	}

	/**
	 * Destroy the graphics library.
	 */
	void LSE_CALL CGraphicsLib::DestroyGraphicsLib() {
		/*CSprite::ShutDownSprites();
		CGfxBase::ShutDownFnd();*/
	}

	/**
	 * Initialize the operating system graphics API.  Must be done after
	 *	a window has been created.  Should not be called by the users.  Let the engine handle this.
	 *
	 * \param _pvParm The meaning of this value changes depending on the graphics API and operating system.
	 */
	void LSE_CALL CGraphicsLib::InitGraphicsLibApi( void * _pvParm ) {
		/*CSprite::ShutDownSprites();
		CGfxBase::InitFnd();*/
#ifdef LSG_OGL
		COpenGl::InitOpenGl( _pvParm );
#endif	// #ifdef LSG_OGL

#ifdef LSG_OGLES
		COpenGlEs::InitOpenGlEs( _pvParm );
#endif	// #ifdef LSG_OGLES

#ifdef LSG_METAL
		CMetal::InitMetal( _pvParm );
#endif	// #ifdef LSG_METAL

#ifdef LSG_VKN
		CVulkan::InitVulkan( reinterpret_cast<CVulkan::LPCLSG_VULKAN_INIT>(_pvParm) );
#endif	// #ifdef LSG_METAL

#ifdef LSG_DX9
		CDirectX9::InitDirectX9( reinterpret_cast<CDirectX9::LPCLSG_DIRECTX9_INIT>(_pvParm) );
#endif	// #ifdef LSG_DX9

#ifdef LSG_DX11
		CDirectX11::InitDirectX11( reinterpret_cast<CDirectX11::LPCLSG_DIRECTX11_INIT>(_pvParm) );
#endif	// #ifdef LSG_DX11
	}

	/**
	 * Inform tha graphics library that the rendering window is being destroyed.
	 */
	void LSE_CALL CGraphicsLib::DestroyGraphicsLibWindow() {
		/*CSprite::ShutDownSprites();
		CGfxBase::ShutDownFnd();*/
#ifdef LSG_OGL
		COpenGl::DestroyOpenGl();
#endif	// #ifdef LSG_OGL

#ifdef LSG_OGLES
		COpenGlEs::DestroyOpenGlEs();
#endif	// #ifdef LSG_OGLES

#ifdef LSG_DX9
		CDirectX9::DestroyDirectX9();
#endif	// #ifdef LSG_DX9

#ifdef LSG_DX11
		CDirectX11::DestroyDirectX11();
#endif	// #ifdef LSG_DX11

#ifdef LSG_VKN
		CVulkan::DestroyVulkan();
#endif	// #ifdef LSG_VKN
	}

	/**
	 * Activate the main rendering context.  Does nothing in DirectX.
	 */
	void LSE_CALL CGraphicsLib::MakeCurrent() {
#ifdef LSG_OGL
		COpenGl::MakeCurrent();
#endif	// #ifdef LSG_OGL

#ifdef LSG_OGLES
		COpenGlEs::MakeCurrent();
#endif	// #ifdef LSG_OGLES
	}

	/**
	 * Free any temporary RAM that may be in use.
	 */
	void LSE_CALL CGraphicsLib::FreeTmpResources() {
/*#ifdef LSG_OGL
		COpenGl::FreeTmpResources();
#endif	// #ifdef LSG_OGL
#ifdef LSG_OGLES
		COpenGlEs::FreeTmpResources();
#endif	// #ifdef LSG_OGLES*/
	}

}	// namespace lsg
