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


#ifndef __LSI_DWT_H__
#define __LSI_DWT_H__

#include "../LSIImageLib.h"
#include "LSITcd.h"
#include "LSIJ2k.h"

namespace lsi {

	/**
	 * Class CDwt
	 * \brief Implementation of a discrete wavelet transform (DWT).
	 *
	 * Description: Implementation of a discrete wavelet transform (DWT).
	 */
	class CDwt {
	public :
		// == Functions.
		/**
		 * Forward 5-3 wavelet tranform in 2-D.
		 *	Apply a reversible DWT transform to a component of an image.
		 *
		 * \param _pttTileC Tile component information (current tile).
		 */
		static void LSE_CALL						dwt_encode( CTcd::opj_tcd_tilecomp_t * _pttTileC );

		/**
		 * Inverse 5-3 wavelet tranform in 2-D.
		 *	Apply a reversible inverse DWT transform to a component of an image.
		 *
		 * \param _pttTileC Tile component information (current tile).
		 * \param _i32NumRes Number of resolution levels to decode.
		 */
		static void LSE_CALL						dwt_decode( CTcd::opj_tcd_tilecomp_t * _pttTileC, int32_t _i32NumRes );

		/**
		 * Gets the gain of a subband for the reversible 5-3 DWT.
		 *
		 * \param _i32Orient Number that identifies the subband (0->LL, 1->HL, 2->LH, 3->HH).
		 * \return Returns 0 if _i32Orient = 0, returns 1 if _i32Orient = 1 or 2, returns 2 otherwise.
		 */
		static int32_t LSE_CALL						dwt_getgain( int32_t _i32Orient );

		/**
		 * Gets the norm of a wavelet function of a subband at a specified level for the reversible 5-3 DWT.
		 *
		 * \param _i32Level Level of the wavelet function.
		 * \param _i32Orient Band of the wavelet function.
		 * \return Returns the norm of the wavelet function.
		 */
		static double LSE_CALL					dwt_getnorm( int32_t _i32Level, int32_t _i32Orient );

		/**
		 * Forward 9-7 wavelet transform in 2-D.
		 *
		 * Apply an irreversible DWT transform to a component of an image.
		 * \param _pttTileC Tile component information (current tile).
		 */
		static void LSE_CALL						dwt_encode_real( CTcd::opj_tcd_tilecomp_t * _pttTileC );

		/**
		 * Inverse 9-7 wavelet transform in 2-D.
		 *
		 * Apply an irreversible inverse DWT transform to a component of an image.
		 * \param _pttTileC Tile component information (current tile).
		 * \param _i32NumRes Number of resolution levels to decode.
		 */
		static void LSE_CALL						dwt_decode_real( CTcd::opj_tcd_tilecomp_t * _pttTileC, int32_t _i32NumRes );

		/**
		 * Gets the gain of a subband for the irreversible 9-7 DWT.
		 *
		 * \param _i32Orient Number that identifies the subband (0->LL, 1->HL, 2->LH, 3->HH).
		 * \return Returns the gain of the 9-7 wavelet transform.
		 */
		static int32_t LSE_CALL						dwt_getgain_real( int32_t _i32Orient );

		/**
		 * Gets the norm of a wavelet function of a subband at a specified level for the irreversible 9-7 DWT.
		 *
		 * \param _i32Level Level of the wavelet function.
		 * \param _i32Orient Band of the wavelet function.
		 * \return Returns the norm of the 9-7 wavelet.
		 */
		static double LSE_CALL					dwt_getnorm_real( int32_t _i32Level, int32_t _i32Orient );

		/**
		 * Explicit calculation of the Quantization Stepsizes.
		 *
		 * \param _ptTccp Tile-component coding parameters.
		 * \param _i32Prec Precint analyzed.
		 */
		static void LSE_CALL						dwt_calc_explicit_stepsizes( CJ2k::opj_tccp_t * _ptTccp, int32_t _i32Prec );


	protected :
		// == Types.
		/**
		 * Undocumented OpenJpeg Library structure.
		 */
		typedef struct dwt_local {
			int32_t *								mem;
			int32_t									dn;
			int32_t									sn;
			int32_t									cas;
		} dwt_t;

		/**
		 * Undocumented OpenJpeg Library structure.
		 */
		typedef union {
			float									f[4];
		} v4;

		/**
		 * Undocumented OpenJpeg Library structure.
		 */
		typedef struct v4dwt_local {
			v4 *									wavelet;
			int32_t									dn;
			int32_t									sn;
			int32_t									cas;
		} v4dwt_t;

		/**
		 * Virtual function type for wavelet transform in 1-D.
		 *
		 * \param _pdV Undocumented parameter from the OpenJpeg Library.
		 */
		typedef void (LSE_CALL *					DWT1DFN)( dwt_t * _pdV );


		// == Members.
		/**
		 * This table contains the norms of the 5-3 wavelets for different bands.
		 */
		static const double						dwt_norms[4][10];

