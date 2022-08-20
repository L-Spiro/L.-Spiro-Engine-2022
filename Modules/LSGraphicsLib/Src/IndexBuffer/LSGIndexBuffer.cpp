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

#include "LSGIndexBuffer.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CIndexBuffer::CIndexBuffer() {
	}
	LSE_CALLCTOR CIndexBuffer::~CIndexBuffer() {
		Reset();
	}

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
	 LSBOOL LSE_CALL CIndexBuffer::CreateIndexBuffer( const void * _pvIndices,
		uint32_t _ui32TotalIndices, uint32_t _ui32IndexSize,
		LSBOOL _bTakeOwnership,
		LSG_INDEX_BUFFER_USAGE_TYPES _ibutUsage,
		LSG_PRIMITIVE_TOPOLOGY _ptTopology ) {
		Reset();

		m_ui32TotalIndices = _ui32TotalIndices;
		m_ui32ElementSize = _ui32IndexSize;
		m_ui32Usage = _ibutUsage;
		m_ui32PrimitiveCount = CVertexBuffer::PrimitiveCount( _ptTopology, _ui32TotalIndices );
		m_ui32Topology = _ptTopology;

		uint32_t ui32Size = m_ui32ElementSize * m_ui32TotalIndices;
		if ( !ui32Size ) { Reset(); return false; }
		if ( _bTakeOwnership ) {
			if ( !m_oobpBuffer.Create( ui32Size ) ) { Reset(); return false; }
			CStd::MemCpy( &m_oobpBuffer[0], _pvIndices, ui32Size );
		}
		else {
			m_oobpBuffer.Borrow( static_cast<const uint8_t *>(_pvIndices), ui32Size );
		}


		if ( !CreateApiIndexBuffer() ) {
			Reset();
			return false;
		}

		return true;
	 }

	 /**
	 * Resets the vertex buffer back to scratch.
	 */
	 void LSE_CALL CIndexBuffer::Reset() {
		 ResetApi();
		 CIndexBufferBase::Reset();
	 }

}	// namespace lsg
