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
 * Description: This low-level allocator represents a single contiguous block of RAM.  It uses 2 hash tables:
 *	one for speedy loop-up by size (free space) and one for speedy loop-up by address (allocated blocks).
 * Each free block is doubly linked forward and backwards linearly in RAM, such that the previous pointer
 *	always points to a lower address in RAM and the next pointer always points to a higher address or NULL.
 *	These pointers point to the previous and next free blocks respectively.  The same blocks are singly--
 *	linked in the hash table organized by size.  Finally, each free block also contains a member specifying
 *	the actual size of the block.  Contiguous free blocks are always merged, so it is always possible to
 *	deduce that either an allocated block or the end of the heap immediately follows a free block.
 * Each allocated block is singly-linked linearly, pointing to the next allocated block or NULL.  Each
 *	allocated block also has a pointer to the previous free block to make removal from the list faster.  They
 *	also have a linked list of pointers used by the address hash table.  The address table size depends on
 *	the LSA_BUCKETS macro.  The buckets in the table are spaced to cover the whole range of given addresses,
 *	so the maximum number of entries in a single bucket depends on both the LSA_BUCKETS macro and the size of
 *	the heap covered by this class.
 * There is no pointer to the first allocated block because it can easily be derived.  The first allocated
 *	block can only be in one of 2 places: at the start of the heap or, if there is free space at the start of
 *	the heap, immediately following the first free block.
 */

#include "LSAStdAllocatorHeap.h"


namespace lsa {

	// == Members.
#ifdef LSA_DEBUG
	// Allocation number given to every allocation.
	uint32_t CStdAllocatorHeap::m_ui32AllocCount = 0;
#endif	// #ifdef LSA_DEBUG

#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
	// Have we made the sin table?
	LSBOOL CStdAllocatorHeap::m_bMadeTable = false;

	// The sin table.
	double CStdAllocatorHeap::m_dSinTable[0x10000] = { 0.0 };
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
	
	/**
	 * Allocates bytes.
	 *
	 * \param _sSize Number of bytes to allocate.
	 * \param _ui32Align Alignment of the allocation.
	 * \return Returns the allocated bytes or NULL.
	 */
	void * LSE_CALL CStdAllocatorHeap::Alloc( LSA_SIZE _sSize, uint32_t _ui32Align LSA_DEBUGPARMSDEF ) {
		if ( _sSize == 0 ) { return NULL; }
		if ( _sSize > m_sSize ) { return NULL; }
		// Convert the alignment to our alignment code.
		if ( _ui32Align ) {
			_ui32Align = ((_ui32Align - 1) >> LSA_MIN_ALIGN_BITS) & 1;
		}
		LPLSA_FREEHEADER lpfhBlock = FindFreeStoreBlock( _sSize, _ui32Align );
		// If nothing could be found, we are done.
		if ( !lpfhBlock ) { return NULL; }

		// Something could be found, so add the block and return its address.
#ifdef LSA_DEBUG
		void * pvRet = AddAlloc( GetNextAllocatedBlock( lpfhBlock ), reinterpret_cast<void *>(lpfhBlock),
			_sSize, _ui32Align LSA_DEBUGPARMSPASS );
		/*static uint32_t ui32Count = 0;
		if ( ++ui32Count % 2 == 0 ) {
			VerifyFreeBlocks();
		}*/
		//VerifyFreeBlocks();
		return pvRet;
#else
		return AddAlloc( GetNextAllocatedBlock( lpfhBlock ), reinterpret_cast<void *>(lpfhBlock),
			_sSize, _ui32Align LSA_DEBUGPARMSPASS );
#endif	// #ifdef LSA_DEBUG
	}

	/**
	 * Free bytes.
	 *
	 * \param _pvAddr Address to free.
	 * \return Returns true if the address was allocated by this heap object.
	 */
	LSBOOL LSE_CALL CStdAllocatorHeap::Free( void * _pvAddr ) {
		if ( !_pvAddr ) { return true; }
		LSA_SIZE sAddr = reinterpret_cast<LSA_SIZE>(_pvAddr);
		LSA_SIZE sLow = reinterpret_cast<LSA_SIZE>(m_pvHeap);
		LSA_SIZE sHi = sLow + m_sSize;
		if ( sAddr < sLow || sAddr >= sHi ) { return false; }
		LPLSA_ALLOCATIONHEADER lpahThis = FindAllocationBlock( _pvAddr );
		if ( !lpahThis ) { return false; }

		RemAlloc( lpahThis );
#ifdef LSA_DEBUG
		/*static uint32_t ui32Count = 0;
		if ( ++ui32Count % 15 == 0 ) {
			VerifyFreeBlocks();
		}*/
		//VerifyFreeBlocks();
#endif	// #ifdef LSA_DEBUG
		return true;
	}

