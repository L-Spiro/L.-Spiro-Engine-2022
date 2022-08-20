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


#ifndef __LSE_SCENEMANAGER3D_H__
#define __LSE_SCENEMANAGER3D_H__

#include "../LSEEngine.h"
#include "../Camera/LSECamera.h"
#include "../Models/LSEModelInstance.h"
#include "Vector/LSTLVector.h"

namespace lse {

	/**
	 * Class CSceneManager3d
	 * \brief A 3D scene manager.
	 *
	 * Description: A 3D scene manager.  Manages all the objects in a single scene.  Multiple instances can be
	 *	created at once if multiple scenes are desired.
	 */
	class CSceneManager3d {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSceneManager3d();
		LSE_CALLCTOR							~CSceneManager3d();


		// == Functions.
		/**
		 * Creates a 3D model instance.
		 *
		 * \param _pcPath Name of the model to create.
		 * \return Returns a shared pointer to the created model instance.  If the model could not be created, the
		 *	shared pointer is invalid.
		 */
		CSharedModelInstancePtr LSE_CALL		CreateModelInstance( const char * _pcPath );

		/**
		 * Resets the scene completely to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Renders the scene with the default (main) camera.
		 */
		void LSE_CALL							Render();

		/**
		 * Renders the scene with the given camera.
		 *
		 * \param _scpCam Camera with which to render the scene.
		 */
		void LSE_CALL							Render( const CSharedCameraPtr & _scpCam );

		/**
		 * Sets the main camera.
		 *
		 * \param _scpCam Shared pointer to the main camera to set.
		 * \return Returns _scpCam.
		 */
		LSE_INLINE CSharedCameraPtr & LSE_CALL	SetMainCam( CSharedCameraPtr & _scpCam );

		/**
		 * Gets a reference to the main camera.
		 *
		 * \return Returns a reference to the main camera.
		 */
		LSE_INLINE CSharedCameraPtr & LSE_FCALL	MainCam();

		/**
		 * Gets a constant reference to the main camera.
		 *
		 * \return Returns a constant reference to the main camera.
		 */
		LSE_INLINE const CSharedCameraPtr & LSE_CALL
												MainCam() const;


	protected :
		// == Members.
		/** Actor allocator. */
		//CAllocator 								m_aActorPool;

		/** List of actors in the scene. */
		CVector<CSharedModelInstancePtr, uint32_t>
												m_vActors;

		/** The camera direction to use for the current render. */
		CVector3								m_vCamDir;

		/** The camera position to use for the current render. */
		CVector3								m_vCamPos;

		/** The frustum to use for the current render. */
		CFrustum								m_fFrustum;

		/** Current projection matrix. */
		CMatrix4x4								m_mProj;

		/** Current view matrix. */
		CMatrix4x4								m_mView;

		/** Current inverse view matrix. */
		CMatrix4x4								m_mInvView;

		/** Temporary list of meshes that are to be drawn. */
		CMeshInstanceArray						m_miaMainMeshList;

		/** The main camera.  Used when no camera is explicitly supplied on rendering. */
		CSharedCameraPtr						m_scpMainCam;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Sets the main camera.
	 *
	 * \param _scpCam Shared pointer to the main camera to set.
	 * \return Returns _scpCam.
	 */
	LSE_INLINE CSharedCameraPtr & LSE_CALL CSceneManager3d::SetMainCam( CSharedCameraPtr & _scpCam ) {
		m_scpMainCam = _scpCam;
		return m_scpMainCam;
	}

	/**
	 * Gets a reference to the main camera.
	 *
	 * \return Returns a reference to the main camera.
	 */
	LSE_INLINE CSharedCameraPtr & LSE_FCALL CSceneManager3d::MainCam() { return m_scpMainCam; }

	/**
	 * Gets a constant reference to the main camera.
	 *
	 * \return Returns a constant reference to the main camera.
	 */
	LSE_INLINE const CSharedCameraPtr & LSE_CALL CSceneManager3d::MainCam() const { return m_scpMainCam; }

}	// namespace lse

#endif	// __LSE_SCENEMANAGER3D_H__
