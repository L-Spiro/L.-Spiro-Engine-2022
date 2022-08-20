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
 * Description: A template providing linked-list functionality.  The list is one-way.
 */


#ifndef __LSTL_SINGLELINKEDLIST_H__
#define __LSTL_SINGLELINKEDLIST_H__

#include "../LSTLib.h"
#include "LSTLLinkedListBase.h"

namespace lstl {

	/**
	 * Class CSingleLinkedList
	 * \brief A template providing linked-list functionality.
	 *
	 * Description: A template providing linked-list functionality.  The list is one-way.
	 */
	template <typename _tType>
	class CSingleLinkedList : protected CLinkedListBase {
	public :
		// == Various constructors.
		LSE_CALL								CSingleLinkedList() :
			m_lpllnHead( NULL ),
			m_paOurAllocator( &Parent::m_aAllocator ) {
		}
		LSE_CALL								~CSingleLinkedList() {
			Reset();
		}


		// == Types.
		/**
		 * Each item in the linked list is of this type.
		 */
		typedef struct LSTL_LINKED_LIST_NODE {
			/**
			 * The object we hold.
			 */
			_tType								tLinkedListObject;

			/**
			 * The next object in the list.
			 */
			LSTL_LINKED_LIST_NODE *				pllnLinkedListNext;
		} * LPLSTL_LINKED_LIST_NODE, * const LPCLSTL_LINKED_LIST_NODE;


		// == Functions.
		/**
		 * Sets the allocator to be used by this instance.  Causes the object to be fully reset.  Cannot be used on
		 *	lists that have contents already.
		 *
		 * \param _paAllocator Pointer to the allocator to use.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			Reset();
			m_paOurAllocator = _paAllocator;
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = &Parent::m_aAllocator;
			}
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns our allocator.
		 */
		CAllocator * LSE_CALL					GetAllocator() {
			return m_paOurAllocator;
		}

		/**
		 * Reset everything.  Clears all resources created by this instance.
		 */
		void LSE_CALL							Reset() {
			LPLSTL_LINKED_LIST_NODE lpllnNext = NULL;
			for ( LPLSTL_LINKED_LIST_NODE lpllnThis = m_lpllnHead;
				lpllnThis; lpllnThis = lpllnNext ) {
				lpllnNext = lpllnThis->pllnLinkedListNext;
				Destroy( (*lpllnThis) );
				m_paOurAllocator->Free( lpllnThis );
			}

			m_lpllnHead = NULL;
		}

		/**
		 * Insert an item after another item.  If the previous item is NULL, the new item is
		 *	inserted at the beginning of the list.
		 *
		 * \param _tNew The item to insert.
		 * \param _lpllnNode The node after which the item is to be inserted or NULL.
		 * \return Returns the created node if insertion was successful.  Returns NULL if allocation fails.
		 */
		LPLSTL_LINKED_LIST_NODE LSE_CALL		InsertAfter( const _tType &_tNew, LPLSTL_LINKED_LIST_NODE _lpllnNode ) {
			LPLSTL_LINKED_LIST_NODE lpllnNew = NewNode();
			if ( !lpllnNew ) { return NULL; }
			Construct( (*lpllnNew) );
			lpllnNew->tLinkedListObject = _tNew;

			if ( !_lpllnNode ) {
				// Insert at the head.
				lpllnNew->pllnLinkedListNext = m_lpllnHead;
				m_lpllnHead = lpllnNew;
			}
			else {
				// Otherwise it goes after some item or another.
				lpllnNew->pllnLinkedListNext = _lpllnNode->pllnLinkedListNext;
				_lpllnNode->pllnLinkedListNext = lpllnNew;
			}

			return lpllnNew;
		}

		/**
		 * Removes an item from the linked list.  The item is deleted upon being removed and the
		 *	item pointer may no longer be used.
		 *
		 * \param _lpllnNode The node to remove from this linked list.
		 */
		void LSE_CALL							RemoveItem( LPLSTL_LINKED_LIST_NODE _lpllnNode ) {
			if ( !_lpllnNode ) { return; }
			// Unlink the object.
			if ( m_lpllnHead == _lpllnNode ) {
				// At the head.
				m_lpllnHead = _lpllnNode->pllnLinkedListNext;
			}
			else {
				// Find the node before it.
				for ( LPLSTL_LINKED_LIST_NODE pllnThis = m_lpllnHead; pllnThis; pllnThis = pllnThis->pllnLinkedListNext ) {
					if ( pllnThis->pllnLinkedListNext == _lpllnNode ) {
						pllnThis->pllnLinkedListNext = _lpllnNode->pllnLinkedListNext;
						break;
					}
				}
			}

			Destroy( (*_lpllnNode) );
			m_paOurAllocator->Free( _lpllnNode );
		}

