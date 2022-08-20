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
 * Description: The base class for generating states.  The default implementation generates predefined
 *	states.  Inherit from this class to generate your own game-specific states.
 * Generated states MUST be created with LSENEW.
 */

#include "LSEStateFactory.h"


namespace lse {

	// == Various constructors.
	LSE_CALL CStateFactory::CStateFactory() {
	}
	LSE_CALL CStateFactory::~CStateFactory() {
	}

	// == Functions.
	/**
	 * Creates a state by ID.
	 *
	 * \param _ui32Id The unique ID of the state to create.
	 * \return Returns the created state or NULL if none could be created.
	 */
	CState * LSE_CALL CStateFactory::GetState( uint32_t _ui32Id ) const {
		switch ( static_cast<int32_t>(_ui32Id) ) {
			case LSE_GS_DEFAULT : {
				//return LSENEW CDefaultState();
			}
		}
		return NULL;
	}

}	// namespace lse