	/**
	 * Are we empty?  In debug mode it also prints files and line numbers of remaining allocations.
	 *
	 * \param _bReport If true, non-empty blocks are printed to the debug stream.
	 * \return Returns true if the heap is empty.
	 */
	LSBOOL LSE_CALL CStdAllocatorHeap::IsEmpty(
#ifndef LSA_DEBUG
		LSBOOL /*_bReport*/
#else
		LSBOOL _bReport
#endif	// #ifndef LSA_DEBUG
		) const {
		LSBOOL bRet = true;


		for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
#ifndef LSA_DEBUG
			if ( m_lpahAllocatedBlocks[I] ) { return false; }
#else	// #ifdef LSA_DEBUG

			for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[I]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
				bRet = false;
				if ( _bReport ) {
					PrintAllocation( lpahThis );
				}
			}
#endif	// #ifdef LSA_DEBUG
		}
		return bRet;

	}

	/**
	 * Trash the contents of the block.  Resets it to a clean slate ready for allocation.
	 */
	void LSE_CALL CStdAllocatorHeap::Trash() {
		for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
			m_lpahAllocatedBlocks[I] = NULL;
			m_lpfhFreeBlocks[I] = NULL;
		}
		m_lpfhFreeHead = NULL;
		m_usLowestFreeBlockInHashTable = LSA_BUCKETS - 1;

		// Add the whole block to the free store.
		AddFreeBlock( NULL, NULL, m_pvHeap, m_sSize );
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
	LSA_SIZE LSE_CALL CStdAllocatorHeap::PrintAllocations(
#ifndef LSA_DEBUG
		uint32_t /*_ui32Count*/, uint32_t /*_ui32End*/
#else
		uint32_t _ui32Count, uint32_t _ui32End
#endif	// #ifndef LSA_DEBUG
		) const {
#ifndef LSA_DEBUG
		return 0;
#else	// #ifndef LSA_DEBUG
		LSA_SIZE sSize = 0;
		for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
			for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[I]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
				if ( lpahThis->ui32Number >= _ui32Count && lpahThis->ui32Number < _ui32End ) {
					sSize += GetSize( lpahThis );
					PrintAllocation( lpahThis );
				}
			}
		}
		return sSize;
