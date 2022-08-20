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
 * Description: An associate container such that each element is the key itself.  CLinearMap's associate keys with
 *	values, whereas CSet simply keeps a list of keys.  Like with CLinearMap, each key is unique in the list, and
 *	sorted by key value.
 * Because keys are sorted, the key type must implement the == and < operators.
 */


#ifndef __LSTL_SET_H__
#define __LSTL_SET_H__

#include "../LSTLib.h"
#include "../Algorithm/LSTLAlgorithm.h"
#include "../Vector/LSTLVector.h"
#include "LSTLSetBase.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lstl {

	/**
	 * Class CSet
	 * \brief An associative container storing one unique instance of each given key.
	 *
	 * Description: Description: An associate container such that each element is the key itself.  CLinearMap's associate keys with
	 *	values, whereas CSet simply keeps a list of keys.  Like with CLinearMap, each key is unique in the list, and
	 *	sorted by key value.
	 * Because keys are sorted, the key type must implement the == and < operators.
	 */
	template <typename _tKeyType>
	class CSet : protected CSetBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSet() {
		}
		explicit LSE_CALLCTOR					CSet( const CSet<_tKeyType> &_sSrc ) :
			m_vKeys( _sSrc.m_vKeys ) {
		}
		explicit LSE_CALLCTOR					CSet( CAllocator * _paAllocator ) :
			m_vKeys( _paAllocator ) {
		}
		LSE_CALLCTOR							~CSet() {
		}


		// == Operators.
		/**
		 * Copy another set into this one.  Each element in the source set is copied.  The allocator
		 *	is not copied, meaning the new CLinearMap does not use the same allocator as the original.
		 *
		 * \param _sSrc The set of which to make a copy.
		 * \return Returns a reference to this object after the copy.
		 */
		CSet<_tKeyType> & LSE_CALL				operator = ( const CSet<_tKeyType> &_sSrc ) {
			// The copy is as easy as copying the vector.
			m_vKeys = _sSrc.m_vKeys;
			return (*this);
		}


		// == Functions.
		/**
		 * Allocates room for at least the given amount of items.  If the allocation size is already
		 *	larger than the given amount, nothing is done.
		 *
		 * \param _ui32Size The minimum number of items to allocate.
		 * \return Returns true if the allocation was successful.
		 */
		LSBOOL LSE_CALL							AllocateAtLeast( uint32_t _ui32Size ) {
			if ( m_vKeys.GetAllocated() >= _ui32Size ) { return true; }
			return m_vKeys.Allocate( _ui32Size );
		}

		/**
		 * Insert an array of keys.
		 *
		 * \param _ptKeys The keys to insert.
		 * \param _ui32Total The total number of keys to insert.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const _tKeyType * _ptKeys, uint32_t _ui32Total ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				if ( !Insert( _ptKeys[I] ) ) { return false; }
			}
			return true;
		}

		/**
		 * Insert a single key.
		 *
		 * \param _tKey The key to insert.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const _tKeyType &_tKey ) {
			uint32_t ui32Index;
			if ( !GetItemIndex( _tKey, ui32Index ) ) {
				// Insert the item at the given index.
				if ( !m_vKeys.Insert( _tKey, ui32Index ) ) {
					return false;
				}
			}
			return true;
		}

		/**
		 * Removes an array of items without reallocating.
		 *
		 * \param _ptKeys The keys to remove.
		 * \param _ui32Total The total number of keys to remove.
		 */
		void LSE_CALL							RemoveNoDealloc( const _tKeyType * _ptKeys, uint32_t _ui32Total ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				RemoveNoDealloc( _ptKeys[I] );
			}
		}

		/**
		 * Removes a key without reallocating.
		 *
		 * \param _tKey The key to remove.
		 */
		void LSE_CALL							RemoveNoDealloc( const _tKeyType &_tKey ) {
			uint32_t ui32Index;
			if ( GetItemIndex( _tKey, ui32Index ) ) {
				m_vKeys.RemoveNoDealloc( ui32Index );
			}
		}

		/**
		 * Removes an array of items.
		 *
		 * \param _ptKeys The keys to remove.
		 * \param _ui32Total The total number of keys to remove.
		 */
		void LSE_CALL							Remove( const _tKeyType * _ptKeys, uint32_t _ui32Total ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				Remove( _ptKeys[I] );
			}
		}

		/**
		 * Removes a pair.
		 *
		 * \param _tKey The key to remove.
		 */
		void LSE_CALL							Remove( const _tKeyType &_tKey ) {
			uint32_t ui32Index;
			if ( GetItemIndex( _tKey, ui32Index ) ) {
				m_vKeys.Remove( ui32Index );
			}
		}

		/**
		 * Removes a pair at the given index without reallocating.
		 *
		 * \param _ui32Index The index of the key to remove.
		 */
		void LSE_CALL							RemoveByIndexNoDealloc( uint32_t _ui32Index ) {
			m_vKeys.RemoveNoDealloc( _ui32Index );
		}

		/**
		 * Removes a pair at the given index.
		 *
		 * \param _ui32Index The index of the key to remove.
		 */
		void LSE_CALL							RemoveByIndex( uint32_t _ui32Index ) {
			m_vKeys.Remove( _ui32Index );
		}

		/**
		 * Reset without reallocating.
		 */
		void LSE_CALL							ResetNoDealloc() {
			m_vKeys.ResetNoDealloc();
		}

		/**
		 * Reset all contents.
		 */
		void LSE_CALL							Reset() {
			m_vKeys.Reset();
		}

		/**
		 * Gets the total number of items.
		 *
		 * \return Returns the total pairs in the set.
		 */
		uint32_t LSE_CALL						Length() const {
			return m_vKeys.Length();
		}

		/**
		 * Gets a key for read-only by index.
		 *
		 * \param _ui32Index The index of the key to get, which must be valid.
		 * \return Returns a reference to the key at the given index inside this set.
		 */
		const _tKeyType & LSE_CALL				GetByIndex( uint32_t _ui32Index ) const {
			return m_vKeys[_ui32Index];
		}

		/**
		 * Sets the allocator.  Causes the object to be fully reset.  Should not be used on
		 *	lists that have contents already.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator for set objects.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			m_vKeys.SetAllocator( _paAllocator );
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns a pointer to the allocator used by this object.
		 */
		CAllocator * LSE_CALL					GetAllocator() {
			return m_vKeys.GetAllocator();
		}

		/**
		 * Determine if an item is in the list or not.
		 *
		 * \param _tKey Key of the item to find.
		 * \return Returns true if the item is in the list.
		 */
		LSBOOL LSE_CALL							ItemExists( const _tKeyType &_tKey ) const {
			uint32_t ui32Index;
			return GetItemIndex( _tKey, ui32Index );
		}

		/**
		 * Gets the index of an existing item.  If the item does not exist, the index indicates
		 *	where it should be if it did exist.
		 *
		 * \param _tKey Key of the item to find.
		 * \param _ui32Index The returned index of the found item or the index where the item should be inserted.
		 * \return Returns true if the item exists, false otherwise.
		 */
		LSBOOL LSE_CALL							GetItemIndex( const _tKeyType &_tKey, uint32_t &_ui32Index ) const {
			return CAlgorithm::BSearch( m_vKeys, _tKey, _ui32Index );
		}

	protected :
		// == Members.
		/** Our internal array of paired items. */
		CVector<_tKeyType, uint32_t>			m_vKeys;

	};

}	// namespace lstl

#pragma warning( pop )

#endif	// __LSTL_SET_H__
