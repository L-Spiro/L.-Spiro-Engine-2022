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
 * Description: A sphere stored as a center position and radius.
 */



#ifndef __LSE_SPHERE_H__
#define __LSE_SPHERE_H__

#include "../LSMMathLib.h"
#include "../Matrix/LSMMatrix4x4.h"
#include "../Vector/LSMVector3.h"

namespace lsm {

	/**
	 * Class CSphere
	 *
	 * Description: A sphere.
	 */
	class CSphere {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CSphere();
		LSE_INLINE LSE_CALLCTOR				CSphere( const CSphere &_sSphere );
		LSE_INLINE LSE_CALLCTOR				CSphere( const CVector3 &_vPos, LSREAL _fRadius );


		// == Functions.
		/**
		 * Return a sphere that encloses the two given spheres.
		 *
		 * \param _sLeft Left operand.
		 * \param _sRight Right operand.
		 * \return Returns a sphere containing the two given sphere.
		 */
		static CSphere LSE_FCALL			SphereEnclosingSpheres( const CSphere &_sLeft, const CSphere &_sRight );

		/**
		 * Creates a new sphere that represents the given sphere modified by the given matrix.
		 *
		 * \param _sSphere The sphere to modify.
		 * \param _mMat The matrix by which to modify the given sphere.
		 * \return Returns a sphere that has been scaled and moved by the given matrix.
		 */
		static CSphere LSE_FCALL			CreateSphereFromSphereAndMatrix( const CSphere &_sSphere, const CMatrix4x4 &_mMat );

		// == Members.
		CVector3							c;
		LSREAL								r;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CSphere::CSphere() {}
	LSE_INLINE LSE_CALLCTOR CSphere::CSphere( const CSphere &_sSphere ) {
		(*this) = _sSphere;
	}
	LSE_INLINE LSE_CALLCTOR CSphere::CSphere( const CVector3 &_vPos, LSREAL _fRadius ) {
		c = _vPos;
		r = _fRadius;
	}


}	// namespace lsm

#endif	// __LSE_SPHERE_H__
