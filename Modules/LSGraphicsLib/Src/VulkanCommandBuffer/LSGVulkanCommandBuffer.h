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


#ifndef __LSG_VULKANCOMMANDBUFFER_H__
#define __LSG_VULKANCOMMANDBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_VKN

#include <vulkan/vulkan.h>

namespace lsg {

	/**
	 * Class CVulkanCommandBuffer
	 * \brief A Vulkan command buffer.
	 *
	 * Description: A Vulkan command buffer.
	 */
	class CVulkanCommandBuffer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVulkanCommandBuffer();
		LSE_CALLCTOR							~CVulkanCommandBuffer();


		// == Types.
		/**
		 * Class CBeginBuffer
		 * \brief Used to begin and end (BeginCommandBuffer()/EndCommandBuffer()) a command buffer.
		 *
		 * Description: Used to begin and end (BeginCommandBuffer()/EndCommandBuffer()) a command buffer.
		 */
		class CBeginBuffer {
		public :
			// == Various constructors.
			LSE_INLINE LSE_CALLCTOR				CBeginBuffer( CVulkanCommandBuffer &_vcbBuffer );
			LSE_INLINE LSE_CALLCTOR				~CBeginBuffer();


		protected :
			// == Types.
			/** The command buffer we begin/end. */
			CVulkanCommandBuffer &				m_vcbTarget;


			// == Operators.
			/**
			 * Assignment operator.
			 *
			 * \param _bbVal The value to copy.
			 * \return Returns the copied object.
			 */
			LSE_INLINE CBeginBuffer & LSE_FCALL	operator = ( const CBeginBuffer &/*_bbVal*/ ) {
				assert( false );
				return (*this);
			}
		};


		// == Functions.
		/**
		 * Allocates this command buffer.
		 *
		 * \return Returns true if the command buffer was allocated.
		 */
		LSBOOL LSE_CALL							AllocateCommandBuffer();

		/**
		 * Destroys this command buffer.
		 */
		void LSE_CALL							FreeCommandBuffer();

		/**
		 * Begins the command buffer.
		 */
		LSE_INLINE void LSE_CALL				BeginCommandBuffer();

		/**
		 * Ends the command buffer.
		 */
		LSE_INLINE void LSE_CALL				EndCommandBuffer();

		/**
		 * Resets the command buffer.
		 *
		 * \param _cbrfFlags If _cbrfFlags includes VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT, then most or all memory resources currently owned by the command buffer should be returned to the parent command pool. If this flag is not set, then the command buffer may hold onto memory resources and reuse them when recording commands.
		 */
		LSE_INLINE void LSE_CALL				ResetCommandBuffer( VkCommandBufferResetFlags _cbrfFlags );

		/**
		 * Inserts an image barrier into the command buffer.
		 *
		 * \param _iImage The image to barrier.
		 * \param _iafAspectFlags What part of the image to barrier (color, depth, stencil, meta-data, or any combination).
		 * \param _ilOldLayout The old layout of the image (VK_IMAGE_LAYOUT_UNDEFINED for new images).
		 * \param _ilNewLayout The old layout of the image.
		 */
		void LSE_CALL							PipelineBarrierImage( VkImage _iImage, VkImageAspectFlags _iafAspectFlags,
			VkImageLayout _ilOldLayout, VkImageLayout _ilNewLayout );

		/**
		 * Queues the command buffer for execution.
		 *
		 * \param _bWaitFence If true, waits on a fence to finish.
		 * \param _pfFence If _bWaitFence is false, _pfFence is filled with the fence on which to wait later.  Otherwise this is set to NULL on
		 *	return.
		 * \param _psfDstFlags Optional pipeline stage flag indicating when the semaphore wait will occur.
		 */
		void LSE_CALL							QueueSubmit( LSBOOL _bWaitFence = true, VkFence * _pfFence = NULL,
			VkPipelineStageFlags _psfDstFlags = VkPipelineStageFlags( 0 ) );

		/**
		 * Waits on the given fence.  The wait period is infinite.  The fence is destroyed after the waiting period.
		 *
		 * \param _pfFence Pointer to the fence on which to wait.
		 */
		void LSE_CALL							WaitForFence( VkFence * _pfFence );


	protected :
		// == Members.
		/** The command-buffer ID. */
		VkCommandBuffer							m_cbCmd;

		/** Indicates whether or not the command buffer is between a begin/end block. */
		uint32_t								m_ui32Open;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CVulkanCommandBuffer::CBeginBuffer::CBeginBuffer( CVulkanCommandBuffer &_vcbBuffer ) :
		m_vcbTarget( _vcbBuffer ) {
		m_vcbTarget.BeginCommandBuffer();
	}
	LSE_INLINE LSE_CALLCTOR CVulkanCommandBuffer::CBeginBuffer::~CBeginBuffer() {
		m_vcbTarget.EndCommandBuffer();
	}

	// == Functions.
	/**
	 * Begins the command buffer.
	 */
	LSE_INLINE void LSE_CALL CVulkanCommandBuffer::BeginCommandBuffer() {
		if ( !m_ui32Open++ ) {
			VkResult LSE_DEBUG_ONLY rRes;

			VkCommandBufferBeginInfo cbbiInfo = {};
			cbbiInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cbbiInfo.pNext = NULL;
			cbbiInfo.flags = 0;
			cbbiInfo.pInheritanceInfo = NULL;

			rRes = ::vkBeginCommandBuffer( m_cbCmd, &cbbiInfo );
			assert( rRes == VK_SUCCESS );
		}
	}

	/**
	 * Ends the command buffer.
	 */
	LSE_INLINE void LSE_CALL CVulkanCommandBuffer::EndCommandBuffer() {
		assert( m_ui32Open );
		if ( !--m_ui32Open ) {
			VkResult LSE_DEBUG_ONLY rRes;

			rRes = ::vkEndCommandBuffer( m_cbCmd );
			assert( rRes == VK_SUCCESS );
		}
	}

	/**
	 * Resets the command buffer.
	 *
	 * \param _cbrfFlags If _cbrfFlags includes VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT, then most or all memory resources currently owned by the command buffer should be returned to the parent command pool. If this flag is not set, then the command buffer may hold onto memory resources and reuse them when recording commands.
	 */
	LSE_INLINE void LSE_CALL CVulkanCommandBuffer::ResetCommandBuffer( VkCommandBufferResetFlags _cbrfFlags ) {
		VkResult LSE_DEBUG_ONLY rRes;

		rRes = ::vkResetCommandBuffer( m_cbCmd, _cbrfFlags );
		assert( rRes == VK_SUCCESS );
		m_ui32Open = 0;
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN

#endif	// __LSG_VULKANCOMMANDBUFFER_H__
