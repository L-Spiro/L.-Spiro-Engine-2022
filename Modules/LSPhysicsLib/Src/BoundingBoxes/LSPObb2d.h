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
 * Description: An oriented bounding box in 2D.  Oriented bounding boxes attempt to make the tightest rectangular
 *	block around an object, using any axis for the fit.  Because their orientations can be arbitrary, collision tests
 *	against them can be more expensive, but they provide fewer false-positives.
 */


#ifndef __LSE_OBB2D_H__
#define __LSE_OBB2D_H__

#include "../LSPPhysicsLib.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Vector/LSMVector2.h"

namespace lsp {

	/**
	 * Class CObb2d
	 * \brief An oriented bounding box in 2D.
	 *
	 * Description: An oriented bounding box in 2D.  Oriented bounding boxes attempt to make the tightest rectangular
 *	block around an object, using any axis for the fit.  Because their orientations can be arbitrary, collision tests
 *	against them can be more expensive, but they provide fewer false-positives.
	 */
	class CObb2d {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// DISTANCE TESTS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// == Points.
		/**
		 * Gets the closest point on this OBB to a given point.
		 *
		 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
		 * \return Returns the closest point on this box from the given point.
		 */
		CVector2 LSE_CALL						ClosestPointToPoint( const CVector2 &_vPoint ) const;
		
		/**
		 * Gets the distance from the given point to this OBB.
		 *
		 * \param _vPoint The point from which the distance to this bounding box is to be calculated.
		 * \return Returns the squared distance from the closest point on this box to the given point to the
		 *	given point.
		 */
		LSREAL LSE_CALL							GetSqDistFromPoint( const CVector2 &_vPoint ) const;

		/**
		 * Creates a new bounding box that represents the result of this bounding box transformed by the given matrix.
		 *
		 * \param _mTransform The matrix transform.
		 * \param _oob2Ret On return, this holds the transformed OBB.
		 */
		void LSE_CALL							Transform( const CMatrix4x4 &_mTransform, CObb2d &_oob2Ret ) const;


		// == Members.
		/**
		 * The center of the box.
		 */
		CVector2								m_vCenter;

		/**
		 * The local X- and Y- axis directions.
		 */
		CVector2								m_vAxes[2];

		/**
		 * The positive half-width extents along each axis.
		 */
		CVector2								m_vExtents;
	};

}	// namespace lsp

#endif	// __LSE_OBB2D_H__
