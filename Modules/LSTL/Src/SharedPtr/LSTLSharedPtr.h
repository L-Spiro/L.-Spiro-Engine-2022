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
 * Description: A standard shared pointer.
 */


#ifndef __LSTL_SHAREDPTR_H__
#define __LSTL_SHAREDPTR_H__

#include "../LSTLib.h"
#include "../Allocator/LSTLAllocator.h"

namespace lstl {

	// == Enumerations.
	/**
	 * A single enumerated value that lets the shared pointer know to create a new object within its constructor.
	 */
	enum LSSTL_SHARED_PTR {
		LSSTL_SP_MAKE_NEW
	};

	/**
	 * Class CSharedPtr
	 * \brief A standard shared pointer.
	 *
	 * Description: A standard shared pointer.
	 */
	template <typename _tType>
	class CSharedPtr {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CSharedPtr() :
			m_pcCont( NULL ),
			m_ptDirect( NULL ) {
		}
		explicit LSE_CALLCTOR			CSharedPtr( LSSTL_SHARED_PTR /*_spFlag*/ ) {
			// Create a new object with a 1 reference count.
			m_pcCont = static_cast<CSharedPtr<_tType>::LSSTL_CONTAINER *>(CMemLib::MNew( NULL, LSSTL_CONTAINER ));
			if ( !m_pcCont ) { throw LSSTD_E_OUTOFMEMORY; }
			m_pcCont->paAllocator = NULL;
			m_pcCont->ui32RefCount = 1;
			m_ptDirect = &m_pcCont->tObj;
		}
		explicit LSE_CALLCTOR			CSharedPtr( CAllocator * _paAllocator ) {
			// Create a new object with a 1 reference count.
			m_pcCont = static_cast<CSharedPtr<_tType>::LSSTL_CONTAINER *>(CMemLib::MNew( _paAllocator ? _paAllocator->GetAllocator() : NULL, LSSTL_CONTAINER ));
			if ( !m_pcCont ) { throw LSSTD_E_OUTOFMEMORY; }
			m_pcCont->paAllocator = _paAllocator;
			m_pcCont->ui32RefCount = 1;
			m_ptDirect = &m_pcCont->tObj;
		}
		LSE_CALLCTOR					CSharedPtr( const CSharedPtr<_tType> &_spOther ) {
			// Reference the object to which it points.
			m_pcCont = _spOther.m_pcCont;
			m_ptDirect = _spOther.m_ptDirect;
			if ( m_pcCont ) {
				++m_pcCont->ui32RefCount;
			}
		}
		template <typename _tCastType>	
		LSE_CALLCTOR					CSharedPtr( const CSharedPtr<_tCastType> &_spOther ) {
			// Reference the object to which it points.
			m_pcCont = const_cast<LSSTL_CONTAINER *>(reinterpret_cast<const LSSTL_CONTAINER *>(_spOther.GetContainer()));
			m_ptDirect = const_cast<_tType *>(static_cast<const _tType *>(_spOther.Get()));
			if ( m_pcCont ) {				
				++m_pcCont->ui32RefCount;
			}
		}
		LSE_CALLCTOR					~CSharedPtr() {
			Reset();
		}


		// == Types.
		/**
		 * Encapsulates our reference count and the object into a structure.
		 */
		typedef struct LSSTL_CONTAINER {
			/** Pointer to the allocator used to create the object.  This must remain
			 *	alive until this object is completely deleted. */
			CAllocator *				paAllocator;

			/** The reference count. */
			uint32_t					ui32RefCount;

			/** The object we encapsulate. */
			_tType						tObj;
		} * LPLSSTL_CONTAINER, * const LPCLSSTL_CONTAINER;


		// == Operators.
		/**
		 * Copy operator.
		 *
		 * \param _spOther The object to copy.
		 * \return Returns this object after the copy.
		 */
		CSharedPtr<_tType> & LSE_CALL	operator = ( const CSharedPtr<_tType> &_spOther ) {
			if ( _spOther.m_pcCont ) {
				// Increase their reference count before decreasing ours.
				++_spOther.m_pcCont->ui32RefCount;
			}
			Reset();
			m_pcCont = _spOther.m_pcCont;
			m_ptDirect = _spOther.m_ptDirect;
			return (*this);
		}

		/**
		 * Dereference operator.
		 *
		 * \return Returns a reference to the object that it shares.
		 */
		_tType & LSE_CALL				operator * () {
			return (*Get());
		}

		/**
		 * Constant dereference operator.
		 *
		 * \return Returns a read-only reference to the object that it shares.
		 */
		const _tType & LSE_CALL			operator * () const {
			return (*Get());
		}

