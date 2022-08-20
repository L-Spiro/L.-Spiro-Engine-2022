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
 * Description: A plain-old-data ring buffer of a certain type.
 */


#ifndef __LSTL_RINGBUFFERPOD_H__
#define __LSTL_RINGBUFFERPOD_H__

#include "../LSTLib.h"
#include "RingBuffer/LSSTDRingBufferCounter.h"

namespace lstl {

	/**
	 * Class CRingBufferPoD
	 * \brief A plain-old-data ring buffer of a certain type.
	 *
	 * Description: A plain-old-data ring buffer of a certain type.
	 */
	template <uintptr_t _uiptrBytesPerAllocation, uintptr_t _uiptrMaxAllocations>
	class CRingBufferPoD {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CRingBufferPoD() {
			m_pui8Buffer = LSENEW uint8_t[_uiptrBytesPerAllocation*_uiptrMaxAllocations];
			m_rbcPointers.SetSize( _uiptrBytesPerAllocation*_uiptrMaxAllocations );
		}
		LSE_CALLCTOR						~CRingBufferPoD() {
			LSEDELETE [] m_pui8Buffer;
			m_pui8Buffer = NULL;
		}


		// == Functions.
		/**
		 * Gets a pointer to the next allocation in the ring buffer of _uiptrBytesPerAllocation bytes.
		 *
		 * \return Returns a pointer to the next _uiptrBytesPerAllocation bytes, or
		 *	NULL if allocation failed.
		 */
		void * LSE_CALL					Alloc() {
			uintptr_t uiptrIndex;
			if ( !m_rbcPointers.Alloc( _uiptrBytesPerAllocation, uiptrIndex ) ) { return NULL; }
			return &m_pui8Buffer[uiptrIndex];
		}

		/**
		 * Frees the earliest set of _uiptrBytesPerAllocation bytes still allocated.
		 */
		void LSE_CALL						Free() {
			uintptr_t uiptrIndex;
#ifdef _DEBUG
			uintptr_t uiptrFreed = m_rbcPointers.Free( _uiptrBytesPerAllocation, uiptrIndex );
			assert( uiptrFreed );
#else
			m_rbcPointers.Free( _uiptrBytesPerAllocation, uiptrIndex );
#endif	// #ifdef _DEBUG
		}

		/**
		 * Gets a pointer to the most distant allocation, if any exists.
		 *
		 * \return Returns a pointer to the most distant allocation, if any exists, NULL otherwise.
		 */
		const void * LSE_CALL				FirstAllocated() const {
			if ( !m_rbcPointers.TotalAllocated() ) { return NULL; }
			return &m_pui8Buffer[m_rbcPointers.OldestAllocated()];
		}

		/**
		 * Gets the counter.
		 *
		 * \return Returns the ring counter.
		 */
		const CRingBufferCounter & LSE_CALL	Counter() const {
			return m_rbcPointers;
		}


	protected :
		// == Members.
		/** The ring-buffer pointer tracker. */
		CRingBufferCounter					m_rbcPointers;

		/** The actual data. */
		uint8_t *							m_pui8Buffer;
	};

}	// namespace lstl

#endif	// __LSTL_RINGBUFFERPOD_H__
