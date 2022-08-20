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
 * Description: An index buffer.
 */


#ifndef __LSG_INDEXBUFFER_H__
#define __LSG_INDEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9
#include "../DirectX9IndexBuffer/LSGDirectX9IndexBuffer.h"
#elif defined( LSG_DX11 )
#include "../DirectX11IndexBuffer/LSGDirectX11IndexBuffer.h"
#elif defined( LSG_METAL )
#include "../MetalIndexBuffer/LSGMetalIndexBuffer.h"
#elif defined( LSG_OGL )
#include "../OpenGLIndexBuffer/LSGOpenGlIndexBuffer.h"
#elif defined( LSG_OGLES )
#include "../OpenGLESIndexBuffer/LSGOpenGlEsIndexBuffer.h"
#elif defined( LSG_VKN )
#include "../VulkanIndexBuffer/LSGVulkanIndexBuffer.h"
#endif	// #ifdef LSG_DX9

#include "../VertexBuffer/LSGVertexBuffer.h"

namespace lsg {

	/**
	 * Class CIndexBuffer
	 * \brief An index buffer.
	 *
	 * Description: An index buffer.
	 */
	class CIndexBuffer : public 
#ifdef LSG_DX9
		CDirectX9IndexBuffer
#elif defined( LSG_DX11 )
		CDirectX11IndexBuffer
#elif defined( LSG_OGL )
		COpenGlIndexBuffer
#elif defined( LSG_OGLES )
		COpenGlEsIndexBuffer
#elif defined( LSG_METAL )
		CMetalIndexBuffer
#elif defined( LSG_VKN )
		CVulkanIndexBuffer
#endif	// #ifdef LSG_DX9
	{
	public :
		// == Various constructors.
		LSE_CALLCTOR							CIndexBuffer();
		LSE_CALLCTOR							~CIndexBuffer();


		// == Functions.
		/**
		 * Creates an index buffer.
		 *
		 * \param _pvIndices Pointer to the actual index data.
		 * \param _ui32TotalIndices Total number of indices to which _pvIndices points.
		 * \param _ui32IndexSize Size, in bytes, of each index in the buffer.
		 * \param _bTakeOwnership If true, the index buffer keeps a copy of _pvIndices for itself.  If false,
		 *	the contents of _pvIndices are used directly by the index buffer.
		 * \param _ibutUsage One of the LSG_INDEX_BUFFER_USAGE_TYPES flags.
		 * \param _ptTopology The primitive topology.
		 * \return Returns true if the index buffer was created.  False indicates a memory/resource failure.
		 */
		 LSBOOL LSE_CALL						CreateIndexBuffer( const void * _pvIndices,
			uint32_t _ui32TotalIndices, uint32_t _ui32IndexSize,
			LSBOOL _bTakeOwnership,
			LSG_INDEX_BUFFER_USAGE_TYPES _ibutUsage,
			LSG_PRIMITIVE_TOPOLOGY _ptTopology );

		/**
		 * Resets the vertex buffer back to scratch.
		 */
		void LSE_CALL							Reset();


	protected :
		// == Members.


	private :
#ifdef LSG_DX9
		typedef CDirectX9IndexBuffer			Parent;
#elif defined( LSG_DX11 )
		typedef CDirectX11IndexBuffer			Parent;
#elif defined( LSG_METAL )
		typedef CMetalIndexBuffer				Parent;
#elif defined( LSG_OGL )
		typedef COpenGlIndexBuffer				Parent;
#elif defined( LSG_OGLES )
		typedef COpenGlEsIndexBuffer			Parent;
#elif defined( LSG_VKN )
		typedef CVulkanIndexBuffer				Parent;
#endif	// #ifdef LSG_DX9
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

}	// namespace lsg

#endif	// __LSG_INDEXBUFFER_H__
