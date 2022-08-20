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
 * Description: Standard search-and-sort routines.
 */

#include "LSSTDSearch.h"


namespace lsstd {

	// == Functions.
	/**
	 * Perform a binary search on an array of data in memory.
	 *
	 * \param _pvKey Object for which to search.
	 * \param _pvBase Pointer to the _pvBase of the data to search.
	 * \param _uiptrNum Number of elements in the data to which _pvBase points.
	 * \param _uiptrWidth Width of the elements in the data to which _pvBase points.
	 * \param _pbsfFunc Callback function that compares two elements. The first argument
	 *	is the _pvContext pointer. The second argument is a pointer to the _pvKey for the search.
	 *	The third argument is a pointer to the array element to be compared with _pvKey.
	 * \param _pvContext A pointer to an object that can be accessed in the comparison function.
	 * \return Returns a pointer to an occurrence of _pvKey in the array to which _pvBase points.
	 *	If _pvKey is not found, the function returns NULL.  If the array is not in ascending sort
	 *	order or contains duplicate records with identical keys, the i32Res is unpredictable.
	 */
	void * LSE_CALL CSearch::BSearch( const void * _pvKey,
		const void * _pvBase,
		uintptr_t _uiptrNum,
		uintptr_t _uiptrWidth,
		PfBSearchFunc _pbsfFunc,
		void * _pvContext ) {

		register const uint8_t * pui8Low = reinterpret_cast<const uint8_t *>(_pvBase);
		register const uint8_t * pui8High = reinterpret_cast<const uint8_t *>(_pvBase) + (_uiptrNum - 1) * _uiptrWidth;
		register const uint8_t * pui8Mid;
		uintptr_t uiptrHalf;
		int32_t i32Res;

		// Validate the data.
		if ( !(_pvBase != NULL || _uiptrNum == 0) ) { return NULL; }
		if ( !(_uiptrWidth > 0) ) { return NULL; }
		if ( !(_pbsfFunc != NULL) ) { return NULL; }

		// We allow a NULL _pvKey because we do not use it ourselves and we cannot be sure that NULL is
		//	not a valid value.
		while ( pui8Low <= pui8High ) {
			if ( (uiptrHalf = (_uiptrNum >> 1)) != 0 ) {
				pui8Mid = pui8Low + (_uiptrNum & 1 ? uiptrHalf : (uiptrHalf - 1)) * _uiptrWidth;
				i32Res = _pbsfFunc( _pvContext, _pvKey, pui8Mid );
				if ( !i32Res ) {
					return const_cast<uint8_t *>(pui8Mid);
				}
				else if ( i32Res < 0 ) {
					pui8High = pui8Mid - _uiptrWidth;
					_uiptrNum = (_uiptrNum & 1) ? uiptrHalf : uiptrHalf - 1;
				}
				else {
					pui8Low = pui8Mid + _uiptrWidth;
					_uiptrNum = uiptrHalf;
				}
			}
			else if ( _uiptrNum ) {
				return (_pbsfFunc( _pvContext, _pvKey, pui8Low ) ? NULL : const_cast<uint8_t *>(pui8Low));
			}
			else {
				break;
			}
		}

		return NULL;
	}

