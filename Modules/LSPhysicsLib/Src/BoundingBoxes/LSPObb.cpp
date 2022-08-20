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
 * Description: An oriented bounding box.  Oriented bounding boxes attempt to make the tightest rectangular
 *	block around an object, using any axis for the fit.  Because their orientations can be arbitrary, collision tests
 *	against them can be more expensive, but they provide fewer false-positives.
 */

#include "LSPObb.h"


namespace lsp {

	// == Functions.
	/**
	 * Gets the closest point on this OBB to a given point.
	 *
	 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
	 * \return Returns the closest point on this box from the given point.
	 */
	CVector3 LSE_CALL CObb::ClosestPointToPoint( const CVector3 &_vPoint ) const {
		CVector3 vD = _vPoint - m_vCenter;

		// Start the result at the center of the box and step outward from there.
		CVector3 vReturn = m_vCenter;
		// For each axis.
		for ( uint32_t I = 3; I--; ) {
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
	LSREAL LSE_CALL CObb::GetSqDistFromPoint( const CVector3 &_vPoint ) const {
		CVector3 vD = _vPoint - m_vCenter;

		LSREAL fSqDist = LSM_ZERO;

		// For each axis.
		for ( uint32_t I = 3; I--; ) {

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
	 * \return Returns a new OBB representing this OBB transformed by the given matrix.
	 */
	CObb LSE_CALL CObb::Transform( const CMatrix4x4 &_mTransform ) const {
		CMatrix4x4 mBase;
		mBase.SetRow( 0, m_vAxes[0] );
		mBase.SetRow( 1, m_vAxes[1] );
		mBase.SetRow( 2, m_vAxes[2] );
		mBase._14 = mBase._24 = mBase._34 = LSM_ZERO;
		mBase.SetRow( 3, m_vCenter );
		mBase._44 = LSM_ONE;

		CMatrix4x4 mFinal = mBase * _mTransform;

		// Extract the data back into the final OBB.
		CObb obbReturn;
		obbReturn.m_vExtents = m_vExtents;
		mFinal.GetRow( 0, obbReturn.m_vAxes[0] );
		mFinal.GetRow( 1, obbReturn.m_vAxes[1] );
		mFinal.GetRow( 2, obbReturn.m_vAxes[2] );
		mFinal.GetRow( 3, obbReturn.m_vCenter );

		// Normalize to account for scaling.
		LSREAL fLen = obbReturn.m_vAxes[0].LenSq();
		if ( !fLen ) {
			obbReturn.m_vExtents[0] = LSM_ZERO;
		}
		else {
			fLen = CMathLib::Sqrt( fLen );
			obbReturn.m_vExtents[0] *= fLen;
			obbReturn.m_vAxes[0] /= fLen;
		}
		fLen = obbReturn.m_vAxes[1].LenSq();
		if ( !fLen ) {
			obbReturn.m_vExtents[1] = LSM_ZERO;
		}
		else {
			fLen = CMathLib::Sqrt( fLen );
			obbReturn.m_vExtents[1] *= fLen;
			obbReturn.m_vAxes[1] /= fLen;
		}
		fLen = obbReturn.m_vAxes[2].LenSq();
		if ( !fLen ) {
			obbReturn.m_vExtents[2] = LSM_ZERO;
		}
		else {
			fLen = CMathLib::Sqrt( fLen );
			obbReturn.m_vExtents[2] *= fLen;
			obbReturn.m_vAxes[2] /= fLen;
		}
		return obbReturn;
	}

}	// namespace lsp
