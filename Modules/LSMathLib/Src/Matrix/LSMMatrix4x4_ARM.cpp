/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn ( L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: An ARM-optimized matrix class.
 */


#ifdef __ARM_NEON__

#include "LSMMatrix4x4_ARM.h"

namespace lsm {

	// == Members.
	/** An identity matrix. */
	const CMatrix4x4_ARM CMatrix4x4_ARM::m_mIdentity = CMatrix4x4_ARM( LSM_ONE, LSM_ZERO, LSM_ZERO, LSM_ZERO,
		LSM_ZERO, LSM_ONE, LSM_ZERO, LSM_ZERO,
		LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ZERO,
		LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE );

}	// namespace lsm

#endif	// #ifdef __ARM_NEON__
