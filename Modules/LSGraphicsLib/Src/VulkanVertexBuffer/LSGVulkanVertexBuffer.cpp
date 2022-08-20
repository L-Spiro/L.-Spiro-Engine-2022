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
 * Description: A Vulkan vertex buffer.
 */

#include "LSGVulkanVertexBuffer.h"

#ifdef LSG_VKN

#include "../Gfx/LSGGfx.h"
//#include "../VulkanShaders/LSGVulkanShaderProgramManager.h"
//#include "../ShaderLanguage/LSGShader.h"

namespace lsg {

	// == Members.
	/** Table of properties of vertex types. */
	CVulkanVertexBuffer::LSG_VULKAN_PREFINED_VERT_ATT CVulkanVertexBuffer::m_opvaVertProps[] = {
		// -- 32-bit Elements. -- //
		// * 1 component. * //
		{ LSG_VE_R32_F,					sizeof( LSGREAL ) * 1,	VK_FORMAT_R32_SFLOAT },
		{ LSG_VE_R32_UI,				sizeof( uint32_t ) * 1,	VK_FORMAT_R32_UINT },
		{ LSG_VE_R32_SI,				sizeof( uint32_t ) * 1,	VK_FORMAT_R32_SINT },

		// * 2 components. * //
		{ LSG_VE_R32G32_F,				sizeof( LSGREAL ) * 2,	VK_FORMAT_R32G32_SFLOAT },
		{ LSG_VE_R32G32_UI,				sizeof( uint32_t ) * 2,	VK_FORMAT_R32G32_UINT },
		{ LSG_VE_R32G32_SI,				sizeof( uint32_t ) * 2,	VK_FORMAT_R32G32_SINT },

		// * 3 components. * //
		{ LSG_VE_R32G32B32_F,			sizeof( LSGREAL ) * 3,	VK_FORMAT_R32G32B32_SFLOAT },
		{ LSG_VE_R32G32B32_UI,			sizeof( uint32_t ) * 3,	VK_FORMAT_R32G32B32_UINT },
		{ LSG_VE_R32G32B32_SI,			sizeof( uint32_t ) * 3,	VK_FORMAT_R32G32B32_SINT },

		// * 4 components. * //
		{ LSG_VE_R32G32B32A32_F,		sizeof( LSGREAL ) * 4,	VK_FORMAT_R32G32B32A32_SFLOAT },
		{ LSG_VE_R32G32B32A32_UI,		sizeof( uint32_t ) * 4,	VK_FORMAT_R32G32B32A32_UINT },
		{ LSG_VE_R32G32B32A32_SI,		sizeof( uint32_t ) * 4,	VK_FORMAT_R32G32B32A32_SINT },

		// -- 16-bit Elements. -- //
		// * 1 component. * //
		{ LSG_VE_R16_F,					sizeof( CFloat16 ) * 1,	VK_FORMAT_R16_SFLOAT },
		{ LSG_VE_R16_UI,				sizeof( uint16_t ) * 1,	VK_FORMAT_R16_UINT },
		{ LSG_VE_R16_SI,				sizeof( uint16_t ) * 1,	VK_FORMAT_R16_SINT },
		{ LSG_VE_R16_UI_N,				sizeof( uint16_t ) * 1,	VK_FORMAT_R16_UNORM },
		{ LSG_VE_R16_SI_N,				sizeof( uint16_t ) * 1,	VK_FORMAT_R16_SNORM },

		// * 2 components. * //
		{ LSG_VE_R16G16_F,				sizeof( CFloat16 ) * 2,	VK_FORMAT_R16G16_SFLOAT },
		{ LSG_VE_R16G16_UI,				sizeof( uint16_t ) * 2,	VK_FORMAT_R16G16_UINT },
		{ LSG_VE_R16G16_SI,				sizeof( uint16_t ) * 2,	VK_FORMAT_R16G16_SINT },
		{ LSG_VE_R16G16_UI_N,			sizeof( uint16_t ) * 2,	VK_FORMAT_R16G16_UNORM },
		{ LSG_VE_R16G16_SI_N,			sizeof( uint16_t ) * 2,	VK_FORMAT_R16G16_SNORM },

		// * 4 components. * //
		{ LSG_VE_R16G16B16A16_F,		sizeof( CFloat16 ) * 4,	VK_FORMAT_R16G16B16A16_SFLOAT },
		{ LSG_VE_R16G16B16A16_UI,		sizeof( uint16_t ) * 4,	VK_FORMAT_R16G16B16A16_UINT },
		{ LSG_VE_R16G16B16A16_SI,		sizeof( uint16_t ) * 4,	VK_FORMAT_R16G16B16A16_SINT },
		{ LSG_VE_R16G16B16A16_UI_N,		sizeof( uint16_t ) * 4,	VK_FORMAT_R16G16B16A16_UNORM },
		{ LSG_VE_R16G16B16A16_SI_N,		sizeof( uint16_t ) * 4,	VK_FORMAT_R16G16B16A16_SNORM },

		// -- 10-bit Elements. -- //
		// * 3 components. * //
		{ LSG_VE_R10G10B10A2_UI,		sizeof( uint32_t ) * 1,	VK_FORMAT_A2R10G10B10_UINT_PACK32 },
		{ LSG_VE_R10G10B10A2_UI_N,		sizeof( uint32_t ) * 1,	VK_FORMAT_A2R10G10B10_UNORM_PACK32 },

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		{ LSG_VE_R8G8B8A8_UI,			sizeof( uint32_t ) * 1,	VK_FORMAT_R8G8B8A8_UINT },
		{ LSG_VE_R8G8B8A8_UI_N,			sizeof( uint32_t ) * 1,	VK_FORMAT_R8G8B8A8_UNORM },
	};

