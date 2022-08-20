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

#include "LSRCamera2d.h"
#include "Gfx/LSGGfx.h"


namespace lsr {

	// == Various constructors.
	LSE_CALLCTOR CCamera2d::CCamera2d() :
		m_vOrigin( CVector2( LSM_ZERO, LSM_ZERO ) ),
		m_fWidth( LSM_ZERO ),
		m_fHeight( LSM_ZERO ),
		m_fNear( LSM_ZERO ),
		m_fFar( LSM_ZERO ),
		m_ui32CurScrW( 0 ),
		m_ui32CurScrH( 0 ),
		m_bRebuildProj( true ) {
		m_ui32EntityType |= LSR_ET_CAMERA2D;
		m_mViewMatrix.Identity();
	}

	// == Functions.
	/**
	 * Causes all matrices to be built.
	 */
	void LSE_CALL CCamera2d::Update() {
		if ( m_bRebuildProj ) {
			CGfx::OrthoOffCenter( m_mProjMatrix,
				-m_vOrigin.x, m_fWidth - m_vOrigin.x,
				m_fHeight - m_vOrigin.y, -m_vOrigin.y,
				m_fNear, m_fFar );
			m_bRebuildProj = false;
		}
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

			CVector2 vR, vU;
			m_oOrientation2d.GetVectors( vR, vU );

			m_mViewMatrix( 3, 0 ) = -(vR.Dot( m_oOrientation2d.Pos() ));
			m_mViewMatrix( 3, 1 ) = -(vU.Dot( m_oOrientation2d.Pos() ));
			m_mViewMatrix( 3, 2 ) = -Depth();
			//m_mViewMatrix = m_mWorldTrans.InverseAccurate();

			m_mViewProjMatrix = m_mViewMatrix * m_mProjMatrix;

			CGfx::ExtractFrustumPlanes( m_mViewProjMatrix, m_fFrustum, true );
		}
	}

}	// namespace lsr
