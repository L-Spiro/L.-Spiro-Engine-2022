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
 * Description: A 3D scene manager.  Manages all the objects in a single scene.  Multiple instances can be
 *	created at once if multiple scenes are desired.
 */

#include "LSESceneManager3d.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CSceneManager3d::CSceneManager3d() {
		//m_aActorPool.SetAllocator( CMemLib::GetAllocatorD( 1024 * 1024, true ) );
	}
	LSE_CALLCTOR CSceneManager3d::~CSceneManager3d() {
		Reset();
		//CMemLib::DestroyAllocator( m_aActorPool.GetAllocator() );
	}

	// == Functions.
	/**
	 * Creates a 3D model instance.
	 *
	 * \param _pcPath Name of the model to create.
	 * \return Returns a shared pointer to the created model instance.  If the model could not be created, the
	 *	shared pointer is invalid.
	 */
	CSharedModelInstancePtr LSE_CALL CSceneManager3d::CreateModelInstance( const char * _pcPath ) {
		//CSharedModelInstancePtr smipRet( &m_aActorPool );
		CSharedModelInstancePtr smipRet;
		smipRet.New();
		if ( !smipRet.Valid() ) { return CSharedModelInstancePtr(); }
		if ( !smipRet->CreateModelInstance( _pcPath ) ) { return CSharedModelInstancePtr(); }
		if ( !m_vActors.Push( smipRet ) ) { return CSharedModelInstancePtr(); }
		return smipRet;
	}

	/**
	 * Resets the scene completely to scratch.
	 */
	void LSE_CALL CSceneManager3d::Reset() {
		m_vActors.Reset();
		m_scpMainCam.Reset();
		m_miaMainMeshList.Reset();
	}

	/**
	 * Renders the scene with the default (main) camera.
	 */
	void LSE_CALL CSceneManager3d::Render() {
		Render( m_scpMainCam );
	}

	/**
	 * Renders the scene with the given camera.
	 *
	 * \param _scpCam Camera with which to render the scene.
	 */
	void LSE_CALL CSceneManager3d::Render( const CSharedCameraPtr & _scpCam ) {
		if ( !_scpCam.Valid() ) {
			CStd::DebugPrintA( "Camera is invalid.\r\n" );
			return;
		}
		const_cast<CCamera &>(*_scpCam).Update();
		// Update values used for the rest of the render.
		m_vCamDir = _scpCam->GetOrientation().GetForward();
		m_vCamPos = _scpCam->GetOrientation().Pos();
		// Later, add debug features to optionally update the frustum.
		m_fFrustum = _scpCam->Frustum();
		m_mProj = _scpCam->ProjMatrix();
		m_mView = _scpCam->ViewMatrix();
		m_mInvView = _scpCam->InvViewMatrix();


		m_miaMainMeshList.ResetNoDealloc();

		// Propagate objects and add the ones in view.
		for ( uint32_t I = m_vActors.Length(); I--; ) {
			m_vActors[I]->Propagate();
			if ( m_vActors[I]->EntityType() & LSE_ET_MODEL ) {
				lse::CModelInstance * pmiThis = static_cast<lse::CModelInstance *>(m_vActors[I].Get());
				for ( uint32_t J = pmiThis->MeshList().Length(); J--; ) {
					if ( !m_miaMainMeshList.Push( pmiThis->MeshList()[J] ) ) {
						CStd::DebugPrintA( "Failed to gather renderables.\r\n" );
						return;
					}
				}
				//if ( static_cast<lse::CModelInstance *>(m_vActors[I].Get())->AddMeshInFrustum( m_fFrustum, m_miaMainMeshList ) ) {
				//}
			}
		}


		// Temp.
		CGfx::SetProjMatrix( m_mProj );
		CGfx::SetViewMatrix( m_mView );
		for ( uint32_t I = 0; I < m_miaMainMeshList.Length(); ++I ) {
			for ( uint32_t J = m_miaMainMeshList[I]->SubMeshes().Length(); J--; ) {
				if ( m_miaMainMeshList[I]->SubMeshes()[J].Material().IsAlpha() ) { continue; }
				m_miaMainMeshList[I]->RenderPart( J );
			}
		}
		for ( uint32_t I = 0; I < m_miaMainMeshList.Length(); ++I ) {
			for ( uint32_t J = m_miaMainMeshList[I]->SubMeshes().Length(); J--; ) {
				if ( !m_miaMainMeshList[I]->SubMeshes()[J].Material().IsAlpha() ) { continue; }
				m_miaMainMeshList[I]->RenderPart( J );
			}
		}
	}

}	// namespace lse
