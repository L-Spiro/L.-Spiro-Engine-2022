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
 * Description: We use the Curious Recurring Template Pattern to change how Allocate() works between
 *	different versions of the vectors, since virtual functions are not allowed on templates.
 * This class provides the base members for all vector instances (which is different from CVectorBase,
 *	which provides static members that vector instances share) and uses _tDerivedType to cast the object to
 *	the correct type before calling functions.  Read about Curious Recurring Template Pattern if you do
 *	not understand the logic behind this.
 * This class cannot be instantiated directly!
 */


#ifndef __LSTL_SVECTORCRTP_H__
#define __LSTL_SVECTORCRTP_H__

#include "../LSTLib.h"
#include "LSTLVectorBase.h"
#include <cassert>
#include <new>

namespace lstl {

	/**
	 * Class CSVectorCrtp
	 * \brief A basic vector.
	 *
	 * Description: A basic vector.
	 */
	template <typename _tDerivedType,
		typename _tType, typename _tDataType = uint16_t, unsigned _uAllocSize = 512>
	class CSVectorCrtp : protected CVectorBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR				CSVectorCrtp() :
			m_tLen( 0 ),
			m_tAllocated( 0 ),
			m_ptData( NULL ) {
		}
		LSE_CALLCTOR				CSVectorCrtp( const CSVectorCrtp<_tDerivedType, _tType, _tDataType, _uAllocSize> &_vcSource ) :
			m_tLen( 0 ),
			m_tAllocated( 0 ),
			m_ptData( NULL ) {
			(*this) = _vcSource;
		}
		LSE_CALLCTOR				~CSVectorCrtp() {
			Reset();
		}


		// == Operators.
		/**
		 * Give rapid array access.
		 *
		 * \param _ui32Index The index of the item to get.
		 * \return Returns a constant reference to the item at the given index.
		 */
		const _tType & LSE_CALL		operator [] ( uint32_t _ui32Index ) const {
			assert( _ui32Index < m_tLen );
			return m_ptData[_ui32Index];
		}

