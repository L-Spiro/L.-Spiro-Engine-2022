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


#ifndef __LSI_T1_H__
#define __LSI_T1_H__

#include "../LSIImageLib.h"
#include "LSIJpegCommonStructs.h"
#include "LSIMqc.h"
#include "LSIRaw.h"
#include "LSITcd.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define T1_NMSEDEC_BITS 7

#define T1_SIG_NE								0x0001		/**< Context orientation: North-East direction. */
#define T1_SIG_SE								0x0002		/**< Context orientation: South-East direction. */
#define T1_SIG_SW								0x0004		/**< Context orientation: South-West direction. */
#define T1_SIG_NW								0x0008		/**< Context orientation: North-West direction. */
#define T1_SIG_N								0x0010		/**< Context orientation: North direction. */
#define T1_SIG_E								0x0020		/**< Context orientation: East direction. */
#define T1_SIG_S								0x0040		/**< Context orientation: South direction. */
#define T1_SIG_W								0x0080		/**< Context orientation: West direction. */
#define T1_SIG_OTH								(T1_SIG_N | T1_SIG_NE | T1_SIG_E | T1_SIG_SE | T1_SIG_S | T1_SIG_SW | T1_SIG_W | T1_SIG_NW)
#define T1_SIG_PRIM								(T1_SIG_N | T1_SIG_E | T1_SIG_S | T1_SIG_W)

#define T1_SGN_N								0x0100
#define T1_SGN_E								0x0200
#define T1_SGN_S								0x0400
#define T1_SGN_W								0x0800
#define T1_SGN									(T1_SGN_N | T1_SGN_E | T1_SGN_S | T1_SGN_W)

#define T1_SIG									0x1000
#define T1_REFINE								0x2000
#define T1_VISIT								0x4000

#define T1_NUMCTXS_ZC							9
#define T1_NUMCTXS_SC							5
#define T1_NUMCTXS_MAG							3
#define T1_NUMCTXS_AGG							1
#define T1_NUMCTXS_UNI							1

#define T1_CTXNO_ZC								0
#define T1_CTXNO_SC								(T1_CTXNO_ZC + T1_NUMCTXS_ZC)
#define T1_CTXNO_MAG							(T1_CTXNO_SC + T1_NUMCTXS_SC)
#define T1_CTXNO_AGG							(T1_CTXNO_MAG + T1_NUMCTXS_MAG)
#define T1_CTXNO_UNI							(T1_CTXNO_AGG + T1_NUMCTXS_AGG)
#define T1_NUMCTXS								(T1_CTXNO_UNI + T1_NUMCTXS_UNI)

#define T1_NMSEDEC_FRACBITS						(T1_NMSEDEC_BITS - 1)

#define T1_TYPE_MQ								0			/**< Normal coding using entropy coder. */
#define T1_TYPE_RAW								1			/**< No encoding the information is store under raw format in codestream (mode switch RAW). */
#define MACRO_t1_flags( X, Y )					_ptT1->flags[((X)*(_ptT1->flags_stride))+(Y)]

namespace lsi {

	/**
	 * Class CT1
	 * \brief Implementation of the tier-1 coding (coding of code-block coefficients) (T1).
	 *
	 * Description: Implementation of the tier-1 coding (coding of code-block coefficients) (T1).
	 */
	class CT1 {
	public :
		// == Types.
		typedef int16_t							flag_t;

		/**
		 * Tier-1 coding (coding of code-block coefficients).
		 */
		typedef struct opj_t1 {
			/** Codec context. */
			CJpegCommonStructs::opj_common_ptr	cinfo;

			/** MQC component. */
			CMqc::opj_mqc_t *					mqc;
			/** RAW component. */
			CRaw::opj_raw_t *					raw;

			int32_t *							data;
			flag_t *							flags;
			int32_t								w;
			int32_t								h;
			int32_t								datasize;
			int32_t								flagssize;
			int32_t								flags_stride;
		} opj_t1_t;

		
		// == Functions.
		/**
		 * Creates a new T1 handle and initialize the look-up tables of the Tier-1 coder/decoder.
		 *
		 * \param _cpInfo Undocumented parameter from the OpenJpeg Library.
		 * \return Returns a new T1 handle if successful, returns NULL otherwise.
		 * \see t1_init_luts
		 */
		static opj_t1_t * LSE_CALL				t1_create( CJpegCommonStructs::opj_common_ptr _cpInfo );
		
