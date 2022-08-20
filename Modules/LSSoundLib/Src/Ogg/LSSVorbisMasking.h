/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: masking curve data for psychoacoustics
 last mod: $Id: masking.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISMASKING_H__
#define __LSS_VORBISMASKING_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisConstants.h"

namespace lss {

	/**
	 * Class CVorbisMasking
	 * \brief Masking curve data for psychoacoustics.
	 *
	 * Description: Masking curve data for psychoacoustics.
	 */
	class CVorbisMasking {
		// All is public.  This class has no secrets.
	public :
		// == Members.
		/**
		 * More detailed ATH.
		 */
		static const float			ATH[];

		/**
		 * Masking tones from -50 to 0dB, 62.5 through 16kHz at half octaves
		 *	test tones from -2 octaves to +5 octaves sampled at eighth octaves.
		 *	(Vorbis 0dB, the loudest possible tone, is assumed to be ~100dB SPL
		 * for collection of these curves)
		 */
		static const float			tonemasks[P_BANDS][6][EHMER_MAX];
	};

}	// namespace lss

#endif	// __LSS_VORBISMASKING_H__

