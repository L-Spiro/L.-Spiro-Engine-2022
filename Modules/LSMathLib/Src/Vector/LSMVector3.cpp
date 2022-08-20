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
 * Description: A standard 2D vector of type LSREAL.
 */


#include "LSMVector3.h"


namespace lsm {

	// == Members.
	/**
	 * Global up vector.
	 */
	CVector3 CVector3::m_vUp = CVector3( LSM_ZERO, LSM_ONE, LSM_ZERO );

	/**
	 * Global right vector.
	 */
	CVector3 CVector3::m_vRight = CVector3( LSM_ONE, LSM_ZERO, LSM_ZERO );

	/**
	 * Global forward vector.
	 */
	CVector3 CVector3::m_vForward = CVector3( LSM_ZERO, LSM_ZERO, LSM_ONE );

	/**
	 * Zero vector.
	 */
	CVector3 CVector3::m_vZero = CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO );

}	// namespace lsm