#endif	// #ifndef LSA_DEBUG
	}

	/**
	 * Gets the allocation counter.  Used for debugging.  If LSA_DEBUG is not defined, this always returns 0.
	 *	This does not return the actual number of allocations.  It returns the unique allocation count to
	 *	be assigned to the next allocation.
	 *
	 * \return Returns the allocation counter, or 0 if LSA_DEBUG is not defined.
	 */
	uint32_t LSE_CALL CStdAllocatorHeap::GetAllocationCounter() {
#ifdef LSA_DEBUG
		return m_ui32AllocCount;
#else	// #ifdef LSA_DEBUG
		return 0;
#endif	// #ifdef LSA_DEBUG
	}

	/**
	 * Adjust the length of an allocation.  If it cannot be adjusted in place it is copied to a new
	 *	location.  NULL is returned if it cannot be adjusted.
	 *
	 * \param _lpahHeader Header.
	 * \param _pvAddr Block to resize.
	 * \param _sSize New size of the block.
	 * \return Returns the new address of the allocation or NULL.
	 */
	void * LSE_CALL CStdAllocatorHeap::ReAlloc( LPLSA_ALLOCATIONHEADER &_lpahHeader, void * _pvAddr, LSA_SIZE _sSize LSA_DEBUGPARMSDEF ) {
		_lpahHeader = NULL;
		// If _pvAddr is NULL, just allocate.
		if ( !_pvAddr ) { return Alloc( _sSize, LSA_MIN_ALIGN LSA_DEBUGPARMSPASS ); }

		// If the new size is 0 error.
		if ( _sSize == 0 ) { return NULL; }

		// Is it in our range?
		LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(_pvAddr);
		if ( sThis < reinterpret_cast<LSA_SIZE>(m_pvHeap) ) {
			return NULL;
		}
		if ( sThis >= reinterpret_cast<LSA_SIZE>(m_pvHeap) + m_sSize ) {
			return NULL;
		}

		// First, find the block to resize.
		LPLSA_ALLOCATIONHEADER lpahThis = FindAllocationBlock( _pvAddr );
		if ( !lpahThis ) { return NULL; }
		_lpahHeader = lpahThis;

		
		LSA_SIZE sOldSize = GetActualSizeOfBlock( reinterpret_cast<void *>(lpahThis), GetSize( lpahThis ), lpahThis->ui32Alignment );
		LSA_SIZE sNewSize = GetActualSizeOfBlock( reinterpret_cast<void *>(lpahThis), _sSize, lpahThis->ui32Alignment );
		// If they are the same size, no changes need to be made.
		if ( sNewSize == sOldSize ) { return _pvAddr; }

		LPLSA_FREEHEADER lpfhNextFree = GetNextFreeBlock( lpahThis );
		LSA_SIZE sNextFree = reinterpret_cast<LSA_SIZE>(lpfhNextFree);
		LSA_SIZE sCurEnd = sOldSize + reinterpret_cast<LSA_SIZE>(lpahThis);
		LSA_SIZE sNewEnd = sNewSize + reinterpret_cast<LSA_SIZE>(lpahThis);


		// If the new size is smaller than before, there will be free space after this block.
		// The free space must be large enough for a LSA_FREEHEADER object or no resize can be made
		//	(which does not constitute an error).
		LSBOOL bFreeSpaceTouchesThisBlock = lpfhNextFree && sNextFree == sCurEnd;
		if ( sNewSize < sOldSize ) {
			// Get the size of the free area after the resize.
			LSA_SIZE sSizeOfFreeAreaAfterResize = sOldSize - sNewSize;

			// If the following block is free store, add its size.
			if ( bFreeSpaceTouchesThisBlock ) {
				sSizeOfFreeAreaAfterResize += lpfhNextFree->sActualSize;
			}
			if ( sSizeOfFreeAreaAfterResize < sizeof( LSA_FREEHEADER ) ) {
				// No adjustments can be made, but none are needed.
				return _pvAddr;
			}
			//VerifyFreeBlocks();

			// From here we know it is enough to shorten this block, add
			//	a free block immediately following, and merge it.
			// But before adding the new free block, if there is another free block
			//	following it and they are close enough that adding this free block
			//	would overwrite data in the following free block, we need to remove
			//	the following free block and add it back as one new block.

			// In any case, we always resize the current block.
			lpahThis->ui32Size = CreateSizeKey( _sSize );

			// If there is a free block immediately following the one we are going to add,
			//	remove it.
			if ( bFreeSpaceTouchesThisBlock ) {
				// Adjust the pointer to the next free block to point to the next next block
				//	for allocation later.
				LPLSA_FREEHEADER lpfhFreeMe = lpfhNextFree;
				lpfhNextFree = lpfhNextFree->pfhNextFree;

				// Erasing this block will cause pointers to it to point to whatever
				//	free block was before it.
				LPLSA_FREEHEADER lpfhPrev = lpfhFreeMe->pfhPrevFree;
				LPLSA_ALLOCATIONHEADER lpahStart = GetNextAllocatedBlock( lpfhFreeMe );

				RemFreeBlockInternal( lpfhFreeMe );
				ReplaceAllocationFreeBlockPointers( lpahStart,
					lpfhFreeMe, lpfhPrev );
			}

			// Add the new free block at the end of this allocated block.
			AddFreeBlock( lpahThis->pfhPrevFree, lpfhNextFree,
				reinterpret_cast<void *>(sNewEnd), sSizeOfFreeAreaAfterResize );
//			PrintFreeBlocks();
#ifdef LSA_DEBUG
			//static uint32_t ui32Count = 0;
			//if ( ++ui32Count % 1 == 0 ) {
				//VerifyFreeBlocks();
			//}
#endif	// #ifdef LSA_DEBUG
			return _pvAddr;
		}

		// It is being resized upwards.
		// There are several conditions that could make us relocate the block.
		// Lay them out easily with a few variables.
		LSBOOL bHaveFreeSpaceAfterBlock = lpfhNextFree != NULL;
		LSBOOL bFollowingFreeSpaceCanHoldNewBytes = bFreeSpaceTouchesThisBlock &&
			((sNewSize - sOldSize) <= lpfhNextFree->sActualSize);

		if ( !(bHaveFreeSpaceAfterBlock &&
			bFollowingFreeSpaceCanHoldNewBytes) ) {
			// Next block is allocation.
			void * pvRet = Alloc( _sSize, GetAlignment( lpahThis->ui32Alignment ) LSA_DEBUGPARMSPASS );

			// Unable to move.  Do not touch the existing memory.
			if ( !pvRet ) { return NULL; }

			// Able to move.  Copy it over.
			LSA_SIZE sActualOldSize = GetSize( lpahThis );
			LSA_SIZE sCopySize = _sSize < sActualOldSize ? _sSize : sActualOldSize;
			CStd::MemCpy( pvRet, _pvAddr, sCopySize );
			Free( _pvAddr );
			//PrintFreeBlocks();
#ifdef LSA_DEBUG
			//static uint32_t ui32Count = 0;
			//if ( ++ui32Count % 1 == 0 ) {
				//VerifyFreeBlocks();
			//}
#endif	// #ifdef LSA_DEBUG
			return pvRet;
		}

		// It is being resized upwards and there is enough space following it that it
		//	can be modified in-place.
		LSBOOL bResizedFreeBlockWillBeLargeEnough = bFollowingFreeSpaceCanHoldNewBytes &&
			(lpfhNextFree->sActualSize - (sNewSize - sOldSize) >= sizeof( LSA_FREEHEADER ));

		// If the resulting free space after this block will not be large enough for a
		//	LSA_FREEHEADER header, adjust the allocation length.
		if ( !bResizedFreeBlockWillBeLargeEnough ) {
			// Adjust the sizes to engulf the free block.
			_sSize = GetSize( lpahThis ) + lpfhNextFree->sActualSize;
			sNewSize = GetActualSizeOfBlock( reinterpret_cast<void *>(lpahThis), _sSize, lpahThis->ui32Alignment );

			// Erasing this block will cause pointers to it to point to whatever
			//	free block was before it.
			LPLSA_FREEHEADER lpfhPrev = lpfhNextFree->pfhPrevFree;
			LPLSA_ALLOCATIONHEADER lpahStart = GetNextAllocatedBlock( lpfhNextFree );

			RemFreeBlockInternal( lpfhNextFree );

			ReplaceAllocationFreeBlockPointers( lpahStart,
				lpfhNextFree, lpfhPrev );
		}

		lpahThis->ui32Size = CreateSizeKey( _sSize );

		
		// If we did not delete the following free-store block entirely, we need to resize it.
		if ( bResizedFreeBlockWillBeLargeEnough ) {
			// Adjust the pointer to the next free block to point to the next next block
			//	for allocation later.
			LPLSA_FREEHEADER lpfhFreeMe = lpfhNextFree;
			lpfhNextFree = lpfhNextFree->pfhNextFree;

			LSA_SIZE sNewSizeOfFreeBlock = lpfhFreeMe->sActualSize - (sNewSize - sOldSize);
			// RemFreeBlockInternal() will not adjust pointers on any trailing
			//	allocation blocks, but they will be adjusted on the following
			//	call to AddFreeBlock() since it will have the same end as this
			//	block.
			RemFreeBlockInternal( lpfhFreeMe );

			// Add the new free block at the end of this allocated block.
			AddFreeBlock( lpahThis->pfhPrevFree, lpfhNextFree,
				reinterpret_cast<void *>(sNewEnd), sNewSizeOfFreeBlock );
		}
		//PrintFreeBlocks();
#ifdef LSA_DEBUG
		/*static uint32_t ui32Count = 0;
		if ( ++ui32Count % 1 == 0 ) {
			VerifyFreeBlocks();
		}*/
#endif	// #ifdef LSA_DEBUG
		return _pvAddr;
	}

	// Get the allocation block previous to the given block, or NULL if there is none.
	CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER LSE_CALL CStdAllocatorHeap::GetPrevAlloc( LPLSA_ALLOCATIONHEADER _pahFrom ) const {
		// Any block that comes before this one must have a hash key equal to or lower than this one.
		// However this object may not inserted linearly yet, so using only the next pointer on
		//	each item is not sufficient.
		uint32_t ui32Key = GetHashKey( GetStartAddress( _pahFrom ) );
		LSA_SIZE sMyStart = reinterpret_cast<LSA_SIZE>(_pahFrom);
		for ( long I = static_cast<long>(ui32Key); I >= 0; --I ) {
			for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[I]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
				// If an item points directly at this object then we have a guaranteed winner.
				if ( lpahThis->pahNext == _pahFrom ) { return lpahThis; }
				// But this object may not be in the linear list yet, so we also have to check
				//	to see if the current object points beyond our object.  If so, this
				//	object is between that and its follower, thus it is the previous block
				//	from this one.
				LSA_SIZE sItsStart = reinterpret_cast<LSA_SIZE>(lpahThis);
				if ( sItsStart < sMyStart ) {
					LSA_SIZE sItsNext = reinterpret_cast<LSA_SIZE>(lpahThis->pahNext);
					// If its next pointer is beyond this one, this item follows it.
					// If its next pointer is NULL, there are no items beyond it and this one
					//	must therefore be beyond it.
					if ( !sItsNext || sItsNext > sMyStart ) { return lpahThis; }
				}
			}

		}

		return NULL;
	}

	// Add an allocation.  Does not remove free blocks or cut anything.  Assumes the address at _pvAddr
	//	is ready to go.
	void * LSE_CALL CStdAllocatorHeap::AddAllocInternal( LPLSA_FREEHEADER _lpfhPrevFree,
		LPLSA_ALLOCATIONHEADER _lpahNextAllocated,
		void * _pvAddr, LSA_SIZE _sSize, uint32_t _ui32Align LSA_DEBUGPARMSDEF ) {
		
		LPLSA_ALLOCATIONHEADER lpahThis = reinterpret_cast<LPLSA_ALLOCATIONHEADER>(_pvAddr);
		lpahThis->pfhPrevFree = _lpfhPrevFree;
		lpahThis->pahNext = _lpahNextAllocated;
		lpahThis->ui32Alignment = _ui32Align;
		lpahThis->ui32Size = CreateSizeKey( _sSize );
#ifdef LSA_DEBUG
		lpahThis->pcFile = _pcFile;
		lpahThis->ui32Line = _ui32Line;
		lpahThis->ui32Number = m_ui32AllocCount++;
		/*if ( lpahThis->ui32Number == 4996 ) {
			int gjhg = 0;
		}*/
#endif	// #ifdef LSA_DEBUG


		LPLSA_ALLOCATIONHEADER lpahPrevAlloc = GetPrevAlloc( lpahThis );
		if ( lpahPrevAlloc ) {
			assert( lpahPrevAlloc->pahNext == _lpahNextAllocated );
			lpahPrevAlloc->pahNext = lpahThis;
		}

		// Add it to the hash table based on address.
		void * pvAddr = GetStartAddress( lpahThis );
		uint32_t ui32Key = GetHashKey( pvAddr );
		lpahThis->pahNextInHash = m_lpahAllocatedBlocks[ui32Key];
		m_lpahAllocatedBlocks[ui32Key] = lpahThis;
		return pvAddr;
	}

	// Add an allocation at the given address.  The given address is assumed to be the header of a free
	//	block which must be adjusted or removed to make room for the allocation block.  Performs all
	//	necessary operations for adding the allocation block.
	void * LSE_CALL CStdAllocatorHeap::AddAlloc( LPLSA_ALLOCATIONHEADER _lpahNextAllocated,
		void * _pvAddr, LSA_SIZE _sSize, uint32_t _ui32Align LSA_DEBUGPARMSDEF ) {

		// The given address must be a free-store block.
		LPLSA_FREEHEADER pfhFreeBlock = reinterpret_cast<LPLSA_FREEHEADER>(_pvAddr);
		LPLSA_FREEHEADER pfhPrevFree = pfhFreeBlock->pfhPrevFree;
		LSA_SIZE sFullSize = GetActualSizeOfBlock( _pvAddr, _sSize, _ui32Align );
		// If the full size is adjusted (which can only be done upwards) we need to
		//	adjust the requested allocation size by the same amount.
		LSA_SIZE sOriginalFullSize = sFullSize;



		// If the size of this block nearly or fully overwrites the existing free-store block,
		//	remove the free-store block entirely.
		// If chopping the free-store block would make it so small that it could not fit its
		//	own header into its given space, remove it entirely and give it to the allocation
		//	block.
		// The size of the new free-store block, if any, would be its size minus the full size
		//	of this block.
		LSA_SIZE sNewSizeOfFreeBlock = pfhFreeBlock->sActualSize - sFullSize;
		LPLSA_FREEHEADER pfhFreeStoreFixup;
		
		if ( sNewSizeOfFreeBlock < sizeof( LSA_FREEHEADER ) ) {
			// Remove it entirely.
			// The new full size of the block will be the full length of the free-store block.
			sFullSize = pfhFreeBlock->sActualSize;

			pfhFreeStoreFixup = pfhFreeBlock->pfhPrevFree;
			RemFreeBlockInternal( pfhFreeBlock );
			ReplaceAllocationFreeBlockPointers( _lpahNextAllocated,
				pfhFreeBlock, pfhFreeStoreFixup );

		}
		else {
			// The left-over space is large enough for a free block, so
			//	just move the free-store block.
			// In 2 steps, remove it from here and add it to there.
			LPLSA_FREEHEADER lpfhPrev = pfhFreeBlock->pfhPrevFree;
			LPLSA_FREEHEADER lpfhNext = pfhFreeBlock->pfhNextFree;
			LSA_SIZE sNewAddr = reinterpret_cast<LSA_SIZE>(pfhFreeBlock);
			sNewAddr += sFullSize;
			void * pvNewAddress = reinterpret_cast<void *>(sNewAddr);
			
			// Remove from there.
			RemFreeBlockInternal( pfhFreeBlock );
			// Add to here.
			AddFreeBlock( lpfhPrev, lpfhNext,
				pvNewAddress, sNewSizeOfFreeBlock );
		}

		// Grow the requested size by the amount of extra free space we may have obtained.
		_sSize += (sFullSize - sOriginalFullSize);
#ifdef _DEBUG
		assert( GetActualSizeOfBlock( _pvAddr, _sSize, _ui32Align ) == sFullSize );
#endif	// #ifdef _DEBUG
		// A hole has been made.  Now add the allocation block.
		return AddAllocInternal( pfhPrevFree, _lpahNextAllocated,
			_pvAddr, _sSize, _ui32Align LSA_DEBUGPARMSPASS );
	}

	// Remove an allocation.  Does all merging and fixing, leaving in a stable state.
	void LSE_CALL CStdAllocatorHeap::RemAlloc( LPLSA_ALLOCATIONHEADER _lpahAllocated ) {
		// Get the full size of the block.
		LSA_SIZE sFullSize = GetActualSizeOfBlock( reinterpret_cast<void *>(_lpahAllocated), GetSize( _lpahAllocated ), _lpahAllocated->ui32Alignment );
		LPLSA_ALLOCATIONHEADER lpahPrev = GetPrevAlloc( _lpahAllocated );
		// Make the previous allocation point past this one.
		if ( lpahPrev ) {
			assert( lpahPrev->pahNext == _lpahAllocated );
			lpahPrev->pahNext = _lpahAllocated->pahNext;
		}

		// Remove it from the hash table.
		uint32_t ui32Key = GetHashKey( GetStartAddress( _lpahAllocated ) );
		// If it is at the front of the list, remove it from there.
		if ( m_lpahAllocatedBlocks[ui32Key] == _lpahAllocated ) {
			m_lpahAllocatedBlocks[ui32Key] = _lpahAllocated->pahNextInHash;
		}
		else {
			// Otherwise look for an item whose next pointer points to this.
			for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[ui32Key]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
				if ( lpahThis->pahNextInHash == _lpahAllocated ) {
					// Found.  Make it point to the next item instead of at this item.
					lpahThis->pahNextInHash = _lpahAllocated->pahNextInHash;
					break;
				}
			}
		}

		// It is now removed from all linked lists and can be discarded.
		// Add the address to the free store.
		LPLSA_FREEHEADER lpfhPrev = _lpahAllocated->pfhPrevFree;
		LPLSA_FREEHEADER lpfhNext = lpfhPrev ? lpfhPrev->pfhNextFree : m_lpfhFreeHead;