		/**
		 * Give rapid array access.
		 *
		 * \param _ui32Index The index of the item to get.
		 * \return Returns a reference to the item at the given index.
		 */
		_tType & LSE_CALL			operator [] ( uint32_t _ui32Index ) {
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
		CSVectorCrtp<_tDerivedType, _tType, _tDataType, _uAllocSize> & LSE_CALL
									operator = ( const CSVectorCrtp<_tDerivedType, _tType, _tDataType, _uAllocSize> &_vcOther ) {
			// Clear every item in this list.
			static_cast<_tDerivedType *>(this)->ResetNoDealloc();

			// If the allocation is too small or too high, reallocate.
			if ( m_tAllocated < _vcOther.m_tLen || m_tAllocated >= (_vcOther.m_tLen << 1) ) {
				static_cast<_tDerivedType *>(this)->Allocate( _vcOther.m_tLen );
			}

			// Copy items one-by-one.
			for ( uint32_t I = 0; I < _vcOther.Length(); ++I ) {
				if ( !static_cast<_tDerivedType *>(this)->Push( _vcOther[I] ) ) { throw LSSTD_E_OUTOFMEMORY; }
			}
			return (*this);
		}
		/**
		 * Moves ownership of the data to this vector from the old one.
		 *
		 * \param _vcOther The source vector.
		 * \return Returns this vector, which contains a copy of every item in the source vector.
		 */
		CSVectorCrtp<_tDerivedType, _tType, _tDataType, _uAllocSize> & LSE_CALL
									operator = ( const CSVectorCrtp<_tDerivedType, _tType, _tDataType, _uAllocSize> &&_vcOther ) {
			// Clear every item in this list.
			static_cast<_tDerivedType *>(this)->Reset();
			m_ptData = _vcOther.m_ptData;
			_vcOther.m_ptData = nullptr;
			m_tAllocated = _vcOther.m_tAllocated;
			_vcOther.m_tAllocated = 0;
			m_tLen = _vcOther.m_tLen;
			_vcOther.m_tLen = 0;
			return (*this);
		}


		// == Functions.
		/**
		 * Reset the list completely.  Frees all memory.
		 */
		void LSE_CALL				Reset() {
			for ( int32_t I = static_cast<int32_t>(m_tLen); --I >= 0; ) {
				Destroy( static_cast<_tDataType>(I) );
			}
			m_paDefaultAllocator->Free( m_ptData );
			m_ptData = NULL;
			m_tLen = m_tAllocated = 0;
		}

		/**
		 * Reset the list without deallocating the whole buffer.
		 */
		void LSE_CALL				ResetNoDealloc() {
			for ( int32_t I = static_cast<int32_t>(m_tLen); --I >= 0; ) {
				Destroy( static_cast<_tDataType>(I) );
			}
			m_tLen = 0;
		}

		/**
		 * Trash the list without freeing the memory.  Calls destructors on objects and resets
		 *	without freeing any memory.  The memory is assumed to be free by another part of the
		 *	application.  For example, if the memory was allocated to a heap and the heap is to
		 *	be freed at all once.  After calling Trash(), the whole heap could be removed in full
		 *	without creating a memory leak.
		 */
		void LSE_CALL				Trash() {
			for ( _tDataType I = m_tLen; I--; ) {
				Destroy( I );
			}
			m_ptData = NULL;
			m_tLen = m_tAllocated = 0;
		}

		/**
		 * Allocates a given number of elements.
		 * If the allocation is less than what there already is, items are removed.
		 *
		 * \param _ui32Total The new number of items to be allocated.
		 * \return Returns false if the memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL				Allocate( uint32_t _ui32Total ) {
			if ( m_tLen == static_cast<_tDataType>(_ui32Total) ) {
				return true;
			}

			// If allocating 0 bytes, just reset the list.
			if ( !_ui32Total ) {
				static_cast<_tDerivedType *>(this)->Reset();
				return true;
			}

			// Destroy items that are going to be removed.
			if ( m_tLen ) {
				for ( _tDataType I = m_tLen; --I >= _ui32Total; ) {
					Destroy( I );
				}
			}
			// Adjust the length.
			if ( m_tLen > static_cast<_tDataType>(_ui32Total) ) {
				m_tLen = static_cast<_tDataType>(_ui32Total);
			}

			// Attempt to allocate.
			_tType * ptNew = reinterpret_cast<_tType *>(m_paDefaultAllocator->ReAlloc( m_ptData, _ui32Total * sizeof( _tType ) ));
			if ( !ptNew ) { return false; }

			// Success.
			m_ptData = ptNew;
			m_tAllocated = static_cast<_tDataType>(_ui32Total);
			return true;
		}

		/**
		 * Gets the number of items allocated.
		 *
		 * \return Returns the number of allocated items.
		 */
		_tDataType LSE_CALL			GetAllocated() const { return m_tAllocated; }

		/**
		 * Resize the array.
		 *
		 * \param _ui32Total The new total of items in the array.
		 * \return Returns false if the memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL				Resize( uint32_t _ui32Total ) {
			if ( !static_cast<_tDerivedType *>(this)->Allocate( _ui32Total ) ) { return false; }
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
		LSBOOL LSE_CALL				ResizeOver( uint32_t _ui32Total ) {
			if ( m_tAllocated < _ui32Total ) {
				if ( !static_cast<_tDerivedType *>(this)->Allocate( _ui32Total + _uAllocSize ) ) { return false; }
			}
			for ( ; m_tLen < _ui32Total; ++m_tLen ) {
				Construct( m_tLen );
			}
			return true;
		}

		/**
		 * Adds to the end of the array.  Item is copied via copy constructor.
		 *
		 * \param _tVal The value to push.
		 * \return Returns false if memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL				Push( const _tType &_tVal ) {
			// Do we need to allocate more space?
			if ( m_tLen == m_tAllocated ) {
				// Overflow checking.
				_tDataType tNewTotal = static_cast<_tDataType>(m_tAllocated + _uAllocSize);
				assert( tNewTotal > m_tAllocated );
				if ( !static_cast<_tDerivedType *>(this)->Allocate( tNewTotal ) ) { return false; }
			}
			Construct( m_tLen );
			m_ptData[m_tLen++] = _tVal;
			return true;
		}

		/**
		 * Removes from the end of the array.
		 */
		void LSE_CALL				Pop() {
			if ( m_tLen == 0 ) { return; }
			Destroy( --m_tLen );
			// If there are fewer than half of the allocated space worth of objects,
			//	adjust the allocation.
			if ( m_tLen < (m_tAllocated >> 1) ) {
				static_cast<_tDerivedType *>(this)->Allocate( (m_tAllocated >> 1) );
			}
		}

		/**
		 * Removes an item from the end of the array without resizing if the array is less than double sized.
		 */
		void LSE_CALL				PopNoDealloc() {
			if ( m_tLen == 0 ) { return; }
			Destroy( --m_tLen );
		}

		/**
		 * Insert an element.
		 *
		 * \param _tVal The item to insert.
		 * \param _ui32Index The index where the item is to be inserted.
		 * \return Returns false if memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL				Insert( const _tType &_tVal, uint32_t _ui32Index ) {
			assert( _ui32Index <= m_tLen );
			if ( m_tLen == m_tAllocated ) {
				// Overflow checking.
				_tDataType tNewTotal = m_tAllocated + _uAllocSize;
				assert( tNewTotal > m_tAllocated );
				if ( !static_cast<_tDerivedType *>(this)->Allocate( tNewTotal ) ) { return false; }
			}

			// Move other items.
			CStd::MemMove( &m_ptData[_ui32Index+1], &m_ptData[_ui32Index], sizeof( _tType ) * (m_tLen - _ui32Index) );
			// Construct and add.
			Construct( static_cast<_tDataType>(_ui32Index) );
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
		LSBOOL LSE_CALL				Append( const _tType * _ptValues, _tDataType _tTotal ) {
			uintptr_t uiptrNewLen = m_tLen + _tTotal;
			if ( uiptrNewLen >= m_tAllocated ) {
				if ( !static_cast<_tDerivedType *>(this)->Allocate( static_cast<_tDataType>(uiptrNewLen + _uAllocSize) ) ) { return false; }
			}
			CStd::MemCpy( &m_ptData[m_tLen], _ptValues, sizeof( _tType ) * _tTotal );
			m_tLen += _tTotal;
			return true;
		}

		/**
		 * Removes an element.
		 *
		 * \param _ui32Index The index of the item to remove.
		 */
		void LSE_CALL				Remove( uint32_t _ui32Index ) {
			static_cast<_tDerivedType *>(this)->RemoveRange( _ui32Index, 1 );
		}

		/**
		 * Removes an element without reallocating.
		 *
		 * \param _ui32Index The index of the item to remove.
		 */
		void LSE_CALL				RemoveNoDealloc( uint32_t _ui32Index ) {
			static_cast<_tDerivedType *>(this)->RemoveRangeNoDealloc( _ui32Index, 1 );
		}

		/**
		 * Removes elements.
		 *
		 * \param _ui32Index The start index of the items to be removed.
		 * \param _ui32Total The number of items to remove.
		 */
		void LSE_CALL				RemoveRange( uint32_t _ui32Index, uint32_t _ui32Total ) {
			static_cast<_tDerivedType *>(this)->RemoveRangeNoDealloc( _ui32Index, _ui32Total );

			// If there are fewer than half of the allocated space worth of objects,
			//	adjust the allocation.
			if ( m_tLen < (m_tAllocated >> 1) ) {
				static_cast<_tDerivedType *>(this)->Allocate( (m_tAllocated >> 1) );
			}
		}

		/**
		 * Removes elements without reallocating.
		 *
		 * \param _ui32Index The start index of the items to be removed.
		 * \param _ui32Total The number of items to remove.
		 */
		void LSE_CALL				RemoveRangeNoDealloc( uint32_t _ui32Index, uint32_t _ui32Total ) {
			assert( _ui32Index < m_tLen );
			assert( _ui32Index + _ui32Total <= m_tLen );
			uint32_t ui32End = _ui32Index + _ui32Total;
			for ( int32_t I = static_cast<int32_t>(ui32End); --I >= static_cast<int32_t>(_ui32Index); ) {
				// Destruct the item.
				Destroy( static_cast<_tDataType>(I) );
			}

			// Copy items over it.
			CStd::MemMove( &m_ptData[_ui32Index], &m_ptData[_ui32Index+_ui32Total], sizeof( _tType ) * ((m_tLen - _ui32Total) - _ui32Index) );
			m_tLen = m_tLen - static_cast<_tDataType>(_ui32Total);
		}

		/**
		 * Gets the number of elements in the array.
		 *
		 * \return Returns the number of items in the array.
		 */
		_tDataType LSE_CALL			Length() const {
			return m_tLen;
		}

		/**
		 * Gets the total number of elements allocated.
		 *
		 * \return Returns the number of items in the array.
		 */
		_tDataType LSE_CALL			Allocated() const {
			return m_tAllocated;
		}

		/**
		 * Snap it to the size of the list.  This removes allocated space beyond the actual length of the array.
		 */
		void LSE_CALL				Snap() {
			static_cast<_tDerivedType *>(this)->Allocate( m_tLen );
		}



	protected :
		// == Members.
		/**
		 * Array of data.
		 */
		_tType *					m_ptData;
		
		/**
		 * Length of the list.
		 */
		_tDataType					m_tLen;
		
		/**
		 * Amount allocated.
		 */
		_tDataType					m_tAllocated;


		// == Functions.
		/**
		 * Secret way of constructing items.
		 *
		 * \param _tIndex The index of the item to construct.
		 */
		void LSE_CALL				Construct( _tDataType _tIndex ) {
			new( &m_ptData[_tIndex] ) _tType;
		}
		
		/**
		 * Secret way of destroying items.  No error checking.
		 *
		 * \param _tIndex The index of the item to destruct.
		 */
		void LSE_CALL				Destroy( _tDataType _tIndex ) {
			// This gives warning C4100 when this class is created with types that have no destructor,
			//	claiming _tIndex is unreferenced.
			// Erase this warning with some do-nothing code.
#ifdef LSE_VISUALSTUDIO
			static_cast<_tDataType>(_tIndex);
#endif	// #ifdef LSE_VISUALSTUDIO
			m_ptData[_tIndex].~_tType();
		}

	};

}	// namespace lstl

#endif	// __LSTL_SVECTORCRTP_H__
