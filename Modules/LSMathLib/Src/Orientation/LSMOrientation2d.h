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
 * Description: A 2D orientation.
 */


#ifndef __LSM_ORIENTATION2D_H__
#define __LSM_ORIENTATION2D_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector2.h"
#include "../Matrix/LSMMatrix3x3.h"
#include "../Matrix/LSMMatrix4x4.h"

namespace lsm {

	// == Types.
	/** Dirty flags for orientations. */
	enum LSM_2D_ORIENT_DIRTY_FLAGS {
		LSM_2DODF_POSITION							= (1 << 0),			/**< Position is dirty. */
		LSM_2DODF_ROTATION							= (1 << 1),			/**< Rotation is dirty. */
		LSM_2DODF_SCALE								= (1 << 2),			/**< Scale is dirty. */
	};

	/**
	 * Class COrientation2d
	 * \brief Orientation of objects.
	 *
	 * Description: A 2D orientation.
	 */
	class COrientation2d {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						COrientation2d();
		LSE_INLINE LSE_CALLCTOR						COrientation2d( const COrientation2d &_oSrc );


		// == Operators.
		/**
		 * Copy an orientation.  The dirty flag on the copied orientation is always set after this
		 *	operation.
		 *
		 * \param _oSrc The source orientation.
		 * \return Returns the copied object.
		 */
		LSE_INLINE COrientation2d & LSE_FCALL		operator = ( const COrientation2d &_oSrc );


		// == Functions.
		/**
		 * Sets the orientation to identity.
		 */
		void LSE_FCALL							SetIdentity();

		/**
		 * Sets the position to [0,0].
		 */
		void LSE_FCALL							ClearPosition();

		/**
		 * Sets the rotation to identity.
		 */
		void LSE_FCALL							ClearRotation();

		/**
		 * Sets the scale to [1,1].
		 */
		void LSE_FCALL							ClearScale();

		/**
		 * Gets the position as read-only.
		 *
		 * \return Returns the position as a read-only reference.
		 */
		LSE_INLINE const CVector2 & LSE_FCALL		Pos() const;

		/**
		 * Gets the position as write-only.
		 *
		 * \return Returns the position as a write-only reference.
		 */
		LSE_INLINE CVector2 & LSE_FCALL				Pos();

		/**
		 * Sets the absolute position.  Only causes a change if the given position is not the same as the
		 *	current position.
		 *
		 * \param _vPos The position to apply to this orientation.
		 */
		void LSE_FCALL							SetPosU( const CVector2 &_vPos );

		/**
		 * Sets the position of this orientation relative to its facing direction.
		 *
		 * \param _vPos The position to apply to this orientation.
		 */
		void LSE_FCALL							SetRelPos( const CVector2 &_vPos );

		/**
		 * Offset the position of this orientation relative to its facing direction.
		 *
		 * \param _vPos The position to add to this orientation.
		 */
		void LSE_FCALL							AddRelPos( const CVector2 &_vPos );

		/**
		 * Sets the rotation in radians.
		 *
		 * \param _fRot Rotation in radians.
		 */
		LSE_INLINE void LSE_CALL					SetRotInRadians( LSREAL _fRot );

		/**
		 * Sets the rotation in degrees.
		 *
		 * \param _fRot Rotation in degrees.
		 */
		LSE_INLINE void LSE_CALL					SetRotInDegrees( LSREAL _fRot );

		/**
		 * Adds a rotation to this object in radians.
		 *
		 * \param _fRot The rotation to add.
		 */
		LSE_INLINE void LSE_CALL					AddRotation( LSREAL _fRot );

		/**
		 * Gets the rotation in radians.
		 *
		 * \return Returns the rotation in radians.
		 */
		LSE_INLINE LSREAL LSE_CALL					Rot() const;

		/**
		 * Sets the scale.
		 *
		 * \param _fScale The new scale of this orientation.
		 */
		LSE_INLINE void LSE_FCALL					SetScale( LSREAL _fScale );

