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

#include "LSASmallAllocator.h"


namespace lsa {

	// == Various constructors.
	LSE_CALLCTOR CSmallAllocator::CSmallAllocator( void * _pvAddress, uint32_t _ui32Total256Blocks ) :
		m_psaNext( NULL ) {
		Init( _pvAddress, _ui32Total256Blocks );
	}

	// == Functions.
	/**
	 * Initialize this object at a given location.
	 *
	 * \param _pvAddress Address of the working set to use.
	 * \param _ui32Total256Blocks Total number of 256-byte-sized blocks to use in the working set.
	 */
	void LSE_CALL CSmallAllocator::Init( void * _pvAddress, uint32_t _ui32Total256Blocks ) {
		m_sLow = reinterpret_cast<uintptr_t>(_pvAddress);
		m_sHi = m_sLow + GetWorkingSetSizeByNumberOf256Blocks( _ui32Total256Blocks, _pvAddress, m_pui8BlockStarts );
		for ( uint32_t I = 0; I < LSA_WS; ++I ) {
			for ( uint32_t ui32ByteForBits = GetElementsFromBits( m_pui8BlockStarts[I].ui32Blocks ); ui32ByteForBits--; ) {
				m_pui8BlockStarts[I].psBits[ui32ByteForBits] = static_cast<LSA_SIZE>(0);
			}
		}
		Trash();
	}

	/**
	 * Allocates a number of bytes.  Returns the allocated address or NULL on error.  There is no debug
	 *	information to store.
	 *
	 * \param _sSize Size of the requested allocation.
	 * \param _ui32Align Requested alignment in bytes.
	 * \return Returns the allocated address or NULL on error.
	 */
	void * LSE_CALL CSmallAllocator::Alloc( LSA_SIZE _sSize, uint32_t _ui32Align ) {
		if ( _ui32Align == 0 ) {
			_ui32Align = LSA_MIN_ALIGN;
		}
		// The minumum set index is easy to calculate.
		// Search from here.  We search larger blocks because we may not find a match in this block
		//	size but it is acceptable to over-allocate when this happens.
		for ( uint32_t ui32Set = static_cast<uint32_t>(_sSize >> LSA_SMALLEST_ALLOCATION_BITS); ui32Set < LSA_WS; ++ui32Set ) {
			LSA_SMALL_ALLOC_BLOCKSET & sabThis = m_pui8BlockStarts[ui32Set];
			for ( uint32_t I = GetElementsFromBits( sabThis.ui32Blocks ); I--; ) {
				LSA_SIZE sValue = sabThis.psBits[I];
				if ( sValue != ~static_cast<LSA_SIZE>(0) ) {
					// A bit in this area is not set.
					// If the alignment is LSA_MIN_ALIGN, we can do a binary search to find an unset bit.
					// Otherwise we have to do a linear search.
					uint32_t ui32Bit( 0 );
					uintptr_t uiptrBase = reinterpret_cast<uintptr_t>(sabThis.pui8Blocks);
					if ( _ui32Align == LSA_MIN_ALIGN ) {
						ui32Bit = Find0Bit( sValue );
					}
					else {
						LSBOOL bFound = false;
						for ( uint32_t J = 8 * sizeof( LSA_SIZE ); J--; ) {
							if ( (sValue & (static_cast<LSA_SIZE>(1) << J)) == 0 ) {
								// Free block, but is it aligned correctly?
								uintptr_t uiptrTempBase = uiptrBase + (((ui32Set + 1) * LSA_SMALLEST_ALLOCATION_SIZE) * ((8 * sizeof( LSA_SIZE ) * I) + J));
								if ( uiptrTempBase % _ui32Align != 0 ) { continue; }
								// Found a match.
								bFound = true;
								ui32Bit = J;
								break;
							}
						}
						// If nothing found in this block region, continue.
						if ( !bFound ) { continue; }
					}

					// Calculate the offset from the base to the free block we found.
					uiptrBase += ((ui32Set + 1) * LSA_SMALLEST_ALLOCATION_SIZE) * ((8 * sizeof( LSA_SIZE ) * I) + ui32Bit);
					// uiptrBase is now the final address.
					// Set the flag to declare this block allocated.
					sabThis.psBits[I] |= (static_cast<LSA_SIZE>(1) << ui32Bit);

					return reinterpret_cast<void *>(uiptrBase);
				}
			}
		}
		return NULL;
	}

