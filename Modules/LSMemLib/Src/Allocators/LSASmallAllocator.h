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
 * Description: Similar to CStdAllocatorHeap in purpose, but this manages only small allocations (256 bytes
 *	and below).  In CStdAllocatorHeap, every allocation, no matter how small, causes 16 extra bytes to be
 *	allocated as overhead for record keeping.  Because the allocations can be of variable sizes, values must
 *	be stored to indicate the size of each block.  Blocks can also be variable distances apart so linked-list
 *	pointers must be inserted.  In debug mode, 32 additional bytes are allocated for each allocation instead of
 *	16.
 * The CSmallAllocator handles small allocations separately to avoid all this needless overhead and not only
 *	improves allocation/deallocation speed but also improves RAM efficiency.  This is done by organizing the
 *	blocks into fixed-sized rows.  The sizes of the blocks are known already so we do not need to store this
 *	information.  The blocks are stacked and are fixed in size so we know where each block is.  A simple bit
 *	array tells us if a block is allocated or not.  Deallocation is exceptionally fast since we only need to
 *	take the address of the block minus the start of the blocks to get a bit index, then switch that bit to
 *	0.  Searching for free blocks is also fast because a search over the bit array can check 32 blocks at once.
 *	When an array of 32 bits is found where not all bits are set to 1, a binary bit scan finds a free block.
 */


#ifndef __LSA_SMALLALLOCATOR_H__
#define __LSA_SMALLALLOCATOR_H__

#include "../OSHeap/LSAOsHeap.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// The number of 256-byte blocks to use as a guideline for the sizes of the small allocators.
#define LSA_SMALL_ALLOCATOR_BLOCKS				64

// The smallest allocation size.
#define LSA_SMALLEST_ALLOCATION_BITS			LSA_MIN_ALIGN_BITS
#define LSA_SMALLEST_ALLOCATION_SIZE			(1 << LSA_SMALLEST_ALLOCATION_BITS)

// The largest allocation size.
#define LSA_LARGEST_ALLOCATION_SIZE				256

// Number of blocks in a full working set.
#define LSA_WS									(LSA_LARGEST_ALLOCATION_SIZE / LSA_SMALLEST_ALLOCATION_SIZE)

namespace lsa {

	/**
	 * Class CSmallAllocator
	 * \brief Provide a more efficient way of handling allocations of small sizes.
	 *
	 * Description: Similar to CStdAllocatorHeap in purpose, but this manages only small allocations (256 bytes
	 *	and below).  In CStdAllocatorHeap, every allocation, no matter how small, causes 16 extra bytes to be
	 *	allocated as overhead for record keeping.  Because the allocations can be of variable sizes, values must
	 *	be stored to indicate the size of each block.  Blocks can also be variable distances apart so linked-list
	 *	pointers must be inserted.  In debug mode, 32 additional bytes are allocated for each allocation instead of
	 *	16.
	 * The CSmallAllocator handles small allocations separately to avoid all this needless overhead and not only
	 *	improves allocation/deallocation speed but also improves RAM efficiency.  This is done by organizing the
	 *	blocks into fixed-sized rows.  The sizes of the blocks are known already so we do not need to store this
	 *	information.  The blocks are stacked and are fixed in size so we know where each block is.  A simple bit
	 *	array tells us if a block is allocated or not.  Deallocation is exceptionally fast since we only need to
	 *	take the address of the block minus the start of the blocks to get a bit index, then switch that bit to
	 *	0.  Searching for free blocks is also fast because a search over the bit array can check 32 blocks at once.
	 *	When an array of 32 bits is found where not all bits are set to 1, a binary bit scan finds a free block.
	 */
	class CSmallAllocator {
		// Only the CStdAllocator class can create this object.
		friend class CStdAllocator;

	protected :
		// == Various constructors.
		LSE_CALLCTOR							CSmallAllocator( void * _pvAddress, uint32_t _ui32Total256Blocks );


