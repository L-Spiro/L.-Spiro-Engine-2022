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
 * Description: DESCRIPTION.
 */


#ifndef __LSS_SOUNDSOURCETYPES_H__
#define __LSS_SOUNDSOURCETYPES_H__

#include "../LSSSoundLib.h"

namespace lss {

	// == Enumerations.
	/**
	 * The types of sound sources there are.
	 */
	enum LSS_SOUND_SOURCE {
		LSS_SS_STATIC					= 0x08000000,					// Ambiguous static source.
		LSS_SS_3DSTATIC					= 0x00000001,					// Non-streaming sound within a 3D world (coordinates expressed in XYZ).
		LSS_SS_2DSTATIC					= 0x00000002,					// Non-streaming sound within a 2D world (coordinates expressed in XY).
		LSS_SS_1DSTATIC					= 0x00000003,					// Non-streaming sound within a 2D world (coordinates expressed in X).  X
																		//	value allows panning between left and right speakers.
		LSS_SS_STREAMING				= 0x01000000,					// Ambiguous streaming source.
		LSS_SS_3DSTREAMING				= 0x00000004,					// Streaming sound within a 3D world (coordinates expressed in XYZ).
		LSS_SS_2DSTREAMING				= 0x00000005,					// Streaming sound within a 2D world (coordinates expressed in XY).
		LSS_SS_1DSTREAMING				= 0x00000006,					// Streaming sound within a 2D world (coordinates expressed in X).  X
																		//	value allows panning between left and right speakers.

		LSS_SS_MASK						= 0x00FFFFFF,					// To mask off the type part (from the category).
	};

}	// namespace lss

#endif	// __LSS_SOUNDSOURCETYPES_H__