		/**
		 * Arrow operator.
		 *
		 * \return Returns the raw pointer to the object that it shares.
		 */
		_tType * LSE_CALL				operator -> () {
			return Get();
		}

		/**
		 * Constant arrow operator.
		 *
		 * \return Returns the read-only raw pointer to the object that it shares.
		 */
		const _tType * LSE_CALL			operator -> () const {
			return Get();
		}

		/**
		 * Casting to type uintptr_t.
		 *
		 * \return Returns the uintptr_t reporesentation of this shared pointer.
		 */
		LSE_CALL						operator uintptr_t() const {
			return reinterpret_cast<uintptr_t>(Get());
		}


		// == Functions.
		/**
		 * Creates a new object.  The previous object, if any, will be deleted if its reference
		 *	count reaches 0.  If this was pointing to a previous object, the same allocator will
		 *	be used.
		 *
		 * \return Returns true if a new object could be made.
		 */
		LSBOOL LSE_CALL					New() {
			CAllocator * paAllocator = NULL;
			if ( m_pcCont ) {
				paAllocator = m_pcCont->paAllocator;
			}
			Reset();
			m_pcCont = static_cast<CSharedPtr<_tType>::LSSTL_CONTAINER *>(CMemLib::MNew( paAllocator ? paAllocator->GetAllocator() : NULL, LSSTL_CONTAINER ));
			if ( !m_pcCont ) { return false; }
			m_pcCont->paAllocator = paAllocator;
			m_pcCont->ui32RefCount = 1;
			m_ptDirect = &m_pcCont->tObj;
			return true;
		}

		/**
		 * Returns the raw pointer to the object that it shares.
		 *
		 * \return Returns the raw pointer to the object that it shares.
		 */
		_tType * LSE_CALL				Get() {
			return m_ptDirect;
		}

		/**
		 * Returns the read-only raw pointer to the object that it shares.
		 *
		 * \return Returns the read-only raw pointer to the object that it shares.
		 */
		const _tType * LSE_CALL			Get() const {
			return m_ptDirect;
		}

		/**
		 * Returns the reference count of the object to which it points.  0 is returned if the pointer
		 *	is invalid.
		 *
		 * \return Returns the reference count of the object to which it points.  0 is returned if the pointer
		 *	is invalid.
		 */
		uint32_t LSE_CALL				RefCount() const {
			return m_pcCont ? m_pcCont->ui32RefCount : 0;
		}

		/**
		 * Returns true or false depending on whether or not the shared pointer points to something.
		 *
		 * \return Returns true if the shared pointer is valid.
		 */
		LSBOOL LSE_CALL					Valid() const {
			return m_pcCont != NULL;
		}

		/**
		 * Decreases the reference count by one on the object to which this points
		 *	and frees it if it reaches 0.
		 */
		void LSE_CALL					Reset() {
			if ( m_pcCont ) {
				if ( --m_pcCont->ui32RefCount == 0 ) {
					// Delete it.  Use the direct pointer to call the destructor.  This is the only
					//	way to get virtual destructors to be called properly in the case that this
					//	object was a cast of a different type of shared pointer.
					m_ptDirect->~_tType();
					if ( m_pcCont->paAllocator ) {
						m_pcCont->paAllocator->Free( m_pcCont );
					}
					else {
						CMemLib::Free( m_pcCont );
					}
				}
				m_pcCont = NULL;
				m_ptDirect = NULL;
			}
		}

		/**
		 * Decreases the reference count by one on the object to which this points
		 *	and trashes the object if it reaches 0.
		 */
		void LSE_CALL					Trash() {
			if ( m_pcCont ) {
				if ( --m_pcCont->ui32RefCount == 0 ) {
					// Trash it.
					m_ptDirect->Trash();
				}
				m_pcCont = NULL;
				m_ptDirect = NULL;
			}
		}

		/**
		 * Gets the container held by this shared pointer which is used for maintaining the
		 *	reference count.  This should never be accessed.  The only reason it is a public function
		 *	is to allow the casting operator to access the protected data of other types of shared
		 *	pointers.  Beyond that, this function is prohibed from use.
		 *
		 * \return Returns a constant reference to the container used by this shared pointer.
		 */
		const LSSTL_CONTAINER * LSE_CALL
										GetContainer() const {
			return m_pcCont;
		}


	protected :
		// == Members.
		/** The container we encapsulate. */
		LSSTL_CONTAINER *				m_pcCont;

		/** Pointer directly to the object.  Important for cleaning up. */
		_tType *						m_ptDirect;

	};

}	// namespace lstl

#endif	// __LSTL_SHAREDPTR_H__
