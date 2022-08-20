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
 * Description: Euler orientation as a helper for a standard orientation.  Its primary use is in animation,
 *	where rotations are expressed as degrees around axes and each rotation is updated by tracks.  This data
 *	is then applied to a COrientation instance for standard processing.
 */


#ifndef __LSM_EULERORIENTATION_H__
#define __LSM_EULERORIENTATION_H__

#include "../LSMMathLib.h"
#include "../Matrix/LSMMatrix4x4.h"
#include "../Vector/LSMVector3.h"

namespace lsm {

	class								COrientation;

	/**
	 * Class CEulerOrientation
	 * \brief Euler orientation as a helper for a standard orientation.
	 *
	 * Description: Euler orientation as a helper for a standard orientation.  Its primary use is in animation,
	 *	where rotations are expressed as degrees around axes and each rotation is updated by tracks.  This data
	 *	is then applied to a COrientation instance for standard processing.
	 */
	class CEulerOrientation {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CEulerOrientation();


		// == Enumerations.
		/** Dirty flags. */
		enum LSM_EULER_DIRTY {
			LSM_ED_POS_X				= (1 << 0),			/**< The X position has changed. */
			LSM_ED_POS_Y				= (1 << 1),			/**< The Y position has changed. */
			LSM_ED_POS_Z				= (1 << 2),			/**< The Z position has changed. */

			LSM_ED_SCALE_X				= (1 << 3),			/**< The X scale has changed. */
			LSM_ED_SCALE_Y				= (1 << 4),			/**< The Y scale has changed. */
			LSM_ED_SCALE_Z				= (1 << 5),			/**< The Z scale has changed. */

			LSM_ED_ROT_X				= (1 << 6),			/**< The X rotation has changed. */
			LSM_ED_ROT_Y				= (1 << 7),			/**< The Y rotation has changed. */
			LSM_ED_ROT_Z				= (1 << 8),			/**< The Z rotation has changed. */
		};

		/** Rotation orders. */
		enum LSM_EULER_ROT_ORDER {
			LSM_ERO_XYZ,									/**< XYZ order. */
			LSM_ERO_YZX,									/**< YZX order. */
			LSM_ERO_ZXY,									/**< ZXY order. */
			LSM_ERO_XZY,									/**< XZY order. */
			LSM_ERO_YXZ,									/**< YXZ order. */
			LSM_ERO_ZYX,									/**< ZYX order. */
		};


		// == Functions.
		/**
		 * Gets the position for read-only.
		 *
		 * \return Returns a read-only reference to the position.
		 */
		LSE_INLINE const CVector3 & LSE_CALL
										Pos() const;

		/**
		 * Gets the position for read and write.
		 *
		 * \return Returns a read-and-write reference to the position.
		 */
		LSE_INLINE CVector3 & LSE_CALL	Pos();

		/**
		 * Gets the scale for read-only.
		 *
		 * \return Returns a read-only reference to the scale.
		 */
		LSE_INLINE const CVector3 & LSE_CALL
										Scale() const;

		/**
		 * Gets the scale for read and write.
		 *
		 * \return Returns a read-and-write reference to the scale.
		 */
		LSE_INLINE CVector3 & LSE_CALL	Scale();

		/**
		 * Gets the rotation for read-only.
		 *
		 * \return Returns a read-only reference to the rotation.
		 */
		LSE_INLINE const CVector3 & LSE_CALL
										Rot() const;

		/**
		 * Gets the rotation for read and write.
		 *
		 * \return Returns a read-and-write reference to the rotation.
		 */
		LSE_INLINE CVector3 & LSE_CALL	Rot();

		/**
		 * Gets the rotation order for read-only.
		 *
		 * \return Returns the rotation order for read-only.
		 */
		LSE_INLINE LSM_EULER_ROT_ORDER LSE_CALL
										RotOrder() const;

		/**
		 * Gets a read-and-write reference to the rotation order.
		 *
		 * \return Returns a read-and-write reference to the rotation order.
		 */
		LSE_INLINE uint32_t & LSE_CALL	RotOrder();

