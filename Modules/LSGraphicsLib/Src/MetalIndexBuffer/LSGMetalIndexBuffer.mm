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
 * Description: A Metal index buffer.
 */

#include "LSGMetalIndexBuffer.h"

#ifdef LSG_METAL

#include "../Metal/LSGMetalObject.h"
#include "../VertexBuffer/LSGVertexBuffer.h"

#include <Metal/MTLRenderCommandEncoder.h>

namespace lsg {

	// == Members.
	/** During the creation of index buffer objects, we must enforce single-threaded access to
	 *	some resources. */
	CCriticalSection CMetalIndexBuffer::m_csCrit;

	// == Various constructors.
	LSE_CALLCTOR CMetalIndexBuffer::CMetalIndexBuffer() :
		m_pvBuffer( NULL ),
		m_utIndexType( MTLIndexTypeUInt16 ) {
	}
	LSE_CALLCTOR CMetalIndexBuffer::~CMetalIndexBuffer() {
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
	LSBOOL LSE_CALL CMetalIndexBuffer::CreateApiIndexBuffer() {
		ResetApi();

		assert( m_ui32ElementSize == 2 || m_ui32ElementSize == 4 );
		switch ( m_ui32ElementSize ) {
			case 2 : { m_utIndexType = MTLIndexTypeUInt16; break; }
			case 4 : { m_utIndexType = MTLIndexTypeUInt32; break; }
		}
		MTLResourceOptions roOptions = (m_ui32Usage & LSG_IBUT_SETONLY) ? MTLResourceOptionCPUCacheModeDefault : MTLResourceOptionCPUCacheModeWriteCombined;

		id <MTLBuffer> bBuffer = [GetMetalDevice() newBufferWithBytes:m_oobpBuffer.Buffer() length:m_oobpBuffer.Length()
			options:roOptions];
		if ( bBuffer == nil ) { ResetApi(); return false; }
		m_pvBuffer = CFBridgingRetain( bBuffer );

		return true;
	}

	/**
	 * Destroy the index buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	LSVOID LSE_CALL CMetalIndexBuffer::ResetApi() {
		if ( m_pvBuffer ) {
			CFBridgingRelease( m_pvBuffer );
			m_pvBuffer = NULL;
		}
		m_utIndexType = MTLIndexTypeUInt16;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartIndex Index of the first vertex to load.  Beginning at _ui32StartIndex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	LSVOID LSE_CALL CMetalIndexBuffer::RenderApi( LSUINT32 _ui32StartIndex, LSUINT32 _ui32TotalPrimitives ) const {
		id <MTLBuffer> bBuffer = (__bridge id <MTLBuffer>)m_pvBuffer;
		
		[GetCommandEncoder() drawIndexedPrimitives:static_cast<MTLPrimitiveType>(Topology())
			indexCount:CVertexBuffer::VertexCount( static_cast<LSG_PRIMITIVE_TOPOLOGY>(Topology()), _ui32TotalPrimitives )
			indexType:static_cast<MTLIndexType>(m_utIndexType)
			indexBuffer:bBuffer
			indexBufferOffset:_ui32StartIndex];
	}

}	// namespace lsg

#endif	// #ifdef LSG_METAL
