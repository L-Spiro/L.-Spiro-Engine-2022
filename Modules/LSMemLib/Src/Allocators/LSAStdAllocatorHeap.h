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


#ifndef __LSA_STDALLOCATORHEAP_H__
#define __LSA_STDALLOCATORHEAP_H__

#include "../OSHeap/LSAOsHeap.h"
#if defined( LSA_USE_FLOAT_OPTIMIZATIONS )
#include <cmath>
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS


namespace lsa {

	/**
	 * Class CStdAllocatorHeap
	 *
	 * Description: Manage a single block of RAM.
	 */
	class CStdAllocatorHeap {
		// Only the CStdAllocator class can create this object.
		friend class									CStdAllocator;

	public :
		// == Functions.
		/**
		 * Allocates bytes.
		 *
		 * \param _sSize Number of bytes to allocate.
		 * \param _ui32Align Alignment of the allocation.
		 * \return Returns the allocated bytes or NULL.
		 */
		void * LSE_CALL								Alloc( LSA_SIZE _sSize, uint32_t _ui32Align = LSA_MIN_ALIGN						LSA_DEBUGPARMSDECL );

		/**
		 * Free bytes.
		 *
		 * \param _pvAddr Address to free.
		 * \return Returns true if the address was allocated by this heap object.
		 */
		LSBOOL LSE_CALL									Free( void * _pvAddr );

		/**
		 * Are we empty?  In debug mode it also prints files and line numbers of remaining allocations.
		 *
		 * \param _bReport If true, non-empty blocks are printed to the debug stream.
		 * \return Returns true if the heap is empty.
		 */
		LSBOOL LSE_CALL									IsEmpty( LSBOOL _bReport ) const;

		/**
		 * Trash the contents of the block.  Resets it to a clean slate ready for allocation.
		 */
		void LSE_CALL									Trash();

		/**
		 * Print allocations with an allocation counter above or equal to the given number.  If LSA_DEBUG is
		 *	not defined, this does nothing.
		 *
		 * \param _ui32Count Allocations with an allocation number above or equal to this value will be printed.
		 *	Use 0 to print all allocations.
		 * \param _ui32End Ending allocation number up to which to print allocations.
		 * \return Returns the combined size of the allocations printed.
		 */
		LSA_SIZE LSE_CALL								PrintAllocations( uint32_t _ui32Count = 0, uint32_t _ui32End = 0xFFFFFFFFUL ) const;

		/**
		 * Gets the size of the heap.  This is the entire size of the heap, not just the amount of RAM allocated from the heap.
		 *
		 * \return Returns the full size of this heap.
		 */
		LSE_INLINE LSA_SIZE LSE_CALL					GetHeapSize() const;

		/**
		 * Gets the allocation counter.  Used for debugging.  If LSA_DEBUG is not defined, this always returns 0.
		 *	This does not return the actual number of allocations.  It returns the unique allocation count to
		 *	be assigned to the next allocation.
		 *
		 * \return Returns the allocation counter, or 0 if LSA_DEBUG is not defined.
		 */
		static uint32_t LSE_CALL						GetAllocationCounter();


	protected :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CStdAllocatorHeap();

	protected :
		// == Types.
		/** A free-store header.  This tells gives us information regarding free blocks of memory. */
#pragma pack( push, 1 )
		typedef struct LSA_FREEHEADER {
			/**
			 * Pointer to the next free block.
			 */
			LSA_FREEHEADER *							pfhNextFree;

			/**
			 * Pointer to the previous free block.
			 */
			LSA_FREEHEADER *							pfhPrevFree;

			/**
			 * Pointer to the next free block in the same hash bucket.
			 */
			LSA_FREEHEADER *							pfhNextInHash;

			/**
			 * Actual size of the block.
			 */
			LSA_SIZE									sActualSize;
		} * LPLSA_FREEHEADER, * const LPCLSA_FREEHEADER;
#pragma pack( pop )

