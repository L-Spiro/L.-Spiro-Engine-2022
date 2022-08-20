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


#ifndef __LSG_INDEXBUFFERBASE_H__
#define __LSG_INDEXBUFFERBASE_H__

#include "../LSGGraphicsLib.h"
#include "OwnedOrBorrowed/LSTLOwnedOrBorrowedPointer.h"

namespace lsg {

	/**
	 * Class CIndexBufferBase
	 * \brief The base class for index buffers.
	 *
	 * Description: The base class for index buffers.
	 */
	class CIndexBufferBase {
	public :
		// == Functions.
		/**
		 * Resets the base members back to their defaults.
		 */
		void LSE_CALL									Reset();

		/**
		 * Gets the ID of this vertex buffer segment.
		 *
		 * \return Returns the ID of this vertex buffer segment.
		 */
		LSE_INLINE uint32_t LSE_CALL					GetId() const;

		/**
		 * Gets the topology.
		 *
		 * \return Returns the topology.
		 */
		LSE_INLINE uint32_t LSE_CALL					Topology() const;

		/**
		 * Gets the element size.
		 *
		 * \return Returns the size in bytes of the indices.
		 */
		LSE_INLINE uint32_t LSE_CALL					IndexSize() const;

		/**
		 * Gets the total number of indices in the index buffer.
		 *
		 * \return Returns the total number of indices in the index buffer.
		 */
		LSE_INLINE uint32_t LSE_CALL					TotalIndices() const;

		/**
		 * Gets the number of primitives.
		 *
		 * \return Returns the number of primitives.
		 */
		LSE_INLINE uint32_t LSE_CALL					Primitives() const;

	protected :
		// == Various constructors.
		LSE_CALLCTOR									CIndexBufferBase();
		LSE_CALLCTOR									~CIndexBufferBase();


		// == Members.
		/** The raw index buffer. */
		COwnedOrBorrowedPointer<uint8_t>				m_oobpBuffer;

		/** Size of each element in the buffer. */
		uint32_t										m_ui32ElementSize;

		/** Usage type. */
		uint32_t										m_ui32Usage;

		/** Number of indices. */
		uint32_t										m_ui32TotalIndices;

		/** The total number of primitives in the vertex buffer. */
		uint32_t										m_ui32PrimitiveCount;

		/** Topology. */
		uint32_t										m_ui32Topology;

		/** The index buffer ID. */
		uint32_t										m_ui32Id;

		/** Index buffer ID values. */
		static uint32_t									m_ui32IndexIdCounter;

		/** Critical section. */
		static CCriticalSection							m_csCrit;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the ID of this vertex buffer segment.
	 *
	 * \return Returns the ID of this vertex buffer segment.
	 */
	LSE_INLINE uint32_t LSE_CALL CIndexBufferBase::GetId() const {
		return m_ui32Id;
	}

	/**
	 * Gets the topology.
	 *
	 * \return Returns the topology.
	 */
	LSE_INLINE uint32_t LSE_CALL CIndexBufferBase::Topology() const {
		return m_ui32Topology;
	}

	/**
	 * Gets the element size.
	 *
	 * \return Returns the size in bytes of the indices.
	 */
	LSE_INLINE uint32_t LSE_CALL CIndexBufferBase::IndexSize() const {
		return m_ui32ElementSize;
	}

	/**
	 * Gets the total number of indices in the index buffer.
	 *
	 * \return Returns the total number of indices in the index buffer.
	 */
	LSE_INLINE uint32_t LSE_CALL CIndexBufferBase::TotalIndices() const {
		return m_ui32TotalIndices;
	}

	/**
	 * Gets the number of primitives.
	 *
	 * \return Returns the number of primitives.
	 */
	LSE_INLINE uint32_t LSE_CALL CIndexBufferBase::Primitives() const {
		return m_ui32PrimitiveCount;
	}

}	// namespace lsg

#endif	// __LSG_INDEXBUFFERBASE_H__
