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
 * Description: An associative container pairing a unique key with a mapped value.  The key and value do not
 *	need to be of the same type.
 *
 * Values are accessed via their keys, so each key is unique.  Mapped values do not need to be unique.  Mapped
 *	values are sorted, and the key type must implement the == and < operators.  This does not offer direct
 *	iteration over the mapped values.  Instead, to iterate over them, call CreateVector() to get a vector of
 *	pointers to each item in the map.  The pointers will be valid as long as the map is not modified.
 * Like with std::map, items in the map are guaranteed to maintain the same address.
 */


#ifndef __LSTL_MAP_H__
#define __LSTL_MAP_H__

#include "../LSTLib.h"
#include "../Allocator/LSTLAllocator.h"
#include "../Vector/LSTLVector.h"
#include "Allocators/LSAFixedExternAllocator.h"
#include "LSTLMapTemplateBase.h"

namespace lstl {

	/**
	 * Class CMap
	 * \brief A container associating uniqe keys with mapped values.
	 *
	 * Description: Description: An associative container pairing a unique key with a mapped value.  The key and value do not
	 *	need to be of the same type.
	 *
	 * Values are accessed via their keys, so each key is unique.  Mapped values do not need to be unique.  Mapped
	 *	values are sorted, and the key type must implement the == and < operators.  This does not offer direct
	 *	iteration over the mapped values.  Instead, to iterate over them, call CreateVector() to get a vector of
	 *	pointers to each item in the map.  The pointers will be valid as long as the map is not modified.
	 * Like with std::map, items in the map are guaranteed to maintain the same address.
	 */
	template <typename _tKeyType, typename _tMappedType, unsigned _uSize = 4096>
	class CMap {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMap() :
			m_paAllocator( NULL ),
			m_pnRoot( NULL ),
			m_pbBuffers( NULL ),
			m_uiptrTotal( 0 ),
			m_ui32TotalBuffers( 0 ) {
			m_nNull.rbColor = LSTL_RB_RED;
			m_nNull.pnParent = m_nNull.pnLeft = m_nNull.pnRight = NULL;//&m_nNull;
		}
		explicit LSE_CALLCTOR					CMap( const CMap<_tKeyType, _tMappedType, _uSize> &_mSrc ) :
			m_paAllocator( NULL ),
			m_pnRoot( NULL ),
			m_pbBuffers( NULL ),
			m_uiptrTotal( 0 ),
			m_ui32TotalBuffers( 0 ) {
			m_nNull.rbColor = LSTL_RB_RED;
			m_nNull.pnParent = m_nNull.pnLeft = m_nNull.pnRight = NULL;//&m_nNull;
		}
		explicit LSE_CALLCTOR					CMap( CAllocator * _paAllocator ) :
			m_paAllocator( NULL ),
			m_pnRoot( NULL ),
			m_pbBuffers( NULL ),
			m_uiptrTotal( 0 ),
			m_ui32TotalBuffers( 0 ) {
			m_nNull.rbColor = LSTL_RB_RED;
			m_nNull.pnParent = m_nNull.pnLeft = m_nNull.pnRight = NULL;//&m_nNull;
		}
		LSE_CALLCTOR							~CMap() {
			Reset();
		}


