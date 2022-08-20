/**
 * Copyright L. Spiro 2011
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A standard camera.
 */


#ifndef __LSE_CAMERA_H__
#define __LSE_CAMERA_H__

#include "../LSEEngine.h"
#include "../Entity/LSEActor.h"
#include "Frustum/LSMFrustum.h"

namespace lse {

	// == Enumerations.
	/** Camera types. */
	enum LSE_CAM_TYPE {
		LSE_CT_PERSP,							/**< A perspective camera. */
		LSE_CT_ORTHO,							/**< An orthogonal camera. */
	};

	/**
	 * Class CCamera
	 * \brief A standard camera.
	 *
	 * Description: A standard camera.
	 */
	class CCamera : public CActor {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CCamera();
		virtual LSE_CALLCTOR					~CCamera();


		// == Functions.
		/**
		 * Sets the projection parameters for a 3D camera.  Creates a right-handed projection matrix.
		 *
		 * \param _fFov Field of view in the Y direction in radians.
		 * \param _fAspect Aspect ratio, defined as view space width divided by height.
		 * \param _fNearPlane Z-value of the near view-plane.
		 * \param _fFarPlane Z-value of the far view-plane.
		 */
		void LSE_CALL							SetPerspParms( LSREAL _fFov, LSREAL _fAspect,
			LSREAL _fNearPlane, LSREAL _fFarPlane );

		/**
		 * Sets the orthogonal projection parameters for a 2D camera.  Creates a right-handed orthogonal matrix.
		 *
		 * \param _fLeft The left of the view.
		 * \param _fTop The top of the view.
		 * \param _fRight The right of the view.
		 * \param _fBottom The bottom of the view.
		 * \param _fNearPlane The near.
		 * \param _fFarPlane The far.
		 */
		void LSE_CALL							SetOrthoParms( LSREAL _fLeft, LSREAL _fTop,
			LSREAL _fRight, LSREAL _fBottom, LSREAL _fNearPlane = -LSM_ONE, LSREAL _fFarPlane = LSM_ONE );

		/**
		 * Causes all matrices to be built.
		 */
		void LSE_CALL							Update();

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
		 * Gets the camera's field-of-view.
		 *
		 * \return Returns the camera's field-of-view.
		 */
		LSE_INLINE LSREAL LSE_CALL				Fov() const;

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
		 * Gets the camera's aspect ratio.
		 *
		 * \return Returns the camera's aspect ratio.
		 */
		LSE_INLINE LSREAL LSE_CALL				Aspect() const;


	protected :
		// == Members.
		/** The view matrix. */
		CMatrix4x4								m_mViewMatrix;

		/** The projection matrix. */
		CMatrix4x4								m_mProjMatrix;

		/** The view-projection matrix. */
		CMatrix4x4								m_mViewProjMatrix;

		/** The view frustum. */
		CFrustum								m_fFrustum;

		/** The camera type. */
		LSE_CAM_TYPE							m_ctCamType;

		/** The near plane. */
		LSREAL									m_fNear;

		/** The far plane. */
		LSREAL									m_fFar;

		/** Field of view. */
		LSREAL									m_fFov;

		/** Aspect ratio. */
		LSREAL									m_fAspect;

		/** Minimum X for orthogonal projections. */
		LSREAL									m_fMinX;

		/** Minimum Y for orthogonal projections. */
		LSREAL									m_fMinY;

		/** Maximum X for orthogonal projections. */
		LSREAL									m_fMaxX;

		/** Maximum Y for orthogonal projections. */
		LSREAL									m_fMaxY;

		/** Current screen width. */
		uint32_t								m_ui32CurScrW;

		/** Current screen height. */
		uint32_t								m_ui32CurScrH;


		// == Functions.
		/**
		 * Adjusts the camera to the current screen resolution.
		 */
		void LSE_CALL							AdjustToCurScreenSize();


	private :
		typedef CActor							Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * A shared pointer to a camera.
	 */
	typedef CSharedPtr<CCamera>					CSharedCameraPtr;

	// == Functions.
	/**
	 * Gets the view matrix.
	 *
	 * \return Returns the view matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera::ViewMatrix() const {
		return m_mViewMatrix;
	}

	/**
	 * Gets the inverse view matrix.
	 *
	 * \return Returns the inverse view matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera::InvViewMatrix() const {
		return m_mWorldTrans;
	}

	/**
	 * Gets the projection matrix.
	 *
	 * \return Returns the projection matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera::ProjMatrix() const {
		return m_mProjMatrix;
	}

	/**
	 * Gets the view-projection matrix.
	 *
	 * \return Returns the view-projection matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CCamera::ViewProjMatrix() const {
		return m_mViewProjMatrix;
	}

	/**
	 * Gets the frustum of this camera.
	 *
	 * \return Returns the frustum of this camera.
	 */
	LSE_INLINE const CFrustum & LSE_CALL CCamera::Frustum() const {
		return m_fFrustum;
	}

	/**
	 * Gets the camera's field-of-view.
	 *
	 * \return Returns the camera's field-of-view.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera::Fov() const {
		return m_fFov;
	}

	/**
	 * Gets the camera's near distance.
	 *
	 * \return Returns the camera's near distance.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera::NearPlane() const {
		return m_fNear;
	}

	/**
	 * Gets the camera's far distance.
	 *
	 * \return Returns the camera's far distance.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera::FarPlane() const {
		return m_fFar;
	}

	/**
	 * Gets the camera's aspect ratio.
	 *
	 * \return Returns the camera's aspect ratio.
	 */
	LSE_INLINE LSREAL LSE_CALL CCamera::Aspect() const {
		return m_fAspect;
	}

}	// namespace lse

#endif	// __LSE_CAMERA_H__