	/**
	 * A quick sort.
	 *
	 * \param _pvBase Start of target array.
	 * \param _uiptrNum Array uiptrSize in elements.
	 * \param _uiptrWidth Element uiptrSize in bytes.
	 * \param _psfFunc Comparison function. The first argument is the _pvContext pointer. The second
	 *	argument is a pointer to the left element to compare. The third argument is a pointer to the 
	 *	right element to be compared against the left.
	 * \param _pvContext A pointer to a _pvContext, which can be any object that the compare routine
	 *	needs to access.
	 */
	void LSE_CALL CSearch::QSort( void * _pvBase,
		uintptr_t _uiptrNum,
		uintptr_t _uiptrWidth,
		PfSortFunc _psfFunc,
		void * _pvContext ) {

#define LSSTD_STACKSIZE (8 * sizeof( void * ) - 2)

		uint8_t * pui8Low, * pui8High;
		uint8_t * pui8Mid;
		uint8_t * pui8LowGuy, * pui8HighGuy;
		uintptr_t uiptrSize;
		uint8_t * pui8LowStack[LSSTD_STACKSIZE], * pui8HighStack[LSSTD_STACKSIZE];
		int32_t i32StackPointer;
#undef LSSTD_STACKSIZE

		// Validate the parameters.
		if ( !(_pvBase != NULL || _uiptrNum == 0) ) { return; }
		if ( !(_uiptrWidth > 0) ) { return; }
		if ( !(_psfFunc != NULL) ) { return; }

		if ( _uiptrNum < 2 ) { return; }

		// Initialize the stack.
		i32StackPointer = 0;

		// Initialize the limits.
		pui8Low = reinterpret_cast<uint8_t *>(_pvBase);
		pui8High = reinterpret_cast<uint8_t *>(_pvBase) + _uiptrWidth * (_uiptrNum - 1);


Recurse :

		// Number of elements to sort.
		uiptrSize = static_cast<uintptr_t>(pui8High - pui8Low) / _uiptrWidth + 1;

		// Below a certain size, it is faster to use an O(n^2) sorting method.
		if ( uiptrSize <= 8 ) {
			ShortSort( pui8Low, pui8High, _uiptrWidth, _psfFunc, _pvContext );
		}
		else {
			// First we pick a partitioning element.  The efficiency of the
			//	algorithm demands that we find one that is approximately the median
			//	of the values, but also that we select one quickly.  We choose the
			//	median of the first, middle, and last elements, to avoid bad
			//	performance in the face of already-sorted data, or data that is made
			//	up of multiple sorted runs appended together.  Testing shows that a
			//	median-of-three algorithm provides better performance than simply
			//	picking the middle element for the latter case.

			// Find the middle element.
			pui8Mid = pui8Low + (uiptrSize >> 1) * _uiptrWidth;

			// Sort the first, middle, last elements into order.
			if ( _psfFunc( _pvContext, pui8Low, pui8Mid ) > 0 ) {
				ShortSwap( pui8Low, pui8Mid, _uiptrWidth );
			}
			if ( _psfFunc( _pvContext, pui8Low, pui8High ) > 0 ) {
				ShortSwap( pui8Low, pui8High, _uiptrWidth );
			}
			if ( _psfFunc( _pvContext, pui8Mid, pui8High ) > 0 ) {
				ShortSwap( pui8Mid, pui8High, _uiptrWidth );
			}

			// We now wish to partition the array into three pieces, one consisting
			//	of elements <= partition element, one of elements equal to the
			//	partition element, and one of elements > than it.  This is done
			//	below; comments indicate conditions established at every step.

			pui8LowGuy = pui8Low;
			pui8HighGuy = pui8High;

			// Note that pui8HighGuy decreases and pui8LowGuy increases on every iteration,
			//	so loop must terminate.
			for ( ; ; ) {
				// pui8Low <= pui8LowGuy < pui8High, pui8Low < pui8HighGuy <= pui8High,
				//	A[i] <= A[pui8Mid] for pui8Low <= i <= pui8LowGuy,
				//	A[i] > A[pui8Mid] for pui8HighGuy <= i < pui8High,
				//	A[pui8High] >= A[pui8Mid]

				// The doubled loop is to avoid calling _psfFunc( pui8Mid, pui8Mid ), since some
				//	existing comparison functions don't work when passed the same
				//	value for both pointers.
				if ( pui8Mid > pui8LowGuy ) {
					do  {
						pui8LowGuy += _uiptrWidth;
					} while ( pui8LowGuy < pui8Mid && _psfFunc( _pvContext, pui8LowGuy, pui8Mid ) <= 0 );
				}
				if ( pui8Mid <= pui8LowGuy ) {
					do  {
						pui8LowGuy += _uiptrWidth;
					} while ( pui8LowGuy <= pui8High && _psfFunc( _pvContext, pui8LowGuy, pui8Mid ) <= 0 );
				}

				// pui8Low < pui8LowGuy <= pui8High+1, A[i] <= A[pui8Mid] for pui8Low <= i < pui8LowGuy,
				//	either pui8LowGuy > pui8High or A[pui8LowGuy] > A[pui8Mid]

				do  {
					pui8HighGuy -= _uiptrWidth;
				} while ( pui8HighGuy > pui8Mid && _psfFunc( _pvContext, pui8HighGuy, pui8Mid ) > 0 );

				// pui8Low <= pui8HighGuy < pui8High, A[i] > A[pui8Mid] for pui8HighGuy < i < pui8High,
				//	either pui8HighGuy == pui8Low or A[pui8HighGuy] <= A[pui8Mid]

				if ( pui8HighGuy < pui8LowGuy ) {
					break;
				}

				// if pui8LowGuy > pui8High or pui8HighGuy == pui8Low, then we would have exited, so
				//	A[pui8LowGuy] > A[pui8Mid], A[pui8HighGuy] <= A[pui8Mid],
				//	pui8LowGuy <= pui8High, pui8HighGuy > pui8Low

				ShortSwap( pui8LowGuy, pui8HighGuy, _uiptrWidth );

				// If the partition element was moved, follow it.  Only need
				//	to check for pui8Mid == pui8HighGuy, since before the swap,
				//	A[pui8LowGuy] > A[pui8Mid] implies pui8LowGuy != pui8Mid.

				if ( pui8Mid == pui8HighGuy ) {
					pui8Mid = pui8LowGuy;
				}

				// A[pui8LowGuy] <= A[pui8Mid], A[pui8HighGuy] > A[pui8Mid]; so condition at top
				//	of loop is re-established.
			}

			// A[i] <= A[pui8Mid] for pui8Low <= i < pui8LowGuy,
			//	A[i] > A[pui8Mid] for pui8HighGuy < i < pui8High,
			//	A[pui8High] >= A[pui8Mid]
			//	pui8HighGuy < pui8LowGuy
			//	implying:
			//	pui8HighGuy == pui8LowGuy-1
			//	or pui8HighGuy == pui8High - 1, pui8LowGuy == pui8High + 1, A[pui8High] == A[pui8Mid]

			// Find adjacent elements equal to the partition element.  The
			//	doubled loop is to avoid calling _psfFunc( pui8Mid, pui8Mid ), since some
			//	existing comparison functions don't work when passed the same value
			//	for both pointers.

			pui8HighGuy += _uiptrWidth;
			if ( pui8Mid < pui8HighGuy ) {
				do  {
					pui8HighGuy -= _uiptrWidth;
				} while ( pui8HighGuy > pui8Mid && _psfFunc( _pvContext, pui8HighGuy, pui8Mid ) == 0 );
			}
			if ( pui8Mid >= pui8HighGuy ) {
				do  {
					pui8HighGuy -= _uiptrWidth;
				} while ( pui8HighGuy > pui8Low && _psfFunc( _pvContext, pui8HighGuy, pui8Mid ) == 0 );
			}

			// Okay, now we have the following:
			//	pui8HighGuy < pui8LowGuy
			//	pui8Low <= pui8HighGuy <= pui8High
			//	A[i] <= A[pui8Mid] for pui8Low <= i <= pui8HighGuy
			//	A[i]  == A[pui8Mid] for pui8HighGuy < i < pui8LowGuy
			//	A[i]  >  A[pui8Mid] for pui8LowGuy <= i < pui8High
			//	A[pui8High] >= A[pui8Mid]

			// We've finished the partition; now we want to sort the subarrays
			//	[pui8Low, pui8HighGuy] and [pui8LowGuy, pui8High].
			//	We do the smaller one first to minimize stack usage.
			//	We only sort arrays of length 2 or more.

			if ( pui8HighGuy - pui8Low >= pui8High - pui8LowGuy ) {
				if ( pui8Low < pui8HighGuy ) {
					pui8LowStack[i32StackPointer] = pui8Low;
					pui8HighStack[i32StackPointer] = pui8HighGuy;
					++i32StackPointer;
				}

				if ( pui8LowGuy < pui8High ) {
					pui8Low = pui8LowGuy;
					goto Recurse;
				}
			}
			else {
				if ( pui8LowGuy < pui8High ) {
					pui8LowStack[i32StackPointer] = pui8LowGuy;
					pui8HighStack[i32StackPointer] = pui8High;
					++i32StackPointer;
				}

				if ( pui8Low < pui8HighGuy ) {
					pui8High = pui8HighGuy;
					goto Recurse;
				}
			}
		}

		// We have sorted the array except for any pending sorts on the stack.
		//	Check if there are any and do them.

		--i32StackPointer;
		if ( i32StackPointer >= 0 ) {
			pui8Low = pui8LowStack[i32StackPointer];
			pui8High = pui8HighStack[i32StackPointer];

			// Pop array from the stack.
			goto Recurse;
		}
		else { return; }
	}

