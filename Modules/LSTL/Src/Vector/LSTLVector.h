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
 * Description: Extends the CSVectorCrtp class to mke it work properly with types that are not plain data.
 *	Use this for types that have a complicated destructor, copy constructor, and copy assignment operator.
 */


#ifndef __LSTL_VECTOR_H__
#define __LSTL_VECTOR_H__

#include "../LSTLib.h"
#include "LSTLSVectorCrtp.h"

namespace lstl {

	/**
	 * Class CVector
	 * \brief A vector class that works on all types that follow the rule of 3.
	 *
	 * Description: Extends the CSVectorCrtp class to mke it work properly with types that are not plain data.
	 *	Use this for types that have a complicated destructor, copy constructor, and copy assignment operator.
	 */
	template <typename _tType, typename _tDataType = uint16_t, unsigned _uAllocSize = 512>
	class CVector : public CSVectorCrtp<CVector<_tType, _tDataType, _uAllocSize>, _tType, _tDataType, _uAllocSize> {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVector() :
			m_paOurAllocator( Parent::m_paDefaultAllocator ) {
		}
		explicit LSE_CALLCTOR					CVector( uint32_t _tTotal ) :
			m_paOurAllocator( Parent::m_paDefaultAllocator ) {
			Allocate( _tTotal );
		}
		explicit LSE_CALLCTOR					CVector( CAllocator * _paAllocator ) :
			m_paOurAllocator( _paAllocator ) {
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
		}
		LSE_CALLCTOR							CVector( uint32_t _tTotal, CAllocator * _paAllocator ) :
			m_paOurAllocator( _paAllocator ) {
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
			Allocate( _tTotal );
		}
		LSE_CALLCTOR							CVector( const CVector<_tType, _tDataType, _uAllocSize> &_vOther, CAllocator * _paAllocator = NULL ) :
			m_paOurAllocator( _paAllocator ) {
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
			(*this) = _vOther;
		}
		LSE_CALLCTOR							~CVector() {
			Reset();
		}


		// == Functions.
		/**
		 * Allocates a given number of elements.
		 * If the allocation is less than what there already is, items are removed.
		 *
		 * \param _ui32Total Number of elements to allocate.
		 * \return Returns true if there was enough memory to allocate the given amount of
		 *	objects.  If _ui32Total is 0, true is always returned.
		 */
		LSBOOL LSE_CALL							Allocate( uint32_t _ui32Total ) {
			if ( Parent::m_tAllocated == static_cast<_tDataType>(_ui32Total) ) {
				return true;
			}

			// If allocating 0 bytes, just reset the list.
			if ( !_ui32Total ) {
				Reset();
				return true;
			}

			// Destroy items that are going to be removed.
			if ( Parent::m_tLen ) {
				for ( uint32_t I = Parent::m_tLen; --I >= _ui32Total; ) {
					Parent::Destroy( static_cast<_tDataType>(I) );
				}
			}
			// Adjust the length.
			if ( Parent::m_tLen > static_cast<_tDataType>(_ui32Total) ) {
				Parent::m_tLen = static_cast<_tDataType>(_ui32Total);
			}

			// Attempt to allocate.
			_tType * ptNew = reinterpret_cast<_tType *>(m_paOurAllocator->Alloc( _ui32Total * sizeof( _tType ) ));
			if ( !ptNew ) { return false; }

			// Construct and copy all the items in the newly allocated array.
			for ( uint32_t I = Parent::m_tLen; I--; ) {
				// Construct new.
				new( &ptNew[I] ) _tType( Parent::m_ptData[I] );
				/*new( &ptNew[I] ) _tType;
				ptNew[I] = Parent::m_ptData[I];*/
				// Destroy old.
				Parent::Destroy( static_cast<_tDataType>(I) );
			}
			// Remove the old list.
			if ( Parent::m_ptData ) {
				m_paOurAllocator->Free( Parent::m_ptData );
			}

			// Success.
			Parent::m_ptData = ptNew;
			Parent::m_tAllocated = static_cast<_tDataType>(_ui32Total);
			return true;
		}

		/**
		 * Reset the list completely.
		 */
		void LSE_CALL							Reset() {
			for ( uint32_t I = Parent::m_tLen; I--; ) {
				Parent::Destroy( static_cast<_tDataType>(I) );
			}
			if ( Parent::m_ptData ) {
				m_paOurAllocator->Free( Parent::m_ptData );
				Parent::m_ptData = NULL;
			}
			Parent::m_tLen = Parent::m_tAllocated = 0;
		}

