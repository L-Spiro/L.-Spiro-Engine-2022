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
 * Description: A pairing between a shader key and a name key.  Added meta-data allows the shader manager to
 *	search for existing matches more quickly.
 */


#ifndef __LSD_SHADERNAMEKEYPAIR_H__
#define __LSD_SHADERNAMEKEYPAIR_H__

#include "../LSDModelLib.h"
#include "LSDNameKey.h"
#include "LSDShaderKey.h"

namespace lsd {

	/** A pairing between a shader key and a name key.  Added meta-data allows the shader manager to
	 *	search for existing matches more quickly. */
	typedef struct LSD_SHADERNAMEKEYPAIR {
		LSD_SHADERKEY									skShaderKey;					/**< The shader key. */
		CNameKey										nkNameKey;						/**< Translator name. */
		void *										pvMeta;							/**< Meta-data. */


		// == Various constructors.
		LSE_CALLCTOR									LSD_SHADERNAMEKEYPAIR();
	} * LPLSD_SHADERNAMEKEYPAIR, * const LPCLSD_SHADERNAMEKEYPAIR;


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR LSD_SHADERNAMEKEYPAIR::LSD_SHADERNAMEKEYPAIR() :
		pvMeta( NULL ) {
	}

}	// namespace lsd

#endif	// __LSD_SHADERNAMEKEYPAIR_H__