	/**
	 * A stable in-place merge sort.  Avoid recursion and makes only one allocation for the whole
	 *	operation.
	 *
	 * \param _pvBase Start of target array.
	 * \param _uiptrNum Array size in elements.
	 * \param _uiptrWidth Element size in bytes.
	 * \param _psfFunc Comparison function. The first argument is the context pointer. The second
	 *	argument is a pointer to the left element to compare. The third argument is a pointer to the 
	 *	right element to be compared against the left.
	 * \param _pvContext A pointer to a context, which can be any object that the compare routine
	 *	needs to access.
	 */
	void LSE_CALL CSearch::MergeSort( void * _pvBase,
		uintptr_t _uiptrNum,
		uintptr_t _uiptrWidth,
		PfSortFunc _psfFunc,
		void * _pvContext ) {
		if ( _uiptrNum <= 1 ) { return; }
		uintptr_t uiptrSize = _uiptrNum * _uiptrWidth;
		uint8_t * pui8NewList = new uint8_t[uiptrSize];

		// This is not a standard implementation.  The standard implementation goes recursive until
		//	it reaches a point where each bucket consists of only one itme, then rejoins each bucket.
		//	The actual sort happens during the rejoining of the buckets.
		// This implies that recursion is not actually a necessary step.  If we just assume that the
		//	array we have been given is already just a set of buckets, each with one item in it, then
		//	we are already at the bottom of the standard implementation's recursive call stack.
		// Additionally, every bucket is the same size except for odd-numbered input cases, which are easy
		//	enough to handle, and every complete pass at rejoining the buckets creates one complete copy
		//	of the original data.
		// Given these two facts, we don't need to rely on recursion to figure out the sizes of each bucket,
		//	and we only need one buffer.  Since we are aiming to copy back over the original buffer anyway,
		//	we can use double-buffering, bouncing between our new buffer and the input buffer on each pass.
		//	This saves us a copy at the end 50% of the time.
		// The advantages are obvious:
		//	#1: No rescursion overhead.
		//	#2: Only one allocation instead of one for each recursion level.
		//	#3: Possible to skip the final buffer copy 50% of the time.
		//	#4: The sort is still stable (items with the same values next to each other stay in the same order).
		// This is the most efficient implementation known.
		

		uint32_t ui32BucketSize = 1;								// Doubles on each pass.
		uint8_t * pui8DestAndSrc[] = {								// Swap between destinations and sources on each pass.
			pui8NewList,
			reinterpret_cast<uint8_t *>(_pvBase)
		};

		// Do each "recursion" in a for loop.  Each iteration of this loop represents one recursion unroll
		//	as buckets are merged in the standard implementation.  This yields the exact same results faster.
		uint8_t * pui8Dest;
		uint32_t I = 0;
		for ( ; ui32BucketSize < _uiptrNum; ++I, ui32BucketSize <<= 1 ) {
			uint8_t * pui8Src = pui8DestAndSrc[(I^LSE_MAXU32)&1];
			pui8Dest = pui8DestAndSrc[I&1];

			// Go over each bucket at this level.
			uint32_t ui32BucketSizeInBytes = static_cast<uint32_t>(ui32BucketSize * _uiptrWidth);
			uint32_t ui32Jump = static_cast<uint32_t>(ui32BucketSizeInBytes << 1);
			
			for ( uint32_t J = 0; J < uiptrSize; J += ui32Jump ) {
				uint8_t * pui8Left = &pui8Src[J];
				uint32_t ui32LeftSize = ui32BucketSizeInBytes;
				if ( ui32LeftSize >= uiptrSize - J ) {
					ui32LeftSize = static_cast<uint32_t>(uiptrSize - J);
					// There is no right side, so just copy the bytes.
					CStd::MemCpy( pui8Dest, pui8Left, ui32LeftSize );
					break;	// Loop is over.
				}

				uint8_t * pui8Right = &pui8Src[J+ui32BucketSizeInBytes];
				uint32_t ui32RightSize = ui32BucketSizeInBytes;

				// Sanity check.
				if ( ui32RightSize > uiptrSize - (J + ui32BucketSizeInBytes) ) {
					ui32RightSize = static_cast<uint32_t>(uiptrSize - (J + ui32BucketSizeInBytes));
				}
				

				Merge( pui8Left, static_cast<uint32_t>(ui32LeftSize / _uiptrWidth), pui8Right, static_cast<uint32_t>(ui32RightSize / _uiptrWidth),
					pui8Dest, _uiptrWidth, _psfFunc, _pvContext );

				pui8Dest += ui32Jump;
			}
		}

		if ( pui8DestAndSrc[(I^LSE_MAXU32)&1] != _pvBase ) {
			// Last pass put the result in the other buffer.  Copy it over.
			CStd::MemCpy( _pvBase, pui8DestAndSrc[(I^LSE_MAXU32)&1], uiptrSize );
		}
		delete [] pui8NewList;
	}

