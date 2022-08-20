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


#ifndef __LSE_ENGINE_H__
#define __LSE_ENGINE_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the compression library.
#include "LSCCompressionLib.h"
using namespace ::lsc;

// Include the model library.
#include "LSDModelLib.h"
using namespace ::lsd;

// Include the file library.
#include "LSFFileLib.h"
using namespace ::lsf;

// Include the graphics library.
#include "LSGGraphicsLib.h"
using namespace ::lsg;

// Include the threading library.
#include "LSHThreadLib.h"
using namespace ::lsh;

// Include the image library.
#include "LSIImageLib.h"
using namespace ::lsi;

// Include the math library.
#include "LSMMathLib.h"
using namespace ::lsm;

// Include the network library.
#include "LSNNetLib.h"
using namespace ::lsn;

// Include the physics library.
#include "LSPPhysicsLib.h"
using namespace ::lsp;

// Include the sprite library.
#include "LSRSpriteLib.h"
using namespace ::lsr;

// Include the terrain library.
/*#include "LSTTerrainLib.h"
using namespace ::lst;*/

// Include the sound library.
#include "LSSSoundLib.h"
using namespace ::lss;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the LSTL.
#include "LSTLib.h"
using namespace ::lstl;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * If using a constant time step, specify the amount here (in microseconds), or set to 0ULL for non-constant time-stepping.
 *	Time-steps are used only for calls to Tick(), which are meant to advance game logic and physics.  Calls to Draw() are not
 *	affected by this value.
 */
#ifndef LSE_CONSTANTTIMESTEP
#define LSE_CONSTANTTIMESTEP		(1000000ULL / 45ULL)
#endif	// #ifndef LSE_CONSTANTTIMESTEP

namespace lse {

	/*
	 * Class CEngine
	 *
	 * Description: Cross-platform file functions.
	 */
	class CEngine {
	public :
		// == Types.
		/**
		 * Engine initialization parameters.  Call InitEngine() before creating your custom game class.
		 *	After creating your custom game class, call SecondaryInit().
		 */
		typedef struct LSE_ENGINE_INIT {
			/**
			 * Size of the default heap used by new and delete.
			 */
			uint32_t				ui32HeapSize;

			/**
			 * If true, the default heap is growable.
			 */
			LSBOOL					bHeapIsGrowable;
		} * LPLSE_ENGINE_INIT, * const LPCLSE_ENGINE_INIT;

		/**
		 * Secondary initialization phase.  Used with SecondaryInit() after calling InitEngine() and then
		 *	creating a game class.
		 */
		typedef struct LSE_ENGINE_SECONDARY_INIT {
			/** Pointer to the game class.  This will normally be created on the stack inside main(), and
			 *	will normally be a custom class that inherits from CGame. */
			class CGame *			pgGame;

			/** Starting windowed-mode width. */
			uint32_t				ui32WindowWidth;

			/** Starting windowed-mode height. */
			uint32_t				ui32WindowHeight;

			/** Starting full-screen width. */
			uint32_t				ui32FullWidth;

			/** Starting full-screen height. */
			uint32_t				ui32FullHeight;

			/** Window title in UTF-8 format. */
			const char *			pcWindowTitle;

			/** Full-screen? */
			LSBOOL					bFullScreen;

			/** Product version. */
			uint32_t				ui32Version;
		} * LPLSE_ENGINE_SECONDARY_INIT, * const LPCLSE_ENGINE_SECONDARY_INIT;

		
		// == Functions.
		/**
		 * Initialize the engine.
		 *
		 * \param _eiParms Initialization parameters.
		 */
		static void LSE_CALL		InitEngine( const LSE_ENGINE_INIT &_eiParms );

		/**
		 * Secondary initialization.  Called after a custom game class has been created.
		 *
		 * \param _esiParms Initialization parameters.
		 */
		static void LSE_CALL		SecondaryInit( const LSE_ENGINE_SECONDARY_INIT &_esiParms );

		/**
		 * Begin primary engine execution.  This causes the game class to be ticked etc.
		 *
		 * \return Returns a shut-down code.
		 */
		static int32_t LSE_CALL		Run();
		
		/**
		 * Shut down the engine.
		 */
		static void LSE_CALL		DestroyEngine();
		

	protected :
		// == Members.
		/**
		 * The main window.
		 */
		static class CWindow *	m_pwWindow;

#ifdef LSE_OSX
		
#endif	// #ifdef LSE_OSX
	};

}	// namespace lse

#endif	// __LSE_ENGINE_H__
