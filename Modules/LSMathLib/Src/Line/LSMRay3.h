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
 * Description: A 3D ray stored as a point and normalized direction.
 */


#ifndef __LSM_RAY3_H__
#define __LSM_RAY3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "LSMRay3Base.h"

namespace lsm {

	/**
	 * Class CRay3
	 * \brief A 3D ray.
	 *
	 * Description: A 3D ray stored as a point and normalized direction.
	 */
	class CRay3 : public CRay3Base<LSREAL, CVector3> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR			CRay3();
		LSE_INLINE LSE_CALLCTOR			CRay3( const CRay3 &_rRay );
		LSE_INLINE LSE_CALLCTOR			CRay3( const CVector3 &_vPos, const CVector3 &_vDir );

	private :
		typedef CRay3Base<LSREAL, CVector3>
										Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CRay3::CRay3() {}
	LSE_INLINE LSE_CALLCTOR CRay3::CRay3( const CRay3 &_rRay ) :
		Parent( _rRay ) {
	}
	LSE_INLINE LSE_CALLCTOR CRay3::CRay3( const CVector3 &_vPos, const CVector3 &_vDir ) :
		Parent( _vPos, _vDir ) {
	}

}	// namespace lsm

#endif	// __LSM_RAY3_H__