		// == Functions.
		/**
		 * Allocates the given number of elements.  If the number of elements is less than or equal to
		 *	the currently allocated number of elements nothing happens.
		 *
		 * \param _ui32Elements Total number of elements to allocate.
		 * \return Returns false if there was an error allocating the elements.
		 */
		LSBOOL LSE_CALL							Allocate( uint32_t _ui32Elements ) {
			// How many will there be?
			uint32_t ui32NewTotal = (_ui32Elements + (_uSize - 1)) / _uSize;
			if ( m_ui32TotalBuffers >= ui32NewTotal ) { return true; }

			// Allocate the new array.
			LSTL_BUFFER * pbNew = reinterpret_cast<LSTL_BUFFER *>(Alloc( ui32NewTotal * sizeof( LSTL_BUFFER ) ));
			if ( !pbNew ) { return false; }

			// Construct them.
			for ( uint32_t I = ui32NewTotal; I--; ) {
				new( &pbNew[I] ) LSTL_BUFFER();
			}
			// Allocate the new buffers.
			for ( uint32_t I = m_ui32TotalBuffers; I < ui32NewTotal; ++I ) {
				pbNew[I].pnNode = reinterpret_cast<LSTL_NODE *>(Alloc( _uSize * sizeof( pbNew[I].pnNode[0] ) ));
				if ( !pbNew[I].pnNode ) {
					// Falied to allocate.
					// Deallocate everything we did in this loop, deallocate pbNew, and return false.
					for ( uint32_t J = ui32NewTotal; J--; ) {
						Free( reinterpret_cast<uint8_t *&>(pbNew[J].pnNode) );
						pbNew[J].~LSTL_BUFFER();
					}
					Free( reinterpret_cast<uint8_t *&>(pbNew) );
					return false;
				}
			}

			// Failure is impossible after this.
			// Copy all the old over.
			for ( uint32_t I = m_ui32TotalBuffers; I--; ) {
				pbNew[I].feaAllocator.Swap( m_pbBuffers[I].feaAllocator );
				CStd::Swap( pbNew[I].pnNode, m_pbBuffers[I].pnNode );
			}
			// Make the new feaAllocator's point to the buffers created for them.
			for ( uint32_t I = m_ui32TotalBuffers; I < ui32NewTotal; ++I ) {
				pbNew[I].feaAllocator.SetPointer( pbNew[I].pnNode );
			}
			// Delete our buffers.
			Free( reinterpret_cast<uint8_t *&>(m_pbBuffers) );
			// Copy members.
			m_pbBuffers = pbNew;
			m_ui32TotalBuffers = ui32NewTotal;
			return true;
		}

		/**
		 * Resets the map fully back to scratch.
		 */
		void LSE_CALL							Reset() {
			for ( uint32_t I = m_ui32TotalBuffers; I--; ) {
				Free( reinterpret_cast<uint8_t *&>(m_pbBuffers[I].pnNode) );
				m_pbBuffers[I].~LSTL_BUFFER();
			}
			Free( reinterpret_cast<uint8_t *&>(m_pbBuffers) );
			m_uiptrTotal = 0;
			m_ui32TotalBuffers = 0;
			m_pnRoot = NULL;
			m_nNull.rbColor = LSTL_RB_RED;
			m_nNull.pnParent = m_nNull.pnLeft = m_nNull.pnRight = NULL;//&m_nNull;
		}

		/**
		 * Determines whether the given key already exists in the map or not and, if so, returns the
		 *	index of the item.
		 *
		 * \param _tKey The key to test for existance in the map.
		 * \return Returns true if the item key is already in the map, false otherwise.
		 */
		LSBOOL LSE_FCALL						ItemExists( const _tKeyType &_tKey ) const {
			LSTL_NODE * pnParent;
			LSTL_NODE ** pnChild;
			return FindPair( _tKey, pnParent, pnChild );
		}

		/**
		 * Insert a single pair of a key and its mapped value.  If the key exists, it is not modified.
		 *
		 * \param _tKey The pair's key.
		 * \param _tValue The pair's mapped value.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const _tKeyType &_tKey, const _tMappedType &_tValue ) {
			LSTL_PAIR<_tKeyType, _tMappedType> pPair;
			pPair.tKey = _tKey;
			pPair.tValue = _tValue;
			return Insert( pPair );
		}

		/**
		 * Insert a single pair of a key and its mapped value.  If the key exists, it is not modified.
		 *
		 * \param _pPair The pair to insert.
		 * \return Returns true if the item was added or already exists.  A return of false always indicates a
		 *	memory failure.
		 */
		LSBOOL LSE_CALL							Insert( const LSTL_PAIR<_tKeyType, _tMappedType> &_pPair ) {
			LSTL_NODE * pnParent;
			LSTL_NODE ** pnChild;
			if ( !FindPair( _pPair.tKey, pnParent, pnChild ) ) {
				LSTL_NODE * pnNode = AllocNode();
				if ( !pnNode ) { return false; }
				(*pnChild) = pnNode;
				pnNode->pnParent = pnParent;
				pnNode->mpPair = _pPair;
				//FixNode1( pnNode );
				InsertIntoTree( pnNode );
				++m_uiptrTotal;
			}
			return true;
		}