		/**
		 * Sets the scale.
		 *
		 * \param _vScale The new scale of this orientation.
		 */
		LSE_INLINE void LSE_FCALL					SetScale( const CVector2 & _vScale );

		/**
		 * Sets the scale.
		 *
		 * \param _fScaleX The new X scale of this orientation.
		 * \param _fScaleY The new Y scale of this orientation.
		 */
		LSE_INLINE void LSE_FCALL					SetScale( LSREAL _fScaleX, LSREAL _fScaleY );

		/**
		 * Adds to the current scale of this object.
		 *
		 * \param _fScale The scale to add to the existing scale of this object.
		 */
		LSE_INLINE void LSE_FCALL					AddScale( LSREAL _fScale );

		/**
		 * Adds to the current scale of this object.
		 *
		 * \param _vScale The scale to add to the existing scale of this object.
		 */
		LSE_INLINE void LSE_FCALL					AddScale( const CVector2 & _vScale );

		/**
		 * Adds to the current scale of this object.
		 *
		 * \param _fScaleX The X scale to add to the existing scale of this object.
		 * \param _fScaleY The Y scale to add to the existing scale of this object.
		 */
		LSE_INLINE void LSE_FCALL					AddScale( LSREAL _fScaleX, LSREAL _fScaleY );

		/**
		 * Gets the scale as read-only.
		 *
		 * \return Returns the scale as a read-only reference.
		 */
		LSE_INLINE const CVector2 & LSE_FCALL		Scale() const;

		/**
		 * Gets the scale as write-only.
		 *
		 * \return Returns the scale as a write-only reference.
		 */
		LSE_INLINE CVector2 & LSE_FCALL				Scale();

		/**
		 * Gets the right and up vectors.
		 *
		 * \param _vRight The returned right vector.
		 * \param _vUp The returned up vector
		 */
		LSE_INLINE void LSE_CALL					GetVectors( CVector2 &_vRight, CVector2 &_vUp ) const;

		/**
		 * Builds a matrix from our data.
		 *
		 * \param _mRet Holds the created matrix.
		 */
		void LSE_FCALL							BuildMatrix( CMatrix3x3 &_mRet ) const;

		/**
		 * Builds a 4-by-4 matrix from our data.
		 *
		 * \param _mRet Holds the created matrix.
		 * \param _mRet The Z position.
		 */
		void LSE_FCALL							BuildMatrix( CMatrix4x4 &_mRet, LSREAL _fZ = LSM_ONE ) const;

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
		LSBOOL LSE_FCALL							UpdateMatrix( CMatrix3x3 &_mRet, LSBOOL _bClearDirty = true ) const;

		/**
		 * Updates, if necessary, the orientation, filling the given matrix with the result.  If the orientation
		 *	has not been modified since the last update, no action is performed.
		 *
		 * \param _mRet Holds the created matrix.
		 * \param _bClearDirty Determines whether the dirty flag will be cleared after this update.  This
		 *	function only updates the given matrix if the dirty flag is set, so clearing the dirty flag
		 *	results in no more updates until the orientation is modified in some way.
		 *	In this manner, updates are only performed when needed.
		 * \param _mRet The Z position.
		 * \return Returns true if an update was performed.
		 */
		LSBOOL LSE_FCALL							UpdateMatrix( CMatrix4x4 &_mRet, LSBOOL _bClearDirty = true, LSREAL _fZ = LSM_ONE ) const;

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
		 * Undirties all flags.
		 */
		LSE_INLINE void LSE_FCALL					Undirty() const;

		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// INTERNAL USE ONLY
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Gets a direct reference to the position vector.  Do not use.
		 *
		 * \return Returns a direct reference to the position vector.
		 */
		LSE_INLINE CVector2 & LSE_CALL				DirectPos();

		/**
		 * Gets a direct reference to the scale vector.  Do not use.
		 *
		 * \return Returns a direct reference to the scale vector.
		 */
		LSE_INLINE CVector2 & LSE_CALL				DirectScale();

