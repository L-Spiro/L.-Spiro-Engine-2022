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

#include "LSIJp2.h"
#include "LSIEvent.h"
#include "LSIJ2k.h"


namespace lsi {

	// == Functions.
	/**
	 * Write the JP2 Header box (used in MJ2).
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Output buffer stream.
	 */
	void LSE_CALL CJp2::jp2_write_jp2h( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_JP2H), 4 );		// JP2H.

		jp2_write_ihdr( _pjJp2, _pcCio );

		if ( _pjJp2->bpc == 255 ) {
			jp2_write_bpcc( _pjJp2, _pcCio );
		}
		jp2_write_colr( _pjJp2, _pcCio );

		jbBox.length = CCio::cio_tell( _pcCio ) - jbBox.init_pos;
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(jbBox.length), 4 );
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );
	}

	/**
	 * Read the JP2 Header box (used in MJ2).
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pjcColor Undocumented paramater from the OpenJpeg Library.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_jp2h( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, opj_jp2_color_t * _pjcColor ) {
		opj_jp2_box_t box;
		uint32_t jp2h_end;

		CJpegCommonStructs::opj_common_ptr cinfo = _pjJp2->cinfo;

		jp2_read_boxhdr( cinfo, _pcCio, &box );
		do {
			if ( JP2_JP2H != box.type ) {
				if ( box.type == JP2_JP2C ) {
					CEvent::opj_event_msg( cinfo, EVT_ERROR, "Expected JP2H marker.\r\n" );
					return false;
				}
				CCio::cio_skip( _pcCio, box.length - 8 );

				if ( _pcCio->bp >= _pcCio->end ) { return false; }

				jp2_read_boxhdr( cinfo, _pcCio, &box );
			}
		} while ( JP2_JP2H != box.type );

		if ( !jp2_read_ihdr( _pjJp2, _pcCio ) ) { return false; }
		jp2h_end = static_cast<uint32_t>(box.init_pos + box.length);

		if ( _pjJp2->bpc == 255 ) {
			if ( !jp2_read_bpcc( _pjJp2, _pcCio ) ) { return false; }
		}
		jp2_read_boxhdr( cinfo, _pcCio, &box );

		while ( static_cast<uint32_t>(CCio::cio_tell( _pcCio )) < jp2h_end ) {
			if ( box.type == JP2_COLR ) {
				if ( !jp2_read_colr( _pjJp2, _pcCio, &box, _pjcColor ) ) {
					CCio::cio_seek( _pcCio, box.init_pos + 8 );
					CCio::cio_skip( _pcCio, box.length - 8 );
				}
				jp2_read_boxhdr( cinfo, _pcCio, &box );
				continue;
			}
			if ( box.type == JP2_CDEF ) {
				if ( !jp2_read_cdef( _pjJp2, _pcCio, &box, _pjcColor ) ) {
					CCio::cio_seek( _pcCio, box.init_pos + 8 );
					CCio::cio_skip( _pcCio, box.length - 8 );
				}
				jp2_read_boxhdr( cinfo, _pcCio, &box );
				continue;
			}
			if ( box.type == JP2_PCLR ) {
				if ( !jp2_read_pclr( _pjJp2, _pcCio, &box, _pjcColor ) ) {
					CCio::cio_seek( _pcCio, box.init_pos + 8 );
					CCio::cio_skip( _pcCio, box.length - 8 );
				}
				jp2_read_boxhdr( cinfo, _pcCio, &box );
				continue;
			}
			if ( box.type == JP2_CMAP ) {
				if ( !jp2_read_cmap( _pjJp2, _pcCio, &box, _pjcColor ) ) {
					CCio::cio_seek( _pcCio, box.init_pos + 8 );
					CCio::cio_skip( _pcCio, box.length - 8 );
				}
				jp2_read_boxhdr( cinfo, _pcCio, &box );
				continue;
			}
			CCio::cio_seek( _pcCio, box.init_pos + 8 );
			CCio::cio_skip( _pcCio, box.length - 8 );
			jp2_read_boxhdr( cinfo, _pcCio, &box );

		}	// while ( CCio::cio_tell( _pcCio ) < jp2h_end ) {

		CCio::cio_seek( _pcCio, static_cast<int32_t>(jp2h_end) );

		// Part 1, I.5.3.3: "must contain at least one".
		return (_pjcColor->jp2_has_colr == 1);
	}

	/**
	 * Creates a JP2 decompression structure.
	 *
	 * \param _cpInfo Codec context info.
	 * \return Returns a handle to a JP2 decompressor if successful, returns NULL otherwise.
	 */
	CJp2::opj_jp2_t * LSE_CALL CJp2::jp2_create_decompress( CJpegCommonStructs::opj_common_ptr _cpInfo ) {
		opj_jp2_t * pjJp2 = static_cast<opj_jp2_t *>(CMemLib::CMAlloc( sizeof( opj_jp2_t ) ));
		if ( pjJp2 ) {
			pjJp2->cinfo = _cpInfo;
			// Create the J2K codec.
			pjJp2->j2k = CJ2k::j2k_create_decompress( _cpInfo );
			if ( pjJp2->j2k == NULL ) {
				jp2_destroy_decompress( pjJp2 );
				return NULL;
			}
		}
		return pjJp2;
	}

	/**
	 * Destroy a JP2 decompressor handle.
	 *
	 * \param _pjJp2 JP2 decompressor handle to destroy.
	 */
	void LSE_CALL CJp2::jp2_destroy_decompress( opj_jp2_t * _pjJp2 ) {
		if ( _pjJp2 ) {
			// Destroy the J2K codec.
			CJ2k::j2k_destroy_decompress( _pjJp2->j2k );

			if ( _pjJp2->comps ) {
				CMemLib::Free( _pjJp2->comps );
			}
			if ( _pjJp2->cl ) {
				CMemLib::Free( _pjJp2->cl );
			}
			CMemLib::Free( _pjJp2 );
		}
	}

	/**
	 * Setup the decoder decoding parameters using user parameters.
	 * Decoding parameters are returned in _pjJp2->j2k->cp.
	 *
	 * \param _pjJp2 JP2 decompressor handle.
	 * \param _pdParameters decompression _pdParameters.
	 */
	void LSE_CALL CJp2::jp2_setup_decoder( opj_jp2_t * _pjJp2, CJpegCommonStructs::opj_dparameters_t * _pdParameters ) {
		// Set up the J2K codec.
		CJ2k::j2k_setup_decoder( _pjJp2->j2k, _pdParameters );
		// Further JP2 initializations go here.
	}

	/**
	 * Decode an image from a JPEG-2000 file stream.
	 *
	 * \param _pjJp2 JP2 decompressor handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pciInfo Codestream information structure if required, NULL otherwise.
	 * \return Returns a decoded image if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * LSE_CALL CJp2::jp2_decode( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciInfo ) {
		CJpegCommonStructs::opj_common_ptr cinfo;
		CJpegImage::opj_image_t *image = NULL;
		opj_jp2_color_t color;

		if ( !_pjJp2 || !_pcCio ) { return NULL; }
		CStd::MemSet( &color, 0, sizeof( opj_jp2_color_t ) );
		cinfo = _pjJp2->cinfo;

		// JP2 decoding.
		if ( !jp2_read_struct( _pjJp2, _pcCio, &color ) ) {
			free_color_data( &color );
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Failed to decode _pjJp2 structure.\r\n" );
			return NULL;
		}

		// J2K decoding.
		image = CJ2k::j2k_decode( _pjJp2->j2k, _pcCio, _pciInfo );

		if ( !image ) {
			free_color_data( &color );
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Failed to decode J2K image.\r\n" );
			return NULL;
		}

		// Set Image Color Space.
		if ( _pjJp2->enumcs == 16 ) {
			image->color_space = CLRSPC_SRGB;
		}
		else if ( _pjJp2->enumcs == 17 ) {
			image->color_space = CLRSPC_GRAY;
		}
		else if ( _pjJp2->enumcs == 18 ) {
			image->color_space = CLRSPC_SYCC;
		}
		else {
			image->color_space = CLRSPC_UNKNOWN;
		}

		if ( color.jp2_cdef ) {
			jp2_apply_cdef( image, &color );
		}
		if ( color.jp2_pclr ) {
			// Part 1, I.5.3.4: Either both or none : */
			if ( !color.jp2_pclr->cmap ) {
				jp2_free_pclr( &color );
			}
			else {
				jp2_apply_pclr( &color, image );
			}
		}
		if ( color.icc_profile_buf ) {
			image->icc_profile_buf = color.icc_profile_buf;
			color.icc_profile_buf = NULL;
			image->icc_profile_len = color.icc_profile_len;
		}
		return image;
	}

	/**
	 * Creates a JP2 compression structure.
	 *
	 * \param _cpInfo Codec context info.
	 * \return Returns a handle to a JP2 compressor if successful, returns NULL otherwise.
	 */
	CJp2::opj_jp2_t * LSE_CALL CJp2::jp2_create_compress( CJpegCommonStructs::opj_common_ptr _cpInfo ) {
		opj_jp2_t * pjJp2 = static_cast<opj_jp2_t *>(CMemLib::MAlloc( sizeof( opj_jp2_t ) ));
		if ( pjJp2 ) {
			pjJp2->cinfo = _cpInfo;
			// Create the J2K codec.
			pjJp2->j2k = CJ2k::j2k_create_compress( _cpInfo );
			if ( pjJp2->j2k == NULL ) {
				jp2_destroy_compress( pjJp2 );
				return NULL;
			}
		}
		return pjJp2;
	}

	/**
	 * Destroy a JP2 compressor handle.
	 *
	 * \param _pjJp2 JP2 compressor handle to destroy.
	 */
	void LSE_CALL CJp2::jp2_destroy_compress( opj_jp2_t * _pjJp2 ) {
		if ( _pjJp2 ) {
			// Destroy the J2K codec.
			CJ2k::j2k_destroy_compress( _pjJp2->j2k );

			if ( _pjJp2->comps ) {
				CMemLib::Free( _pjJp2->comps );
			}
			if ( _pjJp2->cl ) {
				CMemLib::Free( _pjJp2->cl );
			}
			CMemLib::Free( _pjJp2 );
		}
	}

	/**
	 * Setup the encoder _pdParameters using the current image and using user _pdParameters.
	 * Coding _pdParameters are returned in _pjJp2->j2k->cp.
	 *
	 * \param _pjJp2 JP2 compressor handle.
	 * \param _pdParameters compression parameters.
	 * \param _piImage input filled image.
	 */
	void LSE_CALL CJp2::jp2_setup_encoder( opj_jp2_t * _pjJp2, CJpegCommonStructs::opj_cparameters_t * _pdParameters, CJpegImage::opj_image_t * _piImage ) {
		int32_t I;
		int32_t i32Depth0, i32Sign;

		if ( !_pjJp2 || !_pdParameters || !_piImage ) { return; }

		// Setup the J2K codec.

		// Check if number of components respects standard.
		if ( _piImage->numcomps < 1 || _piImage->numcomps > 16384 ) {
			CEvent::opj_event_msg( _pjJp2->cinfo, EVT_ERROR, "Invalid number of components specified while setting up JP2 encoder.\r\n" );
			return;
		}

		CJ2k::j2k_setup_encoder( _pjJp2->j2k, _pdParameters, _piImage );

		// Setup the JP2 codec.
		
		// Profile box.

		_pjJp2->brand = JP2_JP2;	// BR.
		_pjJp2->minversion = 0;		// MinV.
		_pjJp2->numcl = 1;
		_pjJp2->cl = static_cast<uint32_t *>(CMemLib::MAlloc( _pjJp2->numcl * sizeof( uint32_t ) ));
		_pjJp2->cl[0] = JP2_JP2;	// CL0: JP2.

		// Image Header box.

		_pjJp2->numcomps = static_cast<uint32_t>(_piImage->numcomps);			// NC.
		_pjJp2->comps = static_cast<opj_jp2_comps_t *>(CMemLib::MAlloc( _pjJp2->numcomps * sizeof( opj_jp2_comps_t ) ));
		_pjJp2->h = static_cast<uint32_t>(_piImage->y1 - _piImage->y0);		// HEIGHT.
		_pjJp2->w = static_cast<uint32_t>(_piImage->x1 - _piImage->x0);		// WIDTH.
		// BPC.
		i32Depth0 = _piImage->comps[0].prec - 1;
		i32Sign = _piImage->comps[0].sgnd;
		_pjJp2->bpc = static_cast<uint32_t>(i32Depth0 + (i32Sign << 7));
		for ( I = 1; I < _piImage->numcomps; I++ ) {
			int32_t i32Depth = _piImage->comps[I].prec - 1;
			i32Sign = _piImage->comps[I].sgnd;
			if ( i32Depth0 != i32Depth ) {
				_pjJp2->bpc = 255;
			}
		}
		_pjJp2->C = 7;			// C : Always 7.
		_pjJp2->UnkC = 0;		// UnkC, colorspace specified in color box.
		_pjJp2->IPR = 0;		// IPR, no intellectual property.
		
		// BitsPerComponent box.

		for ( I = 0; I < _piImage->numcomps; I++ ) {
			_pjJp2->comps[I].bpcc = _piImage->comps[I].prec - 1 + (_piImage->comps[I].sgnd << 7);
		}

		// Color specification box.
		if ( (_piImage->numcomps == 1 || _piImage->numcomps == 3) && (_pjJp2->bpc != 255) ) {
			_pjJp2->meth = 1;			// METH: Enumerated colorspace.
		}
		else {
			_pjJp2->meth = 2;			// METH: Restricted ICC profile.
		}
		if ( _pjJp2->meth == 1 ) {
			if ( _piImage->color_space == 1 ) {
				_pjJp2->enumcs = 16;	// sRGB as defined by IEC 61966EE.
			}
			else if ( _piImage->color_space == 2 ) {
				_pjJp2->enumcs = 17;	// Greyscale.
			}
			else if ( _piImage->color_space == 3 ) {
				_pjJp2->enumcs = 18;	// YUV.
			}
		}
		else {
			_pjJp2->enumcs = 0;			// PROFILE.
		}
		_pjJp2->precedence = 0;			// PRECEDENCE.
		_pjJp2->approx = 0;				// APPROX.
	}

	/**
	 * Encode an image into a JPEG-2000 file stream.
	 *
	 * \param _pjJp2 JP2 compressor handle.
	 * \param _pcCio Output buffer stream.
	 * \param _piImage Image to encode.
	 * \param _pciInfo Codestream information structure if required, NULL otherwise.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_encode( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciInfo ) {
		// JP2 encoding.

		// JPEG 2000 Signature box.
		jp2_write_jp( _pcCio );
		// File Type box.
		jp2_write_ftyp( _pjJp2, _pcCio );
		// JP2 Header box.
		jp2_write_jp2h( _pjJp2, _pcCio );

		// J2K encoding.
		if ( !jp2_write_jp2c( _pjJp2, _pcCio, _piImage, _pciInfo ) ) {
			CEvent::opj_event_msg( _pjJp2->cinfo, EVT_ERROR, "Failed to encode image.\r\n" );
			return false;
		}

		return true;
	}

	/**
	 * Read box headers.
	 *
	 * \param _cpInfo Codec context info.
	 * \param _pcCio Input stream.
	 * \param box.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_boxhdr( CJpegCommonStructs::opj_common_ptr _cpInfo, CCio::opj_cio_t * _pcCio, opj_jp2_box_t * _pjbBox ) {
		_pjbBox->init_pos = CCio::cio_tell( _pcCio );
		_pjbBox->length = static_cast<int32_t>(CCio::cio_read( _pcCio, 4 ));
		_pjbBox->type = static_cast<int32_t>(CCio::cio_read( _pcCio, 4 ));
		if ( _pjbBox->length == 1 ) {
			if ( CCio::cio_read( _pcCio, 4 ) != 0 ) {
				CEvent::opj_event_msg( _cpInfo, EVT_ERROR, "Cannot handle _pjbBox sizes higher than 2^32\n" );
				return false;
			}
			_pjbBox->length = static_cast<int32_t>(CCio::cio_read( _pcCio, 4 ));
			if ( _pjbBox->length == 0 ) {
				_pjbBox->length = CCio::cio_numbytesleft( _pcCio ) + 12;
			}
		}
		else if ( _pjbBox->length == 0 ) {
			_pjbBox->length = CCio::cio_numbytesleft( _pcCio ) + 8;
		}
		
		return true;
	}

	/**
	 * Read the IHDR box - Image Header box.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_ihdr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		CJpegCommonStructs::opj_common_ptr cpInfo = _pjJp2->cinfo;

		jp2_read_boxhdr( cpInfo, _pcCio, &jbBox );
		if ( JP2_IHDR != jbBox.type ) {
			CEvent::opj_event_msg( cpInfo, EVT_ERROR, "Expected IHDR marker.\r\n" );
			return false;
		}

		_pjJp2->h = CCio::cio_read( _pcCio, 4 );			// HEIGHT.
		_pjJp2->w = CCio::cio_read( _pcCio, 4 );			// WIDTH.
		_pjJp2->numcomps = CCio::cio_read( _pcCio, 2 );		// NC.
		_pjJp2->comps = static_cast<opj_jp2_comps_t *>(CMemLib::MAlloc( _pjJp2->numcomps * sizeof( opj_jp2_comps_t ) ));

		_pjJp2->bpc = CCio::cio_read( _pcCio, 1 );			// BPC.

		_pjJp2->C = CCio::cio_read( _pcCio, 1 );			// C.
		_pjJp2->UnkC = CCio::cio_read( _pcCio, 1 );			// UnkC.
		_pjJp2->IPR = CCio::cio_read( _pcCio, 1 );			// IPR.

		if ( CCio::cio_tell( _pcCio ) - jbBox.init_pos != jbBox.length ) {
			CEvent::opj_event_msg( cpInfo, EVT_ERROR, "Error with IHDR box.\r\n" );
			return false;
		}

		return true;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Output buffer stream.
	 */
	void LSE_CALL CJp2::jp2_write_ihdr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_IHDR), 4 );				// IHDR.

		CCio::cio_write( _pcCio, _pjJp2->h, 4 );			// HEIGHT.
		CCio::cio_write( _pcCio, _pjJp2->w, 4 );			// WIDTH.
		CCio::cio_write( _pcCio, _pjJp2->numcomps, 2 );		// NC.

		CCio::cio_write( _pcCio, _pjJp2->bpc, 1 );			// BPC.

		CCio::cio_write( _pcCio, _pjJp2->C, 1 );			// C : Always 7.
		CCio::cio_write( _pcCio, _pjJp2->UnkC, 1 );			// UnkC, colorspace unknown.
		CCio::cio_write( _pcCio, _pjJp2->IPR, 1 );			// IPR, no intellectual property.

		jbBox.length = CCio::cio_tell( _pcCio ) - jbBox.init_pos;
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, jbBox.length, 4 );			// L.
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Output buffer stream.
	 */
	void LSE_CALL CJp2::jp2_write_bpcc( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_BPCC), 4 );			// BPCC.

		for ( uint32_t I = 0; I < _pjJp2->numcomps; I++ ) {
			CCio::cio_write( _pcCio, _pjJp2->comps[I].bpcc, 1 );
		}

		jbBox.length = CCio::cio_tell( _pcCio ) - jbBox.init_pos;
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, jbBox.length, 4 );		// L.
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param Input Output buffer stream.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_bpcc( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		CJpegCommonStructs::opj_common_ptr cpInfo = _pjJp2->cinfo;

		jp2_read_boxhdr( cpInfo, _pcCio, &jbBox );
		if ( JP2_BPCC != jbBox.type ) {
			CEvent::opj_event_msg( cpInfo, EVT_ERROR, "Expected BPCC marker.\r\n" );
			return false;
		}

		for (  uint32_t I = 0; I < _pjJp2->numcomps; I++ ) {
			_pjJp2->comps[I].bpcc = static_cast<int32_t>(CCio::cio_read( _pcCio, 1 ));
		}

		if ( CCio::cio_tell( _pcCio ) - jbBox.init_pos != jbBox.length ) {
			CEvent::opj_event_msg( cpInfo, EVT_ERROR, "Error with BPCC box.\r\n" );
			return false;
		}

		return true;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Output buffer stream.
	 */
	void LSE_CALL CJp2::jp2_write_colr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_COLR), 4 );				// COLR.

		CCio::cio_write( _pcCio, _pjJp2->meth, 1 );			// METH.
		CCio::cio_write( _pcCio, _pjJp2->precedence, 1 );	// PRECEDENCE.
		CCio::cio_write( _pcCio, _pjJp2->approx, 1 );		// APPROX.

		if ( _pjJp2->meth == 1 ) {
			CCio::cio_write( _pcCio, _pjJp2->enumcs, 4 );	// EnumCS.
		}
		else {
			CCio::cio_write( _pcCio, static_cast<uint32_t>(0), 1 );				// PROFILE.
		}

		jbBox.length = CCio::cio_tell( _pcCio ) - jbBox.init_pos;
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, jbBox.length, 4 );			// L.
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );
	}

	/**
	 * Write the FTYP box - File type box.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Output buffer stream.
	 */
	void LSE_CALL CJp2::jp2_write_ftyp( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		uint32_t I;
		opj_jp2_box_t jbBox;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_FTYP), 4 );			// FTYP.

		CCio::cio_write( _pcCio, _pjJp2->brand, 4 );		// BR.
		CCio::cio_write( _pcCio, _pjJp2->minversion, 4 );	// MinV.

		for ( I = 0; I < _pjJp2->numcl; I++ ) {
			CCio::cio_write( _pcCio, _pjJp2->cl[I], 4 );	// CL.
		}

		jbBox.length = CCio::cio_tell( _pcCio ) - jbBox.init_pos;
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, jbBox.length, 4 );		// L.
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );
	}

	/**
	 * Read the FTYP box - File type box.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_ftyp( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		uint32_t I;
		opj_jp2_box_t jbBox;

		CJpegCommonStructs::opj_common_ptr cinfo = _pjJp2->cinfo;

		jp2_read_boxhdr( cinfo, _pcCio, &jbBox );

		if ( JP2_FTYP != jbBox.type ) {
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Expected FTYP marker.\r\n" );
			return false;
		}

		_pjJp2->brand = CCio::cio_read( _pcCio, 4 );		// BR.
		_pjJp2->minversion = CCio::cio_read( _pcCio, 4 );	// MinV.
		_pjJp2->numcl = (static_cast<uint32_t>(jbBox.length) - 16) / 4;
		_pjJp2->cl = static_cast<uint32_t *>(CMemLib::MAlloc( _pjJp2->numcl * sizeof( uint32_t ) ));

		for ( I = 0; I < _pjJp2->numcl; I++ ) {
			_pjJp2->cl[I] = CCio::cio_read( _pcCio, 4 );	// CLi.
		}

		if ( CCio::cio_tell( _pcCio ) - jbBox.init_pos != jbBox.length ) {
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Error with FTYP box.\r\n" );
			return false;
		}

		return true;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 Undocumented parameter from the OpenJpeg Library.
	 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
	 * \param _piImage Undocumented parameter from the OpenJpeg Library.
	 * \param _pciInfo Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CJp2::jp2_write_jp2c( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciInfo ) {
		uint32_t j2k_codestream_offset, j2k_codestream_length;
		opj_jp2_box_t jbBox;

		CJ2k::opj_j2k_t * pjJ2k = _pjJp2->j2k;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_JP2C), 4 );		// JP2C.

		// J2K encoding.
		j2k_codestream_offset = static_cast<uint32_t>(CCio::cio_tell( _pcCio ));
		if ( !CJ2k::j2k_encode( pjJ2k, _pcCio, _piImage, _pciInfo ) ) {
			CEvent::opj_event_msg( pjJ2k->cinfo, EVT_ERROR, "Failed to encode image.\r\n" );
			return 0;
		}
		j2k_codestream_length = static_cast<uint32_t>(CCio::cio_tell( _pcCio )) - j2k_codestream_offset;

		_pjJp2->j2k_codestream_offset = j2k_codestream_offset;
		_pjJp2->j2k_codestream_length = j2k_codestream_length;

		jbBox.length = static_cast<int32_t>(8 + _pjJp2->j2k_codestream_length);
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, jbBox.length, 4 );	// L.
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );

		return jbBox.length;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 Undocumented parameter from the OpenJpeg Library.
	 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
	 * \param _pui32CodeStreamLen Undocumented parameter from the OpenJpeg Library.
	 * \param _pui32CodeStreamOffset Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_jp2c( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, uint32_t * _pui32CodeStreamLen, uint32_t * _pui32CodeStreamOffset ) {
		opj_jp2_box_t jbBox;

		CJpegCommonStructs::opj_common_ptr cinfo = _pjJp2->cinfo;

		jp2_read_boxhdr( cinfo, _pcCio, &jbBox );
		do {
			if ( JP2_JP2C != jbBox.type ) {
				CCio::cio_skip( _pcCio, jbBox.length - 8 );
				jp2_read_boxhdr( cinfo, _pcCio, &jbBox );
			}
		} while ( JP2_JP2C != jbBox.type );

		(*_pui32CodeStreamOffset) = static_cast<uint32_t>(CCio::cio_tell( _pcCio ));
		(*_pui32CodeStreamLen) = static_cast<uint32_t>(jbBox.length) - 8;

		return true;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CJp2::jp2_write_jp( CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		jbBox.init_pos = CCio::cio_tell( _pcCio );
		CCio::cio_skip( _pcCio, 4 );
		CCio::cio_write( _pcCio, static_cast<uint32_t>(JP2_JP), 4 );		// JP2 signature.
		CCio::cio_write( _pcCio, static_cast<uint32_t>(0x0D0A870AUL), 4 );

		jbBox.length = CCio::cio_tell( _pcCio ) - jbBox.init_pos;
		CCio::cio_seek( _pcCio, jbBox.init_pos );
		CCio::cio_write( _pcCio, jbBox.length, 4 );	// L.
		CCio::cio_seek( _pcCio, jbBox.init_pos + jbBox.length );
	}

	/**
	 * Read the JP box - JPEG 2000 signature.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_jp( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio ) {
		opj_jp2_box_t jbBox;

		CJpegCommonStructs::opj_common_ptr cinfo = _pjJp2->cinfo;

		jp2_read_boxhdr( cinfo, _pcCio, &jbBox );
		if ( JP2_JP != jbBox.type ) {
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Expected JP marker.\r\n" );
			return false;
		}
		if ( 0x0D0A870A != CCio::cio_read( _pcCio, 4 ) ) {
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Error with JP marker.\r\n" );
			return false;
		}
		if ( CCio::cio_tell( _pcCio ) - jbBox.init_pos != jbBox.length ) {
			CEvent::opj_event_msg( cinfo, EVT_ERROR, "Error with JP box size.\r\n" );
			return false;
		}

		return true;
	}

	/**
	 * Decode the structure of a JP2 file.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pjcColor Collector for profile, cdef and pclr data.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_struct( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
		opj_jp2_color_t * _pjcColor ) {
		if ( !jp2_read_jp( _pjJp2, _pcCio ) ) { return false; }
		if ( !jp2_read_ftyp( _pjJp2, _pcCio ) ) { return false; }
		if ( !jp2_read_jp2h( _pjJp2, _pcCio, _pjcColor ) ) { return false; }
		if ( !jp2_read_jp2c( _pjJp2, _pcCio, &_pjJp2->j2k_codestream_length, &_pjJp2->j2k_codestream_offset ) ) { return false; }
		
		return true;
	}

	/**
	 * Apply collected palette data.
	 *
	 * \param _pjcColor Collector for profile, cdef and pclr data.
	 * \param _piImage Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CJp2::jp2_apply_pclr( opj_jp2_color_t * _pjcColor, CJpegImage::opj_image_t * _piImage ) {
		CJpegImage::opj_image_comp_t * piOldComps, * piNewComps;
		uint8_t * pui8ChannelSize, * pui8ChannelSign;
		uint32_t * pui32Entries;
		opj_jp2_cmap_comp_t * pjccMap;
		int32_t * pi32Src, * pi32Dst;
		uint32_t J, ui32Max;
		uint16_t I, ui16NrChannels, ui16Cmp, ui16PcoL;
		int32_t i32K, i32TopK;

		pui8ChannelSize = _pjcColor->jp2_pclr->channel_size;
		pui8ChannelSign = _pjcColor->jp2_pclr->channel_sign;
		pui32Entries = _pjcColor->jp2_pclr->entries;
		pjccMap = _pjcColor->jp2_pclr->cmap;
		ui16NrChannels = _pjcColor->jp2_pclr->nr_channels;

		piOldComps = _piImage->comps;
		piNewComps = static_cast<CJpegImage::opj_image_comp_t *>(CMemLib::MAlloc( ui16NrChannels * sizeof( CJpegImage::opj_image_comp_t ) ));

		for ( I = 0; I < ui16NrChannels; ++I ) {
			ui16PcoL = pjccMap[I].pcol; ui16Cmp = pjccMap[I].cmp;

			piNewComps[ui16PcoL] = piOldComps[ui16Cmp];

			if ( pjccMap[I].mtyp == 0 ) {
				piOldComps[ui16Cmp].data = NULL;
				continue;
			}
			// Palette mapping.
			piNewComps[ui16PcoL].data = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(piOldComps[ui16Cmp].w * piOldComps[ui16Cmp].h) * sizeof( int32_t ) ));
			piNewComps[ui16PcoL].prec = pui8ChannelSize[I];
			piNewComps[ui16PcoL].sgnd = pui8ChannelSign[I];
		}
		i32TopK = _pjcColor->jp2_pclr->nr_entries - 1;

		for ( I = 0; I < ui16NrChannels; ++I ) {
			// Direct use.
			if ( pjccMap[I].mtyp == 0 ) { continue; }

			// Palette mapping.
			ui16Cmp = pjccMap[I].cmp; ui16PcoL = pjccMap[I].pcol;
			pi32Src = piOldComps[ui16Cmp].data; 
			pi32Dst = piNewComps[ui16PcoL].data;
			ui32Max = static_cast<uint32_t>(piNewComps[ui16PcoL].w * piNewComps[ui16PcoL].h);

			for ( J = 0; J < ui32Max; ++J ) {
				// The index.
				if ( (i32K = pi32Src[J]) < 0 ) {
					i32K = 0;
				}
				else if ( i32K > i32TopK ) {
					i32K = i32TopK;
				}
				// The color.
				pi32Dst[J] = static_cast<int32_t>(pui32Entries[i32K*ui16NrChannels+ui16PcoL]);
			}
		}
		ui32Max = static_cast<uint32_t>(_piImage->numcomps);
		for ( I = 0; I < ui32Max; ++I ) {
			if ( piOldComps[I].data )  { CMemLib::Free( piOldComps[I].data ); }
		}
		CMemLib::Free( piOldComps );
		_piImage->comps = piNewComps;
		_piImage->numcomps = ui16NrChannels;

		jp2_free_pclr( _pjcColor );
	}

	/**
	 * Collect palette data.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
	 * \param _pjcColor Collector for profile, cdef and pclr data.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_pclr( opj_jp2_t * /*_pjJp2*/, CCio::opj_cio_t * _pcCio, 
		opj_jp2_box_t * /*_pjbBox*/, opj_jp2_color_t * _pjcColor ) {
		opj_jp2_pclr_t * jp2_pclr;
		uint8_t * pui8ChannelSize, * pui8ChannelSign;
		uint32_t * pui32Entries;
		uint16_t ui16NrEntries, ui16NrChannels;
		uint8_t ui8Uc;

		// Part 1, I.5.3.4: "There shall be at most one Palette box inside a JP2 Header box".
		if ( _pjcColor->jp2_pclr ) { return false; }

		ui16NrEntries = static_cast<uint16_t>(CCio::cio_read( _pcCio, 2 ));		// NE.
		ui16NrChannels = static_cast<uint16_t>(CCio::cio_read( _pcCio, 1 ));	// NPC.

		pui32Entries = static_cast<uint32_t *>(CMemLib::MAlloc( ui16NrChannels * ui16NrEntries * sizeof( uint32_t ) ));
		pui8ChannelSize = static_cast<uint8_t *>(CMemLib::MAlloc( ui16NrChannels ));
		pui8ChannelSign = static_cast<uint8_t *>(CMemLib::MAlloc( ui16NrChannels ));

		jp2_pclr = static_cast<opj_jp2_pclr_t *>(CMemLib::MAlloc( sizeof( opj_jp2_pclr_t ) ));
		jp2_pclr->channel_sign = pui8ChannelSign;
		jp2_pclr->channel_size = pui8ChannelSize;
		jp2_pclr->entries = pui32Entries;
		jp2_pclr->nr_entries = ui16NrEntries;
		jp2_pclr->nr_channels = ui16NrChannels;
		jp2_pclr->cmap = NULL;

		_pjcColor->jp2_pclr = jp2_pclr;

		for ( uint16_t I = 0; I < ui16NrChannels; ++I ) {
			ui8Uc = static_cast<uint8_t>(CCio::cio_read( _pcCio, 1 )); // Bi.
			pui8ChannelSize[I] = (ui8Uc & 0x7F) + 1;
			pui8ChannelSign[I] = (ui8Uc & 0x80) ? 1 : 0;
		}

		for ( uint16_t J = 0; J < ui16NrEntries; ++J ) {
			for ( uint16_t I = 0; I < ui16NrChannels; ++I ) {
				// Cji.
				(*pui32Entries++) = CCio::cio_read( _pcCio, pui8ChannelSize[I]>>3 );
			}
		}

		return true;
	}

	/**
	 * Collect component mapping data.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
	 * \param _pjcColor Collector for profile, cdef and pclr data.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_cmap( opj_jp2_t * /*_pjJp2*/, CCio::opj_cio_t * _pcCio, 
		opj_jp2_box_t * /*_pjbBox*/, opj_jp2_color_t * _pjcColor ) {
		opj_jp2_cmap_comp_t * pccMap;
		uint16_t I, ui16NrChannels;

		// Need ui16NrChannels.
		if ( _pjcColor->jp2_pclr == NULL ) { return false; }

		// Part 1, I.5.3.5: "There shall be at most one Component Mapping box inside a JP2 Header box".
		if ( _pjcColor->jp2_pclr->cmap ) { return false; }

		ui16NrChannels = _pjcColor->jp2_pclr->nr_channels;
		pccMap = static_cast<opj_jp2_cmap_comp_t *>(CMemLib::MAlloc( ui16NrChannels * sizeof( opj_jp2_cmap_comp_t ) ));

		for ( I = 0; I < ui16NrChannels; ++I ) {
			pccMap[I].cmp = static_cast<uint16_t>(CCio::cio_read( _pcCio, 2 ));
			pccMap[I].mtyp = static_cast<uint8_t>(CCio::cio_read( _pcCio, 1 ));
			pccMap[I].pcol = static_cast<uint8_t>(CCio::cio_read( _pcCio, 1 ));

		}
		_pjcColor->jp2_pclr->cmap = pccMap;

		return true;
	}

	/**
	 * Collect color specification data.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
	 * \param _pjcColor Collector for profile, cdef and pclr data.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_colr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
		opj_jp2_box_t * _pjbBox, opj_jp2_color_t * _pjcColor ) {
		int32_t i32SkipLen;
		CJpegCommonStructs::opj_common_ptr cpInfo;

		// Part 1, I.5.3.3 : "A conforming JP2 reader shall ignore all Color Specification boxes after the first."
		if ( _pjcColor->jp2_has_colr ) { return false; }

		cpInfo = _pjJp2->cinfo;

		_pjJp2->meth = CCio::cio_read( _pcCio, 1 );		// METH.
		_pjJp2->precedence = CCio::cio_read( _pcCio, 1 );	// PRECEDENCE.
		_pjJp2->approx = CCio::cio_read( _pcCio, 1 );		// APPROX.

		if ( _pjJp2->meth == 1 ) {
			_pjJp2->enumcs = CCio::cio_read( _pcCio, 4 );	// EnumCS.
		} 
		else {
			// skip PROFILE.
			i32SkipLen = _pjbBox->init_pos + _pjbBox->length - CCio::cio_tell( _pcCio );
			if ( i32SkipLen < 0 ) 		{
				CEvent::opj_event_msg( cpInfo, EVT_ERROR, "Error with COLR box size.\r\n" );
				return false;
			}
			if ( i32SkipLen > 0 ) {
				uint8_t * pui8Start;

				pui8Start = CCio::cio_getbp( _pcCio );
				_pjcColor->icc_profile_buf = static_cast<uint8_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32SkipLen) ));
				_pjcColor->icc_profile_len = i32SkipLen;

				CCio::cio_skip( _pcCio, _pjbBox->init_pos + _pjbBox->length - CCio::cio_tell( _pcCio ) );

				CStd::MemCpy( _pjcColor->icc_profile_buf, pui8Start, static_cast<uint32_t>(i32SkipLen) );
			}
		}

		if ( CCio::cio_tell( _pcCio ) - _pjbBox->init_pos != _pjbBox->length ) {
			CEvent::opj_event_msg( cpInfo, EVT_ERROR, "Error with COLR box.\r\n" );
			return false;
		}
		_pjcColor->jp2_has_colr = 1;

		return true;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjJp2 JP2 handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
	 * \param _pjcColor Collector for profile, cdef and pclr data.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJp2::jp2_read_cdef( opj_jp2_t * /*_pjJp2*/, CCio::opj_cio_t * _pcCio, 
		opj_jp2_box_t * /*_pjbBox*/, opj_jp2_color_t * _pjcColor ) {
		opj_jp2_cdef_info_t * pjciInfo;
		uint16_t I, ui16N;

		// Part 1, I.5.3.6: "The shall be at most one Channel Definition box inside a JP2 Header box."
		if ( _pjcColor->jp2_cdef ) { return false; }

		if ( (ui16N = static_cast<uint16_t>(CCio::cio_read( _pcCio, 2 ))) == 0 ) { return false; }

		pjciInfo = static_cast<opj_jp2_cdef_info_t *>(CMemLib::MAlloc( ui16N * sizeof( opj_jp2_cdef_info_t ) ));

		_pjcColor->jp2_cdef = static_cast<opj_jp2_cdef_t *>(CMemLib::MAlloc( sizeof( opj_jp2_cdef_t ) ));
		_pjcColor->jp2_cdef->info = pjciInfo;
		_pjcColor->jp2_cdef->n = ui16N;

		for ( I = 0; I < ui16N; ++I ) {
			pjciInfo[I].cn = static_cast<uint16_t>(CCio::cio_read( _pcCio, 2 ));
			pjciInfo[I].typ = static_cast<uint16_t>(CCio::cio_read( _pcCio, 2 ));
			pjciInfo[I].asoc = static_cast<uint16_t>(CCio::cio_read( _pcCio, 2 ));
		}
		return true;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjcColor Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CJp2::free_color_data( opj_jp2_color_t * _pjcColor ) {
		if ( _pjcColor->jp2_pclr ) {
			jp2_free_pclr( _pjcColor );
		}
		if ( _pjcColor->jp2_cdef ) {
			if ( _pjcColor->jp2_cdef->info ) { CMemLib::Free( _pjcColor->jp2_cdef->info ); }
			CMemLib::Free( _pjcColor->jp2_cdef );
		}
		if ( _pjcColor->icc_profile_buf ) { CMemLib::Free( _pjcColor->icc_profile_buf ); }
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _piImage Undocumented parameter from the OpenJpeg Library.
	 * \param _pjcColor Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CJp2::jp2_apply_cdef( CJpegImage::opj_image_t * _piImage, opj_jp2_color_t * _pjcColor ) {
		opj_jp2_cdef_info_t * pjciInfo;
		int32_t i32ColorSpace;
		uint16_t I, ui16N, ui16Cn, ui16Typ, ui16Asoc, ui16Acn;

		i32ColorSpace = _piImage->color_space;
		pjciInfo = _pjcColor->jp2_cdef->info;
		ui16N = _pjcColor->jp2_cdef->n;

		for ( I = 0; I < ui16N; ++I ) {
			if ( (ui16Asoc = pjciInfo[I].asoc) == 0 ) { continue; }

			ui16Cn = pjciInfo[I].cn;
			ui16Typ = pjciInfo[I].typ;
			ui16Acn = ui16Asoc - 1;

			if ( ui16Cn != ui16Acn ) {
				CJpegImage::opj_image_comp_t icSaved;

				CStd::MemCpy( &icSaved, &_piImage->comps[ui16Cn], sizeof( CJpegImage::opj_image_comp_t ) );
				CStd::MemCpy( &_piImage->comps[ui16Cn], &_piImage->comps[ui16Acn], sizeof( CJpegImage::opj_image_comp_t ) );
				CStd::MemCpy( &_piImage->comps[ui16Acn], &icSaved, sizeof( CJpegImage::opj_image_comp_t ) );

				pjciInfo[I].asoc = ui16Cn + 1;
				pjciInfo[ui16Acn].asoc = pjciInfo[ui16Acn].cn + 1;
			}
		}
		if ( _pjcColor->jp2_cdef->info ) { CMemLib::Free( _pjcColor->jp2_cdef->info ); }

		CMemLib::Free( _pjcColor->jp2_cdef );
		_pjcColor->jp2_cdef = NULL;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pjcColor Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CJp2::jp2_free_pclr( opj_jp2_color_t * _pjcColor ) {
		CMemLib::Free( _pjcColor->jp2_pclr->channel_sign );
		CMemLib::Free( _pjcColor->jp2_pclr->channel_size );
		CMemLib::Free( _pjcColor->jp2_pclr->entries );

		if ( _pjcColor->jp2_pclr->cmap ) { CMemLib::Free( _pjcColor->jp2_pclr->cmap ); }

		CMemLib::Free( _pjcColor->jp2_pclr );
		_pjcColor->jp2_pclr = NULL;
	}

}	// namespace lsi
