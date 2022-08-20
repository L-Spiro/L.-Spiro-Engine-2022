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


#ifndef __LSE_STATEFACTORY_H__
#define __LSE_STATEFACTORY_H__

#include "../LSEEngine.h"
//#include "LSEDefaultState.h"
#include "LSEState.h"

namespace lse {

	/**
	 * Class CStateFactory
	 *
	 * Description: Base class for generating states.
	 */
	class CStateFactory {
	public :
		// == Various constructors.
		LSE_CALL								CStateFactory();
		virtual LSE_CALL						~CStateFactory();


		// == Functions.
		/**
		 * Creates a state by ID.
		 *
		 * \param _ui32Id The unique ID of the state to create.
		 * \return Returns the created state or NULL if none could be created.
		 */
		virtual CState * LSE_CALL				GetState( uint32_t _ui32Id ) const;
	};

}	// namespace lse

#endif	// __LSE_STATEFACTORY_H__
