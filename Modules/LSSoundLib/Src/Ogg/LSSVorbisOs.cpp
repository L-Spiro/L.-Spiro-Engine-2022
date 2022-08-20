/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: #ifdef jail to whip a few platforms into the UNIX ideal.
 last mod: $Id: os.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include "LSSVorbisOs.h"
#if (defined( _MSC_VER ) && defined( _WIN64 )) || (defined( __GNUC__ ) && defined ( __x86_64__ ))
#include <emmintrin.h>
#endif	// #if (defined( _MSC_VER ) && defined( _WIN64 )) || (defined( __GNUC__ ) && defined ( __x86_64__ ))


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _dVal Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisOs::vorbis_ftoi( double _dVal ) {
#if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
		int32_t i32Value;
		__asm__( "fistl %0": "=m"(i32Value) : "t"(_dVal) );
		return i32Value;
#elif defined( _MSC_VER ) && !defined( _WIN64 ) && !defined( _WIN32_WCE )
		int32_t i32Value;
        __asm {
			fld _dVal
			fistp i32Value
        }
        return i32Value;
#elif (defined( _MSC_VER ) && defined( _WIN64 )) || (defined( __GNUC__ ) && defined ( __x86_64__ ))
		return _mm_cvtsd_si32( _mm_load_sd( &_dVal ) );
#else
		return static_cast<int32_t>(::floor( _dVal + 0.5 ));
#endif	// #if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvfcFpu Undocumented parameter from the Vorbis library.
	 */
	void CVorbisOs::vorbis_fpu_setround(
#if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
		vorbis_fpu_control * _pvfcFpu
#else
		vorbis_fpu_control *
#endif		// #if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
	) {
#if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
		int16_t i16Ret;
		int16_t i16Temp = 0;
		__asm__ __volatile__( "fnstcw %0\n\t"
			"movw %0,%%dx\n\t"
			"andw $62463,%%dx\n\t"
			"movw %%dx,%1\n\t"
			"fldcw %1\n\t":"=m"(i16Ret):"m"(i16Temp): "dx" );
		(*_pvfcFpu) = i16Ret;
#elif defined( _MSC_VER ) && !defined( _WIN64 ) && !defined( _WIN32_WCE )

#elif (defined( _MSC_VER ) && defined( _WIN64 )) || (defined( __GNUC__ ) && defined ( __x86_64__ ))

#else

#endif	// #if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _vfcFpu Undocumented parameter from the Vorbis library.
	 */
	void CVorbisOs::vorbis_fpu_restore(
#if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
		vorbis_fpu_control _vfcFpu
#else
		vorbis_fpu_control
#endif	// #if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
	) {
#if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
		__asm__ __volatile__( "fldcw %0":: "m"(_vfcFpu) );
#elif defined( _MSC_VER ) && !defined( _WIN64 ) && !defined( _WIN32_WCE )
#elif (defined( _MSC_VER ) && defined( _WIN64 )) || (defined( __GNUC__ ) && defined ( __x86_64__ ))
#else

#endif	// #if defined( __i386__ ) && defined( __GNUC__ ) && !defined( __BEOS__ )
	}

}	// namespace lss
