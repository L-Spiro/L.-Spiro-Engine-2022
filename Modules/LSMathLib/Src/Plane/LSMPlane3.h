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
 * Description: A 3D plane stored in constant-normal form (normal + distance from origin).
 *	Note that the plane is stored such that ax + by + cx - d = 0 instead of ax + by + cx + d = 0
 *	to avoid the superfluous negation that frequently occurs in math related to planes.
 */



#ifndef __LSM_PLANE3_H__
#define __LSM_PLANE3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "LSMPlane3Base.h"

namespace lsm {

	

	/**
	 * Class CPlane3
	 * \brief A 3D plane.
	 *
	 * Description: A 3D plane stored in constant-normal form (normal + distance from origin).
	 *	Note that the plane is stored such that ax + by + cx - d = 0 instead of ax + by + cx + d = 0
	 *	to avoid the superfluous negation that frequently occurs in math related to planes.
	 */
	class CPlane3 : public CPlane3Base<LSREAL, CVector3> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CPlane3();
		LSE_INLINE LSE_CALLCTOR				CPlane3( const CPlane3 &_pPlane );
		LSE_INLINE LSE_CALLCTOR				CPlane3( const CVector3 &_vPoint0, const CVector3 &_vPoint1, const CVector3 &_vPoint2 );
		LSE_INLINE LSE_CALLCTOR				CPlane3( const CVector3 &_vNormal, LSREAL _fDistance );
		LSE_INLINE LSE_CALLCTOR				CPlane3( const CVector3 &_vPoint, const CVector3 &_vNormal );


		// == Functions.
		/**
		 * Normalize this plane.
		 */
		LSE_INLINE void LSE_FCALL			Normalize();


	private :
		typedef CPlane3Base<double, CVector3>
											Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CPlane3::CPlane3() {
	}
	LSE_INLINE LSE_CALLCTOR CPlane3::CPlane3( const CPlane3 &_pPlane ) {
		(*this) = _pPlane;
	}
	LSE_INLINE LSE_CALLCTOR CPlane3::CPlane3( const CVector3 &_vPoint0, const CVector3 &_vPoint1, const CVector3 &_vPoint2 ) {
		n = CVector3::CrossProduct( _vPoint1 - _vPoint0, _vPoint2 - _vPoint0 );
		n.Normalize();
		dist = n.Dot( _vPoint0 );
	}
	LSE_INLINE LSE_CALLCTOR CPlane3::CPlane3( const CVector3 &_vNormal, LSREAL _fDistance ) {
		n = _vNormal;
		dist = _fDistance;
	}
	LSE_INLINE LSE_CALLCTOR CPlane3::CPlane3( const CVector3 &_vPoint, const CVector3 &_vNormal ) {
		n = _vNormal;
		dist = n.Dot( _vPoint );
	}

	// == Functions.
	/**
	 * Normalize this plane.
	 */
	LSE_INLINE void LSE_FCALL CPlane3::Normalize() {
		LSREAL fInvMag = CMathLib::InvSqrt( n.LenSq() );

		n.x *= fInvMag;
		n.y *= fInvMag;
		n.z *= fInvMag;
		dist *= fInvMag;
	}


}	// namespace lsm

#endif	// __LSM_PLANE3_H__
