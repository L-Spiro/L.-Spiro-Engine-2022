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
 * Description: A single vertex buffer segment capable of holding up to CGfxBase::m_mMetrics.ui32MaxVertElements
 *	elements.
 */


#ifndef __LSG_VERTEXBUFFER_H__
#define __LSG_VERTEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9
#include "../DirectX9VertexBuffer/LSGDirectX9VertexBuffer.h"
#elif defined( LSG_DX11 )
#include "../DirectX11VertexBuffer/LSGDirectX11VertexBuffer.h"
#elif defined( LSG_METAL )
#include "../MetalVertexBuffer/LSGMetalVertexBuffer.h"
#elif defined( LSG_OGL )
#include "../OpenGLVertexBuffer/LSGOpenGlVertexBuffer.h"
#elif defined( LSG_OGLES )
#include "../OpenGLESVertexBuffer/LSGOpenGlEsVertexBuffer.h"
#elif defined( LSG_VKN )
#include "../VulkanVertexBuffer/LSGVulkanVertexBuffer.h"
#endif	// #ifdef LSG_DX9


namespace lsg {

	/**
	 * Class CVertexBuffer
	 * \brief A single vertex buffer segment capable of holding up to CGfxBase::m_mMetrics.ui32MaxVertElements
	 *	elements.
	 *
	 * Description: A single vertex buffer segment capable of holding up to CGfxBase::m_mMetrics.ui32MaxVertElements
	 *	elements.
	 */
	class CVertexBuffer : public 
#ifdef LSG_DX9
		CDirectX9VertexBuffer
#elif defined( LSG_DX11 )
		CDirectX11VertexBuffer
#elif defined( LSG_METAL )
		CMetalVertexBuffer
#elif defined( LSG_OGL )
		COpenGlVertexBuffer
#elif defined( LSG_OGLES )
		COpenGlEsVertexBuffer
#elif defined( LSG_VKN )
		CVulkanVertexBuffer
#endif	// #ifdef LSG_DX9
	{
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVertexBuffer();
		LSE_CALLCTOR							~CVertexBuffer();


		// == Functions.
		/**
		 * Creates a vertex buffer.
		 *
		 * \param _pvBuffer The vertex data to be placed into the vertex buffer segment.
		 * \param _pvedDesc The vertex description.
		 * \param _ui32TotalDesc Total elements to which _pvedDesc points.
		 * \param _bTakeOwnership If true, the vertex buffer keeps a copy of _pvBuffer for itself.  If false,
		 *	the contents of _pvBuffer are used directly by the vertex buffer.
		 * \param _ui32Stride Number of bytes between each primitive in the vertex buffer.
		 * \param _ui32Elements Number of primitives in the vertex buffer.
		 * \param _vbutUsage One of the LSG_VERTEX_BUFFER_USAGE_TYPES flags.
		 * \param _ptTopology The primitive topology.
		 * \return Returns true if the vertex buffer was created.  False indicates a memory/resource failure.
		 */
		LSBOOL LSE_CALL							CreateVertexBuffer( const void * _pvBuffer,
			const LSG_VERTEX_ELEMENT_DESC * _pvedDesc, uint32_t _ui32TotalDesc,
			LSBOOL _bTakeOwnership,
			uint32_t _ui32Stride, uint32_t _ui32Elements,
			LSG_VERTEX_BUFFER_USAGE_TYPES _vbutUsage,
			LSG_PRIMITIVE_TOPOLOGY _ptTopology );

		/**
		 * Resets the vertex buffer, releasing all resources and freeing all allocated memory.
		 */
		void LSE_CALL							Reset();

		/**
		 * Sets the index buffer to the given stream.
		 *
		 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
		 * \param _ui32Offset Offset in elements of the stream source.
		 * \return Returns true if the vertex buffer was applied to the given stream index.
		 */
		LSBOOL LSE_CALL							SetStream( uint32_t _ui32Index, uint32_t _ui32Offset = 0 ) const;

		
	protected :
		// == Members.


	private :
#ifdef LSG_DX9
		typedef CDirectX9VertexBuffer			Parent;
#elif defined( LSG_DX11 )
		typedef CDirectX11VertexBuffer			Parent;
#elif defined( LSG_METAL )
		typedef CMetalVertexBuffer				Parent;
#elif defined( LSG_OGL )
		typedef COpenGlVertexBuffer				Parent;
#elif defined( LSG_OGLES )
		typedef COpenGlEsVertexBuffer			Parent;
#elif defined( LSG_VKN )
		typedef CVulkanVertexBuffer				Parent;
#endif	// #ifdef LSG_DX9
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// __LSG_VERTEXBUFFER_H__
