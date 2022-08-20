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
 * Description: Extends the lsstd::CBitStream class with memory-manager functionality.
 */

#include "LSCBitStream.h"


namespace lsc {
	
	// == Various constructors.
	LSE_CALLCTOR CBitStream::CBitStream() :
		m_paAllocator( NULL ) {
	}
	LSE_CALLCTOR CBitStream::~CBitStream() {
		Reset();
	}

	/**
	 * Sets the allocator.  Causes all data in the bit stream to be lost if the allocator is not the
	 *	same as the only already set.
	 *
	 * \param _paAllocator The allocator to use by this class.
	 */
	void LSE_CALL CBitStream::SetAllocator( CAllocator * _paAllocator ) {
		if ( _paAllocator != m_paAllocator ) {
			Reset();
			m_paAllocator = _paAllocator;
		}
	}

	/**
	 * Gets our allocator.
	 *
	 * \return Returns the allocator assigned to this object.
	 */
	CAllocator * LSE_CALL CBitStream::GetAllocator() {
		return m_paAllocator;
	}

	/**
	 * Re-allocate bytes.  Overrides the base definition to allow usage of our allocator, if not NULL.
	 *
	 * \param _pvOriginal Address to re-alloate.  Pass NULL to allocate new data.
	 * \param _ui32Total Total number of bytes to allocate.
	 * \return Returns the re-allocated bytes or NULL upon failure.
	 */
	void * LSE_CALL CBitStream::ReAlloc( void * _pvOriginal, uint32_t _ui32Total ) const {
		if ( m_bBorrowed ) { return NULL; }
		if ( !m_paAllocator ) { return CMemLib::MReAlloc( _pvOriginal, _ui32Total ); }
		return m_paAllocator->ReAlloc( _pvOriginal, _ui32Total );
	}

	/**
	 * Free bytes.  Overrides the base definition to allow usage of our allocator, if not NULL.
	 *
	 * \param _pvOriginal Pointer to the data, allocated with ReAlloc() to free.
	 */
	void LSE_CALL CBitStream::Free( void * _pvOriginal ) const {
		if ( m_bBorrowed ) { return; }
		if ( !m_paAllocator ) { CMemLib::Free( _pvOriginal ); }
		else { m_paAllocator->Free( _pvOriginal ); }
	}

}	// namespace lsc
