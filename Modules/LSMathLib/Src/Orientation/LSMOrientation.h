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
 * Description: A base class for all objects that can have an orientation in 3D.
 */


#ifndef __LSM_ORIENTATION_H__
#define __LSM_ORIENTATION_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "../Matrix/LSMMatrix4x4.h"

namespace lsm {

	// == Types.
	/** Dirty flags for orientations. */
	enum LSM_ORIENT_DIRTY_FLAGS {
		LSM_ODF_POSITION							= (1 << 0),			/**< Position is dirty. */
		LSM_ODF_ROTATION							= (1 << 1),			/**< Rotation is dirty. */
		LSM_ODF_SCALE								= (1 << 2),			/**< Scale is dirty. */
		LSM_ODF_NORMALIZATION						= (1 << 3),			/**< Vectors need normalizing. */
	};

	/**
	 * Class COrientation
	 * \brief Orientation of objects.
	 *
	 * Description: A base class for all objects that can have an orientation in 3D.
	 */
	class COrientation {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						COrientation();
		LSE_INLINE LSE_CALLCTOR						COrientation( const COrientation &_oSrc );


		// == Operators.
		/**
		 * Copy an orientation.  The dirty flag on the copied orientation is always set after this
		 *	operation.
		 *
		 * \param _oSrc The source orientation.
		 * \return Returns the copied object.
		 */
		LSE_INLINE COrientation & LSE_FCALL			operator = ( const COrientation &_oSrc );


		// == Functions.
		/**
		 * Gets the forward vector.
		 *
		 * \return Returns the forward vector of this orientation.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		GetForward() const;

		/**
		 * Gets the right vector.
		 *
		 * \return Returns the right vector of this orientation.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		GetRight() const;

		/**
		 * Gets the up vector.
		 *
		 * \return Returns the up vector of this orientation.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		GetUp() const;

		/**
		 * Sets the orientation to identity.
		 */
		void LSE_FCALL							SetIdentity();

		/**
		 * Sets the position to [0,0,0].
		 */
		void LSE_FCALL							ClearPosition();

		/**
		 * Sets the rotation to identity.
		 */
		void LSE_FCALL							ClearRotation();

		/**
		 * Sets the scale to [1,1,1].
		 */
		void LSE_FCALL							ClearScale();

		/**
		 * Gets the position as read-only.
		 *
		 * \return Returns the position as a read-only reference.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		Pos() const;

		/**
		 * Gets the position as write-only.
		 *
		 * \return Returns the position as a write-only reference.
		 */
		LSE_INLINE CVector3 & LSE_FCALL				Pos();

		/**
		 * Sets the absolute position.  Only causes a change if the given position is not the same as the
		 *	current position.
		 *
		 * \param _vPos The position to apply to this orientation.
		 */
		void LSE_FCALL							SetPosU( const CVector3 &_vPos );

		/**
		 * Sets the position of this orientation relative to its facing direction.
		 *
		 * \param _vPos The position to apply to this orientation.
		 */
		void LSE_FCALL							SetRelPos( const CVector3 &_vPos );

		/**
		 * Offset the position of this orientation relative to its facing direction.
		 *
		 * \param _vPos The position to add to this orientation.
		 */
		void LSE_FCALL							AddRelPos( const CVector3 &_vPos );

		/**
		 * Sets the rotation component of this orientation.
		 *
		 * \param _mMat The full rotational component to apply to this object.
		 */
		void LSE_FCALL							SetRotation( const CMatrix4x4 &_mMat );

		/**
		 * Adds a rotation to the existing rotation of this object.
		 *
		 * \param _mMat The rotation, in matrix form, to add to the existing rotation of this object.
		 */
		void LSE_FCALL							AddRotation( const CMatrix4x4 &_mMat );

		/**
		 * Sets the rotation component of this object from forward and up vectors.  The right vector
		 *	is derived automatically.
		 *
		 * \param _vForward The forward component of the rotation.
		 * \param _vUp The up component of the rotation.
		 */
		void LSE_FCALL							SetRotation( const CVector3 &_vForward, const CVector3 &_vUp );

		/**
		 * Sets the rotation component of this object from forward and up vectors.  The right vector
		 *	is derived automatically and the up vector is adjusted.
		 *
		 * \param _vForward The forward component of the rotation.
		 * \param _vUp The up component of the rotation.
		 */
		void LSE_FCALL							SetRotationEasy( const CVector3 &_vForward, const CVector3 &_vUp );

		/**
		 * Adds a rotation to this object from forward and up vectors.  The right vector is derived
		 *	automatically.
		 *
		 * \param _vForward The forward component of the rotation to add.
		 * \param _vUp The up component of the rotation to add.
		 */
		void LSE_FCALL							AddRotation( const CVector3 &_vForward, const CVector3 &_vUp );

