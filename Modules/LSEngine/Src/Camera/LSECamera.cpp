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

#include "LSECamera.h"
#include "Gfx/LSGGfx.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CCamera::CCamera() :
		m_ctCamType( LSE_CT_PERSP ),
		m_fNear( static_cast<LSREAL>(0.1) ),
		m_fFar( static_cast<LSREAL>(1024.0) ),
		m_fFov( static_cast<LSREAL>(54.43) * LSM_DEG2RAD ),
		m_fAspect( static_cast<LSREAL>(CGfx::ScreenWidth()) / static_cast<LSREAL>(CGfx::ScreenHeight()) ),
		m_fMinX( LSM_ZERO ),
		m_fMinY( LSM_ZERO ),
		m_fMaxX( LSM_ZERO ),
		m_fMaxY( LSM_ZERO ) {
		m_mViewMatrix.Identity();
		m_ui32EntityType |= LSE_ET_CAM;

		SetPerspParms( m_fFov,
			m_fAspect,
			m_fNear,
			m_fFar );
		m_mViewProjMatrix = m_mViewMatrix * m_mProjMatrix;
	}
	LSE_CALLCTOR CCamera::~CCamera() {
	}

	// == Functions.
	/**
	 * Sets the projection parameters for a 3D camera.  Creates a right-handed projection matrix.
	 *
	 * \param _fFov Field of view in the Y direction in radians.
	 * \param _fAspect Aspect ratio, defined as view space width divided by height.
	 * \param _fNearPlane Z-value of the near view-plane.
	 * \param _fFarPlane Z-value of the far view-plane.
	 */
	void LSE_CALL CCamera::SetPerspParms( LSREAL _fFov, LSREAL _fAspect,
		LSREAL _fNearPlane, LSREAL _fFarPlane ) {
		m_fFov = _fFov;
		m_fAspect = _fAspect;
		m_fNear = _fNearPlane;
		m_fFar = _fFarPlane;
		m_ctCamType = LSE_CT_PERSP;
		m_ui32CurScrW = CGfx::ScreenWidth();
		m_ui32CurScrH = CGfx::ScreenHeight();

		// Create the projection matrix.
		CGfx::PerspectiveFov( m_mProjMatrix,
			m_fFov,
			m_fAspect,
			m_fNear,
			m_fFar );
		/*m_mProjMatrix.PerspectiveFovRH( m_fFov,
			m_fAspect,
			m_fNear,
			m_fFar );*/
		m_bWorldDirty = true;
	}

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
	void LSE_CALL CCamera::SetOrthoParms( LSREAL _fLeft, LSREAL _fTop,
		LSREAL _fRight, LSREAL _fBottom, LSREAL _fNearPlane, LSREAL _fFarPlane ) {
		m_ctCamType = LSE_CT_ORTHO;
		m_ui32CurScrW = CGfx::ScreenWidth();
		m_ui32CurScrH = CGfx::ScreenHeight();
		m_mProjMatrix.OrthoOffCenterRH( _fLeft, _fRight,
			_fBottom, _fTop,
			_fNearPlane, _fFarPlane );
		m_bWorldDirty = true;
	}

	/**
	 * Causes all matrices to be built.
	 */
	void LSE_CALL CCamera::Update() {
		AdjustToCurScreenSize();
		if ( m_bWorldDirty ) {
			UndirtyWorld();
			m_mViewMatrix( 0, 0 ) = m_mWorldTrans( 0, 0 );
			m_mViewMatrix( 0, 1 ) = m_mWorldTrans( 1, 0 );
			m_mViewMatrix( 0, 2 ) = m_mWorldTrans( 2, 0 );

			m_mViewMatrix( 1, 0 ) = m_mWorldTrans( 0, 1 );
			m_mViewMatrix( 1, 1 ) = m_mWorldTrans( 1, 1 );
			m_mViewMatrix( 1, 2 ) = m_mWorldTrans( 2, 1 );

			m_mViewMatrix( 2, 0 ) = m_mWorldTrans( 0, 2 );
			m_mViewMatrix( 2, 1 ) = m_mWorldTrans( 1, 2 );
			m_mViewMatrix( 2, 2 ) = m_mWorldTrans( 2, 2 );

			m_mViewMatrix( 3, 0 ) = -(m_oOrientation.GetRight().Dot( m_oOrientation.Pos() ));
			m_mViewMatrix( 3, 1 ) = -(m_oOrientation.GetUp().Dot( m_oOrientation.Pos() ));
			m_mViewMatrix( 3, 2 ) = -(m_oOrientation.GetForward().Dot( m_oOrientation.Pos() ));

			m_mViewProjMatrix = m_mViewMatrix * m_mProjMatrix;

			CGfx::ExtractFrustumPlanes( m_mViewProjMatrix, m_fFrustum, true );
		}
	}

	/**
	 * Adjusts the camera to the current screen resolution.
	 */
	void LSE_CALL CCamera::AdjustToCurScreenSize() {
		if ( !CGfx::ScreenHeight() ) { return; }
		if ( m_ui32CurScrW == CGfx::ScreenWidth() && m_ui32CurScrH == CGfx::ScreenHeight() ) { return; }

		m_fAspect = static_cast<LSREAL>(CGfx::ScreenWidth()) / static_cast<LSREAL>(CGfx::ScreenHeight());

		if ( m_ctCamType == LSE_CT_PERSP ) {
			SetPerspParms( m_fFov,
				m_fAspect,
				m_fNear,
				m_fFar );
		}
		

	}

}	// namespace lse
