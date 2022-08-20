/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2001             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *

 ********************************************************************

 function: libvorbis codec headers
 last mod: $Id: codec.h 17021 2010-03-24 09:29:41Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISCODEC_H__
#define __LSS_VORBISCODEC_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"

namespace lss {

	/**
	 * Vorbis error codes.
	 */
	enum LSS_VORBIS_ERRORS {
		OV_FALSE									= -1,
		OV_EOF										= -2,
		OV_HOLE										= -3,

		OV_EREAD									= -128,
		OV_EFAULT									= -129,
		OV_EIMPL									= -130,
		OV_EINVAL									= -131,
		OV_ENOTVORBIS								= -132,
		OV_EBADHEADER								= -133,
		OV_EVERSION									= -134,
		OV_ENOTAUDIO								= -135,
		OV_EBADPACKET								= -136,
		OV_EBADLINK									= -137,
		OV_ENOSEEK									= -138,

		LSS_VE_FORCEDWORD							= 0x7FFFFFFF
	};

	/**
	 * Class CVorbisCodec
	 * \brief The vorbis codec.
	 *
	 * Description: The vorbis codec.
	 */
	class CVorbisCodec {
	public :
		// == Types.
		/**
		 * Contains all the setup information specific to the
		 * specific compression/decompression mode in progress (eg.,
		 * psychoacoustic settings, channel setup, options, codebook
		 * etc).
		 */
		typedef struct vorbis_info {
			int32_t									version;
			int32_t									channels;
			int32_t									rate;

			// The below bitrate declarations are hints.
			// Combinations of the three values carry the following implications:
			// All three set to the same value:
			//	Implies a fixed rate bitstream
			// Only nominal set:
			//	Implies a VBR stream that averages the nominal bitrate.	No hard
			//	upper/lower limit
			// Upper and or lower set:
			//	Implies a VBR bitstream that obeys the bitrate limits. Nominal
			//	may also be set to give a nominal rate.
			// None set:
			//	The coder does not care to speculate.

			int32_t									bitrate_upper;
			int32_t									bitrate_nominal;
			int32_t									bitrate_lower;
			int32_t									bitrate_window;

			void *								codec_setup;
		} * LPvorbis_info, * const LPCvorbis_info;

		/**
		 * Buffers the current vorbis audio analysis/synthesis state.  The DSP state belongs to a specific
		 * logical bitstream.
		 */
		typedef struct vorbis_dsp_state {
			int32_t									analysisp;
			vorbis_info *							vi;

			float **								pcm;
			float **								pcmret;
			int32_t									pcm_storage;
			int32_t									pcm_current;
			int32_t									pcm_returned;

			int32_t									preextrapolate;
			int32_t									eofflag;

			int32_t									lW;
			int32_t									W;
			int32_t									nW;
			int32_t									centerW;

			int64_t									granulepos;
			int64_t									sequence;

			int64_t									glue_bits;
			int64_t									time_bits;
			int64_t									floor_bits;
			int64_t									res_bits;

			void *								backend_state;
		} * LPvorbis_dsp_state, * const LPCvorbis_dsp_state;

		/**
		 * An allocation chain.
		 */
		typedef struct alloc_chain {
			void *								ptr;
			alloc_chain *							next;
		} * LPalloc_chain, * const LPCalloc_chain;

		/**
		 * A single block of data to be processed as part of the analysis/synthesis stream;
		 *	it belongs to a specific logical bitstream, but is independent from other vorbis_blocks
		 *	belonging to that logical bitstream.
		 */
		typedef struct vorbis_block {
			// Necessary stream state for linking to the framing abstraction.
			float **								pcm;       // This is a pointer into local storage.
			COgg::oggpack_buffer					opb;

			int32_t									lW;
			int32_t									W;
			int32_t									nW;
			int32_t									pcmend;
			int32_t									mode;

			int32_t									eofflag;
			int64_t									granulepos;
			int64_t									sequence;
			vorbis_dsp_state *						vd;			// For read-only access of configuration.

			// Local storage to avoid remallocing; it's up to the mapping to
			//	structure it.
			void *								localstore;
			int32_t									localtop;
			int32_t									localalloc;
			int32_t									totaluse;
			alloc_chain *							reap;

			// Bitmetrics for the frame.
			int32_t									glue_bits;
			int32_t									time_bits;
			int32_t									floor_bits;
			int32_t									res_bits;

			void *								internal;

		} * LPvorbis_block, * const LPCvorbis_block;

		/**
		 * The comments are not part of vorbis_info so that vorbis_info can be static storage.
		 */
		typedef struct vorbis_comment {
			// Unlimited user _pcComment fields.  libvorbis writes 'libvorbis'
			//	to whatever vendor is set in encode.
			char **								user_comments;
			int32_t *								comment_lengths;
			int32_t									comments;
			char *								vendor;

		} * LPvorbis_comment, * const LPCvorbis_comment;


