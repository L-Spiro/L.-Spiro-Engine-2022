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
 * Description: A frustum is formed of 6 planes representing the near, far, left, right, top, and bottom
 *	of a 3D box.  It is usually used with cameras to represent the veiwable area of the camera and can be
 *	tested against various types of primites (cylinders, spheres, AABB's, etc.) to determine objects that
 *	are partially or entirely inside the box.  This helps determine which objects should be drawn.
 */


#ifndef __LSM_FRUSTUM_H__
#define __LSM_FRUSTUM_H__

#include "../LSMMathLib.h"
#include "../Matrix/LSMMatrix4x4.h"
#include "../Plane/LSMPlane3.h"

namespace lsm {

	// == Enumerations.
	/**
	 * Frustum planes.
	 */
	enum LSM_FRUSTUM_PLANES {
		LSM_FP_LEFT,
		LSM_FP_RIGHT,
		LSM_FP_TOP,
		LSM_FP_BOTTOM,
		LSM_FP_NEAR,
		LSM_FP_FAR,
		LSM_FP_TOTAL
	};

	/**
	 * Frustum points.
	 */
	enum LSM_FRUSTUM_POINTS {
		LSM_FP_FAR_BOTTOM_LEFT,
		LSM_FP_FAR_TOP_LEFT,
		LSM_FP_FAR_TOP_RIGHT,
		LSM_FP_FAR_BOTTOM_RIGHT,
		LSM_FP_NEAR_BOTTOM_LEFT,
		LSM_FP_NEAR_TOP_LEFT,
		LSM_FP_NEAR_TOP_RIGHT,
		LSM_FP_NEAR_BOTTOM_RIGHT,
	};


	/**
	 * Class CFrustum
	 * \brief A frustum.
	 *
	 * Description: A frustum is formed of 6 planes representing the near, far, left, right, top, and bottom
	 *	of a 3D box.  It is usually used with cameras to represent the veiwable area of the camera and can be
	 *	tested against various types of primites (cylinders, spheres, AABB's, etc.) to determine objects that
	 *	are partially or entirely inside the box.  This helps determine which objects should be drawn.
	 */
	class CFrustum {
		// All is public.  This class has no secrets.
	public :
		// == Operators.
		/**
		 * Provide array access.  For the sake of speed, the index is not checked.
		 *
		 * \param _ui32Index Index of the plane to obtain.
		 * \return Returns the plain at the given index, which should be one of the
		 *	LSM_FRUSTUM_PLANES enumerated values.
		 */
		const CPlane3 & LSE_FCALL	operator [] ( uint32_t _ui32Index ) const;


		// == Functions.
		/**
		 * Determine of the given point is entirely inside the frustum or not.
		 *
		 * \param _vPoint The point to test.
		 * \return Returns true if the point is inside the box described by the 6 planes of
		 *	this frustum.
		 */
		LSE_INLINE LSBOOL LSE_FCALL	TestPoint( const CVector3 &_vPoint ) const;

		/**
		 * Given two planes, returns the two points shared by those planes.  The points are always
		 *	returned in counter-clockwise order, assuming the first input plane is facing towards
		 *	the viewer.
		 *
		 * \param _fpPlane0 The plane facing towards the viewer.
		 * \param _fpPlane1 A plane neighboring _fpPlane0.
		 * \param _fpRet An array of exactly two elements to be filled with the indices of the points
		 *	on return.
		 */
		static void LSE_CALL		GetCornersOfPlanes( LSM_FRUSTUM_PLANES _fpPlane0, LSM_FRUSTUM_PLANES _fpPlane1,
			LSM_FRUSTUM_POINTS _fpRet[2] );

		/**
		 * Given a plane, the 4 neighbors of that plane are returned.
		 *
		 * \param _fpPlane The plane for which to locate neighbors.
		 * \param _fpRet Holds the returned neighbors.  Must be at least 4 elements in the array.
		 */
		static void LSE_CALL		GetNeighbors( LSM_FRUSTUM_PLANES _fpPlane, LSM_FRUSTUM_PLANES _fpRet[4] );


		// == Members.
#pragma pack( push, 1 )
		/** Left plane. */
		CPlane3						m_pLeftPlane;

		/** Right plane. */
		CPlane3						m_pRightPlane;

		/** Top plane. */
		CPlane3						m_pTopPlane;

		/** Bottom plane. */
		CPlane3						m_pBottomPlane;

		/** Near plane. */
		CPlane3						m_pNearPlane;

		/** Far plane. */
		CPlane3						m_pFarPlane;
#pragma pack( pop )


	protected :
		// == Operators.
		/**
		 * Provide array access.  For the sake of speed, the index is not checked.
		 *
		 * \param _ui32Index Index of the plane to obtain.
		 * \return Returns the plain at the given index, which should be one of the
		 *	LSM_FRUSTUM_PLANES enumerated values.
		 */
		CPlane3 & LSE_FCALL			operator [] ( uint32_t _ui32Index );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * Provide array access.  For the sake of speed, the index is not checked.
	 *
	 * \param _ui32Index Index of the plane to obtain.
	 * \return Returns the plain at the given index, which should be one of the
	 *	LSM_FRUSTUM_PLANES enumerated values.
	 */
	LSE_INLINE const CPlane3 & LSE_FCALL CFrustum::operator [] ( uint32_t _ui32Index ) const {
		return reinterpret_cast<const CPlane3 *>(this)[_ui32Index];
	}

	// == Functions.
	/**
	 * Determine of the given point is entirely inside the frustum or not.
	 *
	 * \param _vPoint The point to test.
	 * \return Returns true if the point is inside the box described by the 6 planes of
	 *	this frustum.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CFrustum::TestPoint( const CVector3 &_vPoint ) const {
		if ( (m_pLeftPlane.SignedDistance( _vPoint ) < LSM_ZERO)
			|| (m_pRightPlane.SignedDistance( _vPoint ) < LSM_ZERO)
			|| (m_pTopPlane.SignedDistance( _vPoint ) < LSM_ZERO)
			|| (m_pBottomPlane.SignedDistance( _vPoint ) < LSM_ZERO)
			|| (m_pNearPlane.SignedDistance( _vPoint ) < LSM_ZERO)
			|| (m_pFarPlane.SignedDistance( _vPoint ) < LSM_ZERO) ) {
			return false;
		}
		return true;
	}

	// == Operators.
	/**
	 * Provide array access.  For the sake of speed, the index is not checked.
	 *
	 * \param _ui32Index Index of the plane to obtain.
	 * \return Returns the plain at the given index, which should be one of the
	 *	LSM_FRUSTUM_PLANES enumerated values.
	 */
	LSE_INLINE CPlane3 & LSE_FCALL CFrustum::operator [] ( uint32_t _ui32Index ) {
		return reinterpret_cast<CPlane3 *>(this)[_ui32Index];
	}

}	// namespace lsm

#endif	// __LSM_FRUSTUM_H__
