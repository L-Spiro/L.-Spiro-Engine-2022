/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: bitrate tracking and management
 last mod: $Id: bitrate.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/

#include "LSSVorbisBitrate.h"
#include "LSSVorbisCodecInternal.h"


namespace lss {

	//  == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pbmsBs Undocumented parameter from the Vorbis library.
	 */
	void CVorbisBitrate::vorbis_bitrate_init( CVorbisCodec::vorbis_info * _pviVi, bitrate_manager_state * _pbmsBs ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup );
		bitrate_manager_info * pbmiBi = &pcsiCi->bi;

		CStd::MemSet( _pbmsBs, 0, sizeof( (*_pbmsBs) ) );

		if ( pbmiBi && (pbmiBi->reservoir_bits > 0) ) {
			int32_t i32RateSamples = _pviVi->rate;
			int32_t	i32HalfSamples = pcsiCi->blocksizes[0] >> 1;

			_pbmsBs->short_per_long = pcsiCi->blocksizes[1]/pcsiCi->blocksizes[0];
			_pbmsBs->managed = 1;

			_pbmsBs->avg_bitsper = static_cast<int32_t>(rint( 1.0f * pbmiBi->avg_rate * i32HalfSamples / i32RateSamples ));
			_pbmsBs->min_bitsper = static_cast<int32_t>(rint( 1.0f * pbmiBi->min_rate * i32HalfSamples / i32RateSamples ));
			_pbmsBs->max_bitsper = static_cast<int32_t>(rint( 1.0f * pbmiBi->max_rate * i32HalfSamples / i32RateSamples ));

			_pbmsBs->avgfloat = PACKETBLOBS / 2;

			// Not a necessary fix, but one that leads to a more balanced
			//	typical initialization.
			{
				int32_t i32DesiredFill = static_cast<int32_t>(pbmiBi->reservoir_bits*pbmiBi->reservoir_bias);
				_pbmsBs->minmax_reservoir = i32DesiredFill;
				_pbmsBs->avg_reservoir = i32DesiredFill;
			}

		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pbmsBs Undocumented parameter from the Vorbis library.
	 */
	void CVorbisBitrate::vorbis_bitrate_clear( bitrate_manager_state * _pbmsBs ) {
		CStd::MemSet( _pbmsBs, 0, sizeof( (*_pbmsBs) ) );
		return;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisBitrate::vorbis_bitrate_managed( CVorbisCodec::vorbis_block * _pvbVb ) {
		CVorbisCodec::vorbis_dsp_state * pvdsVd = _pvbVb->vd;
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(pvdsVd->backend_state );
		bitrate_manager_state *bm = &ppsB->bms;

		if ( bm && bm->managed ) { return 1; }
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisBitrate::vorbis_bitrate_addblock( CVorbisCodec::vorbis_block * _pvbVb ) {
		CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal);
		CVorbisCodec::vorbis_dsp_state * pvdsVd = _pvbVb->vd;
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(pvdsVd->backend_state);
		bitrate_manager_state * pbmsBm = &ppsB->bms;
		CVorbisCodec::vorbis_info * pviVi = pvdsVd->vi;
		CVorbisCodecInternal::codec_setup_info * ci = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		bitrate_manager_info * pbmiBi = &ci->bi;

		int32_t	i32Choice = static_cast<int32_t>(rint( pbmsBm->avgfloat ));
		int32_t i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
		int32_t i32MinTargetBits = (_pvbVb->W ? pbmsBm->min_bitsper * pbmsBm->short_per_long : pbmsBm->min_bitsper);
		int32_t i32MaxTargetBits = (_pvbVb->W ? pbmsBm->max_bitsper * pbmsBm->short_per_long : pbmsBm->max_bitsper);
		int32_t	i32Samples = ci->blocksizes[_pvbVb->W] >> 1;
		int32_t i32DesiredFill = static_cast<int32_t>(pbmiBi->reservoir_bits*pbmiBi->reservoir_bias);


		if ( !pbmsBm->managed ) {
			// Not a bitrate managed stream, but for API simplicity, we'll
			//	buffer the packet to keep the code path clean.

			if ( pbmsBm->vb ) { return -1; }	// One has been submitted without
												//	being claimed.
			pbmsBm->vb = _pvbVb;
			return 0;
		}

		pbmsBm->vb = _pvbVb;

		// Look ahead for AVG floater.
		if ( pbmsBm->avg_bitsper > 0 ) {
			double dSlew = 0.0;
			int32_t i32AvgTargetBits = (_pvbVb->W ? pbmsBm->avg_bitsper * pbmsBm->short_per_long : pbmsBm->avg_bitsper);
			double dSlewLimit = 15.0 / pbmiBi->slew_damp;

			// Choosing a new floater:
			//	If we're over target, we dSlew down.
			//	If we're under target, we dSlew up.
			//
			// Choose dSlew as follows: Look through packetblobs of this frame
			//	and set dSlew as the first in the appropriate direction that
			//	gives us the dSlew we want.  This may mean no dSlew if delta is
			//	already favorable.
			//
			// Then limit dSlew to dSlew max.

			if ( pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) > i32DesiredFill ) {
				while ( i32Choice > 0 && i32ThisBits > i32AvgTargetBits &&
					pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) > i32DesiredFill ) {
					i32Choice--;
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}
			else if ( pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) < i32DesiredFill ) {
				while ( i32Choice + 1 < PACKETBLOBS && i32ThisBits < i32AvgTargetBits &&
					pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) < i32DesiredFill ) {
					i32Choice++;
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}

			dSlew = rint( i32Choice-pbmsBm->avgfloat ) / i32Samples*pviVi->rate;
			if ( dSlew<-dSlewLimit ) { dSlew = -dSlewLimit; }
			if ( dSlew>dSlewLimit ) { dSlew = dSlewLimit; } 
			i32Choice = static_cast<int32_t>(rint( pbmsBm->avgfloat += dSlew/pviVi->rate * i32Samples ));
			i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
		}



		// Enforce min (if used) on the current floater (if used).
		if ( pbmsBm->min_bitsper > 0 ) {
			// Do we need to force the bitrate up?
			if ( i32ThisBits < i32MinTargetBits ) {
				while ( pbmsBm->minmax_reservoir - (i32MinTargetBits - i32ThisBits) < 0 ) {
					i32Choice++;
					if ( i32Choice >= PACKETBLOBS ) { break; }
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}
		}

		// Enforce max (if used) on the current floater (if used).
		if ( pbmsBm->max_bitsper > 0 ) {
			// Do we need to force the bitrate down?
			if ( i32ThisBits > i32MaxTargetBits ) {
				while ( pbmsBm->minmax_reservoir + (i32ThisBits - i32MaxTargetBits) > pbmiBi->reservoir_bits ) {
					i32Choice--;
					if ( i32Choice < 0 ) { break; }
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}
		}

		// Choice of packetblobs now made based on floater, and min/max
		//	requirements. Now boundary check extreme choices.

		if ( i32Choice < 0 ) {
			// Choosing a smaller packetblob is insufficient to trim bitrate.
			//	frame will need to be truncated.
			int32_t i32MaxSize = (i32MaxTargetBits + (pbmiBi->reservoir_bits - pbmsBm->minmax_reservoir)) / 8;
			pbmsBm->choice = i32Choice = 0;

			if ( COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) > i32MaxSize ) {

				COgg::oggpack_writetrunc( pvbiVbi->packetblob[i32Choice], i32MaxSize * 8 );
				i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
			}
		}
		else {
			int32_t i32MinSize = (i32MinTargetBits - pbmsBm->minmax_reservoir + 7) / 8;
			if ( i32Choice >= PACKETBLOBS ) {
				i32Choice = PACKETBLOBS - 1;
			}

			pbmsBm->choice = i32Choice;

			// Prop up bitrate according to demand. pad this frame out with zeroes.
			i32MinSize -= COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] );
			while ( i32MinSize-- > 0 ) { COgg::oggpack_write( pvbiVbi->packetblob[i32Choice], 0, 8 ); }
			i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;

		}

