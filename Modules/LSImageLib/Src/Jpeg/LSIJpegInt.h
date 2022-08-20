/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __LSI_JPEGINT_H__
#define __LSI_JPEGINT_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CJpegInt
	 * \brief Implementation of operations on integers (INT).
	 *
	 * Description: Implementation of operations on integers (INT).
	 */
	class CJpegInt {
	public :
		// == Functions.
		/**
		 * Gets the minimum of two integers.
		 *
		 * \return Returns _i32A if _i32A < _i32B else _i32B.
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_min( int32_t _i32A, int32_t _i32B );

		/**
		 * Gets the maximum of two integers.
		 *
		 * \return Returns _i32A if _i32A > _i32B else _i32B.
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_max( int32_t _i32A, int32_t _i32B );

		/**
		 * Clamp an integer inside an interval.
		 *
		 * \return
		 * <ul>
		 * <li>Returns _i32A if ( _i32Min < _i32A < _i32Max ).
		 * <li>Returns _i32Max if ( _i32A > _i32Max ).
		 * <li>Returns _i32Min if ( _i32A < _i32Min ).
		 * </ul>
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_clamp( int32_t _i32A, int32_t _i32Min, int32_t _i32Max );

		/**
		 * Gets the absolute value of integer.
		 *
		 * \return Returns the absolute value of integer.
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_abs( int32_t _i32A );

		/**
		 * Divide an integer and round upwards.
		 *
		 * \return Returns _i32A divided by _i32B.
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_ceildiv( int32_t _i32A, int32_t _i32B );

		/**
		 * Divide an integer by a power of 2 and round upwards.
		 *
		 * \return Returns _i32A divided by 2^_i32B.
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_ceildivpow2( int32_t _i32A, int32_t _i32B );

		/**
		 * Divide an integer by a power of 2 and round downwards.
		 *
		 * \return Returns _i32A divided by 2^_i32B.
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_floordivpow2( int32_t _i32A, int32_t _i32B );

		/**
		 * Gets logarithm of an integer and round downwards.
		 *
		 * \return Returns log2( _i32A ).
		 */
		static LSE_INLINE int32_t LSE_FCALL				int_floorlog2( int32_t _i32A );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the minimum of two integers.
	 *
	 * \return Returns _i32A if _i32A < _i32B else _i32B.
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_min( int32_t _i32A, int32_t _i32B ) {
		return _i32A < _i32B ? _i32A : _i32B;
	}

	/**
	 * Gets the maximum of two integers.
	 *
	 * \return Returns _i32A if _i32A > _i32B else _i32B.
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_max( int32_t _i32A, int32_t _i32B ) {
		return (_i32A > _i32B) ? _i32A : _i32B;
	}

	/**
	 * Clamp an integer inside an interval.
	 *
	 * \return
	 * <ul>
	 * <li>Returns _i32A if ( _i32Min < _i32A < _i32Max ).
	 * <li>Returns _i32Max if ( _i32A > _i32Max ).
	 * <li>Returns _i32Min if ( _i32A < _i32Min ).
	 * </ul>
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_clamp( int32_t _i32A, int32_t _i32Min, int32_t _i32Max ) {
		if ( _i32A < _i32Min ) { return _i32Min; }
		if ( _i32A > _i32Max ) { return _i32Max; }
		return _i32A;
	}

	/**
	 * Gets the absolute value of integer.
	 *
	 * \return Returns the absolute value of integer.
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_abs( int32_t _i32A ) {
		return _i32A < 0 ? -_i32A : _i32A;
	}

	/**
	 * Divide an integer and round upwards.
	 *
	 * \return Returns _i32A divided by _i32B.
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_ceildiv( int32_t _i32A, int32_t _i32B ) {
		return (_i32A + _i32B - 1) / _i32B;
	}

	/**
	 * Divide an integer by a power of 2 and round upwards.
	 *
	 * \return Returns _i32A divided by 2^_i32B.
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_ceildivpow2( int32_t _i32A, int32_t _i32B ) {
		return (_i32A + (1 << _i32B) - 1) >> _i32B;
	}

	/**
	 * Divide an integer by a power of 2 and round downwards.
	 *
	 * \return Returns _i32A divided by 2^_i32B.
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_floordivpow2( int32_t _i32A, int32_t _i32B ) {
		return _i32A >> _i32B;
	}

	/**
	 * Gets logarithm of an integer and round downwards.
	 *
	 * \return Returns log2( _i32A ).
	 */
	LSE_INLINE int32_t LSE_FCALL CJpegInt::int_floorlog2( int32_t _i32A ) {
		int32_t i32L;
		for ( i32L = 0; _i32A > 1; i32L++ ) {
			_i32A >>= 1;
		}
		return i32L;
	}

}	// namespace lsi

#endif	// __LSI_JPEGINT_H__
