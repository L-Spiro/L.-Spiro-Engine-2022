/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
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


#ifndef __LSI_J2K_H__
#define __LSI_J2K_H__

#include "../LSIImageLib.h"
#include "LSICio.h"
#include "LSIJpegCommonStructs.h"
#include "LSIJpegImage.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define J2K_CP_CSTY_PRT 0x01
#define J2K_CP_CSTY_SOP 0x02
#define J2K_CP_CSTY_EPH 0x04
#define J2K_CCP_CSTY_PRT 0x01
#define J2K_CCP_CBLKSTY_LAZY 0x01		/**< Selective arithmetic coding bypass. */
#define J2K_CCP_CBLKSTY_RESET 0x02		/**< Reset context probabilities on coding pass boundaries. */
#define J2K_CCP_CBLKSTY_TERMALL 0x04	/**< Termination on each coding pass. */
#define J2K_CCP_CBLKSTY_VSC 0x08		/**< Vertically stripe causal context. */
#define J2K_CCP_CBLKSTY_PTERM 0x10		/**< Predictable termination. */
#define J2K_CCP_CBLKSTY_SEGSYM 0x20		/**< Segmentation symbols are used. */
#define J2K_CCP_QNTSTY_NOQNT 0
#define J2K_CCP_QNTSTY_SIQNT 1
#define J2K_CCP_QNTSTY_SEQNT 2

#define J2K_MS_SOC 0xFF4F				/**< SOC marker value. */
#define J2K_MS_SOT 0xFF90				/**< SOT marker value. */
#define J2K_MS_SOD 0xFF93				/**< SOD marker value. */
#define J2K_MS_EOC 0xFFD9				/**< EOC marker value. */
#define J2K_MS_SIZ 0xFF51				/**< SIZ marker value. */
#define J2K_MS_COD 0xFF52				/**< COD marker value. */
#define J2K_MS_COC 0xFF53				/**< COC marker value. */
#define J2K_MS_RGN 0xFF5E				/**< RGN marker value. */
#define J2K_MS_QCD 0xFF5C				/**< QCD marker value. */
#define J2K_MS_QCC 0xFF5D				/**< QCC marker value. */
#define J2K_MS_POC 0xFF5F				/**< POC marker value. */
#define J2K_MS_TLM 0xFF55				/**< TLM marker value. */
#define J2K_MS_PLM 0xFF57				/**< PLM marker value. */
#define J2K_MS_PLT 0xFF58				/**< PLT marker value. */
#define J2K_MS_PPM 0xFF60				/**< PPM marker value. */
#define J2K_MS_PPT 0xFF61				/**< PPT marker value. */
#define J2K_MS_SOP 0xFF91				/**< SOP marker value. */
#define J2K_MS_EPH 0xFF92				/**< EPH marker value. */
#define J2K_MS_CRG 0xFF63				/**< CRG marker value. */
#define J2K_MS_COM 0xFF64				/**< COM marker value. */

#ifdef USE_JPWL
#define J2K_MS_EPC 0xFF68				/**< EPC marker value (Part 11: JPEG 2000 for Wireless). */
#define J2K_MS_EPB 0xFF66				/**< EPB marker value (Part 11: JPEG 2000 for Wireless). */ 
#define J2K_MS_ESD 0xFF67				/**< ESD marker value (Part 11: JPEG 2000 for Wireless). */ 
#define J2K_MS_RED 0xFF69				/**< RED marker value (Part 11: JPEG 2000 for Wireless). */
#endif	// #ifdef USE_JPWL
#ifdef USE_JPSEC
#define J2K_MS_SEC 0xFF65				/**< SEC marker value (Part 8: Secure JPEG 2000). */
#define J2K_MS_INSEC 0xFF94				/**< INSEC marker value (Part 8: Secure JPEG 2000). */
#endif	// #ifdef USE_JPSEC

namespace lsi {

