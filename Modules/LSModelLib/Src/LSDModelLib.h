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
 * Description: The model library privides the engine with a model format that is very complete.  Models can
 *	be animated etc.  They generate their own shaders which can be overridden by the user.  IK support is
 *	included.
 */


#ifndef __LSD_MODELLIB_H__
#define __LSD_MODELLIB_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the compression library.
#include "LSCCompressionLib.h"
using namespace ::lsc;

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

// Include the physics library.
#include "LSPPhysicsLib.h"
using namespace ::lsp;

// Include the LSTL.
#include "LSTLib.h"
using namespace ::lstl;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

namespace lsd {

	// == Forward Declarations.
	class CModelManager;
	class CModelTextureManager;
	class CShaderManager;

	// == Enumerations.
	/** Metrics. */
	enum LSD_METRICS {
		LSD_M_MAX_LAYERS					= 12,
	};


	/**
	 * Class CModelLib
	 * \brief The model library privides the engine with a model format that is very complete.
	 *
	 * Description: The model library privides the engine with a model format that is very complete.  Models can
	 *	be animated etc.  They generate their own shaders which can be overridden by the user.  IK support is
	 *	included.
	 */
	class CModelLib {
	public :
		// == Functions.
		/**
		 * Initializes the model library.
		 */
		static void LSE_CALL					InitModelLibrary();

		/**
		 * Shuts down the model library.
		 */
		static void LSE_CALL					ShutDownModelLibrary();

		/**
		 * Clears any model resources that are not currently in use.
		 */
		static void LSE_CALL					SoftReset();

		/**
		 * Gets a reference to the model-library model manager.
		 *
		 * \return Returns a reference to the model-library model manager.
		 */
		static LSE_INLINE CModelManager & LSE_CALL
												SharedModelManager();

		/**
		 * Gets a reference to the model-library texture manager.
		 *
		 * \return Returns a reference to the model-library texture manager.
		 */
		static LSE_INLINE CModelTextureManager & LSE_CALL
												TextureManager();

		/**
		 * Gets a reference to the model-library shader manager.
		 *
		 * \return Returns a reference to the model-library shader manager.
		 */
		static LSE_INLINE CShaderManager & LSE_CALL
												ShaderManager();

	protected :
		// == Members.
		/** The model manager. */
		static CModelManager					m_mmModelManager;

		/** Shared model texture manager. Manages textures only for the model library. */
		static CModelTextureManager				m_mtmTextureManager;

		/** The model-library shader manager. */
		static CShaderManager					m_smShaderManager;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a reference to the model-library model manager.
	 *
	 * \return Returns a reference to the model-library model manager.
	 */
	LSE_INLINE CModelManager & LSE_CALL CModelLib::SharedModelManager() {
		return m_mmModelManager;
	}

	/**
	 * Gets a reference to the model-library texture manager.
	 *
	 * \return Returns a reference to the model-library texture manager.
	 */
	LSE_INLINE CModelTextureManager & LSE_CALL CModelLib::TextureManager() {
		return m_mtmTextureManager;
	}

	/**
	 * Gets a reference to the model-library shader manager.
	 *
	 * \return Returns a reference to the model-library shader manager.
	 */
	LSE_INLINE CShaderManager & LSE_CALL CModelLib::ShaderManager() {
		return m_smShaderManager;
	}

}	// namespace lsd

#endif	// __LSD_MODELLIB_H__
