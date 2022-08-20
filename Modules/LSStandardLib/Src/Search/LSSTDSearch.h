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


#ifndef __LSSTD_SEARCH_H__
#define __LSSTD_SEARCH_H__

#include "../LSSTDStandardLib.h"

namespace lsstd {

	/**
	 * Class CSearch
	 * \brief Standard search-and-sort routines.
	 *
	 * Description: Standard search-and-sort routines.
	 */
	class CSearch {
	public :
		// == Types.
		/**
		 * BSearch callback function prototype.
		 */
		typedef int32_t (LSE_CCALL *		PfBSearchFunc)( void * _pvContext,
			const void * _pvLeft, const void * _pvRight );

		/**
		 * MergeSort, QSort and ShortSort callback function prototype.
		 */
		typedef int32_t (LSE_CCALL *		PfSortFunc)( void * _pvContext,
			const void * _pvLeft, const void * _pvRight );


		// == Functions.
		/**
		 * Perform a binary search on an array of data in memory.
		 *
		 * \param _pvKey Object for which to search.
		 * \param _pvBase Pointer to the base of the data to search.
		 * \param _uiptrNum Number of elements in the data to which _pvBase points.
		 * \param _uiptrWidth Width of the elements in the data to which _pvBase points.
		 * \param _pbsfFunc Callback function that compares two elements. The first argument
		 *	is the _pvContext pointer. The second argument is a pointer to the _pvKey for the search.
		 *	The third argument is a pointer to the array element to be compared with _pvKey.
		 * \param _pvContext A pointer to an object that can be accessed in the comparison function.
		 * \return Returns a pointer to an occurrence of _pvKey in the array to which _pvBase points.
		 *	If _pvKey is not found, the function returns NULL.  If the array is not in ascending sort
		 *	order or contains duplicate records with identical keys, the result is unpredictable.
		 */
		static void * LSE_CALL			BSearch( const void * _pvKey,
			const void * _pvBase,
			uintptr_t _uiptrNum,
			uintptr_t _uiptrWidth,
			PfBSearchFunc _pbsfFunc,
			void * _pvContext );

		/**
		 * A quick sort.
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
		static void LSE_CALL				QSort( void * _pvBase,
			uintptr_t _uiptrNum,
			uintptr_t _uiptrWidth,
			PfSortFunc _psfFunc,
			void * _pvContext );

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
		static void LSE_CALL				MergeSort( void * _pvBase,
			uintptr_t _uiptrNum,
			uintptr_t _uiptrWidth,
			PfSortFunc _psfFunc,
			void * _pvContext );

		/**
		 * A short sort.  This works faster on smaller data sets.  QSort() calls this automatically, so
		 *	there is no need to use this directly unless you know your data is always under 9 elements
		 *	in length.
		 *
		 * \param _pui8Low Start of target array.
		 * \param _pui8High End of the array.
		 * \param _uiptrWidth Element size in bytes.
		 * \param _psfFunc Comparison function. The first argument is the context pointer. The second
		 *	argument is a pointer to the left element to compare. The third argument is a pointer to the 
		 *	right element to be compared against the left.
		 * \param _pvContext A pointer to a context, which can be any object that the compare routine
		 *	needs to access.
		 */
		static void LSE_CALL				ShortSort( uint8_t * _pui8Low,
			uint8_t * _pui8High,
			uintptr_t _uiptrWidth,
			PfSortFunc _psfFunc,
			void * _pvContext );

	protected :
		// == Functions.
		/**
		 * Perform a byte-by-byte swap to avoid alignment issues.
		 *
		 * \param _pui8A The first element.
		 * \param _pui8B The second element.
		 * \param _uiptrSize Size of the elements to be swapped.
		 */
		static void LSE_CALL				ShortSwap( uint8_t * _pui8A,
			uint8_t * _pui8B,
			uintptr_t _uiptrSize );

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
		static void LSE_CALL				Merge( uint8_t * _pui8Left, uint32_t _ui32LeftTotal,
			uint8_t * _pui8Right, uint32_t _ui32RightTotal,
			void * _pvDest,
			uintptr_t _uiptrWidth,
			PfSortFunc _psfFunc, void * _pvContext );
	
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Perform a byte-by-byte swap to avoid alignment issues.
	 *
	 * \param _pui8A The first element.
	 * \param _pui8B The second element.
	 * \param _uiptrSize Size of the elements to be swapped.
	 */
	LSE_INLINE void LSE_CALL CSearch::ShortSwap( uint8_t * _pui8A, 
		uint8_t * _pui8B,
		uintptr_t _uiptrSize ) {
		if ( _pui8A != _pui8B ) {
			if ( (_uiptrSize & 0x7) == 0 ) {
				// Copy 64 bits at a time.
				uint64_t * pui64A = reinterpret_cast<uint64_t *>(_pui8A);
				uint64_t * pui64B = reinterpret_cast<uint64_t *>(_pui8B);
				while ( _uiptrSize ) {
					uint64_t ui64Temp = (*pui64A);
					(*pui64A++) = (*pui64B);
					(*pui64B++) = ui64Temp;

					_uiptrSize -= sizeof( uint64_t );
				}
			}
			else if ( (_uiptrSize & 0x3) == 0 ) {
				// Copy 32 bits at a time.
				uint32_t * pui32A = reinterpret_cast<uint32_t *>(_pui8A);
				uint32_t * pui32B = reinterpret_cast<uint32_t *>(_pui8B);
				while ( _uiptrSize ) {
					uint32_t ui32Temp = (*pui32A);
					(*pui32A++) = (*pui32B);
					(*pui32B++) = ui32Temp;

					_uiptrSize -= sizeof( uint32_t );
				}
			}
			else if ( (_uiptrSize & 0x1) == 0 ) {
				// Copy 16 bits at a time.
				uint16_t * pui16A = reinterpret_cast<uint16_t *>(_pui8A);
				uint16_t * pui16B = reinterpret_cast<uint16_t *>(_pui8B);
				while ( _uiptrSize ) {
					uint16_t ui16Temp = (*pui16A);
					(*pui16A++) = (*pui16B);
					(*pui16B++) = ui16Temp;

					_uiptrSize -= sizeof( uint16_t );
				}
			}
			else {
				while ( _uiptrSize-- ) {
					uint8_t ui8Temp = (*_pui8A);
					(*_pui8A++) = (*_pui8B);
					(*_pui8B++) = ui8Temp;
				}
			}
		}
	}

}	// namespace lsstd

#endif	// __LSSTD_SEARCH_H__
