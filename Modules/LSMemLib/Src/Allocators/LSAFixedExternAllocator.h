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
 * Description: An allocator that works on an external byte array.  All allocations are of a fixed size and there
 *	is a maximum number of allocations because the class does not manage the buffer from which it produces
 *	allocations.
 */


#ifndef __LSA_FIXEDEXTERNALLOCATOR_H__
#define __LSA_FIXEDEXTERNALLOCATOR_H__

#include "../OSHeap/LSAOsHeap.h"
#include <new>

#pragma warning( push )

// warning C4189: 'ptThis' : local variable is initialized but not referenced
#pragma warning( disable : 4189 )
// warning C4309: 'static_cast' : truncation of constant value
#pragma warning( disable : 4309 )

namespace lsa {

	/**
	 * Class CFixedExternalAllocator
	 * \brief An allocator that works on an external byte array.
	 *
	 * Description: An allocator that works on an external byte array.  All allocations are of a fixed size and there
	 *	is a maximum number of allocations because the class does not manage the buffer from which it produces
	 *	allocations.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	class CFixedExternalAllocator {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CFixedExternalAllocator();
		LSE_CALLCTOR							CFixedExternalAllocator( _tType * _ptData );
		LSE_CALLCTOR							~CFixedExternalAllocator();


		// == Functions.
		/**
		 * Sets the buffer.  Causes a full reset.
		 *
		 * \param _ptData The new data pointer.
		 */
		LSE_INLINE void LSE_FCALL				SetPointer( _tType * _ptData );

		/**
		 * Allocates and constructs a single value from the buffer.
		 *
		 * \return Returns a pointer to the allocated object.
		 */
		LSE_INLINE _tType * LSE_FCALL			Alloc();

		/**
		 * Frees a value by pointer.
		 *
		 * \param _ptPtr Value to free by pointer.
		 * \return Returns true if the address was allocated by this allocator.
		 */
		LSE_INLINE LSBOOL LSE_FCALL				Free( _tType * _ptPtr );

		/**
		 * Resets the buffer and destroys all allocations.
		 */
		LSE_INLINE void LSE_CALL				Reset();

		/**
		 * Swaps this object's contents with another's.
		 *
		 * \param _feaOther Object with which to swap contents.
		 */
		LSE_INLINE void LSE_FCALL				Swap( CFixedExternalAllocator<_tType, _uMaxAllocations> &_feaOther );

		/**
		 * Gets the size in bytes of the buffer that should be allocated for this instance of this class.
		 *
		 * \return Returns the size in bytes of the buffer that should be allocated for this instance of this class.
		 */
		static LSE_INLINE uintptr_t LSE_FCALL	AllocationSize();

		/**
		 * Returns 1 << _uiptrBit.
		 *
		 * \param _uiptrBit Bit to set.
		 * \return Returns 1 << _uiptrBit.
		 */
		static LSE_INLINE uintptr_t LSE_FCALL	Bit( uintptr_t _uiptrBit );


	protected :
		// == Members.
#define LSA_BITS		(sizeof( uintptr_t )*8)
#define LSA_LEN2BITS	(_uMaxAllocations/LSA_BITS)
		/** Flags to keep track of allocations made. */
		uintptr_t								m_uiptrFlags[LSA_LEN2BITS+
			((LSA_LEN2BITS*LSA_BITS==_uMaxAllocations)?0:1)];
#undef LSA_LEN2BITS
#undef LSA_BITS

		/** Buffer from which allocations are made. */
		_tType *								m_ptData;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_CALLCTOR CFixedExternalAllocator<_tType, _uMaxAllocations>::CFixedExternalAllocator() :
		m_ptData( NULL ) {
		for ( uintptr_t I = LSE_ELEMENTS( m_uiptrFlags ); I--; ) {
			m_uiptrFlags[I] = 0;
		}
	}
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_CALLCTOR CFixedExternalAllocator<_tType, _uMaxAllocations>::CFixedExternalAllocator( _tType * _ptData ) :
		m_ptData( _ptData ) {
		for ( uintptr_t I = LSE_ELEMENTS( m_uiptrFlags ); I--; ) {
			m_uiptrFlags[I] = 0;
		}
	}
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_CALLCTOR CFixedExternalAllocator<_tType, _uMaxAllocations>::~CFixedExternalAllocator() {
		Reset();
	}

