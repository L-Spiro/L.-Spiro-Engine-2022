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
 * Description: Parameters passed when creating a sound source.  This is easier to extend and pass than a
 *	bunch of parameters.
 */


#ifndef __LSS_SOUNDSOURCELOADPARMS_H__
#define __LSS_SOUNDSOURCELOADPARMS_H__

#include "../LSSSoundLib.h"
#include "Vector/LSMVector3.h"

namespace lss {

	/**
	 * Parameters passed when creating a sound source.  This is easier to extend and pass than a
	 *	bunch of parameters.
	 */
	typedef struct LSS_SOUND_SOURCE_LOAD_PARMS {
		/**
		 * Position.
		 */
		CVector3							vPos,
		/**
		 * Velocity.
		 */
											vVel,
		/**
		 * Direction.
		 */
											vDir;
		/**
		 * Pitch multiplier.
		 */
		LSSREAL								fPitch;

		/**
		 * Gain (volume).
		 */
		LSSREAL								fGain;

		/**
		 * Maximum distance.
		 */
		LSSREAL								fMaxDist;

		/**
		 * Rolloff.
		 */
		LSSREAL								fRollOff;

		/**
		 * Minimum gain.
		 */
		LSSREAL								fMinGain;

		/**
		 * Maximum gain.
		 */
		LSSREAL								fMaxGain;

		/**
		 * Gain outside of the cone.
		 */
		LSSREAL								fOuterGain;

		/**
		 * Inner cone angle in degrees.
		 */
		LSSREAL								fInnerConeAngle;
		
		/**
		 * Cone angle in degrees.
		 */
		LSSREAL								fConeAngle;

		/**
		 * Unique ID or 0 to have one generated.  If a unique ID is supplied it must be greater than or
		 *	equal to LSS_UNIQUE_ID.
		 */
		uint32_t							ui32Id;

	} * LPLSS_SOUND_SOURCE_LOAD_PARMS, * const LPCLSS_SOUND_SOURCE_LOAD_PARMS;

}	// namespace lss

#endif	// __LSS_SOUNDSOURCELOADPARMS_H__
