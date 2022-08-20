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
 * Description: Handles classifications of primitives.  Specifically, it classifies primitives in relation
 *	with planes.
 */


#ifndef __LSP_CLASSIFY_H__
#define __LSP_CLASSIFY_H__

#include "../LSPPhysicsLib.h"
#include "../BoundingBoxes/LSPAabb.h"
#include "Plane/LSMPlane3.h"

namespace lsp {

	/**
	 * Class CClassify
	 * \brief Handles classifications of primitives.
	 *
	 * Description: Handles classifications of primitives.  Specifically, it classifies primitives in relation
	 *	with planes.
	 */
	class CClassify {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Classifies an axis-aligned bounding box in relation to a given plane.
		 *
		 * \param _aAabb The AABB to test against the given plane.
		 * \param _pPlane The plane to test against the given AABB.
		 * \return Returns a plane classification.
		 */
		static LSE_INLINE LSM_PLANE_INTERSECT LSE_CALL				Aabb( const CAabb &_aAabb, const CPlane3 &_pPlane );

		/**
		 * Classifies a point in relation to a given plane.
		 *
		 * \param _vPoint The point to test against the given plane.
		 * \param _pPlane The plane to test against the given point.
		 * \param _fThickness The thickness of the plane.
		 * \return Returns a plane classification.
		 */
		template <typename _tType, typename _tVec3Type, typename _tPlane3Type>
		static LSE_INLINE LSM_PLANE_INTERSECT LSE_CALL				Point( const _tVec3Type &_vPoint, const _tPlane3Type &_pPlane, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			_tType fDist = (_pPlane.n * _vPoint) - _pPlane.dist;
			if ( fDist > _fThickness ) { return LSM_PI_FRONT; }
			if ( fDist < -_fThickness ) { return LSM_PI_BACK; }
			return LSM_PI_COPLANAR;
		}

		/**
		 * Classifies a polygon in relation to a given plane.
		 *
		 * \param _pPoly The polygon to test against the given plane.
		 * \param _pPlane The plane to test against the given point.
		 * \param _fThickness The thickness of the plane.
		 * \return Returns a plane classification.
		 */
		template <typename _tType, typename _tPoly3Type, typename _tPlane3Type>
		static LSE_INLINE LSM_PLANE_INTERSECT LSE_CALL				Polygon( const _tPoly3Type &_pPoly, const _tPlane3Type &_pPlane, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			uint32_t ui32Front = 0, ui32Back = 0;

			// Easy as pie.  Loop over vertices and determine if all are on one side or not.
			for ( uint32_t I = _pPoly.TotalSegments(); I--; ) {
				switch ( Point<_tType>( _pPoly[I]->p, _pPlane, _fThickness ) ) {
					case LSM_PI_FRONT : {
						// This one is in front.  If any are in back then we are straddling.
						if ( ui32Back ) { return LSM_PI_INTERSECT; }
						++ui32Front;
						break;
					}
					case LSM_PI_BACK : {
						// This one is in back.  If any are in front then we are straddling.
						if ( ui32Front ) { return LSM_PI_INTERSECT; }
						++ui32Back;
						break;
					}
				}
			}
			// We are not straddling.  Then we must be fully in front, fully behind, or coplanar.
			if ( ui32Front ) { return LSM_PI_FRONT; }
			return ui32Back ? LSM_PI_BACK : LSM_PI_COPLANAR;
		}

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Classifies an axis-aligned bounding box in relation to a given plane.
	 *
	 * \param _aAabb The AABB to test against the given plane.
	 * \param _pPlane The plane to test against the given AABB.
	 * \return Returns a plane classification.
	 */
	LSE_INLINE LSM_PLANE_INTERSECT LSE_CALL CClassify::Aabb( const CAabb &_aAabb, const CPlane3 &_pPlane ) {
		// Center of the AABB.
		CVector3 vC = (_aAabb.m_vMax + _aAabb.m_vMin) * LSM_HALF;
		// Positive extents.
		CVector3 vE = _aAabb.m_vMax - vC;

		// Computes the projected interval radius of _aAabb onto L(t) = _aAabb.c + t * _pPlane.n.
		LSREAL fR = vE[0] * CMathLib::Abs( _pPlane.n[0] ) +
			vE[1] * CMathLib::Abs( _pPlane.n[1] ) +
			vE[2] * CMathLib::Abs( _pPlane.n[2] );

		// Distance from box center to plane.
		LSREAL fS = _pPlane.n.Dot( vC ) - _pPlane.dist;

		// If less than R, return overlap.
		if ( CMathLib::Abs( fS ) <= fR ) { return LSM_PI_INTERSECT; }
		// Otherwise it is in front or back of the plane.
		return fS > fR ? LSM_PI_FRONT : LSM_PI_BACK;
	}

}	// namespace lsp

#endif	// __LSP_CLASSIFY_H__