#ifdef LSA_DEBUG
		LSA_SIZE sPrev = reinterpret_cast<LSA_SIZE>(lpfhPrev);
		LSA_SIZE sNext = reinterpret_cast<LSA_SIZE>(lpfhNext);
		if ( sPrev && sNext ) {
			assert( sPrev < sNext );
		}
#endif	// #ifdef LSA_DEBUG

		_lpahAllocated->pahNext = reinterpret_cast<LPLSA_ALLOCATIONHEADER>(-1);
		_lpahAllocated->pahNextInHash = reinterpret_cast<LPLSA_ALLOCATIONHEADER>(-1);

		AddFreeBlock( lpfhPrev, lpfhNext, reinterpret_cast<void *>(_lpahAllocated), sFullSize );
	}

	// Add a free-store block without merging or fixing-up.
	void LSE_CALL CStdAllocatorHeap::AddFreeBlockInternal( LPLSA_FREEHEADER _lpfhAfterMe, LPLSA_FREEHEADER _lpfhBeforeMe,
		void * _pvAddr, LSA_SIZE _sSize ) {
		// Insert between the previous and next blocks by adjusting the linked-list pointers.
		LPLSA_FREEHEADER lpfhThis = reinterpret_cast<LPLSA_FREEHEADER>(_pvAddr);

		lpfhThis->pfhPrevFree = _lpfhAfterMe;
		lpfhThis->pfhNextFree = _lpfhBeforeMe;

		// If there is a previous.
		if ( lpfhThis->pfhPrevFree ) {
			lpfhThis->pfhPrevFree->pfhNextFree = lpfhThis;
		}
		if ( lpfhThis->pfhNextFree ) {
			lpfhThis->pfhNextFree->pfhPrevFree = lpfhThis;
		}

		// If there is no previous block, this is the new header.
		if ( !lpfhThis->pfhPrevFree ) {
			m_lpfhFreeHead = lpfhThis;
		}

		// Set the size of the block.
		// This measures the full space of the block.
		lpfhThis->sActualSize = _sSize;

		// Finally, add it to the hash bucket.
		uint32_t ui32HashKey = GetHashKey( _sSize );

		// Add it to the start of that bucket.
		lpfhThis->pfhNextInHash = m_lpfhFreeBlocks[ui32HashKey];
		m_lpfhFreeBlocks[ui32HashKey] = lpfhThis;

		// If it is lower than the lowest free block, adjust the index.
		if ( ui32HashKey < m_usLowestFreeBlockInHashTable ) {
			m_usLowestFreeBlockInHashTable = static_cast<uint16_t>(ui32HashKey);
		}

		return;
	}

	// Remove a free-store block.  Does not chop, merge, or fix-up.
	void LSE_CALL CStdAllocatorHeap::RemFreeBlockInternal( LPLSA_FREEHEADER _lpfhRemoveMe ) {
		// Remove it from the hash first.
		uint32_t ui32HashKey = GetHashKey( _lpfhRemoveMe->sActualSize );

		// If the first block points to this, adjust it.
		if ( m_lpfhFreeBlocks[ui32HashKey] == _lpfhRemoveMe ) {
			m_lpfhFreeBlocks[ui32HashKey] = _lpfhRemoveMe->pfhNextInHash;
		}
		else {
			// Otherwise use the next pointers to find and remove it.
			for ( LPLSA_FREEHEADER lpfhThis = m_lpfhFreeBlocks[ui32HashKey];
				lpfhThis;
				lpfhThis = lpfhThis->pfhNextInHash ) {
				// If the next item is this.
				if ( lpfhThis->pfhNextInHash == _lpfhRemoveMe ) {
					lpfhThis->pfhNextInHash = _lpfhRemoveMe->pfhNextInHash;
					break;
				}
			}
		}

		// If this list in the hash table is empty, and this was the previous lowest index (m_usLowestFreeBlockInHashTable),
		//	redetermine what m_usLowestFreeBlockInHashTable should be.
		if ( ui32HashKey == m_usLowestFreeBlockInHashTable && !m_lpfhFreeBlocks[ui32HashKey] ) {
			for ( ; m_usLowestFreeBlockInHashTable < LSA_BUCKETS; ++m_usLowestFreeBlockInHashTable ) {
				if ( m_lpfhFreeBlocks[m_usLowestFreeBlockInHashTable] ) { break; }
			}
		}


		// Just for security reasons.
		_lpfhRemoveMe->pfhNextInHash = NULL;

		// Now fix up the linked lists.
		if ( _lpfhRemoveMe->pfhNextFree ) {
			_lpfhRemoveMe->pfhNextFree->pfhPrevFree = _lpfhRemoveMe->pfhPrevFree;
		}
		if ( _lpfhRemoveMe->pfhPrevFree ) {
			_lpfhRemoveMe->pfhPrevFree->pfhNextFree = _lpfhRemoveMe->pfhNextFree;
		}


		// If this is the root node (has no previous pointer), adjust it to point to the new start of the list.
		// It may be NULL also.
		if ( !_lpfhRemoveMe->pfhPrevFree ) {
			m_lpfhFreeHead = _lpfhRemoveMe->pfhNextFree;
		}
	}

	// Add a free-store block.  Performs merging and fixing.
	void LSE_CALL CStdAllocatorHeap::AddFreeBlock( LPLSA_FREEHEADER _lpfhAfterMe, LPLSA_FREEHEADER _lpfhBeforeMe,
		void * _pvAddr, LSA_SIZE _sSize ) {
#ifdef LSA_DEBUG
#ifdef _DEBUG
		LSA_SIZE sPrev = reinterpret_cast<LSA_SIZE>(_lpfhAfterMe);
		LSA_SIZE sNext = reinterpret_cast<LSA_SIZE>(_lpfhBeforeMe);
		LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(_pvAddr);

		assert( sThis > sPrev );
		if ( sNext ) {
			assert( sThis < sNext );
		}
		assert( sThis >= reinterpret_cast<LSA_SIZE>(m_pvHeap) );
		assert( sThis + sizeof( LSA_FREEHEADER ) <= reinterpret_cast<LSA_SIZE>(m_pvHeap) + m_sSize );
		assert( sThis + _sSize <= reinterpret_cast<LSA_SIZE>(m_pvHeap) + m_sSize );
		
		
#endif	// #ifdef _DEBUG
#endif	// #ifdef LSA_DEBUG
		// Add the block.
		AddFreeBlockInternal( _lpfhAfterMe, _lpfhBeforeMe,
			_pvAddr, _sSize );
		// Merge.
		LPLSA_FREEHEADER lpfhThis = reinterpret_cast<LPLSA_FREEHEADER>(_pvAddr);
		MergeFreeStoreBlocks( lpfhThis );
		if ( MergeFreeStoreBlocks( _lpfhAfterMe ) ) {
			lpfhThis = _lpfhAfterMe;
		}

		// Fix pointers on the allocation blocks that need to point to the previous free block.
		FixUpAllocationFreeBlockPointers( lpfhThis );

#ifdef LSA_DEBUG
#ifdef _DEBUG
		//VerifyFreeBlocks();
#endif	// #ifdef _DEBUG
#endif	// #ifdef LSA_DEBUG
	}

	// Merge connected free-store blocks.
	LSBOOL LSE_CALL CStdAllocatorHeap::MergeFreeStoreBlocks( LPLSA_FREEHEADER _lpfhFirst ) {
		// If there is no block, do nothing.
		if ( !_lpfhFirst ) { return false; }
		// If there is no block after this, nothing we can merge.
		if ( !_lpfhFirst->pfhNextFree ) { return false; }
		// If the next block immediately touches the given block, merge them.
		LSA_SIZE sEndOfThis = reinterpret_cast<LSA_SIZE>(_lpfhFirst);
		sEndOfThis += _lpfhFirst->sActualSize;

		LSA_SIZE sNext = reinterpret_cast<LSA_SIZE>(_lpfhFirst->pfhNextFree);

		// If the end of this block is the start of the next block.
		if ( sEndOfThis == sNext ) {
			// The new size of the block will be the combined sizes of these blocks.
			LSA_SIZE sNewSize = _lpfhFirst->sActualSize + _lpfhFirst->pfhNextFree->sActualSize;
			// The address of the block will be the same as what was passed to us.
			void * pvNewAddr = _lpfhFirst;
			// The block before this will be the same as it is now.
			LPLSA_FREEHEADER lpfhPrev = _lpfhFirst->pfhPrevFree;
			// The block after this, after removing the next block, will be the next block after the next block.
			LPLSA_FREEHEADER lpfhNext = _lpfhFirst->pfhNextFree->pfhNextFree;
			// Remove both blocks.
			RemFreeBlockInternal( _lpfhFirst->pfhNextFree );
			RemFreeBlockInternal( _lpfhFirst );

			// Add the new block.
			AddFreeBlockInternal( lpfhPrev, lpfhNext,
				pvNewAddr, sNewSize );
			
			return true;
		}
		return false;
	}
	
	// Find a free-store block large enough for a given allocation size and alignment.
	CStdAllocatorHeap::LPLSA_FREEHEADER LSE_CALL CStdAllocatorHeap::FindFreeStoreBlock( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		LPLSA_FREEHEADER fhRet = NULL;

		// We search the bucket for the best-fitting free block in order to decrease chunking.
		// However a full search of the bucket takes a lot of time and slows allocation noticeably.  We do a partial
		//	search using ui32Count.  When ui32Count reaches 0, the best block found so far is returned.
		LSA_SIZE sLowestSize = 0;

		// The low-end devices require the full bucket search in order to save RAM.
		// We cannot use this optimization on iPhone.
#ifndef LSE_IPHONE
		uint32_t ui32Count = 8;
#endif	// #ifndef LSE_IPHONE

		// For every bucket capable of storing something of this size.
		uint32_t ui32Key = GetHashKey( _sSize );

		// If there are no buckets below this point do not start there.
		if ( ui32Key < m_usLowestFreeBlockInHashTable ) {
			ui32Key = m_usLowestFreeBlockInHashTable;
		}
		for ( ; ui32Key < LSA_BUCKETS; ++ui32Key ) {
			// For every free-store table in that bucket.
			for ( LPLSA_FREEHEADER lpfhThis = m_lpfhFreeBlocks[ui32Key]; lpfhThis; lpfhThis = lpfhThis->pfhNextInHash ) {
				// If this address could contain the allocation, use it.
				LSA_SIZE sSizeIfPlacedHere = GetActualSizeOfBlock( reinterpret_cast<void *>(lpfhThis), _sSize, _ui32Align );
				/*
				if ( sSizeIfPlacedHere <= lpfhThis->sActualSize ) {
					return lpfhThis;
				}
				*/

				// If the fit is perfect, use it.
				if ( sSizeIfPlacedHere == lpfhThis->sActualSize ) {
					return lpfhThis;
				}
				// Otherwise find the best fit to avoid fragmentation.
				if ( sSizeIfPlacedHere < lpfhThis->sActualSize ) {
					if ( lpfhThis->sActualSize < sLowestSize || !fhRet ) {
						fhRet = lpfhThis;
						sLowestSize = lpfhThis->sActualSize;
					}
				}
				
#ifndef LSE_IPHONE
				// If a potential heap has been found, avoid an exhaustive search.
				if ( fhRet ) {
					if ( --ui32Count == 0 ) {
						return fhRet;
					}
				}
#endif	// #ifndef LSE_IPHONE

			}
			
			// Do not go to the next bucket if we already have a candidate.
			if ( fhRet ) { return fhRet; }
		}
		// Unable to find anything.
		return fhRet;
	}