	/**
	 * Class CJ2k
	 * \brief The JPEG-2000 Codestream Reader/Writer (J2K).
	 *
	 * Description: The JPEG-2000 Codestream Reader/Writer (J2K).
	 */
	class CJ2k {
	public :
		// == Enumerations.
		/**
		 * Values that specify the status of the decoding process when decoding the main header. 
		 *	These values may be combined with a | operator. 
		 */
		typedef enum J2K_STATUS {
			J2K_STATE_MHSOC				= 0x0001,	/**< A SOC marker is expected. */
			J2K_STATE_MHSIZ				= 0x0002,	/**< A SIZ marker is expected. */
			J2K_STATE_MH				= 0x0004,	/**< The decoding process is in the main header. */
			J2K_STATE_TPHSOT			= 0x0008,	/**< The decoding process is in a tile part header and expects a SOT marker. */
			J2K_STATE_TPH				= 0x0010,	/**< The decoding process is in a tile part header. */
			J2K_STATE_MT				= 0x0020,	/**< The EOC marker has just been read. */
			J2K_STATE_NEOC				= 0x0040,	/**< The decoding process must not expect a EOC marker because the codestream is truncated. */
			J2K_STATE_ERR				= 0x0080	/**< The decoding process has encountered an error. */
		} J2K_STATUS;

		/** 
		 * T2 encoding mode.
		 */
		typedef enum T2_MODE {
			THRESH_CALC					= 0,		/** Function called in Rate allocation process. */
			FINAL_PASS					= 1			/** Function called in Tier 2 process. */
		}J2K_T2_MODE;


		// == Types.
		/**
		 * Quantization stepsize
		 */
		typedef struct opj_stepsize {
			/** Exponent. */
			int32_t						expn;
			/** Mantissa. */
			int32_t						mant;
		} opj_stepsize_t;

		/**
		 * Tile-component coding parameters
		 */
		typedef struct opj_tccp {
			/** Coding style. */
			int32_t						csty;
			/** Number of resolutions. */
			int32_t						numresolutions;
			/** Code-blocks width. */
			int32_t						cblkw;
			/** Code-blocks height. */
			int32_t						cblkh;
			/** Code-block coding style. */
			int32_t						cblksty;
			/** Discrete wavelet transform identifier. */
			int32_t						qmfbid;
			/** Quantisation style. */
			int32_t						qntsty;
			/** Stepsizes used for quantization. */
			opj_stepsize_t				stepsizes[J2K_MAXBANDS];
			/** Number of guard bits. */
			int32_t						numgbits;
			/** Region Of Interest shift. */
			int32_t						roishift;
			/** Precinct width. */
			int32_t						prcw[J2K_MAXRLVLS];
			/** Precinct height. */
			int32_t						prch[J2K_MAXRLVLS];	
		} opj_tccp_t;

		/**
		 * Tile coding parameters:
		 *	this structure is used to store coding/decoding parameters common to all
		 *	tiles (information like COD, COC in main header).
		 */
		typedef struct opj_tcp {
			/** 1: first part-tile of a tile. */
			int32_t						first;
			/** Coding style. */
			int32_t						csty;
			/** Progression order. */
			OPJ_PROG_ORDER				prg;
			/** Number of layers. */
			int32_t						numlayers;
			/** Multi-component transform identifier. */
			int32_t						mct;
			/** Rates of layers. */
			float						rates[100];
			/** Number of progression order changes. */
			int32_t						numpocs;
			/** Indicates if a POC marker has been used O:NO, 1:YES. */
			int32_t						POC;
			/** Progression order changes. */
			CJpegCommonStructs::opj_poc_t
										pocs[32];
			/** Packet header store there for futur use in t2_decode_packet. */
			uint8_t *					ppt_data;
			/** Pointer remaining on the first byte of the first header if ppt is used. */
			uint8_t *					ppt_data_first;
			/** If ppt == 1 --> there was a PPT marker for the present tile. */
			int32_t						ppt;
			/** Used in case of multiple marker PPT ( number of info already stored ). */
			int32_t						ppt_store;
			/** ppmbug1. */
			int32_t						ppt_len;
			/** Adds fixed_quality. */
			float						distoratio[100];
			/** Tile-component coding parameters. */
			opj_tccp_t *				tccps;
		} opj_tcp_t;

