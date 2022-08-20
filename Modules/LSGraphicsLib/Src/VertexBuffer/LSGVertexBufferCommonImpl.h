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
 *	this file and LSGVertexBufferCommonDecl.h.
 */


#ifndef LSG_VBUFCOMMONDEF_CLASS
#error "LSG_VBUFCOMMONDEF_CLASS must be defined (for example as CDirectX9 or COpenGl)."
#endif	// #ifndef LSG_VBUFCOMMONDEF_CLASS

/**
 * Returns the number of primitives for the given topology and element count.
 *
 * \param _ptTopology The topology.
 * \param _ui32Elements The number of elements.
 * \return Returns the number of primitives for the given topology and element count.
 */
LSE_INLINE uint32_t LSE_CALL LSG_VBUFCOMMONDEF_CLASS::PrimitiveCount( LSG_PRIMITIVE_TOPOLOGY _ptTopology, uint32_t _ui32Elements ) {
	switch ( _ptTopology ) {
		case LSG_PT_POINTLIST : { return _ui32Elements; }
		case LSG_PT_LINELIST : { return _ui32Elements >> 1; }
		case LSG_PT_LINESTRIP : { return _ui32Elements - 1; }
		case LSG_PT_TRIANGLELIST : { return _ui32Elements / 3; }
		case LSG_PT_TRIANGLESTRIP : { return _ui32Elements - 2; }
		default : { return 0; }
	}
}

/**
 * Returns the number of vertices for the given topology and primitive count.
 *
 * \param _ptTopology The topology.
 * \param _ui32Primitives The number of primitives.
 * \return Returns the number of vertices for the given topology and primitive count.
 */
LSE_INLINE uint32_t LSE_CALL LSG_VBUFCOMMONDEF_CLASS::VertexCount( LSG_PRIMITIVE_TOPOLOGY _ptTopology, uint32_t _ui32Primitives ) {
	switch ( _ptTopology ) {
		case LSG_PT_POINTLIST : { return _ui32Primitives; }
		case LSG_PT_LINELIST : { return _ui32Primitives << 1; }
		case LSG_PT_LINESTRIP : { return _ui32Primitives + 1; }
		case LSG_PT_TRIANGLELIST : { return _ui32Primitives * 3; }
		case LSG_PT_TRIANGLESTRIP : { return _ui32Primitives + 2; }
		default : { return 0; }
	}
}

#undef LSG_VBUFCOMMONDEF_CLASS