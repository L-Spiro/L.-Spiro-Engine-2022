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
 * Description: Provide a base interface for allocation.  All objects in this library use this when they
 *	want to allocate RAM.  Inherit from this and modify the Alloc, ReAlloc, and Free functions, then pass
 *	your class to the templates in this library to make them use your allocation methods instead.
 */

#include "LSTLAllocator.h"


namespace lstl {

	// == Members.
	/** Allocation count.  Very simple counter that increases with each allocation and
	 *	decreases with each release.  If not 0 at the end of the game's life, something was leaked or
	 *	over-deallocated. */
	uint32_t CAllocator::m_ui32AllocCount = 0;

	/** Class counter.  Keeps track of the number of this object in existance.  When this reaches 0,
	 *	m_ui32AllocCount is checked for being 0.  If not, debug information is printed. */
	uint32_t CAllocator::m_ui32ObjCount = 0;

	/** Thread safety. */
	CCriticalSection * CAllocator::m_pcsCrit;

	// == Various constructors.
	LSE_CALLCTOR CAllocator::CAllocator() :
		m_psaAllocator( NULL ),
		m_ui32TotalAllocs( 0 ) {
		// This should be a static member, not a static pointer member, but Microsoft Visual Studio is being gay
		//	and it fails to initialize the static critical section before coming here.
		// So we have to initialize it manually via new.
		//	The first and last CAllocator objects to be created and destroyed are static class members, so the
		//	creation and destruction of the first and last living CAllocator objects is single-threaded,
		//	so this work-around is safe, but still a gay hack.
		if ( !m_pcsCrit ) {
			m_pcsCrit = LSENEW CCriticalSection();
		}
		CCriticalSection::CLocker lLockMe( (*m_pcsCrit) );
		++m_ui32ObjCount;
	}
	LSE_CALLCTOR CAllocator::~CAllocator() {
		m_pcsCrit->Lock();
		if ( --m_ui32ObjCount == 0 ) {
			if ( m_ui32AllocCount != 0 ) {
				char szBuffer[128];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%d allocations not released (CAllocator).\r\n", m_ui32AllocCount );
				CStd::DebugPrintA( szBuffer );
			}
			m_pcsCrit->UnLock();
			LSEDELETE m_pcsCrit;
			m_pcsCrit = NULL;
			// The above appears to have the ability to cause undesired behavior, but when object count reaches zero
			//	the game is shutting down and no new CAllocator objects can possibly be made.
		}
		else {
			m_pcsCrit->UnLock();
		}
	}

	// == Functions.
	/**
	 * Allocates.
	 *
	 * \param _uiptrSize Number of bytes to allocate.
	 * \return Returns the allocated bytes or NULL on failure.
	 */
	void * LSE_CALL CAllocator::Alloc( uintptr_t _uiptrSize ) {
		void * pvAddr = m_psaAllocator ? m_psaAllocator->Alloc( _uiptrSize ) : CMemLib::MAlloc( _uiptrSize );
		if ( pvAddr ) {
			CCriticalSection::CLocker lLockMe( (*m_pcsCrit) );
			++m_ui32AllocCount;
			++m_ui32TotalAllocs;
		}
		return pvAddr;
	}

	/**
	 * * Reallocate memory.
	 *
	 * \param _pvAddr The address whose allocation size is to be changed.  If NULL, this behaves
	 *	exactly the same way as Alloc().
	 * \param _uiptrSize The new size of the allocated data.
	 * \return Returns the reallocated pointer or NULL on failure.  Upon failure, the original
	 *	data is not modified and _pvAddr remains valid.
	 */
	void * LSE_CALL CAllocator::ReAlloc( void * _pvAddr, uintptr_t _uiptrSize ) {
		void * pvAddr = m_psaAllocator ? m_psaAllocator->ReAlloc( _pvAddr, _uiptrSize ) : CMemLib::MReAlloc( _pvAddr, _uiptrSize );
		if ( !_pvAddr && pvAddr ) {
			CCriticalSection::CLocker lLockMe( (*m_pcsCrit) );
			++m_ui32AllocCount;
			++m_ui32TotalAllocs;
		}
		return pvAddr;
	}

	/**
	 * Free memory previously allocated with Alloc() or ReAlloc().
	 *
	 * \param _pvAddr The address to free or NULL.
	 */
	void LSE_CALL CAllocator::Free( void * _pvAddr ) {
		if ( m_psaAllocator ) {
			if ( m_psaAllocator->Free( _pvAddr ) ) {
				if ( _pvAddr ) {
					CCriticalSection::CLocker lLockMe( (*m_pcsCrit) );
					--m_ui32AllocCount;
					--m_ui32TotalAllocs;
				}
				return;
			}
		}
		if ( !CMemLib::Free( _pvAddr ) ) {
			char szBuffer[32];
			CStd::UI64toA( reinterpret_cast<uintptr_t>(_pvAddr), szBuffer, 16 );
			CStd::DebugPrintA( "CAllocator::Free(): Attempting to free unknown address: " );
			CStd::DebugPrintA( szBuffer );
			CStd::DebugPrintA( ".\r\n" );
		}
		else if ( _pvAddr ) {
			CCriticalSection::CLocker lLockMe( (*m_pcsCrit) );
			--m_ui32AllocCount;
			--m_ui32TotalAllocs;
		}
	}

	/**
	 * Sets the allocator we use.
	 *
	 * \param _psaAllocator The allocator to be used by this object.
	 */
	void LSE_CALL CAllocator::SetAllocator( CStdAllocator * _psaAllocator ) {
		m_psaAllocator = _psaAllocator;
	}

	/**
	 * Gets the allocator used by this object.
	 *
	 * \return Returns the allocator used by this object.
	 */
	CStdAllocator * LSE_CALL CAllocator::GetAllocator() {
		return m_psaAllocator;
	}

	/**
	 * Trash this allocator.  Assumes a special heap has been assigned to this allocator
	 *	and that heap is about to be trashed.
	 */
	void LSE_CALL CAllocator::Trash() {
		if ( m_psaAllocator ) {
			m_psaAllocator->Trash();
			m_ui32AllocCount -= m_ui32TotalAllocs;
			m_ui32TotalAllocs = 0;
		}
	}

	/**
	 * Gets the allocator used by this object.
	 *
	 * \return Returns the allocator used by this object.
	 */
	const CStdAllocator * LSE_CALL CAllocator::GetAllocator() const {
		return m_psaAllocator;
	}

}	// namespace lstl
