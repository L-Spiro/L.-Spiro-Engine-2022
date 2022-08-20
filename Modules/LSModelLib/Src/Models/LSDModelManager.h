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
 * Description: Ensures that no model file is loaded redundantly.
 */


#ifndef __LSD_MODELMANAGER_H__
#define __LSD_MODELMANAGER_H__

#include "../LSDModelLib.h"
#include "LSDSharedModel.h"
#include "Map/LSTLLinearMap.h"

namespace lsd {

	/**
	 * Class CModelManager
	 * \brief Ensures that no model file is loaded redundantly.
	 *
	 * Description: Ensures that no model file is loaded redundantly.
	 */
	class CModelManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CModelManager();
		LSE_CALLCTOR							~CModelManager();


		// == Functions.
		/**
		 * Loads (if necessary) the given model and returns a shared pointer.
		 *
		 * \param _pcPath Path to the model file to load.
		 * \return Returns a shared pointer to the file designated by its path.
		 */
		CSharedSharedModelPtr LSE_CALL			LoadModel( const char * _pcPath );

		/**
		 * Resets the manager back to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Clears any model resources that are not currently in use.
		 */
		void LSE_CALL							SoftReset();


	protected :
		// == Members.
		/** Loaded models. */
		CLinearMap<CString, CSharedSharedModelPtr>
												m_lmModels;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsd

#endif	// __LSD_MODELMANAGER_H__