		// Now we have the final packet and the final packet size.	Update statistics
		//	min and max reservoir.
		if ( pbmsBm->min_bitsper > 0 || pbmsBm->max_bitsper > 0 ) {

			if ( i32MaxTargetBits > 0 && i32ThisBits > i32MaxTargetBits ) {
				pbmsBm->minmax_reservoir += (i32ThisBits - i32MaxTargetBits);
			}
			else if ( i32MinTargetBits > 0 && i32ThisBits < i32MinTargetBits ) {
				pbmsBm->minmax_reservoir += (i32ThisBits - i32MinTargetBits);
			}
			else {
				// Inbetween; we want to take reservoir toward but not past i32DesiredFill.
				if ( pbmsBm->minmax_reservoir > i32DesiredFill ) {
					if ( i32MaxTargetBits>0 ) {		// Logical bulletproofing against initialization state.
						pbmsBm->minmax_reservoir += (i32ThisBits - i32MaxTargetBits);
						if ( pbmsBm->minmax_reservoir < i32DesiredFill ) { pbmsBm->minmax_reservoir = i32DesiredFill; }
					}
					else {
						pbmsBm->minmax_reservoir = i32DesiredFill;
					}
				}
				else {
					if ( i32MinTargetBits > 0 ) {	// Logical bulletproofing against initialization state.
						pbmsBm->minmax_reservoir += (i32ThisBits - i32MinTargetBits);
						if ( pbmsBm->minmax_reservoir > i32DesiredFill ) { pbmsBm->minmax_reservoir = i32DesiredFill; }
					}
					else {
						pbmsBm->minmax_reservoir = i32DesiredFill;
					}
				}
			}
		}

		// AVG reservoir.
		if ( pbmsBm->avg_bitsper > 0 ) {
			int32_t i32AvgTargetBits = (_pvbVb->W ? pbmsBm->avg_bitsper * pbmsBm->short_per_long : pbmsBm->avg_bitsper );
			pbmsBm->avg_reservoir += i32ThisBits - i32AvgTargetBits;
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsVd Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisBitrate::vorbis_bitrate_flushpacket( CVorbisCodec::vorbis_dsp_state * _pvdsVd, COgg::ogg_packet * _popOp ) {
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(_pvdsVd->backend_state);
		bitrate_manager_state * pbmsBm = &ppsB->bms;
		CVorbisCodec::vorbis_block * pvbVb = pbmsBm->vb;
		int32_t i32Choice = PACKETBLOBS / 2;
		if ( !pvbVb ) { return 0; }

		if ( _popOp ) {
			CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(pvbVb->internal);

			if ( vorbis_bitrate_managed( pvbVb ) ) {
				i32Choice = pbmsBm->choice;
			}

			_popOp->packet = COgg::oggpack_get_buffer( pvbiVbi->packetblob[i32Choice] );
			_popOp->bytes = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] );
			_popOp->b_o_s = 0;
			_popOp->e_o_s = pvbVb->eofflag;
			_popOp->granulepos = pvbVb->granulepos;
			_popOp->packetno = pvbVb->sequence;		// For sake of completeness.
		}

		pbmsBm->vb = 0;
		return 1;
	}

}	// namespace lss
