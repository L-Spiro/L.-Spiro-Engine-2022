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
 * Description: A templated "std::memcmp() == 0" routine for small sets of data.  The purpose is to fully inline
 *	the comparisons while making the sizes of each compare optimal without using run-time branching.
 *
 * Proper alignment is assumed.
 */


#ifndef __LSTL_MEMPCOMPF_H__
#define __LSTL_MEMPCOMPF_H__

#include "../LSTLib.h"

namespace lstl {

	/**
	 * Class CMemCmpF
	 * \brief A templated "std::memcmp() == 0" routine for small sets of data.
	 *
	 * Description: A templated "std::memcmp() == 0" routine for small sets of data.  The purpose is to fully inline
	 *	the comparisons while making the sizes of each compare optimal without using run-time branching.
	 *
	 * Proper alignment is assumed.
	 */
	class CMemCmpF {
		// All is public.  This class has no secrets.
	public :


		// == Functions.
		/**
		 * Compare memory in such a way that only returns true if the memory exactly matches or false otherwise.
		 *
		 * \param _pvLeft The left data to compare.
		 * \param _pvRight The right data to compare.
		 * \param _uiptrSize The size of the data to compare.
		 * \return Returns true if the data exactly matches, false otherwise.
		 */
		static bool LSE_FCALL							Cmp( const void * _pvLeft, const void * _pvRight, const uintptr_t _uiptrSize ) {
#define LSTL_TYPE( TYPE, VAR )		reinterpret_cast<const TYPE *>(VAR)
			// Check 64 bits at a time.
#define LSTL_CHECK( TYPE )																								\
	if ( _uiptrSize >= sizeof( TYPE ) ) {																				\
		if ( (*LSTL_TYPE( TYPE, _pvLeft )) != (*LSTL_TYPE( TYPE, _pvRight )) ) { return false; }						\
		return Cmp( &LSTL_TYPE( TYPE, _pvLeft )[1], &LSTL_TYPE( TYPE, _pvRight )[1], _uiptrSize - sizeof( TYPE ) );		\
	}
			LSTL_CHECK( uint64_t )
			LSTL_CHECK( uint32_t )
			LSTL_CHECK( uint16_t )
			LSTL_CHECK( uint8_t )
			return true;
#undef LSTL_CHECK
#undef LSTL_TYPE
		}


	};

}	// namespace lstl

#endif	// __LSTL_MEMPCOMPF_H__
