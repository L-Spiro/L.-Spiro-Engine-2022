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
 * Description: Vulkan texture constants.
 */


#ifndef __LSG_VULKANTEXTURECONSTANTS_H__
#define __LSG_VULKANTEXTURECONSTANTS_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_VKN

#include "../Vulkan/LSGVulkan.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Texture usage flags. */
	enum LSG_TEXTURE_USAGE_TYPES {
		LSG_TUT_SETONLY							= (1 << 0),										/**< Texture data is never read or written to. */
		LSG_TUT_STANDARD						= (1 << 1),										/**< Texture data is not read or written to often by the CPU. */
		LSG_TUT_DYNAMIC							= (1 << 2),										/**< Texture data is updated frequently. */
		LSG_TUT_RENDERTARGET					= (1 << 3),										/**< The resource will be a render target. */

		LSG_TUT_FORCE_DWORD						= 0x7FFFFFFF
	};

}	// namespace lsg

#endif	// #ifdef LSG_VKN

#endif	// __LSG_VULKANTEXTURECONSTANTS_H__
