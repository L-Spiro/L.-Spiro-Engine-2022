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
 * Description: An FBX scene.
 */


#ifndef __LSX_SCENE_H__
#define __LSX_SCENE_H__

#include "../LSXFbxConverter.h"
#include "LSXAnimation.h"
#include "LSXMaterial.h"
#include "LSXMeshInstance.h"
#include "LSXSkeleton.h"
#include "LSXTexture.h"
#include "Models/LSDModelAttributes.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** File header for .LSM files. */
#define LSX_LSM_HEADER							LSD_MA_MODEL_HEADER

/** LSM version. */
#define LSX_LSM_VERSION							LSD_MA_MODEL_VERSION

/** File header for .LSA files. */
#define LSX_ANIM_HEADER							LSD_MA_ANIM_HEADER

namespace lsx {

	/**
	 * Class CScene
	 * \brief An FBX scene.
	 *
	 * Description: An FBX scene.
	 */
	class CScene {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CScene();
		LSE_CALLCTOR							~CScene();


		// == Functions.
		/**
		 * Load a scene.  The path must be in UTF-8 format.  The existing file, if any, is unloaded.
		 *
		 * \param _pcPath The full path to the file to load, in UTF-8 format.
		 * \param _pcPassword The file password, if any.
		 * \param _ui32Options Options.
		 * \return Returns true if the file was loaded.
		 */
		LSBOOL LSE_CALL							LoadFile( const LSUTF8 * _pcPath, const LSCHAR * _pcPassword, LSUINT32 _ui32Options = 0UL );

		/**
		 * Save the data to the given file.
		 *
		 * \param _pcPath File to which to save the scene data.
		 * \return Returns an error code indicating success or failure.
		 */
		LSSTD_ERRORS LSE_CALL					SaveTo( const LSUTF8 * _pcPath );

		/**
		 * Reset everything to scratch.
		 */
		LSVOID LSE_CALL							Reset();

		/**
		 * Loads mesh data.
		 *
		 * \return Returns true if mesh data was loaded.
		 */
		LSBOOL LSE_CALL							LoadMeshes();


	protected :
		// == Members.
		/** The SDK manager. */
		FbxManager *							m_pfsmManager;

		/** The file (scene) we imported. */
		FbxScene *								m_pfsScene;

		/** The scene skeleton. */
		CSkeleton								m_sSkeleton;

		/** Loaded meshes. */
		CVector<CMeshInstance, LSUINT32>		m_vMeshes;

		/** Scene animation. */
		CAnimation								m_aAnimation;

		/** List of referenced materials. */
		CSet<FbxSurfaceMaterial *>				m_sReferencedMaterials;

		/** Referenced materials in our format. */
		CVector<CMaterial, LSUINT32>			m_vMaterials;

		/** List of finalized textures. */
		CTextureArray							m_taTextures;

		/** List of referenced textures. */
		CTexture::LSX_TEXTURE_REF_SET			m_trsReferencedTextures;

		/** Referenced normal maps and bump maps. */
		CTexture::LSX_TEXTURE_REF_SET			m_trsReferencedNormalTextures;


		// == Functions.
		/**
		 * Loads referenced materials.
		 *
		 * \return Returns true if material data was loaded.
		 */
		LSBOOL LSE_CALL							LoadMaterials();

		/**
		 * Loads and finalizes all of the referenced textures.
		 *
		 * \return Returns an error code indicating success or failure.
		 */
		LSSTD_ERRORS LSE_CALL					FinalizeTextures();

		/**
		 * Exports a node and all of its children recursively.  The exported data includes the node ID, its type, and
		 *	full transform information, including pivots.
		 *
		 * \param _pfNode The node to export.
		 * \param _bBuffer The buffer to which to export the node.
		 * \return Returns false if there is not enough memory to write the full node tree to the buffer.
		 */
		LSBOOL LSE_CALL LSE_CALL				WriteNode( FbxNode * _pfNode, lsc::CMisc::LSC_BUFFER &_bBuffer );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsx

#endif	// __LSX_SCENE_H__