	/**
	 * Free an address.
	 *
	 * \param _pvAddr Address to free.
	 * \return Returns true if the address was freed.  Returns false if the given address was not allocated
	 *	by this object.
	 */
	LSBOOL LSE_CALL CSmallAllocator::Free( void * _pvAddr ) {
		uintptr_t uiptrAddr = reinterpret_cast<uintptr_t>(_pvAddr);
		// Find the set containing the address.
		for ( uint32_t I = 0; I < LSA_WS; ++I ) {
			uintptr_t uiptrThis = reinterpret_cast<uintptr_t>(m_pui8BlockStarts[I].pui8Blocks);
			if ( uiptrAddr < uiptrThis ) { continue; }
			uint32_t ui32Size = (I + 1) * LSA_SMALLEST_ALLOCATION_SIZE;
			uintptr_t uiptrEnd = uiptrThis + ui32Size * m_pui8BlockStarts[I].ui32Blocks;
			if ( uiptrAddr >= uiptrEnd ) { continue; }

			// Found the set containing the address.
			// Which index is it?
			uint32_t ui32Index = static_cast<uint32_t>((uiptrAddr - uiptrThis) / ui32Size);
			// If the address is not an exact match, return false.
			if ( uiptrAddr - uiptrThis != ui32Index * ui32Size ) { return false; }
			// Unset the bit.
			uint32_t ui32Bit;
			uint32_t ui32Byte = GetByteAndBitFromBit( ui32Index, ui32Bit );
			uint8_t * pui8Bits = reinterpret_cast<uint8_t *>(m_pui8BlockStarts[I].psBits);
			// Was the block even allocated?
			if ( (pui8Bits[ui32Byte] & (1 << ui32Bit)) == 0 ) { return false; }
			pui8Bits[ui32Byte] &= ~(1 << ui32Bit);
			return true;
		}

		return false;
	}

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
	void * LSE_CALL CSmallAllocator::ReAlloc( void * _pvAddr, LSA_SIZE _sSize,
		LSA_SIZE &_sOldSize ) {

		_sOldSize = 0;
		// Revert to regular Alloc() if _pvAddr is NULL.
		if ( !_pvAddr ) {
			return Alloc( _sSize );
		}

		uintptr_t uiptrAddr = reinterpret_cast<uintptr_t>(_pvAddr);
		// Find the set containing the address.
		for ( uint32_t I = 0; I < LSA_WS; ++I ) {
			uintptr_t uiptrThis = reinterpret_cast<uintptr_t>(m_pui8BlockStarts[I].pui8Blocks);
			if ( uiptrAddr < uiptrThis ) { continue; }
			uint32_t ui32Size = (I + 1) * LSA_SMALLEST_ALLOCATION_SIZE;
			uintptr_t uiptrEnd = uiptrThis + ui32Size * m_pui8BlockStarts[I].ui32Blocks;
			if ( uiptrAddr >= uiptrEnd ) { continue; }

			// Found the set containing the address.
			// Which index is it?
			uint32_t ui32Index = static_cast<uint32_t>((uiptrAddr - uiptrThis) / ui32Size);
			// If the address is not an exact match, return false.
			if ( uiptrAddr - uiptrThis != ui32Index * ui32Size ) { return NULL; }
			// Unset the bit.
			uint32_t ui32Bit;
			uint32_t ui32Byte = GetByteAndBitFromBit( ui32Index, ui32Bit );
			uint8_t * pui8Bits = reinterpret_cast<uint8_t *>(m_pui8BlockStarts[I].psBits);
			// Was the block even allocated?
			if ( (pui8Bits[ui32Byte] & (1 << ui32Bit)) == 0 ) { return NULL; }


			// If the new size is smaller than the existing size, do nothing.
			if ( _sSize <= ui32Size ) { return _pvAddr; }

			// If the new size is too large for this heap, return just the size.
			if ( _sSize > LSA_LARGEST_ALLOCATION_SIZE ) {
				_sOldSize = ui32Size;
				return NULL;
			}

			// In every other case we have to move the address to a new location.
			// We do not store alignment data, so the only way to be safe is to use the existing alignment.
			uint32_t ui32Align = (uiptrAddr % (LSA_MIN_ALIGN << 1)) == 0 ? (LSA_MIN_ALIGN << 1) : LSA_MIN_ALIGN;
			void * pvNew = Alloc( _sSize, ui32Align );
			if ( !pvNew ) {
				// The size return can now become valid.
				_sOldSize = ui32Size;
				return NULL;
			}

			// Just copy the memory over.
			CStd::MemCpy( pvNew, _pvAddr, ui32Size < _sSize ? ui32Size : _sSize );
			Free( _pvAddr );
			return pvNew;
		}
		return NULL;
	}

