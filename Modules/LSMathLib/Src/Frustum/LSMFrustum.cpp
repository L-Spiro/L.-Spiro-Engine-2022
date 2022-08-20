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

#include "LSMFrustum.h"


namespace lsm {

	// == Functions.
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
	void LSE_CALL CFrustum::GetCornersOfPlanes( LSM_FRUSTUM_PLANES _fpPlane0, LSM_FRUSTUM_PLANES _fpPlane1,
		LSM_FRUSTUM_POINTS _fpRet[2] ) {
		static const LSM_FRUSTUM_POINTS fpTable[LSM_FP_TOTAL][LSM_FP_TOTAL][2] = {
			{	// LSM_FP_LEFT
				{	// LSM_FP_LEFT
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,		// Invalid combination.
				},
				{	// LSM_FP_RIGHT
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,		// Invalid combination.
				},
				{	// LSM_FP_TOP
					LSM_FP_NEAR_TOP_LEFT, LSM_FP_FAR_TOP_LEFT,
				},
				{	// LSM_FP_BOTTOM
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,
				},
				{	// LSM_FP_NEAR
					LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_NEAR_TOP_LEFT,
				},
				{	// LSM_FP_FAR
					LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_BOTTOM_LEFT,
				},
			},
			{	// LSM_FP_RIGHT
				{	// LSM_FP_LEFT
					LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,		// Invalid combination.
				},
				{	// LSM_FP_RIGHT
					LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,		// Invalid combination.
				},
				{	// LSM_FP_TOP
					LSM_FP_FAR_TOP_RIGHT, LSM_FP_NEAR_TOP_RIGHT,
				},
				{	// LSM_FP_BOTTOM
					LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,
				},
				{	// LSM_FP_NEAR
					LSM_FP_NEAR_TOP_RIGHT, LSM_FP_NEAR_BOTTOM_RIGHT,
				},
				{	// LSM_FP_FAR
					LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_TOP_RIGHT,
				},
			},

			// ==

			{	// LSM_FP_TOP
				{	// LSM_FP_LEFT
					LSM_FP_FAR_TOP_LEFT, LSM_FP_NEAR_TOP_LEFT,
				},
				{	// LSM_FP_RIGHT
					LSM_FP_NEAR_TOP_RIGHT, LSM_FP_FAR_TOP_RIGHT,
				},
				{	// LSM_FP_TOP
					LSM_FP_NEAR_TOP_LEFT, LSM_FP_FAR_TOP_LEFT,			// Invalid combination.
				},
				{	// LSM_FP_BOTTOM
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,	// Invalid combination.
				},
				{	// LSM_FP_NEAR
					LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_TOP_RIGHT,
				},
				{	// LSM_FP_FAR
					LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,
				},
			},
			{	// LSM_FP_BOTTOM
				{	// LSM_FP_LEFT
					LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,
				},
				{	// LSM_FP_RIGHT
					LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_NEAR_BOTTOM_RIGHT,
				},
				{	// LSM_FP_TOP
					LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,	// Invalid combination.
				},
				{	// LSM_FP_BOTTOM
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,	// Invalid combination.
				},
				{	// LSM_FP_NEAR
					LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_NEAR_BOTTOM_LEFT,
				},
				{	// LSM_FP_FAR
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_RIGHT,
				},
			},

			// ==

			{	// LSM_FP_NEAR
				{	// LSM_FP_LEFT
					LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,
				},
				{	// LSM_FP_RIGHT
					LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_NEAR_TOP_RIGHT,
				},
				{	// LSM_FP_TOP
					LSM_FP_NEAR_TOP_RIGHT, LSM_FP_NEAR_TOP_LEFT,
				},
				{	// LSM_FP_BOTTOM
					LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_RIGHT,
				},
				{	// LSM_FP_NEAR
					LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_TOP_RIGHT,		// Invalid combination.
				},
				{	// LSM_FP_FAR
					LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,			// Invalid combination.
				},
			},
			{	// LSM_FP_FAR
				{	// LSM_FP_LEFT
					LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_TOP_LEFT,
				},
				{	// LSM_FP_RIGHT
					LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,
				},
				{	// LSM_FP_TOP
					LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_TOP_RIGHT,
				},
				{	// LSM_FP_BOTTOM
					LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_LEFT,
				},
				{	// LSM_FP_NEAR
					LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_TOP_RIGHT,			// Invalid combination.
				},
				{	// LSM_FP_FAR
					LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,			// Invalid combination.
				},
			},
		};
		_fpRet[0] = fpTable[_fpPlane0][_fpPlane1][0];
		_fpRet[1] = fpTable[_fpPlane0][_fpPlane1][1];
	}

	/**
	 * Given a plane, the 4 neighbors of that plane are returned.
	 *
	 * \param _fpPlane The plane for which to locate neighbors.
	 * \param _fpRet Holds the returned neighbors.  Must be at least 4 elements in the array.
	 */
	void LSE_CALL CFrustum::GetNeighbors( LSM_FRUSTUM_PLANES _fpPlane, LSM_FRUSTUM_PLANES _fpRet[4] ) {
		static const LSM_FRUSTUM_PLANES fpTable[LSM_FP_TOTAL][4] = {
			{	// LSM_FP_LEFT
				LSM_FP_TOP,
				LSM_FP_BOTTOM,
				LSM_FP_NEAR,
				LSM_FP_FAR
			},
			{	// LSM_FP_RIGHT
				LSM_FP_TOP,
				LSM_FP_BOTTOM,
				LSM_FP_NEAR,
				LSM_FP_FAR
			},
			{	// LSM_FP_TOP
				LSM_FP_LEFT,
				LSM_FP_RIGHT,
				LSM_FP_NEAR,
				LSM_FP_FAR
			},
			{	// LSM_FP_BOTTOM
				LSM_FP_LEFT,
				LSM_FP_RIGHT,
				LSM_FP_NEAR,
				LSM_FP_FAR
			},
			{	// LSM_FP_NEAR
				LSM_FP_LEFT,
				LSM_FP_RIGHT,
				LSM_FP_TOP,
				LSM_FP_BOTTOM
			},
			{	// LSM_FP_FAR
				LSM_FP_LEFT,
				LSM_FP_RIGHT,
				LSM_FP_TOP,
				LSM_FP_BOTTOM
			},
		};

		for ( uint32_t I = 4; I--; ) {
			_fpRet[I] = fpTable[_fpPlane][I];
		}
	}

}	// namespace lsm
