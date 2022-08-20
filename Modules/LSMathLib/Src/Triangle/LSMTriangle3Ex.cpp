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
 * Description: An extended 3D triangle with precomputed barycentric weights and a normal.
 */

#include "LSMTriangle3Ex.h"

namespace lsm {

	// == Functions.
	/**
	 * Gets the barycentric coordinates of a given point.
	 *
	 * \param _vPoint The point whose barycentric weights are to be obtained.
	 * \param _fU Barycentric U coordinate.
	 * \param _fV Barycentric V coordinate.
	 * \param _fW Barycentric W coordinate.
	 */
	void LSE_FCALL CTriangle3Ex::Barycentric( const CVector3 &_vPoint,
		LSREAL &_fU, LSREAL &_fV, LSREAL &_fW ) const {
		CVector3 v0 = b - a;
		CVector3 v1 = c - a;
		CVector3 v2 = _vPoint - a;
		LSREAL fD20 = v2.Dot( v0 );
		LSREAL fD21 = v2.Dot( v1 );
		_fV = (m_fD11 * fD20 - m_fD01 * fD21) / m_fDenom;
		_fW = (m_fD00 * fD21 - m_fD01 * fD20) / m_fDenom;
		_fU = LSM_ONE - _fV - _fW;
	}

	/**
	 * Update the data.
	 */
	void LSE_FCALL CTriangle3Ex::Update() {
		// Barycentric terms.
		CVector3 v0 = b - a;
		CVector3 v1 = c - a;
		m_fD00 = v0.Dot( v0 );
		m_fD01 = v0.Dot( v1 );
		m_fD11 = v1.Dot( v1 );
		m_fDenom = HP_TO_LSREAL( LSREAL_TO_HP( m_fD00 ) * LSREAL_TO_HP( m_fD11 ) - LSREAL_TO_HP( m_fD01 ) * LSREAL_TO_HP( m_fD01 ));
		// Normal.
		m_vNormal = v0 % v1;
		m_vNormal.Normalize();
	}


}	// namespace lsm
