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

#include "LSMSphere.h"


namespace lsm {

	// == Functions.
//#pragma optimize( "", off )
	/**
	 * Return a sphere that encloses the two given spheres.
	 *
	 * \param _sLeft Left operand.
	 * \param _sRight Right operand.
	 * \return Returns a sphere containing the two given sphere.
	 */
	CSphere LSE_FCALL CSphere::SphereEnclosingSpheres( const CSphere &_sLeft, const CSphere &_sRight ) {
		CVector3 vD = _sRight.c - _sLeft.c;
		LSREAL fDist = vD.LenSq();
		LSREAL fSqrt = _sRight.r - _sLeft.r;
		if ( fSqrt * fSqrt >= fDist ) {
			// One sphere encloses the other.  Return the larger of the 2.
			return _sRight.r > _sLeft.r ?
				_sRight :
				_sLeft;
		}
		
		// Spheres are disjoint or partially overlapping.
		fDist = CMathLib::Sqrt( fDist );
		CSphere sRet;
		sRet.r = (fDist + _sLeft.r + _sRight.r) * LSM_HALF;
		sRet.c = _sLeft.c;
		if ( fDist > LSM_REAL_EPSILON ) {
			sRet.c += vD * ((sRet.r - _sLeft.r) / fDist);
		}
		return sRet;
	}
//#pragma optimize( "", on )

	/**
	 * Creates a new sphere that represents the given sphere modified by the given matrix.
	 *
	 * \param _sSphere The sphere to modify.
	 * \param _mMat The matrix by which to modify the given sphere.
	 * \return Returns a sphere that has been scaled and moved by the given matrix.
	 */
	CSphere LSE_FCALL CSphere::CreateSphereFromSphereAndMatrix( const CSphere &_sSphere, const CMatrix4x4 &_mMat ) {
		CSphere sRet;
		CVector3 vRow;
		_mMat.GetRow( 0, vRow );
		sRet.r = vRow.LenSq();
		_mMat.GetRow( 1, vRow );
		sRet.r = CStd::Max( sRet.r, vRow.LenSq() );
		_mMat.GetRow( 2, vRow );
		sRet.r = CStd::Max( sRet.r, vRow.LenSq() );
		sRet.r = CMathLib::Sqrt( sRet.r ) * _sSphere.r;
		//vRow = _sSphere.c;
		sRet.c = _mMat * _sSphere.c;
		return sRet;
	}

}	// namespace lsm