		/**
		 * This table contains the norms of the 9-7 wavelets for different bands.
		 */
		static const double						dwt_norms_real[4][10];

		/**
		 * Undocumented value from the OpenJpeg Library.
		 */
		static const float						m_fK;

		/**
		 * Undocumented value from the OpenJpeg Library.
		 */
		static const float						m_c13318;

		/**
		 * Undocumented value from the OpenJpeg Library.
		 */
		static const float						m_dwt_alpha;

		/**
		 * Undocumented value from the OpenJpeg Library.
		 */
		static const float						m_dwt_beta;

		/**
		 * Undocumented value from the OpenJpeg Library.
		 */
		static const float						m_dwt_gamma;

		/**
		 * Undocumented value from the OpenJpeg Library.
		 */
		static const float						m_dwt_delta;


		// == Functions.
		/**
		 * Forward lazy transform (horizontal).
		 *
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32B Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_deinterleave_h( int32_t * _pi32A, int32_t * _pi32B, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas );

		/**
		 * Forward lazy transform (vertical).
		 *
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32B Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32X Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_deinterleave_v( int32_t * _pi32A, int32_t * _pi32B, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32X, int32_t _i32Cas );

		/**
		 * Inverse lazy transform (horizontal).
		 *
		 * \param _pdH Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_interleave_h( dwt_t * _pdH, int32_t * _pi32A );

		/**
		 * Inverse lazy transform (vertical).
		 *
		 * \param _pdV Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 * \param _i32X Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_interleave_v( dwt_t * _pdV, int32_t * _pi32A, int32_t _i32X );

		/**
		 * Forward 5-3 wavelet transform in 1-D.
		 *
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_encode_1( int32_t * _pi32A, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas );

		/**
		 * Inverse 5-3 wavelet transform in 1-D.
		 *
		 * \param _pdV Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_decode_1( dwt_t * _pdV );

		/**
		 * Forward 9-7 wavelet transform in 1-D.
		 *
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_encode_1_real( int32_t * _pi32A, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas );

		/**
		 * Explicit calculation of the Quantization Stepsizes .
		 *
		 * \param _i32StepSize Undocumented parameter from the OpenJpeg Library.
		 * \param _i32NumBps Undocumented parameter from the OpenJpeg Library.
		 * \param _psBandNoStepSize Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_encode_stepsize( int32_t _i32StepSize, int32_t _i32NumBps, CJ2k::opj_stepsize_t * _psBandNoStepSize );

		/**
		 * Inverse wavelet transform in 2-D.
		 *
		 * \param _pttTileC Undocumented parameter from the OpenJpeg Library.
		 * \param _i32I Undocumented parameter from the OpenJpeg Library.
		 * \param _dFn Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_decode_tile( CTcd::opj_tcd_tilecomp_t * _pttTileC, int32_t _i32I, DWT1DFN _dFn );

		/**
		 * Determine maximum computed resolution level for inverse wavelet transform.
		 *
		 * \param _ptrR Undocumented parameter from the OpenJpeg Library.
		 * \param _i32I Undocumented parameter from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL						dwt_decode_max_resolution( CTcd::opj_tcd_resolution_t * LSE_RESTRICT _ptrR, int32_t _i32I );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pvW Undocumented parameter from the OpenJpeg Library.
		 * \param _pfA Undocumented parameter from the OpenJpeg Library.
		 * \param _i32X Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Size Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						v4dwt_interleave_h( v4dwt_t * LSE_RESTRICT _pvW, float * LSE_RESTRICT _pfA, int32_t _i32X, int32_t _i32Size );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pvV Undocumented parameter from the OpenJpeg Library.
		 * \param _pfA Undocumented parameter from the OpenJpeg Library.
		 * \param _i32X Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						v4dwt_interleave_v( v4dwt_t * LSE_RESTRICT _pvV, float * LSE_RESTRICT _pfA, int32_t _i32X );

		/**
		 * Inverse 9-7 wavelet transform in 1-D.
		 *
		 * \param _pvDwt Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						v4dwt_decode( v4dwt_t * LSE_RESTRICT _pvDwt );

		/**
		 * Inverse 5-3 wavelet transform in 1-D.
		 *
		 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						dwt_decode_1_( int32_t * _pi32A, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pvW Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Count Undocumented parameter from the OpenJpeg Library.
		 * \param _fC Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						v4dwt_decode_step1( v4 * _pvW, int32_t _i32Count, const float _fC );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pvL Undocumented parameter from the OpenJpeg Library.
		 * \param _pvW Undocumented parameter from the OpenJpeg Library.
		 * \param _i32K Undocumented parameter from the OpenJpeg Library.
		 * \param _i32M Undocumented parameter from the OpenJpeg Library.
		 * \param _fC Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						v4dwt_decode_step2( v4 * _pvL, v4 * _pvW, int32_t _i32K, int32_t _i32M, float _fC );
	};

}	// namespace lsi

#endif	// __LSI_DWT_H__
