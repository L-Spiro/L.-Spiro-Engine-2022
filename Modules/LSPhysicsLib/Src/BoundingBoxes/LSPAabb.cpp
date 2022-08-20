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
 * Description: A min-max axis-aligned bounding box.
 */

#include "LSPAabb.h"


namespace lsp {

	// == Functions.
	/**
	 * Computes an AABB from a set of points given in the form of an array of CVector3's.
	 *
	 * \param _vPoints The array of points to enclose.
	 * \param _ui32Total The number of points in the array to which _vPoints points.
	 */
	void LSE_CALL CAabb::ComputeAabbFromPointArray( const CVector3 * _pvPoints, uint32_t _ui32Total ) {
		MakeInfinitelySmall();
		for ( uint32_t I = _ui32Total; I--; ) {
			m_vMin.x = CStd::Min( m_vMin.x, _pvPoints->x );
			m_vMin.y = CStd::Min( m_vMin.y, _pvPoints->y );
			m_vMin.z = CStd::Min( m_vMin.z, _pvPoints->z );

			m_vMax.x = CStd::Max( m_vMax.x, _pvPoints->x );
			m_vMax.y = CStd::Max( m_vMax.y, _pvPoints->y );
			m_vMax.z = CStd::Max( m_vMax.z, _pvPoints->z );
			_pvPoints++;
		}
	}

	/**
	 * Computes an AABB from an existing AABB with a matrix-stored translation.
	 *	The matrix must be row-major.
	 *
	 * \param _aabbOther The source bounding box to transform by the given matrix and store in this object.
	 * \param _mTrans The matrix used to transform _aabbOther into this bounding box.
	 */
	void LSE_CALL CAabb::ComputeAabbFromAabbAndMatrix( const CAabb &_aabbOther, const CMatrix4x4 &_mTrans ) {
		// For each axis.
		for ( uint32_t I = 0; I < 3; ++I ) {
			// Add the translation.
			m_vMin[I] = m_vMax[I] = _mTrans( 3, I );

			// Create extents by summing smaller and larger terms on this axis only.
			for ( uint32_t J = 0; J < 3; ++J ) {
				LSREAL fE = _mTrans( J, I ) * _aabbOther.m_vMin[J];
				LSREAL fF = _mTrans( J, I ) * _aabbOther.m_vMax[J];

				if ( fE < fF ) {
					m_vMin[I] += fE;
					m_vMax[I] += fF;
				}
				else {
					m_vMin[I] += fF;
					m_vMax[I] += fE;
				}
			}
		}
	}

	/**
	 * Change the size of the AABB to encompass another AABB.
	 *
	 * \param _aabbOther The bounding box to fully encompass by this bounding box.
	 */
	void LSE_CALL CAabb::Encompass( const CAabb &_aabbOther ) {
		m_vMin.x = CStd::Min( m_vMin.x, _aabbOther.m_vMin.x );
		m_vMax.x = CStd::Max( m_vMax.x, _aabbOther.m_vMax.x );

		m_vMin.y = CStd::Min( m_vMin.y, _aabbOther.m_vMin.y );
		m_vMax.y = CStd::Max( m_vMax.y, _aabbOther.m_vMax.y );

		m_vMin.z = CStd::Min( m_vMin.z, _aabbOther.m_vMin.z );
		m_vMax.z = CStd::Max( m_vMax.z, _aabbOther.m_vMax.z );
	}

	/**
	 * Change the size of the AABB to encompass a point.
	 *
	 * \param _vOther The point to fully encompass by this bounding box.
	 */
	void LSE_CALL CAabb::Encompass( const CVector3 &_vOther ) {
		m_vMin.x = CStd::Min( m_vMin.x, _vOther.x );
		m_vMax.x = CStd::Max( m_vMax.x, _vOther.x );

		m_vMin.y = CStd::Min( m_vMin.y, _vOther.y );
		m_vMax.y = CStd::Max( m_vMax.y, _vOther.y );

		m_vMin.z = CStd::Min( m_vMin.z, _vOther.z );
		m_vMax.z = CStd::Max( m_vMax.z, _vOther.z );
	}

	/**
	 * Expands the box in all 3 directions by the given amount.
	 *
	 * \param _fAmount The amount by which to expand the box.
	 */
	void LSE_CALL CAabb::Expand( LSREAL _fAmount ) {
		m_vMin -= _fAmount;
		m_vMax += _fAmount;
	}

