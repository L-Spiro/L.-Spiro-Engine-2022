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


#ifndef __LSI_JPEGFIXED_H__
#define __LSI_JPEGFIXED_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CJpegFixed
	 * \brief Implementation of operations of specific multiplication (FIX).
	 *
	 * Description: Implementation of operations of specific multiplication (FIX).
	 */
	class CJpegFixed {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Multiply two fixed-precision rational numbers.
		 *
		 * \param _i32A Left operand.
		 * \param _i32B Right operand.
		 * \return Returns _i32A * _i32B.
		 */
		static LSE_INLINE int32_t LSE_CALL								Mul( int32_t _i32A, int32_t _i32B );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Multiply two fixed-precision rational numbers.
	 *
	 * \param _i32A Left operand.
	 * \param _i32B Right operand.
	 * \return Returns _i32A * _i32B.
	 */
	LSE_INLINE int32_t LSE_CALL CJpegFixed::Mul( int32_t _i32A, int32_t _i32B ) {
		int64_t i64Temp = static_cast<int64_t>(_i32A) * static_cast<int64_t>(_i32B);
		i64Temp += i64Temp & 4096;
		return static_cast<int32_t>(i64Temp >> 13);
	}

}	// namespace lsi

#endif	// __LSI_JPEGFIXED_H__
