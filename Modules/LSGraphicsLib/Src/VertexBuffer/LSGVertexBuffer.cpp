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

#include "LSGVertexBuffer.h"
#include "../Gfx/LSGGfx.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVertexBuffer::CVertexBuffer() {
	}
	LSE_CALLCTOR CVertexBuffer::~CVertexBuffer() {
		Reset();
	}

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
	LSBOOL LSE_CALL CVertexBuffer::CreateVertexBuffer( const void * _pvBuffer,
		const LSG_VERTEX_ELEMENT_DESC * _pvedDesc, uint32_t _ui32TotalDesc,
		LSBOOL _bTakeOwnership,
		uint32_t _ui32Stride, uint32_t _ui32Elements,
		LSG_VERTEX_BUFFER_USAGE_TYPES _vbutUsage,
		LSG_PRIMITIVE_TOPOLOGY _ptTopology ) {
		Reset();

		//assert( _ui32Elements <= CGfx::GetMetrics().ui32MaxVertElements );
		assert( _ui32TotalDesc <= LSE_ELEMENTS( m_veElements ) );

		m_ui32Topology = _ptTopology;
		m_ui32Usage = _vbutUsage;
		m_ui32Stride = _ui32Stride;
		m_ui32Elements = _ui32Elements;
		m_ui32PrimitiveCount = PrimitiveCount( _ptTopology, _ui32Elements );
		m_ui16TotalDecl = static_cast<uint16_t>(_ui32TotalDesc);
		CStd::MemCpy( m_veElements, _pvedDesc, _ui32TotalDesc * sizeof( LSG_VERTEX_ELEMENT_DESC ) );
		uint32_t ui32Size = m_ui32Stride * m_ui32Elements;
		if ( !ui32Size ) { Reset(); return false; }
		if ( _bTakeOwnership ) {
			if ( !m_oobpBuffer.Create( ui32Size ) ) { Reset(); return false; }
			if ( _pvBuffer ) {
				CStd::MemCpy( &m_oobpBuffer[0], _pvBuffer, ui32Size );
			}
		}
		else {
			m_oobpBuffer.Borrow( static_cast<const uint8_t *>(_pvBuffer), ui32Size );
		}

		if ( !CreateApiVertexBuffer() ) {
			Reset();
			return false;
		}

		return true;
	}

	/**
	 * Resets the vertex buffer, releasing all resources and freeing all allocated memory.
	 */
	void LSE_CALL CVertexBuffer::Reset() {
		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			if ( CGfx::VertexBuffers()[I].pvbVertBuffer ) {
				if ( CGfx::VertexBuffers()[I].pvbVertBuffer->GetId() == GetId() ) {
					CGfx::VertexBuffers()[I].pvbVertBuffer = NULL;
					if ( CGfx::TotalActiveVertexBuffers() == I + 1 ) { CGfx::TotalActiveVertexBuffers()--; }
				}
			}
		}
		ResetApi();
		CVertexBufferBase::Reset();
	}

	/**
	 * Sets the index buffer to the given stream.
	 *
	 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
	 * \param _ui32Offset Offset in elements of the stream source.
	 * \return Returns true if the vertex buffer was applied to the given stream index.
	 */
	LSBOOL LSE_CALL CVertexBuffer::SetStream( uint32_t _ui32Index, uint32_t _ui32Offset ) const {
		assert( _ui32Index < LSG_MAX_VERTEX_STREAMS );

		CGfx::VertexBuffers()[_ui32Index].pvbVertBuffer = this;
		CGfx::VertexBuffers()[_ui32Index].ui32Offset = _ui32Offset;
		CGfx::TotalActiveVertexBuffers() = CStd::Max<uint32_t>( CGfx::TotalActiveVertexBuffers(), _ui32Index + 1 );
		return true;
	}

	
}	// namespace lsg