		/**
		 * Yaw the orientation by the given amount.  The object always rotates around axis [0,1,0].
		 *
		 * \param _fAmount Amount, in radians, to yaw.
		 */
		void LSE_FCALL							Yaw( LSREAL _fAmount );

		/**
		 * Sets the yaw to the given amount.  The object always rotates around axis [0,1,0].
		 *
		 * \param _fAmount Amount, in radians, to which to set the yaw.
		 */
		void LSE_FCALL							SetYaw( LSREAL _fAmount );

		/**
		 * Pitch the orientation by the given amount.  The object always rotates around its right axis.
		 *
		 * \param _fAmount Amount, in radians, to pitch.
		 */
		void LSE_FCALL							Pitch( LSREAL _fAmount );

		/**
		 * Roll the orientation by the given amount.  The object always rotates around its forward axis.
		 *
		 * \param _fAmount Amount, in radians, to roll.
		 */
		void LSE_FCALL							Roll( LSREAL _fAmount );

		/**
		 * Sets the rotation by the given yaw, pitch, and roll.
		 *
		 * \param _fYaw Yaw around the Y axis, in radians.
		 * \param _fPitch Pitch around the X axis, in radians.
		 * \param _fRoll Roll around the Z axis, in radians.
		 */
		void LSE_FCALL							SetRotationYawPitchRoll( LSREAL _fYaw, LSREAL _fPitch, LSREAL _fRoll );

		/**
		 * Sets the scale.
		 *
		 * \param _fScale The new scale of this orientation.
		 */
		LSE_INLINE void LSE_FCALL					SetScale( LSREAL _fScale );

		/**
		 * Adds to the current scale of this object.
		 *
		 * \param _fScale The scale to add to the existing scale of this object.
		 */
		LSE_INLINE void LSE_FCALL					AddScale( LSREAL _fScale );

		/**
		 * Gets the scale as read-only.
		 *
		 * \return Returns the scale as a read-only reference.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		Scale() const;

		/**
		 * Gets the scale as write-only.
		 *
		 * \return Returns the scale as a write-only reference.
		 */
		LSE_INLINE CVector3 & LSE_FCALL				Scale();

		/**
		 * Builds a matrix from our data.
		 *
		 * \param _mRet Holds the created matrix.
		 */
		void LSE_FCALL							BuildMatrix( CMatrix4x4 &_mRet ) const;

		/**
		 * Updates, if necessary, the orientation, filling the given matrix with the result.  If the orientation
		 *	has not been modified since the last update, no action is performed.
		 *
		 * \param _mRet Holds the created matrix.
		 * \param _bClearDirty Determines whether the dirty flag will be cleared after this update.  This
		 *	function only updates the given matrix if the dirty flag is set, so clearing the dirty flag
		 *	results in no more updates until the orientation is modified in some way.
		 *	In this manner, updates are only performed when needed.
		 * \return Returns true if an update was performed.
		 */
		LSBOOL LSE_FCALL							UpdateMatrix( CMatrix4x4 &_mRet, LSBOOL _bClearDirty = true ) const;

		/**
		 * Are we dirty at all?
		 *
		 * \return Returns true if the orientation is dirty and needs to update matrices.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					IsDirty() const;
		
		/**
		 * Make us dirty.  This causes an update at the next UpdateMatrix() call.
		 */
		LSE_INLINE void LSE_FCALL					MakeDirty();

		/**
		 * Undirty rotations if necessary.
		 */
		LSE_INLINE void LSE_FCALL					UndirtyRotations() const;

		/**
		 * Undirties all flags.
		 */
		LSE_INLINE void LSE_FCALL					Undirty() const;
		
		
	protected :
		// == Members.
		/**
		 * Orientation.
		 */
		mutable CVector3							m_vRight,
													m_vUp,
													m_vForward;

		/**
		 * Position.
		 */
		CVector3									m_vPos;

		/**
		 * Scale.
		 */
		CVector3									m_vScale;

		/**
		 * Dirty flags.
		 */
		mutable uint32_t							m_ui32Dirty;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR COrientation::COrientation() :
		m_vRight( LSM_ONE, LSM_ZERO, LSM_ZERO ),
		m_vUp( LSM_ZERO, LSM_ONE, LSM_ZERO ),
		m_vForward( LSM_ZERO, LSM_ZERO, LSM_ONE ),
		m_vPos( LSM_ZERO, LSM_ZERO, LSM_ZERO ),
		m_vScale( LSM_ONE, LSM_ONE, LSM_ONE ),
		m_ui32Dirty( LSM_ODF_POSITION | LSM_ODF_ROTATION | LSM_ODF_SCALE ) {
	}
	LSE_INLINE LSE_CALLCTOR COrientation::COrientation( const COrientation &_oSrc ) :
		m_vRight( _oSrc.m_vRight ),
		m_vUp( _oSrc.m_vUp ),
		m_vForward( _oSrc.m_vForward ),
		m_vPos( _oSrc.m_vPos ),
		m_vScale( _oSrc.m_vScale ),
		m_ui32Dirty( _oSrc.m_ui32Dirty ) {
	}

