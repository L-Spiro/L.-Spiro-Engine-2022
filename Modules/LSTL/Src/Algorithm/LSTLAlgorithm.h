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
 * Description: Algorithms related to our templates.
 */


#ifndef __LSTL_ALGORITHM_H__
#define __LSTL_ALGORITHM_H__

#include "../LSTLib.h"
#include "../Vector/LSTLSVectorPoD.h"
#include "../Vector/LSTLVector.h"
#include "../Vector/LSTLVectorPoD.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lstl {

	/**
	 * Class CAlgorithm
	 *
	 * Description: Algorithms related to our templates.
	 */
	class CAlgorithm {
		// All is public.  This class has no secrets.
	public :

		/**
		 * Find an item in a simple vector.  Searches linearly until found.  Returns the length of the list
		 *	if not found, otherwise the index of the matched item into the list.
		 *
		 * \param _vVec The vector object to scan.
		 * \param _tVal The value to find within the vector.
		 * \return Returns the index of the given item if found, or the length of the vector if not.
		 */
		template <typename _tType, typename _tDataType, unsigned _uAllocSize>
		static uint32_t LSE_CALL			Find( const CVectorPoD<_tType, _tDataType, _uAllocSize> &_vVec, const _tType &_tVal ) {
			for ( uint32_t I = 0; I < _vVec.Length(); ++I ) {
				if ( _vVec[I] == _tVal ) { return I; }
			}
			return _vVec.Length();
		}
		
		/**
		 * Find an item in a simple vector.  Searches linearly until found.  Returns the length of the list
		 *	if not found, otherwise the index of the matched item into the list.
		 *
		 * \param _vVec The vector object to scan.
		 * \param _tVal The value to find within the vector.
		 * \return Returns the index of the given item if found, or the length of the vector if not.
		 */
		template <typename _tType, typename _tDataType, unsigned _uAllocSize>
		static uint32_t LSE_CALL			Find( const CSVectorPoD<_tType, _tDataType, _uAllocSize> &_vVec, const _tType &_tVal ) {
			for ( uint32_t I = 0; I < _vVec.Length(); ++I ) {
				if ( _vVec[I] == _tVal ) { return I; }
			}
			return _vVec.Length();
		}

		/**
		 * Find an item in a normal vector.  Searches linearly until found.  Returns the length of the list
		 *	if not found, otherwise the index of the matched item into the list.
		 *
		 * \param _vVec The vector object to scan.
		 * \param _tVal The value to find within the vector.
		 * \return Returns the index of the given item if found, or the length of the vector if not.
		 */
		template <typename _tType, typename _tDataType, unsigned _uAllocSize>
		static uint32_t LSE_CALL			Find( const CVector<_tType, _tDataType, _uAllocSize> &_vVec, const _tType &_tVal ) {
			for ( uint32_t I = 0; I < _vVec.Length(); ++I ) {
				if ( _vVec[I] == _tVal ) { return I; }
			}
			return _vVec.Length();
		}

		/**
		 * Searches for an item in a normal vector using a binary search.  If the items in the vector are not
		 *	sorted, the results are undefined.   
		 *
		 * \param _vVec The vector object to scan.
		 * \param _tVal The value to find within the vector.
		 * \param _ui32Index Holds the returned index.
		 * \return If the value is found, true is returned and the index of that value is held by _ui32Index.
		 *	If not found, false is returned and _ui32Index holds the index where the item should be in the array.
		 */
		template <typename _tType, typename _tDataType, unsigned _uAllocSize>
		static LSBOOL LSE_CALL				BSearch( const CVector<_tType, _tDataType, _uAllocSize> &_vVec, const _tType &_tVal, uint32_t &_ui32Index ) {
			return BSearch<_tType, _tDataType, _uAllocSize, _tType>( _vVec, _tVal, _ui32Index );
		}

		/**
		 * Searches for an item in a normal vector using a binary search.  If the items in the vector are not
		 *	sorted, the results are undefined.   
		 *
		 * \param _vVec The vector object to scan.
		 * \param _tVal The value to find within the vector.
		 * \param _ui32Index Holds the returned index.
		 * \return If the value is found, true is returned and the index of that value is held by _ui32Index.
		 *	If not found, false is returned and _ui32Index holds the index where the item should be in the array.
		 */
		template <typename _tType, typename _tDataType, unsigned _uAllocSize, typename _tKeyType>
		static LSBOOL LSE_CALL				BSearch( const CVector<_tType, _tDataType, _uAllocSize> &_vVec, const _tKeyType &_tVal, uint32_t &_ui32Index ) {
			if ( !_vVec.Length() ) {
				_ui32Index = 0;
				return false;
			}
			return BSearch( &_vVec[0], static_cast<uint32_t>(_vVec.Length()), _tVal, _ui32Index );
		}

		/**
		 * Searches for an item in a normal vector using a binary search.  If the items in the vector are not
		 *	sorted, the results are undefined.   
		 *
		 * \param _ptArray The vector object to scan.
		 * \param _ui32Len Length of the array
		 * \param _tVal The value to find within the vector.
		 * \param _ui32Index Holds the returned index.
		 * \return If the value is found, true is returned and the index of that value is held by _ui32Index.
		 *	If not found, false is returned and _ui32Index holds the index where the item should be in the array.
		 */
		template <typename _tType, typename _tKeyType>
		static LSBOOL LSE_CALL				BSearch( const _tType * _ptArray, uint32_t _ui32Len, const _tKeyType &_tVal, uint32_t &_ui32Index ) {
			uint32_t ui32Hi = _ui32Len;
			if ( !ui32Hi ) {
				_ui32Index = 0;
				return false;
			}
			// Handle the worst case also.
			if ( _ptArray[ui32Hi-1] < _tVal ) {
				// It goes at the end of the list.
				_ui32Index = ui32Hi;
				return false;
			}

			uint32_t ui32Low = 0;
			

			while ( true ) {
				uint32_t ui32Index = ((ui32Hi - ui32Low) >> 1) + ui32Low;
				if ( _ptArray[ui32Index] == _tVal ) {
					_ui32Index = ui32Index;
					return true;
				}
				if ( _ptArray[ui32Index] < _tVal ) {
					// Move up.
					if ( ui32Index == ui32Low ) {
						_ui32Index = ui32Index + 1;
						return false;
					}
					ui32Low = ui32Index;
				}
				else {
					// Move down.
					if ( ui32Index == ui32Hi ) {
						_ui32Index = ui32Index;
						return false;
					}
					ui32Hi = ui32Index;
				}
			}
		}

		/**
		 * A short sort.  This works faster on smaller data sets.  QSort() calls this automatically, so
		 *	there is no need to use this directly unless you know your data is always under 9 elements
		 *	in length.
		 *
		 * \param _ptValues Start of target array.
		 * \param _ui32Total Total elements in the array.
		 */
		template <typename _tType>
		static void LSE_CALL				ShortSort( _tType * _ptValues, uintptr_t _uiptrTotal ) {
			_tType * ptLow = _ptValues;
			_tType * ptHi = _ptValues + _uiptrTotal;
			_tType * ptP, * ptMax;
			// Note: In assertions below, i and j are alway inside original bounds of the
			//	array to sort.

			while ( ptHi > ptLow ) {
				// A[i] <= A[j] for i <= j, j > hi
				ptMax = ptLow;
				for ( ptP = ptLow + 1; ptP <= ptHi; ++ptP ) {
					// A[i] <= A[ptMax] for lo <= i < p
					if ( (*ptMax) < (*ptP) ) {
						ptMax = ptP;
					}
					// A[i] <= A[ptMax] for lo <= i <= p
				}

				// A[i] <= A[ptMax] for lo <= i <= hi
				_tType tTemp = (*ptMax);
				(*ptMax) = (*ptHi);
				(*ptHi) = tTemp;

				// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
				--ptHi;

				// A[i] <= A[j] for i <= j, j > hi, loop top condition established
			}
			// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
			//	so array is sorted.
		}

		/**
		 * A quick sort.
		 *
		 * \param _ptValues The values to sort in-place.
		 * \param _uiptrTotal Total elements to which _ptValues points.
		 */
		template <typename _tType>
		static void LSE_CALL				QSort( _tType * _ptValues, uintptr_t _uiptrTotal ) {
#define LSSTL_STACKSIZE (8 * sizeof( void * ) - 2)

			_tType * ptLow, * ptHigh;
			_tType * ptMid;
			_tType * ptLowGuy, * ptHighGuy;
			uintptr_t uiptrSize;
			_tType * ptLowStack[LSSTL_STACKSIZE], * ptHighStack[LSSTL_STACKSIZE];
			int32_t i32StackPointer;
#undef LSSTL_STACKSIZE

			// Validate the parameters.
			if ( !(_ptValues != NULL || _uiptrTotal == 0) ) { return; }
			if ( _uiptrTotal < 2 ) { return; }

			// Initialize the stack.
			i32StackPointer = 0;

			// Initialize the limits.
			ptLow = _ptValues;
			ptHigh = _ptValues + (_uiptrTotal - 1);


	Recurse :

			// Number of elements to sort.
			uiptrSize = (ptHigh - ptLow) + 1;

			// Below a certain size, it is faster to use an O(n^2) sorting method.
			if ( uiptrSize <= 8 ) {
				ShortSort( ptLow, uiptrSize - 1 );
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
				ptMid = ptLow + (uiptrSize >> 1);

				// Sort the first, middle, last elements into order.
				if ( (*ptMid) < (*ptLow) ) {
					ShortSwap( ptLow, ptMid );
				}
				if ( (*ptHigh) < (*ptLow) ) {
					ShortSwap( ptLow, ptHigh );
				}
				if ( (*ptHigh) < (*ptMid) ) {
					ShortSwap( ptMid, ptHigh );
				}

				// We now wish to partition the array into three pieces, one consisting
				//	of elements <= partition element, one of elements equal to the
				//	partition element, and one of elements > than it.  This is done
				//	below; comments indicate conditions established at every step.

				ptLowGuy = ptLow;
				ptHighGuy = ptHigh;

				// Note that ptHighGuy decreases and ptLowGuy increases on every iteration,
				//	so loop must terminate.
				for ( ; ; ) {
					// ptLow <= ptLowGuy < ptHigh, ptLow < ptHighGuy <= ptHigh,
					//	A[i] <= A[ptMid] for ptLow <= i <= ptLowGuy,
					//	A[i] > A[ptMid] for ptHighGuy <= i < ptHigh,
					//	A[ptHigh] >= A[ptMid]

					// The doubled loop is to avoid calling _psfFunc( ptMid, ptMid ), since some
					//	existing comparison functions don't work when passed the same
					//	value for both pointers.
					if ( ptMid > ptLowGuy ) {
						do  {
							++ptLowGuy;
						} while ( ptLowGuy < ptMid && ((*ptLowGuy) < (*ptMid) ||
							(*ptLowGuy) == (*ptMid)) );
					}
					if ( ptMid <= ptLowGuy ) {
						do  {
							++ptLowGuy;
						} while ( ptLowGuy <= ptHigh && ((*ptLowGuy) < (*ptMid) ||
							(*ptLowGuy) == (*ptMid)) );
					}

					// ptLow < ptLowGuy <= ptHigh+1, A[i] <= A[ptMid] for ptLow <= i < ptLowGuy,
					//	either ptLowGuy > ptHigh or A[ptLowGuy] > A[ptMid]

					do  {
						--ptHighGuy;
					} while ( ptHighGuy > ptMid && 
						(*ptMid) < (*ptHighGuy) );

					// ptLow <= ptHighGuy < ptHigh, A[i] > A[ptMid] for ptHighGuy < i < ptHigh,
					//	either ptHighGuy == ptLow or A[ptHighGuy] <= A[ptMid]

					if ( ptHighGuy < ptLowGuy ) {
						break;
					}

					// if ptLowGuy > ptHigh or ptHighGuy == ptLow, then we would have exited, so
					//	A[ptLowGuy] > A[ptMid], A[ptHighGuy] <= A[ptMid],
					//	ptLowGuy <= ptHigh, ptHighGuy > ptLow

					ShortSwap( ptLowGuy, ptHighGuy );

					// If the partition element was moved, follow it.  Only need
					//	to check for ptMid == ptHighGuy, since before the swap,
					//	A[ptLowGuy] > A[ptMid] implies ptLowGuy != ptMid.

					if ( ptMid == ptHighGuy ) {
						ptMid = ptLowGuy;
					}

					// A[ptLowGuy] <= A[ptMid], A[ptHighGuy] > A[ptMid]; so condition at top
					//	of loop is re-established.
				}

				// A[i] <= A[ptMid] for ptLow <= i < ptLowGuy,
				//	A[i] > A[ptMid] for ptHighGuy < i < ptHigh,
				//	A[ptHigh] >= A[ptMid]
				//	ptHighGuy < ptLowGuy
				//	implying:
				//	ptHighGuy == ptLowGuy-1
				//	or ptHighGuy == ptHigh - 1, ptLowGuy == ptHigh + 1, A[ptHigh] == A[ptMid]

				// Find adjacent elements equal to the partition element.  The
				//	doubled loop is to avoid calling _psfFunc( ptMid, ptMid ), since some
				//	existing comparison functions don't work when passed the same value
				//	for both pointers.

				++ptHighGuy;
				if ( ptMid < ptHighGuy ) {
					do  {
						--ptHighGuy;
					} while ( ptHighGuy > ptMid &&
						(*ptHighGuy) == (*ptMid) );
				}
				if ( ptMid >= ptHighGuy ) {
					do  {
						--ptHighGuy;
					} while ( ptHighGuy > ptLow &&
						(*ptHighGuy) == (*ptMid) );
				}

				// Okay, now we have the following:
				//	ptHighGuy < ptLowGuy
				//	ptLow <= ptHighGuy <= ptHigh
				//	A[i] <= A[ptMid] for ptLow <= i <= ptHighGuy
				//	A[i]  == A[ptMid] for ptHighGuy < i < ptLowGuy
				//	A[i]  >  A[ptMid] for ptLowGuy <= i < ptHigh
				//	A[ptHigh] >= A[ptMid]

				// We've finished the partition; now we want to sort the subarrays
				//	[ptLow, ptHighGuy] and [ptLowGuy, ptHigh].
				//	We do the smaller one first to minimize stack usage.
				//	We only sort arrays of length 2 or more.

				if ( ptHighGuy - ptLow >= ptHigh - ptLowGuy ) {
					if ( ptLow < ptHighGuy ) {
						ptLowStack[i32StackPointer] = ptLow;
						ptHighStack[i32StackPointer] = ptHighGuy;
						++i32StackPointer;
					}

					if ( ptLowGuy < ptHigh ) {
						ptLow = ptLowGuy;
						goto Recurse;
					}
				}
				else {
					if ( ptLowGuy < ptHigh ) {
						ptLowStack[i32StackPointer] = ptLowGuy;
						ptHighStack[i32StackPointer] = ptHigh;
						++i32StackPointer;
					}

					if ( ptLow < ptHighGuy ) {
						ptHigh = ptHighGuy;
						goto Recurse;
					}
				}
			}

			// We have sorted the array except for any pending sorts on the stack.
			//	Check if there are any and do them.
			--i32StackPointer;
			if ( i32StackPointer >= 0 ) {
				ptLow = ptLowStack[i32StackPointer];
				ptHigh = ptHighStack[i32StackPointer];

				// Pop array from the stack.
				goto Recurse;
			}
			else { return; }
		}


	protected :
		// == Functions.
		/**
		 * Swap elements.
		 *
		 * \param _pui8A The first element.
		 * \param _pui8B The second element.
		 */
		template <typename _tType>
		static void LSE_CALL				ShortSwap( _tType * _ptA,
			_tType * _ptB ) {
			_tType tTemp = (*_ptA);
			(*_ptA) = (*_ptB);
			(*_ptB) = tTemp;
		}

	};


}	// namespace lstl

#pragma warning( pop )

#endif	// __LSTL_ALGORITHM_H__
