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
 * Description: A Vulkan index buffer.
 */

#include "LSGVulkanIndexBuffer.h"

#ifdef LSG_VKN

//#include "../VertexBuffer/LSGVertexBuffer.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVulkanIndexBuffer::CVulkanIndexBuffer() :
		m_itIndexType( VK_INDEX_TYPE_UINT32 ) {
	}
	LSE_CALLCTOR CVulkanIndexBuffer::~CVulkanIndexBuffer() {
		ResetApi();
	}

	// == Functions.
	/**
	 * Create an index buffer for the graphics API.  This should use the data in this object to create
	 *	the buffer.
	 *
	 * \return Returns false if there are any errors during the creation of the API index buffer.  This
	 *	always indicates a lack of resources.
	 */
	LSBOOL LSE_CALL CVulkanIndexBuffer::CreateApiIndexBuffer() {
		ResetApi();

		assert( m_ui32ElementSize == 2 || m_ui32ElementSize == 4 );
		switch ( m_ui32ElementSize ) {
			case 2 : { m_itIndexType = VK_INDEX_TYPE_UINT16; break; }
			case 4 : { m_itIndexType = VK_INDEX_TYPE_UINT32; break; }
		}


		VkBufferCreateInfo bciBufferInfo = {};
		bciBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bciBufferInfo.pNext = NULL;
		bciBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		bciBufferInfo.size = m_oobpBuffer.Length();
		bciBufferInfo.queueFamilyIndexCount = 0;
		bciBufferInfo.pQueueFamilyIndices = NULL;
		bciBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bciBufferInfo.flags = 0;
		if ( !m_bBuffer.CreateBuffer( &bciBufferInfo ) ) {
			return false;
		}

		VkMemoryRequirements mrMemReqs;
		::vkGetBufferMemoryRequirements( CVulkan::GetVulkanDevice(), m_bBuffer.Buffer(), &mrMemReqs );

		VkMemoryAllocateInfo maiAllocInfo = {};
		maiAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		maiAllocInfo.pNext = NULL;
		maiAllocInfo.memoryTypeIndex = 0;

		maiAllocInfo.allocationSize = mrMemReqs.size;

		if ( !CVulkan::MemoryTypeFromProperties( mrMemReqs.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			maiAllocInfo.memoryTypeIndex ) ) {
			return false;
		}



		if ( !m_dmMemory.AllocateMemory( &maiAllocInfo ) ) {
			return false;
		}
		/*info.vertex_buffer.buffer_info.range = mem_reqs.size;
		info.vertex_buffer.buffer_info.offset = 0;*/

		VkResult rRes;
		void * pvDst;
		rRes = ::vkMapMemory( CVulkan::GetVulkanDevice(), m_dmMemory.DeviceMemory(), 0, mrMemReqs.size, 0, &pvDst );
		if ( rRes != VK_SUCCESS ) { return false; }
		CStd::MemCpy( pvDst, m_oobpBuffer.Buffer(), m_oobpBuffer.Length() );
		::vkUnmapMemory( CVulkan::GetVulkanDevice(), m_dmMemory.DeviceMemory() );
		

		rRes = ::vkBindBufferMemory( CVulkan::GetVulkanDevice(), m_bBuffer.Buffer(),
            m_dmMemory.DeviceMemory(), 0 );
		if ( rRes != VK_SUCCESS ) { return false; }


		if ( (m_ui32Usage & LSG_IBUT_SETONLY) && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}

		return true;
	}

	/**
	 * Destroy the index buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL CVulkanIndexBuffer::ResetApi() {
		m_bBuffer.DestroyBuffer();
		m_dmMemory.FreeMemory();
		m_itIndexType = VK_INDEX_TYPE_UINT32;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL CVulkanIndexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		if ( m_bBuffer.Buffer() != VK_NULL_HANDLE ) {
			/*::vkCmdBindIndexBuffer( CVulkan::GetVulkanCommandBuffer(),
				m_bBuffer.Buffer(),
				VkDeviceSize offset,
				m_itIndexType );*/

			/*::vkCmdDrawIndexed( CVulkan::GetVulkanCommandBuffer(),
				uint32_t indexCount,
				uint32_t instanceCount,
				uint32_t firstIndex,
				int32_t vertexOffset,
				uint32_t firstInstance);*/
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_VKN