		/**
		 * Gets the value associated with a key.
		 *
		 * \param _tKey The key to find.
		 * \param _tReturn Holds the returned value.
		 * \return Returns true if the key exists.
		 */
		LSBOOL LSE_FCALL						GetValue( const _tKeyType &_tKey, _tMappedType &_tReturn ) const {
			LSTL_NODE * pnNode = m_pnRoot;
			while ( pnNode ) {
				if ( pnNode->mpPair.tKey == _tKey ) {
					_tReturn = pnNode->mpPair.tValue;
					return true;
				}
				if ( pnNode->mpPair.tKey < _tKey ) {
					pnNode = pnNode->pnLeft;
				}
				else {
					pnNode = pnNode->pnRight;
				}
			}
			return false;
		}

		/**
		 * Creates an array of pointers to each key in the map.
		 *
		 * \param _vReturn Holds the returned array of pointers to each key in the map.
		 * \return Returns true if the vector was created.
		 */
		LSBOOL LSE_FCALL						GetKeys( CVector<_tKeyType *, uint32_t> &_vReturn ) {
			_vReturn.ResetNoDealloc();
			if ( !m_uiptrTotal ) { return true; }
			if ( !_vReturn.Resize( m_uiptrTotal ) ) { return false; }
			GetKeys( _vReturn, m_pnRoot );
			return true;
		}

		/**
		 * Creates an array of pointers to each value in the map.
		 *
		 * \param _vReturn Holds the returned array of pointers to each value in the map.
		 * \return Returns true if the vector was created.
		 */
		LSBOOL LSE_FCALL						GetValues( CVector<_tMappedType *, uint32_t> &_vReturn ) {
			_vReturn.ResetNoDealloc();
			if ( !m_uiptrTotal ) { return true; }
			if ( !_vReturn.Resize( m_uiptrTotal ) ) { return false; }
			GetValues( _vReturn, m_pnRoot );
			return true;
		}

		/**
		 * Sets the allocator.  Causes the object to be fully reset.  Should not be used on
		 *	lists that have contents already.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator for map objects.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			Reset();
			m_paAllocator = _paAllocator;
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns a pointer to the allocator used by this object.
		 */
		CAllocator * LSE_CALL					GetAllocator() {
			return m_paAllocator;
		}

	protected :
		// == Enumerations.
		/** Colors red and black. */
		enum LSTL_RED_BLACK {
			LSTL_RB_RED,						/**< Red nodes. */
			LSTL_RB_BLACK,						/**< Black nodes. */
		};

		// == Types.
		/** The mapped type. */
		typedef LSTL_PAIR<_tKeyType, _tMappedType>
												LSTL_MAP_PAIR;

		/** A node, including the pair, parent, 2 children, and color. */
		typedef struct LSTL_NODE {
			LSTL_RED_BLACK						rbColor;								/**< Node color. */
			LSTL_NODE *							pnParent;								/**< Parent node. */
			LSTL_NODE *							pnLeft;									/**< Left child. */
			LSTL_NODE *							pnRight;								/**< Right child. */
			LSTL_MAP_PAIR						mpPair;									/**< Key and value pair for this node. */


			// == Various constructors.
			LSE_CALLCTOR						LSTL_NODE() :
				rbColor( LSTL_RB_RED ),
				pnParent( NULL ),
				pnLeft( NULL ),
				pnRight( NULL ) {
			}
		} * LPLSTL_NODE, * const LPCLSTL_NODE;