	/**
	 * Determine if this heap is empty (no allocations remain).
	 *
	 * \return Returns true is their are no allocations resident in this heap.
	 */
	LSBOOL LSE_CALL CSmallAllocator::IsEmpty() const {
		for ( uint32_t ui32Set = LSA_WS; ui32Set--; ) {
			const LSA_SMALL_ALLOC_BLOCKSET & sabThis = m_pui8BlockStarts[ui32Set];
			uint32_t ui32Total = GetElementsFromBits( sabThis.ui32Blocks );
			// Check the last bits one-by-one.
			// Check the rest in full sets.
			if ( ui32Total > 1 ) {
				for ( uint32_t I = ui32Total - 1; I--; ) {
					// Check 32 or 64 bits at a time.
					if ( sabThis.psBits[I] != ~static_cast<LSA_SIZE>(0) ) {
						return false;
					}
				}
			}
			if ( ui32Total != 0 ) {
				// Scan only up to the actual number of bits we use.
				// Remember, the last few trailing bits might be the dummies we set in Init(), but
				//	we do not want to count them as allocations in this function.
				uint32_t ui32BitStart = (ui32Total - 1) * (8 * sizeof( LSA_SIZE ));
				uint32_t ui32BitsToCheck = (sabThis.ui32Blocks) - ui32BitStart;
				uint8_t * pui8Bits = reinterpret_cast<uint8_t *>(sabThis.psBits);
				for ( uint32_t I = ui32BitsToCheck; I--; ) {
					uint32_t ui32Bit;
					uint32_t ui32Byte = GetByteAndBitFromBit( ui32BitStart + I, ui32Bit );
					if ( (pui8Bits[ui32Byte] & (1 << ui32Bit)) ) { return false; }
				}
			}
		}
		return true;
	}

	/**
	 * Trash all data.  The contents of the heap are completely reset and all allocations are lost.
	 */
	void LSE_CALL CSmallAllocator::Trash() {
		for ( uint32_t I = 0; I < LSA_WS; ++I ) {
			// Set the unused bits at the end to allocated so we do not try to allocate them later.
			uint32_t ui32TotalBits = GetElementsFromBits( m_pui8BlockStarts[I].ui32Blocks ) * (sizeof( LSA_SIZE ) * 8);
			uint32_t ui32UnusedBits = ui32TotalBits - m_pui8BlockStarts[I].ui32Blocks;

			for ( uint32_t J = ui32TotalBits - ui32UnusedBits; J < ui32TotalBits; ++J ) {
				uint32_t ui32Bit;
				uint32_t ui32Byte = GetByteAndBitFromBit( J, ui32Bit );
				reinterpret_cast<uint8_t *>(m_pui8BlockStarts[I].psBits)[ui32Byte] |= (1 << ui32Bit);
			}
		}
	}

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
	LSA_SIZE LSE_CALL CSmallAllocator::GetWorkingSetSizeByNumberOf256Blocks( uint32_t _ui32Total256Blocks,
		const void * _pvAddress,
		LSA_SMALL_ALLOC_BLOCKSET * _psabBlocks ) {
		LSA_SIZE sTotal = 0;
		LSA_SIZE sSizeOfLargest = _ui32Total256Blocks * 256;
		uintptr_t uiptrAddr = reinterpret_cast<uintptr_t>(_pvAddress);


		for ( uint32_t I = 0; I < LSA_WS; ++I ) {
			// Total for this size.
			uint32_t ui32ThisSize = (I + 1) * LSA_SMALLEST_ALLOCATION_SIZE;
			uint32_t ui32TotalFullBlocks = static_cast<uint32_t>(sSizeOfLargest / ui32ThisSize);
			uint32_t ui32BytesNeededForBits = LSE_ROUND_UP( ui32TotalFullBlocks, 8 ) >> 3;
			ui32BytesNeededForBits = LSE_ROUND_UP( ui32BytesNeededForBits, sizeof( LSA_SIZE ) );

			// Blocks start on boundaries of LSA_MIN_ALIGN << 1.
			sTotal = LSE_ROUND_UP( sTotal, LSA_MIN_ALIGN << 1 );

			// Add the full block size followed immediately by the bits that indicate usage.
			// We do not need to align the bits because the basic math above guarantees the blocks
			//	will always end on a 16-byte-aligned boundary, so the following usage bits will
			//	always be aligned by this amount.
			if ( uiptrAddr && _psabBlocks ) {
				_psabBlocks[I].pui8Blocks = reinterpret_cast<uint8_t *>(uiptrAddr + sTotal);
				_psabBlocks[I].psBits = reinterpret_cast<LSA_SIZE *>((uiptrAddr + sTotal) + (ui32ThisSize * ui32TotalFullBlocks));
				_psabBlocks[I].ui32Blocks = ui32TotalFullBlocks;
			}
			sTotal += ui32ThisSize * ui32TotalFullBlocks + ui32BytesNeededForBits;
		}

		return sTotal;
	}

