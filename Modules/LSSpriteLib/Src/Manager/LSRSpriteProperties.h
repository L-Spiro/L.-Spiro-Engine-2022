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
 * Description: Properties of a sprite include the presence of an alpha channel and various flags that might
 *	affect the sprite's shader etc.
 */


#ifndef __LSR_SPRITEPROPERTIES_H__
#define __LSR_SPRITEPROPERTIES_H__

#include "../LSRSpriteLib.h"

namespace lsr {

	/** Alpha flags. */
	enum LSR_ALPHA_FLAGS {
		LSR_AF_ALPHA_ON,													/**< Enables alpha on the sprite. */
		LSR_AF_ALPHA_OFF,													/**< Disables alpha on the sprite. */
		LSR_AF_ALPHA_AUTO,													/**< Enables or disables alpha conditionally based on whether alpha is present in the sprite's original image. */
	};

	/**
	 * Class LSR_SPRITE_PROPERTIES
	 * \brief Properties of a sprite include the presence of an alpha channel and various flags that might
	 *	affect the sprite's shader etc.
	 *
	 * Description: Properties of a sprite include the presence of an alpha channel and various flags that might
	 *	affect the sprite's shader etc.
	 */
	typedef struct LSR_SPRITE_PROPERTIES {
		LSR_ALPHA_FLAGS						afAlpha;						/**< Alpha flags. */
	} * LPLSR_SPRITE_PROPERTIES, * const LPCLSR_SPRITE_PROPERTIES;

}	// namespace lsr

#endif	// __LSR_SPRITEPROPERTIES_H__
