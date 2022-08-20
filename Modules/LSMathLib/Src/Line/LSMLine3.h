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
 * Description: A 3D line stored as 2 points on the line.
 */



#ifndef __LSM_LINE3_H__
#define __LSM_LINE3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"

namespace lsm {

	/**
	 * Class CLine3
	 * \brief A 3D line and the math that loves it.
	 *
	 * Description: A 3D line stored as 2 points on the line.
	 */
	class CLine3 {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR			CLine3();
		LSE_INLINE LSE_CALLCTOR			CLine3( const CVector3 &_vStart, const CVector3 &_vEnd );
		LSE_INLINE LSE_CALLCTOR			CLine3( const CLine3 &_lLine );


		// == Functions.
		/**
		 * Gets the closest point on the line to the given point.
		 *	Also returns t for the position of that point along this line.
		 *
		 * \param _vPoint Point to test against this line.
		 * \param _fT Holds the returned time value of the returned point.
		 * \return Returns the closest point on the line to the given point and the time (t) for the
		 *	position of the point along the line.
		 */
		LSE_INLINE CVector3 LSE_FCALL	ClosestPointOnLineToPoint( const CVector3 &_vPoint, LSREAL &_fT ) const;


		// == Members.
		/**
		 * The starting point.
		 */
		CVector3						p;

		/**
		 * The ending point.
		 */
		CVector3						q;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CLine3::CLine3() {
	}
	LSE_INLINE LSE_CALLCTOR CLine3::CLine3( const CVector3 &_vStart, const CVector3 &_vEnd ) {
		p = _vStart;
		q = _vEnd;
	}
	LSE_INLINE LSE_CALLCTOR CLine3::CLine3( const CLine3 &_lLine ) {
		(*this) = _lLine;
	}

	// == Functions.
	/**
	 * Gets the closest point on the line to the given point.
	 *	Also returns t for the position of that point along this line.
	 *
	 * \param _vPoint Point to test against this line.
	 * \param _fT Holds the returned time value of the returned point.
	 * \return Returns the closest point on the line to the given point and the time (t) for the
	 *	position of the point along the line.
	 */
	LSE_INLINE CVector3 LSE_FCALL CLine3::ClosestPointOnLineToPoint( const CVector3 &_vPoint, LSREAL &_fT ) const {
		CVector3 vAb = q - p;
		// Project C onto AB, computing parameterized position d(t) = a + t*(b - a).
		_fT = vAb.Dot( _vPoint - p ) / vAb.LenSq();
		return p + (vAb * _fT);
	}

}	// namespace lsm

#endif	// __LSM_LINE3_H__
