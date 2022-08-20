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
 * Description: Performs management tasks associated with a ring buffer but does not actually probide a buffer.
 *	The purpose is to allow the buffer of memory itself to be created and maintained in any way desired, whether
 *	that be through a raw pointer allocated with LSENEW, a CVector, etc.  This class simply keeps track of how
 *	much has been allocated, where the next allocation will occur, and how much memory remains.
 */


#ifndef __LSSTD_RINGBUFFERCOUNTER_H__
#define __LSSTD_RINGBUFFERCOUNTER_H__

#include "../LSSTDStandardLib.h"

namespace lsstd {

	/**
	 * Class CRingBufferCounter
	 * \brief Performs management tasks associated with a ring buffer but does not actually probide a buffer.
	 *
	 * Description: Performs management tasks associated with a ring buffer but does not actually probide a buffer.
	 *	The purpose is to allow the buffer of memory itself to be created and maintained in any way desired, whether
	 *	that be through a raw pointer allocated with LSENEW, a CVector, etc.  This class simply keeps track of how
	 *	much has been allocated, where the next allocation will occur, and how much memory remains.
	 */
	class CRingBufferCounter {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CRingBufferCounter();


		// == Functions.
		/**
		 * Sets the size of the buffer.
		 *
		 * \param _uiptrSize Size of the buffer.
		 */
		void LSE_CALL							SetSize( uintptr_t _uiptrSize );

		/**
		 * Returns true if the requested number of bytes can be allocated from the ring
		 *	buffer.
		 *
		 * \param _uiptrSize Number of bytes to check for being possible to allocate.
		 * \return Returns true if the requested number of bytes can be allocated from the ring
		 *	buffer.
		 */
		LSBOOL LSE_CALL							CanAllocate( uintptr_t _uiptrSize ) const;

		/**
		 * Gets the index in bytes of the allocated memory.  If there is not enough memory
		 *	to perform the allocation false is returned.
		 *
		 * \param _uiptrSize Size in bytes to allocate.
		 * \param _uiptrIndex Index of the allocated bytes within the buffer in bytes.
		 * \return Returns true if the allocation succeeded.
		 */
		LSBOOL LSE_CALL							Alloc( uintptr_t _uiptrSize, uintptr_t &_uiptrIndex );

		/**
		 * Frees memory back to the ring buffer.
		 *
		 * \param _uiptrSize Size in bytes to return back to the ring buffer.
		 * \param _uiptrIndex Index of the freed bytes within the buffer in bytes.
		 * \return Returns the number of bytes actually returned back to the buffer, which may be larger than
		 *	the requested size on wrap-around frees.  A return of 0 indicates no memory was freed because
		 *	the free amount is not allocated from the fing buffer.
		 */
		uintptr_t LSE_CALL						Free( uintptr_t _uiptrSize, uintptr_t &_uiptrIndex );

		/**
		 * Gets the index of the oldest-allocated item.
		 *
		 * \return Returns the index of the oldest-allocated item.
		 */
		LSE_INLINE uintptr_t LSE_CALL			OldestAllocated() const;

		/**
		 * Gets the total number of bytes allocated.
		 *
		 * \return Returns the total number of bytes allocated.
		 */
		LSE_INLINE uintptr_t LSE_CALL			TotalAllocated() const;


	protected :
		// == Members.
		/** The next location from which an allocation will be made. */
		uintptr_t								m_uiptrNext;

		/** The start of the allocated memory. */
		uintptr_t								m_uiptrEnd;

		/** The size of the buffer in bytes. */
		uintptr_t								m_uiptrSize;

		/** How much total memory has been allocated? */
		uintptr_t								m_uiptrTotalAllocated;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the index of the oldest-allocated item.
	 *
	 * \return Returns the index of the oldest-allocated item.
	 */
	LSE_INLINE uintptr_t LSE_CALL CRingBufferCounter::OldestAllocated() const {
		return m_uiptrEnd;
	}

	/**
	 * Gets the total number of bytes allocated.
	 *
	 * \return Returns the total number of bytes allocated.
	 */
	LSE_INLINE uintptr_t LSE_CALL CRingBufferCounter::TotalAllocated() const {
		return m_uiptrTotalAllocated;
	}

}	// namespace lsstd

#endif	// __LSSTD_RINGBUFFERCOUNTER_H__
