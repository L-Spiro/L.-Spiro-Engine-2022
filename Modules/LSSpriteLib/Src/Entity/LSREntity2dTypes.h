/**
* Copyright L. Spiro 2016
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


#ifndef __LSR_ENTITYTYPES_H__
#define __LSR_ENTITYTYPES_H__

#include "../LSRSpriteLib.h"

namespace lsr {

	// == Enumerations.
	/** Types of entities known to the sprite engine.  Support for more can easily be added. */
	enum LSR_ENTITY_TYPES {
		LSR_ET_ENTITY2D				= 0,													/**< CEntity2d. */
		LSR_ET_ACTOR2D				= (1UL << 0UL),											/**< CActor2d. */
		LSR_ET_SPRITE				= (1UL << 1UL),											/**< CSprite. */
		LSR_ET_ANIMATEDSPRITE		= (1UL << 2UL),											/**< CAnimatedSprite. */
		LSR_ET_CAMERA2D				= (1UL << 3UL),											/**< CCamera2d. */
		LSR_ET_STATICLAYER			= (1UL << 4UL),											/**< CStaticLayer. */

		LSR_ET_CUSTOM				= 10UL,													/**< Add custom entities starting with this shift. */
	};

}	// namespace lsr

#endif	// __LSR_ENTITYTYPES_H__

