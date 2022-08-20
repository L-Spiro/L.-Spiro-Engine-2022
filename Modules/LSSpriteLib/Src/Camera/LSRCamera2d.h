/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: An orthogonal camera with the origin in the upper-left corner.
 */


#ifndef __LSR_CAMERA2D_H__
#define __LSR_CAMERA2D_H__

#include "../LSRSpriteLib.h"
#include "../Entity/LSRActor2d.h"
#include "Frustum/LSMFrustum.h"

namespace lsr {

	/**
	 * Class CCamera2d
	 * \brief An orthogonal camera with the origin in the upper-left corner.
	 *
	 * Description: An orthogonal camera with the origin in the upper-left corner.
	 */
	class CCamera2d : public CActor2d {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CCamera2d();


		// == Functions.
		/**
		 * Causes all matrices to be built.
		 */
		void LSE_CALL							Update();

		/**
		 * Sets the width and height of the camera.
		 *
		 * \param _fWidth Width of the camera.
		 * \param _fHeight Height of the camera.
		 */
		LSE_INLINE void LSE_CALL				SetWidthHeight( LSREAL _fWidth, LSREAL _fHeight );

		/**
		 * Gets the camera's width.
		 *
		 * \return Returns the camera's width.
		 */
		LSE_INLINE LSREAL LSE_CALL				Width() const;
		
		/**
		 * Gets the camera's height.
		 *
		 * \return Returns the camera's height.
		 */
		LSE_INLINE LSREAL LSE_CALL				Height() const;

		/**
		 * Sets the near and far of the camera.
		 *
		 * \param _fNear The near plane of the camera.
		 * \param _fFar The far plane of the camera.
		 */
		LSE_INLINE void LSE_CALL				SetNearFar( LSREAL _fNear, LSREAL _fFar );

		/**
		 * Gets the camera's near distance.
		 *
		 * \return Returns the camera's near distance.
		 */
		LSE_INLINE LSREAL LSE_CALL				NearPlane() const;

		/**
		 * Gets the camera's far distance.
		 *
		 * \return Returns the camera's far distance.
		 */
		LSE_INLINE LSREAL LSE_CALL				FarPlane() const;

		/**
		 * Gets the view matrix.
		 *
		 * \return Returns the view matrix.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	ViewMatrix() const;

		/**
 		 * Gets the inverse view matrix.
	 	 *
		 * \return Returns the inverse view matrix.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	InvViewMatrix() const;

		/**
		 * Gets the projection matrix.
		 *
		 * \return Returns the projection matrix.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	ProjMatrix() const;

		/**
		 * Gets the view-projection matrix.
		 *
		 * \return Returns the view-projection matrix.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	ViewProjMatrix() const;

		/**
		 * Gets the frustum of this camera.
		 *
		 * \return Returns the frustum of this camera.
		 */
		LSE_INLINE const CFrustum & LSE_CALL	Frustum() const;

		/**
		 * Gets a constant reference to the camera screen origin vector.
		 *
		 * \return Returns a constant reference to the camera screen origin vector.
		 */
		LSE_INLINE const CVector2 & LSE_CALL	Origin() const;

		/**
		 * Sets the camera screen origin vector.
		 *
		 * \param _vOrigin The new origin vector indicating where [0,0] is on the screen.
		 */
		LSE_INLINE void LSE_CALL				SetOrigin( const CVector2 &_vOrigin );


	protected :
		// == Members.
		/** The view matrix. */
		CMatrix4x4								m_mViewMatrix;

		/** The projection matrix. */
		CMatrix4x4								m_mProjMatrix;

		/** The view-projection matrix. */
		CMatrix4x4								m_mViewProjMatrix;

		/** The origin of the camera. */
		CVector2								m_vOrigin;

		/** The view frustum. */
		CFrustum								m_fFrustum;

		/** Width of the projection in pixels. */
		LSREAL									m_fWidth;

		/** Height of the projection in pixels. */
		LSREAL									m_fHeight;

		/** Near. */
		LSREAL									m_fNear;

		/** Far. */
		LSREAL									m_fFar;

		/** Current screen width. */
		uint32_t								m_ui32CurScrW;

		/** Current screen height. */
		uint32_t								m_ui32CurScrH;

		/** Rebuild the projection matrix? */
		LSBOOL									m_bRebuildProj;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Sets the width and height of the camera.
	 *
	 * \param _fWidth Width of the camera.
	 * \param _fHeight Height of the camera.
	 */
	LSE_INLINE void LSE_CALL CCamera2d::SetWidthHeight( LSREAL _fWidth, LSREAL _fHeight ) {
		m_bRebuildProj |= (m_fWidth != _fWidth) | (m_fHeight != _fHeight);
		m_fWidth = _fWidth;
		m_fHeight = _fHeight;
	}

	/**
	 * Gets the camera's width.
	 *
	 * \return Returns the camera's width.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera2d::Width() const {
		return m_fWidth;
	}

	/**
	 * Gets the camera's height.
	 *
	 * \return Returns the camera's height.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera2d::Height() const {
		return m_fHeight;
	}

	/**
	 * Sets the near and far of the camera.
	 *
	 * \param _fNear The near plane of the camera.
	 * \param _fFar The far plane of the camera.
	 */
	LSE_INLINE void LSE_CALL CCamera2d::SetNearFar( LSREAL _fNear, LSREAL _fFar ) {
		m_bRebuildProj |= (m_fNear != _fNear) | (m_fFar != _fFar);
		m_fNear = _fNear;
		m_fFar = _fFar;
	}

	/**
	 * Gets the camera's near distance.
	 *
	 * \return Returns the camera's near distance.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera2d::NearPlane() const {
		return m_fNear;
	}

	/**
	 * Gets the camera's far distance.
	 *
	 * \return Returns the camera's far distance.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera2d::FarPlane() const {
		return m_fFar;
	}

	/**
	 * Gets the view matrix.
	 *
	 * \return Returns the view matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera2d::ViewMatrix() const {
		return m_mViewMatrix;
	}

	/**
	 * Gets the inverse view matrix.
	 *
	 * \return Returns the inverse view matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera2d::InvViewMatrix() const {
		return m_mWorldTrans;
	}

	/**
	 * Gets the projection matrix.
	 *
	 * \return Returns the projection matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera2d::ProjMatrix() const {
		return m_mProjMatrix;
	}

	/**
	 * Gets the view-projection matrix.
	 *
	 * \return Returns the view-projection matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera2d::ViewProjMatrix() const {
		return m_mViewProjMatrix;
	}

	/**
	 * Gets the frustum of this camera.
	 *
	 * \return Returns the frustum of this camera.
	 */
	LSE_INLINE const CFrustum & LSE_CALL CCamera2d::Frustum() const {
		return m_fFrustum;
	}

	/**
	 * Gets a constant reference to the camera screen origin vector.
	 *
	 * \return Returns a constant reference to the camera screen origin vector.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CCamera2d::Origin() const {
		return m_vOrigin;
	}

	/**
	 * Sets the camera screen origin vector.
	 *
	 * \param _vOrigin The new origin vector indicating where [0,0] is on the screen.
	 */
	LSE_INLINE void LSE_CALL CCamera2d::SetOrigin( const CVector2 &_vOrigin ) {
		m_bRebuildProj = true;
		m_vOrigin = _vOrigin;
	}

}	// namespace lsr

#endif	// __LSR_CAMERA2D_H__
