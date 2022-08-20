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
 * Description: A template for any type of 3D plane.
 */


#ifndef __LSE_PLANE3BASE_H__
#define __LSE_PLANE3BASE_H__

#include "../LSMMathLib.h"

namespace lsm {

	// == Enumerations.
	/**
	 * Intersection.
	 */
	enum LSM_PLANE_INTERSECT {
		LSM_PI_FRONT,
		LSM_PI_BACK,
		LSM_PI_INTERSECT,
		LSM_PI_STRADDLING,
		LSM_PI_COPLANAR,
	};

	/**
	 * Class CPlane3BaseBase
	 * \brief A template for any type of 3D plane.
	 *
	 * Description: A template for any type of 3D plane.
	 */
	template <typename _tType, typename _tVec3Type>
	class CPlane3Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CPlane3Base() {
		}
		LSE_INLINE LSE_CALLCTOR				CPlane3Base( const CPlane3Base<_tType, _tVec3Type> &_pPlane ) {
			(*this) = _pPlane;
		}
		LSE_INLINE LSE_CALLCTOR				CPlane3Base( const _tVec3Type &_vPoint0, const _tVec3Type &_vPoint1, const _tVec3Type &_vPoint2 ) {
			n = (_vPoint1 - _vPoint0) % (_vPoint2 - _vPoint0);
			n.Normalize();
			dist = n.Dot( _vPoint0 );
		}
		LSE_INLINE LSE_CALLCTOR				CPlane3Base( const _tVec3Type &_vNormal, _tType _fDistance ) {
			n = _vNormal;
			dist = _fDistance;
		}
		LSE_INLINE LSE_CALLCTOR				CPlane3Base( const _tVec3Type &_vPoint, const _tVec3Type &_vNormal ) {
			n = _vNormal;
			dist = n.Dot( _vPoint );
		}


		// == Operators.
		/**
		 * Copy the given plane into this one.
		 *
		 * \param _pPlane The plane to copy.
		 * \return Returns this plane after the copy.
		 */
		CPlane3Base<_tType, _tVec3Type> & LSE_FCALL
											operator = ( const CPlane3Base<_tType, _tVec3Type> &_pPlane ) {
			n = _pPlane.n;
			dist = _pPlane.dist;
			return (*this);
		}

		/**
		 * Equality operator.
		 *
		 * \param _pOther The plane against which to check for equality.
		 * \return Returns true if the planes are exactly equal.
		 */
		LSBOOL LSE_CALL						operator == ( const CPlane3Base<_tType, _tVec3Type> &_pOther ) const {
			return dist == _pOther.dist && n == _pOther.n;
		}

		/**
		 * Negation operaor.
		 *
		 * \return Returns the negated form of this plane.
		 */
		CPlane3Base<_tType, _tVec3Type> LSE_CALL
											operator - () const {
			return CPlane3Base<_tType, _tVec3Type>( -n, -dist );
		}


		// == Functions.
		/**
		 * Gets the distance from this plane to a point.  This plane must be normalized.
		 *
		 * \param _vPoint Point from which to get the distance to this plane.
		 * \return Returns the distance from the point to this plane, signed.
		 */
		LSE_INLINE _tType LSE_FCALL			SignedDistance( const _tVec3Type &_vPoint ) const {
			return n.Dot( _vPoint ) - dist;
		}

		/**
		 * Gets the distance from this plane to a point.  This plane does not need to be normalized.
		 *
		 * \param _vPoint Point from which to get the distance to this plane.
		 * \return Returns the distance from the point to this plane, signed.
		 */
		LSE_INLINE _tType LSE_FCALL			SignedDistanceNonNormal( const _tVec3Type &_vPoint ) const {
			return (n.Dot( _vPoint ) - dist) / n.LenSq();
		}

		/**
		 * Project the given point into this plane.  This plane must be normalized.
		 *
		 * \param _vPoint The point to project onto this plane.
		 * \return Returns the point projected onto this plane.
		 */
		LSE_INLINE _tVec3Type LSE_FCALL		ProjectPointToPlane( const _tVec3Type &_vPoint ) const {
			_tType fDist = -SignedDistance( _vPoint );
			return _tVec3Type( (n.x * fDist) + _vPoint.x,
				(n.y * fDist) + _vPoint.y,
				(n.z * fDist) + _vPoint.z );
		}

		/**
		 * Returns the closest point on this plane to the given point.  This plane must be normalized.
		 *
		 * \param _vPoint The point to use as a reference for determining the point on this plane to return.
		 * \return Returns the closest point on this plane to the given point.  This plane must be normalized.
		 */
		LSE_INLINE _tVec3Type LSE_FCALL		ClosestPointOnPlane( const _tVec3Type &_vPoint ) const {
			_tType fT = n.Dot( _vPoint ) - dist;
			return _vPoint - (n * fT);
		}

