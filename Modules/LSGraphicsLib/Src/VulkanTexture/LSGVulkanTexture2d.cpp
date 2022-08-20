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

#include "LSGVulkanTexture2d.h"

#ifdef LSG_VKN

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVulkanTexture2d::CVulkanTexture2d() :
		m_ivImageView( VK_NULL_HANDLE ),
		m_ilLayout( VK_IMAGE_LAYOUT_GENERAL ) {
	}
	LSE_CALLCTOR CVulkanTexture2d::~CVulkanTexture2d() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL CVulkanTexture2d::Reset() {
		if ( m_ivImageView != VK_NULL_HANDLE ) {
			::vkDestroyImageView( CVulkan::GetVulkanDevice(), m_ivImageView, NULL );
			m_ivImageView = VK_NULL_HANDLE;
		}
		m_iImage.DestroyImage();
		m_dmMemory.FreeMemory();

		m_ilLayout = VK_IMAGE_LAYOUT_GENERAL;

		Parent::Reset();
	}

	/**
	 * Creates a Vulkan texture and fills it with our texel pvData.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL CVulkanTexture2d::CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total ) {
		VkFormat fFormat = VulkanFormat( _piImages->GetFormat(), _piImages->IsSRgb() );
		if ( fFormat == VK_FORMAT_UNDEFINED ) {
			CStd::DebugPrintA( "Unsupported texture format.\r\n" );
			return false;
		}
		CVulkanCommandBuffer::CBeginBuffer bbBegin( CVulkan::GetVulkanCommandBuffer() );

		VkFormatProperties fpFormatProps;
		::vkGetPhysicalDeviceFormatProperties( CVulkan::GetVulkanPhysicalDevice(), fFormat,
			&fpFormatProps );
		LSBOOL bNeedsStaging = (!(fpFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) ? true : false;

		VkImageCreateInfo iciImgCreateInfo = {};
		iciImgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		iciImgCreateInfo.pNext = NULL;
		iciImgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		iciImgCreateInfo.format = fFormat;
		iciImgCreateInfo.extent.width = _piImages->GetWidth();
		iciImgCreateInfo.extent.height = _piImages->GetHeight();
		iciImgCreateInfo.extent.depth = 1;
		iciImgCreateInfo.mipLevels = 1;
		iciImgCreateInfo.arrayLayers = 1;
		iciImgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		iciImgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
		iciImgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iciImgCreateInfo.usage = bNeedsStaging ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT;
		iciImgCreateInfo.queueFamilyIndexCount = 0;
		iciImgCreateInfo.pQueueFamilyIndices = NULL;
		iciImgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iciImgCreateInfo.flags = 0;

		// The mappable image (final image if direct write supported, otherwise staging).
		CVulkan::CImage iMappableImage;
		if ( !iMappableImage.CreateImage( &iciImgCreateInfo ) ) { return false; }

		VkMemoryAllocateInfo maiMemAlloc = {};
		maiMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		maiMemAlloc.pNext = NULL;
		maiMemAlloc.allocationSize = 0;
		maiMemAlloc.memoryTypeIndex = 0;
		
		// Allocate memory for it.
		VkMemoryRequirements mrMemReqs;
		::vkGetImageMemoryRequirements( CVulkan::GetVulkanDevice(), iMappableImage.Image(), &mrMemReqs );
		maiMemAlloc.allocationSize = mrMemReqs.size;

		if ( !CVulkan::MemoryTypeFromProperties( mrMemReqs.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			maiMemAlloc.memoryTypeIndex ) ) { return false; }

		CVulkan::CDeviceMemory dmMappableMemory;
		if ( !dmMappableMemory.AllocateMemory( &maiMemAlloc ) ) { return false; }

		VkResult rRes = ::vkBindImageMemory( CVulkan::GetVulkanDevice(), iMappableImage.Image(), dmMappableMemory.DeviceMemory(), 0 );
		if ( rRes != VK_SUCCESS ) { return false; }

		CVulkan::GetVulkanCommandBuffer().PipelineBarrierImage( iMappableImage.Image(), VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL );


		VkFence fCmdFence;
		CVulkan::GetVulkanCommandBuffer().QueueSubmit( false, &fCmdFence );

		VkImageSubresource isSubRes = {};
		isSubRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		isSubRes.mipLevel = 0;
		isSubRes.arrayLayer = 0;

		VkSubresourceLayout slLayout;
		void * pvData;

		// Get the subresource layout so we know what the row pitch is.
		::vkGetImageSubresourceLayout( CVulkan::GetVulkanDevice(), iMappableImage.Image(), &isSubRes, &slLayout );

		// Make sure command buffer is finished before mapping.
		CVulkan::GetVulkanCommandBuffer().WaitForFence( &fCmdFence );


		rRes = ::vkMapMemory( CVulkan::GetVulkanDevice(), dmMappableMemory.DeviceMemory(), 0, mrMemReqs.size, 0, &pvData );
		if ( rRes != VK_SUCCESS ) { return false; }

		uint8_t * pui8Dst = static_cast<uint8_t *>(pvData);
		const uint8_t * pui8Src = static_cast<const uint8_t *>(_piImages->GetBufferData());
		uint32_t ui32Pitch = CImageLib::GetRowSize( _piImages->GetFormat(), _piImages->GetWidth() );
		for ( uint32_t H = 0; H < _piImages->GetHeight(); ++H ) {
			CStd::MemCpy( pui8Dst, pui8Src, slLayout.rowPitch );
			pui8Dst += slLayout.rowPitch;
			pui8Src += ui32Pitch;
		}

		::vkUnmapMemory( CVulkan::GetVulkanDevice(), dmMappableMemory.DeviceMemory() );

		CVulkan::GetVulkanCommandBuffer().ResetCommandBuffer( 0 );
		CVulkan::GetVulkanCommandBuffer().BeginCommandBuffer();


		if ( !bNeedsStaging ) {
			m_iImage = iMappableImage;
			m_dmMemory = dmMappableMemory;
			m_ilLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			CVulkan::GetVulkanCommandBuffer().PipelineBarrierImage( m_iImage.Image(), VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_GENERAL, m_ilLayout );
		}
		else {
			assert( 0 );
		}


		CVulkan::GetVulkanCommandBuffer().EndCommandBuffer();
		CVulkan::GetVulkanCommandBuffer().QueueSubmit();


		VkImageViewCreateInfo ivciViewInfo = {};
		ivciViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ivciViewInfo.pNext = NULL;
		ivciViewInfo.image = VK_NULL_HANDLE;
		ivciViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivciViewInfo.format = fFormat;
		ivciViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		ivciViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		ivciViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		ivciViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		ivciViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivciViewInfo.subresourceRange.baseMipLevel = 0;
		ivciViewInfo.subresourceRange.levelCount = 1;
		ivciViewInfo.subresourceRange.baseArrayLayer = 0;
		ivciViewInfo.subresourceRange.layerCount = 1;

		// Create image view.
		ivciViewInfo.image = m_iImage.Image();
		rRes = ::vkCreateImageView( CVulkan::GetVulkanDevice(), &ivciViewInfo, NULL, &m_ivImageView );
		if ( rRes != VK_SUCCESS ) { return false; }

		return true;
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN
