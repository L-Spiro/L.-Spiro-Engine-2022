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
 * Description: A framebuffer describes what render targets are set at a time.
 */


#ifndef __LSR_FRAMEBUFFER_H__
#define __LSR_FRAMEBUFFER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_DX11 )
#include "../DirectX11Framebuffer/LSGDirectX11FramebufferBase.h"
#elif defined( LSG_DX9 )
#include "../DirectX9Framebuffer/LSGDirectX9FramebufferBase.h"
#elif defined( LSG_METAL )
#include "../MetalFramebuffer/LSGMetalFramebufferBase.h"
#elif defined( LSG_OGL )
#include "../OpenGLFramebuffer/LSGOpenGlFramebufferBase.h"
#elif defined( LSG_OGLES )
#include "../OpenGLESFramebuffer/LSGOpenGlEsFramebufferBase.h"
#elif defined( LSG_VKN )
#endif

namespace lsg {

	/**
	 * Class CFramebuffer
	 * \brief A framebuffer describes what render targets are set at a time.
	 *
	 * Description: A framebuffer describes what render targets are set at a time.
	 */
	class CFramebuffer : public
#if defined( LSG_DX11 )
		CDirectX11FramebufferBase
#elif defined( LSG_DX9 )
		CDirectX9FramebufferBase
#elif defined( LSG_METAL )
		CMetalFramebufferBase
#elif defined( LSG_OGL )
		COpenGlFramebufferBase
#elif defined( LSG_OGLES )
		COpenGlEsFramebufferBase
#elif defined( LSG_VKN )
		CVulkanFramebufferBase
#endif
	{
	public :
		// == Various constructors.
		LSE_CALLCTOR							CFramebuffer();
		LSE_CALLCTOR							~CFramebuffer();


		// == Functions.


	protected :
		// == Members.


	private :
#if defined( LSG_DX11 )
		typedef CDirectX11FramebufferBase		Parent;
#elif defined( LSG_DX9 )
		typedef CDirectX9FramebufferBase		Parent;
#elif defined( LSG_METAL )
		typedef CMetalFramebufferBase			Parent;
#elif defined( LSG_OGL )
		typedef COpenGlFramebufferBase			Parent;
#elif defined( LSG_OGLES )
		typedef COpenGlEsFramebufferBase		Parent;
#elif defined( LSG_VKN )
		typedef CVulkanFramebufferBase			Parent;
#endif
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// __LSR_FRAMEBUFFER_H__
