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
 * Description: An oriented bounding box in 2D.  Oriented bounding boxes attempt to make the tightest rectangular
 *	block around an object, using any axis for the fit.  Because their orientations can be arbitrary, collision tests
 *	against them can be more expensive, but they provide fewer false-positives.
 */

#include "LSPObb2d.h"


namespace lsp {

	// == Functions.
	/**
	 * Gets the closest point on this OBB to a given point.
	 *
	 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
	 * \return Returns the closest point on this box from the given point.
	 */
	CVector2 LSE_CALL CObb2d::ClosestPointToPoint( const CVector2 &_vPoint ) const {
		CVector2 vD = _vPoint - m_vCenter;

		// Start the result at the center of the box and step outward from there.
		CVector2 vReturn = m_vCenter;
		// For each axis.
		for ( uint32_t I = 2; I--; ) {
			// Project D onto the axis and get the distance along the axis of D
			//	from the box center.
			LSREAL fDist = vD.Dot( m_vAxes[I] );
			
			// If farther than the box extends, clamp.
			fDist = CStd::Min( fDist, m_vExtents[I] );
			fDist = CStd::Max( fDist, -m_vExtents[I] );

			// Step that distance along the world axis to get the world coordinate.
			vReturn += m_vAxes[I] * fDist;
		}
		return vReturn;
	}

	/**
	 * Gets the distance from the given point to this OBB.
	 *
	 * \param _vPoint The point from which the distance to this bounding box is to be calculated.
	 * \return Returns the squared distance from the closest point on this box to the given point to the
	 *	given point.
	 */
	LSREAL LSE_CALL CObb2d::GetSqDistFromPoint( const CVector2 &_vPoint ) const {
		CVector2 vD = _vPoint - m_vCenter;

		LSREAL fSqDist = LSM_ZERO;

		// For each axis.
		for ( uint32_t I = 2; I--; ) {

			// Project D onto the axis and get the distance along the axis of D
			//	from the box center and count any excess distance outside the box extents.
			LSREAL fDist = vD.Dot( m_vAxes[I] ), fExcess = LSM_ZERO;
			if ( fDist < -m_vExtents[I] ) {
				fExcess = fDist + m_vExtents[I];
			}
			else if ( fDist > m_vExtents[I] ) {
				fExcess = fDist - m_vExtents[I];
			}
			fSqDist += fExcess * fExcess;
		}
		return fSqDist;
	}

	/**
	 * Creates a new bounding box that represents the result of this bounding box transformed by the given matrix.
	 *
	 * \param _mTransform The matrix transform.
	 * \param _oob2Ret On return, this holds the transformed OBB.
	 */
	void LSE_CALL CObb2d::Transform( const CMatrix4x4 &_mTransform, CObb2d &_oob2Ret ) const {
		CMatrix4x4 mBase;
		mBase.SetRow( 0, CVector3( m_vAxes[0].x, m_vAxes[0].y, LSM_ZERO ) );
		mBase.SetRow( 1, CVector3( m_vAxes[1].x, m_vAxes[1].y, LSM_ZERO ) );
		mBase.SetRow( 2, CVector3( LSM_ZERO, LSM_ZERO, LSM_ONE ) );
		mBase._14 = mBase._24 = mBase._34 = LSM_ZERO;
		mBase.SetRow( 3, CVector3( m_vCenter.x, m_vCenter.y, LSM_ZERO ) );
		mBase._44 = LSM_ONE;

		CMatrix4x4 mFinal = mBase * _mTransform;

		// Extract the data back into the final OBB.
		_oob2Ret.m_vExtents = m_vExtents;
		CVector3 vTemp;
		mFinal.GetRow( 0, vTemp );
		_oob2Ret.m_vAxes[0] = CVector2( vTemp.x, vTemp.y );
		mFinal.GetRow( 1, vTemp );
		_oob2Ret.m_vAxes[1] = CVector2( vTemp.x, vTemp.y );
		mFinal.GetRow( 3, vTemp );
		_oob2Ret.m_vCenter = CVector2( vTemp.x, vTemp.y );

		// Normalize to account for scaling.
		LSREAL fLen = _oob2Ret.m_vAxes[0].LenSq();
		if ( !fLen ) {
			_oob2Ret.m_vExtents[0] = LSM_ZERO;
		}
		else {
			fLen = CMathLib::Sqrt( fLen );
			_oob2Ret.m_vExtents[0] *= fLen;
			_oob2Ret.m_vAxes[0] /= fLen;
		}
		fLen = _oob2Ret.m_vAxes[1].LenSq();
		if ( !fLen ) {
			_oob2Ret.m_vExtents[1] = LSM_ZERO;
		}
		else {
			fLen = CMathLib::Sqrt( fLen );
			_oob2Ret.m_vExtents[1] *= fLen;
			_oob2Ret.m_vAxes[1] /= fLen;
		}
	}

}	// namespace lsp
