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
 * Description: Manages the creation of sprites, sharing textures between them, creating and assigning shaders, etc.
 */

#include "LSRSpriteManager.h"


namespace lsr {

	/** The sprite shader manager. */
	CSpriteShaderManager CSpriteManager::m_ssmShaderMan;

	/** Map of file names and their unique identifiers. */
	CLinearMap<CString, uint64_t> CSpriteManager::m_lmFilesToIds;

	/** Map of unique identifiers and their textures. */
	CLinearMap<uint64_t, CSharedTexture2dPtr> CSpriteManager::m_lmIdsToTextures;

	/** Map between texture pointers and texture properties. */
	CLinearMap<CTexture2d *, CSpriteManager::LSR_TEXTURE_PROPS> CSpriteManager::m_lmTexToProps;

	/** Tracks how many instances of this class there are so that the vertex buffer is only created once. */
	CAtomic::ATOM CSpriteManager::m_aInstances = 0;

	/** The default sprite sampler. */
	CSampler CSpriteManager::m_sSampler;

	/** The sprite raster mode. */
	LSG_RASTERIZER_STATE * CSpriteManager::m_prsRasterState = NULL;

	/** The depth mode. */
	LSG_DEPTH_STENCIL_STATE * CSpriteManager::m_pdssDepthState = NULL;

	/** The blend state for alpha objects. */
	LSG_BLEND_STATE * CSpriteManager::m_pbsAlphaBlendState = NULL;

	/** The blend state for non-alpha objects. */
	LSG_BLEND_STATE * CSpriteManager::m_pbsNonAlphaBlendState = NULL;

	// == Various constructors.
	LSE_CALLCTOR CSpriteManager::CSpriteManager() :
		m_ui32TotalSprites( 0 ),
		m_bInBatch( false ) {
		if ( CAtomic::InterlockedIncrement( m_aInstances ) == 1 ) {
			m_sSampler.SetAddressMode( LSG_TAM_CLAMP, LSG_TAM_CLAMP, LSG_TAM_CLAMP );
			m_sSampler.SetMaxAnisotropy( 1 );
			m_sSampler.SetFilter( LSG_F_MIN_MAG_MIP_POINT );

			LSG_RASTERIZER_DESC rdRastrDesc;
			rdRastrDesc = CGfx::DefaultRasterizerDesc( rdRastrDesc );
			rdRastrDesc.CullMode = CStd::ImpCast( rdRastrDesc.CullMode, LSG_CM_NONE );
			CGfx::CreateRasterizerState( rdRastrDesc, &m_prsRasterState );

			LSG_DEPTH_STENCIL_DESC dsdDepdthDesc;
			dsdDepdthDesc = CGfx::DefaultDepthStencilDesc( dsdDepdthDesc );
			dsdDepdthDesc.StencilEnable = false;
			dsdDepdthDesc.DepthEnable = false;
			CGfx::CreateDepthStencilState( dsdDepdthDesc, &m_pdssDepthState );

			LSG_BLEND_DESC bdBlendDec;
			bdBlendDec = CGfx::DefaultBlendDesc( bdBlendDec );
			CGfx::CreateBlendState( bdBlendDec, &m_pbsNonAlphaBlendState );
			bdBlendDec.RenderTarget[0].BlendEnable = true;
			bdBlendDec.RenderTarget[0].SrcBlend = CStd::ImpCast( bdBlendDec.RenderTarget[0].SrcBlend, LSG_BM_SRC_ALPHA );
			bdBlendDec.RenderTarget[0].DestBlend = CStd::ImpCast( bdBlendDec.RenderTarget[0].DestBlend, LSG_BM_INV_SRC_ALPHA );
			bdBlendDec.RenderTarget[0].SrcBlendAlpha = CStd::ImpCast( bdBlendDec.RenderTarget[0].SrcBlendAlpha, LSG_BM_SRC_ALPHA );
			bdBlendDec.RenderTarget[0].DestBlendAlpha = CStd::ImpCast( bdBlendDec.RenderTarget[0].DestBlendAlpha, LSG_BM_ONE );
			bdBlendDec.RenderTarget[0].RenderTargetWriteMask = LSG_CWM_RED | LSG_CWM_GREEN | LSG_CWM_BLUE;
			CGfx::CreateBlendState( bdBlendDec, &m_pbsAlphaBlendState );
		}
		static LSG_VERTEX_ELEMENT_DESC vedVertDesc[] = {
			{
				LSG_VE_R32G32B32_F,
				0,
				LSG_S_POSITION,
				0
			},
			{
				LSG_VE_R32G32_F,
				0,
				LSG_S_TEXCOORD,
				0
			},
		};
		CStd::MemCpy( m_vedVertDesc, vedVertDesc, sizeof( vedVertDesc ) );
		m_ui32VertDeclTotal = LSE_ELEMENTS( vedVertDesc );
		m_ui32Stride = sizeof( LSR_SPRITE_VERTEX );

		

		m_vbmMap.pvData = NULL;
		m_vbmMap.ui32Size = 0;
	}
	LSE_CALLCTOR CSpriteManager::~CSpriteManager() {
		if ( CAtomic::InterlockedDecrement( m_aInstances ) == 0 ) {
			m_lmFilesToIds.Reset();
			m_lmIdsToTextures.Reset();
			m_lmTexToProps.Reset();
			m_ssmShaderMan.Reset();
			m_sSampler.Reset();
			CGfx::ReleaseState( m_prsRasterState );
			CGfx::ReleaseState( m_pdssDepthState );
			CGfx::ReleaseState( m_pbsNonAlphaBlendState );
			CGfx::ReleaseState( m_pbsAlphaBlendState );
		}
	}

