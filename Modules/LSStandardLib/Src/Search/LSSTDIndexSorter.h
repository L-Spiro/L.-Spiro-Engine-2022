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
 * Description: Sorts data by index references.  Instead of moving the actual data around, it creates an index table
 *	for that data and sorts the indices.  This is often faster than trying to sort data composed of many-byte elements.
 *	Uses a quick sort.  The data being sorted must provide < and == operators.
 */


#ifndef __LSSTD_INDEXSORTER_H__
#define __LSSTD_INDEXSORTER_H__

#include "../LSSTDStandardLib.h"
#include "LSSTDSearch.h"

namespace lsstd {

	/**
	 * Class CIndexSorter
	 * \brief Sorts data by index references.
	 *
	 * Description: Sorts data by index references.  Instead of moving the actual data around, it creates an index table
	 *	for that data and sorts the indices.  This is often faster than trying to sort data composed of many-byte elements.
	 *	Uses a quick sort.  The data being sorted must provide < and == operators.
	 */
	template <typename _tType>
	class CIndexSorter {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CIndexSorter() :
			m_pui32Indices( NULL ),
			m_ui32TotalIndices( 0 ),
			m_ui32AllocIndices( 0 ) {
		}
		LSE_CALLCTOR							~CIndexSorter() {
			delete [] m_pui32Indices;
		}


		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _isOther The object to copy.
		 * \return Returns the copied instance.
		 */
		CIndexSorter<_tType> & LSE_CALL			operator = ( const CIndexSorter<_tType> &_isOther ) {
			if ( _isOther.m_pui32Indices > m_pui32Indices ) {
				// Have to allocate more.
				delete [] m_pui32Indices;
				m_pui32Indices = new uint32_t[_isOther.m_pui32Indices];
				m_ui32AllocIndices = _isOther.m_pui32Indices;
			}
			m_ui32TotalIndices = _isOther.m_pui32Indices;
			for ( uint32_t I = m_ui32TotalIndices; I--; ) {
				m_pui32Indices[I] = _isOther.m_pui32Indices[I];
			}
			return (*this);
		}


		// == Functions.
		/**
		 * Sort the given data.
		 *
		 * \param _ptData The data to sort.
		 * \param _ui32Total Total objects to sort.
		 * \param _bCheckIfSorted If true, the sort first checks to see if items need sorting.
		 * \return Returns a reference to this object.
		 */
		CIndexSorter<_tType> & LSE_CALL			Sort( const _tType * _ptData, uint32_t _ui32Total, LSBOOL _bCheckIfSorted = true ) {
			PrepareFor( _ui32Total );
			if ( _ui32Total <= 1 ) { return (*this); }	// 0 or 1 items cannot be sorted.

			// Temporal coherence allows us to avoid work on already-sorted data.
			//	When unsorted data is passed in, usually this loop will exit very quickly, so
			//	there is almost no loss when data is never sorted.
			if ( _bCheckIfSorted ) {
				LSBOOL bSorted = true;
				for ( uint32_t I = _ui32Total - 1; I--; ) {
					if ( !(_ptData[I] < _ptData[I+1]) ) {
						bSorted = false;
						break;
					}
				}
				if ( bSorted ) { return (*this); }
			}

			// Sort it.
			CSearch::MergeSort( m_pui32Indices, m_ui32TotalIndices, sizeof( uint32_t ),
				SortComp, static_cast<void *>(const_cast<_tType *>(_ptData)) );
			return (*this);
		}

		/**
		 * Sort the given data using a quick sort.
		 *
		 * \param _ptData The data to sort.
		 * \param _ui32Total Total objects to sort.
		 * \param _bCheckIfSorted If true, the sort first checks to see if items need sorting.
		 * \return Returns a reference to this object.
		 */
		CIndexSorter<_tType> & LSE_CALL			QuickSort( const _tType * _ptData, uint32_t _ui32Total, LSBOOL _bCheckIfSorted = false ) {
			PrepareFor( _ui32Total );
			if ( _ui32Total <= 1 ) { return (*this); }	// 0 or 1 items cannot be sorted.

			// Temporal coherence allows us to avoid work on already-sorted data.
			//	When unsorted data is passed in, usually this loop will exit very quickly, so
			//	there is almost no loss when data is never sorted.
			if ( _bCheckIfSorted ) {
				LSBOOL bSorted = true;
				for ( uint32_t I = _ui32Total - 1; I--; ) {
					if ( !(_ptData[I] < _ptData[I+1]) ) {
						bSorted = false;
						break;
					}
				}
				if ( bSorted ) { return (*this); }
			}

			// Sort it.
			QSort( m_pui32Indices, _ptData, m_ui32TotalIndices );
			return (*this);
		}