	// == Operators.
	/**
	 * Copy an orientation.  The dirty flag on the copied orientation is always set after this
	 *	operation.
	 *
	 * \param _oSrc The source orientation.
	 * \return Returns the copied object.
	 */
	LSE_INLINE COrientation & LSE_FCALL COrientation::operator = ( const COrientation &_oSrc ) {
		m_vRight = _oSrc.m_vRight;
		m_vUp = _oSrc.m_vUp;
		m_vForward = _oSrc.m_vForward;
		m_vPos = _oSrc.m_vPos;
		m_vScale = _oSrc.m_vScale;
		m_ui32Dirty = LSM_ODF_NORMALIZATION | LSM_ODF_POSITION | LSM_ODF_ROTATION | LSM_ODF_SCALE;
		return (*this);
	}

	// == Functions.	
	/**
	 * Gets the forward vector.
	 *
	 * \return Returns the forward vector of this orientation.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL COrientation::GetForward() const	{ const_cast<COrientation *>(this)->UndirtyRotations(); return m_vForward; }
	
	/**
	 * Gets the right vector.
	 *
	 * \return Returns the right vector of this orientation.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL COrientation::GetRight() const	{ const_cast<COrientation *>(this)->UndirtyRotations(); return m_vRight; }
	
	/**
	 * Gets the up vector.
	 *
	 * \return Returns the up vector of this orientation.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL COrientation::GetUp() const		{ const_cast<COrientation *>(this)->UndirtyRotations(); return m_vUp; }

	/**
	 * Gets the position as read-only.
	 *
	 * \return Returns the position as a read-only reference.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL COrientation::Pos() const {
		return m_vPos;
	}

	/**
	 * Gets the position as write-only.
	 *
	 * \return Returns the position as a write-only reference.
	 */
	LSE_INLINE CVector3 & LSE_FCALL COrientation::Pos() {
		m_ui32Dirty |= LSM_ODF_POSITION;
		return m_vPos;
	}

	/**
	 * Sets the scale.
	 *
	 * \param _fScale The new scale of this orientation.
	 */
	LSE_INLINE void LSE_FCALL COrientation::SetScale( LSREAL _fScale ) {
		m_vScale.Set( _fScale, _fScale, _fScale );
		m_ui32Dirty |= LSM_ODF_SCALE;
	}

	/**
	 * Adds to the current scale of this object.
	 *
	 * \param _fScale The scale to add to the existing scale of this object.
	 */
	LSE_INLINE void LSE_FCALL COrientation::AddScale( LSREAL _fScale ) {
		m_vScale.Set( m_vScale.x + _fScale, m_vScale.y + _fScale, m_vScale.z + _fScale );
		m_ui32Dirty |= LSM_ODF_SCALE;
	}

	/**
	 * Gets the scale as read-only.
	 *
	 * \return Returns the scale as a read-only reference.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL COrientation::Scale() const {
		return m_vScale;
	}

	/**
	 * Gets the scale as write-only.
	 *
	 * \return Returns the scale as a write-only reference.
	 */
	LSE_INLINE CVector3 & LSE_FCALL COrientation::Scale() {
		m_ui32Dirty |= LSM_ODF_POSITION;
		return m_vScale;
	}

	/**
	 * Are we dirty at all?
	 *
	 * \return Returns true if the orientation is dirty and needs to update matrices.
	 */
	LSE_INLINE LSBOOL LSE_FCALL COrientation::IsDirty() const {
		return m_ui32Dirty != 0;
	}
	
	/**
	 * Make us dirty.  This causes an update at the next UpdateMatrix() call.
	 */
	LSE_INLINE void LSE_FCALL COrientation::MakeDirty() {
		m_ui32Dirty = LSM_ODF_NORMALIZATION | LSM_ODF_POSITION | LSM_ODF_ROTATION | LSM_ODF_SCALE;
	}

	/**
	 * Undirty rotations if necessary.
	 */
	LSE_INLINE void LSE_FCALL COrientation::UndirtyRotations() const {
		if ( m_ui32Dirty & LSM_ODF_NORMALIZATION ) {
			m_vRight.Normalize();
			m_vUp.Normalize();
			m_vForward.Normalize();
			m_ui32Dirty &= ~static_cast<uint32_t>(LSM_ODF_NORMALIZATION);
		}
	}

	/**
	 * Undirties all flags.
	 */
	LSE_INLINE void LSE_FCALL COrientation::Undirty() const {
		m_ui32Dirty = 0;
	}

}	// namespace lsm

#endif	// __LSM_ORIENTATION_H__
