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
 * Description: Enumerates the types of entities known to the engine.  Add custom entities after 
 */


#ifndef __LSE_ENTITYTYPES_H__
#define __LSE_ENTITYTYPES_H__

#include "../LSEEngine.h"

namespace lse {

	// == Enumerations.
	/** Types of entities known to the engine.  Support for more can easily be added. */
	enum LSE_ENTITY_TYPES {
		LSE_ET_ENTITY				= 0,													/**< CEntity. */
		LSE_ET_ACTOR				= (1UL << 0UL),											/**< CActor. */
		LSE_ET_MESH					= (1UL << 1UL),											/**< CMesh. */
		LSE_ET_MODEL				= (1UL << 2UL),											/**< CModelInstance. */
		LSE_ET_CAM					= (1UL << 3UL),											/**< CCamera. */

		LSE_ET_CUSTOM				= 10UL,													/**< Add custom entities starting with this shift. */
	};

}	// namespace lse

#endif	// __LSE_ENTITYTYPES_H__