		/** The buffer type.  Holds a pointer to data (managed by CMap) and a CFixedExternalAllocator. */
		typedef struct LSTL_BUFFER {
			LSTL_NODE *							pnNode;									/**< Array of nodes. */
			CFixedExternalAllocator<LSTL_NODE, _uSize>
												feaAllocator;							/**< Allocator to manage pnNode. */


			// == Various constructors.
			LSE_CALLCTOR						LSTL_BUFFER() :
				pnNode( NULL ) {
			}
		} * LPLSTL_BUFFER, * const LPCLSTL_BUFFER;

		// == Members.
		/** The allocator to use with this map. */
		CAllocator *							m_paAllocator;

		/** Root node. */
		LSTL_NODE *								m_pnRoot;

		/** Null node. */
		LSTL_NODE								m_nNull;

		/** The array of arrays of pairs and allocators. */
		LPLSTL_BUFFER							m_pbBuffers;

		/** Total items in the map. */
		uintptr_t								m_uiptrTotal;

		/** Total buffers. */
		uint32_t								m_ui32TotalBuffers;


		// == Functions.
		/**
		 * Allocates the given number of bytes.
		 *
		 * \param _uiptrTotal Number of bytes to allocate.
		 * \return Returns a pointer to the created bytes or NULL.
		 */
		uint8_t * LSE_FCALL						Alloc( uintptr_t _uiptrTotal ) const {
			return m_paAllocator ? static_cast<uint8_t *>(m_paAllocator->Alloc( _uiptrTotal )) : LSENEW uint8_t[_uiptrTotal];
		}

		/**
		 * Frees the given address.
		 *
		 * \param _pui8Address Address to free.
		 */
		void LSE_FCALL						Free( uint8_t * &_pui8Address ) {
			if ( m_paAllocator ) { m_paAllocator->Free( _pui8Address ); }
			else { LSEDELETE [] _pui8Address; }
			_pui8Address = NULL;
		}

		/**
		 * Creates an array of pointers to each key in the map.
		 *
		 * \param _vReturn Holds the returned array of pointers to each key in the map.
		 * \param _pnNode Current node.
		 */
		void LSE_FCALL						GetKeys( CVector<_tKeyType *, uint32_t> &_vReturn, const LSTL_NODE * _pnNode ) {
			if ( _pnNode ) {
				GetKeys( _vReturn, _pnNode->pnRight );
				_vReturn.Push( &_pnNode->mpPair.tKey );
				GetKeys( _vReturn, _pnNode->pnLeft );
			}
		}

		/**
		 * Creates an array of pointers to each key in the map.
		 *
		 * \param _vReturn Holds the returned array of pointers to each key in the map.
		 * \param _pnNode Current node.
		 */
		void LSE_FCALL						GetValues( CVector<_tMappedType *, uint32_t> &_vReturn, const LSTL_NODE * _pnNode ) {
			if ( _pnNode ) {
				GetValues( _vReturn, _pnNode->pnRight );
				_vReturn.Push( &_pnNode->mpPair.tValue );
				GetValues( _vReturn, _pnNode->pnLeft );
			}
		}

		/**
		 * Gets the grandparent of a node.
		 *
		 * \param _pnNode The node whose grandparent is to be returned.
		 * \return Returns the grandparent of a node.
		 */
		LSTL_NODE * LSE_FCALL					GrandParent( const LSTL_NODE * _pnNode ) {
			if ( !_pnNode ) { return NULL; }
			if ( !_pnNode->pnParent ) { return NULL; }
			return _pnNode->pnParent->pnParent;
		}

		/**
		 * Gets the uncle of a node.
		 *
		 * \param _pnNode The node whose uncle is to be returned.
		 * \return Returns the uncle of a node.
		 */
		LSTL_NODE * LSE_FCALL					Uncle( const LSTL_NODE * _pnNode ) {
			LSTL_NODE * pnGrand = GrandParent( _pnNode );
			if ( !pnGrand ) { return NULL; }
			return (_pnNode->pnParent == pnGrand->pnLeft) ? pnGrand->pnRight : pnGrand->pnLeft;
		}

