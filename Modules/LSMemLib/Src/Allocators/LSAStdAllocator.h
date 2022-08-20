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
 * Description: By default it manages a single CStdAllocatorHeap object, which represents one contiguous block
 *	of RAM from which you can allocate, reallocate, and free.  If the heap is growable, it can add more of these
 *	objects in a singly-linked list to manage more blocks of memory.
 * By using CStdAllocatorHeap objects, there is no restriction on the sizes of allocations, order of
 *	allocations, or order of freeing.
 */


#ifndef __LSA_STDALLOCATOR_H__
#define __LSA_STDALLOCATOR_H__

#include "../OSHeap/LSAOsHeap.h"
#ifndef LSA_DEBUG
#include "LSASmallAllocator.h"
#endif	// #ifndef LSA_DEBUG
#include "LSAStdAllocatorHeap.h"
#include <new>

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsa {

	/**
	 * Class CStdAllocator
	 *
	 * Description: A fully featured memory manager with multiple blocks of RAM managed
	 *	by CStdAllocatorHeap objects.
	 */
	class CStdAllocator {
	protected :
		friend class CMemLib;
		// == Various constructors.
		LSE_CALLCTOR								CStdAllocator();
		LSE_CALLCTOR								~CStdAllocator();


	public :
		// == Functions.
		/**
		 * Allocates RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
		 *
		 * \param _sSize Amount of RAM to allocate.
		 * \param _ui32Align Alignment of the RAM to allocate.  Can be 16 or 32.
		 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
		 */
		void * LSE_CALL							Alloc( LSA_SIZE _sSize, uint32_t _ui32Align = 16						LSA_DEBUGPARMSDECL );

		/**
		 * Exactly the same as Alloc(), but the allocated RAM, if any, is filled with 0's.
		 *
		 * \param _sSize Amount of RAM to allocate.
		 * \param _ui32Align Alignment of the RAM to allocate.  Can be 16 or 32.
		 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
		 */
		void * LSE_CALL							CAlloc( LSA_SIZE _sSize, uint32_t _ui32Align = 16						LSA_DEBUGPARMSDECL );

		/**
		 * Free RAM.
		 *
		 * \param _pvAddr The RAM to free.
		 * \return Returns true if the given pointer was previously returned by a call to Alloc().
		 */
		LSE_INLINE LSBOOL LSE_CALL					Free( void * _pvAddr );

		/**
		 * Reallocate RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
		 *
		 * \param _pvAddr Address to reallocate.
		 * \param _sSize New size of the allocation.
		 * \return Returns a pointer to the reallocated data or NULL if there is not enough RAM available to make the allocation.
		 */
		void * LSE_CALL							ReAlloc( void * _pvAddr, LSA_SIZE _sSize								LSA_DEBUGPARMSDECL );

		/**
		 * Clear everything.  Returns false if any of the blocks were not fully empty.
		 *
		 * \return Returns true if all blocks were cleared.
		 */
		LSBOOL LSE_CALL								Clear();

		/**
		 * Trash the whole block of memory.  Assumes the user is aware that there may be allocated
		 *	data in the block but does not care.  Does not throw warnings.  Sets the block to its
		 *	default state and allocation can resume as normal.
		 */
		LSE_INLINE void LSE_CALL					Trash();
		
		/**
		 * Release/remove empty blocks.  Does not remove the initial block.  Does not clear blocks in which
		 *	there are still allocations.  Can be called at any time.
		 */
		void LSE_CALL								ReleaseEmptyHeaps();

		/**
		 * Print allocations with an allocation counter above or equal to the given number.  If LSA_DEBUG is
		 *	not defined, this does nothing.
		 *
		 * \param _ui32Count Allocations with an allocation number above or equal to this value will be printed.
		 *	Use 0 to print all allocations.
		 * \param _ui32End Ending allocation number up to which to print allocations.
		 * \return Returns the combined size of the allocations printed.
		 */
		LSA_SIZE LSE_CALL							PrintAllocations( uint32_t _ui32Count = 0, uint32_t _ui32End = 0xFFFFFFFFUL ) const;

		/**
		 * Gets the total amount of RAM allocated from this heap.  This is the amount of RAM that this heap has
		 *	allocated from the system for its own use, and includes the RAM that it gives to the application upon
		 *	allocation requests.
		 *
		 * \return Returns the total amount of RAM allocated by this heap.
		 */
		LSA_SIZE LSE_CALL							GetTotalAllocatedSize();

#ifdef LSA_DEBUG
		/**
		 * Verify the statuses of all heaps.  This debug-only feature ensures that the memory manager
		 *	has not been corrupted by a buffer overrun.
		 */
		void LSE_CALL								VerifyBlocks() const;
#endif	// #ifdef LSA_DEBUG
		
	protected :
		/**
		 * Sets the size of the heap.  Can only be done once.
		 *
		 * \param _sSize Size of the heap.
		 * \param _bGrowable Whether the heap is growable or not.
		 * \return Returns true if the heap was sized correctly.  A return of false always indicates a lack of memory.
		 */
		LSBOOL LSE_CALL								SetSize( LSA_SIZE _sSize, LSBOOL _bGrowable );

		/**
		 * Adds a new block.
		 *
		 * \param _sSize Size of the block to add.
		 * \return Returns true if a block was added with the given size.
		 */
		LSBOOL LSE_CALL								AddBlock( LSA_SIZE _sSize );

#ifndef LSA_DEBUG
		/**
		 * Adds a new small allocator.
		 * \return Returns true if successful.
		 */
		LSBOOL LSE_CALL								AddSmallAllocator();
#endif	// #ifndef LSA_DEBUG

		/**
		 * Gets the first allocatable address in a block given its address.
		 *
		 * \param _psahBlock Block.
		 * \return Returns the first allocatable address in the block.
		 */
		LSE_INLINE static void * LSE_CALL			GetStartAddress( const CStdAllocatorHeap * _psahBlock );

		/**
		 * Gets the minimum size of a block.
		 *
		 * \param _psahBlock Block.
		 * \return Returns the minimum size of a block.
		 */
		LSE_INLINE static LSA_SIZE LSE_CALL			GetMinSizeOfBlock( const CStdAllocatorHeap * _psahBlock );

		/**
		 * Allocates interally.  Does not try to add heaps etc.  Does not lock.
		 *
		 * \param _sSize Size of the allocation.
		 * \param _ui32Align Alignment of the allocation.
		 * \return Returns the address of the allocation or NULL if the allocation failed.
		 */
		LSE_INLINE void * LSE_CALL				AllocInternal( LSA_SIZE _sSize, uint32_t _ui32Align						LSA_DEBUGPARMSDECL );

#ifndef LSA_DEBUG
		/**
		 * Allocates from a small heap.  Does not try to add heaps etc.  Does not lock.
		 *
		 * \param _sSize The size to try to allocate.
		 * \param _ui32Align The alignment of the address to return
		 * \return Returns the allocated address or NULL if none could be allocated.
		 */
		LSE_INLINE void * LSE_CALL				AllocInternalSmall( LSA_SIZE _sSize, uint32_t _ui32Align );
#endif	// #ifndef LSA_DEBUG

		/**
		 * Reallocate internally.  Does not try to add heaps etc.  Does not lock.
		 * On failure, it returns the block that could not be moved (if any) because we need to know its size and
		 *	alignment to move it manually.
		 *
		 * \param _lpahBlock Block.
		 * \param _pvAddr Address to reallocate.
		 * \param _sSize New allocation size.
		 * \return Returns a pointer to the reallocated data or NULL if there is not enough RAM available to make the allocation. 
		 */
		void * LSE_CALL							ReAllocInternal( CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER &_lpahBlock, void * _pvAddr, LSA_SIZE _sSize		LSA_DEBUGPARMSDECL );

		// == Members.
		// Thread safety.
		mutable CCriticalSection					m_csCrit;

		// Linked list of blocks.
		CStdAllocatorHeap *							m_psahHead;

#ifndef LSA_DEBUG
		// Linked list of small blocks.
		CSmallAllocator *							m_psaSmallHead;
#endif	// #ifndef LSA_DEBUG

		// Total size of the heap.
		LSA_SIZE									m_sSize;
		// Can the heap grow?
		LSBOOL										m_bGrowable;

		// Range of addresses covered by this object.
		LSA_SIZE									m_sLow,
													m_sHi;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Free RAM.
	 *
	 * \param _pvAddr The RAM to free.
	 * \return Returns true if the given pointer was previously returned by a call to Alloc().
	 */
	LSE_INLINE LSBOOL LSE_CALL CStdAllocator::Free( void * _pvAddr ) {
		if ( !_pvAddr ) { return true; }
		CCriticalSection::CLocker lLock( m_csCrit );
		// Quick range check.
		LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(_pvAddr);
		if ( sThis < m_sLow || sThis >= m_sHi ) { return false; }
#ifndef LSA_DEBUG
		// Check small heaps.
		for ( CSmallAllocator * psaThis = m_psaSmallHead; psaThis; psaThis = psaThis->m_psaNext ) {
			if ( psaThis->HaveAddress( _pvAddr ) ) {
				return psaThis->Free( _pvAddr );
			}
		}
#endif	// #ifndef LSA_DEBUG

		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			if ( psahThis->Free( _pvAddr ) ) { return true; }
		}
		return false;
	}

	/**
	 * Trash the whole block of memory.  Assumes the user is aware that there may be allocated
	 *	data in the block but does not care.  Does not throw warnings.  Sets the block to its
	 *	default state and allocation can resume as normal.
	 */
	LSE_INLINE void LSE_CALL CStdAllocator::Trash() {
		CCriticalSection::CLocker lLock( m_csCrit );
		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			psahThis->Trash();
		}
#ifndef LSA_DEBUG
		for ( CSmallAllocator * psaThis = m_psaSmallHead; psaThis; psaThis = psaThis->m_psaNext ) {
			psaThis->Trash();
		}
#endif	// #ifndef LSA_DEBUG
	}

	/**
	 * Gets the first allocatable address in a block given its address.
	 *
	 * \param _psahBlock Block.
	 * \return Returns the first allocatable address in the block.
	 */
	LSE_INLINE void * LSE_CALL CStdAllocator::GetStartAddress( const CStdAllocatorHeap * _psahBlock ) {
		LSA_SIZE sStart = reinterpret_cast<LSA_SIZE>(_psahBlock);
		sStart += sizeof( CStdAllocatorHeap );
		sStart = CStdAllocatorHeap::RoundUp( sStart );
		return reinterpret_cast<void *>(sStart);
	}

	/**
	 * Gets the minimum size of a block.
	 *
	 * \param _psahBlock Block.
	 * \return Returns the minimum size of a block.
	 */
	LSE_INLINE LSA_SIZE LSE_CALL CStdAllocator::GetMinSizeOfBlock( const CStdAllocatorHeap * _psahBlock ) {
		LSA_SIZE sStart = reinterpret_cast<LSA_SIZE>(_psahBlock);
		LSA_SIZE sEnd = reinterpret_cast<LSA_SIZE>(GetStartAddress( _psahBlock ));
		// Must have enough space for an allocation-block header and 4 allocation units.
		sEnd += LSA_MIN_ALIGN * 4 + sizeof( CStdAllocatorHeap::LSA_ALLOCATIONHEADER );
		return CStdAllocatorHeap::RoundUp( sEnd ) - sStart;
	}

	/**
	 * Allocates interally.  Does not try to add heaps etc.  Does not lock.
	 *
	 * \param _sSize Size of the allocation.
	 * \param _ui32Align Alignment of the allocation.
	 * \return Returns the address of the allocation or NULL if the allocation failed.
	 */
	LSE_INLINE void * LSE_CALL CStdAllocator::AllocInternal( LSA_SIZE _sSize, uint32_t _ui32Align					LSA_DEBUGPARMSDEF ) {
		// Find a block that manages to allocate it.
		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			void * pvRet = psahThis->Alloc( _sSize, _ui32Align LSA_DEBUGPARMSPASS );
			if ( pvRet ) { return pvRet; }
		}
		// No blocks could allocate it.  Fail.
		return NULL;
	}

#ifndef LSA_DEBUG
	/**
	 * Allocates from a small heap.  Does not try to add heaps etc.  Does not lock.
	 *
	 * \param _sSize The size to try to allocate.
	 * \param _ui32Align The alignment of the address to return
	 * \return Returns the allocated address or NULL if none could be allocated.
	 */
	LSE_INLINE void * LSE_CALL CStdAllocator::AllocInternalSmall( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		// Find a block that manages to allocate it.
		for ( CSmallAllocator * psaThis = m_psaSmallHead; psaThis; psaThis = psaThis->m_psaNext ) {
			void * pvRet = psaThis->Alloc( _sSize, _ui32Align );
			if ( pvRet ) { return pvRet; }
		}
		// No blocks could allocate it.  Fail.
		return NULL;
	}
#endif	// #ifndef LSA_DEBUG

}	// namespace lsa

#pragma warning( pop )

#endif	// __LSA_STDALLOCATOR_H__