		/**
		 * Sort the given data using an insertion sort.
		 *
		 * \param _ptData The data to sort.
		 * \param _ui32Total Total objects to sort.
		 * \return Returns a reference to this object.
		 */
		CIndexSorter<_tType> & LSE_CALL			InsertionSort( const _tType * _ptData, uint32_t _ui32Total ) {
			PrepareFor( _ui32Total );
			if ( _ui32Total <= 1 ) { return (*this); }	// 0 or 1 items cannot be sorted.

			// Sort it.
			InsertionSort( m_pui32Indices, _ptData, m_ui32TotalIndices );
			return (*this);
		}

		/**
		 * Gets a pointer to the (sorted) index data.
		 *
		 * \return Returns a pointer to the (sorted) index data.
		 */
		const uint32_t * LSE_CALL				GetIndices() const { return m_pui32Indices; }

		/**
		 * Prepares to sort the given number of elements, but does not sort them.
		 *
		 * \param PARM The number of elements to prepare to sort.
		 */
		void LSE_CALL							PrepareFor( uint32_t _ui32Total ) {
			if ( _ui32Total > m_ui32AllocIndices ) {
				// Up-size the allocated amount.
				delete [] m_pui32Indices;
				m_pui32Indices = new uint32_t[_ui32Total];
				m_ui32AllocIndices = _ui32Total;
				m_ui32TotalIndices = 0;
			}
			if ( m_ui32TotalIndices > _ui32Total ) {
				for ( uint32_t I = 0; I < _ui32Total; ++I ) {
					// Add indices that were not added before.
					m_pui32Indices[I] = I;
				}
			}
			else {
				for ( uint32_t I = m_ui32TotalIndices; I < _ui32Total; ++I ) {
					// Add indices that were not added before.
					m_pui32Indices[I] = I;
				}
			}
			m_ui32TotalIndices = _ui32Total;
		}


	protected :
		// == Members.
		/**
		 * Sorted indices.
		 */
		uint32_t *								m_pui32Indices;

		/**
		 * Total indices.
		 */
		uint32_t								m_ui32TotalIndices;

		/**
		 * Total indices allocated.
		 */
		uint32_t								m_ui32AllocIndices;


		// == Functions.
		/**
		 * The comparison function.
		 *
		 * \param _pvContext Holds a pointer to the data our indices reference.
		 * \param _pvLeft The left operand.
		 * \param _pvRight The right operand.
		 * \return Returns -1 if _pvLeft < _pvRight, 0 if _pvLeft == _pvRight, and 1 otherwise.
		 */
		static int32_t LSE_CCALL				SortComp( void * _pvContext,
			const void * _pvLeft, const void * _pvRight ) {
			_tType * ptSrc = static_cast<_tType *>(_pvContext);
			const _tType * ptLeft = &ptSrc[(*static_cast<const uint32_t *>(_pvLeft))];
			const _tType * ptRight = &ptSrc[(*static_cast<const uint32_t *>(_pvRight))];
			if ( (*ptLeft) < (*ptRight) ) { return -1; }
			if ( (*ptLeft) == (*ptRight) ) { return 0; }
			return 1;
		}

		/**
		 * Swap elements.
		 *
		 * \param _pui8A The first element.
		 * \param _pui8B The second element.
		 */
		template <typename _tSwapType>
		static void LSE_CALL					ShortSwap( _tSwapType * _ptA,
			_tSwapType * _ptB ) {
			_tSwapType tTemp = (*_ptA);
			(*_ptA) = (*_ptB);
			(*_ptB) = tTemp;
		}

		/**
		 * A short sort for indexed data.
		 *
		 * \param _pui32Indices The indices to sort.
		 * \param _ptValues Source data.
		 * \param _ui32Total Total indices to which _pui32Indices points.
		 */
		static void LSE_CALL					ShortSort( uint32_t * _pui32Indices, const _tType * _ptValues, uintptr_t _uiptrTotal ) {
			uint32_t * pui32Low = _pui32Indices;
			uint32_t * pui32Hi = _pui32Indices + _uiptrTotal;
			uint32_t * pui32P, * pui32Max;
			// Note: In assertions below, i and j are alway inside original bounds of the
			//	array to sort.

			while ( pui32Hi > pui32Low ) {
				// A[i] <= A[j] for i <= j, j > hi
				pui32Max = pui32Low;
				for ( pui32P = pui32Low + 1; pui32P <= pui32Hi; ++pui32P ) {
					// A[i] <= A[pui32Max] for lo <= i < p
					if ( _ptValues[(*pui32Max)] < _ptValues[(*pui32P)] ) {
						pui32Max = pui32P;
					}
					// A[i] <= A[pui32Max] for lo <= i <= p
				}

				// A[i] <= A[pui32Max] for lo <= i <= hi
				uint32_t ui32Temp = (*pui32Max);
				(*pui32Max) = (*pui32Hi);
				(*pui32Hi) = ui32Temp;

				// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
				--pui32Hi;

				// A[i] <= A[j] for i <= j, j > hi, loop top condition established
			}
			// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
			//	so array is sorted.
		}

