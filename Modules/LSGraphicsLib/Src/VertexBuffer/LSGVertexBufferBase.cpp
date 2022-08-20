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
 * Description: The base class for all vertex buffers.
 */

#include "LSGVertexBufferBase.h"


namespace lsg {

	// == Members.
	/** Vertex buffer ID values. */
	uint32_t CVertexBufferBase::m_ui32VertIdCounter = 0;

	/** Critical section. */
	CCriticalSection CVertexBufferBase::m_csCrit;

	// == Various constructors.
	LSE_CALLCTOR CVertexBufferBase::CVertexBufferBase() :
		m_ui32Topology( LSE_MAXU32 ),
		m_ui32Usage( LSE_MAXU32 ),
		m_ui16ElementSize( 0xFFFF ),
		m_ui32Stride( 0 ),
		//m_ui32Lock( 0 ),
		m_bFinalized( false ),
		m_ui32Elements( 0 ),
		m_ui32PrimitiveCount( 0 ),
		m_ui16TotalDecl( 0 ) {

		m_ui32Id = NewId();
	}

	// == Functions.
	/**
	 * Resets the vertex buffer base, freeing all allocated memory.
	 */
	void LSE_CALL CVertexBufferBase::Reset() {
		m_ui32Topology = LSE_MAXU32;
		m_ui32Usage = LSE_MAXU32;
		m_ui16ElementSize = 0xFFFF;
		m_ui32Stride = 0;
		//m_ui32Lock = 0;
		m_bFinalized = false;
		m_ui32Elements = 0;
		m_ui32PrimitiveCount = 0;
		m_ui16TotalDecl = 0;
		m_oobpBuffer.Reset();
	}

}	// namespace lsg

