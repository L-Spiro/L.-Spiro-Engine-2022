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


#ifndef __LSP_TEST_H__
#define __LSP_TEST_H__

#include "../LSPPhysicsLib.h"
#include "../BoundingBoxes/LSPAabb.h"
#include "../BoundingBoxes/LSPAabb2d.h"
#include "../BoundingBoxes/LSPCappedKDop.h"
#include "../BoundingBoxes/LSPKDop.h"
#include "Frustum/LSMFrustum.h"
#include "LSPClassify.h"
#include "Plane/LSMPlane3.h"

namespace lsp {

	/**
	 * Class CTest
	 * \brief Provides functions for testing if primitives overlap or intersect.
	 *
	 * Description: Provides functions for testing if primitives overlap or intersect.  As it only performs a boolean
	 *	test, these functions are faster than the CIntersect and CCollide functions.
	 */
	class CTest {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Determines if the given AABB is partially or entirely inside the frustum.
		 *
		 * \param _aAabb The AABB to check for being partially or fully inside the frustum.
		 * \param _fFrustum The frustum.
		 * \return Returns true if the AABB is fully or partially inside the given frustum.
		 */
		static LSE_INLINE LSBOOL LSE_CALL						AabbFrustum( const CAabb &_aAabb, const CFrustum &_fFrustum );

		/**
		 * Determines if the given AABB is partially or entirely inside the frustum.
		 *
		 * \param _aAabb The AABB to check for being partially or fully inside the frustum.
		 * \param _fFrustum The frustum.
		 * \param _bFully Indicates whether the AABB is fully inside the frustum or not.
		 * \return Returns true if the AABB is fully or partially inside the given frustum.
		 */
		static LSBOOL LSE_CALL									AabbFrustum( const CAabb &_aAabb, const CFrustum &_fFrustum, LSBOOL &_bFully );

		/**
		 * Determines if the given AABB is partially or entirely inside the k-DOP.
		 *
		 * \param _aabbBox The axis-aligned bounding box to test for being contained by the given k-DOP.
		 * \param _kdDop The k-DOP to test for containment.
		 * \return Returns true if the AABB is partially or entirely inside the k-DOP.
		 */
		template <unsigned _uMax>
		static LSE_INLINE LSBOOL LSE_CALL						AabbKDop( const CAabb &_aAabb, const CCappedKDop<_uMax> &_ckdDop );

		/**
		 * Determines if the given AABB is partially or entirely inside the k-DOP.
		 *
		 * \param _aabbBox The axis-aligned bounding box to test for being contained by the given k-DOP.
		 * \param _kdDop The k-DOP to test for containment.
		 * \param _bFully Indicates whether the AABB is fully inside the frustum or not.
		 * \return Returns true if the AABB is partially or entirely inside the k-DOP.
		 */
		template <unsigned _uMax>
		static LSE_INLINE LSBOOL LSE_CALL						AabbKDop( const CAabb &_aAabb, const CCappedKDop<_uMax> &_ckdDop, LSBOOL &_bFully );

		/**
		 * Determines if the given 3D AABB's overlap in any way.
		 *
		 * \param _aLeft The left 3D axis-aligned bounding box.
		 * \param _aRight The right 3D axis-aligned bounding box.
		 * \return Returns true if the boxes overlap in any way.
		 */
		static LSE_INLINE LSBOOL LSE_CALL						AabbAabb( const CAabb &_aLeft, const CAabb &_aRight );

