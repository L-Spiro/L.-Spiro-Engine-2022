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

#include "LSAStdAllocator.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsa {

	// == Various constructors.
	LSE_CALLCTOR CStdAllocator::CStdAllocator() :
		m_psahHead( NULL ),
#ifndef LSA_DEBUG
		m_psaSmallHead( NULL ),
#endif	// #ifndef LSA_DEBUG
		m_sSize( 0 ),
		m_sLow( 0 ),
		m_sHi( 0 ),
		m_bGrowable( false ) {
	}
	LSE_CALLCTOR CStdAllocator::~CStdAllocator() {
		Clear();
	}
	
	// == Functions.
	/**
	 * Allocates RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
	 *
	 * \param _sSize Amount of RAM to allocate.
	 * \param _ui32Align Alignment of the RAM to allocate.  Can be 16 or 32.
	 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
	 */
	void * LSE_CALL CStdAllocator::Alloc( LSA_SIZE _sSize, uint32_t _ui32Align					LSA_DEBUGPARMSDEF ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		if ( _sSize == 0 ) {
			_sSize = 1;
		}
#ifndef LSA_DEBUG
		// If the request is small enough, get it from a small heap.
		if ( _sSize <= LSA_LARGEST_ALLOCATION_SIZE ) {
			void * pvRet = AllocInternalSmall( _sSize, _ui32Align );
			if ( pvRet ) { return pvRet; }

			// Add a new small allocator and try to allocate from that.
			if ( AddSmallAllocator() ) {
				pvRet = AllocInternalSmall( _sSize, _ui32Align );
				if ( pvRet ) { return pvRet; }
			}
		}
#endif	// #ifndef LSA_DEBUG

		void * pvRet = AllocInternal( _sSize, _ui32Align LSA_DEBUGPARMSPASS );
		if ( pvRet ) { return pvRet; }
		
		// Failed to allocate, so see if we can grow.
		if ( m_bGrowable ) {
			LSA_SIZE sMinSizeRequired = _sSize + (sizeof( CStdAllocatorHeap ) + (LSA_MIN_ALIGN * 8));
			// Attempt to allocate more than the request amount.
			LSA_SIZE sAttempSize = _sSize << 1;
			// Start by allocating no fewer than 1 megabyte, or 0.5 if on a low-end device.
#ifdef LSE_IPHONE
			if ( sAttempSize < 1 * 512 * 1024 ) { sAttempSize = 1 * 512 * 1024; }
#else
			if ( sAttempSize < 1 * 1024 * 1024 ) { sAttempSize = 1 * 1024 * 1024; }
#endif	// #ifdef LSE_IPHONE

			// To help some OS memory managers.  This may save us a page of memory.
			sAttempSize -= 0x20;
			sAttempSize = sAttempSize > sMinSizeRequired ? sAttempSize : sMinSizeRequired;
			
			while ( true ) {
				if ( AddBlock( sAttempSize ) ) { break; }
				// Block was too big.  Try again at half size.
				sAttempSize += 0x20;
				sAttempSize >>= 1;
				sAttempSize -= 0x20;
				// If the new block would be too small, try to allocate the minimum amount.
				if ( sAttempSize < sMinSizeRequired ) {
					if ( !AddBlock( sMinSizeRequired ) ) { return NULL; }
					break;
				}
			}
			
			// Print a message saying we needed to grow.
			
			// Try the allocation again now that we have a block of RAM large enough.
			return AllocInternal( _sSize, _ui32Align LSA_DEBUGPARMSPASS );
		}
		
		// Not growable.  Fail.
		return NULL;
	}

	/**
	 * Exactly the same as Alloc(), but the allocated RAM, if any, is filled with 0's.
	 *
	 * \param _sSize Amount of RAM to allocate.
	 * \param _ui32Align Alignment of the RAM to allocate.  Can be 16 or 32.
	 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
	 */
	void * LSE_CALL CStdAllocator::CAlloc( LSA_SIZE _sSize, uint32_t _ui32Align					LSA_DEBUGPARMSDEF ) {
		void * pvRet = Alloc( _sSize, _ui32Align LSA_DEBUGPARMSPASS );
		if ( pvRet ) {
			CStd::MemSet( pvRet, 0, _sSize );
		}
		return pvRet;
	}
	
	/**
	 * Reallocate RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
	 *
	 * \param _pvAddr Address to reallocate.
	 * \param _sSize New size of the allocation.
	 * \return Returns a pointer to the reallocated data or NULL if there is not enough RAM available to make the allocation.
	 */
	void * LSE_CALL CStdAllocator::ReAlloc( void * _pvAddr, LSA_SIZE _sSize						LSA_DEBUGPARMSDEF ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		if ( !_pvAddr ) { return Alloc( _sSize, LSA_MIN_ALIGN LSA_DEBUGPARMSPASS ); }

#ifndef LSA_DEBUG
		// Mixing the small heaps and the variable-sized heaps complicates things a bit.
		// There are 4 cases to handle:
		// #1: Source and destination are in small heaps.
		// #2: Source is in a small heap and destination is in a regular heap.
		// #3: Source is in a regular heap an destination is in a small heap.
		// #4: Source and destination are in regular heaps.
		// Start by scanning the small heaps for the allocation.
		for ( CSmallAllocator * psaThis = m_psaSmallHead; psaThis; psaThis = psaThis->m_psaNext ) {
			if ( psaThis->HaveAddress( _pvAddr ) ) {
				// It is in this small heap.
				// Only need to handle cases 1 and 2.
				LSA_SIZE sOldSize;
				// Attempting reallocation handle case 1 for us and in the event of case 2 it gives us
				//	the size of the block to allow us to handle things.
				void * pvNewAddr = psaThis->ReAlloc( _pvAddr, _sSize, sOldSize );
				if ( pvNewAddr ) { return pvNewAddr; }
				// From here, both cases require copying from the old address to a new address and we need to
				//	have the size of the old data to do that.
				if ( _sSize == 0 ) { return NULL; }
				// Cases 1 and 2 are handled the same by calling Alloc().
				// Alloc() will get the value from a small heap or a regular heap as per needed.
				// Get the previous alignment of the data.
				uintptr_t uiptrAddr = reinterpret_cast<uintptr_t>(_pvAddr);
				uint32_t ui32Align = (uiptrAddr % (LSA_MIN_ALIGN << 1) == 0) ? (LSA_MIN_ALIGN << 1) : LSA_MIN_ALIGN;
				pvNewAddr = Alloc( _sSize, ui32Align LSA_DEBUGPARMSPASS );

				if ( !pvNewAddr ) { return NULL; }

				// Copy from the old location to the new.
				CStd::MemCpy( pvNewAddr, _pvAddr, sOldSize < _sSize ? sOldSize : _sSize );

				// Free the old, which we know will succeed.
				psaThis->Free( _pvAddr );

				// Return the new.
				return pvNewAddr;
			}
		}
		// Check for case 3.
		if ( _sSize <= LSA_LARGEST_ALLOCATION_SIZE ) {
			// From a regular heap to a small heap.
			// Get the allocation's size from the regular heap.
			LSA_SIZE sOldSize = 0;
			CStdAllocatorHeap * psahHeap = NULL;
			LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(_pvAddr);
			for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
				// Is it in range?
				if ( sThis >= reinterpret_cast<LSA_SIZE>(psahThis->m_pvHeap) && sThis < reinterpret_cast<LSA_SIZE>(psahThis->m_pvHeap) + psahThis->m_sSize ) {
					CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER lpahHeader = psahThis->FindAllocationBlock( _pvAddr );
					if ( lpahHeader ) {
						psahHeap = psahThis;
						sOldSize = CStdAllocatorHeap::GetSize( lpahHeader );
						break;
					}
				}
			}
			if ( !psahHeap ) { return NULL; }


			// Alloc() will get the value from a small heap or a regular heap as per needed.
			// Get the previous alignment of the data.
			uintptr_t uiptrAddr = reinterpret_cast<uintptr_t>(_pvAddr);
			uint32_t ui32Align = (uiptrAddr % (LSA_MIN_ALIGN << 1) == 0) ? (LSA_MIN_ALIGN << 1) : LSA_MIN_ALIGN;
			void * pvNewAddr = Alloc( _sSize, ui32Align LSA_DEBUGPARMSPASS );
			if ( !pvNewAddr ) { return NULL; }

			// Copy from the old location to the new.
			CStd::MemCpy( pvNewAddr, _pvAddr, sOldSize < _sSize ? sOldSize : _sSize );

			// Free it from the heap in which it used to be.
			psahHeap->Free( _pvAddr );
			return pvNewAddr;
		}
		// Case 4 (drop through to unmodified regular code).