		/**
		 * Allocates a node.
		 *
		 * \return Returns the allocated node or NULL on memory failure.
		 */
		LSTL_NODE * LSE_CALL					AllocNode() {
			for ( uint32_t I = 0; I < m_ui32TotalBuffers; ++I ) {
				LSTL_NODE * pRet = m_pbBuffers[I].feaAllocator.Alloc();
				if ( pRet ) { return pRet; }
			}
			// None of the allocators have available memory.
			if ( !Allocate( (m_ui32TotalBuffers + 1) * _uSize ) ) { return NULL; }
			return AllocNode();
		}

		/**
		 * Finds the parent node and its child where the given pair should exist, and returns true if the
		 *	given pair exists there already.
		 *
		 * \param _ktKey The pair to find in the tree.
		 * \param _pnParent Holds the returned parent of the node under which the given pair should exist if it
		 *	does not, or already exists.
		 * \param _pnChild Holds the returned child of _pnParent where the given pair should exist if it
		 *	does not, or the actual node if it already exists.
		 * \return Returns true if the given pair exists in the tree.
		 */
		LSBOOL LSE_CALL							FindPair( const _tKeyType &_ktKey, LSTL_NODE * &_pnParent, LSTL_NODE ** &_pnChild ) const {
			_pnParent = NULL;
			return FindPair( _ktKey, const_cast<LSTL_NODE * &>(m_pnRoot), _pnParent, _pnChild );		
		}

		/**
		 * Finds the parent node and its child where the given pair should exist, and returns true if the
		 *	given pair exists there already.
		 *
		 * \param _ktKey The pair to find in the tree.
		 * \param _pnNode Current node to search recursively.
		 * \param _pnParent Holds the returned parent of the node under which the given pair should exist if it
		 *	does not, or already exists.
		 * \param _pnChild Holds the returned child of _pnParent where the given pair should exist if it
		 *	does not, or the actual node if it already exists.
		 * \return Returns true if the given pair exists in the tree.
		 */
		LSBOOL LSE_CALL							FindPair( const _tKeyType &_ktKey, LSTL_NODE * &_pnNode,
			LSTL_NODE * &_pnParent, LSTL_NODE ** &_pnChild ) const {
			if ( !_pnNode ) {
				// Found a dead-end.
				_pnChild = &_pnNode;
				return false;
			}
			if ( _pnNode->mpPair == _ktKey ) {
				// Found the pair.
				_pnChild = &_pnNode;
				return true;
			}
			// Go to one child or the other.
			_pnParent = _pnNode;
			if ( _pnNode->mpPair < _ktKey ) {
				// Go left (key is greater, <- goes to higher numbers).
				return FindPair( _ktKey, _pnNode->pnLeft, _pnParent, _pnChild );
			}
			// Go right (key is smaller, -> goes to smaller numbers).
			return FindPair( _ktKey, _pnNode->pnRight, _pnParent, _pnChild );
		}

		/**
		 * Fixes a node that was just added to maintain the structure of a red-black tree (case 1).
		 *
		 * \param _pnNode The node from which to begin the fix-up procedure.
		 */
		void LSE_CALL							FixNode1( LSTL_NODE * _pnNode ) {
			if ( _pnNode->pnParent == NULL ) {
				_pnNode->rbColor = LSTL_RB_BLACK;
			}
			else {
				FixNode2( _pnNode );
			}
		}

		/**
		 * Fixes a node that was just added to maintain the structure of a red-black tree (case 2).
		 *
		 * \param _pnNode The node from which to begin the fix-up procedure.
		 */
		void LSE_CALL							FixNode2( LSTL_NODE * _pnNode ) {
			if ( _pnNode->pnParent->rbColor == LSTL_RB_BLACK ) { return; }
			FixNode3( _pnNode );
		}

