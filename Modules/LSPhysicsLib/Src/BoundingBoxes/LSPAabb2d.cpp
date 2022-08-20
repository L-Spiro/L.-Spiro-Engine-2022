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
 * Description: A min-max axis-aligned 2D bounding box.
 */

#include "LSPAabb2d.h"
#include "Vector/LSMVector3.h"


namespace lsp {

	// == Functions.
	/**
	 * Make the box infinitely small.
	 */
	void LSE_CALL CAabb2d::MakeInfinitelySmall() {
		m_vMin.x = m_vMin.y = LSM_INFINITY;
		m_vMax.x = m_vMax.y = -LSM_INFINITY;
	}

	/**
	 * Computes an AABB from a set of points given in the form of an array of CVector2's.
	 *
	 * \param _vPoints The array of points to enclose.
	 * \param _uiTotal The number of points in the array to which _vPoints points.
	 */
	void LSE_CALL CAabb2d::ComputeAabbFromPointArray( const CVector2 * _pvPoints, uint32_t _uiTotal ) {
		m_vMin.x = m_vMin.y = LSM_INFINITY;
		m_vMax.x = m_vMax.y = -LSM_INFINITY;
		for ( uint32_t I = _uiTotal; I--; ) {
			m_vMin.x = CStd::Min( m_vMin.x, _pvPoints->x );
			m_vMin.y = CStd::Min( m_vMin.y, _pvPoints->y );

			m_vMax.x = CStd::Max( m_vMax.x, _pvPoints->x );
			m_vMax.y = CStd::Max( m_vMax.y, _pvPoints->y );
			_pvPoints++;
		}
	}

	/**
	 * Computes an AABB from an existing AABB with a matrix-stored translation.
	 *	The matrix must be row-major.
	 *
	 * \param _aabb2Other The source bounding box to transform by the given matrix and store in this object.
	 * \param _mTrans The matrix used to transform _aabb2Other into this bounding box.
	 */
	void LSE_CALL CAabb2d::ComputeAabbFromAabbAndMatrix( const CAabb2d &_aabb2Other, const CMatrix4x4 &_mTrans ) {
		// For each axis.
		for ( uint32_t I = 0; I < 2; ++I ) {
			// Add the translation.
			m_vMin[I] = m_vMax[I] = _mTrans( 3, I );

			// Create extents by summing smaller and larger terms on this axis only.
			for ( uint32_t J = 0; J < 2; ++J ) {
				LSREAL fE = _mTrans( J, I ) * _aabb2Other.m_vMin[J];
				LSREAL fF = _mTrans( J, I ) * _aabb2Other.m_vMax[J];

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
	 * \param _aabb2Other The bounding box to fully encompass by this bounding box.
	 */
	void LSE_CALL CAabb2d::Encompass( const CAabb2d &_aabb2Other ) {
		m_vMin.x = CStd::Min( m_vMin.x, _aabb2Other.m_vMin.x );
		m_vMax.x = CStd::Max( m_vMax.x, _aabb2Other.m_vMax.x );

		m_vMin.y = CStd::Min( m_vMin.y, _aabb2Other.m_vMin.y );
		m_vMax.y = CStd::Max( m_vMax.y, _aabb2Other.m_vMax.y );
	}

	/**
	 * Change the size of the AABB to encompass a point.
	 *
	 * \param _vOther The point to fully encompass by this bounding box.
	 */
	void LSE_CALL CAabb2d::Encompass( const CVector2 &_vOther ) {
		m_vMin.x = CStd::Min( m_vMin.x, _vOther.x );
		m_vMax.x = CStd::Max( m_vMax.x, _vOther.x );

		m_vMin.y = CStd::Min( m_vMin.y, _vOther.y );
		m_vMax.y = CStd::Max( m_vMax.y, _vOther.y );
	}

	/**
	 * Fill an array of 4 vectors with the vertices of the bounding box represented by this object.
	 *
	 * \param _vPoints An array of at least 4 vectors to be filled with the points at the corners of
	 *	this bounding box.
	 */
	void LSE_CALL CAabb2d::GetBox( CVector2 _vPoints[4] ) const {
		_vPoints[LSP_A2C_UPPER_LEFT] = CVector2( m_vMin.x, m_vMax.y );
		_vPoints[LSP_A2C_UPPER_RIGHT] = CVector2( m_vMax.x, m_vMax.y );
		_vPoints[LSP_A2C_LOWER_LEFT] = CVector2( m_vMin.x, m_vMin.y );
		_vPoints[LSP_A2C_LOWER_RIGHT] = CVector2( m_vMax.x, m_vMin.y );
	}

	/**
	 * Gets the center of the bounding box.
	 *
	 * \return Returns a point marking the center of this bounding box.
	 */
	CVector2 LSE_CALL CAabb2d::GetCenter() const {
		return (m_vMin + m_vMax) * LSM_HALF;
	}

	/**
	 * Gets its area.
	 *
	 * \return Returns the area of this bounding box.
	 */
	LSREAL LSE_CALL CAabb2d::GetArea() const {
		return GetSizeX() * GetSizeY();
	}

	/**
	 * Gets the closest point on this AABB to a given point.
	 *
	 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
	 * \return Returns the closest point on this box from the given point.
	 */
	CVector2 LSE_CALL CAabb2d::ClosestPointToPoint( const CVector2 &_vPoint ) const {
		CVector2 vRet;
		LSREAL * pfRet = reinterpret_cast<LSREAL *>(&vRet);
		// For each axis, if the point is outside the box, clamp
		//	it to the bounds of the box.  Otherwise keep it as it is.
		for ( uint32_t I = 2; I--; ) {
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
	LSREAL LSE_CALL CAabb2d::GetSqDistFromPoint( const CVector2 &_vPoint ) const {
		LSREAL fDist = LSM_ZERO;

		// We do not need to get the actual position of the closest point.  It is sufficient
		//	enough to simply check if it extends beyond any axes and for each apply the correct
		//	squared distance.
		for ( uint32_t I = 0; I < 2; ++I ) {
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

}	// namespace lsp
