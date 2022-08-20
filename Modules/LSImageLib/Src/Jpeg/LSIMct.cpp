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

#include "LSIMct.h"
#include "LSIJpegFixed.h"


namespace lsi {

	// == Members.
	/**
	 * This table contains the normals of the basis function of the reversible MCT.
	 */
	const double CMct::m_dNorms[3] = {
		1.732,
		0.8292,
		0.8292
	};

	/**
	 * This table contains the norms of the basis function of the irreversible MCT.
	 */
	const double CMct::m_dNormsReal[3] = {
		1.732,
		1.805,
		1.573
	};

	// == Functions.
	/**
	 * Apply a reversible multi-component transform to an image.
	 *
	 * \param _pi32C0 Samples for red component
	 * \param _pi32C1 Samples for green component
	 * \param _pi32C2 Samples blue component
	 * \param _i32N Number of samples for each component
	 */
	void LSE_CALL CMct::mct_encode( int32_t * _pi32C0, int32_t * _pi32C1, int32_t * _pi32C2, int32_t _i32N ) {
		for ( int32_t I = 0; I < _i32N; ++I ) {
			int32_t i32R = _pi32C0[I];
			int32_t i32G = _pi32C1[I];
			int32_t i32B = _pi32C2[I];
			int32_t i32Y = (i32R + (i32G * 2) + i32B) >> 2;
			int32_t i32U = i32B - i32G;
			int32_t i32V = i32R - i32G;
			_pi32C0[I] = i32Y;
			_pi32C1[I] = i32U;
			_pi32C2[I] = i32V;
		}
	}

	/**
	 * Apply a reversible multi-component inverse transform to an image.
	 *
	 * \param _pi32C0 Samples for luminance component.
	 * \param _pi32C1 Samples for red chrominance component.
	 * \param _pi32C2 Samples for blue chrominance component.
	 * \param _i32N Number of samples for each component.
	 */
	void LSE_CALL CMct::mct_decode( int32_t * _pi32C0, int32_t * _pi32C1, int32_t * _pi32C2, int32_t _i32N ) {
		for ( int32_t I = 0; I < _i32N; ++I ) {
			int32_t i32Y = _pi32C0[I];
			int32_t i32U = _pi32C1[I];
			int32_t i32V = _pi32C2[I];
			int32_t i32G = i32Y - ((i32U + i32V) >> 2);
			int32_t i32R = i32V + i32G;
			int32_t i32B = i32U + i32G;
			_pi32C0[I] = i32R;
			_pi32C1[I] = i32G;
			_pi32C2[I] = i32B;
		}
	}

	/**
	 * Gets norm of the basis function used for the reversible multi-component transform.
	 *
	 * \param _i32CompNo Number of the component (0->Y, 1->U, 2->V).
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	double LSE_CALL CMct::mct_getnorm( int32_t _i32CompNo ) {
		return m_dNorms[_i32CompNo];
	}

	/**
	 * Apply an irreversible multi-component transform to an image.
	 *
	 * \param _pi32C0 Samples for red component.
	 * \param _pi32C1 Samples for green component.
	 * \param _pi32C2 Samples blue component.
	 * \param _i32N Number of samples for each component.
	 */
	void LSE_CALL CMct::mct_encode_real( int32_t * _pi32C0, int32_t * _pi32C1, int32_t * _pi32C2, int32_t _i32N ) {
		for ( int32_t I = 0; I < _i32N; ++I ) {
			int32_t i32R = _pi32C0[I];
			int32_t i32G = _pi32C1[I];
			int32_t i32B = _pi32C2[I];
			int32_t i32Y = CJpegFixed::Mul( i32R, 2449 ) + CJpegFixed::Mul( i32G, 4809 ) + CJpegFixed::Mul( i32B, 934 );
			int32_t i32U = -CJpegFixed::Mul( i32R, 1382 ) - CJpegFixed::Mul( i32G, 2714 ) + CJpegFixed::Mul( i32B, 4096 );
			int32_t i32V = CJpegFixed::Mul( i32R, 4096 ) - CJpegFixed::Mul( i32G, 3430 ) - CJpegFixed::Mul( i32B, 666 );
			_pi32C0[I] = i32Y;
			_pi32C1[I] = i32U;
			_pi32C2[I] = i32V;
		}
	}

	/**
	 * Apply an irreversible multi-component inverse transform to an image.
	 *
	 * \param _pfC0 Samples for luminance component.
	 * \param _pfC1 Samples for red chrominance component.
	 * \param _pfC2 Samples for blue chrominance component.
	 * \param _i32N Number of samples for each component.
	 */
	void LSE_CALL CMct::mct_decode_real( float * _pfC0, float * _pfC1, float * _pfC2, int32_t _i32N ) {
		for ( int32_t I = 0; I < _i32N; ++I ) {
			float fY = _pfC0[I];
			float fU = _pfC1[I];
			float fV = _pfC2[I];
			float fR = fY + (fV * 1.402f);
			float fG = fY - (fU * 0.34413f) - (fV * (0.71414f));
			float fB = fY + (fU * 1.772f);
			_pfC0[I] = fR;
			_pfC1[I] = fG;
			_pfC2[I] = fB;
		}
	}

	/**
	 * Gets norm of the basis function used for the irreversible multi-component transform.
	 *
	 * \param _i32CompNo Number of the component (0->Y, 1->U, 2->V).
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	double LSE_CALL CMct::mct_getnorm_real( int32_t _i32CompNo ) {
		return m_dNormsReal[_i32CompNo];
	}

}	// namespace lsi