		/**
		 * Fixes a node that was just added to maintain the structure of a red-black tree (case 3).
		 *
		 * \param _pnNode The node from which to begin the fix-up procedure.
		 */
		void LSE_CALL							FixNode3( LSTL_NODE * _pnNode ) {
			LSTL_NODE * _pnUncle = Uncle( _pnNode );
			if ( _pnUncle && _pnUncle->rbColor == LSTL_RB_RED ) {
				_pnNode->pnParent->rbColor = LSTL_RB_BLACK;
				_pnUncle->rbColor = LSTL_RB_BLACK;
				LSTL_NODE * _pnGrand = GrandParent( _pnNode );
				_pnGrand->rbColor = LSTL_RB_RED;
				FixNode1( _pnGrand );
			}
			else {
				FixNode4( _pnNode );
			}
		}

		/**
		 * Fixes a node that was just added to maintain the structure of a red-black tree (case 4).
		 *
		 * \param _pnNode The node from which to begin the fix-up procedure.
		 */
		void LSE_CALL							FixNode4( LSTL_NODE * _pnNode ) {
			LSTL_NODE * _pnGrand = GrandParent( _pnNode );
			if ( _pnGrand ) {
				if ( _pnNode == _pnNode->pnParent->pnRight && _pnNode->pnParent == _pnGrand->pnLeft ) {
					RotateLeft( _pnNode->pnParent );
					_pnNode = _pnNode->pnLeft;
				}
				else if ( _pnNode == _pnNode->pnParent->pnLeft && _pnNode->pnParent == _pnGrand->pnRight ) {
					RotateRight( _pnNode->pnParent );
					_pnNode = _pnNode->pnRight;
				}
			}
			FixNode5( _pnNode );
		}

		/**
		 * Fixes a node that was just added to maintain the structure of a red-black tree (case 5).
		 *
		 * \param _pnNode The node from which to begin the fix-up procedure.
		 */
		void LSE_CALL							FixNode5( LSTL_NODE * _pnNode ) {
			LSTL_NODE * _pnGrand = GrandParent( _pnNode );
			_pnNode->pnParent->rbColor = LSTL_RB_BLACK;
			_pnGrand->rbColor = LSTL_RB_RED;
			if ( _pnNode == _pnNode->pnParent->pnLeft ) {
				RotateRight( _pnGrand );
			}
			else {
				RotateLeft( _pnGrand );
			}
		}

		/**
		 * Rotates a node left.
		 *
		 * \param _pnNode The node to rotate left.
		 */
		void LSE_CALL							RotateLeft( LSTL_NODE * _pnNode ) {
			LSTL_NODE * pnNil = &m_nNull;
			LSTL_NODE * pnY = _pnNode->pnRight;
			_pnNode->pnRight = pnY->pnLeft;

			if ( pnY->pnLeft != pnNil ) { pnY->pnLeft->pnParent = _pnNode; }
  
			pnY->pnParent=_pnNode->pnParent;   

			if ( _pnNode == _pnNode->pnParent->pnLeft ) {
				_pnNode->pnParent->pnLeft = pnY;
			}
			else {
				_pnNode->pnParent->pnRight = pnY;
			}
			pnY->pnLeft = _pnNode;
			_pnNode->pnParent = pnY;

			assert( pnNil->rbColor == LSTL_RB_RED );
		}

		/**
		 * Rotates a node right.
		 *
		 * \param _pnNode The node to rotate right.
		 */
		void LSE_CALL							RotateRight( LSTL_NODE * _pnNode ) {
			LSTL_NODE * pnNil = &m_nNull;
			LSTL_NODE * pnX = _pnNode->pnLeft;
			_pnNode->pnLeft = pnX->pnRight;

			if ( pnNil != pnX->pnRight ) { pnX->pnRight->pnParent = _pnNode; }

			pnX->pnParent = _pnNode->pnParent;
			if ( _pnNode == _pnNode->pnParent->pnLeft ) {
				_pnNode->pnParent->pnLeft = pnX;
			}
			else {
				_pnNode->pnParent->pnRight = pnX;
			}
			pnX->pnRight = _pnNode;
			_pnNode->pnParent = pnX;

			assert( pnNil->rbColor == LSTL_RB_RED );
		}