		/**
		 * Gets a direct reference to the rotation float.  Do not use.
		 *
		 * \return Returns a direct reference to the rotation float.
		 */
		LSE_INLINE LSREAL & LSE_CALL				DirectRot();

		/**
		 * Gets a direct reference to the dirty flag.  Do not use.
		 *
		 * \return Returns a direct reference to the dirty flag.
		 */
		LSE_INLINE uint32_t & LSE_CALL				DirectDirty();
		
		
	protected :
		// == Members.
		/** Rotation in radians. */
		LSREAL										m_fRot;

		/** Position. */
		CVector2									m_vPos;

		/** Scale. */
		CVector2									m_vScale;

		/** Dirty flags. */
		mutable uint32_t							m_ui32Dirty;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR COrientation2d::COrientation2d() :
		m_fRot( LSM_ZERO ),
		m_vPos( LSM_ZERO, LSM_ZERO ),
		m_vScale( LSM_ONE, LSM_ONE ),
		m_ui32Dirty( LSM_2DODF_POSITION | LSM_2DODF_ROTATION | LSM_2DODF_SCALE ) {
	}
	LSE_INLINE LSE_CALLCTOR COrientation2d::COrientation2d( const COrientation2d &_oSrc ) :
		m_fRot( _oSrc.m_fRot ),
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
	LSE_INLINE COrientation2d & LSE_FCALL COrientation2d::operator = ( const COrientation2d &_oSrc ) {
		m_fRot = _oSrc.m_fRot;
		m_vPos = _oSrc.m_vPos;
		m_vScale = _oSrc.m_vScale;
		m_ui32Dirty = _oSrc.m_ui32Dirty;
		return (*this);
	}

	// == Functions.
	/**
	 * Gets the position as read-only.
	 *
	 * \return Returns the position as a read-only reference.
	 */
	LSE_INLINE const CVector2 & LSE_FCALL COrientation2d::Pos() const {
		return m_vPos;
	}

	/**
	 * Gets the position as write-only.
	 *
	 * \return Returns the position as a write-only reference.
	 */
	LSE_INLINE CVector2 & LSE_FCALL COrientation2d::Pos() {
		m_ui32Dirty |= LSM_2DODF_POSITION;
		return m_vPos;
	}

	/**
	 * Sets the rotation in radians.
	 *
	 * \param _fRot Rotation in radians.
	 */
	LSE_INLINE void LSE_CALL COrientation2d::SetRotInRadians( LSREAL _fRot ) {
		m_fRot = CMathLib::FMod( _fRot, LSM_DOUBLEPI );
		m_ui32Dirty |= LSM_2DODF_ROTATION;
	}

	/**
	 * Sets the rotation in degrees.
	 *
	 * \param _fRot Rotation in degrees.
	 */
	LSE_INLINE void LSE_CALL COrientation2d::SetRotInDegrees( LSREAL _fRot ) {
		m_fRot = CMathLib::FMod( _fRot * LSM_DEG2RAD, LSM_DOUBLEPI );
		m_ui32Dirty |= LSM_2DODF_ROTATION;
	}

	/**
	 * Adds a rotation to this object in radians.
	 *
	 * \param _fRot The rotation to add.
	 */
	LSE_INLINE void LSE_CALL COrientation2d::AddRotation( LSREAL _fRot ) {
		if ( _fRot ) {
			m_fRot = CMathLib::FMod( m_fRot + _fRot, LSM_DOUBLEPI );
			m_ui32Dirty |= LSM_2DODF_ROTATION;
		}
	}

	/**
	 * Gets the rotation in radians.
	 *
	 * \return Returns the rotation in radians.
	 */
	LSE_INLINE LSREAL LSE_CALL COrientation2d::Rot() const {
		return m_fRot;
	}