		// == Functions.
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// GENERAL
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_info_init( vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_info_clear( vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _i32Zo Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_info_blocksize( vorbis_info * _pviVi, int32_t _i32Zo );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_comment_init( vorbis_comment * _pvcVc );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _pcComment Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_comment_add( vorbis_comment * _pvcVc, const char * _pcComment );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _pcTag Undocumented parameter from the Vorbis library.
		 * \param _pcContents Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_comment_add_tag( vorbis_comment * _pvcVc, 
			const char * _pcTag, const char * _pcContents );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _pcTag Undocumented parameter from the Vorbis library.
		 * \param _i32Count Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static char * 							vorbis_comment_query( vorbis_comment * _pvcVc, const char * _pcTag, int32_t _i32Count );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _pcTag Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_comment_query_count( vorbis_comment * _pvcVc, const char * _pcTag );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_comment_clear( vorbis_comment * _pvcVc );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_block_init( vorbis_dsp_state * _pvdsV, vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_block_clear( vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 */
		static void								vorbis_dsp_clear( vorbis_dsp_state * _pvdsV );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _i64GranulePos Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static double								vorbis_granule_time( vorbis_dsp_state * _pvdsV, 
			int64_t _i64GranulePos );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \return Undocumented return from the Vorbis library.
		 */
		static const char * 						vorbis_version_string();



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// DSP LAYER
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_analysis_init( vorbis_dsp_state * _pvdsV, vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_commentheader_out( vorbis_comment * _pvcVc, COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \param _popComm Undocumented parameter from the Vorbis library.
		 * \param _popCode Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_analysis_headerout( vorbis_dsp_state * _pvdsV, 
			vorbis_comment * _pvcVc, 
			COgg::ogg_packet * _popOp, 
			COgg::ogg_packet * _popComm, 
			COgg::ogg_packet * _popCode );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _i32Vals Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float ** 							vorbis_analysis_buffer( vorbis_dsp_state * _pvdsV, int32_t _i32Vals );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _i32Vals Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_analysis_wrote( vorbis_dsp_state * _pvdsV, int32_t _i32Vals );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_analysis_blockout( vorbis_dsp_state * _pvdsV, vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_analysis( vorbis_block * _pvbVb, COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_bitrate_addblock( vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsVd Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_bitrate_flushpacket( vorbis_dsp_state * _pvdsVd, 
			COgg::ogg_packet * _popOp );



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// SYNTHESIS LAYER
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_idheader( COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_headerin( vorbis_info * _pviVi, vorbis_comment * _pvcVc, 
			COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_init( vorbis_dsp_state * _pvdsV, vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_restart( vorbis_dsp_state * _pvdsV );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis( vorbis_block * _pvbVb, COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_trackonly( vorbis_block * _pvbVb, COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_blockin( vorbis_dsp_state * _pvdsV, vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pppfPcm Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_pcmout( vorbis_dsp_state * _pvdsV, float *** _pppfPcm );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pppfPcm Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_lapout( vorbis_dsp_state * _pvdsV, float *** _pppfPcm );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _i32Samples Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_read( vorbis_dsp_state * _pvdsV, int32_t _i32Samples );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_packet_blocksize( vorbis_info * _pviVi, COgg::ogg_packet * _popOp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _i32Flag Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_halfrate( vorbis_info * _pvdsV, int32_t _i32Flag );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_synthesis_halfrate_p( vorbis_info * _pvdsV );



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// MISCELLANEOUS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _i32Window Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float *							vorbis_window( vorbis_dsp_state *_pvdsV, int32_t _i32Window );


	protected :
		// == Members.


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcS1 Undocumented parameter from the Vorbis library.
		 * \param _pcS2 Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								tagcompare( const char * _pcS1, const char * _pcS2, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _i32Encp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vds_shared_init( CVorbisCodec::vorbis_dsp_state * _pvdsV, CVorbisCodec::vorbis_info * _pviVi, int32_t _i32Encp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								ilog2( uint32_t _ui32V );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vorbis_pack_comment( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_comment * _pvcVc );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOut Undocumented parameter from the Vorbis library.
		 * \param _pcString Undocumented parameter from the Vorbis library.
		 * \param _i32Length Undocumented parameter from the Vorbis library.
		 */
		static void								_v_writestring( COgg::oggpack_buffer * _pobOut, const char * _pcString, int32_t _i32Length );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vorbis_pack_info( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vorbis_pack_books( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 */
		static void								_preextrapolate_helper( CVorbisCodec::vorbis_dsp_state * _pvdsV );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _popbBuffer Undocumented parameter from the Vorbis library.
		 * \param _pcBuf Undocumented parameter from the Vorbis library.
		 * \param _i32Bytes Undocumented parameter from the Vorbis library.
		 */
		static void								_v_readstring( COgg::oggpack_buffer * _popbBuffer, char * _pcBuf, int32_t _i32Bytes );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vorbis_unpack_info( vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvcVc Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vorbis_unpack_comment( vorbis_comment * _pvcVc, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								_vorbis_unpack_books( vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb );
	};

}	// namespace lss

#endif	// __LSS_VORBISCODEC_H__