		/**
		 * Destroy a previously created T1 handle.
		 *
		 * \param _ptT1 T1 handle to destroy.
		 */
		static void LSE_CALL					t1_destroy( opj_t1_t * _ptT1 );
		
		/**
		 * Encode the code-blocks of a tile.
		 *
		 * \param _ptT1 T1 handle.
		 * \param _pttTile The tile to encode.
		 * \param _ptTcp Tile coding parameters.
		 */
		static void LSE_CALL					t1_encode_cblks( opj_t1_t * _ptT1, CTcd::opj_tcd_tile_t * _pttTile, CJ2k::opj_tcp_t * _ptTcp );
		
		/**
		 * Decode the code-blocks of a tile.
		 *
		 * \param _ptT1 T1 handle.
		 * \param _pttTileC The tile to decode.
		 * \param _ptTccp Tile coding parameters.
		 */
		static void LSE_CALL					t1_decode_cblks( opj_t1_t * _ptT1, CTcd::opj_tcd_tilecomp_t * _pttTileC, CJ2k::opj_tccp_t * _ptTccp );

	protected :
		// == Members.
		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static char							lut_ctxno_zc[1024];

		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static char							lut_ctxno_sc[256];

		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static char							lut_spb[256];

		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static int16_t							lut_nmsedec_sig[1<<T1_NMSEDEC_BITS];

		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static int16_t							lut_nmsedec_sig0[1<<T1_NMSEDEC_BITS];

		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static int16_t							lut_nmsedec_ref[1<<T1_NMSEDEC_BITS];

		/**
		 * Undocumented OpenJpeg Library data.
		 */
		static int16_t							lut_nmsedec_ref0[1<<T1_NMSEDEC_BITS];

		// == Functions.
		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _i32F Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static char LSE_CALL					t1_getctxno_zc( int32_t _i32F, int32_t _i32Orient );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _i32F Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static char LSE_CALL					t1_getctxno_sc( int32_t _i32F );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _i32F Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL					t1_getctxno_mag( int32_t _i32F );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _i32F Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static char LSE_CALL					t1_getspb( int32_t _i32F );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _i32X Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BitPos Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int16_t LSE_CALL					t1_getnmsedec_sig( int32_t _i32X, int32_t _i32BitPos );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _i32X Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BitPos Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int16_t LSE_CALL					t1_getnmsedec_ref( int32_t _i32X, int32_t _i32BitPos );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _i32S Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Stride Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_updateflags( flag_t * _pfFlags, int32_t _i32S, int32_t _i32Stride );
		