		/**
		 * Determines if the given 2D AABB's overlap in any way.
		 *
		 * \param _a2Left The left 2D axis-aligned bounding box.
		 * \param _a2Right The right 2D axis-aligned bounding box.
		 * \return Returns true if the boxes overlap in any way.
		 */
		static LSE_INLINE LSBOOL LSE_CALL						Aabb2dAabb2d( const CAabb2d &_a2Left, const CAabb2d &_a2Right );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Determines if the given AABB is partially or entirely inside the frustum.
	 *
	 * \param _aAabb The AABB to check for being partially or fully inside the frustum.
	 * \param _fFrustum The frustum.
	 * \return Returns true if the AABB is fully or partially inside the given frustum.
	 */
	LSE_INLINE LSBOOL LSE_CALL CTest::AabbFrustum( const CAabb &_aAabb, const CFrustum &_fFrustum ) {
		if ( (CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_LEFT] ) == LSM_PI_BACK)
			|| (CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_RIGHT] ) == LSM_PI_BACK)
			|| (CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_TOP] ) == LSM_PI_BACK)
			|| (CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_BOTTOM] ) == LSM_PI_BACK)
			|| (CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_NEAR] ) == LSM_PI_BACK)
			|| (CClassify::Aabb( _aAabb, _fFrustum[LSM_FP_FAR] ) == LSM_PI_BACK) ) {
			return false;
		}
		return true;
	}

	/**
	 * Determines if the given AABB is partially or entirely inside the k-DOP.
	 *
	 * \param _aabbBox The axis-aligned bounding box to test for being contained by the given k-DOP.
	 * \param _kdDop The k-DOP to test for containment.
	 * \return Returns true if the AABB is partially or entirely inside the k-DOP.
	 */
	template <unsigned _uMax>
	LSE_INLINE LSBOOL LSE_CALL CTest::AabbKDop( const CAabb &_aAabb, const CCappedKDop<_uMax> &_ckdDop ) {
		for ( uint32_t I = _ckdDop.TotalPlanes(); I--; ) {
			if ( CClassify::Aabb( _aAabb, _ckdDop[I] ) == LSM_PI_BACK ) { return false; }
		}
		return true;
	}

	/**
	 * Determines if the given AABB is partially or entirely inside the k-DOP.
	 *
	 * \param _aabbBox The axis-aligned bounding box to test for being contained by the given k-DOP.
	 * \param _kdDop The k-DOP to test for containment.
	 * \param _bFully Indicates whether the AABB is fully inside the frustum or not.
	 * \return Returns true if the AABB is partially or entirely inside the k-DOP.
	 */
	template <unsigned _uMax>
	LSE_INLINE LSBOOL LSE_CALL CTest::AabbKDop( const CAabb &_aAabb, const CCappedKDop<_uMax> &_ckdDop, LSBOOL &_bFully ) {
		_bFully = true;
		for ( uint32_t I = _ckdDop.TotalPlanes(); I--; ) {
			LSM_PLANE_INTERSECT piThis = CClassify::Aabb( _aAabb, _ckdDop[I] );
			if ( piThis == LSM_PI_BACK ) { return false; }
			if ( piThis != LSM_PI_FRONT ) { _bFully = false; }
		}
		return true;
	}

	/**
	 * Determines if the given 3D AABB's overlap in any way.
	 *
	 * \param _aLeft The left 3D axis-aligned bounding box.
	 * \param _aRight The right 3D axis-aligned bounding box.
	 * \return Returns true if the boxes overlap in any way.
	 */
	LSE_INLINE LSBOOL LSE_CALL CTest::AabbAabb( const CAabb &_aLeft, const CAabb &_aRight ) {
		return (_aLeft.m_vMin.x <= _aRight.m_vMax.x) && (_aLeft.m_vMin.y <= _aRight.m_vMax.y) && (_aLeft.m_vMin.z <= _aRight.m_vMax.z) &&
			(_aLeft.m_vMax.x >= _aRight.m_vMin.x) && (_aLeft.m_vMax.y >= _aRight.m_vMin.y) && (_aLeft.m_vMax.z >= _aRight.m_vMin.z);
	}

	/**
	 * Determines if the given 2D AABB's overlap in any way.
	 *
	 * \param _a2Left The left 2D axis-aligned bounding box.
	 * \param _a2Right The right 2D axis-aligned bounding box.
	 * \return Returns true if the boxes overlap in any way.
	 */
	LSE_INLINE LSBOOL LSE_CALL CTest::Aabb2dAabb2d( const CAabb2d &_a2Left, const CAabb2d &_a2Right ) {
		return (_a2Left.m_vMin.x <= _a2Right.m_vMax.x) && (_a2Left.m_vMin.y <= _a2Right.m_vMax.y) &&
			(_a2Left.m_vMax.x >= _a2Right.m_vMin.x) && (_a2Left.m_vMax.y >= _a2Right.m_vMin.y);
	}

}	// namespace lsp

#endif	// __LSP_TEST_H__