	/**
	 * Sets the scale.
	 *
	 * \param _fScale The new scale of this orientation.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::SetScale( LSREAL _fScale ) {
		m_vScale.Set( _fScale, _fScale );
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Sets the scale.
	 *
	 * \param _vScale The new scale of this orientation.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::SetScale( const CVector2 & _vScale ) {
		m_vScale = _vScale;
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Sets the scale.
	 *
	 * \param _fScaleX The new X scale of this orientation.
	 * \param _fScaleY The new Y scale of this orientation.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::SetScale( LSREAL _fScaleX, LSREAL _fScaleY ) {
		m_vScale.Set( _fScaleX, _fScaleY );
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Adds to the current scale of this object.
	 *
	 * \param _fScale The scale to add to the existing scale of this object.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::AddScale( LSREAL _fScale ) {
		m_vScale.Set( m_vScale.x + _fScale, m_vScale.y + _fScale );
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Adds to the current scale of this object.
	 *
	 * \param _vScale The scale to add to the existing scale of this object.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::AddScale( const CVector2 & _vScale ) {
		m_vScale += _vScale;
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Adds to the current scale of this object.
	 *
	 * \param _fScaleX The X scale to add to the existing scale of this object.
	 * \param _fScaleY The Y scale to add to the existing scale of this object.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::AddScale( LSREAL _fScaleX, LSREAL _fScaleY ) {
		m_vScale.Set( m_vScale.x + _fScaleX, m_vScale.y + _fScaleY );
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Gets the scale as read-only.
	 *
	 * \return Returns the scale as a read-only reference.
	 */
	LSE_INLINE const CVector2 & LSE_FCALL COrientation2d::Scale() const {
		return m_vScale;
	}

	/**
	 * Gets the scale as write-only.
	 *
	 * \return Returns the scale as a write-only reference.
	 */
	LSE_INLINE CVector2 & LSE_FCALL COrientation2d::Scale() {
		m_ui32Dirty |= LSM_2DODF_POSITION;
		return m_vScale;
	}

	/**
	 * Gets the right and up vectors.
	 *
	 * \param _vRight The returned right vector.
	 * \param _vUp The returned up vector
	 */
	LSE_INLINE void LSE_CALL COrientation2d::GetVectors( CVector2 &_vRight, CVector2 &_vUp ) const {
		LSREAL fS, fC;
		CMathLib::SinCos( m_fRot, fS, fC );
		_vRight.Set( fC, fS );
		_vUp.Set( -fS, fC );
	}

	/**
	 * Are we dirty at all?
	 *
	 * \return Returns true if the orientation is dirty and needs to update matrices.
	 */
	LSE_INLINE LSBOOL LSE_FCALL COrientation2d::IsDirty() const {
		return m_ui32Dirty != 0;
	}
	
	/**
	 * Make us dirty.  This causes an update at the next UpdateMatrix() call.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::MakeDirty() {
		m_ui32Dirty = LSM_2DODF_POSITION | LSM_2DODF_ROTATION | LSM_2DODF_SCALE;
	}

	/**
	 * Undirties all flags.
	 */
	LSE_INLINE void LSE_FCALL COrientation2d::Undirty() const {
		m_ui32Dirty = 0;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// INTERNAL USE ONLY
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Gets a direct reference to the position vector.  Do not use.
	 *
	 * \return Returns a direct reference to the position vector.
	 */
	LSE_INLINE CVector2 & LSE_CALL COrientation2d::DirectPos() {
		return m_vPos;
	}

	/**
	 * Gets a direct reference to the scale vector.  Do not use.
	 *
	 * \return Returns a direct reference to the scale vector.
	 */
	LSE_INLINE CVector2 & LSE_CALL COrientation2d::DirectScale() {
		return m_vScale;
	}

	/**
	 * Gets a direct reference to the rotation float.  Do not use.
	 *
	 * \return Returns a direct reference to the rotation float.
	 */
	LSE_INLINE LSREAL & LSE_CALL COrientation2d::DirectRot() {
		return m_fRot;
	}

	/**
	 * Gets a direct reference to the dirty flag.  Do not use.
	 *
	 * \return Returns a direct reference to the dirty flag.
	 */
	LSE_INLINE uint32_t & LSE_CALL COrientation2d::DirectDirty() {
		return m_ui32Dirty;
	}

}	// namespace lsm

#endif	// __LSM_ORIENTATION2D_H__