		/**
		 * Coding parameters
		 */
		typedef struct opj_cp {
			/** Digital cinema profile. */
			OPJ_CINEMA_MODE				cinema;
			/** Maximum rate for each component. If == 0, component size limitation is not considered. */
			int32_t						max_comp_size;
			/** Size of the image in bits. */
			int32_t						img_size;
			/** Rsiz*/
			OPJ_RSIZ_CAPABILITIES		rsiz;
			/** Enabling Tile part generation. */
			char						tp_on;
			/** Flag determining tile part generation. */
			char						tp_flag;
			/** Position of tile part flag in progression order. */
			int32_t						tp_pos;
			/** Allocation by rate/distortion. */
			int32_t						disto_alloc;
			/** Allocation by fixed layer. */
			int32_t						fixed_alloc;
			/** Adds fixed_quality. */
			int32_t						fixed_quality;
			/** If != 0, then original dimension divided by 2^( reduce ); if == 0 or not used, image is decoded to the full resolution. */
			int32_t						reduce;
			/** If != 0, then only the first "layer" layers are decoded; if == 0 or not used, all the quality layers are decoded. */
			int32_t						layer;
			/** If == NO_LIMITATION, decode entire codestream; if == LIMIT_TO_MAIN_HEADER then only decode the main header. */
			OPJ_LIMIT_DECODING			limit_decoding;
			/** XTOsiz. */
			int32_t						tx0;
			/** YTOsiz. */
			int32_t						ty0;
			/** XTsiz. */
			int32_t						tdx;
			/** YTsiz. */
			int32_t						tdy;
			/** Comment for coding. */
			char *					comment;
			/** Number of tiles in width. */
			int32_t						tw;
			/** Number of tiles in heigth. */
			int32_t						th;
			/** ID number of the tiles present in the codestream. */
			int32_t *					tileno;
			/** Size of the vector tileno. */
			int32_t						tileno_size;
			/** Packet header store there for futur use in t2_decode_packet. */
			uint8_t *					ppm_data;
			/** Pointer remaining on the first byte of the first header if ppm is used. */
			uint8_t *					ppm_data_first;
			/** If ppm == 1 --> there was a PPM marker for the present tile. */
			int32_t						ppm;
			/** Use in case of multiple marker PPM (number of info already store). */
			int32_t						ppm_store;
			/** Use in case of multiple marker PPM (case on non-finished previous info). */
			int32_t						ppm_previous;
			/** ppmbug1. */
			int32_t						ppm_len;
			/** Tile coding parameters. */
			opj_tcp_t *					tcps;
			/** Fixed layer. */
			int32_t *					matrice;
		} opj_cp_t;

		/**
		 * JPEG-2000 codestream reader/writer.
		 */
		typedef struct opj_j2k {
			/** Codec context. */
			CJpegCommonStructs::opj_common_ptr 
										cinfo;

			/** Locate in which part of the codestream the decoder is (main header, tile header, end). */
			int32_t						state;
			/** Number of the tile curently concern by coding/decoding. */
			int32_t						curtileno;
			/** Tile part number. */
			int32_t						tp_num;
			/** Tilepart number currently coding. */
			int32_t						cur_tp_num;
			/** Total number of tileparts of the current tile. */
			int32_t *					cur_totnum_tp;
			/**
			 * Locate the start position of the TLM marker  
			 *	after encoding the tilepart, a jump (in j2k_write_sod) is done to the TLM marker to store the value of its length. 
			 */
			int32_t						tlm_start;
			/** Total num of tile parts in whole image += num tiles * num tileparts in each tile. */
			/** Used in TLMmarker. */
			int32_t						totnum_tp;
			/** 
			 * Locate the position of the end of the tile in the codestream, 
			 *	used to detect a truncated codestream (in j2k_read_sod).
			 */
			uint8_t *					eot;
			/**
			 * Locate the start position of the SOT marker of the current coded tile:  
			 *	after encoding the tile, a jump ( in j2k_write_sod ) is done to the SOT marker to store the value of its length. 
			 */
			int32_t						sot_start;
			int32_t						sod_start;
			/**
			 * As the J2K-file is written in several parts during encoding, 
			 *	it enables to make the right correction in position return by cio_tell.
			 */
			int32_t						pos_correction;
			/** Array used to store the data of each tile. */
			uint8_t **					tile_data;
			/** Array used to store the length of each tile. */
			int32_t *					tile_len;
			/** 
			 * Decompression only:
			 *	store decoding parameters common to all tiles (information such as COD, COC in main header).
			 */
			opj_tcp_t *					default_tcp;
			/** Pointer to the encoded/decoded image. */
			CJpegImage::opj_image_t *	image;
			/** Pointer to the coding parameters. */
			opj_cp_t *					cp;
			/** Helper used to write the index file. */
			CJpegCommonStructs::opj_codestream_info_t *
										cstr_info;
			/** Pointer to the byte i/o stream. */
			CCio::opj_cio_t *			cio;
		} opj_j2k_t;


