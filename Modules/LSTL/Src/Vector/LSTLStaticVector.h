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
 * Description: A vector with a fixed maximum length.  No allocations are used during the life of the vector,
 *	its goal being performance.
 */


#ifndef __LSTL_STATICVECTOR_H__
#define __LSTL_STATICVECTOR_H__

#include "../LSTLib.h"
#include <cassert>
#include <new>

namespace lstl {

	/**
	 * Class CStaticVector
	 * \brief A vector with a fixed maximum length.
	 *
	 * Description: A vector with a fixed maximum length.  No allocations are used during the life of the vector,
	 *	its goal being performance.
	 */
	template <typename _tType, unsigned _uMaxSize = 512>
	class CStaticVector {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CStaticVector() :
		   m_tLen( 0 ) {
		}
		LSE_CALLCTOR							CStaticVector( uint32_t _tTotal ) :
		   m_tLen( 0 ) {
		   assert( _tTotal <= _uMaxSize );
		   for ( uint32_t I = _tTotal; I--; ) {
			   Construct( I );
		   }
		}
		LSE_CALLCTOR							CStaticVector( const CStaticVector<_tType, _uMaxSize> &_vOther ) {
		   (*this) = _vOther;
		}
		LSE_CALLCTOR							~CStaticVector() {
			Reset();
		}


		// == Operators.
		/**
		 * Give rapid array access.
		 *
		 * \param _ui32Index The index of the item to get.
		 * \return Returns a constant reference to the item at the given index.
		 */
		const _tType & LSE_CALL					operator [] ( uint32_t _ui32Index ) const {
			assert( _ui32Index < m_tLen );
			return m_ptData[_ui32Index];
		}

		/**
		 * Give rapid array access.
		 *
		 * \param _ui32Index The index of the item to get.
		 * \return Returns a reference to the item at the given index.
		 */
		_tType & LSE_CALL						operator [] ( uint32_t _ui32Index ) {
			assert( _ui32Index < m_tLen );
			return m_ptData[_ui32Index];
		}

		/**
		 * Copy the vector and every item in it to a new vector.  Each item is duplicated
		 *	in the new vector.
		 *
		 * \param _vcOther The source vector.
		 * \return Returns this vector, which contains a copy of every item in the source vector.
		 */
		CStaticVector<_tType, _uMaxSize> & LSE_CALL
												operator = ( const CStaticVector<_tType, _uMaxSize> &_vcOther ) {
			// Clear every item in this list.
			Reset();

			// Copy items one-by-one.
			for ( uint32_t I = 0; I < _vcOther.Length(); ++I ) {
				if ( !Push( _vcOther[I] ) ) { throw LSSTD_E_OUTOFMEMORY; }
			}
			return (*this);
		}

		/**
		 * Moves ownership of the data to this vector from the old one.
		 *
		 * \param _vcOther The source vector.
		 * \return Returns this vector, which contains a copy of every item in the source vector.
		 */
		CStaticVector<_tType, _uMaxSize> & LSE_CALL
												operator = ( CStaticVector<_tType, _uMaxSize> &&_vcOther ) {
			// Clear every item in this list.
			Reset();

			m_ptData = _vcOther.m_ptData;
			_vcOther.m_ptData = nullptr;
			m_tLen = _vcOther.m_tLen;
			_vcOther.m_tLen = 0;
			return (*this);
		}


		// == Functions.
		/**
		 * Reset the list completely.
		 */
		void LSE_CALL							Reset() {
			for ( int32_t I = static_cast<int32_t>(m_tLen); --I >= 0; ) {
				Destroy( static_cast<uint32_t>(I) );
			}
			m_tLen = 0;
		}

		/**
		 * An alias for Reset().
		 */
		void LSE_CALL							ResetNoDealloc() {
			Reset();
		}

		/**
		 * Because no allocations are made by this class, this is the same as Reset().
		 */
		void LSE_CALL							Trash() {
			Reset();
		}

		/**
		 * If _ui32Total is 0, the array is reset, otherwise false is returned.
		 *
		 * \param _ui32Total The new number of items to be allocated.
		 * \return Returns false if _ui32Total is not 0.
		 */
		LSBOOL LSE_CALL							Allocate( uint32_t _ui32Total ) {
			// If allocating 0 bytes, just reset the list.
			if ( !_ui32Total ) {
				Reset();
				return true;
			}

			return false;
		}

		/**
		 * Returns the _uMaxSize template parameter.
		 *
		 * \return Returns the _uMaxSize template parameter.
		 */
		uint32_t LSE_CALL						GetAllocated() const { return _uMaxSize; }

		/**
		 * Resize the array.
		 *
		 * \param _ui32Total The new total of items in the array.
		 * \return Returns false if the memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL							Resize( uint32_t _ui32Total ) {
			assert( m_tLen + _ui32Total <= _uMaxSize );
			for ( ; m_tLen < _ui32Total; ++m_tLen ) {
				Construct( m_tLen );
			}
			return true;
		}

		/**
		 * Resize the array with a little over-allocation if necessary.
		 *
		 * \param _ui32Total The new total of items in the array.
		 * \return Returns false if the memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL							ResizeOver( uint32_t _ui32Total ) {
			return Resize( _ui32Total );
		}

		/**
		 * Adds to the end of the array.  Item is copied via copy constructor.
		 *
		 * \param _tVal The value to push.
		 * \return Returns false if memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL							Push( const _tType &_tVal ) {
			// Do we need to allocate more space?
			if ( m_tLen == _uMaxSize ) {
				// Overflow checking.
				return false;
			}
			Construct( m_tLen );
			m_ptData[m_tLen++] = _tVal;
			return true;
		}

		/**
		 * Removes from the end of the array.
		 */
		void LSE_CALL							Pop() {
			if ( m_tLen == 0 ) { return; }
			Destroy( --m_tLen );
		}

