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
 * Description: A base for map templates.  This allows some maps to use static vectors and others to use
 *	standard vectors.
 */


#ifndef __LSTL_MAPTEMPLATEBASE_H__
#define __LSTL_MAPTEMPLATEBASE_H__

#include "../LSTLib.h"
#include "../Algorithm/LSTLAlgorithm.h"
#include "LSTLMapBase.h"

namespace lstl {

	// == Types.
	/**
	 * Our internal representation of the mapped values.  These are sorted by key.
	 */
	template <typename _tKeyType, typename _tMappedType>
	struct LSTL_PAIR {
		/**
		 * The key value.
		 */
		_tKeyType							tKey;

		/**
		 * The mapped value.
		 */
		_tMappedType						tValue;

		/**
		 * A copy operator.
		 *
		 * \param _pSrc The source object to copy.
		 * \return Returns the copied pair.
		 */
		LSTL_PAIR & LSE_CALL				operator = ( const LSTL_PAIR &_pSrc ) {
			tKey = _pSrc.tKey;
			tValue = _pSrc.tValue;
			return (*this);
		}

		/**
		 * Less-than operator.  Used to compare this pair with the key.
		 *
		 * \param PARM The key against which to compare this pair.
		 * \return Returns true if the key of this pair is less than the given key.
		 */
		LSBOOL LSE_CALL						operator < ( const _tKeyType &_tVal ) const {
			return tKey < _tVal;
		}

		/**
		 * Equality operator.  Used to compare this pair with the key.
		 *
		 * \param PARM The key against which to compare this pair.
		 * \return Returns true if the key of this pair is equal to the given key.
		 */
		LSBOOL LSE_CALL						operator == ( const _tKeyType &_tVal ) const {
			return tKey == _tVal;
		}

		/**
		 * Constructor.
		 */
		LSE_CALLCTOR						LSTL_PAIR() :
			tKey(),
			tValue() {
		}
	};

	/**
	 * Class CMapTemplateBase
	 * \brief A base for map templates.
	 *
	 * Description: A base for map templates.  This allows some maps to use static vectors and others to use
	 *	standard vectors.
	 */
	template <typename _tKeyType, typename _tMappedType, typename _tArrayType>
	class CMapTemplateBase : protected CMapBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMapTemplateBase() {
		}
		explicit LSE_CALLCTOR					CMapTemplateBase( const CMapTemplateBase<_tKeyType, _tMappedType, _tArrayType> &_mSrc ) :
			m_vPairs( _mSrc.m_vPairs ) {
		}
		LSE_CALLCTOR							~CMapTemplateBase() {
			Reset();
		}


		// == Operators.
		/**
		 * Access a mapped value by its key.  If the given key does not exist, it is added and its default
		 *	mapped value is returned.  Throws an exception if a memory failure prevents the adding of a key.
		 *
		 * \param _tKey The key of the value to find.
		 * \return Returns the mapped value associated with the given key.
		 */
		_tMappedType & LSE_CALL					operator [] ( const _tKeyType &_tKey ) {
			_tMappedType * ptValue = NULL;
			_tMappedType tDefault = _tMappedType();
			if ( !Insert( _tKey, tDefault, &ptValue ) ) {
				throw LSSTD_E_OUTOFMEMORY;
			}
			return (*ptValue);
		}

		/**
		 * Access a mapped value by its key.  If the given key does not exist, it is added and its default
		 *	mapped value is returned.  Throws an exception if a memory failure prevents the adding of a key.
		 *
		 * \param _tKey The key of the value to find.
		 * \return Returns the mapped value associated with the given key.
		 */
		const _tMappedType & LSE_CALL			operator [] ( const _tKeyType &_tKey ) const {
			uint32_t ui32Index;
			if ( !GetItemIndex( _tKey, ui32Index ) ) {
				// The const form of this function can only be used when values are
				//	known to exist in the list.
				throw LSSTD_E_INVALIDCALL;
			}
			return m_vPairs[ui32Index].tValue;
		}