		/**
		 * A quick sort.
		 *
		 * \param _pui32Indices The indices to sort.
		 * \param _ptValues The values to sort in-place.
		 * \param _uiptrTotal Total elements to which _ptValues and _pui32Indices point.
		 */
		static void LSE_CALL					QSort( uint32_t * _pui32Indices, const _tType * _ptValues, uintptr_t _uiptrTotal ) {
#define LSSTL_STACKSIZE (8 * sizeof( void * ) - 2)

			uint32_t * ptLow, * ptHigh;
			uint32_t * ptMid;
			uint32_t * ptLowGuy, * ptHighGuy;
			uintptr_t uiptrSize;
			uint32_t * ptLowStack[LSSTL_STACKSIZE], * ptHighStack[LSSTL_STACKSIZE];
			int32_t i32StackPointer;
#undef LSSTL_STACKSIZE

			// Validate the parameters.
			if ( !(_ptValues != NULL || _uiptrTotal == 0) ) { return; }
			if ( _uiptrTotal < 2 ) { return; }

			// Initialize the stack.
			i32StackPointer = 0;

			// Initialize the limits.
			ptLow = _pui32Indices;
			ptHigh = _pui32Indices + (_uiptrTotal - 1);


	Recurse :

			// Number of elements to sort.
			uiptrSize = static_cast<uintptr_t>(ptHigh - ptLow) + 1;

			// Below a certain size, it is faster to use an O(n^2) sorting method.
			if ( uiptrSize <= 8 ) {
				ShortSort( ptLow, _ptValues, uiptrSize - 1 );
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
				if ( _ptValues[(*ptMid)] < _ptValues[(*ptLow)] ) {
					ShortSwap( ptLow, ptMid );
				}
				if ( _ptValues[(*ptHigh)] < _ptValues[(*ptLow)] ) {
					ShortSwap( ptLow, ptHigh );
				}
				if ( _ptValues[(*ptHigh)] < _ptValues[(*ptMid)] ) {
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
						} while ( ptLowGuy < ptMid && (_ptValues[(*ptLowGuy)] < _ptValues[(*ptMid)] ||
							_ptValues[(*ptLowGuy)] == _ptValues[(*ptMid)]) );
					}
					if ( ptMid <= ptLowGuy ) {
						do  {
							++ptLowGuy;
						} while ( ptLowGuy <= ptHigh && (_ptValues[(*ptLowGuy)] < _ptValues[(*ptMid)] ||
							_ptValues[(*ptLowGuy)] == _ptValues[(*ptMid)]) );
					}

					// ptLow < ptLowGuy <= ptHigh+1, A[i] <= A[ptMid] for ptLow <= i < ptLowGuy,
					//	either ptLowGuy > ptHigh or A[ptLowGuy] > A[ptMid]

					do  {
						--ptHighGuy;
					} while ( ptHighGuy > ptMid && 
						_ptValues[(*ptMid)] < _ptValues[(*ptHighGuy)] );

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
						_ptValues[(*ptHighGuy)] == _ptValues[(*ptMid)] );
				}
				if ( ptMid >= ptHighGuy ) {
					do  {
						--ptHighGuy;
					} while ( ptHighGuy > ptLow &&
						_ptValues[(*ptHighGuy)] == _ptValues[(*ptMid)] );
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

		/**
		 * Use an insertion sort to sort the array of indices.
		 *
		 * \param _pui32Indices The indices to sort.
		 * \param _ptValues The values to sort in-place.
		 * \param _uiptrTotal Total elements to which _ptValues and _pui32Indices point.
		 */
		static void LSE_CALL					InsertionSort( uint32_t * _pui32Indices, const _tType * _ptValues, uintptr_t _uiptrTotal ) {
			if ( _uiptrTotal <= 1 ) { return; }
			uint32_t * pui32Cur = _pui32Indices + 1;
			uint32_t * pui32End = _pui32Indices + _uiptrTotal;
			while ( pui32Cur < pui32End ) {
				const _tType * ptKey = &_ptValues[(*pui32Cur)];
				for ( uint32_t * pui32NextDown = pui32Cur - 1; pui32NextDown >= _pui32Indices && (*ptKey) < _ptValues[(*pui32NextDown)]; --pui32NextDown ) {
					uint32_t ui32Temp = (*pui32NextDown);
					(*pui32NextDown) = pui32NextDown[1];
					pui32NextDown[1] = ui32Temp;
				}
				++pui32Cur;
			}
		}
	};

}	// namespace lsstd

#endif	// __LSSTD_INDEXSORTER_H__
