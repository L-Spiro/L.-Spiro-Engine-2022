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


#ifndef __LSP_AABB2D_H__
#define __LSP_AABB2D_H__

#include "../LSPPhysicsLib.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Vector/LSMVector2.h"

namespace lsp {

	// == Enumerations.
	/**
	 * The axis of each side of the AABB.
	 */
	enum LSP_AABB_2D_CORNERS {
		LSP_A2C_UPPER_LEFT,
		LSP_A2C_UPPER_RIGHT,

		LSP_A2C_LOWER_LEFT,
		LSP_A2C_LOWER_RIGHT,
	};

	/**
	 * Class CAabb2d
	 * \brief A min-max axis-aligned 2D bounding box.
	 *
	 * Description: A min-max axis-aligned 2D bounding box.
	 */
	class CAabb2d {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Make the box infinitely small.
		 */
		void LSE_CALL							MakeInfinitelySmall();

		/**
		 * Computes an AABB from a set of points given in the form of an array of CVector2's.
		 *
		 * \param _vPoints The array of points to enclose.
		 * \param _uiTotal The number of points in the array to which _vPoints points.
		 */
		void LSE_CALL							ComputeAabbFromPointArray( const CVector2 * _pvPoints, uint32_t _uiTotal );

		/**
		 * Computes an AABB from an existing AABB with a matrix-stored translation.
		 *	The matrix must be row-major.
		 *
		 * \param _aabb2Other The source bounding box to transform by the given matrix and store in this object.
		 * \param _mTrans The matrix used to transform _aabb2Other into this bounding box.
		 */
		void LSE_CALL							ComputeAabbFromAabbAndMatrix( const CAabb2d &_aabb2Other, const CMatrix4x4 &_mTrans );

		/**
		 * Change the size of the AABB to encompass another AABB.
		 *
		 * \param _aabb2Other The bounding box to fully encompass by this bounding box.
		 */
		void LSE_CALL							Encompass( const CAabb2d &_aabb2Other );

		/**
		 * Change the size of the AABB to encompass a point.
		 *
		 * \param _vOther The point to fully encompass by this bounding box.
		 */
		void LSE_CALL							Encompass( const CVector2 &_vOther );

		/**
		 * Fill an array of 4 vectors with the vertices of the bounding box represented by this object.
		 *
		 * \param _vPoints An array of at least 4 vectors to be filled with the points at the corners of
		 *	this bounding box.
		 */
		void LSE_CALL							GetBox( CVector2 _vPoints[4] ) const;

		/**
		 * Gets the center of the bounding box.
		 *
		 * \return Returns a point marking the center of this bounding box.
		 */
		CVector2 LSE_CALL						GetCenter() const;

		/**
		 * Gets its X size.
		 *
		 * \return Returns the length of this bounding box along the X axis.
		 */
		LSREAL LSE_CALL							GetSizeX() const;

		/**
		 * Gets its Y size.
		 *
		 * \return Returns the length of this bounding box along the Y axis.
		 */
		LSREAL LSE_CALL							GetSizeY() const;

		/**
		 * Gets its area.
		 *
		 * \return Returns the area of this bounding box.
		 */
		LSREAL LSE_CALL							GetArea() const;



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// DISTANCE TESTS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// == Points.
		/**
		 * Gets the closest point on this AABB to a given point.
		 *
		 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
		 * \return Returns the closest point on this box from the given point.
		 */
		CVector2 LSE_CALL						ClosestPointToPoint( const CVector2 &_vPoint ) const;
		
		/**
		 * Gets the distance from the given point to this AABB.
		 *
		 * \param _vPoint The point from which the distance to this bounding box is to be calculated.
		 * \return Returns the squared distance from the closest point on this box to the given point to the
		 *	given point.
		 */
		LSREAL LSE_CALL							GetSqDistFromPoint( const CVector2 &_vPoint ) const;


		// == Members.
		/**
		 * The minimum X-, Y-, and Z- axis values.
		 */
		CVector2								m_vMin;

		/**
		 * The maximum X-, Y-, and Z- axis values.
		 */
		CVector2								m_vMax;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets its X size.
	 *
	 * \return Returns the length of this bounding box along the X axis.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb2d::GetSizeX() const {
		return m_vMax.x - m_vMin.x;
	}

	/**
	 * Gets its Y size.
	 *
	 * \return Returns the length of this bounding box along the Y axis.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb2d::GetSizeY() const {
		return m_vMax.y - m_vMin.y;
	}

}	// namespace lsp

#endif	// __LSP_AABB2D_H__