#endif	// #ifndef LSA_DEBUG
		CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER lpahHeader;
		void * pvNew = ReAllocInternal( lpahHeader, _pvAddr, _sSize LSA_DEBUGPARMSPASS );
		if ( pvNew ) { return pvNew; }
		
		// If not requesting a resize, fail.
		if ( !lpahHeader ) { return NULL; }
		// If unable to grow, fail.
		if ( !m_bGrowable ) { return NULL; }
		
		// Manually move the data.
		pvNew = Alloc( _sSize, CStdAllocatorHeap::GetAlignment( lpahHeader->ui32Alignment ) LSA_DEBUGPARMSPASS );
		if ( !pvNew ) { return NULL; }
		LSA_SIZE sSizeToMove = _sSize;
		if ( CStdAllocatorHeap::GetSize( lpahHeader ) < sSizeToMove ) {
			sSizeToMove = CStdAllocatorHeap::GetSize( lpahHeader );
		}
		CStd::MemCpy( pvNew, _pvAddr, sSizeToMove );
		Free( _pvAddr );
		return pvNew;
	}
	
	/**
	 * Clear everything.  Returns false if any of the blocks were not fully empty.
	 *
	 * \return Returns true if all blocks were cleared.
	 */
	LSBOOL LSE_CALL CStdAllocator::Clear() {
		CCriticalSection::CLocker lLock( m_csCrit );
		LSBOOL bRet = true;
		CStdAllocatorHeap * psahNext = NULL;
		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahNext ) {
			psahNext = psahThis->m_psahNext;
			bRet = bRet && psahThis->IsEmpty( true );
			psahThis->~CStdAllocatorHeap();
			COsHeap::Free( psahThis );
		}
		m_psahHead = NULL;