	// == Functions.
	/**
	 * Sets the maximum number of sprites in a batch.
	 *
	 * \param _ui32Total Total number of sprites in a batch.  When the batch is full all sprites in it are flushed (rendered).
	 */
	void LSE_CALL CSpriteManager::SetBatchSize( uint32_t _ui32Total ) {
		if ( m_ui32MaxSpritesPerBatch != _ui32Total ) {
			EndBatch();
			Flush();
			m_nbvbVertBuffer.Reset();
			for ( uint32_t I = m_nbvbVertBuffer.TotalBuffers(); I--; ) {
				if ( !m_nbvbVertBuffer.GetBuffer( I ).CreateVertexBuffer( NULL, m_vedVertDesc, m_ui32VertDeclTotal, false,
					m_ui32Stride, _ui32Total * 6, LSG_VBUT_DYNAMIC,
					LSG_PT_TRIANGLELIST ) ) {
					CStd::DebugPrintA( "CSpriteManager::SetBatchSize(): Failed to create vertex buffer of the appropriate size." );
					m_ui32MaxSpritesPerBatch = 0;
					m_nbvbVertBuffer.Reset();
					return;
				}

			}
			m_ui32MaxSpritesPerBatch = _ui32Total;
		}
	}

	/**
	 * Gets or creates (and returns) a texture by path.  If the texture has already been created the existing
	 *	texture is returned, otherwise the texture is created.
	 *
	 * \param _pcPath Path of the texture to get or create.
	 * \return Returns a shared texture pointer.  If the texture does not exist, the shared pointer is not valid.
	 */
	CSharedTexture2dPtr LSE_CALL CSpriteManager::GetOrCreateTextureByPath( const char * _pcPath ) {
		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcPath, szBuffer );
		CString sPath = szBuffer;
		CSharedTexture2dPtr st2pTex = TextureByPath( sPath.CStr() );
		if ( st2pTex.Valid() ) { return st2pTex; }

		// Have to load the texture.