	// == Functions.
	/**
	 * Sets the buffer.  Causes a full reset.
	 *
	 * \param _ptData The new data pointer.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE void LSE_FCALL CFixedExternalAllocator<_tType, _uMaxAllocations>::SetPointer( _tType * _ptData ) {
		Reset();
		m_ptData = _ptData;
	}

	/**
	 * Allocates and constructs a single value from the buffer.
	 *
	 * \return Returns a pointer to the allocated object.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE _tType * LSE_FCALL CFixedExternalAllocator<_tType, _uMaxAllocations>::Alloc() {
		for ( uintptr_t I = 0; I < LSE_ELEMENTS( m_uiptrFlags ); ++I ) {
			if ( m_uiptrFlags[I] != static_cast<uintptr_t>(LSE_MAXU64) ) {
				// At least 1 bit not set here.
				uint32_t ui32Bit = 0;
				for ( uint32_t J = 0; J < sizeof( uintptr_t ) * 8; ++J ) {
					if ( !(m_uiptrFlags[I] & Bit( J )) ) {
						ui32Bit = J;
						break;
					}
				}
				// Convert to an address.
				_tType * ptThis = m_ptData + I * sizeof( uintptr_t ) * 8 + ui32Bit;
				if ( ptThis >= m_ptData + _uMaxAllocations ) { return NULL; }
				m_uiptrFlags[I] |= Bit( ui32Bit );
				return new( ptThis ) _tType();
			}
		}
		return NULL;
	}

	/**
	 * Frees a value by pointer.
	 *
	 * \param _ptPtr Value to free by pointer.
	 * \return Returns true if the address was allocated by this allocator.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE LSBOOL LSE_FCALL CFixedExternalAllocator<_tType, _uMaxAllocations>::Free( _tType * _ptPtr ) {
		uintptr_t uiptrIndex = _ptPtr - m_ptData;
		if ( uiptrIndex >= _uMaxAllocations ) { return false; }
		// Convert to bits.
		uintptr_t uiptrArrayIndex = uiptrIndex / (sizeof( uintptr_t ) * 8);
		uintptr_t uiptrBitIndex = uiptrIndex - uiptrArrayIndex * (sizeof( uintptr_t ) * 8);
		uiptrBitIndex = Bit( uiptrBitIndex );
		if ( !(m_uiptrFlags[uiptrArrayIndex] & uiptrBitIndex) ) { return false; }
		_ptPtr->~_tType();
		m_uiptrFlags[uiptrArrayIndex] &= ~uiptrBitIndex;
		return true;
	}

	/**
	 * Resets the buffer and destroys all allocations.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE void LSE_CALL CFixedExternalAllocator<_tType, _uMaxAllocations>::Reset() {
		const uint32_t ui32Bits = sizeof( uintptr_t ) * 8;
		for ( uintptr_t I = 0; I < LSE_ELEMENTS( m_uiptrFlags ); ++I ) {
			for ( uint32_t J = 0; J < ui32Bits; ++J ) {
				if ( I * ui32Bits + J >= _uMaxAllocations ) { return; }
				if ( m_uiptrFlags[I] & Bit( J ) ) {
					_tType * ptThis = m_ptData + (I * ui32Bits + J);
					ptThis->~_tType();
				}
			}
			m_uiptrFlags[I] = 0;
		}
		m_ptData = NULL;
	}

	/**
	 * Swaps this object's contents with another's.
	 *
	 * \param _feaOther Object with which to swap contents.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE void LSE_FCALL CFixedExternalAllocator<_tType, _uMaxAllocations>::Swap( CFixedExternalAllocator<_tType, _uMaxAllocations> &_feaOther ) {
		CStd::Swap( m_ptData, _feaOther.m_ptData );
		for ( uintptr_t I = 0; I < LSE_ELEMENTS( m_uiptrFlags ); ++I ) {
			CStd::Swap( m_uiptrFlags[I], _feaOther.m_uiptrFlags[I] );
		}
	}

	/**
	 * Gets the size in bytes of the buffer that should be allocated for this instance of this class.
	 *
	 * \return Returns the size in bytes of the buffer that should be allocated for this instance of this class.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE uintptr_t LSE_FCALL CFixedExternalAllocator<_tType, _uMaxAllocations>::AllocationSize() {
		return _uMaxAllocations * sizeof( _tType );
	}

	/**
	 * Returns 1 << _uiptrBit.
	 *
	 * \param _uiptrBit Bit to set.
	 * \return Returns 1 << _uiptrBit.
	 */
	template <typename _tType, unsigned _uMaxAllocations>
	LSE_INLINE uintptr_t LSE_FCALL CFixedExternalAllocator<_tType, _uMaxAllocations>::Bit( uintptr_t _uiptrBit ) {
		return (static_cast<uintptr_t>(1) << _uiptrBit);
	}

}	// namespace lsa

#pragma warning( pop )

#endif	// __LSA_FIXEDEXTERNALLOCATOR_H__