		/**
		 * Returns the closest point on this plane to the given point.  This plane does not need to be normalized.
		 *
		 * \param _vPoint The point to use as a reference for determining the point on this plane to return.
		 * \return Returns the closest point on this plane to the given point.  This plane must be normalized.
		 */
		LSE_INLINE _tVec3Type LSE_FCALL		ClosestPointOnNonNormalPlane( const _tVec3Type &_vPoint ) const {
			_tType fT = (n.Dot( _vPoint ) - dist) / n.LenSq();
			return _vPoint - (n * fT);
		}

		/**
		 * Normalize this plane.
		 */
		LSE_INLINE void LSE_FCALL			Normalize() {
			_tType fInvMag = static_cast<_tType>(1.0) / n.Len();

			n.x *= fInvMag;
			n.y *= fInvMag;
			n.z *= fInvMag;
			dist *= fInvMag;
		}

		/**
		 * Gets the cosine of the angle between 2 intersecting planes.  If the planes do not intersect,
		 *	false is returned.
		 *
		 * \param _pbLeft The left plane.
		 * \param _pbRight The right plane.
		 * \param _tRet Holds the returned value if the result is true.
		 * \param _tEpsilon Epsilon to use for the test.
		 * \return Returns true if the planes intersect.
		 */
		static LSBOOL LSE_CALL				PlanesFormConvexEdge( const CPlane3Base<_tType, _tVec3Type> &_pbLeft, const CPlane3Base<_tType, _tVec3Type> &_pbRight,
			_tType _tEpsilon ) {
			// Get the direction of the intersecting line.
			_tVec3Type vDir = _pbLeft.n % _pbRight.n;

			_tType tDenom = vDir * vDir;
			if ( tDenom < _tEpsilon ) { return false; }	// Not forming an edge at all.

			vDir.Normalize();
			_tVec3Type vDirFromLefToRight = vDir % _pbLeft.n;
			_tType tInDirFromLeft = vDirFromLefToRight * _pbRight.n;
			
			return tInDirFromLeft < _tType( 0 );
		}

		/**
		 * Creates a Newell Plane given 2 pools of points.
		 *
		 * \param _ptLeft The points in the left pool.
		 * \param _ui32LeftTotal Total points to which _ptLeft points.
		 * \param _ptRight The points in the right pool.
		 * \param _ui32RightTotal Total points to which _ptRight points.
		 * \return Returns the Newell Plane that matches the given sets of points.
		 */
		static CPlane3Base<_tType, _tVec3Type> LSE_CALL
											NewellPlane( const _tVec3Type * _ptLeft, uint32_t _ui32LeftTotal,
			const _tVec3Type * _ptRight, uint32_t _ui32RightTotal ) {
			_tVec3Type tCenter( _tType( 0.0 ), _tType( 0.0 ), _tType( 0.0 ) );
			_tVec3Type tNormal( _tType( 0.0 ), _tType( 0.0 ), _tType( 0.0 ) );

			for ( uint32_t I = _ui32LeftTotal - 1, J = 0;
				J < _ui32LeftTotal;
				I = J++ ) {
				tNormal.x += (_ptLeft[I].y - _ptLeft[J].y) * (_ptLeft[I].z + _ptLeft[J].z);
				tNormal.y += (_ptLeft[I].z - _ptLeft[J].z) * (_ptLeft[I].x + _ptLeft[J].x);
				tNormal.z += (_ptLeft[I].x - _ptLeft[J].x) * (_ptLeft[I].y + _ptLeft[J].y);
				tCenter += _ptLeft[J];
			}

			for ( uint32_t I = _ui32RightTotal - 1, J = 0;
				J < _ui32RightTotal;
				I = J++ ) {
				tNormal.x += (_ptRight[I].y - _ptRight[J].y) * (_ptRight[I].z + _ptRight[J].z);
				tNormal.y += (_ptRight[I].z - _ptRight[J].z) * (_ptRight[I].x + _ptRight[J].x);
				tNormal.z += (_ptRight[I].x - _ptRight[J].x) * (_ptRight[I].y + _ptRight[J].y);
				tCenter += _ptRight[J];
			}

			tNormal.Normalize();
			return CPlane3Base<_tType, _tVec3Type>( tNormal, tCenter.Dot( tNormal ) / (_ui32LeftTotal + _ui32RightTotal) );
		}


		// == Members.
		/** Plane normal. */
		_tVec3Type							n;

		/** Plane distance, signed. */
		_tType								dist;

	};

}	// namespace lsm

#endif	// __LSE_PLANE3BASE_H__