		/**
		 * Applies this orientation to a COrientation object, updating only things that
		 *	have been modified on this object.  Also clears the dirty flag.
		 *
		 * \param _oOrientation The orientation to update with the values in this
		 *	Euler orientation that have changed.
		 */
		void LSE_CALL					UpdateOrientation( COrientation &_oOrientation ) const;


	protected :
		// == Members.
		/**
		 * The position of the object.
		 */
		CVector3						m_vPos;

		/**
		 * The scale of the object.
		 */
		CVector3						m_vScale;

		/**
		 * The Euler of the object.
		 */
		CVector3						m_vRot;

		/**
		 * Dirty flags for the orientation.
		 */
		mutable uint32_t				m_ui32Dirty;

		/**
		 * Rotation order.
		 */
		uint32_t						m_ui32RotType;


		// == Functions.
		/**
		 * Creates a rotation matrix given the Euler angles and order of rotation.
		 *
		 * \param _mMat The matrix to be filled with the result.
		 * \return Returns _mMat.
		 */
		CMatrix4x4 & LSE_CALL			CreateRotMatrix( CMatrix4x4 &_mMat ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the position for read-only.
	 *
	 * \return Returns a read-only reference to the position.
	 */
	LSE_INLINE const CVector3 & LSE_CALL CEulerOrientation::Pos() const {
		return m_vPos;
	}

	/**
	 * Gets the position for read and write.
	 *
	 * \return Returns a read-and-write reference to the position.
	 */
	LSE_INLINE CVector3 & LSE_CALL CEulerOrientation::Pos() {
		m_ui32Dirty |= (LSM_ED_POS_X | LSM_ED_POS_Y | LSM_ED_POS_Z);
		return m_vPos;
	}

	/**
	 * Gets the scale for read-only.
	 *
	 * \return Returns a read-only reference to the scale.
	 */
	LSE_INLINE const CVector3 & LSE_CALL CEulerOrientation::Scale() const {
		return m_vScale;
	}

	/**
	 * Gets the scale for read and write.
	 *
	 * \return Returns a read-and-write reference to the scale.
	 */
	LSE_INLINE CVector3 & LSE_CALL CEulerOrientation::Scale() {
		m_ui32Dirty |= (LSM_ED_SCALE_X | LSM_ED_SCALE_Y | LSM_ED_SCALE_Z);
		return m_vScale;
	}

	/**
	 * Gets the rotation for read-only.
	 *
	 * \return Returns a read-only reference to the rotation.
	 */
	LSE_INLINE const CVector3 & LSE_CALL CEulerOrientation::Rot() const {
		return m_vRot;
	}

	/**
	 * Gets the rotation for read and write.
	 *
	 * \return Returns a read-and-write reference to the rotation.
	 */
	LSE_INLINE CVector3 & LSE_CALL CEulerOrientation::Rot() {
		m_ui32Dirty |= (LSM_ED_ROT_X | LSM_ED_ROT_Y | LSM_ED_ROT_Z);
		return m_vRot;
	}

	/**
	 * Gets the rotation order for read-only.
	 *
	 * \return Returns the rotation order for read-only.
	 */
	LSE_INLINE CEulerOrientation::LSM_EULER_ROT_ORDER LSE_CALL CEulerOrientation::RotOrder() const {
		return static_cast<LSM_EULER_ROT_ORDER>(m_ui32RotType);
	}

	/**
	 * Gets a read-and-write reference to the rotation order.
	 *
	 * \return Returns a read-and-write reference to the rotation order.
	 */
	LSE_INLINE uint32_t & LSE_CALL CEulerOrientation::RotOrder() {
		m_ui32Dirty |= (LSM_ED_ROT_X | LSM_ED_ROT_Y | LSM_ED_ROT_Z);
		return m_ui32RotType;
	}

}	// namespace lsm

#endif	// __LSM_EULERORIENTATION_H__