		/**
		 * Copy another map into this one.  Each element in the source map is copied.  The allocator
		 *	is not copied, meaning the new CLinearMap does not use the same allocator as the original.
		 *
		 * \param _mSrc The map of which to make a copy.
		 * \return Returns a reference to this object after the copy.
		 */
		CMapTemplateBase<_tKeyType, _tMappedType, _tArrayType> & LSE_CALL
												operator = ( const CMapTemplateBase<_tKeyType, _tMappedType, _tArrayType> &_mSrc ) {
			// The copy is as easy as copying the vector.
			m_vPairs = _mSrc.m_vPairs;
			return (*this);
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
			return m_vPairs.Allocate( _ui32Total );
		}

		/**
		 * Insert an array of pairs of keys and mapped values.  Any keys that exist will not be modified.
		 *
		 * \param _ppPairs The pairs to insert.
		 * \param _ui32Total The total number of pairs to insert.
		 * \return Returns true if the items were added or already existed.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const _tArrayType * _ppPairs, uint32_t _ui32Total ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				if ( !Insert( _ppPairs[I] ) ) { return false; }
			}
			return true;
		}

		/**
		 * Insert a single pair of a key and its mapped value.  If the key exists, it is not modified.
		 *
		 * \param _pPair The pair to insert.
		 * \param _pptReturnValue The returned mapped value.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const LSTL_PAIR<_tKeyType, _tMappedType> &_pPair, _tMappedType ** _pptReturnValue = NULL ) {
			uint32_t ui32Index;
			if ( !GetItemIndex( _pPair.tKey, ui32Index ) ) {
				// Insert the item at the given index.
				if ( !m_vPairs.Insert( _pPair, ui32Index ) ) {
					if ( _pptReturnValue ) {
						(*_pptReturnValue) = NULL;
					}
					return false;
				}
			}
			if ( _pptReturnValue ) {
				(*_pptReturnValue) = &m_vPairs[ui32Index].tValue;
			}
			return true;
		}

		/**
		 * Insert a single pair of a key and its mapped value.  If the key exists, it is not modified.
		 *
		 * \param _tKey The pair's key.
		 * \param _tValue The pair's mapped value.
		 * \param _pptReturnValue The returned mapped value.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const _tKeyType &_tKey, const _tMappedType &_tValue, _tMappedType ** _pptReturnValue = NULL ) {
			LSTL_PAIR<_tKeyType, _tMappedType> pPair;
			pPair.tKey = _tKey;
			pPair.tValue = _tValue;
			return Insert( pPair, _pptReturnValue );
		}

		/**
		 * Unsafely inserts an item at a given index.  If the index is not the correct index for the item, the proper functioning of the map will be
		 *	lost.  Call GetItemIndex() or ItemExists() to get the proper index before calling this.
		 *
		 * \param _tKey The pair's key.
		 * \param _tValue The pair's mapped value.
		 * \param _ui32Index The index at which to insert the key/value pair.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							InsertUnsafe( const _tKeyType &_tKey, const _tMappedType &_tValue, uint32_t _ui32Index ) {
			// Insert the item at the given index.
			LSTL_PAIR<_tKeyType, _tMappedType> pPair;
			pPair.tKey = _tKey;
			pPair.tValue = _tValue;
			return m_vPairs.Insert( pPair, _ui32Index );
		}

		/**
		 * Removes an array of items without reallocating.
		 *
		 * \param _ptKeys The keys of the items to remove.
		 * \param _ui32Total The total number of pairs to remove.
		 */
		void LSE_CALL							RemoveNoDealloc( const _tKeyType * _ptKeys, uint32_t _ui32Total ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				RemoveNoDealloc( _ptKeys[I] );
			}
		}

		/**
		 * Removes a pair without reallocating.
		 *
		 * \param _tKey The key of the pair to remove.
		 */
		void LSE_CALL							RemoveNoDealloc( const _tKeyType &_tKey ) {
			uint32_t ui32Index;
			if ( GetItemIndex( _tKey, ui32Index ) ) {
				m_vPairs.RemoveNoDealloc( ui32Index );
			}
		}

		/**
		 * Removes an array of items.
		 *
		 * \param _ptKeys The keys of the items to remove.
		 * \param _ui32Total The total number of pairs to remove.
		 */
		void LSE_CALL							Remove( const _tKeyType * _ptKeys, uint32_t _ui32Total ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				Remove( _ptKeys[I] );
			}
		}

		/**
		 * Removes a pair.
		 *
		 * \param _tKey The key of the pair to remove.
		 */
		void LSE_CALL							Remove( const _tKeyType &_tKey ) {
			uint32_t ui32Index;
			if ( GetItemIndex( _tKey, ui32Index ) ) {
				m_vPairs.Remove( ui32Index );
			}
		}

		/**
		 * Removes by index.
		 *
		 * \param _ui32Index Index of the pair to remove.
		 */
		void LSE_CALL							RemoveByIndex( uint32_t _ui32Index ) {
			m_vPairs.Remove( _ui32Index );
		}

		/**
		 * Removes by index without deallocating.
		 *
		 * \param _ui32Index Index of the pair to remove.
		 */
		void LSE_CALL							RemoveByIndexNoDealloc( uint32_t _ui32Index ) {
			m_vPairs.RemoveNoDealloc( _ui32Index );
		}

		/**
		 * Reset without reallocating.
		 */
		void LSE_CALL							ResetNoDealloc() {
			m_vPairs.ResetNoDealloc();
		}

		/**
		 * Reset all contents.
		 */
		void LSE_CALL							Reset() {
			m_vPairs.Reset();
		}

		/**
		 * Gets the total number of items.
		 *
		 * \return Returns the total pairs in the map.
		 */
		uint32_t LSE_CALL						Length() const {
			return m_vPairs.Length();
		}

		/**
		 * Gets a mapped value by index.  Values are always sorted by their respective keys.
		 *
		 * \param _ui32Index The index of the mapped value to get, which must be valid.
		 * \return Returns a reference to the mapped value at the given index inside this map.
		 */
		_tMappedType & LSE_CALL					GetByIndex( uint32_t _ui32Index ) {
			return m_vPairs[_ui32Index].tValue;
		}

		/**
		 * Gets a mapped value by index.  Values are always sorted by their respective keys.
		 *
		 * \param _ui32Index The index of the mapped value to get, which must be valid.
		 * \return Returns a reference to the mapped value at the given index inside this map.
		 */
		const _tMappedType & LSE_CALL			GetByIndex( uint32_t _ui32Index ) const {
			return m_vPairs[_ui32Index].tValue;
		}

		/**
		 * Gets a key value by index.
		 *
		 * \param _ui32Index The index of the key value to get, which must be valid.
		 * \return Returns a reference to the key value at the given index inside this map.
		 */
		_tKeyType & LSE_CALL					GetKeyByIndex( uint32_t _ui32Index ) {
			return m_vPairs[_ui32Index].tKey;
		}

		/**
		 * Gets a key value by index.
		 *
		 * \param _ui32Index The index of the key value to get, which must be valid.
		 * \return Returns a reference to the key value at the given index inside this map.
		 */
		const _tKeyType & LSE_CALL				GetKeyByIndex( uint32_t _ui32Index ) const {
			return m_vPairs[_ui32Index].tKey;
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
			if ( !m_vPairs.Length() ) {
				_ui32Index = 0;
				return false;
			}
			return CAlgorithm::BSearch( &m_vPairs[0], m_vPairs.Length(), _tKey, _ui32Index );
		}

		/**
		 * Determines whether the given key already exists in the map or not.
		 *
		 * \param _tKey The key to test for existance in the map.
		 * \return Returns true if the item key is already in the map, false otherwise.
		 */
		LSBOOL LSE_CALL							ItemExists( const _tKeyType &_tKey ) const {
			uint32_t ui32Index;
			return GetItemIndex( _tKey, ui32Index );
		}

		/**
		 * Determines whether the given key already exists in the map or not and, if so, returns the
		 *	index of the item.
		 *
		 * \param _tKey The key to test for existance in the map.
		 * \param _ui32Index The index of the item in the map, if it exists.
		 * \return Returns true if the item key is already in the map, false otherwise.
		 */
		LSBOOL LSE_CALL							ItemExists( const _tKeyType &_tKey, uint32_t &_ui32Index ) const {
			return GetItemIndex( _tKey, _ui32Index );
		}


	protected :
		// == Various Constructors.
		explicit LSE_CALLCTOR					CMapTemplateBase( const _tArrayType &_pSrc ) :
			m_vPairs( _pSrc ) {
		}


		// == Members.
		/**
		 * Our internal array of paired items.
		 */
		_tArrayType								m_vPairs;
	};

}	// namespace lstl

#endif	// __LSTL_MAPTEMPLATEBASE_H__