		// == Functions.
		/**
		 * Creates a J2K decompression structure.
		 *
		 * \param _cpInfoC Codec context info.
		 * \return Returns a handle to a J2K decompressor if successful, returns NULL otherwise.
		 */
		static opj_j2k_t * LSE_CALL		j2k_create_decompress( CJpegCommonStructs::opj_common_ptr _cpInfoC );
		
		/**
		 * Destroy a J2K decompressor handle.
		 *
		 * \param _pjJ2k J2K decompressor handle to destroy.
		 */
		static void LSE_CALL			j2k_destroy_decompress( opj_j2k_t * _pjJ2k );
		
		/**
		 * Setup the decoder decoding parameters using user parameters.
		 * Decoding parameters are returned in _pjJ2k->cp.
		 *
		 * \param _pjJ2k J2K decompressor handle.
		 * \param _pdParameters decompression _pdParameters.
		 */
		static void LSE_CALL			j2k_setup_decoder( opj_j2k_t * _pjJ2k, CJpegCommonStructs::opj_dparameters_t * _pdParameters );

		/**
		 * Decode an image from a JPEG-2000 codestream.
		 *
		 * \param _pjJ2k J2K decompressor handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pciCStrInfo Codestream information structure if required, NULL otherwise.
		 * \return Returns a decoded image if successful, returns NULL otherwise.
		 */
		static CJpegImage::opj_image_t * LSE_CALL
										j2k_decode( opj_j2k_t * _pjJ2k, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciCStrInfo );

		/**
		 * Decode an image form a JPT-stream (JPEG 2000, JPIP).
		 *
		 * \param _pjJ2k J2K decompressor handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pciCStrInfo Codestream information structure if required, NULL otherwise.
		 * \return Returns a decoded image if successful, returns NULL otherwise.
		 */
		static CJpegImage::opj_image_t * LSE_CALL
										j2k_decode_jpt_stream( opj_j2k_t * _pjJ2k, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciCStrInfo );

		/**
		 * Creates a J2K compression structure.
		 *
		 * \param _cpInfo Codec context info.
		 * \return Returns a handle to a J2K compressor if successful, returns NULL otherwise.
		 */
		static opj_j2k_t * LSE_CALL		j2k_create_compress( CJpegCommonStructs::opj_common_ptr _cpInfo );

		/**
		 * Destroy a J2K compressor handle.
		 *
		 * \param _pjJ2k J2K compressor handle to destroy.
		 */
		static void LSE_CALL			j2k_destroy_compress( opj_j2k_t * _pjJ2k );

		/**
		 * Setup the encoder parameters using the current image and using user parameters.
		 * Coding parameters are returned in _pjJ2k->cp.
		 *
		 * \param _pjJ2k J2K compressor handle.
		 * \param _pcpParameters compression parameters.
		 * \param _piImage input filled image.
		 */
		static void LSE_CALL			j2k_setup_encoder( opj_j2k_t * _pjJ2k, CJpegCommonStructs::opj_cparameters_t * _pcpParameters, CJpegImage::opj_image_t * _piImage );