#ifdef LSA_DEBUG
	// Print free blocks.
	void LSE_CALL CStdAllocatorHeap::PrintFreeBlocks() const {
		for ( LPLSA_FREEHEADER lpfhThis = m_lpfhFreeHead; lpfhThis; lpfhThis = lpfhThis->pfhNextFree ) {
			// Does not need to be fancy.
			char szBuffer[128];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%p %u -> ", lpfhThis, lpfhThis->sActualSize );
			CStd::DebugPrintA( szBuffer );
		}
		CStd::DebugPrintA( "\r\n" );
	}
	
	// Verify free blocks.
	void LSE_CALL CStdAllocatorHeap::VerifyFreeBlocks() const {
#ifdef _DEBUG
		LSA_SIZE sHead = reinterpret_cast<LSA_SIZE>(m_lpfhFreeHead);
		
		for ( LPLSA_FREEHEADER lpfhThis = m_lpfhFreeHead; lpfhThis; lpfhThis = lpfhThis->pfhNextFree ) {
			assert( lpfhThis != lpfhThis->pfhNextFree );

			LSA_SIZE sPrev = reinterpret_cast<LSA_SIZE>(lpfhThis->pfhPrevFree);
			LSA_SIZE sNext = reinterpret_cast<LSA_SIZE>(lpfhThis->pfhNextFree);
			if ( sPrev && sNext ) {
				assert( sPrev < sNext );
			}
			
			LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(lpfhThis);
			if ( sPrev ) {
				assert( sPrev < sThis );
			}
			if ( sNext ) {
				assert( sNext > sThis + lpfhThis->sActualSize );
			}
			assert( sThis >= sHead );
			
			LSA_SIZE sThisEnd = sThis + lpfhThis->sActualSize;
			assert( sThis >= reinterpret_cast<LSA_SIZE>(m_pvHeap) );
			assert( sThisEnd <= reinterpret_cast<LSA_SIZE>(m_pvHeap) + m_sSize );
			
			
			LSBOOL bSomethingTouchesThisTail = false;
			LSA_SIZE sTail = reinterpret_cast<LSA_SIZE>(GetNextAllocatedBlock( lpfhThis ));
			if ( !sTail ) {
				bSomethingTouchesThisTail = true;
			}
			for ( LPLSA_ALLOCATIONHEADER lpahThis = GetNextAllocatedBlock( lpfhThis ); lpahThis; lpahThis = lpahThis->pahNext ) {
				LSA_SIZE sThisAlloc = reinterpret_cast<LSA_SIZE>(lpahThis);
				assert( sThisAlloc > sThis );
				
				if ( !bSomethingTouchesThisTail ) {
					if ( sThisAlloc == sTail ) {
						bSomethingTouchesThisTail = true;
					}
				}
				
				if ( sNext ) {
					if ( sThisAlloc >= sNext ) { break; }
				}
				assert( lpahThis->pfhPrevFree == lpfhThis );
				
			}
			
			assert( bSomethingTouchesThisTail );
		}
#endif	// #ifdef _DEBUG
		VerifyAllocatedBlocks();
	}
	
	// Verify allocated blocks.
	void LSE_CALL CStdAllocatorHeap::VerifyAllocatedBlocks() const {
#ifdef _DEBUG
		LSA_SIZE sFreeHead = reinterpret_cast<LSA_SIZE>(m_lpfhFreeHead);
		uint32_t ui32Total = 0;
		for ( LPLSA_ALLOCATIONHEADER lpahThis = GetAllocationHead(); lpahThis; lpahThis = lpahThis->pahNext ) {
			assert( lpahThis != lpahThis->pahNext );
			ui32Total++;
		}
		uint32_t ui32NewTotal = 0;
		for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
			for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[I]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
				assert( lpahThis != lpahThis->pahNext );
				LSA_SIZE sNext = reinterpret_cast<LSA_SIZE>(lpahThis->pahNext);
				LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(lpahThis);
				if ( sNext ) {
					assert( sNext > sThis );
				}
				
				
				LSA_SIZE sPrevFree = reinterpret_cast<LSA_SIZE>(lpahThis->pfhPrevFree);
				assert( sPrevFree < sThis );
				if ( sThis > sFreeHead ) {
					assert( sPrevFree >= sFreeHead );
				}
				
				LPLSA_ALLOCATIONHEADER lpahPrev = GetPrevAlloc( lpahThis );
				if ( lpahPrev ) {
					assert( lpahPrev->pahNext == lpahThis );
				}
				
				LSA_SIZE sThisEnd = sThis + GetSize( lpahThis );
				assert( sThis >= reinterpret_cast<LSA_SIZE>(m_pvHeap) );
				assert( sThisEnd <= reinterpret_cast<LSA_SIZE>(m_pvHeap) + m_sSize );
				
				++ui32NewTotal;
			}
		}
		//assert( ui32NewTotal == ui32Total );