		/**
		 * An alias for Pop().
		 */
		void LSE_CALL							PopNoDealloc() {
			Pop();
		}

		/**
		 * Insert an element.
		 *
		 * \param _tVal The item to insert.
		 * \param _ui32Index The index where the item is to be inserted.
		 * \return Returns false if memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL							Insert( const _tType &_tVal, uint32_t _ui32Index ) {
			assert( _ui32Index <= m_tLen );
			// If inserting at the end, just push the item.
			if ( _ui32Index == m_tLen ) { return this->Push( _tVal ); }

			// Now we know we are inserting in the middle somewhere.
			if ( m_tLen == _uMaxSize ) {
				// Overflow checking.
				return false;
			}

			// Move other items.
			// Since this is not a PoD handler, we cannot simply move memory.
			// The last item has not been constructed yet, so we cannot call its copy operator yet.
			this->Construct( m_tLen );
			// Move items up one-by-one.
			for ( uint32_t I = m_tLen; I > _ui32Index; --I ) {
				m_ptData[I] = m_ptData[I-1];
			}
			// No need to destroy/construct the item at the given location.  Its copy operator should handle freeing of
			//	its memory.
			m_ptData[_ui32Index] = _tVal;
			++m_tLen;
			return true;
		}

		/**
		 * Append an array of values to the end of the vector.
		 *
		 * \param _ptValues The values to append.
		 * \param _tTotal The number of values to append.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							Append( const _tType * _ptValues, uint32_t _tTotal ) {
			uintptr_t uiptrNewLen = m_tLen + _tTotal;
			if ( uiptrNewLen >= _uMaxSize ) {
				return false;
			}
			for ( uintptr_t I = m_tLen; I < uiptrNewLen; ++I ) {
				new( &m_ptData[I] ) _tType( (*_ptValues++) );
			}
			m_tLen += _tTotal;
			return true;
		}

		/**
		 * Removes an element.
		 *
		 * \param _ui32Index The index of the item to remove.
		 */
		void LSE_CALL							Remove( uint32_t _ui32Index ) {
			RemoveRange( _ui32Index, 1 );
		}

		/**
		 * An alias for Remove()
		 *
		 * \param _ui32Index The index of the item to remove.
		 */
		void LSE_CALL							RemoveNoDealloc( uint32_t _ui32Index ) {
			Remove( _ui32Index );
		}

		/**
		 * Removes elements.
		 *
		 * \param _ui32Index The start index of the items to be removed.
		 * \param _ui32Total The number of items to remove.
		 */
		void LSE_CALL							RemoveRange( uint32_t _ui32Index, uint32_t _ui32Total ) {
			if ( _ui32Total == 0 ) { return; }
			assert( _ui32Index < m_tLen );
			assert( _ui32Index + _ui32Total <= m_tLen );
			uint32_t ui32End = m_tLen - _ui32Total;
			
			// Copy items over it.
			// Since this is not a PoD handler, we cannot simply move memory.
			uint32_t ui32CopyEnd = m_tLen - _ui32Total;
			for ( uint32_t I = _ui32Index; I < ui32CopyEnd; ++I ) {
				m_ptData[I] = m_ptData[I+_ui32Total];
			}

			// Destroy the tail items that were moved down.
			for ( int32_t I = static_cast<int32_t>(m_tLen); --I >= static_cast<int32_t>(ui32End); ) {
				// Destruct the item.
				Destroy( static_cast<uint32_t>(I) );
			}
			m_tLen = m_tLen - static_cast<uint32_t>(_ui32Total);
		}

		/**
		 * An alias for RemoveRange().
		 *
		 * \param _ui32Index The start index of the items to be removed.
		 * \param _ui32Total The number of items to remove.
		 */
		void LSE_CALL							RemoveRangeNoDealloc( uint32_t _ui32Index, uint32_t _ui32Total ) {
			RemoveRange( _ui32Index, _ui32Total );
		}

		/**
		 * Gets the number of elements in the array.
		 *
		 * \return Returns the number of items in the array.
		 */
		uint32_t LSE_CALL						Length() const {
			return m_tLen;
		}

		/**
		 * Snap it to the size of the list.  This removes allocated space beyond the actual length of the array.
		 */
		void LSE_CALL							Snap() {
			Allocate( m_tLen );
		}


	protected :
		// == Members.
		/**
		 * Array of data.
		 */
		_tType									m_ptData[_uMaxSize];

		/**
		 * Length of the list.
		 */
		uint32_t								m_tLen;


		// == Functions.
		/**
		 * Secret way of constructing items.
		 *
		 * \param _tIndex The index of the item to construct.
		 */
		void LSE_CALL							Construct( uint32_t _tIndex ) {
			new( &m_ptData[_tIndex] ) _tType;
		}
		
		/**
		 * Secret way of destroying items.  No error checking.
		 *
		 * \param _tIndex The index of the item to destruct.
		 */
		void LSE_CALL							Destroy( uint32_t _tIndex ) {
			// This gives warning C4100 when this class is created with types that have no destructor,
			//	claiming _tIndex is unreferenced.
			// Erase this warning with some do-nothing code.
#ifdef LSE_VISUALSTUDIO
			static_cast<uint32_t>(_tIndex);
#endif	// #ifdef LSE_VISUALSTUDIO
			m_ptData[_tIndex].~_tType();
		}
	};

}	// namespace lstl

#endif	// __LSTL_STATICVECTOR_H__
