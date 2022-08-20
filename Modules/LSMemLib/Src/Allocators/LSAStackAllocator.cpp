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
 * Description: Stack allocators allocate from the top of a stack and from no where else.  They are designed to
 *	provide instant allocation but do not offer any way to free individual addresses.  This facilitates systems
 *	that never allocate more than they need and free all of their allocations at the end of their lives.
 *	Allocation time is reduced dramatically and release time is entirely eliminated, and there is no book-keeping
 *	for each allocation allowing for an extreme gain in efficiency.  With proper organization, this type of
 *	allocator can be the most useful of all allocators.
 */

#include "LSAStackAllocator.h"

#include <new>


namespace lsa {

	// == Various constructors.
	LSE_CALLCTOR CStackAllocator::CStackAllocator() :
		m_psHead( NULL ),
		m_psSearchStart( NULL ) {
	}
	LSE_CALLCTOR CStackAllocator::~CStackAllocator() {
		Reset();
	}

	// == Functions.
	/**
	 * Allocates RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
	 *
	 * \param _sSize Amount of RAM to allocate.
	 * \param _ui32Align Alignment of the RAM to allocate.  Can be any power of 2.
	 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
	 */
	void * LSE_CALL CStackAllocator::Alloc( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		// Find a stack that can deliver.
		for ( LSA_STACK * psThis = m_psSearchStart; psThis; psThis = psThis->psNext ) {
			LSA_SIZE sBaseAddr = reinterpret_cast<LSA_SIZE>(psThis) + sizeof( LSA_STACK );
			LSA_SIZE sStartAddr = reinterpret_cast<LSA_SIZE>(psThis->pui8CurPos);
			LSA_SIZE sNewAddr = LSE_ROUND_UP( sStartAddr, _ui32Align );

			LSA_SIZE sTotalAllocSize = (sNewAddr - sStartAddr) + _sSize;
			LSA_SIZE sTotalRemainingSize = psThis->sSize - (sStartAddr - sBaseAddr);
			if ( !sTotalRemainingSize && psThis->psNext ) {
				m_psSearchStart = psThis->psNext;
				continue;
			}
			if ( sTotalAllocSize > sTotalRemainingSize ) { continue; }
			psThis->pui8CurPos += sTotalAllocSize;
			return reinterpret_cast<void *>(sNewAddr);
		}

		// Failed to find a match.
		// Try to allocate enough for more than just this allocation.
		LSA_SIZE sThisSize = _sSize * 2 + _ui32Align;
		sThisSize = CStd::Max<LSA_SIZE>( sThisSize, 16 * 1024 * 1024 );
		while ( sThisSize >= _sSize + _ui32Align ) {
			if ( AddStack( sThisSize ) ) { return Alloc( _sSize, _ui32Align ); }
			sThisSize -= 1 * 1024 * 1024;
		}
		return NULL;
	}

	/**
	 * Resets the stack allocator completely, releasing all resources back to the operating system.
	 */
	void LSE_CALL CStackAllocator::Reset() {
		LSA_STACK * psNext;
		for ( LSA_STACK * psThis = m_psHead; psThis; psThis = psNext ) {
			psNext = psThis->psNext;
			COsHeap::Free( psThis );
		}
		m_psHead = m_psSearchStart = NULL;
	}

	/**
	 * Adds a stack to the linked list of stacks with the given size.
	 *
	 * \param _sSize Size of the stack to add.
	 * \return Returns true if there was enough memory to add a stack of the given size.
	 */
	LSBOOL LSE_CALL CStackAllocator::AddStack( LSA_SIZE _sSize ) {
		// The actual size we need is sizeof( LSA_STACK ) larger than the requested.
		LSA_SIZE sActual = _sSize + sizeof( LSA_STACK );
		// Try to allocate that much RAM.
		uint8_t * pui8Alloc = static_cast<uint8_t *>(COsHeap::Alloc( sActual ));
		if ( !pui8Alloc ) { return false; }

		// We got some RAM.  Set everything up.
		LSA_STACK * psThis = new( pui8Alloc ) LSA_STACK;
		psThis->psNext = NULL;
		psThis->pui8CurPos = pui8Alloc + sizeof( LSA_STACK );
		psThis->sSize = _sSize;
		if ( !m_psHead ) {
			m_psHead = psThis;
			m_psSearchStart = psThis;
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

}	// namespace lsa
