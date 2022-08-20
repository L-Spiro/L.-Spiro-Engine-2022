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
 * Description: An associative container in which multiple values may share the same keys.  This similar to
 *	CLinearMap, except that a CLinearMap allows only one key/value pair.  A multimap supports key/[value/value/value/...]
 *	associations.
 *
 * Because keys are sorted, the key type must implement the == and < operators.
 */


#ifndef __LSTL_MULTIMAP_H__
#define __LSTL_MULTIMAP_H__

#include "../LSTLib.h"
#include "../Algorithm/LSTLAlgorithm.h"
#include "../Vector/LSTLVector.h"
#include "LSTLMultiMapBase.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lstl {

	/**
	 * Class CMultiMap
	 * \brief A container allowing multiple values to be associated with a single key.
	 *
	 * Description: An associative container in which multiple values may share the same keys.  This similar to
	 *	CLinearMap, except that a CLinearMap allows only one key/value pair.  A multimap supports key/[value/value/value/...]
	 *	associations.
	 *
	 * Because keys are sorted, the key type must implement the == and < operators.
	 */
	template <typename _tKeyType, typename _tMappedType>
	class CMultiMap : protected CMultiMapBase {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMultiMap() {
		}
		explicit LSE_CALLCTOR					CMultiMap( const CMultiMap<_tKeyType, _tMappedType> &_mmSrc ) :
			m_vPairs( _mmSrc.m_vPairs ) {
		}
		explicit LSE_CALLCTOR					CMultiMap( CAllocator * _paAllocator ) :
			m_vPairs( _paAllocator ) {
		}
		LSE_CALLCTOR							~CMultiMap() {
		}


		// == Operators.
		/**
		 * Copy operator.
		 *
		 * \param _mmOther The object to copy.
		 * \return Returns this object after the copy.
		 */
		CMultiMap & LSE_CALL					operator = ( const CMultiMap &_mmOther ) {
			m_vPairs = _mmOther.m_vPairs;

			return (*this);
		}


		// == Functions.
		/**
		 * Insert a single pair of a key and its mapped value.  If the given key does not exist, it is added to
		 *	the multimap.  If it does exist, the mapped value is added to its associations.
		 *
		 * \param _tKey The pair's key.
		 * \param _tValue The pair's mapped value.
		 * \param _pptReturnValue The returned mapped value.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const _tKeyType &_tKey, const _tMappedType &_tValue, _tMappedType ** _pptReturnValue = NULL ) {
			uint32_t ui32Index;
			if ( !GetItemIndex( _tKey, ui32Index ) ) {
				// Pair does not exist, so add it.
				LSTL_PAIR pPair;
				pPair.tKey = _tKey;
				// Do not add the value.
				if ( !m_vPairs.Insert( pPair, ui32Index ) ) {
					if ( _pptReturnValue ) {
						(*_pptReturnValue) = NULL;
					}
					return false;
				}
				// It was inserted.  Apply the allocator to the vector.
				m_vPairs[ui32Index].vValues.SetAllocator( GetAllocator() );
			}

			// Key already exists.  Add the mapped value to it.
			uint32_t ui32Total = m_vPairs[ui32Index].vValues.Length();
			if ( !m_vPairs[ui32Index].vValues.Push( _tValue ) ) {
				if ( _pptReturnValue ) {
					(*_pptReturnValue) = NULL;
				}
				return false;
			}
			if ( _pptReturnValue ) {
				(*_pptReturnValue) = &m_vPairs[ui32Index].vValues[ui32Total];
			}
			return true;
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
		 * Gets the total number of unique keys.
		 *
		 * \return Returns the total unique keys in the multimap.
		 */
		uint32_t LSE_CALL						Length() const {
			return m_vPairs.Length();
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
			return CAlgorithm::BSearch( m_vPairs, _tKey, _ui32Index );
		}

		/**
		 * Gets the number of mapped values associated with the key at the given index.
		 *
		 * \param _ui32KeyIndex The index of the key whose total mapped values is to be found.
		 * \return Returns the total mapped values associated with the key at the given index.
		 */
		uint32_t LSE_CALL						ItemsOnKey( uint32_t _ui32Index ) const {
			return m_vPairs[_ui32Index].vValues.Length();
		}

		/**
		 * Gets a key by index.
		 *
		 * \param _ui32KeyIndex The index of the key containing the mapped value to get.  Index must be less than Length().
		 * \return Returns a constant reference to the key at the given index.
		 */
		const _tKeyType & LSE_CALL				GetKeyByIndex( uint32_t _ui32KeyIndex ) const {
			return m_vPairs[_ui32KeyIndex].tKey;
		}

		/**
		 * Gets a mapped value by index.  _ui32KeyIndex is the index of the key containing the
		 *	mapped value to get.
		 *
		 * \param _ui32KeyIndex The index of the key containing the mapped value to get.  Index must be less than Length().
		 * \param _ui32Index The index of the mapped value to get.  Must be less than ItemsOnKey( _ui32KeyIndex ).
		 * \return Returns a reference to the mapped value at the given index inside this map.
		 */
		_tMappedType & LSE_CALL					GetByIndex( uint32_t _ui32KeyIndex, uint32_t _ui32Index ) {
			return m_vPairs[_ui32KeyIndex].vValues[_ui32Index];
		}

		/**
		 * Gets a mapped value by index.  _ui32KeyIndex is the index of the key containing the
		 *	mapped value to get.
		 *
		 * \param _ui32KeyIndex The index of the key containing the mapped value to get.  Index must be less than Length().
		 * \param _ui32Index The index of the mapped value to get.  Must be less than ItemsOnKey( _ui32KeyIndex ).
		 * \return Returns a reference to the mapped value at the given index inside this map.
		 */
		const _tMappedType & LSE_CALL			GetByIndex( uint32_t _ui32KeyIndex, uint32_t _ui32Index ) const {
			return m_vPairs[_ui32KeyIndex].vValues[_ui32Index];
		}

		/**
		 * Sets the allocator.  Causes the object to be fully reset.  Should not be used on
		 *	lists that have contents already.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator for map objects.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			m_vPairs.SetAllocator( _paAllocator );
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns a pointer to the allocator used by this object.
		 */
		CAllocator * LSE_CALL					GetAllocator() {
			return m_vPairs.GetAllocator();
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns a pointer to the allocator used by this object.
		 */
		const CAllocator * LSE_CALL				GetAllocator() const {
			return m_vPairs.GetAllocator();
		}


	protected :
		// == Types.
		/**
		 * Our internal representation of the mapped values.  These are sorted by key.
		 */
		typedef struct LSTL_PAIR {
			/**
			 * The key value.
			 */
			_tKeyType							tKey;

			/**
			 * The mapped value.
			 */
			CVector<_tMappedType, uint16_t, 1024>
												vValues;

			/**
			 * A copy operator.
			 *
			 * \param _pSrc The source object to copy.
			 * \return Returns the copied pair.
			 */
			LSTL_PAIR & LSE_CALL				operator = ( const LSTL_PAIR &_pSrc ) {
				tKey = _pSrc.tKey;
				vValues = _pSrc.vValues;
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
				vValues() {
			}
		} * LPLSTL_PAIR, * const LPCLSTL_PAIR;

		
		// == Members.
		/**
		 * Our list of keys and their associated (possibly multiple) values.
		 */
		CVector<LSTL_PAIR, uint32_t, 1024>	m_vPairs;

	};

}	// namespace lstl

#pragma warning( pop )

#endif	// __LSTL_MULTIMAP_H__