#ifndef LSA_DEBUG
		CSmallAllocator * psaNext = NULL;
		for ( CSmallAllocator * psaThis = m_psaSmallHead; psaThis; psaThis = psaNext ) {
			psaNext = psaThis->m_psaNext;
			bRet = bRet && psaThis->IsEmpty();
			psaThis->~CSmallAllocator();
			COsHeap::Free( psaThis );
		}
		m_psaSmallHead = NULL;
#endif	// #ifndef LSA_DEBUG

		m_sSize = 0;
		m_bGrowable = false;
		m_sLow = m_sHi = 0;
		
		return bRet;
	}

	/**
	 * Release/remove empty blocks.  Does not remove the initial block.  Does not clear blocks in which
	 *	there are still allocations.  Can be called at any time.
	 */
	void LSE_CALL CStdAllocator::ReleaseEmptyHeaps() {
		CCriticalSection::CLocker lLock( m_csCrit );
		if ( m_psahHead ) {
			CStdAllocatorHeap * psahNext = NULL;
			CStdAllocatorHeap * psahPrev = m_psahHead;
			// Skip the head, which may not be removed by this function.
			for ( CStdAllocatorHeap * psahThis = m_psahHead->m_psahNext; psahThis; psahThis = psahNext ) {
				psahNext = psahThis->m_psahNext;
				if ( psahThis->IsEmpty( false ) ) {
					// Remove this item from the linked list.
					psahPrev->m_psahNext = psahNext;
					
					// Then delete it.
					psahThis->~CStdAllocatorHeap();
					COsHeap::Free( psahThis );
				}
				else {
					// Only advance the previous pointer if we did not remove this item.
					psahPrev = psahThis;
				}
			}
		}

#ifndef LSA_DEBUG
		if ( m_psaSmallHead ) {
			CSmallAllocator * psaNext = NULL;
			CSmallAllocator * psaPrev = m_psaSmallHead;
			// Skip the head, which may not be removed by this function.
			for ( CSmallAllocator * psaThis = m_psaSmallHead->m_psaNext; psaThis; psaThis = psaNext ) {
				psaNext = psaThis->m_psaNext;
				if ( psaThis->IsEmpty() ) {
					// Remove this item from the linked list.
					psaPrev->m_psaNext = psaNext;
					
					// Then delete it.
					psaThis->~CSmallAllocator();
					COsHeap::Free( psaThis );
				}
				else {
					// Only advance the previous pointer if we did not remove this item.
					psaPrev = psaThis;
				}
			}
		}
#endif	// #ifndef LSA_DEBUG
	}
	
	/**
	 * Print allocations with an allocation counter above or equal to the given number.  If LSA_DEBUG is
	 *	not defined, this does nothing.
	 *
	 * \param _ui32Count Allocations with an allocation number above or equal to this value will be printed.
	 *	Use 0 to print all allocations.
	 * \param _ui32End Ending allocation number up to which to print allocations.
	 * \return Returns the combined size of the allocations printed.
	 */
	LSA_SIZE LSE_CALL CStdAllocator::PrintAllocations(
#ifdef LSA_DEBUG
		uint32_t _ui32Count, uint32_t _ui32End
#else
		uint32_t /*_ui32Count*/, uint32_t /*_ui32End*/
#endif	// #ifdef LSA_DEBUG
		) const {
#ifdef LSA_DEBUG
		CCriticalSection::CLocker lLock( m_csCrit );
		if ( !m_psahHead ) { return 0; }

		LSA_SIZE sSize = 0;
		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			sSize += psahThis->PrintAllocations( _ui32Count, _ui32End );
		}
		return sSize;
