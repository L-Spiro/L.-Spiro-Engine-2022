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


#include "LSMMatrix4x4_SSE3.h"

#ifdef LSE_X64

namespace lsm {

	// == Members.
	/** An identity matrix. */
	const CMatrix4x4_SSE3 CMatrix4x4_SSE3::m_mIdentity = CMatrix4x4_SSE3( LSM_ONE, LSM_ZERO, LSM_ZERO, LSM_ZERO,
		LSM_ZERO, LSM_ONE, LSM_ZERO, LSM_ZERO,
		LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ZERO,
		LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE );

}	// namespace lsm

#endif	// #ifdef LSE_X64