	/**
	 * Fill an array of 8 vectors with the vertices of the bounding box represented by this object.
	 *
	 * \param _vPoints An array of at least 8 vectors to be filled with the points at the corners of
	 *	this bounding box.
	 */
	void LSE_CALL CAabb::GetBox( CVector3 _vPoints[8] ) const {
		_vPoints[LSP_AC_FORWARD_LEFT_BOTTOM].x = m_vMin.x;
		_vPoints[LSP_AC_FORWARD_LEFT_BOTTOM].y = m_vMax.y;
		_vPoints[LSP_AC_FORWARD_LEFT_BOTTOM].z = m_vMin.z;

		_vPoints[LSP_AC_FORWARD_LEFT_TOP].x = m_vMin.x;
		_vPoints[LSP_AC_FORWARD_LEFT_TOP].y = m_vMax.y;
		_vPoints[LSP_AC_FORWARD_LEFT_TOP].z = m_vMax.z;

		_vPoints[LSP_AC_FORWARD_RIGHT_BOTTOM].x = m_vMax.x;
		_vPoints[LSP_AC_FORWARD_RIGHT_BOTTOM].y = m_vMax.y;
		_vPoints[LSP_AC_FORWARD_RIGHT_BOTTOM].z = m_vMin.z;

		_vPoints[LSP_AC_FORWARD_RIGHT_TOP].x = m_vMax.x;
		_vPoints[LSP_AC_FORWARD_RIGHT_TOP].y = m_vMax.y;
		_vPoints[LSP_AC_FORWARD_RIGHT_TOP].z = m_vMax.z;


		_vPoints[LSP_AC_BACK_LEFT_BOTTOM].x = m_vMin.x;
		_vPoints[LSP_AC_BACK_LEFT_BOTTOM].y = m_vMin.y;
		_vPoints[LSP_AC_BACK_LEFT_BOTTOM].z = m_vMin.z;

		_vPoints[LSP_AC_BACK_LEFT_TOP].x = m_vMin.x;
		_vPoints[LSP_AC_BACK_LEFT_TOP].y = m_vMin.y;
		_vPoints[LSP_AC_BACK_LEFT_TOP].z = m_vMax.z;

		_vPoints[LSP_AC_BACK_RIGHT_BOTTOM].x = m_vMax.x;
		_vPoints[LSP_AC_BACK_RIGHT_BOTTOM].y = m_vMin.y;
		_vPoints[LSP_AC_BACK_RIGHT_BOTTOM].z = m_vMin.z;

		_vPoints[LSP_AC_BACK_RIGHT_TOP].x = m_vMax.x;
		_vPoints[LSP_AC_BACK_RIGHT_TOP].y = m_vMin.y;
		_vPoints[LSP_AC_BACK_RIGHT_TOP].z = m_vMax.z;
	}

	/**
	 * Gets the center of the bounding box.
	 *
	 * \return Returns a point marking the center of this bounding box.
	 */
	CVector3 LSE_CALL CAabb::GetCenter() const {
		return (m_vMin + m_vMax) * LSM_HALF;
	}


	/**
	 * Gets the closest point on this AABB to a given point.
	 *
	 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
	 * \return Returns the closest point on this box from the given point.
	 */
	CVector3 LSE_CALL CAabb::ClosestPointToPoint( const CVector3 &_vPoint ) const {
		CVector3 vRet;
		LSREAL * pfRet = reinterpret_cast<LSREAL *>(&vRet);
		// For each axis, if the point is outside the box, clamp
		//	it to the bounds of the box.  Otherwise keep it as it is.
		for ( uint32_t I = 3; I--; ) {
			register LSREAL fVal = _vPoint[I];
			pfRet[I] = CStd::Clamp( fVal, m_vMin[I], m_vMax[I] );
		}
		return vRet;
	}

	/**
	 * Gets the distance from the given point to this AABB.
	 *
	 * \param _vPoint The point from which the distance to this bounding box is to be calculated.
	 * \return Returns the squared distance from the closest point on this box to the given point to the
	 *	given point.
	 */
	LSREAL LSE_CALL CAabb::GetSqDistFromPoint( const CVector3 &_vPoint ) const {
		LSREAL fDist = LSM_ZERO;

		// We do not need to get the actual position of the closest point.  It is sufficient
		//	enough to simply check if it extends beyond any axes and for each apply the correct
		//	squared distance.
		for ( uint32_t I = 0; I < 3; ++I ) {
			register LSREAL fVal = _vPoint[I];
			if ( fVal < m_vMin[I] ) {
				fDist += (m_vMin[I] - fVal) * (m_vMin[I] - fVal);
			}
			if ( fVal > m_vMax[I] ) {
				fDist += (fVal - m_vMax[I]) * (fVal - m_vMax[I]);
			}
		}
		return fDist;
	}

	/**
	 * Returns a point of an AABB by a special bit mask.
	 *
	 * \param _ui32Mask The bit mask.
	 * \return Returns the point determined by the special bit mask.
	 */
	CVector3 LSE_CALL CAabb::GetCorner( uint32_t _ui32Mask ) const {
		CVector3 vRet;
		vRet.x = (_ui32Mask & (1 << 0)) ? m_vMax.x : m_vMin.x;
		vRet.y = (_ui32Mask & (1 << 1)) ? m_vMax.y : m_vMin.y;
		vRet.z = (_ui32Mask & (1 << 2)) ? m_vMax.z : m_vMin.z;
		return vRet;
	}

}	// namespace lsp
