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
 * Description: Handles a pointer to a given type of data.  If the pointer was created by this object it will
 *	free it upon destruction or inside Reset().  If the pointer was given to this object by something else,
 *	the giver is assumed responsible for the pointer and this class will not free it nor allow modifications
 *	to it.
 */


#ifndef __LSTL_OWNEDORBORROWEDPOINTER_H__
#define __LSTL_OWNEDORBORROWEDPOINTER_H__

#include "../LSTLib.h"
#include "LSTLOwnerOrBorrowedPointerBase.h"

namespace lstl {

	/**
	 * Class COwnedOrBorrowedPointer
	 * \brief Handles a pointer to a given type of data.
	 *
	 * Description: Handles a pointer to a given type of data.  If the pointer was created by this object it will
	 *	free it upon destruction or inside Reset().  If the pointer was given to this object by something else,
	 *	the giver is assumed responsible for the pointer and this class will not free it nor allow modifications
	 *	to it.
	 */
	template <typename _tType>
	class COwnedOrBorrowedPointer : protected COwnerOrBorrowedPointerBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR					COwnedOrBorrowedPointer() :
			m_ptObjects( NULL ),
			m_ui32Total( 0 ),
			m_bBorrowed( false ) {
		}
		LSE_CALLCTOR					~COwnedOrBorrowedPointer() {
			Reset();
		}


		// == Operators.
		/**
		 * Constant array access.  Always granted.
		 *
		 * \param _ui32Index The index of the object to get.
		 * \return Returns a reference to the object at the given index.
		 */
		const _tType & LSE_CALL			operator [] ( uint32_t _ui32Index ) const {
			assert( _ui32Index < m_ui32Total );
			return m_ptObjects[_ui32Index];
		}

		/**
		 * Writeable array access.  Granted only if this object owns the pointer.
		 *
		 * \param _ui32Index The index of the object to get.
		 * \return Returns a reference to the object at the given index.
		 */
		_tType & LSE_CALL				operator [] ( uint32_t _ui32Index ) {
			assert( m_bBorrowed == false );
			assert( _ui32Index < m_ui32Total );
			return m_ptObjects[_ui32Index];
		}


		// == Functions.
		/**
		 * Returns true if the buffer it manages is borrowed.
		 *
		 * \return Returns true if the buffer it manages is borrowed.
		 */
		LSBOOL LSE_CALL					IsBorrowed() const { return m_bBorrowed; }

		/**
		 * Resets the object, freeing resources only if this object owns the resource(s) to which it points.
		 */
		void LSE_CALL					Reset() {
			if ( !m_bBorrowed ) {
				for ( uint32_t I = m_ui32Total; I--; ) {
					Destroy( I );
				}
				m_paDefaultAllocator->Free( m_ptObjects );
			}
			m_bBorrowed = false;
			m_ptObjects = NULL;
			m_ui32Total = 0;
		}

		/**
		 * Creates an owned pointer to the given number of elements.  The created objects will be freed inside Reset().
		 *
		 * \param _ui32Total Total number of objects to create.
		 * \return Returns true if there were no memory failures during the creation of the object(s).
		 */
		LSBOOL LSE_CALL					Create( uint32_t _ui32Total ) {
			Reset();
			if ( !_ui32Total ) { return true; }
			
			m_ptObjects = static_cast<_tType *>(m_paDefaultAllocator->Alloc( sizeof( _tType ) * _ui32Total ));
			if ( !m_ptObjects ) { return false; }

			for ( uint32_t I = 0; I < _ui32Total; ++I ) {
				Construct( I );
			}
			m_bBorrowed = false;
			m_ui32Total = _ui32Total;
			return true;
		}

		/**
		 * Borrows a pointer from another source.  Does not allow modifying of the data and does not free it inside Reset().
		 *
		 * \param _ptPointer Pointer to borrow.
		 * \param _ui32Total Total number of objects to which the pointer points.
		 */
		void LSE_CALL					Borrow( const _tType * _ptPointer, uint32_t _ui32Total ) {
			Reset();

			m_bBorrowed = true;
			m_ptObjects = const_cast<_tType *>(_ptPointer);
			m_ui32Total = _ui32Total;
		}

		/**
		 * Gets the length of the buffer.
		 *
		 * \return Returns the length of the buffer.
		 */
		uint32_t						Length() const {
			return m_ui32Total;
		}

		/**
		 * Gets the buffer pointer for reading.
		 *
		 * \return Returns a constant pointer to the buffer.
		 */
		const _tType *					Buffer() const {
			return m_ptObjects;
		}

		/**
		 * Gets the buffer pointer.
		 *
		 * \return Returns a pointer to the buffer.
		 */
		_tType *						Buffer() {
			return m_ptObjects;
		}


	protected :
		// == Members.
		/** Pointer to the owned or borrowed object(s). */
		_tType *						m_ptObjects;

		/** Total number of objects. */
		uint32_t						m_ui32Total;

		/** Is our pointer borrowed? */
		LSBOOL							m_bBorrowed;


		// == Functions.
		/**
		 * Secret way of constructing items.
		 *
		 * \param _ui32Index The index of the item to construct.
		 */
		void LSE_CALL					Construct( uint32_t _ui32Index ) {
			new( &m_ptObjects[_ui32Index] ) _tType;
		}
		
		/**
		 * Secret way of destroying items.  No error checking.
		 *
		 * \param _ui32Index The index of the item to destruct.
		 */
		void LSE_CALL					Destroy( uint32_t _ui32Index ) {
			// This gives warning C4100 when this class is created with types that have no destructor,
			//	claiming _ui32Index is unreferenced.
			// Erase this warning with some do-nothing code.
#ifdef LSE_VISUALSTUDIO
			static_cast<uint32_t>(_ui32Index);
#endif	// #ifdef LSE_VISUALSTUDIO
			m_ptObjects[_ui32Index].~_tType();
		}
	};

}	// namespace lstl

#endif	// __LSTL_OWNEDORBORROWEDPOINTER_H__