	/**
	 * A short sort.  This works faster on smaller data sets.  QSort() calls this automatically, so
	 *	there is no need to use this directly unless you know your data is always under 9 elements
	 *	in length.
	 *
	 * \param _pui8Low Start of target array.
	 * \param _pui8High End of the array.
	 * \param _uiptrWidth Element uiptrSize in bytes.
	 * \param _psfFunc Comparison function. The first argument is the _pvContext pointer. The second
	 *	argument is a pointer to the left element to compare. The third argument is a pointer to the 
	 *	right element to be compared against the left.
	 * \param _pvContext A pointer to a _pvContext, which can be any object that the compare routine
	 *	needs to access.
	 */
	void LSE_CALL CSearch::ShortSort( uint8_t * _pui8Low,
		uint8_t * _pui8High,
		uintptr_t _uiptrWidth,
		PfSortFunc _psfFunc,
		void * _pvContext ) {

		uint8_t * pui8P, * pui8Max;
		// Note: In assertions below, i and j are alway inside original bounds of the
		//	array to sort.

		while ( _pui8High > _pui8Low ) {
			// A[i] <= A[j] for i <= j, j > hi
			pui8Max = _pui8Low;
			for ( pui8P = _pui8Low + _uiptrWidth; pui8P <= _pui8High; pui8P += _uiptrWidth ) {
				// A[i] <= A[pui8Max] for lo <= i < p
				if ( _psfFunc( _pvContext, pui8P, pui8Max ) > 0 ) {
					pui8Max = pui8P;
				}
				// A[i] <= A[pui8Max] for lo <= i <= p
			}

			// A[i] <= A[pui8Max] for lo <= i <= hi
			ShortSwap( pui8Max, _pui8High, _uiptrWidth );

			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
			_pui8High -= _uiptrWidth;

			// A[i] <= A[j] for i <= j, j > hi, loop top condition established
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		//	so array is sorted.
	}

	/**
	 * Merge buckets in a merge sort.
	 *
	 * \param _pui8Left Pointer to the first item in the left bucket.
	 * \param _ui32LeftTotal Total items in the left bucket.
	 * \param _pui8Right Pointer to the first item in the right bucket.
	 * \param _ui32RightTotal Total items in the right bucket.
	 * \param _pvDest Destination where to copy the items.
	 * \param _uiptrWidth Element size in bytes.
	 * \param _psfFunc Comparison function. The first argument is the context pointer. The second
	 *	argument is a pointer to the left element to compare. The third argument is a pointer to the 
	 *	right element to be compared against the left.
	 * \param _pvContext A pointer to a context, which can be any object that the compare routine
	 *	needs to access.
	 */
	void LSE_CALL CSearch::Merge( uint8_t * _pui8Left, uint32_t _ui32LeftTotal,
		uint8_t * _pui8Right, uint32_t _ui32RightTotal,
		void * _pvDest,
		uintptr_t _uiptrWidth,
		PfSortFunc _psfFunc, void * _pvContext ) {
		while ( _ui32LeftTotal || _ui32RightTotal ) {
			if ( _ui32LeftTotal && _ui32RightTotal ) {
				int32_t iComp = _psfFunc( _pvContext, _pui8Left, _pui8Right );
				if ( iComp <= 0 ) {
					// Move left over.
					CStd::MemCpy( _pvDest, _pui8Left, _uiptrWidth );
					--_ui32LeftTotal;
					_pui8Left += _uiptrWidth;
				}
				else {
					// Move right over.
					CStd::MemCpy( _pvDest, _pui8Right, _uiptrWidth );
					--_ui32RightTotal;
					_pui8Right += _uiptrWidth;
				}
			}
			else if ( _ui32LeftTotal ) {
				// Move left over.
				CStd::MemCpy( _pvDest, _pui8Left, _uiptrWidth );
				--_ui32LeftTotal;
				_pui8Left += _uiptrWidth;
			}
			else {
				// Move right over.
				CStd::MemCpy( _pvDest, _pui8Right, _uiptrWidth );
				--_ui32RightTotal;
				_pui8Right += _uiptrWidth;
			}
			_pvDest = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(_pvDest) + _uiptrWidth);
		}
	}

}	// namespace lsstd
