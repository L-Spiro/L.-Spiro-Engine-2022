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
 * Description: Provides functions for testing if primitives overlap or intersect.  As it only performs a boolean
 *	test, these functions are faster than the CIntersect and CCollide functions.
 */

#include "LSPTest.h"


namespace lsp {

	// == Functions.
	/**
	 * Determines if the given AABB is partially or entirely inside the frustum.
	 *
	 * \param _aAabb The AABB to check for being partially or fully inside the frustum.
	 * \param _fFrustum The frustum.
	 * \param _bFully Indicates whether the AABB is fully inside the frustum or not.
	 * \return Returns true if the AABB is fully or partially inside the given frustum.
	 */
	LSBOOL LSE_CALL CTest::AabbFrustum( const CAabb &_aAabb, const CFrustum &_fFrustum, LSBOOL &_bFully ) {
		LSM_PLANE_INTERSECT piValue;
		_bFully = true;
		
		piValue = CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_LEFT] );
		if ( piValue == LSM_PI_BACK ) { return false; }
		if ( piValue != LSM_PI_FRONT ) { _bFully = false; }

		piValue = CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_RIGHT] );
		if ( piValue == LSM_PI_BACK ) { return false; }
		if ( piValue != LSM_PI_FRONT ) { _bFully = false; }

		piValue = CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_TOP] );
		if ( piValue == LSM_PI_BACK ) { return false; }
		if ( piValue != LSM_PI_FRONT ) { _bFully = false; }

		piValue = CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_BOTTOM] );
		if ( piValue == LSM_PI_BACK ) { return false; }
		if ( piValue != LSM_PI_FRONT ) { _bFully = false; }

		piValue = CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_NEAR] );
		if ( piValue == LSM_PI_BACK ) { return false; }
		if ( piValue != LSM_PI_FRONT ) { _bFully = false; }

		piValue = CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_FAR] );
		if ( piValue == LSM_PI_BACK ) { return false; }
		if ( piValue != LSM_PI_FRONT ) { _bFully = false; }
		return true;
	}

}	// namespace lsp