#else	// #ifdef LSA_DEBUG
		return 0;
#endif	// #ifdef LSA_DEBUG
	}

	/**
	 * Gets the total amount of RAM allocated from this heap.  This is the amount of RAM that this heap has
	 *	allocated from the system for its own use, and includes the RAM that it gives to the application upon
	 *	allocation requests.
	 *
	 * \return Returns the total amount of RAM allocated by this heap.
	 */
	LSA_SIZE LSE_CALL CStdAllocator::GetTotalAllocatedSize() {
		CCriticalSection::CLocker lLock( m_csCrit );
		if ( !m_psahHead ) { return 0; }
		LSA_SIZE sSize = 0;
		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			sSize += psahThis->GetHeapSize();
		}
		return sSize;
	}

#ifdef LSA_DEBUG
	/**
	 * Verify the statuses of all heaps.  This debug-only feature ensures that the memory manager
	 *	has not been corrupted by a buffer overrun.
	 */
	void LSE_CALL CStdAllocator::VerifyBlocks() const {
		CCriticalSection::CLocker lLock( m_csCrit );
		
		if ( !m_psahHead ) { return; }

		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			psahThis->VerifyFreeBlocks();
		}
	}
#endif	// #ifdef LSA_DEBUG

	/**
	 * Sets the size of the heap.  Can only be done once.
	 *
	 * \param _sSize Size of the heap.
	 * \param _bGrowable Whether the heap is growable or not.
	 * \return Returns true if the heap was sized correctly.  A return of false always indicates a lack of memory.
	 */
	LSBOOL LSE_CALL CStdAllocator::SetSize( LSA_SIZE _sSize, LSBOOL _bGrowable ) {
		CCriticalSection::CLocker lLock( m_csCrit );
		
		if ( !_sSize ) { return true; }
		if ( m_sSize ) { return false; }

		m_sSize = 0;
		m_bGrowable = _bGrowable;

		return AddBlock( _sSize );
	}
	
	/**
	 * Adds a new block.
	 *
	 * \param _sSize Size of the block to add.
	 * \return Returns true if a block was added with the given size.
	 */
	LSBOOL LSE_CALL CStdAllocator::AddBlock( LSA_SIZE _sSize ) {
		void * pvNewBlock = COsHeap::Alloc( _sSize );
		if ( !pvNewBlock ) { return false; }
		
		// If the block is too small, we can do nothing with it.
		CStdAllocatorHeap * psahNewBlock = reinterpret_cast<CStdAllocatorHeap *>(pvNewBlock);
		if ( _sSize < GetMinSizeOfBlock( psahNewBlock ) ) {
			COsHeap::Free( pvNewBlock );
			return false;
		}
		
		LSA_SIZE sThisAddr = reinterpret_cast<LSA_SIZE>(psahNewBlock);
		if ( m_sLow == 0 || sThisAddr < m_sLow ) {
			m_sLow = sThisAddr;
		}

		// Construct the block.
		new( psahNewBlock ) CStdAllocatorHeap();

		LSA_SIZE sStartOfHeap = reinterpret_cast<LSA_SIZE>(GetStartAddress( psahNewBlock ));
		LSA_SIZE sSizeOfFreeArea = _sSize - (sStartOfHeap - reinterpret_cast<LSA_SIZE>(psahNewBlock));
		sSizeOfFreeArea /= LSA_MIN_ALIGN;
		sSizeOfFreeArea *= LSA_MIN_ALIGN;
		psahNewBlock->Set( reinterpret_cast<void *>(sStartOfHeap), sSizeOfFreeArea );
		
		LSA_SIZE sEnd = sStartOfHeap + sSizeOfFreeArea;
		if ( sEnd > m_sHi ) {
			m_sHi = sEnd;
		}
		m_sSize += sSizeOfFreeArea;


		// Add the block to the start of the linked list.
		psahNewBlock->m_psahNext = m_psahHead;
		m_psahHead = psahNewBlock;
		/*if ( !m_psahHead ) {
			m_psahHead = psahNewBlock;
		}
		else {
			for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
				if ( !psahThis->m_psahNext ) {
					psahThis->m_psahNext = psahNewBlock;
					break;
				}
			}
		}*/
		return true;
	}

