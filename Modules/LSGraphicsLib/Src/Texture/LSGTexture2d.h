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
 * Description: A 2D texture.
 */


#ifndef __LSG_TEXTURE2D_H__
#define __LSG_TEXTURE2D_H__

#include "../LSGGraphicsLib.h"
#include "../DirectX11Texture/LSGDirectX11Texture2d.h"
#include "../DirectX9Texture/LSGDirectX9Texture2d.h"
#include "../MetalTexture/LSGMetalTexture2d.h"
#include "../OpenGLESTexture/LSGOpenGlEsTexture2d.h"
#include "../OpenGLTexture/LSGOpenGlTexture2d.h"
#include "../VulkanTexture/LSGVulkanTexture2d.h"
#include "SharedPtr/LSTLSharedPtr.h"

namespace lsg {

	/**
	 * Class CTexture2d
	 * \brief A 2D texture.
	 *
	 * Description: A 2D texture.
	 */
	class CTexture2d : public
#if defined( LSG_DX11 )
		CDirectX11Texture2d
#elif defined( LSG_DX9 )
		CDirectX9Texture2d
#elif defined( LSG_METAL )
		CMetalTexture2d
#elif defined( LSG_OGLES )
		COpenGlEsTexture2d
#elif defined( LSG_OGL )
		COpenGlTexture2d
#elif defined( LSG_VKN )
		CVulkanTexture2d
#endif	// #if defined( LSG_DX11 )
	{
	public :
		// == Various constructors.
		LSE_CALLCTOR							CTexture2d();
		LSE_CALLCTOR							~CTexture2d();


		// == Functions.
		/**
		 * Creates a texture and fills it with our texel data.  Mipmaps are generated if necessary.
		 *
		 * \param _piImages Array of images to use to create the texture.
		 * \param _ui32Total Number of images to which _piImages points.
		 * \param _ui32Usage Usage type.
		 * \param _ui32MultiSampleCount Multisample count.
		 * \param _ui32MultiSampleQuality Multisample quality.
		 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
		 */
		LSBOOL LSE_CALL							CreateTexture( const CImage * _piImages, uint32_t _ui32Total = 1,
			uint32_t _ui32Usage = LSG_TUT_STANDARD, uint32_t _ui32MultiSampleCount = 1, uint32_t _ui32MultiSampleQuality = 0 );

		/**
		 * Activates the texture in the given slot.
		 *
		 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
		 * \return Returns true if the texture was activated in the given slot.
		 */
		LSBOOL LSE_CALL							Activate( uint32_t _ui32Slot );


	protected :
		// == Members.


	private :
#if defined( LSG_DX11 )
		typedef CDirectX11Texture2d				Parent;
#elif defined( LSG_DX9 )
		typedef CDirectX9Texture2d				Parent;
#elif defined( LSG_METAL )
		typedef CMetalTexture2d					Parent;
#elif defined( LSG_OGLES )
		typedef COpenGlEsTexture2d				Parent;
#elif defined( LSG_OGL )
		typedef COpenGlTexture2d				Parent;
#elif defined( LSG_VKN )
		typedef CVulkanTexture2d				Parent;
#endif	// #if defined( LSG_DX11 )
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** A shared texture pointer. */
	typedef CSharedPtr<CTexture2d>				CSharedTexture2dPtr;

}	// namespace lsg

#endif	// __LSG_TEXTURE2D_H__