		CImage iImage;
		if ( !iImage.LoadFile( sPath.CStr() ) ) {
			CStd::DebugPrintA( "CSpriteManager::GetOrCreateTextureByPath(): Failed to load sprite image.\r\n" );
			return CSharedTexture2dPtr();
		}
		uint64_t ui64Id = iImage.GetCrc32() | (static_cast<uint64_t>(iImage.GetCrc32Alt()) << 32);
		// The ID might already exist (just the path wasn't linked to it).
		st2pTex = TextureById( ui64Id );
		if ( !st2pTex.Valid() ) {
			// Create the texture.
			if ( !st2pTex.New() ) {
				CStd::DebugPrintA( "CSpriteManager::GetOrCreateTextureByPath(): Failed to create sprite texture (out of memory).\r\n" );
				return CSharedTexture2dPtr();
			}

			if ( !st2pTex->CreateTexture( &iImage, 1, LSG_TUT_STANDARD ) ) {
				CStd::DebugPrintA( "CSpriteManager::GetOrCreateTextureByPath(): Failed to create sprite texture.\r\n" );
				return CSharedTexture2dPtr();
			}


			// Register everything.
			if ( !m_lmFilesToIds.Insert( sPath, ui64Id ) ) {
				CStd::DebugPrintA( "CSpriteManager::GetOrCreateTextureByPath(): Failed to register sprite path ID.\r\n" );
				return CSharedTexture2dPtr();
			}
			LSR_TEXTURE_PROPS tpProps = {
				iImage.HasAlpha()
			};
			if ( !AddTexture( st2pTex, ui64Id, tpProps ) ) {
				m_lmFilesToIds.Remove( sPath );
				CStd::DebugPrintA( "CSpriteManager::GetOrCreateTextureByPath(): Failed to register sprite texture properties.\r\n" );
				return CSharedTexture2dPtr();
			}
		}
		return st2pTex;
	}

	/**
	 * Begins a batch of sprites.
	 */
	void LSE_CALL CSpriteManager::BeginBatch() {
		SetBatchSize( 256 );
		EndBatch();
		m_vbmMap = m_nbvbVertBuffer.Cur().Map( LSG_VBMA_WRITE_DISCARD );
		m_bInBatch = true;
	}

	/**
	 * Ends a batch of sprites.
	 */
	void LSE_CALL CSpriteManager::EndBatch() {
		if ( m_bInBatch ) {
			// Close the vertex buffer.
			m_nbvbVertBuffer.Cur().Unmap( m_vbmMap );
			Flush();	// Draw it.
			m_bInBatch = false;
		}
	}

	/**
	 * Adds a sprite to the batch.  Automatically calls BeginBatch() if it is not already between a BeginBatch() and EndBatch() pair.
	 *
	 * \param _sSprite The sprite to add to the batch.
	 */
	void LSE_CALL CSpriteManager::BatchSprite( const CSprite &_sSprite ) {
		if ( !m_ui32MaxSpritesPerBatch ) { return; }
		// Are we at our limit?
		if ( m_ui32TotalSprites == m_ui32MaxSpritesPerBatch ) {
			EndBatch();
		}
		if ( !m_bInBatch ) {
			BeginBatch();
		}
		if ( !m_vbmMap.pvData ) { return; }
		
		LSREAL fLeft = -_sSprite.Center().x;
		LSREAL fRight = _sSprite.TexRect().GetWidth() - _sSprite.Center().x;
		LSREAL fBottom = _sSprite.TexRect().GetHeight() - _sSprite.Center().y;
		LSREAL fTop = -_sSprite.Center().y;
		CVector4 vVerts[6] = {
			// ============
			// 1st Triangle
			// ============
			// Upper-left.
			CVector4( fLeft, fTop, _sSprite.GetDepth(), LSM_ONE ),
			// Upper-right.
			CVector4( fRight, fTop, _sSprite.GetDepth(), LSM_ONE ),
			// Bottom-left.
			CVector4( fLeft, fBottom, _sSprite.GetDepth(), LSM_ONE ),
			// ============
			// 2nd Triangle
			// ============
			// Upper-right.
			CVector4( fRight, fTop, _sSprite.GetDepth(), LSM_ONE ),
			// Bottom-right.
			CVector4( fRight, fBottom, _sSprite.GetDepth(), LSM_ONE ),
			// Bottom-left.
			CVector4( fLeft, fBottom, _sSprite.GetDepth(), LSM_ONE ),
		};
		CVector4 vVertsTransformed[6];
		CMatrix4x4::MultiplyVec4Batch( _sSprite.WorldTrans(), vVerts, 6, vVertsTransformed );
		LSR_SPRITE_VERTEX * psvDst = reinterpret_cast<LSR_SPRITE_VERTEX *>(m_vbmMap.pvData) + m_ui32TotalSprites * 6;
		
#define LSR_COPY_VERT( INDEX, U, V )												\
		psvDst->fPos[0] = vVertsTransformed[INDEX].x + _sSprite.Offset().x;			\
		psvDst->fPos[1] = vVertsTransformed[INDEX].y + _sSprite.Offset().y;			\
		psvDst->fPos[2] = vVertsTransformed[INDEX].z;								\
		psvDst->fUv[0] = U;															\
		psvDst->fUv[1] = V

		LSE_PREFETCH_LINE_WRITE( psvDst + 2 );
		LSR_COPY_VERT( 0, _sSprite.TexRect().Uvs().x, _sSprite.TexRect().Uvs().y );
		++psvDst;
		LSR_COPY_VERT( 1, _sSprite.TexRect().Uvs().z, _sSprite.TexRect().Uvs().y );
		++psvDst;
		LSR_COPY_VERT( 2, _sSprite.TexRect().Uvs().x, _sSprite.TexRect().Uvs().w );
		++psvDst;
		LSR_COPY_VERT( 3, _sSprite.TexRect().Uvs().z, _sSprite.TexRect().Uvs().y );
		++psvDst;
		LSR_COPY_VERT( 4, _sSprite.TexRect().Uvs().z, _sSprite.TexRect().Uvs().w );
		++psvDst;
		LSR_COPY_VERT( 5, _sSprite.TexRect().Uvs().x, _sSprite.TexRect().Uvs().w );
#undef LSR_COPY_VERT
		++m_ui32TotalSprites;
	}

	/**
	 * Gets a texture based off its ID.
	 *
	 * \param _ui64Id ID of the texture to get.
	 * \return Returns a shared texture pointer.  If the texture does not exist, the shared pointer is not valid.
	 */
	CSharedTexture2dPtr LSE_CALL CSpriteManager::TextureById( uint64_t _ui64Id ) {
		uint32_t ui32Index;
		if ( m_lmIdsToTextures.ItemExists( _ui64Id, ui32Index ) ) {
			return m_lmIdsToTextures.GetByIndex( ui32Index );
		}
		return CSharedTexture2dPtr();
	}

	/**
	 * Gets a texture based off its file path.
	 *
	 * \param _pcPath Path of the texture to get.
	 * \return Returns a shared texture pointer.  If the texture does not exist, the shared pointer is not valid.
	 */
	CSharedTexture2dPtr LSE_CALL CSpriteManager::TextureByPath( const CString &_sPath ) {
		uint32_t ui32Index;
		if ( m_lmFilesToIds.ItemExists( _sPath, ui32Index ) ) {
			return TextureById( m_lmFilesToIds.GetByIndex( ui32Index ) );
		}
		return CSharedTexture2dPtr();
	}

	/**
	 * Gets texture properties by texture.
	 *
	 * \param _st2pTex Shared texture pointer whose properties are to be gotten.
	 * \return Returns the texture properties associated with the given ID or a default set of properties if no texture exists.
	 */
	CSpriteManager::LSR_TEXTURE_PROPS LSE_CALL CSpriteManager::TexturePropsByTexture( const CSharedTexture2dPtr &_st2pTex ) {
		uint32_t ui32Index;
		if ( m_lmTexToProps.ItemExists( const_cast<CTexture2d *const>(_st2pTex.Get()), ui32Index ) ) {
			return m_lmTexToProps.GetByIndex( ui32Index );
		}
		LSR_TEXTURE_PROPS tpRet = {
			true,							// bHasAlpha
		};
		return tpRet;
	}

	/**
	 * Draws all the sprites currently in the current vertex buffer.
	 */
	void LSE_CALL CSpriteManager::Flush() {
		if ( m_ui32TotalSprites ) {
			// Shader and texture(s) should already be set.
			m_nbvbVertBuffer.Cur().SetStream( 0 );
			CGfx::Render( NULL,
				0, m_ui32TotalSprites * 2 );
			m_ui32TotalSprites = 0;
			m_nbvbVertBuffer.Swap();
		}
	}

}	// namespace lsr