		/**
		 * Converts an enum type progression order to string type.
		 *
		 * \param _poOrder Undocumented parameter.
		 * \return Undocumented return.
		 */
		static const char * LSE_CALL	j2k_convert_progression_order( OPJ_PROG_ORDER _poOrder );

		/**
		 * Encode an image into a JPEG-2000 codestream.
		 *
		 * \param _pjJ2k J2K compressor handle.
		 * \param _pcCio Output buffer stream.
		 * \param _piImage Image to encode.
		 * \param _pciCStrInfo Codestream information structure if required, NULL otherwise.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			j2k_encode( opj_j2k_t * _pjJ2k, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciCStrInfo );


	protected :
		// == Types.
		typedef struct opj_dec_mstabent_t {
			/** Marker value. */
			int32_t						id;
			/** Value of the state when the marker can appear. */
			int32_t						states;
			/** Action linked to the marker. */
			void (LSE_CALL *			handler)( opj_j2k_t * j2k );
		} * LPopj_dec_mstabent_t, * const LPCopj_dec_mstabent_t;


		// == Members.
		static opj_dec_mstabent_t		j2k_dec_mstab[];


		// == Functions.
		/**
		 * Read the lookup table containing all the marker, status and action.
		 *
		 * \param _i32Id Marker value.
		 * \return Undocumented return.
		 */
		static opj_dec_mstabent_t * LSE_CALL
										j2k_dec_mstab_lookup( int32_t _i32Id );

		/**
		 * Write the SOC marker (Start Of Codestream).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_soc( opj_j2k_t * _pjJ2k );

		/**
		 * Read the SOC marker (Start of Codestream).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_soc( opj_j2k_t * _pjJ2k );

		/**
		 * Write the SIZ marker (image and tile size).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_siz( opj_j2k_t * _pjJ2k );

		/**
		 * Read the SIZ marker (image and tile size).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_siz( opj_j2k_t * _pjJ2k );

		/**
		 * Write the COM marker (comment).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_com( opj_j2k_t * _pjJ2k );

		/**
		 * Read the COM marker (comment).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_com( opj_j2k_t * _pjJ2k );

		/**
		 * Write the value concerning the specified component in the marker COD and COC.
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concerned by the information written.
		 */
		static void LSE_CALL			j2k_write_cox( opj_j2k_t * _pjJ2k, int32_t _i32CompNo );

		/**
		 * Read the value concerning the specified component in the marker COD and COC.
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concerned by the information read.
		 */
		static void LSE_CALL			j2k_read_cox( opj_j2k_t * _pjJ2k, int32_t _i32CompNo );

		/**
		 * Write the COD marker (coding style default).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_cod( opj_j2k_t * _pjJ2k );

		/**
		 * Read the COD marker (coding style default).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_cod( opj_j2k_t * _pjJ2k );

		/**
		 * Write the COC marker (coding style component).
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concerned by the information written.
		 */
		static void LSE_CALL			j2k_write_coc( opj_j2k_t * _pjJ2k, int32_t _i32CompNo );

		/**
		 * Read the COC marker (coding style component).
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_coc( opj_j2k_t * _pjJ2k );

		/**
		 * Write the value concerning the specified component in the marker QCD and QCC.
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concerned by the information written.
		 */
		static void LSE_CALL			j2k_write_qcx( opj_j2k_t * _pjJ2k, int32_t _i32CompNo );

		/**
		 * Read the value concerning the specified component in the marker QCD and QCC.
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concern by the information read.
		 * \param _i32Len Length of the information in the QCX part of the marker QCD/QCC.
		 */
		static void LSE_CALL			j2k_read_qcx( opj_j2k_t * _pjJ2k, int32_t _i32CompNo, int32_t _i32Len );