		/**
		 * Gets the head of this list.
		 *
		 * \return Returns the head of the linked list, which may be NULL.
		 */
		LPLSTL_LINKED_LIST_NODE LSE_CALL		Head() {
			return m_lpllnHead;
		}

		/**
		 * Gets the head of this list.
		 *
		 * \return Returns the head of the linked list, which may be NULL.
		 */
		const LPLSTL_LINKED_LIST_NODE LSE_CALL	Head() const {
			return m_lpllnHead;
		}

		/**
		 * Insert at the front of the list.
		 *
		 * \param _tNew The item to add to the list.
		 * \return Returns the created node if insertion was successful.  Returns NULL if allocation fails.
		 */
		LPLSTL_LINKED_LIST_NODE LSE_CALL		PushFront( const _tType &_tNew ) {
			return InsertAfter( _tNew, NULL );
		}

		/**
		 * Gets the total number of items in the list.
		 *
		 * \return Returns the number of items in the linked list.
		 */
		uint32_t LSE_CALL						Length() {
			uint32_t ui32Len = 0;
			for ( LPLSTL_LINKED_LIST_NODE lpllnThis = m_lpllnHead;
				lpllnThis; lpllnThis = lpllnThis->pllnLinkedListNext ) {
				++ui32Len;
			}
			return ui32Len;
		}

		/**
		 * Finds the linked-list entry containing the given value.
		 *
		 * \param _tValue The value to find.
		 * \return Returns a pointer to the linked-list entry with the given value or NULL.
		 */
		LPLSTL_LINKED_LIST_NODE LSE_CALL		Find( const _tType &_tValue ) {
			for ( LPLSTL_LINKED_LIST_NODE lpllnThis = m_lpllnHead;
				lpllnThis; lpllnThis = lpllnThis->pllnLinkedListNext ) {
				if ( lpllnThis->tLinkedListObject == _tValue ) {
					return lpllnThis;
				}
			}
			return NULL;
		}


	protected :
		// == Members.
		/**
		 * The head of the list.
		 */
		LPLSTL_LINKED_LIST_NODE					m_lpllnHead;

		/**
		 * The allocator we should use.
		 */
		CAllocator *							m_paOurAllocator;

		
		// == Functions.
		/**
		 * Secret way of constructing items.
		 *
		 * \param _llnNode The item to be constructed.
		 */
		void LSE_CALL							Construct( LSTL_LINKED_LIST_NODE &_llnNode ) {
			_llnNode.pllnLinkedListNext = NULL;
			new( &_llnNode.tLinkedListObject ) _tType;
		}
		/**
		 * Secret way of destroying items.  No error checking.
		 *
		 * \param _llnNode The item to be destroyed.
		 */
		void LSE_CALL							Destroy( const LSTL_LINKED_LIST_NODE &_llnNode ) {
			// This gives warning C4100 when this class is created with types that have no destructor,
			//	claiming _llnNode is unreferenced.
			// Erase this warning with some do-nothing code.
#ifdef LSE_VISUALSTUDIO
			static_cast<LSTL_LINKED_LIST_NODE>(_llnNode);
#endif	// #ifdef LSE_VISUALSTUDIO
			_llnNode.tLinkedListObject.~_tType();
		}

		/**
		 * Creates a new node.
		 *
		 * \return Returns a new node.
		 */
		LPLSTL_LINKED_LIST_NODE LSE_CALL		NewNode() {
			return static_cast<LPLSTL_LINKED_LIST_NODE>(m_paOurAllocator->Alloc( sizeof( LSTL_LINKED_LIST_NODE ) ));
		}

	private :
		typedef CLinkedListBase					Parent;
	};

}	// namespace lstl

#endif	// __LSTL_SINGLELINKEDLIST_H__
