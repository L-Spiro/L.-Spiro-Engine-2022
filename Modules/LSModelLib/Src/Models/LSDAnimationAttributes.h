/**
 * Copyright L. Spiro 2012
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


#ifndef __LSD_ANIMATIONATTRIBUTES_H__
#define __LSD_ANIMATIONATTRIBUTES_H__

#include "../LSDModelLib.h"

namespace lsd {

	// == Enumerations.
	/** Animation types. */
	enum LSD_ANIMATION_ATTRIBUTES {
		// General.
		LSD_AA_TRANS_X,							/**< X translation. */
		LSD_AA_TRANS_Y,							/**< Y translation. */
		LSD_AA_TRANS_Z,							/**< Z translation. */
		LSD_AA_ROT_X,							/**< X rotation. */
		LSD_AA_ROT_Y,							/**< Y rotation. */
		LSD_AA_ROT_Z,							/**< Z rotation. */
		LSD_AA_SCALE_X,							/**< X scale. */
		LSD_AA_SCALE_Y,							/**< Y scale. */
		LSD_AA_SCALE_Z,							/**< Z scale. */

		LSD_AA_RED,								/**< Red. */
		LSD_AA_GREEN,							/**< Green. */
		LSD_AA_BLUE,							/**< Blue. */

		// Visibility.
		LSD_AA_VISIBILITY,						/**< Visibility. */

		// Occlusion.
		LSD_AA_OCCLUSION,						/**< Occlusion. */

		// IK.
		LSD_AA_IK_REACH_TRANSLATION,			/**< IK reach translation. */
		LSD_AA_IK_REACH_ROTATION,				/**< IK reach rotation. */

		// Lights.
		LSD_AA_LIGHT_CONEANGLE,					/**< Light cone angle. */
		LSD_AA_LIGHT_FOG,						/**< Light fog. */
		LSD_AA_LIGHT_INTENSITY,					/**< Light intensity. */

		// Cameras.
		LSD_AA_CAMERA_FIELDOFVIEW,				/**< Camera field of view. */
		LSD_AA_CAMERA_FIELDOFVIEWX,				/**< Camera field of view X. */
		LSD_AA_CAMERA_FIELDOFVIEWY,				/**< Camera field of view Y. */
		LSD_AA_CAMERA_FOCALLENGTH,				/**< Camera focal length. */
		LSD_AA_CAMERA_OPTICALCENTERX,			/**< Camera optical center along the X axis. */
		LSD_AA_CAMERA_OPTICALCENTERY,			/**< Camera optical center along the Y axis. */
		LSD_AA_CAMERA_ROLL,						/**< Camera roll. */
		LSD_AA_CAMERA_TURNTABLE,				/**< Camera turntable. */
		LSD_AA_CAMERA_INDEX,					/**< Camera index (for camera switchers). */

		// Materials.
		LSD_AA_MATERIAL_OPACITY,				/**< Material opacity. */
		LSD_AA_MATERIAL_REFLECTIVITY,			/**< Material reflectivity. */
		LSD_AA_MATERIAL_SHININESS,				/**< Material shininess. */


		// Higher-level attributes.  Shifted left by 16.
		LSD_AA_NONE = 0,						/**< None. */
		LSD_AA_LOCAL_TRANSFORM,					/**< Local transform. */

		// Unused.
		LSD_AA_FORCE_DWORD						= 0x7FFFFFFF,
	};

}	// namespace lsd

#endif	// __LSD_ANIMATIONATTRIBUTES_H__
