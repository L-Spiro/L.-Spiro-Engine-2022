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
 * Description: A simple vector.  It gives direct array access for the same speed as
 *	any array, but the sizes of the vector allocation counters can be specified, allowing
 *	for arrays of limited length but few bytes vs. arrays of large lengths but requiring
 *	more memory.
 * Additionally, the vectors save valuable space by allocating linearly as it grows rather
 *	than doubling in size each time.  The amount by which it grows can be specified.
 * This vector also cleans up after itself when removing items from the list.  When the
 *	list is less than half the allocated length the allocation is adjusted to half of what
 *	it is.
 * Note that the whole point of this template is to be as small as possible to save as much
 *	space as possible.  Consider a vector of a vector where the outer vector can be large and
 *	uses 32-bit indices while the inner vector is small enough to use 8-bit indices and saves
 *	6 bytes of RAM for every index in the outer vector.  Because of this focus on size, there
 *	is no allocator pointer in this template.  Instead, we add limited support for allocators
 *	with a macro that, when defined, uses CMemLib for allocation instead of the standard C
 *	functions.  This makes it possible to use the template with an allocator from this set of
 *	libraries, but it will only allocate from the main pool.  Luckily, this is enough for most
 *	situations.  If you strictly must allocate from a specific pool, you will implicitly have
 *	to sacrifice the size of your vector object, and a CVectorPoD should be used instead.
 */


#ifndef __LSTL_SVECTORPOD_H__
#define __LSTL_SVECTORPOD_H__

#include "../LSTLib.h"
#include "LSTLSVectorCrtp.h"
#include <cassert>
#include <new>


namespace lstl {

	/**
	 * Class CSVectorPoD
	 *
	 * Description: A simple vector.
	 */
	template <typename _tType, typename _tDataType = uint16_t, unsigned _uAllocSize = 512>
	class CSVectorPoD : public CSVectorCrtp<CSVectorPoD<_tType, _tDataType, _uAllocSize>, _tType, _tDataType, _uAllocSize> {
	public :
		// == Various constructors.
		LSE_CALLCTOR				CSVectorPoD() {
		}
		explicit LSE_CALLCTOR		CSVectorPoD( uint32_t _ui32Total ) :
			Parent() {
			Parent::Allocate( _ui32Total );
		}
		LSE_CALLCTOR				~CSVectorPoD() {
			Parent::Reset();
		}


	protected :
		// == Members.
		
	private :
		typedef CSVectorCrtp<CSVectorPoD<_tType, _tDataType, _uAllocSize>, _tType, _tDataType, _uAllocSize>
									Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// VARIATIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// 1-byte storage = max of 255 elements.
	template <typename _tType, unsigned _uAllocSize = 32>
	class CSVector8 : public CSVectorPoD<_tType, uint8_t, _uAllocSize> {
	public :
		LSE_CALLCTOR CSVector8() {}
		explicit LSE_CALLCTOR CSVector8( uint8_t _tTotal ) :
			CSVectorPoD<_tType, uint8_t, _uAllocSize>( _tTotal ) {}
	};

	// 2-byte storage = max of 65,535 elements.
	template <typename _tType, unsigned _uAllocSize = 32>
	class CSVector16 : public CSVectorPoD<_tType, uint16_t, _uAllocSize> {
	public :
		LSE_CALLCTOR CSVector16() {}
		explicit LSE_CALLCTOR CSVector16( uint16_t _tTotal ) :
			CSVectorPoD<_tType, uint16_t, _uAllocSize>( _tTotal ) {}
	};

	// 4-byte storage = max of 4,294,967,295 elements.
	template <typename _tType, unsigned _uAllocSize = 32>
	class CSVector32 : public CSVectorPoD<_tType, uint32_t, _uAllocSize> {
	public :
		LSE_CALLCTOR CSVector32() {}
		explicit LSE_CALLCTOR CSVector32( uint32_t _tTotal ) :
			CSVectorPoD<_tType, uint32_t, _uAllocSize>( _tTotal ) {}
	};

}	// namespace lstl

#endif	// __LSTL_SVECTORPOD_H__
