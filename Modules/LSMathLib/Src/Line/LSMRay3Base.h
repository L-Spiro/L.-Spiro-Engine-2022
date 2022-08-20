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
 * Description: A 3D ray stored as 1 point and a normalized direction.
 */


#ifndef __LSM_RAY3BASE_H__
#define __LSM_RAY3BASE_H__

#include "../LSMMathLib.h"

namespace lsm {

	/**
	 * Class CRay3Base
	 * \brief A 3D ray stored as 1 point and a normalized direction.
	 *
	 * Description: A 3D ray stored as 1 point and a normalized direction.
	 */
	template <typename _tType, typename _tVec3Type>
	class CRay3Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR			CRay3Base() {
		}
		LSE_INLINE LSE_CALLCTOR			CRay3Base( const CRay3Base<_tType, _tVec3Type> &_rRay ) {
			(*this) = _rRay;
		}
		LSE_INLINE LSE_CALLCTOR			CRay3Base( const _tVec3Type &_vPos, const _tVec3Type &_vDir ) {
			p = _vPos;
			dir = _vDir;
			dir.Normalize();
		}


		// == Functions.
		/**
		 * Creates a ray between two points such that the ray begins at the first point
		 *	and points through the second.
		 *
		 * \param _vPos0 The first position, which acts as the starting position for the created ray.
		 * \param _vPos1 The second position, through which the created ray points.
		 */
		LSE_INLINE void LSE_FCALL		CreateBetweenPoints( const _tVec3Type &_vPos0, const _tVec3Type &_vPos1 ) {
			p = _vPos0;
			dir = _vPos1 - _vPos0;
			dir.Normalize();
		}


		// == Members.
		/**
		 * The point of the ray.
		 */
		_tVec3Type						p;

		/**
		 * The direction of the ray.
		 */
		_tVec3Type						dir;

	};

}	// namespace lsm

#endif	// __LSM_RAY3BASE_H__
