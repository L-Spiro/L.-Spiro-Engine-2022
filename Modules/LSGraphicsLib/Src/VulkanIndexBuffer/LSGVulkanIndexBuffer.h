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


#ifndef __LSG_VULKANINDEXBUFFER_H__
#define __LSG_VULKANINDEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_VKN

#include "../IndexBuffer/LSGIndexBufferBase.h"
#include "../Vulkan/LSGVulkan.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Usage types. */
	enum LSG_INDEX_BUFFER_USAGE_TYPES {
		LSG_IBUT_SETONLY						= (1 << 0),										/**< Index data is never read or written to. */
		LSG_IBUT_STANDARD						= (1 << 1),										/**< Index data is not read or written to often by the CPU. */
		LSG_IBUT_DYNAMIC						= (1 << 2),										/**< Index data is updated frequently. */

		LSG_IBUT_FORCE_DWORD					= 0x7FFFFFFF
	};

	/**
	 * Class CVulkanIndexBuffer
	 * \brief A Vulkan index buffer.
	 *
	 * Description: A Vulkan index buffer.
	 */
	class CVulkanIndexBuffer : public CIndexBufferBase {
		friend									class CGfx;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVulkanIndexBuffer();
		LSE_CALLCTOR							~CVulkanIndexBuffer();


		// == Functions.


	protected :
		// == Members.
		/** The buffer object. */
		CVulkan::CBuffer						m_bBuffer;

		/** The allocation for the object. */
		CVulkan::CDeviceMemory					m_dmMemory;

		/** Index type. */
		VkIndexType								m_itIndexType;


		// == Functions.
		/**
		 * Create an index buffer for the graphics API.  This should use the data in this object to create
		 *	the buffer.
		 *
		 * \return Returns false if there are any errors during the creation of the API index buffer.  This
		 *	always indicates a lack of resources.
		 */
		LSBOOL LSE_CALL							CreateApiIndexBuffer();

		/**
		 * Destroy the index buffer that is compliant with the current graphics API and all related
		 *	resources.
		 */
		void LSE_CALL							ResetApi();

		/**
		 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
		 *	and have the same number of elements active.  If not, the system will throw an error.
		 *
		 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
		 * \param _ui32TotalPrimitives The total number of primitives to render.
		 */
		void LSE_CALL							RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// #ifdef LSG_VKN

#endif	// __LSG_VULKANINDEXBUFFER_H__
