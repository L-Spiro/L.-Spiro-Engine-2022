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
 * Description: The hierarchy structure inside meshes.
 */


#ifndef __LSD_MESHHIERARCHY_H__
#define __LSD_MESHHIERARCHY_H__

#include "../LSDModelLib.h"

namespace lsd {

	/**
	 * Class LSD_MESH_HIERARCHY
	 * \brief The hierarchy structure inside meshes.
	 *
	 * Description: The hierarchy structure inside meshes.
	 */
	typedef struct LSD_MESH_HIERARCHY {
		uint64_t										ui64Id;					/**< Object ID. */
		uint64_t										ui64ParentId;			/**< Parent ID. */
		uint16_t										ui16Type;				/**< One of the LSD_MODEL_ATTRIBUTES enumerations. */
		uint16_t										ui16Children;			/**< Total children. */
		float											fScalePivot[3];			/**< Scale pivot point. */
		float											fScale[3];				/**< Scale. */
		float											fScaleOffset[3];		/**< Scale offset. */
		float											fRotPivot[3];			/**< Rotation pivot point. */
		float											fPostRot[3];			/**< Post rotation. */
		float											fRot[3];				/**< Rotation. */
		float											fPreRot[3];				/**< Pre rotation. */
		float											fRotOffset[3];			/**< Rotation offset. */
		float											fTranslation[3];		/**< Translation. */
	} * LPLSD_MESH_HIERARCHY, * const LPCLSD_MESH_HIERARCHY;

}	// namespace lsd

#endif	// __LSD_MESHHIERARCHY_H__
