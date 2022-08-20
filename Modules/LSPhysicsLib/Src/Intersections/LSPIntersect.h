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
 * Description: Performs intersection tests between various primitive types.
 */


#ifndef __LSP_INTERSECT_H__
#define __LSP_INTERSECT_H__

#include "../LSPPhysicsLib.h"
#include "Plane/LSMPlane3.h"
#include "Triangle/LSMTriangle3.h"

// == Forward Declarations.
namespace lsm {
	class CRay3;
	class CTriangle3Ex;
}	// namespace lsm

namespace lsp {

	/**
	 * Class CIntersect
	 * \brief Performs intersection tests between various primitive types.
	 *
	 * Description: Performs intersection tests between various primitive types.
	 */
	class CIntersect {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Gets the intersection point of 3 planes if there is one.  If not, false is returned.
		 *
		 * \param _pP0 Plane 1.
		 * \param _pP1 Plane 2.
		 * \param _pP2 Plane 3.
		 * \param _vRet The returned intersection point, valid only if true is returned.
		 * \return Returns true if the 3 planes intersect at a single point.
		 */
		template <typename _tType, typename _tPlaneType, typename _tVecType>
		static LSE_INLINE LSBOOL LSE_CALL						ThreePlanes( const _tPlaneType &_pP0, const _tPlaneType &_pP1, const _tPlaneType &_pP2, _tVecType &_vRet ) {
			_tVecType vU = _pP1.n % _pP2.n;

			_tType fDenom = _pP0.n * vU;
			if ( CMathLib::AbsT( fDenom ) <= static_cast<_tType>(LSM_REAL_EPSILON) ) { return false; }

			_vRet = (vU * _pP0.dist + _pP0.n.Cross( _pP1.n * _pP2.dist - _pP2.n * _pP1.dist )) / fDenom;

			return true;
		}

		/**
		 * If the given planes intersect, the ray defining the intersection is returned.  Planes must be normalized.
		 *
		 * \param _pP0 Plane 1.
		 * \param _pP1 Plane 2.
		 * \param _rRay The return ray.
		 * \return Returns true if the 2 planes intersect.
		 */
		template <typename _tType, typename _tPlaneType, typename _tRayType>
		static LSE_INLINE LSBOOL LSE_CALL						TwoPlanes( const _tPlaneType &_pP0, const _tPlaneType &_pP1, _tRayType &_rRay ) {
			// Direction of the intersection.
			_rRay.dir = _pP0.n % _pP1.n;

			// If it is (near) 0, the planes are parallel (and separated) or coincident and not considered
			//	intersecting.
			if ( _rRay.dir.Dot( _rRay.dir ) < static_cast<_tType>(LSM_REAL_EPSILON) ) { return false; }

			// Finish the ray by computing a point on the intersection line.
			_rRay.p = ((_pP1.n * _pP0.dist) - (_pP0.n * _pP1.dist)) % _rRay.dir;
			return true;
		}

		/**
		 * If the given line segment intersects the given plane, true is returned and the point where the line segment intersects the plane
		 *	is returned.
		 *
		 * \param _vStart The starting point of the line segment to test for intersection against the given plane.
		 * \param _vEnd The end point of the line segment to test for intersection against the given plane.
		 * \param _pPlane The plane against which to test for intersection.
		 * \param _fT The returned intersection T value, if any.
		 * \param _vRet The returned intersection point, if any.
		 * \return Returns true if the line segment intersects the plane.
		 */
		template <typename _tType, typename _tVecType, typename _tPlaneType>
		static LSE_INLINE LSBOOL LSE_CALL						LineSegPlane( const _tVecType &_vStart, const _tVecType &_vEnd, const _tPlaneType &_pPlane, _tType &_fT, _tVecType &_vRet ) {
			_tVecType vAb = _vEnd - _vStart;
			_tType fDenom = _pPlane.n.Dot( vAb );
			if ( fDenom == static_cast<_tType>(0.0) ) {
				// Coplanar.
				return false;
			}
			_fT = (_pPlane.dist - _pPlane.n.Dot( _vStart )) / fDenom;
			if ( _fT >= static_cast<_tType>(0.0) && _fT <= static_cast<_tType>(1.0) ) {
				_vRet = _vStart + vAb * _fT;
				return true;
			}
			return false;
		}

		/**
		 * Assumes the line segment is known to intersect the given plane and returns the point of intersection.
		 *
		 * \param _vStart The starting point of the line segment to test for intersection against the given plane.
		 * \param _vEnd The end point of the line segment to test for intersection against the given plane.
		 * \param _pPlane The plane against which to test for intersection.
		 * \param _vRet The returned intersection point.
		 */
		template <typename _tType, typename _tVecType, typename _tPlaneType>
		static LSE_INLINE void LSE_CALL						LineSegPlaneFast( const _tVecType &_vStart, const _tVecType &_vEnd, const _tPlaneType &_pPlane, _tVecType &_vRet ) {
			_tVecType vAb = _vEnd - _vStart;
			_tType fT = (_pPlane.dist - _pPlane.n.Dot( _vStart )) / _pPlane.n.Dot( vAb );
			_vRet = _vStart + vAb * fT;
		}

		/**
		 * Returns true or false indicating intersection.  Upon intersection, the barycentric coordinates
		 *	of the intersection are also returned.
		 *
		 * \param _tTri The triangle to test for intersection.
		 * \param _rRay The ray to test against the given triangle for intersection.
		 * \param _fU The returned U barycentric coordinate.
		 * \param _fV The returned V barycentric coordinate.
		 * \param _fW The returned W barycentric coordinate.
		 * \param _fT Time of intersection along the ray.
		 * \return Returns true if the ray interesects the given triangle.  In the case of a true return, the barycentric coordinates are
		 *	also returned.
		 */
		static LSBOOL LSE_CALL									RayTriangle( const CTriangle3Ex &_tTri, const CRay3 &_rRay,
			LSREAL &_fU, LSREAL &_fV, LSREAL &_fW, LSREAL &_fT );

		/**
		 * Returns true or false indicating intersection.  Upon intersection, the barycentric coordinates
		 *	of the intersection are also returned.
		 *
		 * \param _tTri The triangle to test for intersection.
		 * \param _rRay The ray to test against the given triangle for intersection.
		 * \param _fU The returned U barycentric coordinate.
		 * \param _fV The returned V barycentric coordinate.
		 * \param _fW The returned W barycentric coordinate.
		 * \param _fT Time of intersection along the ray.
		 * \return Returns true if the ray interesects the given triangle.  In the case of a true return, the barycentric coordinates are
		 *	also returned.
		 */
		static LSBOOL LSE_CALL									RayTriangle( const CTriangle3 &_tTri, const CRay3 &_rRay,
			LSREAL &_fU, LSREAL &_fV, LSREAL &_fW, LSREAL &_fT );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsp

#endif	// __LSP_INTERSECT_H__