		/** An allocation header.  Without debugging information, it should be smaller than
		 *	or equal to the alignment (LSA_MIN_ALIGN). */
#pragma pack( push, 1 )
		typedef struct LSA_ALLOCATIONHEADER {
			/** Pointer to the previous free block. */
			LPLSA_FREEHEADER							pfhPrevFree;
			
			/** Pointer to the next allocated block linearly in RAM. */
			LSA_ALLOCATIONHEADER *						pahNext;
			
			/** Pointer to the next allocated block with the same hash index. */
			LSA_ALLOCATIONHEADER *						pahNextInHash;
			
			/** Information regarding this block.
			 * Size of the block in units of LSA_MIN_ALIGN.  This gives us a maximum allocation of
			 *	32 gigabytes per block for 16-byte alignment. */
			uint32_t									ui32Size		: 31;
			
			/** Alignment = 0 or 1, for LSA_MIN_ALIGN or (LSA_MIN_ALIGN*2). */
			uint32_t									ui32Alignment	: 1;

			// Debug information, if any.
#ifdef LSA_DEBUG
			/** Line in the file. */
			uint32_t									ui32Line;
			
			/** Name of the file. */
			const char *								pcFile;
			
			/** Allocation number. */
			uint32_t									ui32Number;
#endif	// #ifdef LSA_DEBUG
		} * LPLSA_ALLOCATIONHEADER, * const LPCLSA_ALLOCATIONHEADER;
#pragma pack( pop )


	protected :
		// == Functions.
		/**
		 * Sets the starting point and size of the heap.
		 *
		 * \param _pvHeap Heap starting address.
		 * \param _sSizeOfHeap Size of the heap.
		 */
		LSE_INLINE void LSE_CALL						Set( void * _pvHeap, LSA_SIZE _sSizeOfHeap );

		/**
		 * Adjust the length of an allocation.  If it cannot be adjusted in place it is copied to a new
		 *	location.  NULL is returned if it cannot be adjusted.
		 *
		 * \param _lpahHeader Header.
		 * \param _pvAddr Block to resize.
		 * \param _sSize New size of the block.
		 * \return Returns the new address of the allocation or NULL.
		 */
		void * LSE_CALL								ReAlloc( LPLSA_ALLOCATIONHEADER &_lpahHeader, void * _pvAddr, LSA_SIZE _sSize	LSA_DEBUGPARMSDECL );

		// Given the address of a block, get the address returned to the user.
		LSE_INLINE static void * LSE_CALL				GetStartAddress( const LPLSA_ALLOCATIONHEADER _lpahHeader );

		// Get the actual size of a block given the requested size, alignment, and start address.
		LSE_INLINE static LSA_SIZE LSE_CALL				GetActualSizeOfBlock( void * _pvAddr, LSA_SIZE _sSize, uint32_t _ui32Align );

		// Get the size of an allocated block.
		LSE_INLINE static LSA_SIZE LSE_CALL				GetSize( const LPLSA_ALLOCATIONHEADER _lpahHeader );

		// Given an address returned to the user, generate a hash key.
		LSE_INLINE uint32_t LSE_CALL					GetHashKey( void * _pvAddr ) const;

		// Given a size, generate a hash key.
		LSE_INLINE static uint32_t LSE_CALL				GetHashKey( LSA_SIZE _sSize );

		// Round up to the nearest alignment.
		LSE_INLINE static LSA_SIZE LSE_CALL				RoundUp( LSA_SIZE _sSize );

		// Create a size key for allocation blocks.
		LSE_INLINE static uint32_t LSE_CALL				CreateSizeKey( LSA_SIZE _sSize );

		// Get the alignment in decimal form given an alignment key.
		LSE_INLINE static uint32_t LSE_CALL				GetAlignment( uint32_t _ui32Key );

		// Get the allocation block previous to the given block, or NULL if there is none.
		LPLSA_ALLOCATIONHEADER LSE_CALL					GetPrevAlloc( LPLSA_ALLOCATIONHEADER _pahFrom ) const;

		// Add an allocation.  Does not remove free blocks or cut anything.  Assumes the address at _pvAddr
		//	is ready to go.
		void * LSE_CALL								AddAllocInternal( LPLSA_FREEHEADER _lpfhPrevFree,
			LPLSA_ALLOCATIONHEADER _lpahNextAllocated,
			void * _pvAddr, LSA_SIZE _sSize, uint32_t _ui32Align
			LSA_DEBUGPARMSDECL );

