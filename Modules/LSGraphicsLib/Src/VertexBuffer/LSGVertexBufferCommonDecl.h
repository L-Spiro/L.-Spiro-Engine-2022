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
 * Description: Common declarations for vertex API buffers.  Each API vertex buffer defines LSG_PRIMITIVE_TOPOLOGY
 *	differently, so basic functions that use it, such as PrimitiveCount() and VertexCount(), can't just be defined
 *	on the base class.  It basically has to be reimplemented for each API class.  So this is done just by including
 *	this file and LSGVertexBufferCommonImpl.h.
 */


// == Functions.
/**
 * Maps the vertex buffer for updating.  The returned pointer can be used for directly writing to or reading from
 *	the vertex buffer.
 *
 * \param _vbmaAccessType The type of access needed for the mapping.
 * \return Returns a pointer to which the new vertex-buffer data can be directly written or read.
 */
LSG_VERTEX_BUFFER_MAP LSE_CALL					Map( LSG_VERTEX_BUFFER_MAP_ACCESS _vbmaAccessType );

/**
 * Unmaps the vertex buffer after an update.
 *
 * \param _vbmMappedData The returned mapped data from a previous call to Map().
 */
void LSE_CALL									Unmap( LSG_VERTEX_BUFFER_MAP &_vbmMappedData );


/**
 * Returns the number of primitives for the given topology and element count.
 *
 * \param _ptTopology The topology.
 * \param _ui32Elements The number of elements.
 * \return Returns the number of primitives for the given topology and element count.
 */
static LSE_INLINE uint32_t LSE_CALL				PrimitiveCount( LSG_PRIMITIVE_TOPOLOGY _ptTopology, uint32_t _ui32Elements );

/**
 * Returns the number of vertices for the given topology and primitive count.
 *
 * \param _ptTopology The topology.
 * \param _ui32Primitives The number of primitives.
 * \return Returns the number of vertices for the given topology and primitive count.
 */
static LSE_INLINE uint32_t LSE_CALL				VertexCount( LSG_PRIMITIVE_TOPOLOGY _ptTopology, uint32_t _ui32Primitives );