		/**
		 * Insert an element.
		 *
		 * \param _tVal The item to insert.
		 * \param _ui32Index The index where the item is to be inserted.
		 * \return Returns false if memory could not be allocated.  In this case, the list is not modified.
		 */
		LSBOOL LSE_CALL							Insert( const _tType &_tVal, uint32_t _ui32Index ) {
			assert( _ui32Index <= Parent::m_tLen );
			// If inserting at the end, just push the item.
			if ( _ui32Index == Parent::m_tLen ) { return this->Push( _tVal ); }

			// Now we know we are inserting in the middle somewhere.
			if ( Parent::m_tLen == Parent::m_tAllocated ) {
				// Overflow checking.
				_tDataType tNewTotal = Parent::m_tAllocated + _uAllocSize;
				assert( tNewTotal > Parent::m_tAllocated );
				if ( !Allocate( tNewTotal ) ) { return false; }
			}

			// Move other items.
			// Since this is not a PoD handler, we cannot simply move memory.
			// The last item has not been constructed yet, so we cannot call its copy operator yet.
			this->Construct( Parent::m_tLen );
			// Move items up one-by-one.
			_tType * ptLow = &Parent::m_ptData[_ui32Index];
			LSE_PREFETCH_LINE( ptLow );
			_tType * ptDst = &Parent::m_ptData[Parent::m_tLen];
			_tType * ptSrc = ptDst - 1;
			while ( ptDst > ptLow ) {
				(*ptDst--) = (*ptSrc--);
			}
			// No need to destroy/construct the item at the given location.  Its copy operator should handle freeing of
			//	its memory.
			(*ptLow) = _tVal;
			++Parent::m_tLen;
			return true;
		}

		/**
		 * Append an array of values to the end of the vector.
		 *
		 * \param _ptValues The values to append.
		 * \param _tTotal The number of values to append.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							Append( const _tType * _ptValues, _tDataType _tTotal ) {
			uintptr_t uiptrNewLen = Parent::m_tLen + _tTotal;
			if ( uiptrNewLen >= Parent::m_tAllocated ) {
				if ( !Allocate( static_cast<_tDataType>(uiptrNewLen + _uAllocSize) ) ) { return false; }
			}
			for ( uintptr_t I = Parent::m_tLen; I < uiptrNewLen; ++I ) {
				new( &Parent::m_ptData[I] ) _tType( (*_ptValues++) );
			}
			Parent::m_tLen += _tTotal;
			return true;
		}

		/**
		 * Removes elements without reallocating.
		 *
		 * \param _ui32Index The start index of the items to be removed.
		 * \param _ui32Total The number of items to remove.
		 */
		void LSE_CALL							RemoveRangeNoDealloc( uint32_t _ui32Index, uint32_t _ui32Total ) {
			if ( _ui32Total == 0 ) { return; }
			assert( _ui32Index < Parent::m_tLen );
			assert( _ui32Index + _ui32Total <= Parent::m_tLen );
			uint32_t ui32End = Parent::m_tLen - _ui32Total;
			
			// Copy items over it.
			// Since this is not a PoD handler, we cannot simply move memory.
			uint32_t ui32CopyEnd = Parent::m_tLen - _ui32Total;
			for ( uint32_t I = _ui32Index; I < ui32CopyEnd; ++I ) {
				Parent::m_ptData[I] = Parent::m_ptData[I+_ui32Total];
			}

			// Destroy the tail items that were moved down.
			for ( int32_t I = static_cast<int32_t>(Parent::m_tLen); --I >= static_cast<int32_t>(ui32End); ) {
				// Destruct the item.
				Parent::Destroy( static_cast<_tDataType>(I) );
			}
			Parent::m_tLen = Parent::m_tLen - static_cast<_tDataType>(_ui32Total);
		}

		// Set the allocator.  Causes the object to be fully reset.  Cannot be used on
		//	lists that have contents already.
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			Reset();
			m_paOurAllocator = _paAllocator;
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
		}

		// Get our allocator.
		CAllocator * LSE_CALL					GetAllocator() {
			return m_paOurAllocator;
		}

		/**
		 * Trash the list without freeing the memory.  Calls .Trash() on objects and resets
		 *	without freeing any memory.  The memory is assumed to be free by another part of the
		 *	application.  For example, if the memory was allocated to a heap and the heap is to
		 *	be freed at all once.  After calling Trash(), the whole heap could be removed in full
		 *	without creating a memory leak.
		 * After calling .Trash() on each object, the object's destructor is called.  This can only
		 *	be called if the objects in the list have a .Trash() method.
		 */
		void LSE_CALL							Trash() {
			for ( _tDataType I = Parent::m_tLen; I--; ) {
				Parent::m_ptData[I].Trash();
				Parent::Destroy( I );
			}
			Parent::m_ptData = NULL;
			Parent::m_tLen = Parent::m_tAllocated = 0;
		}

	protected :
		// == Members.
		// The allocator we should use.
		CAllocator *							m_paOurAllocator;

	private :
		typedef CSVectorCrtp<CVector<_tType, _tDataType, _uAllocSize>, _tType, _tDataType, _uAllocSize>
			Parent;
	};

}	// namespace lstl

#endif	// __LSTL_VECTOR_H__