#endif	// #ifdef _DEBUG
	}

	/**
	 * Print an allocation's information to the debug console.
	 *
	 * \param _lpahHeader The allocation whose information is to be printed.
	 */
	void LSE_CALL CStdAllocatorHeap::PrintAllocation( const LPLSA_ALLOCATIONHEADER _lpahHeader ) {
		// Does not need to be fancy.
		// Avoid allocating anything while printing this message.  Use a bunch of debug prints.
		char szBuffer[32];
		CStd::DebugPrintA( "File " );
		CStd::DebugPrintA( _lpahHeader->pcFile ? _lpahHeader->pcFile : "<Unknown File>" );
		CStd::DebugPrintA( " line " );
		CStd::DebugPrintA( CStd::UI32toA( _lpahHeader->ui32Line, szBuffer, 10 ) );
		CStd::DebugPrintA( " (#" );
		CStd::DebugPrintA( CStd::UI32toA( _lpahHeader->ui32Number, szBuffer, 10 ) );
		CStd::DebugPrintA( ") leaked ~" );
		CStd::DebugPrintA( CStd::UI32toA( static_cast<uint32_t>(GetSize( _lpahHeader )), szBuffer, 10 ) );
		CStd::DebugPrintA( " bytes at " );
		CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ),
#if defined( MACOS_X ) || defined( __APPLE__ )
			// Poor Macintosh computers cannot handle 64-bit values?
			"%.8X", static_cast<uint32_t>(reinterpret_cast<uintptr_t>(GetStartAddress( _lpahHeader ))) );
#else	// // #if defined( MACOS_X ) || defined( __APPLE__ )
			"%.8I64X", static_cast<uint64_t>(reinterpret_cast<uintptr_t>(GetStartAddress( _lpahHeader ))) );
#endif	// #if defined( MACOS_X ) || defined( __APPLE__ )
		CStd::DebugPrintA( szBuffer );
		CStd::DebugPrintA( ".\r\n" );
	}
#endif	// #ifdef LSA_DEBUG
	
}	// namespace lsa
