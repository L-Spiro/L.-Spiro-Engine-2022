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
 * Description: A Vulkan 2D texture.
 */


#ifndef __LSG_VULKANTEXTURE2D_H__
#define __LSG_VULKANTEXTURE2D_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_VKN

#include "../Texture/LSGTexture2dBase.h"
#include "../Vulkan/LSGVulkan.h"
#include "LSGVulkanTextureConstants.h"

namespace lsg {

	/**
	 * Class CVulkanTexture2d
	 * \brief A Vulkan 2D texture.
	 *
	 * Description: A Vulkan 2D texture.
	 */
	class CVulkanTexture2d : public CTexture2dBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVulkanTexture2d();
		LSE_CALLCTOR							~CVulkanTexture2d();


		// == Functions.
		/**
		 * Resets everything to scratch.
		 */
		void LSE_CALL							Reset();


	protected :
		// == Members.
		/** The image object. */
		CVulkan::CImage							m_iImage;

		/** The image view. */
		VkImageView								m_ivImageView;

		/** Image memory. */
		CVulkan::CDeviceMemory					m_dmMemory;

		/** Image layout. */
		VkImageLayout							m_ilLayout;


		// == Functions.
		/**
		 * Creates a Vulkan texture and fills it with our texel data.  Mipmaps are generated if necessary.
		 *
		 * \param _piImages Array of images to use to create the texture.
		 * \param _ui32Total Number of images to which _piImages points.
		 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
		 */
		LSBOOL LSE_CALL							CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total );

		/**
		 * Activates the texture in the given slot.
		 *
		 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
		 * \return Returns true if the texture was activated in the given slot.
		 */
		LSE_INLINE LSBOOL LSE_CALL				ActivateApi( uint32_t _ui32Slot );

		/**
		 * Gets this object's LSG_SHADER_RESOURCE ID.
		 *
		 * \return Returns this object's LSG_SHADER_RESOURCE ID.
		 */
		LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL
												ShaderResourceApi();

		/**
		 * Gets the Vulkan texture format given an LSImageLib format.
		 *
		 * \param _pfFormat The format to translate to Vulkan's texture format.
		 * \param _bSrgb If true, the format will include an _sRGB extension.
		 * \return Returns the Vulkan texture format given an LSImageLib format or VK_FORMAT_UNDEFINED if there is no conversion.
		 */
		static LSE_INLINE VkFormat LSE_FCALL	VulkanFormat( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bSrgb = false );


	private :
		// == Types.
		typedef CTexture2dBase					Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Activates the texture in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
	 * \return Returns true if the texture was activated in the given slot.
	 */
	LSE_INLINE LSBOOL LSE_CALL CVulkanTexture2d::ActivateApi( uint32_t /*_ui32Slot*/ ) {
		return true;
	}

	/**
	 * Gets this object's LSG_SHADER_RESOURCE ID.
	 *
	 * \return Returns this object's LSG_SHADER_RESOURCE ID.
	 */
	LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL CVulkanTexture2d::ShaderResourceApi() {
		return &m_ivImageView;
	}

	/**
	 * Gets the Vulkan texture format given an LSImageLib format.
	 *
	 * \param _pfFormat The format to translate to Vulkan's texture format.
	 * \param _bSrgb If true, the format will include an _sRGB extension.
	 * \return Returns the Vulkan texture format given an LSImageLib format or VK_FORMAT_UNDEFINED if there is no conversion.
	 */
	LSE_INLINE VkFormat LSE_FCALL CVulkanTexture2d::VulkanFormat( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bSrgb ) {
		if ( _bSrgb ) {
			static const VkFormat fBasicFormats[] = {
				VK_FORMAT_UNDEFINED,				// LSI_PF_A8
				VK_FORMAT_UNDEFINED,				// LSI_PF_R3G3B2
				VK_FORMAT_UNDEFINED,				// LSI_PF_R5G6B5
				VK_FORMAT_UNDEFINED,				// LSI_PF_R4G4B4A4
				VK_FORMAT_UNDEFINED,				// LSI_PF_R5G5B5A1
				VK_FORMAT_R8G8B8_SRGB,				// LSI_PF_R8G8B8
				VK_FORMAT_R8G8B8A8_SRGB,			// LSI_PF_R8G8B8A8
				VK_FORMAT_UNDEFINED,				// LSI_PF_R16G16B16A16
				VK_FORMAT_UNDEFINED,				// LSI_PF_R16G16B16A16F
				VK_FORMAT_UNDEFINED,				// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return fBasicFormats[_pfFormat-LSI_PF_A8]; }

			static const VkFormat fDxtFormats[] = {
				VK_FORMAT_BC1_RGBA_SRGB_BLOCK,		// LSI_PF_DXT1
				VK_FORMAT_BC2_SRGB_BLOCK,			// LSI_PF_DXT2
				VK_FORMAT_BC2_SRGB_BLOCK,			// LSI_PF_DXT3
				VK_FORMAT_BC3_SRGB_BLOCK,			// LSI_PF_DXT4
				VK_FORMAT_BC3_SRGB_BLOCK,			// LSI_PF_DXT5
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC4U
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC4S
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC5U
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC5S
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC6U
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC6S
				VK_FORMAT_UNDEFINED,				// LSI_PF_BC7U
				VK_FORMAT_BC7_SRGB_BLOCK,			// LSI_PF_BC7_SRGB
			};
			if ( CImageLib::IsCompressed( _pfFormat ) ) { return fDxtFormats[_pfFormat-LSI_PF_DXT1]; }
		}
		else {
			static const VkFormat fBasicFormats[] = {
				VK_FORMAT_UNDEFINED,				// LSI_PF_A8
				VK_FORMAT_UNDEFINED,				// LSI_PF_R3G3B2
				VK_FORMAT_B5G6R5_UNORM_PACK16,		// LSI_PF_R5G6B5
				VK_FORMAT_UNDEFINED,				// LSI_PF_R4G4B4A4 (DXGI_FORMAT_B4G4R4A4_UNORM supported from Windows 8)
				VK_FORMAT_B5G5R5A1_UNORM_PACK16,	// LSI_PF_R5G5B5A1
				VK_FORMAT_B8G8R8_UNORM,				// LSI_PF_R8G8B8
				VK_FORMAT_R8G8B8A8_UNORM,			// LSI_PF_R8G8B8A8
				VK_FORMAT_R16G16B16A16_UNORM,		// LSI_PF_R16G16B16A16
				VK_FORMAT_R16G16B16A16_SFLOAT,		// LSI_PF_R16G16B16A16F
				VK_FORMAT_R32G32B32A32_SFLOAT,		// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return fBasicFormats[_pfFormat-LSI_PF_A8]; }

			static const VkFormat fDxtFormats[] = {
				VK_FORMAT_BC1_RGBA_UNORM_BLOCK,		// LSI_PF_DXT1
				VK_FORMAT_BC2_UNORM_BLOCK,			// LSI_PF_DXT2
				VK_FORMAT_BC2_UNORM_BLOCK,			// LSI_PF_DXT3
				VK_FORMAT_BC3_UNORM_BLOCK,			// LSI_PF_DXT4
				VK_FORMAT_BC3_UNORM_BLOCK,			// LSI_PF_DXT5
				VK_FORMAT_BC4_UNORM_BLOCK,			// LSI_PF_BC4U
				VK_FORMAT_BC4_SNORM_BLOCK,			// LSI_PF_BC4S
				VK_FORMAT_BC5_UNORM_BLOCK,			// LSI_PF_BC5U
				VK_FORMAT_BC5_SNORM_BLOCK,			// LSI_PF_BC5S
				VK_FORMAT_BC6H_UFLOAT_BLOCK,		// LSI_PF_BC6U
				VK_FORMAT_BC6H_SFLOAT_BLOCK,		// LSI_PF_BC6S
				VK_FORMAT_BC7_UNORM_BLOCK,			// LSI_PF_BC7U
				VK_FORMAT_BC7_SRGB_BLOCK,			// LSI_PF_BC7_SRGB
			};
			if ( CImageLib::IsCompressed( _pfFormat ) ) { return fDxtFormats[_pfFormat-LSI_PF_DXT1]; }
		}
		return VK_FORMAT_UNDEFINED;
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN

#endif	// __LSG_VULKANTEXTURE2D_H__
