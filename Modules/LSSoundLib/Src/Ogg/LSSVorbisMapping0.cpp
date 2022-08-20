/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2010             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: channel mapping 0 implementation
 last mod: $Id: mapping0.c 17022 2010-03-25 03:45:42Z xiphmont $

 ********************************************************************/

#include "LSSVorbisMapping0.h"
#include "LSSOggWindow.h"
#include "LSSVorbisCodecInternal.h"
#include "LSSVorbisMdct.h"
#include "LSSVorbisMisc.h"
#include "LSSVorbisPsy.h"
#include "LSSVorbisRegistry.h"
#include "LSSVorbisSmallFt.h"


namespace lss {

	// == Members.
	/**
	 * Undocumented Vorbis library data.
	 */
	const CVorbisBackends::vorbis_func_mapping CVorbisMapping0::mapping0_exportbundle = {
		mapping0_pack, 
		mapping0_unpack, 
		mapping0_free_info, 
		mapping0_forward, 
		mapping0_inverse
	};

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pvimVm Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMapping0::mapping0_pack( CVorbisCodec::vorbis_info * _pviVi, CVorbisStructs::vorbis_info_mapping * _pvimVm, COgg::oggpack_buffer * _pobOpb ) {
		int32_t I;
		CVorbisStructs::vorbis_info_mapping0 * pvimInfo = static_cast<CVorbisStructs::vorbis_info_mapping0 *>(_pvimVm);

		// Another "we meant to do it this way" hack.  Up to beta 4, we
		//	packed 4 binary zeros here to signify one submapping in use.  We
		//	now redefine that to mean four bitflags that indicate use of
		//	deeper features; bit0:submappings, bit1:coupling, 
		//	bit2, 3:reserved.  This is backward compatable with all actual uses
		//	of the beta code.

		if ( pvimInfo->submaps>1 ) {
			COgg::oggpack_write( _pobOpb, 1, 1 );
			COgg::oggpack_write( _pobOpb, pvimInfo->submaps - 1, 4 );
		}
		else {
			COgg::oggpack_write( _pobOpb, 0, 1 );
		}

		if ( pvimInfo->coupling_steps > 0 ) {
			COgg::oggpack_write( _pobOpb, 1, 1 );
			COgg::oggpack_write( _pobOpb, pvimInfo->coupling_steps-1, 8 );

			for ( I = 0; I < pvimInfo->coupling_steps; I++ ) {
				COgg::oggpack_write( _pobOpb, pvimInfo->coupling_mag[I], ilog( _pviVi->channels ) );
				COgg::oggpack_write( _pobOpb, pvimInfo->coupling_ang[I], ilog( _pviVi->channels ) );
			}
		}
		else {
			COgg::oggpack_write( _pobOpb, 0, 1 );
		}

		COgg::oggpack_write( _pobOpb, 0, 2 ); // 2 and 3: reserved.

		// We don't write the channel submappings if we only have one.
		if ( pvimInfo->submaps>1 ) {
			for ( I = 0; I < _pviVi->channels; I++ ) {
				COgg::oggpack_write( _pobOpb, pvimInfo->chmuxlist[I], 4 );
			}
		}
		for ( I = 0; I < pvimInfo->submaps; I++ ) {
			COgg::oggpack_write( _pobOpb, 0, 8 );	// Time i32SubMap unused.
			COgg::oggpack_write( _pobOpb, pvimInfo->floorsubmap[I], 8 );
			COgg::oggpack_write( _pobOpb, pvimInfo->residuesubmap[I], 8 );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_info_mapping * CVorbisMapping0::mapping0_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb ) {
		int32_t I, i32B;
		CVorbisStructs::vorbis_info_mapping0 * pvimInfo = static_cast<CVorbisStructs::vorbis_info_mapping0 *>(CMemLib::CMAlloc( sizeof( (*pvimInfo) ) ) );
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		CStd::MemSet( pvimInfo, 0, sizeof( (*pvimInfo) ) );

		i32B = COgg::oggpack_read( _pobOpb, 1 );
		if ( i32B < 0 ) { goto ErrOut; }
		if ( i32B ) {
			pvimInfo->submaps = COgg::oggpack_read( _pobOpb, 4 ) + 1;
			if ( pvimInfo->submaps <= 0 ) { goto ErrOut; }
		}
		else {
			pvimInfo->submaps = 1;
		}

		i32B = COgg::oggpack_read( _pobOpb, 1 );
		if ( i32B < 0 ) { goto ErrOut; }
		if ( i32B ) {
			pvimInfo->coupling_steps = COgg::oggpack_read( _pobOpb, 8 ) + 1;
			if ( pvimInfo->coupling_steps <= 0 ) { goto ErrOut; }
			for ( I = 0; I < pvimInfo->coupling_steps; I++ ) {
				int32_t i32TestM = pvimInfo->coupling_mag[I] = COgg::oggpack_read( _pobOpb, ilog( _pviVi->channels ) );
				int32_t i32TestA = pvimInfo->coupling_ang[I] = COgg::oggpack_read( _pobOpb, ilog( _pviVi->channels ) );

				if ( i32TestM < 0 || i32TestA < 0 ||
					 i32TestM == i32TestA ||
					 i32TestM >= _pviVi->channels ||
					 i32TestA >= _pviVi->channels ) { goto ErrOut; }
			}

		}

		if ( COgg::oggpack_read( _pobOpb, 2 ) != 0 ) { goto ErrOut; }

		if ( pvimInfo->submaps>1 ) {
			for ( I = 0; I < _pviVi->channels; I++ ) {
				pvimInfo->chmuxlist[I] = COgg::oggpack_read( _pobOpb, 4 );
				if ( pvimInfo->chmuxlist[I] >= pvimInfo->submaps || pvimInfo->chmuxlist[I] < 0 ) { goto ErrOut; }
			}
		}
		for ( I = 0; I < pvimInfo->submaps; I++ ) {
			COgg::oggpack_read( _pobOpb, 8 );
			pvimInfo->floorsubmap[I] = COgg::oggpack_read( _pobOpb, 8 );
			if ( pvimInfo->floorsubmap[I] >= pcsiCi->floors || pvimInfo->floorsubmap[I] < 0 ) { goto ErrOut; }
			pvimInfo->residuesubmap[I] = COgg::oggpack_read( _pobOpb, 8 );
			if ( pvimInfo->residuesubmap[I] >= pcsiCi->residues || pvimInfo->residuesubmap[I] < 0 ) { goto ErrOut; }
		}

		return pvimInfo;

	ErrOut:
		mapping0_free_info( pvimInfo );
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvmiI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMapping0::mapping0_free_info( CVorbisStructs::vorbis_info_mapping * _pvmiI ) {
		CVorbisStructs::vorbis_info_mapping0 * pvimInfo = static_cast<CVorbisStructs::vorbis_info_mapping0 *>(_pvmiI);
		if ( pvimInfo ) {
			CStd::MemSet( pvimInfo, 0, sizeof( (*pvimInfo) ) );
			CMemLib::Free( pvimInfo );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisMapping0::mapping0_forward( CVorbisCodec::vorbis_block * _pvbVb ) {
		CVorbisCodec::vorbis_dsp_state * pvdsVd = _pvbVb->vd;
		CVorbisCodec::vorbis_info * pviVi = pvdsVd->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		CVorbisCodecInternal::private_state * i32B = static_cast<CVorbisCodecInternal::private_state *>(_pvbVb->vd->backend_state);
		CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal);
		int32_t i32N = _pvbVb->pcmend;
		int32_t I, J, K;

		int32_t * pi32NonZero = static_cast<int32_t *>(::alloca( sizeof( (*pi32NonZero) ) * pviVi->channels ));
		float	** ppfMdct = static_cast<float **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, pviVi->channels * sizeof( (*ppfMdct) ) ));
		int32_t ** ppi32WorkI = static_cast<int32_t **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, pviVi->channels * sizeof( (*ppi32WorkI) ) ));
		int32_t *** pppi32FloorPosts = static_cast<int32_t ***>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, pviVi->channels * sizeof( (*pppi32FloorPosts) ) ));

		float fGlobalAmpMax = pvbiVbi->ampmax;
		float * pfLocalAmpMax = static_cast<float *>(::alloca( sizeof( (*pfLocalAmpMax) ) * pviVi->channels ));
		int32_t i32BlockType = pvbiVbi->blocktype;

		int32_t i32ModeNumber = _pvbVb->W;
		CVorbisStructs::vorbis_info_mapping0 * pvimInfo = static_cast<CVorbisStructs::vorbis_info_mapping0 *>(pcsiCi->map_param[i32ModeNumber]);
		CVorbisPsy::vorbis_look_psy * pvlpPsyLook = i32B->psy + i32BlockType + (_pvbVb->W ? 2 : 0);

		_pvbVb->mode = i32ModeNumber;

		for ( I = 0; I < pviVi->channels; I++ ) {
			float fScale = 4.0f / i32N;
			float fScaleDb;

			float * pfPcm = _pvbVb->pcm[I];
			float * pfLogFft = pfPcm;

			ppi32WorkI[I] = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32N / 2 * sizeof( (*(*ppi32WorkI)) ) ));
			ppfMdct[I] = static_cast<float *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32N / 2 * sizeof( (*(*ppfMdct)) ) ) );

			fScaleDb = CVorbisConstants::todB( &fScale ) + 0.345f;
			COggWindow::_vorbis_apply_window( pfPcm, i32B->window, pcsiCi->blocksizes, _pvbVb->lW, _pvbVb->W, _pvbVb->nW );



			// Transform the PCM data.
			// Only MDCT right now.
			CVorbisMdct::mdct_forward( static_cast<CVorbisMdct::mdct_lookup *>(i32B->transform[_pvbVb->W][0] ), pfPcm, ppfMdct[I] );

			// FFT yields more accurate tonal estimation (not phase sensitive ).
			CVorbisSmallFt::drft_forward( &i32B->fft_look[_pvbVb->W], pfPcm );
			pfLogFft[0] = fScaleDb + CVorbisConstants::todB( pfPcm ) + 0.345f;
			pfLocalAmpMax[I] = pfLogFft[0];
			for ( J = 1; J < i32N - 1; J += 2 ) {
				float fTemp = pfPcm[J]*pfPcm[J]+pfPcm[J+1]*pfPcm[J+1];
				fTemp = pfLogFft[( J+1 )>>1] = fScaleDb+.5f*CVorbisConstants::todB( &fTemp ) + 0.345f;
				if ( fTemp>pfLocalAmpMax[I] )pfLocalAmpMax[I] = fTemp;
			}

			if ( pfLocalAmpMax[I] > 0.0f ) { pfLocalAmpMax[I] = 0.0f; }
			if ( pfLocalAmpMax[I] > fGlobalAmpMax ) { fGlobalAmpMax = pfLocalAmpMax[I]; }



		}

		{
			float * pfNoise = static_cast<float *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32N / 2 * sizeof( (*pfNoise) ) ));
			float * pfTone = static_cast<float *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32N / 2 * sizeof( (*pfTone) ) ));

			for ( I = 0; I < pviVi->channels; I++ ) {
				// The encoder setup assumes that all the modes used by any specific bitrate tweaking use the same floor.
				int32_t i32SubMap = pvimInfo->chmuxlist[I];

				/* the following makes things clearer to *me* anyway */
				float * pfMdct = ppfMdct[I];
				float * pfLogFft = _pvbVb->pcm[I];

				float * pfLogMdct = pfLogFft+i32N / 2;
				float * pfLogMask = pfLogFft;

				_pvbVb->mode = i32ModeNumber;

				pppi32FloorPosts[I] = static_cast<int32_t **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, PACKETBLOBS * sizeof( (*(*pppi32FloorPosts)) ) ));
				CStd::MemSet( pppi32FloorPosts[I], 0, sizeof( (*(*pppi32FloorPosts)) ) * PACKETBLOBS );

				for ( J = 0; J < i32N / 2; J++ ) {
					pfLogMdct[J] = CVorbisConstants::todB( pfMdct+J )	+ 0.345f;
				}



				// First step; noise masking.  Not only does "noise masking"
				//	give us curves from which we can decide how much resolution
				//	to give pfNoise parts of the spectrum, it also implicitly hands
				//	us a tonality estimate ( the larger the value in the
				//	"noise_depth" vector, the more tonal that area is).

				CVorbisPsy::_vp_noisemask( pvlpPsyLook, pfLogMdct, pfNoise );


				// Second step: "all the other crap"; all the stuff that isn't
				//	computed/fit for bitrate management goes in the second psy
				//	vector.  This includes pfTone masking, peak limiting and ATH.
				CVorbisPsy::_vp_tonemask( pvlpPsyLook, pfLogFft, pfTone, fGlobalAmpMax, pfLocalAmpMax[I] );



				// Third step; we offset the pfNoise vectors, overlay tone
				//	masking.  We then do a floor1-specific line fit.  If we're
				//	performing bitrate management, the line fit is performed
				//	multiple times for up/down tweakage on demand.
				CVorbisPsy::_vp_offset_and_mix( pvlpPsyLook, 
					pfNoise, 
					pfTone, 
					1, 
					pfLogMask, 
					pfMdct, 
					pfLogMdct );



				// This algorithm is hardwired to floor 1 for now; abort out if
				//	we're not floor1.  This won't happen unless someone has
				//	broken the encode setup lib.  Guard it anyway.
				if ( pcsiCi->floor_type[pvimInfo->floorsubmap[i32SubMap]] != 1 ) { return -1; }

				pppi32FloorPosts[I][PACKETBLOBS/2] = 
					CVorbisCodecInternal::floor1_fit( _pvbVb, 
					static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(i32B->flr[pvimInfo->floorsubmap[i32SubMap]] ), 
					pfLogMdct, 
					pfLogMask );

				// Are we managing bitrate?  If so, perform two more fits for
				//	later rate tweaking (fits represent hi/lo).
				if ( CVorbisBitrate::vorbis_bitrate_managed( _pvbVb ) && pppi32FloorPosts[I][PACKETBLOBS/2] ) {
					// Higher rate by way of lower noise curve.
					CVorbisPsy::_vp_offset_and_mix( pvlpPsyLook, 
						pfNoise, 
						pfTone, 
						2, 
						pfLogMask, 
						pfMdct, 
						pfLogMdct );



					pppi32FloorPosts[I][PACKETBLOBS-1] = 
						CVorbisCodecInternal::floor1_fit( _pvbVb, 
						static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(i32B->flr[pvimInfo->floorsubmap[i32SubMap]]), 
						pfLogMdct, 
						pfLogMask );

					// Lower rate by way of higher noise curve.
					CVorbisPsy::_vp_offset_and_mix( pvlpPsyLook, 
						pfNoise, 
						pfTone, 
						0, 
						pfLogMask, 
						pfMdct, 
						pfLogMdct );



					pppi32FloorPosts[I][0] = 
						CVorbisCodecInternal::floor1_fit( _pvbVb, 
						static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(i32B->flr[pvimInfo->floorsubmap[i32SubMap]]), 
						pfLogMdct, 
						pfLogMask );

					// We also interpolate a range of intermediate curves for
					//	intermediate rates.
					for ( K = 1; K < PACKETBLOBS / 2; K++ )
						pppi32FloorPosts[I][K] = 
							CVorbisCodecInternal::floor1_interpolate_fit( _pvbVb, 
							static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(i32B->flr[pvimInfo->floorsubmap[i32SubMap]]), 
							pppi32FloorPosts[I][0], 
							pppi32FloorPosts[I][PACKETBLOBS/2], 
							K * 65536 / (PACKETBLOBS / 2) );
					for ( K = PACKETBLOBS / 2 + 1; K < PACKETBLOBS - 1; K++ )
						pppi32FloorPosts[I][K] = 
							CVorbisCodecInternal::floor1_interpolate_fit( _pvbVb, 
							static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(i32B->flr[pvimInfo->floorsubmap[i32SubMap]]), 
							pppi32FloorPosts[I][PACKETBLOBS/2], 
							pppi32FloorPosts[I][PACKETBLOBS-1], 
							(K - PACKETBLOBS / 2) * 65536 / (PACKETBLOBS / 2) );
				}
			}
		}
		pvbiVbi->ampmax = fGlobalAmpMax;



		// Iterate over the many masking curve fits we've created.

		{
			int32_t ** ppi32CoupleBundle = static_cast<int32_t **>(::alloca( sizeof( (*ppi32CoupleBundle) ) * pviVi->channels ));
			int32_t * pi32ZeroBundle = static_cast<int32_t *>(::alloca( sizeof( (*pi32ZeroBundle) ) * pviVi->channels ));

			for ( K = (CVorbisBitrate::vorbis_bitrate_managed( _pvbVb ) ? 0 : PACKETBLOBS / 2);
				K <= (CVorbisBitrate::vorbis_bitrate_managed( _pvbVb ) ? PACKETBLOBS - 1 : PACKETBLOBS / 2);
				K++ ) {
				COgg::oggpack_buffer * pobOpb = pvbiVbi->packetblob[K];

				// Start out our new packet blob with packet type and mode.
				// Encode the packet type.
				COgg::oggpack_write( pobOpb, 0, 1 );
				// Encode the i32ModeNumber.
				// Encode frame mode, pre, post windowsize, then dispatch.
				COgg::oggpack_write( pobOpb, i32ModeNumber, i32B->modebits );
				if ( _pvbVb->W ) {
					COgg::oggpack_write( pobOpb, _pvbVb->lW, 1 );
					COgg::oggpack_write( pobOpb, _pvbVb->nW, 1 );
				}

				// Encode floor, compute masking curve, sep out residue.
				for ( I = 0; I < pviVi->channels; I++ ) {
					int32_t i32SubMap = pvimInfo->chmuxlist[I];
					int32_t * pi32LogMaskI = ppi32WorkI[I];

					pi32NonZero[I] = CVorbisCodecInternal::floor1_encode( pobOpb, _pvbVb, 
						static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(i32B->flr[pvimInfo->floorsubmap[i32SubMap]]), 
						pppi32FloorPosts[I][K], 
						pi32LogMaskI );

				}

				// Our iteration is now based on masking curve, not prequant and coupling.  Only one prequant/coupling step.

				// Quantize/couple.
				// Incomplete implementation that assumes the tree is all depth one, or no tree at all.
				CVorbisPsy::_vp_couple_quantize_normalize( K, 
					&pcsiCi->psy_g_param, 
					pvlpPsyLook, 
					pvimInfo, 
					ppfMdct, 
					ppi32WorkI, 
					pi32NonZero, 
					pcsiCi->psy_g_param.sliding_lowpass[_pvbVb->W][K], 
					pviVi->channels );



				// Classify and encode by submap.
				for ( I = 0; I < pvimInfo->submaps; I++ ) {
					int32_t i32ChInBundle = 0;
					int32_t ** ppi32Classifications;
					int32_t i32ResNum = pvimInfo->residuesubmap[I];

					for ( J = 0; J < pviVi->channels; J++ ) {
						if ( pvimInfo->chmuxlist[J] == I ) {
							pi32ZeroBundle[i32ChInBundle] = 0;
							if ( pi32NonZero[J] ) { pi32ZeroBundle[i32ChInBundle] = 1; }
							ppi32CoupleBundle[i32ChInBundle++] = ppi32WorkI[J];
						}
					}

					ppi32Classifications = static_cast<int32_t **>(CVorbisRegistry::_residue_P[pcsiCi->residue_type[i32ResNum]]->
						class0( _pvbVb, i32B->residue[i32ResNum], ppi32CoupleBundle, pi32ZeroBundle, i32ChInBundle ));

					i32ChInBundle = 0;
					for ( J = 0; J < pviVi->channels; J++ ) {
						if ( pvimInfo->chmuxlist[J] == I ) {
							ppi32CoupleBundle[i32ChInBundle++] = ppi32WorkI[J];
						}
					}

					CVorbisRegistry::_residue_P[pcsiCi->residue_type[i32ResNum]]->
						forward( pobOpb, _pvbVb, i32B->residue[i32ResNum], ppi32CoupleBundle, pi32ZeroBundle, i32ChInBundle, ppi32Classifications, I );
				}

				// Done encoding.  Next protopacket.
			}

		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvimL Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisMapping0::mapping0_inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_info_mapping * _pvimL ) {
		CVorbisCodec::vorbis_dsp_state * pvdsVd = _pvbVb->vd;
		CVorbisCodec::vorbis_info * pviVi = pvdsVd->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		CVorbisCodecInternal::private_state * i32B = static_cast<CVorbisCodecInternal::private_state *>(pvdsVd->backend_state);
		CVorbisStructs::vorbis_info_mapping0 * pvimInfo = static_cast<CVorbisStructs::vorbis_info_mapping0 *>(_pvimL);

		int32_t I, J;
		int32_t i32N = _pvbVb->pcmend = pcsiCi->blocksizes[_pvbVb->W];

		float ** ppfPcmBundle = static_cast<float **>(::alloca( sizeof( (*ppfPcmBundle) ) * pviVi->channels ));
		int32_t * pi32ZeroBundle = static_cast<int32_t *>(::alloca( sizeof( (*pi32ZeroBundle) ) * pviVi->channels ));

		int32_t * pi32NonZero = static_cast<int32_t *>(::alloca( sizeof( (*pi32NonZero) ) * pviVi->channels ));
		void ** ppvFloorMemo = static_cast<void **>(::alloca( sizeof( (*ppvFloorMemo) ) * pviVi->channels ));

		// Recover the spectral envelope; store it in the PCM vector for now.
		for ( I = 0; I < pviVi->channels; I++ ) {
			int32_t i32SubMap = pvimInfo->chmuxlist[I];
			ppvFloorMemo[I] = CVorbisRegistry::_floor_P[pcsiCi->floor_type[pvimInfo->floorsubmap[i32SubMap]]]->
				inverse1( _pvbVb, i32B->flr[pvimInfo->floorsubmap[i32SubMap]] );
			if ( ppvFloorMemo[I] ) {
				pi32NonZero[I] = 1;
			}
			else {
				pi32NonZero[I] = 0;
			}
			CStd::MemSet( _pvbVb->pcm[I], 0, sizeof( (*_pvbVb->pcm[I]) ) * i32N / 2 );
		}

		// Channel coupling can "dirty" the pi32NonZero listing.
		for ( I = 0; I < pvimInfo->coupling_steps; I++ ) {
			if ( pi32NonZero[pvimInfo->coupling_mag[I]] ||
				 pi32NonZero[pvimInfo->coupling_ang[I]] ) {
				pi32NonZero[pvimInfo->coupling_mag[I]] = 1;
				pi32NonZero[pvimInfo->coupling_ang[I]] = 1;
			}
		}

		// Recover the residue into our working vectors.
		for ( I = 0; I < pvimInfo->submaps; I++ ) {
			int32_t i32ChInBundle = 0;
			for ( J = 0; J < pviVi->channels; J++ ) {
				if ( pvimInfo->chmuxlist[J] == I ) {
					if ( pi32NonZero[J] ) {
						pi32ZeroBundle[i32ChInBundle] = 1;
					}
					else {
						pi32ZeroBundle[i32ChInBundle] = 0;
					}
					ppfPcmBundle[i32ChInBundle++] = _pvbVb->pcm[J];
				}
			}

			CVorbisRegistry::_residue_P[pcsiCi->residue_type[pvimInfo->residuesubmap[I]]]->
				inverse( _pvbVb, i32B->residue[pvimInfo->residuesubmap[I]], ppfPcmBundle, pi32ZeroBundle, i32ChInBundle );
		}

		// Channel coupling.
		for ( I = pvimInfo->coupling_steps-1; I >= 0; I-- ) {
			float * pfPcmM = _pvbVb->pcm[pvimInfo->coupling_mag[I]];
			float * pfPcmA = _pvbVb->pcm[pvimInfo->coupling_ang[I]];

			for ( J = 0; J < i32N / 2; J++ ) {
				float fMag = pfPcmM[J];
				float fAng = pfPcmA[J];

				if ( fMag > 0 )
					if ( fAng > 0 ) {
						pfPcmM[J] = fMag;
						pfPcmA[J] = fMag - fAng;
					}
					else {
						pfPcmA[J] = fMag;
						pfPcmM[J] = fMag + fAng;
					}
				else {
					if ( fAng > 0 ) {
						pfPcmM[J] = fMag;
						pfPcmA[J] = fMag + fAng;
					}
					else {
						pfPcmA[J] = fMag;
						pfPcmM[J] = fMag - fAng;
					}
				}
			}
		}

		// Compute and apply spectral envelope.
		for ( I = 0; I < pviVi->channels; I++ ) {
			float * pfPcm = _pvbVb->pcm[I];
			int32_t i32SubMap = pvimInfo->chmuxlist[I];
			CVorbisRegistry::_floor_P[pcsiCi->floor_type[pvimInfo->floorsubmap[i32SubMap]]]->
				inverse2( _pvbVb, i32B->flr[pvimInfo->floorsubmap[i32SubMap]], ppvFloorMemo[I], pfPcm );
		}

		// Transform the PCM data; takes PCM vector, vb; modifies PCM vector.
		// Only MDCT right now.
		for ( I = 0; I < pviVi->channels; I++ ) {
			float * pfPcm = _pvbVb->pcm[I];
			CVorbisMdct::mdct_backward( static_cast<CVorbisMdct::mdct_lookup *>(i32B->transform[_pvbVb->W][0]), pfPcm, pfPcm );
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisMapping0::ilog( uint32_t _ui32V ) {
		int32_t i32Ret = 0;
		if ( _ui32V ) { --_ui32V; }
		while ( _ui32V ) {
			i32Ret++;
			_ui32V >>= 1;
		}
		return i32Ret;
	}

}	// namespace lss
