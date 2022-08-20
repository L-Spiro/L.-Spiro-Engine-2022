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
 * Description: A set with a fixed maximum number of elements.  No allocations are made during the lifetime of
 *	an object of this type, making it more suitable for areas of high performance with rapid creating and
 *	destructing sets.
 */


#ifndef __LSE_FIXEDSET_H__
#define __LSE_FIXEDSET_H__

#include "../LSTLib.h"
#include "../Algorithm/LSTLAlgorithm.h"

namespace lstl {

	/**
	 * Class CFixedSet
	 * \brief A set with a fixed maximum number of elements.
	 *
	 * Description: A set with a fixed maximum number of elements.  No allocations are made during the lifetime of
	 *	an object of this type, making it more suitable for areas of high performance with rapid creating and
	 *	destructing sets.
	 */
	template <typename _tKeyType, unsigned _uMaxItems>
	class CFixedSet {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CFixedSet() :
			m_ui32Total( 0 ) {
		}
		LSE_CALLCTOR							~CFixedSet() {
		}


		// == Operators.
		/**
		 * Copy another set into this one.  Each element in the source set is copied.  The allocator
		 *	is not copied, meaning the new CLinearMap does not use the same allocator as the original.
		 *
		 * \param _sSrc The set of which to make a copy.
		 * \return Returns a reference to this object after the copy.
		 */
		CFixedSet<_tKeyType, _uMaxItems> & LSE_CALL
												operator = ( const CFixedSet<_tKeyType, _uMaxItems> &_fsSrc ) {
			for ( uint32_t I = _fsSrc.m_ui32Total; I--; ) {
				m_ktArray[I] = _fsSrc.m_ktArray[I];
			}
			m_ui32Total = _fsSrc.m_ui32Total;
			return (*this);
		}


		// == Functions.
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
				if ( m_ui32Total == _uMaxItems ) { return false; }
				// Insert the item at the given index.
				for ( uint32_t I = m_ui32Total; I >= ui32Index + 1; --I ) {
					m_ktArray[I] = m_ktArray[I-1];
				}
				m_ktArray[ui32Index] = _tKey;
				++m_ui32Total;
			}
			return true;
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
				RemoveByIndex( ui32Index );
			}
		}

		/**
		 * Removes a pair at the given index.
		 *
		 * \param _ui32Index The index of the key to remove.
		 */
		void LSE_CALL							RemoveByIndex( uint32_t _ui32Index ) {
			if ( _ui32Index >= m_ui32Total ) { return; }
			--m_ui32Total;
			for ( uint32_t I = _ui32Index; I < m_ui32Total; ++I ) {
				m_ktArray[I] = m_ktArray[I+1];
			}
		}

		/**
		 * Reset all contents.
		 */
		void LSE_CALL							Reset() {
			m_ui32Total = 0;
		}

		/**
		 * Gets the total number of items.
		 *
		 * \return Returns the total pairs in the set.
		 */
		uint32_t LSE_CALL						Length() const {
			return m_ui32Total;
		}

		/**
		 * Gets a key for read-only by index.
		 *
		 * \param _ui32Index The index of the key to get, which must be valid.
		 * \return Returns a reference to the key at the given index inside this set.
		 */
		const _tKeyType & LSE_CALL				GetByIndex( uint32_t _ui32Index ) const {
			return m_ktArray[_ui32Index];
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
			return CAlgorithm::BSearch( m_ktArray, m_ui32Total, _tKey, _ui32Index );
		}


	protected :
		// == Members.
		/** Our internal array of paired items. */
		_tKeyType								m_ktArray[_uMaxItems];

		/** How many items in the set? */
		uint32_t								m_ui32Total;
	};

}	// namespace lstl

#endif	// __LSE_FIXEDSET_H__
