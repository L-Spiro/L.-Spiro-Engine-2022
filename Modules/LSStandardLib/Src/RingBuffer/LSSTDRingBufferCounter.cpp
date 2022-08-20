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

#include "LSSTDRingBufferCounter.h"

#include <cassert>


namespace lsstd {

	// == Various constructors.
	LSE_CALLCTOR CRingBufferCounter::CRingBufferCounter() :
		m_uiptrNext( 0 ),
		m_uiptrEnd( 0 ),
		m_uiptrSize( 0 ),
		m_uiptrTotalAllocated( 0 ) {
	}

	// == Functions.
	/**
	 * Sets the size of the buffer.
	 *
	 * \param _uiptrSize Size of the buffer.
	 */
	void LSE_CALL CRingBufferCounter::SetSize( uintptr_t _uiptrSize ) {
		m_uiptrSize = _uiptrSize;
		m_uiptrNext = m_uiptrEnd = m_uiptrTotalAllocated;
	}

	/**
	 * Returns true if the requested number of bytes can be allocated from the ring
	 *	buffer.
	 *
	 * \param _uiptrSize Number of bytes to check for being possible to allocate.
	 * \return Returns true if the requested number of bytes can be allocated from the ring
	 *	buffer.
	 */
	LSBOOL LSE_CALL CRingBufferCounter::CanAllocate( uintptr_t _uiptrSize ) const {
		// There is a special case in which the start and end pointers are the same but not because
		//	there is nothing allocated.  It happens when the buffer is perfectly full.
		if ( (m_uiptrNext == m_uiptrEnd && m_uiptrTotalAllocated) || _uiptrSize > m_uiptrSize ) { return false; }

		// If the next pointer is higher than or equal to the allocation-start pointer,
		//	first see if there is enough memory until the end of the buffer.
		if ( m_uiptrNext >= m_uiptrEnd ) {
			// Can we allocate directly from m_uiptrNext?  Is there enough space until the end of the buffer?
			if ( m_uiptrSize - m_uiptrNext >= _uiptrSize ) { return true; }
			// What if we allocated from the start of the ring buffer with m_uiptrNext = 0?
			if ( m_uiptrEnd >= _uiptrSize ) { return true; }
			return false;
		}
		// The only factor here is how much space there is between m_uiptrEnd (the high number) and m_uiptrNext.
		return (m_uiptrEnd - m_uiptrNext) >= _uiptrSize;
	}

	/**
	 * Gets the index in bytes of the allocated memory.  If there is not enough memory
	 *	to perform the allocation false is returned.
	 *
	 * \param _uiptrSize Size in bytes to allocate.
	 * \param _uiptrIndex Index of the allocated bytes within the buffer in bytes.
	 * \return Returns true if the allocation succeeded.
	 */
	LSBOOL LSE_CALL CRingBufferCounter::Alloc( uintptr_t _uiptrSize, uintptr_t &_uiptrIndex ) {
		// There is a special case in which the start and end pointers are the same but not because
		//	there is nothing allocated.  It happens when the buffer is perfectly full.
		if ( (m_uiptrNext == m_uiptrEnd && m_uiptrTotalAllocated) || _uiptrSize > m_uiptrSize ) { return false; }
		// If the next pointer is higher than or equal to the allocation-start pointer,
		//	first see if there is enough memory until the end of the buffer.
		if ( m_uiptrNext >= m_uiptrEnd ) {
			// Can we allocate directly from m_uiptrNext?  Is there enough space until the end of the buffer?
			if ( m_uiptrSize - m_uiptrNext >= _uiptrSize ) {
				_uiptrIndex = m_uiptrNext;
				m_uiptrNext += _uiptrSize;
				m_uiptrTotalAllocated += _uiptrSize;
				return true;
			}
			// What if we allocated from the start of the ring buffer with m_uiptrNext = 0?
			if ( m_uiptrEnd >= _uiptrSize ) {
				// Total allocated is increased to include the bytes at the end of the buffer we aren't using.
				m_uiptrTotalAllocated += m_uiptrSize - m_uiptrNext + _uiptrSize;
				_uiptrIndex = 0;
				m_uiptrNext = _uiptrSize;
				return true;
			}
			return false;
		}
		if ( (m_uiptrEnd - m_uiptrNext) >= _uiptrSize ) {
			_uiptrIndex = m_uiptrNext;
			m_uiptrNext += _uiptrSize;
			m_uiptrTotalAllocated += _uiptrSize;
			return true;
		}
		return false;
	}

	/**
	 * Frees memory back to the ring buffer.
	 *
	 * \param _uiptrSize Size in bytes to return back to the ring buffer.
	 * \param _uiptrIndex Index of the freed bytes within the buffer in bytes.
	 * \return Returns the number of bytes actually returned back to the buffer, which may be larger than
	 *	the requested size on wrap-around frees.  A return of 0 indicates no memory was freed because
	 *	the free amount is not allocated from the fing buffer.
	 */
	uintptr_t LSE_CALL CRingBufferCounter::Free( uintptr_t _uiptrSize, uintptr_t &_uiptrIndex ) {
		_uiptrIndex = static_cast<uintptr_t>(-1);
		if ( m_uiptrTotalAllocated < _uiptrSize ) { return 0; }

		uintptr_t uiptrTotal = m_uiptrTotalAllocated;
		// If the end is below the start.
		if ( m_uiptrEnd <= m_uiptrNext ) {
			// We can free up to m_uiptrNext - m_uiptrEnd bytes.
			if ( m_uiptrNext - m_uiptrEnd >= _uiptrSize ) {
				_uiptrIndex = m_uiptrEnd;
				m_uiptrTotalAllocated -= _uiptrSize;
				m_uiptrEnd += _uiptrSize;
			}
		}
		else {
			// If there is not enough space to free from m_uiptrEnd until the end of the buffer
			//	then the allocation must have also overflowed the buffer.  Free starting from 0
			//	but count the trailing bytes in the total freed.
			if ( m_uiptrSize - m_uiptrEnd < _uiptrSize ) {
				// Not enough past the end of the buffer.  Is there enough from the start of the buffer?
				if ( m_uiptrNext >= _uiptrSize ) {
					m_uiptrTotalAllocated -= (m_uiptrSize - m_uiptrEnd) + _uiptrSize;
					m_uiptrEnd = _uiptrSize;
					_uiptrIndex = 0;
				}
			}
			else {
				// Enough past the end of the buffer.
				_uiptrIndex = m_uiptrEnd;
				m_uiptrTotalAllocated -= _uiptrSize;
				m_uiptrEnd += _uiptrSize;
				m_uiptrEnd %= m_uiptrSize;
			}
		}

		if ( m_uiptrTotalAllocated == 0 ) {
			assert( m_uiptrNext == m_uiptrEnd );
			// Nothing allocated so set the pointers back to 0 to increase efficiency.
			m_uiptrNext = m_uiptrEnd = 0;
		}
		return uiptrTotal - m_uiptrTotalAllocated;
	}

}	// namespace lsstd
