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
 * Description: The base class for index buffers.
 */

#include "LSGIndexBufferBase.h"


namespace lsg {

	// == Members.
	/** Index buffer ID values. */
	uint32_t CIndexBufferBase::m_ui32IndexIdCounter = 0;

	/** Critical section. */
	CCriticalSection CIndexBufferBase::m_csCrit;

	// == Various constructors.
	LSE_CALLCTOR CIndexBufferBase::CIndexBufferBase() :
		m_ui32ElementSize( 0 ),
		m_ui32Usage( 0 ),
		m_ui32TotalIndices( 0 ),
		m_ui32PrimitiveCount( 0 ),
		m_ui32Topology( 0 ) {

		CCriticalSection::CLocker lLockMe( m_csCrit );
		m_ui32Id = ++m_ui32IndexIdCounter;
	}
	LSE_CALLCTOR CIndexBufferBase::~CIndexBufferBase() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the base members back to their defaults.
	 */
	void LSE_CALL CIndexBufferBase::Reset() {
		m_oobpBuffer.Reset();
		m_ui32ElementSize = 0;
		m_ui32Usage = 0;
		m_ui32TotalIndices = 0;
		m_ui32PrimitiveCount = 0;
		m_ui32Topology = 0;
	}

}	// namespace lsg
