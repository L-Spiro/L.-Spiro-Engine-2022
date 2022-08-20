/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A standard 2D vector of type float.
 */


#include "LSMVector3F.h"


namespace lsm {

	// == Members.
	/**
	 * Global up vector.
	 */
	CVector3F CVector3F::m_vUp = CVector3F( 0.0f, 1.0f, 0.0f );

	/**
	 * Global right vector.
	 */
	CVector3F CVector3F::m_vRight = CVector3F( 1.0f, 0.0f, 0.0f );

	/**
	 * Global forward vector.
	 */
	CVector3F CVector3F::m_vForward = CVector3F( 0.0f, 0.0f, 1.0f );

	/**
	 * Zero vector.
	 */
	CVector3F CVector3F::m_vZero = CVector3F( 0.0f, 0.0f, 0.0f );

}	// namespace lsm
