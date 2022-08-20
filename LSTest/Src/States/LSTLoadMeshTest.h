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
 * Description: DESCRIPTION.
 */


#ifndef __LST_LOADMESHTEST_H__
#define __LST_LOADMESHTEST_H__

#include "../LSTTest.h"
#include "BoundingBoxes/LSPAabb.h"
#include "Models/LSDSharedModel.h"
#include "Scene/LSESceneManager3d.h"
#include "Sphere/LSMSphere.h"
#include "Streams/LSSTDExternalByteStream.h"


enum {
	LST_MAX_UVS							= 4,						/**< Maximum UV coordinates. */
	LST_MAX_INTERNAL_SKIN_WEIGHTS		= 256,						/**< Maximum skin weights internally. */
	LST_MAX_SKIN_WEIGHTS				= 4,						/**< Maximum skin weights exported. */
	LST_NORMALS							= (1 << 0),					/**< Normal data is included. */
	LST_COLORS							= (1 << 1),					/**< Color data is included. */
	LST_SKIN							= (1 << 2),					/**< Skin data is included. */
};

/**
 * Class CLoadMeshTest
 * \brief BRIEF
 *
 * Description: DESCRIPTION.
 */
class CLoadMeshTest : public CState {
public :
	// == Various constructors.
	LSE_CALLCTOR							CLoadMeshTest();
	LSE_CALLCTOR							~CLoadMeshTest();


	// == Functions.
	/**
	 * Called when the state is just created.  Use this to initialize resources.
	 *
	 * \param _pgGame Can be accessed for whatever game-specific information needs to be
	 *	obtained.
	 * \param _ui32State The current state identifier (the same class could
	 *	then be used for multiple states if they are similar enough).
	 * \param _ui32PrevState The identifier of the previous state.
	 * \param _uptrUser User-defined data passed to the state.  How this is used is entirely
	 *	defined by the state.  Examples of use:
	 * #1: Which map to load in a game state.
	 * #2: Which tutorial page to show in a tutorial state.
	 * #3: Used as a pointer to data containing a lot of state-specific information such as
	 *	map file to load, previous state of the player, etc.
	 */
	virtual void LSE_CALL					Init( CGame * _pgGame, int32_t _ui32State, int32_t _ui32PrevState, uintptr_t _uptrUser );

	/**
	 * Called when it is time for the state to die.  Free resources here.
	 *
	 * \param _pgGame Pointer to the game class which allows access to game-specific resources.
	 * \param _ui32NextState The identifier of the next state.
	 */
	virtual void LSE_CALL					Destroy( CGame * _pgGame, int32_t _ui32NextState );

	/**
	 * Called once every frame the state has focus to allow the state to render itself.
	 * If the state is in the background, this is not called.  Return false to exit the game.
	 *
	 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
	 * \return Return false if the game should close.
	 */
	virtual LSBOOL LSE_CALL					Draw( CGame * _pgGame );

	/**
	 * Gets the size of the heap to be allocated for this state, or 0 for none.
	 *
	 * \param _pgGame The game pointer.
	 * \param _ui32State The ID of the current state.
	 * \param _uptrUser User data passed to the state upon initialization.  Same value as what is passed in
	 *	Init().
	 * \return Return a non-zero value to indicate the size of the heap to be allocated for this class.
	 */
	virtual LSA_SIZE LSE_CALL				GetHeapSize( CGame * _pgGame, int32_t _ui32State, uintptr_t _uptrUser ) const;


protected :
	// == Types.
	struct LST_SHADER_KEY_PAIR {
		CSharedShaderPtr					sspShader;
		LSD_SHADERKEY						skKey;
	};


	// == Members.
	CShader									m_sShader;
	//CSharedShaderPtr						m_sspShader;
	CVector<LST_SHADER_KEY_PAIR>			m_vShaders;
	uint8_t *								m_pui8File;
	uintptr_t								m_uiptrSize;
	CVector<lsd::CMesh>						m_vMeshes;
	CVector<CMeshInstance>					m_vMeshInstances;

	CSharedModel							m_smModel;
	CSceneManager3d							m_sm3dScene;
	CSharedModelInstancePtr					m_smipModel;

	uint32_t								m_ui32Frames;
	uint64_t								m_ui64LastTime;

	// == Functions.
	// Creates the shader key and shader.
	LSE_INLINE void LSE_CALL				CreateShader( uint32_t _ui32ShaderIndex, const lsd::CMesh &_mMesh, const CModelMaterial &_mmMaterial );

	// Sets material information and activates textures.
	static void LSE_CALL					ActivateMaterial( const CModelMaterial &_mmMaterial, const LSD_SHADERKEY &_skKey );

	// Is the given sub-mesh alpha-blend-enabled?
	static LSE_INLINE LSBOOL LSE_CALL		IsAlpha( const lsd::CSubMesh &_smMesh ) {
		return _smMesh.Material()->IsAlpha();
	}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// == Functions.
// Creates the shader key and shader.
LSE_INLINE void LSE_CALL CLoadMeshTest::CreateShader( uint32_t _ui32ShaderIndex, const lsd::CMesh &_mMesh, const CModelMaterial &_mmMaterial ) {
	LSD_SHADERKEY skNewFlags;
	CSharedModel::SetShaderKeyFlags( skNewFlags, _mmMaterial );
	LSBOOL bUpdate = !m_vShaders[_ui32ShaderIndex].sspShader.Valid() ||
		m_vShaders[_ui32ShaderIndex].skKey.ui8DirLights != CGfx::GetDirLightTotal() ||
		m_vShaders[_ui32ShaderIndex].skKey.ui8PointLights != CGfx::GetPointLightTotal() ||
		m_vShaders[_ui32ShaderIndex].skKey.ui64Flags != skNewFlags.ui64Flags;
	if ( bUpdate ) {
		// Create the shader.
		LSD_SHADERNAMEKEYPAIR snkpPair;
		m_vShaders[_ui32ShaderIndex].skKey.ui8DirLights = static_cast<uint8_t>(CGfx::GetDirLightTotal());
		m_vShaders[_ui32ShaderIndex].skKey.ui8PointLights = static_cast<uint8_t>(CGfx::GetPointLightTotal());
		snkpPair.skShaderKey = CSharedModel::CreateStandardShaderKey( m_vShaders[_ui32ShaderIndex].skKey, _mMesh, _mmMaterial );
		snkpPair.nkNameKey = "default";
		m_vShaders[_ui32ShaderIndex].sspShader = CModelLib::ShaderManager().Translate( snkpPair );
	}
}

#endif	// __LST_LOADMESHTEST_H__
