/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A fast allocator for very specific types of allocations.  Allocations are made in a stack
 *	fashion and can't be freed until the allocator itself is freed, but unlike CStackAllocator these allow
 *	the last allocation to be reallocated.  A single object at a time can allocate from this allocator and
 *	resize the allocation up or down, then calls Done() to indicate that it is done allocating and another
 *	resizable allocation can be made.
 */

#include "LSADynamicStackAllocator.h"

#include <new>


namespace lsa {

	// == Various constructors.
	LSE_CALLCTOR CDynamicStackAllocator::CDynamicStackAllocator() :
		m_psHead( NULL ),
		m_psSearchStart( NULL ),
		m_psLast( NULL ),
		m_ui32Align( 0 ) {
		
	}
	LSE_CALLCTOR CDynamicStackAllocator::~CDynamicStackAllocator() {
		Reset();
	}

	// == Functions.
	/**
	 * Allocates RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
	 *
	 * \param _sSize Amount of RAM to allocate.
	 * \param _ui32Align Alignment of the RAM to allocate.
	 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
	 */
	void * LSE_CALL CDynamicStackAllocator::Alloc( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		_sSize = CStd::Max<LSA_SIZE>( _sSize, 1 );
		// Previous allocation is done.
		Done();
		LSA_STACK * psThis = FindStack( _sSize, _ui32Align );
		if ( !psThis ) {
			LSA_SIZE sThisSize = _sSize * 2 + _ui32Align;
			sThisSize = CStd::Max<LSA_SIZE>( sThisSize, 16 * 1024 * 1024 );
			while ( sThisSize >= _sSize + _ui32Align ) {
				if ( AddStack( sThisSize ) ) { return Alloc( _sSize, _ui32Align ); }
				sThisSize -= 1 * 1024 * 1024;
			}
			return NULL;
		}

		// Found something.
		m_psLast = psThis;
		m_ui32Align = _ui32Align;
		uintptr_t uiptrBase = reinterpret_cast<uintptr_t>(psThis->pui8CurStart);
		uintptr_t uiptrTemp = LSE_ROUND_UP_ANY( uiptrBase, _ui32Align );
		uintptr_t uiptrFinalSize = uiptrTemp - uiptrBase;
		assert( uiptrFinalSize <= psThis->sSize );
		psThis->pui8CurStart += uiptrFinalSize;
		psThis->sSize -= uiptrFinalSize;
		psThis->sCurSize = _sSize;

		return psThis->pui8CurStart;
	}

	/**
	 * Resizes the last allocation made to the new given size.  If no allocation was made last, this behaves the same
	 *	as Alloc().
	 *
	 * \param _sSize New size of the previous allocation.
	 * \param _ui32Align Alignment of the RAM to allocate.  Only used if no previous allocation has been made.
	 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
	 */
	void * LSE_CALL CDynamicStackAllocator::ReAlloc( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		if ( !m_psLast || m_psLast->sCurSize == 0 ) {
			// No allocations have been made.
			return Alloc( _sSize, _ui32Align );
		}

		// If the new size is small enough to fit into the existing allocation.
		if ( _sSize <= m_psLast->sSize ) {
			m_psLast->sCurSize = _sSize;
			return m_psLast->pui8CurStart;
		}
		
		// A new block has to be made.
		LSA_STACK * psThis = m_psLast;
		if ( !Alloc( _sSize, m_ui32Align ) ) {
			// Failed to allocate enough for the new size.  Do nothing to the old data.
			return NULL;
		}

		// m_psLast now points to the new data.
		// Copy old to new.
		CStd::MemCpy( m_psLast->pui8CurStart, psThis->pui8CurStart, psThis->sCurSize );
		// Remove old allocation.
		psThis->sCurSize = 0;
		return m_psLast->pui8CurStart;
	}

	/**
	 * Resets the stack allocator completely, releasing all resources back to the operating system.
	 */
	void LSE_CALL CDynamicStackAllocator::Reset() {
		LSA_STACK * psNext;
		for ( LSA_STACK * psThis = m_psHead; psThis; psThis = psNext ) {
			psNext = psThis->psNext;
			COsHeap::Free( psThis );
		}
		m_psHead = m_psSearchStart = m_psLast = NULL;
		m_ui32Align = 0;
	}

	/**
	 * Allocates a new stack with the given size.
	 *
	 * \param _sSize Size of the stack to add.
	 * \return Returns true if there was enough memory to add a stack of the given size.
	 */
	LSBOOL LSE_CALL CDynamicStackAllocator::AddStack( LSA_SIZE _sSize ) {
		// The actual size we need is sizeof( LSA_STACK ) larger than the requested.
		LSA_SIZE sActual = _sSize + sizeof( LSA_STACK );
		// Try to allocate that much RAM.
		uint8_t * pui8Alloc = static_cast<uint8_t *>(COsHeap::Alloc( sActual ));
		if ( !pui8Alloc ) { return false; }

		// We got some RAM.  Set everything up.
		LSA_STACK * psThis = new( pui8Alloc ) LSA_STACK;
		psThis->psNext = NULL;
		psThis->pui8CurStart = pui8Alloc + sizeof( LSA_STACK );
		psThis->sSize = _sSize;
		psThis->sCurSize = 0;
		m_psSearchStart = psThis;
		if ( !m_psHead ) {
			m_psHead = psThis;
		}
		else {
			for ( LSA_STACK * psThat = m_psHead; psThat; psThat = psThat->psNext ) {
				if ( !psThat->psNext ) {
					psThat->psNext = psThis;
					break;
				}
			}
		}
		
		return true;
	}

	/**
	 * Finds a stack with enough space for the requested allocation given an alignment.
	 *
	 * \param _sSize The requested size.
	 * \param _ui32Align The requested allignment.
	 * \return Returns an existing stack if it is large enough for the allocation or NULL if none are.
	 */
	CDynamicStackAllocator::LSA_STACK * LSE_CALL CDynamicStackAllocator::FindStack( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		for ( LSA_STACK * psThis = m_psSearchStart; psThis; psThis = psThis->psNext ) {
			if ( psThis->sCurSize ) { continue; }	// Can't allocate past existing allocations.
			// If the allocation was made here.
			uintptr_t uiptrBase = reinterpret_cast<uintptr_t>(psThis->pui8CurStart);
			uintptr_t uiptrTemp = LSE_ROUND_UP_ANY( uiptrBase, _ui32Align ) + _sSize;
			uintptr_t uiptrFinalSize = uiptrTemp - uiptrBase;
			if ( uiptrFinalSize < psThis->sSize ) { return psThis; }
		}

		// Start the search over from the beginning.
		for ( LSA_STACK * psThis = m_psHead; psThis && psThis != m_psSearchStart; psThis = psThis->psNext ) {
			if ( psThis->sCurSize ) { continue; }	// Can't allocate past existing allocations.
			// If the allocation was made here.
			uintptr_t uiptrBase = reinterpret_cast<uintptr_t>(psThis->pui8CurStart);
			uintptr_t uiptrTemp = LSE_ROUND_UP_ANY( uiptrBase, _ui32Align ) + _sSize;
			uintptr_t uiptrFinalSize = uiptrTemp - uiptrBase;
			if ( uiptrFinalSize < psThis->sSize ) { return psThis; }
		}
		return NULL;
	}

}	// namespace lsa