		// == Types.
		/**
		 * The bocks are stored as a pointer to the free/used key bits followed by a pointer to the
		 *	first block in the set.
		 */
		typedef struct LSA_SMALL_ALLOC_BLOCKSET {
			/**
			 * Pointer to the array of bits that determine which blocks are used.
			 * The bits are packed in sets of either 32 bits or 64 bits depending on the
			 *	operating system (remember to make separate builds for each operating system).
			 */
			LSA_SIZE *							psBits;

			/**
			 * The pointer to the first block in this set.
			 */
			uint8_t *							pui8Blocks;

			/**
			 * Number of blocks in this set.
			 */
			uint32_t							ui32Blocks;
		} * LPLSA_SMALL_ALLOC_BLOCKSET, * const LPCLSA_SMALL_ALLOC_BLOCKSET;


		// == Members.
		// The pointer to the next CSmallAllocator object in the linked list managed by our
		//	superiors.
		CSmallAllocator *						m_psaNext;

		// The arrays of pointers to the starts of each row of blocks for each given size.
		// We handle 16 sizes (from 16 bytes to 256 bytes).  These point to the first blocks
		//	in each size class.
		LSA_SMALL_ALLOC_BLOCKSET				m_pui8BlockStarts[LSA_WS];

		// The beginning address of our heap.
		LSA_SIZE								m_sLow;

		// The ending address of our heap.
		LSA_SIZE								m_sHi;


		// == Functions.
		/**
		 * Initialize this object at a given location.
		 *
		 * \param _pvAddress Address of the working set to use.
		 * \param _ui32Total256Blocks Total number of 256-byte-sized blocks to use in the working set.
		 */
		void LSE_CALL							Init( void * _pvAddress, uint32_t _ui32Total256Blocks );

		/**
		 * Allocates a number of bytes.  Returns the allocated address or NULL on error.  There is no debug
		 *	information to store.
		 *
		 * \param _sSize Size of the requested allocation.
		 * \param _ui32Align Requested alignment in bytes.
		 * \return Returns the allocated address or NULL on error.
		 */
		void * LSE_CALL						Alloc( LSA_SIZE _sSize, uint32_t _ui32Align = LSA_MIN_ALIGN );

		/**
		 * Free an address.
		 *
		 * \param _pvAddr Address to free.
		 * \return Returns true if the address was freed.  Returns false if the given address was not allocated
		 *	by this object.
		 */
		LSBOOL LSE_CALL							Free( void * _pvAddr );

		/**
		 * Reallocate a given address.
		 *
		 * \param _pvAddr The address to reallocate.  If NULL, this behaves the same way as Alloc().
		 * \param _sSize The new size of the item to allocate.
		 * \param _sOldSize If reallocation failed but the address is valid within our working set,
		 *	this receives the size of the data.  Only valid if this function returns NULL.
		 * \return Returns the new address of the item or NULL if none could be allocated.  If failure is
		 *	caused by a reason other than the given address not being part of our working set, _sOldSize
		 *	is filled with the size of the data at the given address.  This allows the managing class to
		 *	move the correct amount of data from the given address to the new address after it handles the
		 *	reallocation itself.
		 */
		void * LSE_CALL						ReAlloc( void * _pvAddr, LSA_SIZE _sSize,
			LSA_SIZE &_sOldSize );

		/**
		 * Determine if this heap is empty (no allocations remain).
		 *
		 * \return Returns true is their are no allocations resident in this heap.
		 */
		LSBOOL LSE_CALL							IsEmpty() const;

		/**
		 * Trash all data.  The contents of the heap are completely reset and all allocations are lost.
		 */
		void LSE_CALL							Trash();

		/**
		 * Is the given address in range of our heap?
		 *
		 * \param _pvAddr The address to check for being within our working set.
		 * \return Returns true if the address is in range of our working set.
		 */
		LSBOOL LSE_CALL							HaveAddress( const void * _pvAddr ) const;