		/**
		 * Inserts a node with the assumption that the tree is well-formed.
		 *
		 * \param _pnNode The node to insert.
		 */
		void LSE_CALL							InsertHelper( LSTL_NODE * _pnNode ) {
			LSTL_NODE * pnNil = &m_nNull;
  
			_pnNode->pnLeft = _pnNode->pnRight = pnNil;
			LSTL_NODE * pnY = m_pnRoot;
			LSTL_NODE * pnX = m_pnRoot->pnLeft;
			while ( pnX != pnNil ) {
				pnY = pnX;
				if ( _pnNode->mpPair.tKey < pnX->mpPair.tKey ) {	// pnX.mpPair > _pnNode.mpPair
					pnX = pnX->pnLeft;
				}
				else {	// pnX.mpPair <= _pnNode.mpPair
					pnX = pnX->pnRight;
				}
			}
			_pnNode->pnParent = pnY;
			if ( (pnY == m_pnRoot) ||
				(_pnNode->mpPair.tKey < pnY->mpPair.tKey) ) { // pnY.mpPair > _pnNode.mpPair
				pnY->pnLeft = _pnNode;
			}
			else {	// pnX.mpPair <= _pnNode.mpPair
				pnY->pnRight = _pnNode;
			}

			assert( pnNil->rbColor == LSTL_RB_RED );
		}

		/**
		 * Inserts a node into the reb-black tree.
		 *
		 * \param _pnNode The node to insert.
		 */
		void LSE_CALL							InsertIntoTree( LSTL_NODE * _pnNode ) {
			LSTL_NODE * pnY;
			LSTL_NODE * pnNewNode;
			
			InsertHelper( _pnNode );
			pnNewNode = _pnNode;
			_pnNode->rbColor = LSTL_RB_RED;
			while ( _pnNode->pnParent->rbColor ) { /* use sentinel instead of checking for root */
				if ( _pnNode->pnParent == _pnNode->pnParent->pnParent->pnLeft ) {
					pnY = _pnNode->pnParent->pnParent->pnRight;
					if ( pnY->rbColor ) {
						_pnNode->pnParent->rbColor = LSTL_RB_BLACK;
						pnY->rbColor = LSTL_RB_BLACK;
						_pnNode->pnParent->pnParent->rbColor = LSTL_RB_RED;
						_pnNode = _pnNode->pnParent->pnParent;
					}
					else {
						if ( _pnNode == _pnNode->pnParent->pnRight ) {
							_pnNode = _pnNode->pnParent;
							RotateLeft( _pnNode );
						}
						_pnNode->pnParent->rbColor = LSTL_RB_BLACK;
						_pnNode->pnParent->pnParent->rbColor = LSTL_RB_RED;
						RotateRight( _pnNode->pnParent->pnParent );
					} 
				}
				else { /* case for _pnNode->pnParent == _pnNode->pnParent->pnParent->pnRight */
					pnY = _pnNode->pnParent->pnParent->pnLeft;
					if ( pnY->rbColor ) {
						_pnNode->pnParent->rbColor = LSTL_RB_BLACK;
						pnY->rbColor = LSTL_RB_BLACK;
						_pnNode->pnParent->pnParent->rbColor = LSTL_RB_RED;
						_pnNode = _pnNode->pnParent->pnParent;
					}
					else {
						if ( _pnNode == _pnNode->pnParent->pnLeft ) {
							_pnNode = _pnNode->pnParent;
							RotateRight( _pnNode );
						}
						_pnNode->pnParent->rbColor = LSTL_RB_BLACK;
						_pnNode->pnParent->pnParent->rbColor = LSTL_RB_RED;
						RotateLeft( _pnNode->pnParent->pnParent );
					} 
				}
			}
			m_pnRoot->pnLeft->rbColor = LSTL_RB_BLACK;
			//return pnNewNode;
		}
	};

}	// namespace lstl

#endif	// __LSTL_MAP_H__
