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
 * Description: Direct3D 11 texture constants.
 */


#ifndef __LSG_DIRECTX11TEXTURECONSTANTS_H__
#define __LSG_DIRECTX11TEXTURECONSTANTS_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../DirectX11/LSGDirectX11.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Texture usage flags. */
	enum LSG_TEXTURE_USAGE_TYPES {
		LSG_TUT_SETONLY							= D3D11_USAGE_IMMUTABLE,						/**< Texture data is never read or written to. */
		LSG_TUT_STANDARD						= D3D11_USAGE_DEFAULT,							/**< Texture data is not read or written to often by the CPU. */
		LSG_TUT_DYNAMIC							= D3D11_USAGE_DYNAMIC,							/**< Texture data is updated frequently. */
		LSG_TUT_RENDERTARGET					= (1 << 16),									/**< The resource will be a render target. */

		LSG_TUT_FORCE_DWORD						= 0x7FFFFFFF
	};

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11TEXTURECONSTANTS_H__
