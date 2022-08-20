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
 * Description: A standard 2D vector of type double.
 */


#include "LSMVector3D.h"


namespace lsm {

	// == Members.
	/**
	 * Global up vector.
	 */
	CVector3D CVector3D::m_vUp = CVector3D( 0.0, 1.0, 0.0 );

	/**
	 * Global right vector.
	 */
	CVector3D CVector3D::m_vRight = CVector3D( 1.0, 0.0, 0.0 );

	/**
	 * Global forward vector.
	 */
	CVector3D CVector3D::m_vForward = CVector3D( 0.0, 0.0, 1.0 );

	/**
	 * Zero vector.
	 */
	CVector3D CVector3D::m_vZero = CVector3D( 0.0, 0.0, 0.0 );

}	// namespace lsm