		// Add an allocation at the given address.  The given address is assumed to be the header of a free
		//	block which must be adjusted or removed to make room for the allocation block.  Performs all
		//	necessary operations for adding the allocation block.
		void * LSE_CALL								AddAlloc( LPLSA_ALLOCATIONHEADER _lpahNextAllocated,
			void * _pvAddr, LSA_SIZE _sSize, uint32_t _ui32Align
			LSA_DEBUGPARMSDECL );

		// Remove an allocation.  Does all merging and fixing, leaving in a stable state.
		void LSE_CALL									RemAlloc( LPLSA_ALLOCATIONHEADER _lpahAllocated );

		// Add a free-store block without merging or fixing-up.
		void LSE_CALL									AddFreeBlockInternal( LPLSA_FREEHEADER _lpfhAfterMe, LPLSA_FREEHEADER _lpfhBeforeMe,
			void * _pvAddr, LSA_SIZE _sSize );

		// Remove a free-store block.  Does not chop, merge, or fix-up.
		void LSE_CALL									RemFreeBlockInternal( LPLSA_FREEHEADER _lpfhRemoveMe );

		// Add a free-store block.  Performs merging and fixing.
		void LSE_CALL									AddFreeBlock( LPLSA_FREEHEADER _lpfhAfterMe, LPLSA_FREEHEADER _lpfhBeforeMe,
			void * _pvAddr, LSA_SIZE _sSize );

		// Merge connected free-store blocks.
		LSBOOL LSE_CALL									MergeFreeStoreBlocks( LPLSA_FREEHEADER _lpfhFirst );

		// Get the head of the allocation list.
		LSE_INLINE LPLSA_ALLOCATIONHEADER LSE_CALL		GetAllocationHead() const;

		// Get the next allocated block after a free block or NULL if there are none.
		LSE_INLINE LPLSA_ALLOCATIONHEADER LSE_CALL		GetNextAllocatedBlock( LPLSA_FREEHEADER _lpAfterMe ) const;

		// Get the next free block after the given allocated block, or NULL for none.
		LSE_INLINE LPLSA_FREEHEADER LSE_CALL			GetNextFreeBlock( LPLSA_ALLOCATIONHEADER _lpahAfterMe );

		// Fix up the free pointers on allocated blocks following a free block.
		LSE_INLINE void LSE_CALL						FixUpAllocationFreeBlockPointers( LPLSA_FREEHEADER _lpfhAfterMe );

		// In a linked-list of allocated blocks, replace each elementís previous free pointer with the one given
		//	if it matches the provided original free-block pointer.  Stops scanning at the first block that does
		//	not match the given key.
		LSE_INLINE void LSE_CALL						ReplaceAllocationFreeBlockPointers( LPLSA_ALLOCATIONHEADER _lpahStart, LPLSA_FREEHEADER _lpfhReplaceMe, LPLSA_FREEHEADER _lpfhWithMe );

		// Find a free-store block large enough for a given allocation size and alignment.
		LPLSA_FREEHEADER LSE_CALL						FindFreeStoreBlock( LSA_SIZE _sSize, uint32_t _ui32Align );

		// Find an allocated block given its address.
		LSE_INLINE LPLSA_ALLOCATIONHEADER LSE_CALL		FindAllocationBlock( void * _pvAddr );

		// Additional debug functions.
#ifdef LSA_DEBUG
		// Print free blocks.
		void LSE_CALL									PrintFreeBlocks() const;

		// Verify free blocks.
		void LSE_CALL									VerifyFreeBlocks() const;

		// Verify allocated blocks.
		void LSE_CALL									VerifyAllocatedBlocks() const;

		/**
		 * Print an allocation's information to the debug console.
		 *
		 * \param _lpahHeader The allocation whose information is to be printed.
		 */
		static void LSE_CALL							PrintAllocation( const LPLSA_ALLOCATIONHEADER _lpahHeader );
#endif	// #ifdef LSA_DEBUG

		// == Members.
		/**
		 * Pointer to the start of the block of memory.
		 */
		void *										m_pvHeap;
		
		/**
		 * Size of the block.
		 */
		LSA_SIZE										m_sSize;

		/**
		 * Next block in the linked list of blocks.
		 */
		CStdAllocatorHeap *								m_psahNext;

