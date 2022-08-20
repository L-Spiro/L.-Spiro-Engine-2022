/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS ( C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: toplevel libogg include
 last mod: $Id: ogg.h 17571 2010-10-27 13:28:20Z xiphmont $

 ********************************************************************/


#ifndef __LSS_OGG_H__
#define __LSS_OGG_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class COgg
	 * \brief Class wrapper for the Ogg base header.
	 *
	 * Description: Provides the base definitions for the Ogg library.
	 */
	class COgg {
	public :
		// == Types.
		/**
		 * Ogg I/O vector.
		 */
		typedef struct ogg_iovec_t {
			void *				iov_base;
			uintptr_t				iov_len;
		} * LPogg_iovec_t, * const LPCogg_iovec_t;

		/**
		 * Ogg pack buffer.
		 */
		typedef struct oggpack_buffer {
			int32_t					endbyte;
			int32_t					endbit;

			uint8_t *				buffer;
			uint8_t *				ptr;
			int32_t					storage;
		} * LPoggpack_buffer, * const LPCoggpack_buffer;

		/**
		 * Ogg page.  Used to encapsulate the data in one Ogg bitstream page.
		 */
		typedef struct ogg_page {
			uint8_t *				header;
			int32_t					header_len;
			uint8_t *				body;
			int32_t					body_len;
		} * LPogg_page, * const LPCogg_page;

		typedef struct ogg_stream_state {
			uint8_t *				body_data;				// Bytes from packet bodies.
			int32_t					body_storage;			// Storage elements allocated.
			int32_t					body_fill;				// Elements stored; fill mark.
			int32_t					body_returned;			// Elements of fill returned.


			int32_t *				lacing_vals;			// The values that will go to the segment table.
			int64_t *				granule_vals;			// Granulepos values for headers. Not compact
															//	this way, but it is simple coupled to the
															//	lacing FIFO.
			int32_t					lacing_storage;
			int32_t					lacing_fill;
			int32_t					lacing_packet;
			int32_t					lacing_returned;

			uint8_t					header[282];			// Working space for header encode.
			int32_t					header_fill;

			int32_t					e_o_s;					// Set when we have buffered the last packet in the
															//	logical bitstream.
			int32_t					b_o_s;					// Set after we've written the initial page
															//	of a logical bitstream.
			int32_t					serialno;
			int32_t					pageno;
			int64_t					packetno;				// Sequence number for decode; the framing
															//	knows where there's a hole in the data,
															//	but we need coupling so that the codec
															//	(which is in a separate abstraction
															//	layer) also knows about the gap.
			int64_t					granulepos;

		} * LPogg_stream_state, * const LPCogg_stream_state;

		/**
		 * Ogg packet.  Used to encapsulate the data and metadata belonging
		 *	to a single raw Ogg/Vorbis packet.
		 */
		typedef struct ogg_packet {
			uint8_t *				packet;
			int32_t					bytes;
			int32_t					b_o_s;
			int32_t					e_o_s;

			int64_t					granulepos;

			int64_t					packetno;				// Sequence number for decode; the framing
															//	knows where there's a hole in the data,
															//	but we need coupling so that the codec
															//	(which is in a separate abstraction
															//	layer) also knows about the gap.
		} * LPogg_packet, * const LPCogg_packet;

		/**
		 * Ogg sync state.
		 */
		typedef struct ogg_sync_state {
			uint8_t *				data;
			int32_t					storage;
			int32_t					fill;
			int32_t					returned;

			int32_t					unsynced;
			int32_t					headerbytes;
			int32_t					bodybytes;
		} * LPogg_sync_state, * const LPCogg_sync_state;


		// == Functions.
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// BIT STREAM
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_writeinit( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_writecheck( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_writetrunc( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_writealign( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_writecopy( oggpack_buffer * _ppbBuffer, void * _pvSource, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_reset( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_writeclear( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _pui8Buf Undocumented parameter from the Ogg library.
		 * \param _i32Bytes Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_readinit( oggpack_buffer * _ppbBuffer, uint8_t * _pui8Buf, int32_t _i32Bytes );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _ui32Value Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_write( oggpack_buffer * _ppbBuffer, uint32_t _ui32Value, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_look( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_look1( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_adv( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_adv1( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_read( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_read1( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_bytes( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpack_bits( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static uint8_t *			oggpack_get_buffer( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_writeinit( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_writecheck( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_writetrunc( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_writealign( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _pvSource Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_writecopy( oggpack_buffer * _ppbBuffer, void * _pvSource, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_reset( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_writeclear( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _pui8Buf Undocumented parameter from the Ogg library.
		 * \param _i32Bytes Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_readinit( oggpack_buffer * _ppbBuffer, uint8_t * _pui8Buf, int32_t _i32Bytes );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _ui32Value Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_write( oggpack_buffer * _ppbBuffer, uint32_t _ui32Value, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_look( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );
		
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_look1( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_adv( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpackB_adv1( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_read( oggpack_buffer * _ppbBuffer, int32_t _i32Bits );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_read1( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_bytes( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		oggpackB_bits( oggpack_buffer * _ppbBuffer );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static uint8_t * LSE_CALL	oggpackB_get_buffer( oggpack_buffer * _ppbBuffer );



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// ENCODING
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _possOs Undocumented parameter from the Ogg library.
		 * \param _popOp Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_packetin( ogg_stream_state * _possOs, ogg_packet * _popOp );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _possOs Undocumented parameter from the Ogg library.
		 * \param _poiIov Undocumented parameter from the Ogg library.
		 * \param _i32Count Undocumented parameter from the Ogg library.
		 * \param _i32Eos Undocumented parameter from the Ogg library.
		 * \param _i64GranulePos Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_iovecin( ogg_stream_state * _possOs, ogg_iovec_t * _poiIov, 
			int32_t _i32Count, int32_t _i32Eos, int64_t _i64GranulePos );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _possOs Undocumented parameter from the Ogg library.
		 * \param _popOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_pageout( ogg_stream_state * _possOs, ogg_page * _popOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _possOs Undocumented parameter from the Ogg library.
		 * \param _popOg Undocumented parameter from the Ogg library.
		 * \param _i32NFill Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_pageout_fill( ogg_stream_state * _possOs, ogg_page * _popOg, int32_t _i32NFill );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _possOs Undocumented parameter from the Ogg library.
		 * \param _popOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_flush( ogg_stream_state * _possOs, ogg_page * _popOg );


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// DECODING
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_init( ogg_sync_state * _pssOy );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_clear( ogg_sync_state * _pssOy );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_reset( ogg_sync_state * _pssOy );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_destroy( ogg_sync_state * _pssOy );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_check( ogg_sync_state * _pssOy );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \param _i32Size Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int8_t * LSE_CALL	ogg_sync_buffer( ogg_sync_state * _pssOy,  int32_t _i32Size );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \param _i32Bytes Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_wrote( ogg_sync_state * _pssOy,  int32_t _i32Bytes );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_pageseek( ogg_sync_state * _pssOy, ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOy Undocumented parameter from the Ogg library.
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_sync_pageout( ogg_sync_state * _pssOy,  ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_pagein( ogg_stream_state * _pssOs,  ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _ppOp Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_packetout( ogg_stream_state * _pssOs, ogg_packet * _ppOp );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _ppOp Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_packetpeek( ogg_stream_state * _pssOs, ogg_packet * _ppOp );



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// GENERAL
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _i32SerialNo Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_init( ogg_stream_state * _pssOs, int32_t _i32SerialNo );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_clear( ogg_stream_state * _pssOs );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_reset( ogg_stream_state * _pssOs );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _i32SerialNo Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_reset_serialno( ogg_stream_state * _pssOs, int32_t _i32SerialNo );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_destroy( ogg_stream_state * _pssOs );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_check( ogg_stream_state * _pssOs );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_eos( ogg_stream_state * _pssOs );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		ogg_page_checksum_set( ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_version( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_continued( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_bos( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_eos( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int64_t LSE_CALL		ogg_page_granulepos( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_serialno( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_pageno( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOg Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_page_packets( const ogg_page * _ppOg );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppOp Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static void LSE_CALL		ogg_packet_clear( ogg_packet * _ppOp );


	protected :
		// == Members.
		/**
		 * Undocumented data from the Ogg library.
		 */
		static uint32_t				m_ui32Mask[];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static uint32_t				m_ui32Mask8B[];

		/**
		 * Undocumented data from the Ogg library.
		 */
		static uint32_t				crc_lookup[256];

		// == Functions.
		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _ppbBuffer Undocumented parameter from the Ogg library.
		 * \param _pvSource Undocumented parameter from the Ogg library.
		 * \param _i32Bits Undocumented parameter from the Ogg library.
		 * \param _i32Msb Undocumented parameter from the Ogg library.
		 */
		static void LSE_CALL		oggpack_writecopy_helper( oggpack_buffer * _ppbBuffer,
			void * _pvSource,
			int32_t _i32Bits,
			void (LSE_CALL * PfFunc)( oggpack_buffer *,
				uint32_t,
				int32_t ),
			int32_t _i32Msb );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _i32Needed Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		_os_lacing_expand( ogg_stream_state * _pssOs, int32_t _i32Needed );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _i32Needed Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		_os_body_expand( ogg_stream_state * _pssOs, int32_t _i32Needed );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _ppOp Undocumented parameter from the Ogg library.
		 * \param _i32Adv Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		_packetout( ogg_stream_state * _pssOs, ogg_packet * _ppOp, int32_t _i32Adv );

		/**
		 * Undocumented function from the Ogg library.
		 *
		 * \param _pssOs Undocumented parameter from the Ogg library.
		 * \param _popOg Undocumented parameter from the Ogg library.
		 * \param _i32Force Undocumented parameter from the Ogg library.
		 * \param _i32NFill Undocumented parameter from the Ogg library.
		 * \return Undocumented return from the Ogg library.
		 */
		static int32_t LSE_CALL		ogg_stream_flush_i( ogg_stream_state * _pssOs, ogg_page * _popOg, int32_t _i32Force, int32_t _i32NFill );

	};

}	// namespace lss

#endif	// __LSS_OGG_H__

