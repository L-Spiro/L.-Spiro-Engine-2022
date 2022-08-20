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
 * Description: A 3D capsule stored as 2 points and a radius.
 */


#ifndef __LSM_CAPSULE3_H__
#define __LSM_CAPSULE3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"

namespace lsm {

	/**
	 * Class CCapsule3
	 * \brief A 3D capsule.
	 *
	 * Description: A 3D capsule stored as 2 points and a radius.
	 */
	class CCapsule3 {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CCapsule3();
		LSE_INLINE LSE_CALLCTOR				CCapsule3( const CCapsule3 &_cCapsule );
		LSE_INLINE LSE_CALLCTOR				CCapsule3( const CVector3 &_vA, const CVector3 &_vB, LSREAL _fR );


		// == Members.
		/**
		 * The starting point.
		 */
		CVector3							a;

		/**
		 * Ending point.
		 */
		CVector3							b;

		/**
		 * Radius.
		 */
		LSREAL								r;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CCapsule3::CCapsule3() {
	}
	LSE_INLINE LSE_CALLCTOR CCapsule3::CCapsule3( const CCapsule3 &_cCapsule ) :
		a( _cCapsule.a ),
		b( _cCapsule.b ),
		r( _cCapsule.r ) {
	}
	LSE_INLINE LSE_CALLCTOR CCapsule3::CCapsule3( const CVector3 &_vA, const CVector3 &_vB, LSREAL _fR ) :
		a( _vA ),
		b( _vB ),
		r( _fR ) {
	}

}	// namespace lsm

#endif	// __LSM_CAPSULE3_H__