	/**
	 * Gets the full size of a CSmallAllocator object if applied at a given address.
	 *
	 * \param _pvAddress The address assumed as the address where a CSmallAllocator object will be applied.
	 * \param _ui32Total256Blocks Total number of 256-byte-sized blocks to use as a guideline for
	 *	calculating the size of the full working set.
	 * \return The full size of the object if used at the given address.
	 */
	LSA_SIZE LSE_CALL CSmallAllocator::GetFullAllocatorSizeWithNumberOf256Blocks( void * _pvAddress,
		uint32_t _ui32Total256Blocks ) {

		uintptr_t uiptrAddr = reinterpret_cast<uintptr_t>(_pvAddress);
		uintptr_t uiptrStart = uiptrAddr;
		// Add the size of the object.
		uiptrStart += sizeof( CSmallAllocator );

		// Rond up to the minumum allocation alignment.
		uiptrStart = LSE_ROUND_UP( uiptrStart, LSA_MIN_ALIGN );

		// Add the size of the working set.
		return (uiptrStart + GetWorkingSetSizeByNumberOf256Blocks( _ui32Total256Blocks )) - uiptrAddr;
	}

	/**
	 * Gets the number of LSA_SIZE objects needed to hold the number of bits given for use as book keeping.
	 *
	 * \param _ui32Bits Number of bits to convert
	 * \return Returns the number of LSA_SIZE objects needed to hold the number of bits given.
	 */
	uint32_t LSE_CALL CSmallAllocator::GetElementsFromBits( uint32_t _ui32Bits ) {
		uint32_t ui32BytesNeededForBits = _ui32Bits >> 3;	// Divide by 8.
		ui32BytesNeededForBits = LSE_ROUND_UP( ui32BytesNeededForBits, sizeof( LSA_SIZE ) );
		return ui32BytesNeededForBits >> (sizeof( LSA_SIZE ) / 4 + 1);
	}

	/**
	 * Given a bit index into the bit array used for book keeping, return a byte and bit index.
	 *
	 * \param _ui32Bit Bit to be converted to a byte and bit combination.
	 * \param _ui32ReturnBit Bit part of the returned index.
	 * \return Returns the byte part of the index.
	 */
	uint32_t LSE_CALL CSmallAllocator::GetByteAndBitFromBit( const uint32_t &_ui32Bit,
		uint32_t &_ui32ReturnBit ) {
		_ui32ReturnBit = (_ui32Bit & 0x7);	// % 8.
		return (_ui32Bit >> 3);				// / 8.
	}

	/**
	 * Perform a binary search for a bit set to 0.
	 *
	 * \param _sValue The value to search for a 0 bit.
	 * \return Returns the first-found 0 bit.  There must be at least 1 0 bit in the value.
	 */
	uint32_t LSE_CALL CSmallAllocator::Find0Bit( LSA_SIZE _sValue ) {
		LSA_SIZE sCurBit = 0;
		LSA_SIZE sBits = ((sizeof( LSA_SIZE ) * 8) >> 1);

		while ( sBits ) {
			// Check the current X bits.
			LSA_SIZE sMask = ((static_cast<LSA_SIZE>(1) << sBits) - static_cast<LSA_SIZE>(1)) << sCurBit;
			if ( (_sValue & sMask) != sMask ) {
				// One of the bits in the this section is set.
				// Decrease the size of sBits without increasing the size of sCurBit.
				sBits >>= 1;
			}
			else {
				// Bit to find must be in the upper half.
				// Increase the offset and loop.
				sCurBit += sBits;
			}
		}
		return static_cast<uint32_t>(sCurBit);
	}

}	// namespace lsa
