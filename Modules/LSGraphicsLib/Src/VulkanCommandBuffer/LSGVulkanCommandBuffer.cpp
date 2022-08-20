/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A Vulkan command buffer.
 */

#include "LSGVulkanCommandBuffer.h"

#ifdef LSG_VKN

#include "../Vulkan/LSGVulkan.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVulkanCommandBuffer::CVulkanCommandBuffer() :
		m_cbCmd( VK_NULL_HANDLE ),
		m_ui32Open( 0 ) {
	}
	LSE_CALLCTOR CVulkanCommandBuffer::~CVulkanCommandBuffer() {
		FreeCommandBuffer();
	}

	// == Functions.
	/**
	 * Allocates this command buffer.
	 *
	 * \return Returns true if the command buffer was allocated.
	 */
	LSBOOL LSE_CALL CVulkanCommandBuffer::AllocateCommandBuffer() {
		FreeCommandBuffer();

		VkCommandBufferAllocateInfo cbaiCmdAllocInfo = {};
		cbaiCmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbaiCmdAllocInfo.pNext = NULL;
		cbaiCmdAllocInfo.commandPool = CVulkan::GetVulkanCommandPool();
		cbaiCmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cbaiCmdAllocInfo.commandBufferCount = 1;

		if ( CVulkan::PrintVulkanError( "CVulkanCommandBuffer::AllocateCommandBuffer(): ",
			::vkAllocateCommandBuffers( CVulkan::GetVulkanDevice(), &cbaiCmdAllocInfo, &m_cbCmd ) ) ) {
			return false;
		}
		return true;
	}

	/**
	 * Destroys this command buffer.
	 */
	void LSE_CALL CVulkanCommandBuffer::FreeCommandBuffer() {
		if ( m_cbCmd != VK_NULL_HANDLE ) {
			::vkFreeCommandBuffers( CVulkan::GetVulkanDevice(), CVulkan::GetVulkanCommandPool(), 1, &m_cbCmd );
			m_cbCmd = VK_NULL_HANDLE;
		}
		m_ui32Open = 0;
	}

	/**
	 * Inserts an image barrier into the command buffer.
	 *
	 * \param _iImage The image to barrier.
	 * \param _iafAspectFlags What part of the image to barrier (color, depth, stencil, meta-data, or any combination).
	 * \param _ilOldLayout The old layout of the image (VK_IMAGE_LAYOUT_UNDEFINED for new images).
	 * \param _ilNewLayout The old layout of the image.
	 */
	void LSE_CALL CVulkanCommandBuffer::PipelineBarrierImage( VkImage _iImage, VkImageAspectFlags _iafAspectFlags,
		VkImageLayout _ilOldLayout, VkImageLayout _ilNewLayout ) {
		assert( m_ui32Open );
		VkImageMemoryBarrier imbBarrier = {};
		imbBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imbBarrier.pNext = NULL;
		imbBarrier.srcAccessMask = 0;
		imbBarrier.dstAccessMask = 0;
		imbBarrier.oldLayout = _ilOldLayout;
		imbBarrier.newLayout = _ilNewLayout;
		imbBarrier.image = _iImage;
		imbBarrier.subresourceRange.aspectMask = _iafAspectFlags;
		imbBarrier.subresourceRange.baseMipLevel = 0;
		imbBarrier.subresourceRange.levelCount = 1;
		imbBarrier.subresourceRange.layerCount = 1;

		if ( _ilOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) {
			imbBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		if ( _ilNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
			// Make sure anything that was copying from this image has completed.
			imbBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		}

		if ( _ilNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
			// Make sure any copy or CPU writes to the image are flushed.
			imbBarrier.srcAccessMask =
				VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			imbBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}

		if ( _ilNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) {
			imbBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		}

		if ( _ilNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
			imbBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}

		const VkPipelineStageFlags psfSrcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		const VkPipelineStageFlags psfDstStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		::vkCmdPipelineBarrier( m_cbCmd, psfSrcStages, psfDstStages, 0, 0, NULL, 0, NULL,
			1, &imbBarrier );
	}

	/**
	 * Queues the command buffer for execution.
	 *
	 * \param _bWaitFence If true, waits on a fence to finish.
	 * \param _pfFence If _bWaitFence is false, _pfFence is filled with the fence on which to wait later.  Otherwise this is set to NULL on
	 *	return.
	 * \param _psfDstFlags Optional pipeline stage flag indicating when the semaphore wait will occur.
	 */
	void LSE_CALL CVulkanCommandBuffer::QueueSubmit( LSBOOL _bWaitFence, VkFence * _pfFence,
		VkPipelineStageFlags _psfDstFlags ) {
		VkResult LSE_DEBUG_ONLY rRes;

		// Queue the command buffer for execution.
		VkFenceCreateInfo fciFenceInfo;
		VkFence fDrawFence;
		fciFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fciFenceInfo.pNext = NULL;
		fciFenceInfo.flags = 0;
		rRes = ::vkCreateFence( CVulkan::GetVulkanDevice(), &fciFenceInfo, NULL, &fDrawFence );
		assert( rRes == VK_SUCCESS );

		//VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		VkSubmitInfo siSubmitInfo = {};
		siSubmitInfo.pNext = NULL;
		siSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		siSubmitInfo.waitSemaphoreCount = 0;
		siSubmitInfo.pWaitSemaphores = NULL;
		siSubmitInfo.pWaitDstStageMask = &_psfDstFlags;	// VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
		siSubmitInfo.commandBufferCount = 1;
		siSubmitInfo.pCommandBuffers = &m_cbCmd;
		siSubmitInfo.signalSemaphoreCount = 0;
		siSubmitInfo.pSignalSemaphores = NULL;

		rRes = ::vkQueueSubmit( CVulkan::GetVulkanQueue(), 1, &siSubmitInfo, fDrawFence );
		assert( rRes == VK_SUCCESS );

		if ( _bWaitFence ) {
			do {
				rRes = ::vkWaitForFences( CVulkan::GetVulkanDevice(), 1, &fDrawFence, VK_TRUE, LSG_FENCE_TIMEOUT );
			} while ( rRes == VK_TIMEOUT );
			assert( rRes == VK_SUCCESS );

			::vkDestroyFence( CVulkan::GetVulkanDevice(), fDrawFence, NULL );
			if ( _pfFence ) {
				(*_pfFence) = NULL;
			}
		}
		else {
			assert( _pfFence );
			(*_pfFence) = fDrawFence;
		}
	}

	/**
	 * Waits on the given fence.  The wait period is infinite.  The fence is destroyed after the waiting period.
	 *
	 * \param _pfFence Pointer to the fence on which to wait.
	 */
	void LSE_CALL CVulkanCommandBuffer::WaitForFence( VkFence * _pfFence ) {
		if ( _pfFence ) {
			VkResult LSE_DEBUG_ONLY rRes;
			do {
				rRes = ::vkWaitForFences( CVulkan::GetVulkanDevice(), 1, _pfFence, VK_TRUE, LSG_FENCE_TIMEOUT );
			} while ( rRes == VK_TIMEOUT );
			assert( rRes == VK_SUCCESS );

			::vkDestroyFence( CVulkan::GetVulkanDevice(), (*_pfFence), NULL );
			(*_pfFence) = NULL;
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN
