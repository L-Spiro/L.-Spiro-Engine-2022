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
 * Description: A 3D cylinder stored as 2 points and a radius.
 */


#ifndef __LSM_CYLINDER3_H__
#define __LSM_CYLINDER3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"

namespace lsm {

	/**
	 * Class CCylinder3
	 * \brief A 3D cylinder.
	 *
	 * Description: A 3D cylinder stored as 2 points and a radius.
	 */
	class CCylinder3 {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CCylinder3();
		LSE_INLINE LSE_CALLCTOR				CCylinder3( const CCylinder3 &_cCylinder );
		LSE_INLINE LSE_CALLCTOR				CCylinder3( const CVector3 &_vA, const CVector3 &_vB, LSREAL _fR );


		// == Members.
		// The starting point.
		CVector3							a;

		// Ending point.
		CVector3							b;

		// Radius.
		LSREAL								r;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CCylinder3::CCylinder3() {
	}
	LSE_INLINE LSE_CALLCTOR CCylinder3::CCylinder3( const CCylinder3 &_cCylinder ) :
		a( _cCylinder.a ),
		b( _cCylinder.b ),
		r( _cCylinder.r ) {
	}
	LSE_INLINE LSE_CALLCTOR CCylinder3::CCylinder3( const CVector3 &_vA, const CVector3 &_vB, LSREAL _fR ) :
		a( _vA ),
		b( _vB ),
		r( _fR ) {
	}

}	// namespace lsm

#endif	// __LSM_CYLINDER3_H__
