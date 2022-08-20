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

#include "LSDModelLib.h"
#include "Models/LSDModelManager.h"
#include "Shaders/LSDShaderManager.h"
#include "Textures/LSDModelTextureManager.h"


namespace lsd {

	// == Members.
	/** Shared model texture manager. Manages textures only for the model library. */
	CModelTextureManager CModelLib::m_mtmTextureManager;

	/** The model-library shader manager. */
	CShaderManager CModelLib::m_smShaderManager;

	/** The model manager. */
	CModelManager CModelLib::m_mmModelManager;

	// == Functions.
	/**
	 * Initializes the model library.
	 */
	void LSE_CALL CModelLib::InitModelLibrary() {
	}

	/**
	 * Shuts down the model library.
	 */
	void LSE_CALL CModelLib::ShutDownModelLibrary() {
		m_mmModelManager.Reset();
	}

	/**
	 * Clears any model resources that are not currently in use.
	 */
	void LSE_CALL CModelLib::SoftReset() {
		m_mmModelManager.SoftReset();
	}

}	// namespace lsd
