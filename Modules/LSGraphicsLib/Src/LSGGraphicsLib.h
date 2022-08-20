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


#ifndef __LSG_GRAPHICSLIB_H__
#define __LSG_GRAPHICSLIB_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the compression library.
#include "LSCCompressionLib.h"
using namespace ::lsc;

// Include the file library.
#include "LSFFileLib.h"
using namespace ::lsf;

// Include the threading library.
#include "LSHThreadLib.h"
using namespace ::lsh;

// Include the image library.
#include "LSIImageLib.h"
using namespace ::lsi;

// Include the math library.
#include "LSMMathLib.h"
using namespace ::lsm;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the LSTL.
#include "LSTLib.h"
using namespace ::lstl;



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// API
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifdef LSE_OSX
#define LSG_OPENGL
#endif	// #ifdef LSE_OSX

#ifdef LSE_IPHONE
#ifdef LSE_METAL
#define LSG_METAL
#undef LSG_OGLES
#undef LSG_OGL
#undef LSG_DX9
#undef LSG_DX11
#undef LSG_VULKAN
#define LSG_API_METAL
#else	// #ifdef LSE_METAL
#define LSG_OGLES
#undef LSG_OGL
#undef LSG_DX9
#undef LSG_DX11
#undef LSG_VULKAN
#undef LSG_METAL
#define LSG_API_OPENGL
#endif	// #ifdef LSE_METAL
#endif	// #ifdef LSE_IPHONE

#ifdef LSG_OPENGL 
#define LSG_OGL
#undef LSG_OGLES
#undef LSG_DX9
#undef LSG_DX11
#undef LSG_VULKAN
#undef LSG_METAL
#define LSG_API_OPENGL
#endif	// #ifdef LSG_OPENGL

#ifdef LSG_DIRECTX9
#define LSG_DX9
#undef LSG_OGLES
#undef LSG_OGL
#undef LSG_DX11
#undef LSG_VULKAN
#undef LSG_METAL
#define LSG_API_DIRECTX
#endif	// #ifdef LSG_DIRECTX9

#ifdef LSG_DIRECTX11
#define LSG_DX11
#undef LSG_OGLES
#undef LSG_OGL
#undef LSG_DX9
#undef LSG_VULKAN
#undef LSG_METAL
#define LSG_API_DIRECTX
#endif	// #ifdef LSG_DIRECTX11

#ifdef LSG_VULKAN 
#define LSG_VKN
#undef LSG_OGLES
#undef LSG_OGL
#undef LSG_DX9
#undef LSG_DX11
#undef LSG_METAL
#define LSG_API_VULKAN
#endif	// #ifdef LSG_VULKAN

#if !defined( LSG_OGL ) && !defined( LSG_OGLES ) && !defined( LSG_DX9 ) && !defined( LSG_DX11 ) && !defined( LSG_METAL ) && !defined( LSG_VKN )
#error "No graphics API specified.  Define LSG_OPENGL, LSG_OPENGLES, LSG_DIRECTX9, LSG_DIRECTX11, or LSG_METAL."
#endif	// #if !defined( LSG_OGL ) && !defined( LSG_OGLES ) && !defined( LSG_DX9 ) && !defined( LSG_DX11 ) && !defined( LSG_METAL ) && !defined( LSG_VKN )



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// SETTINGS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifdef LSG_API_OPENGL
// For OpenGL ES, we have 16:16 as an option too.
typedef float											LSGREAL;
#elif defined( LSG_API_DIRECTX )
typedef float											LSGREAL;
#else
typedef float											LSGREAL;
#endif	// #ifdef LSG_API_OPENGL

namespace lsg {

	/**
	 * Class CGraphicsLib
	 * \brief Core of the graphics library.
	 *
	 * Description: Core of the graphics library.
	 */
	class CGraphicsLib {
	public :
		// == Functions.
		/**
		 * Initialize the graphics library.
		 */
		static void LSE_CALL							InitGraphicsLib();

		/**
		 * Secondary initialization of the graphics library.  Called after the window is created.
		 */
		static void LSE_CALL							PostInitGraphicsLib();

		/**
		 * Destroy the graphics library.
		 */
		static void LSE_CALL							DestroyGraphicsLib();

		/**
		 * Initialize the operating system graphics API.  Must be done after
		 *	a window has been created.  Should not be called by the users.  Let the engine handle this.
		 *
		 * \param _pvParm The meaning of this value changes depending on the graphics API and operating system.
		 */
		static void LSE_CALL							InitGraphicsLibApi( void * _pvParm );

		/**
		 * Inform tha graphics library that the rendering window is being destroyed.
		 */
		static void LSE_CALL							DestroyGraphicsLibWindow();

		/**
		 * Activate the main rendering context.  Does nothing in DirectX.
		 */
		static void LSE_CALL							MakeCurrent();

		/**
		 * Free any temporary RAM that may be in use.
		 */
		static void LSE_CALL							FreeTmpResources();


	protected :
		// == Members.
		
	};

}	// namespace lsg

#endif	// __LSG_GRAPHICSLIB_H__
