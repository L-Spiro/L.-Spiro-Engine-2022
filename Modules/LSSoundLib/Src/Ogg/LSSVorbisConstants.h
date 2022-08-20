/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Constants found throughout the Vorbis code.  We keep them in one place to avoid circular
 *	dependencies that are hidden due to the strange nature of the Vorbis library include system (such that
 *	#includes can be anywhere in any file, not just at the top, etc.)
 */


#ifndef __LSS_VORBISCONSTANTS_H__
#define __LSS_VORBISCONSTANTS_H__

#include "../LSSSoundLib.h"


// General.
#define GENERAL_VENDOR_STRING					"Xiph.Org libVorbis 1.3.2"
#define ENCODE_VENDOR_STRING					"Xiph.Org libVorbis I 20101101 (Schaufenugget)"



// Vorbis PSY.
#define P_BANDS									17								/* 62Hz to 16kHz */
#define P_LEVELS								8								/* 30dB to 100dB */
#define P_LEVEL_0								30.0f							/* 30 dB */
#define P_NOISECURVES							3

#define NOISE_COMPAND_LEVELS					40

// Vorbis MDCT
#define DATA_TYPE								float
#define REG_TYPE								float
#define cPI3_8									0.38268343236508977175f
#define cPI2_8									0.70710678118654752441f
#define cPI1_8									0.92387953251128675613f

#define FLOAT_CONV( X )							static_cast<float>(X)
#define MULT_NORM( X )							(X)
#define HALVE(X)								((X) * 0.5f)

#define rint( X )								(::floor( (X) + 0.5f ))


#define fromdB( X )								(::exp( (X) * 0.11512925f ))

// The bark scale equations are approximations, since the original
//	table was somewhat hand rolled.  The below are chosen to have the
//	best possible fit to the rolled tables, thus their somewhat odd
//	appearance (these are more accurate and over a longer range than
//	the oft-quoted bark equations found in the texts I have).  The
//	approximations are valid from 0 - 30kHz (nyquist) or so.
// All f in Hz, z in Bark.

#define toBARK(N)								(13.1f * ::atan( 0.00074f * (N) ) + 2.24f * ::atan( (N) * (N) * 1.85e-8f ) + 1e-4f * (N))
#define fromBARK(Z)								(102.0f * (Z) - 2.0f * std::pow( Z, 2.0f ) + 0.4f * std::pow( Z, 3.0f ) + std::pow( 1.46f, Z ) - 1.0f)
#define toMEL(N)								(::log( 1.0f + (N) * 0.001f ) * 1442.695f)
#define fromMEL(M)								(1000.0f * ::exp( (M) / 1442.695f ) - 1000.0f)

// Frequency to octave.  We arbitrarily declare 63.5 Hz to be octave 0.0.
#define toOC(N)									(::log( N ) * 1.442695f - 5.965784f)
#define fromOC(O)								(::exp( ((O) + 5.965784f) * 0.693147f ))

#define NEGINF									-9999.0f


namespace lss {

	// == Types.
	/**
	 * File library values.
	 */
	enum {
		NOTOPEN									= 0,
		PARTOPEN								= 1,
		OPENED									= 2,
		STREAMSET								= 3,
		INITSET									= 4,

		CHUNKSIZE								= 65536,
		READSIZE								= 2048,
	};
	/**
	 * Envelope settings.
	 */
	enum {
		VE_PRE									= 16,
		VE_WIN									= 4,
		VE_POST									= 2,
		VE_AMP									= (VE_PRE + VE_POST - 1),

		VE_BANDS								= 7,
		VE_NEARDC								= 15,

		VE_MINSTRETCH							= 2,				// A bit less than short block.
		VE_MAXSTRETCH							= 12,				// one-third full block.
	};

	/**
	 * Block settings.
	 */
	enum LSS_VORBIS_BLOCK_SETTINGS {
		BLOCKTYPE_IMPULSE						= 0,
		BLOCKTYPE_PADDING						= 1,
		BLOCKTYPE_TRANSITION					= 0,
		BLOCKTYPE_LONG							= 1,
	};

	/**
	 * Packet settings.
	 */
	enum LSS_VORBIS_PACKET {
		PACKETBLOBS								= 15
	};

	/**
	 * Backend settings.
	 */
	enum LSS_VORBIS_BACKEND {
		VIF_POSIT								= 63,
		VIF_CLASS								= 16,
		VIF_PARTS								= 31,
	};

	/**
	 * Masking.
	 */
	enum LSS_VORBIS_MASKING {
		MAX_ATH									= 88,

		EHMER_OFFSET							= 16,
		EHMER_MAX								= 56,
	};

	/**
	 * Codebook.
	 */
	enum LSS_CODE_BOOK {
		// 32 bit float (not IEEE; nonnormalized mantissa +
		//	biased exponent): neeeeeee eeemmmmm mmmmmmmm mmmmmmmm
		//	Why not IEEE?  It's just not that important here.
		VQ_FEXP									= 10,
		VQ_FMAN									= 21,
		VQ_FEXP_BIAS							= 768, // Bias toward values smaller than 1.
	};

	/**
	 * Blocks.
	 */
	enum LSS_BLOCKS {
		WORD_ALIGN								= 8
	};

	/**
	 * Registry.
	 */
	enum LSS_REGISTRY {
		VI_TRANSFORMB							= 1,
		VI_WINDOWB								= 1,
		VI_TIMEB								= 1,
		VI_FLOORB								= 2,
		VI_RESB									= 3,
		VI_MAPB									= 1,
	};

	/**
	 * Class CVorbisConstants
	 * \brief A few helper functions.
	 *
	 * Description: A few helper functions.
	 */
	class CVorbisConstants {
	public :
		// == Members.
		/**
		 * Undocumented Vorbis data.
		 */
		static const double					stereo_threshholds[];

		/**
		 * Undocumented Vorbis data.
		 */
		static const double					stereo_threshholds_limited[];

		/**
		 * Undocumented Vorbis data.
		 */
		static float							FLOOR1_fromdB_LOOKUP[256];


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _fX Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float							unitnorm( float _fX );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfX Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float							todB( const float * _pfX );
#define todB_nn( X )							todB( X )

	};

}	// namespace lss

#endif	// __LSS_VORBISCONSTANTS_H__