	// == Various constructors.
	LSE_CALLCTOR CVulkanVertexBuffer::CVulkanVertexBuffer() :
		m_ui32StreamOffset( 0 ) {
	}
	LSE_CALLCTOR CVulkanVertexBuffer::~CVulkanVertexBuffer() {
		ResetApi();
	}

	// == Functions.
	/**
	 * Creates a vertex buffer compliant with the current graphics API.
	 *
	 * \return Returns false if the creation of the buffer failed.
	 */
	LSBOOL LSE_CALL CVulkanVertexBuffer::CreateApiVertexBuffer() {
		ResetApi();

		uint32_t ui32Offset = 0;
		for ( uint32_t I = 0; I < m_ui16TotalDecl; ++I ) {
			if ( m_veElements[I].ui32Offset ) {
				ui32Offset = m_veElements[I].ui32Offset;
			}
			VkVertexInputAttributeDescription vapPointer = {};
			const LSG_VULKAN_PREFINED_VERT_ATT * popvaAttr = GetVertexElementAttr( static_cast<LSG_VERTEX_ELEMENT>(m_veElements[I].ui32Format) );
			if ( !popvaAttr ) {
				CStd::DebugPrintA( "CVulkanVertexBuffer::CreateApiVertexBuffer(): Unrecognized format in vertex buffer." );
				ResetApi();
				return false;
			}

			vapPointer.offset = ui32Offset;
			/*vapPointer.ui16LocClass = static_cast<uint16_t>(m_veElements[I].ui32Semantic);
			vapPointer.ui16LocUsage = static_cast<uint16_t>(m_veElements[I].ui32SemanticIndex);*/
			vapPointer.format = popvaAttr->fType;
			
			m_viadAttribDesc[I] = vapPointer;
			ui32Offset += popvaAttr->ui32Size;
		}


		uint32_t ui32Size = m_ui32Stride * (m_ui32Elements > 1 ? m_ui32Elements : 1);

		VkBufferCreateInfo bciBufferInfo = {};
		bciBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bciBufferInfo.pNext = NULL;
		bciBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bciBufferInfo.size = ui32Size;
		bciBufferInfo.queueFamilyIndexCount = 0;
		bciBufferInfo.pQueueFamilyIndices = NULL;
		bciBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bciBufferInfo.flags = 0;
		if ( !m_bBuffer.CreateBuffer( &bciBufferInfo ) ) { return false; }

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


		if ( (m_ui32Usage & LSG_VBUT_SETONLY) && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}

		return true;
	}

	/**
	 * Destroy the vertex buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL CVulkanVertexBuffer::ResetApi() {
		m_bBuffer.DestroyBuffer();
		m_dmMemory.FreeMemory();
		m_ui32StreamOffset = 0;
	}

	/**
	 * Sets the vertex buffer on a given stream.
	 *
	 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
	 * \param _ui32Offset Offset in elements of the stream source.
	 * \return Returns true if the vertex buffer was applied to the given stream index.
	 */
	LSBOOL LSE_CALL CVulkanVertexBuffer::SetStreamApi( uint32_t /*_ui32Index*/, uint32_t _ui32Offset ) const {
		m_ui32StreamOffset = _ui32Offset;
		return true;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL CVulkanVertexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		if ( _ui32TotalPrimitives ) {
			/*vkCmdBindVertexBuffers( VkCommandBuffer commandBuffer,
				uint32_t firstBinding,
				uint32_t bindingCount,
				const VkBuffer* pBuffers,
				const VkDeviceSize* pOffsets);*/
		}
	}

	/**
	 * Prepare for rendering.
	 */
	void LSE_CALL CVulkanVertexBuffer::PrepareToRenderApi() const {
		
	}

	/**
	 * Finish rendering.
	 */
	void LSE_CALL CVulkanVertexBuffer::EndRenderApi() const {
		
	}

	/**
	 * Gets a pointer to a LSG_VULKAN_PREFINED_VERT_ATT structure from the m_opvaVertProps array by ID (a LSG_VERTEX_ELEMENT value).
	 *
	 * \param _veType The type whose information inside m_opvaVertProps is to be found.
	 * \return Returns a pointer to an item inside m_opvaVertProps or NULL if the item does not exist.
	 */
	const CVulkanVertexBuffer::LSG_VULKAN_PREFINED_VERT_ATT * LSE_CALL CVulkanVertexBuffer::GetVertexElementAttr( LSG_VERTEX_ELEMENT _veType ) {
		assert( _veType < LSE_ELEMENTS( m_opvaVertProps ) );
		if ( m_opvaVertProps[_veType].ui32Mask == static_cast<uint32_t>(_veType) ) { return &m_opvaVertProps[_veType]; }
		for ( uint32_t I = LSE_ELEMENTS( m_opvaVertProps ); I--; ) {
			if ( m_opvaVertProps[I].ui32Mask == static_cast<uint32_t>(_veType) ) { return &m_opvaVertProps[I]; }
		}
		return NULL;
	}
}

#endif	// #ifdef LSG_VKN