		/**
		 * Write the QCD marker (quantization default).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_qcd( opj_j2k_t * _pjJ2k );

		/**
		 * Read the QCD marker (quantization default).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_qcd( opj_j2k_t * _pjJ2k );

		/**
		 * Write the QCC marker (quantization component).
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concerned by the information written.
		 */
		static void LSE_CALL			j2k_write_qcc( opj_j2k_t * _pjJ2k, int32_t _i32CompNo );

		/**
		 * Read the QCC marker (quantization component).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_qcc( opj_j2k_t * _pjJ2k );

		/**
		 * Write the POC marker (progression order change).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_poc( opj_j2k_t * _pjJ2k );

		/**
		 * Read the POC marker (progression order change).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_poc( opj_j2k_t * _pjJ2k );

		/**
		 * Read the CRG marker (component registration).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_crg( opj_j2k_t * _pjJ2k );

		/**
		 * Read the TLM marker (tile-part lengths).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_tlm( opj_j2k_t * _pjJ2k );

		/**
		 * Read the PLM marker (packet length, main header).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_plm( opj_j2k_t * _pjJ2k );

		/**
		 * Read the PLT marker (packet length, tile-part header).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_plt( opj_j2k_t * _pjJ2k );

		/**
		 * Read the PPM marker (packet packet headers, main header).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_ppm( opj_j2k_t * _pjJ2k );

		/**
		 * Read the PPT marker (packet packet headers, tile-part header).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_ppt( opj_j2k_t * _pjJ2k );

		/**
		 * Write the TLM marker (Mainheader).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_tlm( opj_j2k_t * _pjJ2k );

		/**
		 * Write the SOT marker (start of tile-part).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_sot( opj_j2k_t * _pjJ2k );

		/**
		 * Read the SOT marker (start of tile-part).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_sot( opj_j2k_t * _pjJ2k );

		/**
		 * Write the SOD marker (start of data).
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32TileCoder Pointer to a TCD handle.
		 */
		static void LSE_CALL			j2k_write_sod( opj_j2k_t * _pjJ2k, void * _i32TileCoder );

		/**
		 * Read the SOD marker (start of data).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_sod( opj_j2k_t * _pjJ2k );

		/**
		 * Write the RGN marker (region-of-interest).
		 *
		 * \param _pjJ2k J2K handle.
		 * \param _i32CompNo Number of the component concerned by the information written.
		 * \param _i32TileNo Number of the tile concerned by the information written.
		 */
		static void LSE_CALL			j2k_write_rgn( opj_j2k_t * _pjJ2k, int32_t _i32CompNo, int32_t _i32TileNo );

		/**
		 * Read the RGN marker (region-of-interest).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_rgn( opj_j2k_t * _pjJ2k );

		/**
		 * Write the EOC marker (end of codestream).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_write_eoc( opj_j2k_t * _pjJ2k );

		/**
		 * Read the EOC marker (end of codestream).
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_eoc( opj_j2k_t * _pjJ2k );

		/**
		 * Read an unknown marker.
		 *
		 * \param _pjJ2k J2K handle.
		 */
		static void LSE_CALL			j2k_read_unk( opj_j2k_t * _pjJ2k );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pcCp Undocumented function from the OpenJpeg Library.
		 * \param _i32PiNo Undocumented function from the OpenJpeg Library.
		 * \param _i32TileNo Undocumented function from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL			j2k_get_num_tp( opj_cp_t * _pcCp, int32_t _i32PiNo, int32_t _i32TileNo );

		/**
		 * Memory allocation for TLM marker.
		 *
		 * \param _pcCp Undocumented function from the OpenJpeg Library.
		 * \param _i32ImageNumComp Undocumented function from the OpenJpeg Library.
		 * \param _piImage Undocumented function from the OpenJpeg Library.
		 * \param _pjJ2k Undocumented function from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL			j2k_calculate_tp( opj_cp_t * _pcCp, CJpegImage::opj_image_t * _piImage, opj_j2k_t * _pjJ2k );

	};

}	// namespace lsi

#endif	// __LSI_J2K_H__