#ifndef LSA_DEBUG
	/**
	 * Adds a new small allocator.
	 * \return Returns true if successful.
	 */
	LSBOOL LSE_CALL CStdAllocator::AddSmallAllocator() {
		// We need to get the size of the object but we need to allocate the
		//	object to know what the size it will be.
		// The only way to go is to over-allocate by a given amount.
		LSA_SIZE sSize = CSmallAllocator::GetFullAllocatorSizeWithNumberOf256Blocks( NULL, LSA_SMALL_ALLOCATOR_BLOCKS ) + (LSA_MIN_ALIGN << 1);
		void * pvNewBlock = COsHeap::Alloc( sSize );
		if ( !pvNewBlock ) { return false; }

		uintptr_t uiptrHeapStart = reinterpret_cast<uintptr_t>(pvNewBlock);
		uiptrHeapStart += sizeof( CSmallAllocator );
		uiptrHeapStart = LSE_ROUND_UP( uiptrHeapStart, LSA_MIN_ALIGN << 1 );

		// Construct the object here.
		CSmallAllocator * psaNew = new( pvNewBlock ) CSmallAllocator( reinterpret_cast<void *>(uiptrHeapStart),
			LSA_SMALL_ALLOCATOR_BLOCKS );

		if ( !m_psaSmallHead ) {
			m_psaSmallHead = psaNew;
		}
		else {
			for ( CSmallAllocator * psaThis = m_psaSmallHead; psaThis; psaThis = psaThis->m_psaNext ) {
				if ( !psaThis->m_psaNext ) {
					psaThis->m_psaNext = psaNew;
					break;
				}
			}
		}

		if ( m_sLow == 0 || psaNew->m_sLow < m_sLow ) {
			m_sLow = psaNew->m_sLow;
		}

		LSA_SIZE sEnd = psaNew->m_sHi;
		if ( sEnd > m_sHi ) {
			m_sHi = sEnd;
		}
		m_sSize += psaNew->m_sHi - psaNew->m_sLow;
		return true;
	}
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
	void * LSE_CALL CStdAllocator::ReAllocInternal( CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER &_lpahBlock, void * _pvAddr, LSA_SIZE _sSize	LSA_DEBUGPARMSDEF ) {
		_lpahBlock = NULL;
		// Determine if the given address exists at all.
		LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(_pvAddr);
		if ( sThis < m_sLow || sThis >= m_sHi ) { return NULL; }
		
		// We might have it.
		// Be sure by scanning.
		for ( CStdAllocatorHeap * psahThis = m_psahHead; psahThis; psahThis = psahThis->m_psahNext ) {
			// Attempt to resize on this block.
			void * pvNew = psahThis->ReAlloc( _lpahBlock, _pvAddr, _sSize LSA_DEBUGPARMSPASS );
			
			// If it succeeded we are done.
			if ( pvNew ) { return pvNew; }
			// If it did not have the address, keep looking.
			if ( !_lpahBlock ) { continue; }
			
			return NULL;
		}
		
		// Print an error saying the requested address was not found.
		_lpahBlock = NULL;
		return NULL;
	}

}	// namespace lsa

#pragma warning( pop )