		/**
		 * Given the number of 256-sized blocks desired, determine how much RAM will be needed to
		 *	allow us to work with a set where each smaller block set consumes the same or less RAM
		 *	as the 256-sized block set.
		 * In other words, if 16 256-sized blocks are given, the total size for that set will be
		 *	4,096 bytes (not including the 16 bits for record-keeping).  Then, 256 16-byte blocks
		 *	will be created, 128 32-byte blocks, 85 48-byte blocks, etc.
		 * The sum of all of these blocks, including the sizes of the bits needed for record keeping,
		 *	is returned.
		 *
		 * \param _ui32Total256Blocks Total number of 256-byte-sized blocks to consider as a guideline
		 *	for the rest of the working set.
		 * \param _pvAddress If not NULL, this is the address where the working set should be applied.
		 * \param _psabBlocks If not NULL, the pointers in the given LSA_SMALL_ALLOC_BLOCKSET objects are
		 *	set while the size is being calculated.
		 * \return Total RAM consumed for the whole working set assuming a starting address aligned on
		 *	a 16-byte boundary.
		 */
		static LSA_SIZE LSE_CALL				GetWorkingSetSizeByNumberOf256Blocks( uint32_t _ui32Total256Blocks,
			const void * _pvAddress = NULL,
			LSA_SMALL_ALLOC_BLOCKSET * _psabBlocks = NULL );

		/**
		 * Gets the full size of a CSmallAllocator object if applied at a given address.
		 *
		 * \param _pvAddress The address assumed as the address where a CSmallAllocator object will be applied.
		 * \param _ui32Total256Blocks Total number of 256-byte-sized blocks to use as a guideline for
		 *	calculating the size of the full working set.
		 * \return The full size of the object if used at the given address.
		 */
		static LSA_SIZE LSE_CALL				GetFullAllocatorSizeWithNumberOf256Blocks( void * _pvAddress,
			uint32_t _ui32Total256Blocks );

		/**
		 * Gets the number of LSA_SIZE objects needed to hold the number of bits given for use as book keeping.
		 *
		 * \param _ui32Bits Number of bits to convert
		 * \return Returns the number of LSA_SIZE objects needed to hold the number of bits given.
		 */
		static uint32_t LSE_CALL				GetElementsFromBits( uint32_t _ui32Bits );

		/**
		 * Given a bit index into the bit array used for book keeping, return a byte and bit index.
		 *
		 * \param _ui32Bit Bit to be converted to a byte and bit combination.
		 * \param _ui32ReturnBit Bit part of the returned index.
		 * \return Returns the byte part of the index.
		 */
		static uint32_t LSE_CALL				GetByteAndBitFromBit( const uint32_t &_ui32Bit,
			uint32_t &_ui32ReturnBit );

		/**
		 * Perform a binary search for a bit set to 0.
		 *
		 * \param _sValue The value to search for a 0 bit.
		 * \return Returns the first-found 0 bit.  There must be at least 1 0 bit in the value.
		 */
		static uint32_t LSE_CALL				Find0Bit( LSA_SIZE _sValue );

		/**
		 * Gets the offset from the base of a block set where the block set and block are given as indices.
		 *
		 * \param _ui32SetIndex Block set index.
		 * \param _ui32BlockIndex Block index.
		 * \return Returns the offset from the start of the block set where the given block can be found.
		 */
		LSE_INLINE static uintptr_t LSE_CALL	GetBlockOffset( uint32_t _ui32SetIndex, uint32_t _ui32BlockIndex );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Is the given address in range of our heap?
	 *
	 * \param _pvAddr The address to check for being within our working set.
	 * \return Returns true if the address is in range of our working set.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSmallAllocator::HaveAddress( const void * _pvAddr ) const {
		uintptr_t uiptrThis = reinterpret_cast<uintptr_t>(_pvAddr);
		return uiptrThis >= m_sLow && uiptrThis < m_sHi;
	}

	/**
	 * Gets the offset from the base of a block set where the block set and block are given as indices.
	 *
	 * \param _ui32SetIndex Block set index.
	 * \param _ui32BlockIndex Block index.
	 * \return Returns the offset from the start of the block set where the given block can be found.
	 */
	LSE_INLINE uintptr_t LSE_CALL CSmallAllocator::GetBlockOffset( uint32_t _ui32SetIndex, uint32_t _ui32BlockIndex ) {
		uintptr_t uiptrSize = (_ui32SetIndex + 1) * LSA_SMALLEST_ALLOCATION_SIZE;
		return uiptrSize * _ui32BlockIndex;
	}
	
	
}	// namespace lsa

#endif	// __LSA_SMALLALLOCATOR_H__