		/**
		 * Hash table for allocated blocks.
		 */
		LPLSA_ALLOCATIONHEADER							m_lpahAllocatedBlocks[LSA_BUCKETS];

		/**
		 * Hash table for free blocks.
		 */
		LPLSA_FREEHEADER								m_lpfhFreeBlocks[LSA_BUCKETS];

		/**
		 * Head of the linked list of free blocks.
		 */
		LPLSA_FREEHEADER								m_lpfhFreeHead;

		/**
		 * Index of the first free block in the hash table.
		 */
		uint16_t										m_usLowestFreeBlockInHashTable;

#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		/**
		 * The inverse size of the heap.
		 */
		double										m_dInvSize;
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS

#ifdef LSA_DEBUG
		/**
		 * Allocation number given to every allocation.
		 */
		static uint32_t									m_ui32AllocCount;
#endif	// #ifdef LSA_DEBUG

#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		/**
		 * Have we made the sin table?
		 */
		static LSBOOL									m_bMadeTable;

		/**
		 * The sin table.
		 */
		static double									m_dSinTable[0x10000];
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CStdAllocatorHeap::CStdAllocatorHeap() :
#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		m_dInvSize( 0.0 ),
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		m_sSize( 0 ),
		m_pvHeap( NULL ),
		//m_lpahAllocatedHead( NULL ),
		m_lpfhFreeHead( NULL ),
		m_psahNext( NULL ),
		m_usLowestFreeBlockInHashTable( LSA_BUCKETS - 1 ) {
		for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
			m_lpahAllocatedBlocks[I] = NULL;
			m_lpfhFreeBlocks[I] = NULL;
		}
#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		if ( !m_bMadeTable ) {
			for ( int32_t I = 0x10000; --I >= 0; ) {
				m_dSinTable[I] = ::sin( (2.0 * 3.1415926535897932384626433832795 / 0x10000) * I );
			}
			m_bMadeTable = true;
		}
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
	}

	// == Functions.
	/**
	 * Gets the size of the heap.  This is the entire size of the heap, not just the amount of RAM allocated from the heap.
	 *
	 * \return Returns the full size of this heap.
	 */
	LSE_INLINE LSA_SIZE LSE_CALL CStdAllocatorHeap::GetHeapSize() const {
		return m_sSize;
	}

	/**
	 * Sets the starting point and size of the heap.
	 *
	 * \param _pvHeap Heap starting address.
	 * \param _sSizeOfHeap Size of the heap.
	 */
	LSE_INLINE void LSE_CALL CStdAllocatorHeap::Set( void * _pvHeap, LSA_SIZE _sSizeOfHeap ) {
		m_pvHeap = _pvHeap;
		m_sSize = _sSizeOfHeap;
#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		m_dInvSize = 1.0 / m_sSize;
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		//m_lpahAllocatedHead = NULL;
		m_lpfhFreeHead = NULL;
		m_psahNext = NULL;
		for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
			m_lpahAllocatedBlocks[I] = NULL;
			m_lpfhFreeBlocks[I] = NULL;
		}

		m_usLowestFreeBlockInHashTable = LSA_BUCKETS - 1;

		// Add the whole block to the free store.
		AddFreeBlock( NULL, NULL, m_pvHeap, m_sSize );
	}

	// Given the address of a block, get the address returned to the user.
	LSE_INLINE void * CStdAllocatorHeap::GetStartAddress( const LPLSA_ALLOCATIONHEADER _lpahHeader ) {
		LSA_SIZE sStart = reinterpret_cast<LSA_SIZE>(_lpahHeader);

		// Skip the header.
		sStart += sizeof( LSA_ALLOCATIONHEADER );
		// From here, round up based on the alignment.
		uint32_t ui32Align = LSA_MIN_ALIGN + LSA_MIN_ALIGN * _lpahHeader->ui32Alignment;
		return reinterpret_cast<void *>(LSE_ROUND_UP( sStart, ui32Align ));
	}

	// Get the actual size of a block given the requested size, alignment, and start address.
	LSE_INLINE LSA_SIZE LSE_CALL CStdAllocatorHeap::GetActualSizeOfBlock( void * _pvAddr, LSA_SIZE _sSize, uint32_t _ui32Align ) {
		LSA_SIZE sStart = reinterpret_cast<LSA_SIZE>(_pvAddr);
		LSA_SIZE sOriginal = sStart;
		// Skip the header.
		sStart += sizeof( LSA_ALLOCATIONHEADER );
		// From here, round up based on the alignment.
		uint32_t ui32Align = LSA_MIN_ALIGN + LSA_MIN_ALIGN * _ui32Align;

		sStart = LSE_ROUND_UP( sStart, ui32Align );

		// Now add the requested allocation length.
		sStart += _sSize;

		sStart = LSE_ROUND_UP( sStart, LSA_MIN_ALIGN );

		return sStart - sOriginal;
	}

	// Get the size of an allocated block.
	LSE_INLINE LSA_SIZE CStdAllocatorHeap::GetSize( const LPLSA_ALLOCATIONHEADER _lpahHeader ) {
		return _lpahHeader->ui32Size * LSA_MIN_ALIGN;
	}

	// Given an address returned to the user, generate a hash key.
	LSE_INLINE uint32_t CStdAllocatorHeap::GetHashKey( void * _pvAddr ) const {
		LSA_SUPERSIZE sStart = reinterpret_cast<LSA_SIZE>(_pvAddr);
		LSA_SUPERSIZE sBase = reinterpret_cast<LSA_SIZE>(m_pvHeap);
		sStart -= sBase;
#ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		double dFrac = sStart * m_dInvSize;
		dFrac *= static_cast<double>(1.5707963267948966192313216916398);
		return static_cast<uint32_t>(m_dSinTable[static_cast<uint16_t>(1.0/(3.1415926535897932384626433832795*2.0)*0x10000*dFrac)] * (LSA_BUCKETS - 1));
#else	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
		sStart = sStart * LSA_BUCKETS / m_sSize;
		return static_cast<uint32_t>(sStart);
#endif	// #ifdef LSA_USE_FLOAT_OPTIMIZATIONS
	}

	// Given a size, generate a hash key.
	LSE_INLINE uint32_t CStdAllocatorHeap::GetHashKey( LSA_SIZE _sSize ) {
		_sSize >>= 12;
		if ( _sSize < LSA_BUCKETS ) { return static_cast<uint32_t>(_sSize); }
		return LSA_BUCKETS - 1;
	}

	// Round up to the nearest alignment.
	LSE_INLINE LSA_SIZE LSE_CALL CStdAllocatorHeap::RoundUp( LSA_SIZE _sSize ) {
		LSA_SIZE sThis = _sSize >> LSA_MIN_ALIGN_BITS;
		if ( (_sSize & (LSA_MIN_ALIGN - 1)) ) {
			return (sThis + 1) << LSA_MIN_ALIGN_BITS;
		}
		return sThis << LSA_MIN_ALIGN_BITS;
	}

	// Create a size key for allocation blocks.
	LSE_INLINE uint32_t LSE_CALL CStdAllocatorHeap::CreateSizeKey( LSA_SIZE _sSize ) {
		LSA_SIZE sThis = _sSize >> LSA_MIN_ALIGN_BITS;
		if ( (_sSize & (LSA_MIN_ALIGN - 1)) ) {
			return static_cast<uint32_t>(sThis + 1);
		}
		return static_cast<uint32_t>(sThis);
		//return static_cast<uint32_t>(RoundUp( _sSize ) >> LSA_MIN_ALIGN_BITS);
	}

	// Get the alignment in decimal form given an alignment key.
	LSE_INLINE uint32_t LSE_CALL CStdAllocatorHeap::GetAlignment( uint32_t _ui32Key ) {
		return LSA_MIN_ALIGN + LSA_MIN_ALIGN * _ui32Key;
	}

	// Get the head of the allocation list.
	LSE_INLINE CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER LSE_CALL CStdAllocatorHeap::GetAllocationHead() const {
		if ( m_lpfhFreeHead == m_pvHeap ) {
			return GetNextAllocatedBlock( m_lpfhFreeHead );
		}
		return static_cast<LPLSA_ALLOCATIONHEADER>(m_pvHeap);
	}

	// Get the next allocated block after a free block or NULL if there are none.
	LSE_INLINE CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER LSE_CALL CStdAllocatorHeap::GetNextAllocatedBlock( LPLSA_FREEHEADER _lpAfterMe ) const {
		LSA_SIZE sStart = reinterpret_cast<LSA_SIZE>(_lpAfterMe);
		sStart += _lpAfterMe->sActualSize;
		LSA_SIZE sOurEnd = reinterpret_cast<LSA_SIZE>(m_pvHeap);

		// If it points to the end of the block or beyond there is nothing there.
		if ( sStart >= (sOurEnd + m_sSize) ) {
			return NULL;
		}
		return reinterpret_cast<LPLSA_ALLOCATIONHEADER>(sStart);
	}

	// Get the next free block after the given allocated block, or NULL for none.
	LSE_INLINE CStdAllocatorHeap::LPLSA_FREEHEADER LSE_CALL CStdAllocatorHeap::GetNextFreeBlock( LPLSA_ALLOCATIONHEADER _lpahAfterMe ) {
		// Go to the free block before this allocation.
		LPLSA_FREEHEADER lpfhThis = _lpahAfterMe->pfhPrevFree;
		if ( !lpfhThis ) {
			return m_lpfhFreeHead;
		}
		// Return the next free block after that.
		return lpfhThis->pfhNextFree;
	}

	// Fix up the free pointers on allocated blocks following a free block.
	LSE_INLINE void LSE_CALL CStdAllocatorHeap::FixUpAllocationFreeBlockPointers( LPLSA_FREEHEADER _lpfhAfterMe ) {
		//LSA_SIZE sIamAt = reinterpret_cast<LSA_SIZE>(_lpfhAfterMe);
		LSA_SIZE sEnd = reinterpret_cast<LSA_SIZE>(_lpfhAfterMe->pfhNextFree);
		if ( !sEnd ) {
			sEnd = static_cast<LSA_SIZE>(-1);
		}

		/*for ( uint32_t I = 0; I < LSA_BUCKETS; ++I ) {
			for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[I]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
				LSA_SIZE sThis = reinterpret_cast<LSA_SIZE>(lpahThis);
				if ( sThis > sIamAt && sThis < sEnd ) {
					lpahThis->pfhPrevFree = _lpfhAfterMe;
				}
			}
		}*/


		LPLSA_ALLOCATIONHEADER lpahStart;
		for ( lpahStart = GetNextAllocatedBlock( _lpfhAfterMe );
			lpahStart;
			lpahStart = lpahStart->pahNext ) {
			LSA_SIZE sItPointsTo = reinterpret_cast<LSA_SIZE>(lpahStart->pfhPrevFree);
			// If it points below us, we are the new closest.
			// Make it point at us.
			if ( sItPointsTo < sEnd ) {
				lpahStart->pfhPrevFree = _lpfhAfterMe;
			}
			else {
				break;
			}
		}
	}

	// In a linked-list of allocated blocks, replace each elementís previous free pointer with the one given
	//	if it matches the provided original free-block pointer.  Stops scanning at the first block that does
	//	not match the given key.
	LSE_INLINE void LSE_CALL CStdAllocatorHeap::ReplaceAllocationFreeBlockPointers( LPLSA_ALLOCATIONHEADER _lpahStart, LPLSA_FREEHEADER _lpfhReplaceMe, LPLSA_FREEHEADER _lpfhWithMe ) {
		for ( ; _lpahStart; _lpahStart = _lpahStart->pahNext ) {
			if ( _lpahStart->pfhPrevFree != _lpfhReplaceMe ) { return; }
			_lpahStart->pfhPrevFree = _lpfhWithMe;
		}
	}

	// Find an allocated block given its address.
	LSE_INLINE CStdAllocatorHeap::LPLSA_ALLOCATIONHEADER LSE_CALL CStdAllocatorHeap::FindAllocationBlock( void * _pvAddr ) {
		uint32_t ui32Key = GetHashKey( _pvAddr );
		// For every allocation in the hash table.
		for ( LPLSA_ALLOCATIONHEADER lpahThis = m_lpahAllocatedBlocks[ui32Key]; lpahThis; lpahThis = lpahThis->pahNextInHash ) {
			if ( GetStartAddress( lpahThis ) == _pvAddr ) {
				return lpahThis;
			}
		}
		return NULL;
	}


}	// namespace lsa

#endif	// __LSA_STDALLOCATORHEAP_H__
