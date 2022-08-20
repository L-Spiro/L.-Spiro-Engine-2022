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
 * Description: An oriented bounding box.  Oriented bounding boxes attempt to make the tightest rectangular
 *	block around an object, using any axis for the fit.  Because their orientations can be arbitrary, collision tests
 *	against them can be more expensive, but they provide fewer false-positives.
 */


#ifndef __LSP_OBB_H__
#define __LSP_OBB_H__

#include "../LSPPhysicsLib.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Vector/LSMVector3.h"

namespace lsp {

	/**
	 * Class CObb
	 * \brief An oriented bounding box.
	 *
	 * Description: An oriented bounding box.  Oriented bounding boxes attempt to make the tightest rectangular
	 *	block around an object, using any axis for the fit.  Because their orientations can be arbitrary, collision tests
	 *	against them can be more expensive, but they provide fewer false-positives.
	 */
	class CObb {
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
		CVector3 LSE_CALL						ClosestPointToPoint( const CVector3 &_vPoint ) const;
		
		/**
		 * Gets the distance from the given point to this OBB.
		 *
		 * \param _vPoint The point from which the distance to this bounding box is to be calculated.
		 * \return Returns the squared distance from the closest point on this box to the given point to the
		 *	given point.
		 */
		LSREAL LSE_CALL							GetSqDistFromPoint( const CVector3 &_vPoint ) const;

		/**
		 * Creates a new bounding box that represents the result of this bounding box transformed by the given matrix.
		 *
		 * \param _mTransform The matrix transform.
		 * \return Returns a new OBB representing this OBB transformed by the given matrix.
		 */
		CObb LSE_CALL							Transform( const CMatrix4x4 &_mTransform ) const;


		// == Members.
		/**
		 * The center of the box.
		 */
		CVector3								m_vCenter;

		/**
		 * The local X-, Y-, and Z- axis directions.
		 */
		CVector3								m_vAxes[3];

		/**
		 * The positive half-width extents along each axis.
		 */
		CVector3								m_vExtents;

	};

}	// namespace lsp

#endif	// __LSP_OBB_H__