		/**
		 * Encode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32One Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Type Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_enc_sigpass_step(
			opj_t1_t * _ptT1,
			flag_t * _pfFlags,
			int32_t * _pi32Data,
			int32_t _i32Orient,
			int32_t _i32BpNo,
			int32_t _i32One,
			int32_t * _pi32NmseDec,
			char _i32Type,
			int32_t _i32Vsc );
		
		/**
		 * Decode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_sigpass_step_raw( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32OnePlusHalf, 
			int32_t _i32Vsc );

		/**
		 * Decode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_sigpass_step_mqc( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32OnePlusHalf );

		/**
		 * Decode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_sigpass_step_mqc_vsc( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32OnePlusHalf, 
			int32_t _i32Vsc );
		
		/**
		 * Encode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _cType Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_enc_sigpass( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32Orient, 
			int32_t * _pi32NmseDec, 
			char _cType, 
			int32_t _i32CodeBlockStY );

		/**
		 * Decode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_sigpass_raw( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32Orient, 
			int32_t _i32CodeBlockStY );

		/**
		 * Decode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_sigpass_mqc( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32Orient );

		/**
		 * Decode significant pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_sigpass_mqc_vsc( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32Orient );

		/**
		 * Encode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32One Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _cType Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_enc_refpass_step( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32BpNo, 
			int32_t _i32One, 
			int32_t * _pi32NmseDec, 
			char _cType, 
			int32_t _i32Vsc );
		
		/**
		 * Decode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32PosHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32NegHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_refpass_step_raw( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32PosHalf, 
			int32_t _i32NegHalf, 
			int32_t _i32Vsc );

		/**
		 * Decode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32PosHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32NegHalf Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_refpass_step_mqc( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32PosHalf, 
			int32_t _i32NegHalf );

		/**
		 * Decode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32PosHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32NegHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_refpass_step_mqc_vsc( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32PosHalf, 
			int32_t _i32NegHalf, 
			int32_t _i32Vsc );

		/**
		 * Encode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _cType Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_enc_refpass( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t * _pi32NmseDec, 
			char _cType, 
			int32_t _i32CodeBlockStY );

		/**
		 * Decode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_refpass_raw( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32CodeBlockStY );

		/**
		 * Decode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_refpass_mqc( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo );

		/**
		 * Decode refinement pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_refpass_mqc_vsc( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo );

		/**
		 * Encode clean-up pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32One Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Partial Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_enc_clnpass_step( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32BpNo, 
			int32_t _i32One, 
			int32_t * _pi32NmseDec, 
			int32_t _i32Partial, 
			int32_t _i32Vsc );

		/**
		 * Decode clean-up pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_clnpass_step_partial( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32OnePlusHalf );

		/**
		 * Decode clean-up pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_clnpass_step( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t * _pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32OnePlusHalf );

		/**
		 * Decode clean-up pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _pfFlags Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32Data Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32OnePlusHalf Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Partial Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Vsc Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_clnpass_step_vsc( 
			opj_t1_t * _ptT1, 
			flag_t * _pfFlags, 
			int32_t *_pi32Data, 
			int32_t _i32Orient, 
			int32_t _i32OnePlusHalf, 
			int32_t _i32Partial, 
			int32_t _i32Vsc );

		/**
		 * Encode clean-up pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_enc_clnpass( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32Orient, 
			int32_t * _pi32NmseDec, 
			int32_t _i32CodeBlockStY );

		/**
		 * Decode clean-up pass.
		 *
		 * \param _ptT1 Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_dec_clnpass( 
			opj_t1_t * _ptT1, 
			int32_t _i32BpNo, 
			int32_t _i32Orient, 
			int32_t _i32CodeBlockStY );

		/**
		 * Decode clean-up pass.
		 *
		 * \param _pi32NmseDec Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CompNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Level Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32BpNo Undocumented parameter from the OpenJpeg Library.
		 * \param _i32QmfbId Undocumented parameter from the OpenJpeg Library.
		 * \param _dStepSize Undocumented parameter from the OpenJpeg Library.
		 * \param _i32NumComps Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Mct Undocumented parameter from the OpenJpeg Library.
		 */
		static double LSE_CALL				t1_getwmsedec( 
			int32_t _pi32NmseDec, 
			int32_t _i32CompNo, 
			int32_t _i32Level, 
			int32_t _i32Orient, 
			int32_t _i32BpNo, 
			int32_t _i32QmfbId, 
			double _dStepSize, 
			int32_t _i32NumComps, 
			int32_t _i32Mct );
		
		/**
		 * Encode 1 code-block.
		 *
		 * \param _ptT1 T1 handle.
		 * \param _ptceCodeBlock Code-block coding parameters.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CompNo Component number.
		 * \param _i32Level Undocumented parameter from the OpenJpeg Library.
		 * \param _i32QmfbId Undocumented parameter from the OpenJpeg Library.
		 * \param _dStepSize Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Code-block style.
		 * \param _i32NumComps Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Mct Undocumented parameter from the OpenJpeg Library.
		 * \param _pttTile Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					t1_encode_cblk( 
			opj_t1_t * _ptT1, 
			CTcd::opj_tcd_cblk_enc_t * _ptceCodeBlock, 
			int32_t _i32Orient, 
			int32_t _i32CompNo, 
			int32_t _i32Level, 
			int32_t _i32QmfbId, 
			double _dStepSize, 
			int32_t _i32CodeBlockStY, 
			int32_t _i32NumComps, 
			int32_t _i32Mct, 
			CTcd::opj_tcd_tile_t * _pttTile );

		/**
		 * Decode 1 code-block.
		 *
		 * \param _ptT1 T1 handle.
		 * \param _ptceCodeBlock Code-block coding parameters.
		 * \param _i32Orient Undocumented parameter from the OpenJpeg Library.
		 * \param _i32RoiShift Region of interest shifting value.
		 * \param _i32CodeBlockStY Code-block style.
		 */
		static void LSE_CALL					t1_decode_cblk( 
			opj_t1_t * _ptT1,
			CTcd::opj_tcd_cblk_dec_t * _ptceCodeBlock, 
			int32_t _i32Orient, 
			int32_t _i32RoiShift, 
			int32_t _i32CodeBlockStY );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _ptT1 T1 handle.
		 * \param _i32W Undocumented parameter from the OpenJpeg Library.
		 * \param _i32H Undocumented parameter from the OpenJpeg Library.
		 */
		static LSBOOL LSE_CALL					allocate_buffers( 
			opj_t1_t * _ptT1, 
			int32_t _i32W, 
			int32_t _i32H );
	};

}	// namespace lsi

#endif	// __LSI_T1_H__
