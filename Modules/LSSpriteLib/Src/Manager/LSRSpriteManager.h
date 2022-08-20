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


#ifndef __LSR_SPRITEMANAGER_H__
#define __LSR_SPRITEMANAGER_H__

#include "../LSRSpriteLib.h"
#include "../Sprite/LSRAnimatedSprite.h"
#include "../Sprite/LSRSprite.h"
#include "LSRSpriteProperties.h"
#include "LSRSpriteShaderManager.h"
#include "Sampler/LSGSampler.h"
#include "VertexBuffer/LSGNBufferVertexBuffer.h"

namespace lsr {

	/**
	 * Class CSpriteManager
	 * \brief Manages the creation of sprites, sharing textures between them, creating and assigning shaders, etc.
	 *
	 * Description: Manages the creation of sprites, sharing textures between them, creating and assigning shaders, etc.
	 */
	class CSpriteManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSpriteManager();
		LSE_CALLCTOR							~CSpriteManager();


		// == Types.
		/** The system-provided vertex type, which consists of just a position and UV coordinates. */
#pragma pack( push, 1 )
		typedef struct LSR_SPRITE_VERTEX {
			float								fPos[3];								/**< Vertex position. */
			float								fUv[2];									/**< UV coordinates. */
		} * LPLSR_SPRITE_VERTEX, * const LPCLSR_SPRITE_VERTEX;
#pragma pack( pop )


		// == Functions.
		/**
		 * Creates a sprite or animated sprite given an image file name.
		 *
		 * \param _pcPath Path to the file to load.
		 * \param _spProps Sprite properties.
		 * \return Returns a shared sprite pointer which will be invalid upon failure (file not found etc.)
		 */
		template <typename _tRetType = CSharedSpritePtr>
		_tRetType LSE_CALL						CreateSprite( const char * _pcPath, const LSR_SPRITE_PROPERTIES &_spProps ) {
			// Load the given image file.
			CSharedTexture2dPtr st2pTex = GetOrCreateTextureByPath( _pcPath );
			if ( !st2pTex.Valid() ) { return _tRetType(); }

			return CreateSprite<_tRetType>( st2pTex, _spProps );
		}

		/**
		 * Creates a sprite or animated sprite given an image.
		 *
		 * \param _iImage Image for the sprite.
		 * \param _spProps Sprite properties.
		 * \return Returns a shared animated sprite pointer which will be invalid upon failure (file not found etc.)
		 */
		template <typename _tRetType = CSharedSpritePtr>
		_tRetType LSE_CALL						CreateSprite( const CImage &_iImage, const LSR_SPRITE_PROPERTIES &_spProps ) {
			uint64_t ui64Id = _iImage.GetCrc32() | (static_cast<uint64_t>(_iImage.GetCrc32Alt()) << 32);
			CSharedTexture2dPtr st2pTex = TextureById( ui64Id );
			if ( !st2pTex.Valid() ) {
				// Create the texture.
				if ( !st2pTex.New() ) {
					CStd::DebugPrintA( "CSpriteManager::CreateSprite(): Failed to create sprite texture (out of memory).\r\n" );
					return _tRetType();
				}

				if ( !st2pTex->CreateTexture( &_iImage, 1, LSG_TUT_SETONLY ) ) {
					CStd::DebugPrintA( "CSpriteManager::CreateSprite(): Failed to create sprite texture.\r\n" );
					return _tRetType();
				}

				// Register it.
				LSR_TEXTURE_PROPS tpProps = {
					_iImage.HasAlpha()
				};
				if ( !AddTexture( st2pTex, ui64Id, tpProps ) ) {
					CStd::DebugPrintA( "CSpriteManager::CreateSprite(): Failed to register sprite texture properties.\r\n" );
					return _tRetType();
				}

			}
			return CreateSprite<_tRetType>( st2pTex, _spProps );
		}

		/**
		 * Creates an animated sprite given an image file name.
		 *
		 * \param _pcPath Path to the file to load.
		 * \param _spProps Sprite properties.
		 * \return Returns a shared sprite pointer which will be invalid upon failure (file not found etc.)
		 */
		LSE_INLINE CSharedAnimatedSpritePtr LSE_CALL
												CreateAnimSprite( const char * _pcPath, const LSR_SPRITE_PROPERTIES &_spProps );

		/**
		 * Creates an animated sprite given an image.
		 *
		 * \param _iImage Image for the sprite.
		 * \param _spProps Sprite properties.
		 * \return Returns a shared animated sprite pointer which will be invalid upon failure (file not found etc.)
		 */
		LSE_INLINE CSharedAnimatedSpritePtr LSE_CALL
												CreateAnimSprite( const CImage &_iImage, const LSR_SPRITE_PROPERTIES &_spProps );

		/**
		 * Gets or creates (and returns) a texture by path.  If the texture has already been created the existing
		 *	texture is returned, otherwise the texture is created.
		 *
		 * \param _pcPath Path of the texture to get or create.
		 * \return Returns a shared texture pointer.  If the texture does not exist, the shared pointer is not valid.
		 */
		CSharedTexture2dPtr LSE_CALL			GetOrCreateTextureByPath( const char * _pcPath );

		/**
		 * Sets the maximum number of sprites in a batch.
		 *
		 * \param _ui32Total Total number of sprites in a batch.  When the batch is full all sprites in it are flushed (rendered).
		 */
		void LSE_CALL							SetBatchSize( uint32_t _ui32Total );

		/**
		 * Begins a batch of sprites.
		 */
		void LSE_CALL							BeginBatch();

		/**
		 * Ends a batch of sprites.
		 */
		void LSE_CALL							EndBatch();

		/**
		 * Adds a sprite to the batch.  Automatically calls BeginBatch() if it is not already between a BeginBatch() and EndBatch() pair.
		 *
		 * \param _sSprite The sprite to add to the batch.
		 */
		void LSE_CALL							BatchSprite( const CSprite &_sSprite );

		/**
		 * Gets a constant reference to the sprite sampler.  The sprite sampler uses clamping and a point filter.
		 *
		 * \return Returns a constant reference to the sprite sampler.
		 */
		static LSE_INLINE CSampler & LSE_CALL	Sampler();

		/**
		 * Gets the raster state.
		 *
		 * \return Returns the raster state commonly used for sprites.
		 */
		static LSE_INLINE LSG_RASTERIZER_STATE * LSE_CALL
												RasterState();

		/**
		 * Gets the depth/stencil state.
		 *
		 * \return Returns the depth/stencil state commonly used for sprites.
		 */
		static LSE_INLINE LSG_DEPTH_STENCIL_STATE * LSE_CALL
												DepthStencilState();

		/**
		 * Gets the blend state for opaque sprites.
		 *
		 * \return Returns the blend state for opaque sprites commonly used for sprites.
		 */
		static LSE_INLINE LSG_BLEND_STATE * LSE_CALL
												OpaqueBlendState();

		/**
		 * Gets the blend state for translucent sprites.
		 *
		 * \return Returns the blend state for translucent sprites commonly used for sprites.
		 */
		static LSE_INLINE LSG_BLEND_STATE * LSE_CALL
												AlphaBlendState();

		/**
		 * Applies the raster, depth/stencil, and blend states commonly used for sprite rendering.
		 *
		 * \param _bAlpha If true, blending is enabled, otherwise it is not.
		 */
		static LSE_INLINE void LSE_CALL		SetStates( LSBOOL _bAlpha = true );


	protected :
		// == Types.
		/** Texture properties. */
		typedef struct LSR_TEXTURE_PROPS {
			LSBOOL								bHasAlpha;								/**< Indicates whether or not there are any non-one alpha pixels in a texture. */
		} * LPLSR_TEXTURE_PROPS, * const LPCLSR_TEXTURE_PROPS;

		// == Members.
		/** Maximum number of sprites that can be batched. */
		uint32_t								m_ui32MaxSpritesPerBatch;

		/** The vertex buffers. */
		CNBufferVertexBuffer<4>					m_nbvbVertBuffer;

		/** The default vertex declaration. */
		LSG_VERTEX_ELEMENT_DESC					m_vedVertDesc[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/** The total number of vertex declarations. */
		uint32_t								m_ui32VertDeclTotal;

		/** The stride of the vertices. */
		uint32_t								m_ui32Stride;

		/** Total sprites in the current vertex buffer. */
		uint32_t								m_ui32TotalSprites;

		/** Are we in a batch? */
		LSBOOL									m_bInBatch;

		/** The mapped vertex data. */
		LSG_VERTEX_BUFFER_MAP					m_vbmMap;

		/** The sprite shader manager. */
		static CSpriteShaderManager				m_ssmShaderMan;

		/** Map of file names and their unique identifiers. */
		static CLinearMap<CString, uint64_t>	m_lmFilesToIds;

		/** Map of unique identifiers and their textures. */
		static CLinearMap<uint64_t, CSharedTexture2dPtr>
												m_lmIdsToTextures;

		/** Map between texture pointers and texture properties. */
		static CLinearMap<CTexture2d *, LSR_TEXTURE_PROPS>
												m_lmTexToProps;

		/** Tracks how many instances of this class there are so that the vertex buffer is only created once. */
		static CAtomic::ATOM					m_aInstances;

		/** The default sprite sampler. */
		static CSampler							m_sSampler;

		/** The sprite raster mode. */
		static LSG_RASTERIZER_STATE *			m_prsRasterState;

		/** The depth mode. */
		static LSG_DEPTH_STENCIL_STATE *		m_pdssDepthState;

		/** The blend state for alpha objects. */
		static LSG_BLEND_STATE *				m_pbsAlphaBlendState;

		/** The blend state for non-alpha objects. */
		static LSG_BLEND_STATE *				m_pbsNonAlphaBlendState;


		// == Functions.
		/**
		 * Gets a texture based off its ID.
		 *
		 * \param _ui64Id ID of the texture to get.
		 * \return Returns a shared texture pointer.  If the texture does not exist, the shared pointer is not valid.
		 */
		CSharedTexture2dPtr LSE_CALL			TextureById( uint64_t _ui64Id );

		/**
		 * Gets a texture based off its file path.
		 *
		 * \param _sPath Path of the texture to get.
		 * \return Returns a shared texture pointer.  If the texture does not exist, the shared pointer is not valid.
		 */
		CSharedTexture2dPtr LSE_CALL			TextureByPath( const CString &_sPath );

		/**
		 * Gets texture properties by texture.
		 *
		 * \param _st2pTex Shared texture pointer whose properties are to be gotten.
		 * \return Returns the texture properties associated with the given ID or a default set of properties if no texture exists.
		 */
		LSR_TEXTURE_PROPS LSE_CALL				TexturePropsByTexture( const CSharedTexture2dPtr &_st2pTex );

		/**
		 * Creates a sprite given a shared texture and properties.
		 *
		 * \param _st2pTex The texture to assign to the sprite.
		 * \param _spProps Properties that determine which shader to use, etc.
		 * \return Returns the created sprite, which will be invalid on failure.
		 */
		template <typename _tRetType = CSharedSpritePtr>
		_tRetType LSE_CALL						CreateSprite( CSharedTexture2dPtr &_st2pTex, const LSR_SPRITE_PROPERTIES &_spProps ) {
			LSR_TEXTURE_PROPS tpTextureProps = TexturePropsByTexture( _st2pTex );
			LSR_SPRITE_PROPERTIES spCopy = _spProps;
			spCopy.afAlpha = (_spProps.afAlpha == LSR_AF_ALPHA_AUTO) ?
				(tpTextureProps.bHasAlpha ? LSR_AF_ALPHA_ON : LSR_AF_ALPHA_OFF) :
				_spProps.afAlpha;

			_tRetType tRet;
			if ( !tRet.New() ) {
				CStd::DebugPrintA( "CSpriteManager::CreateSprite(): Failed to create sprite (out of memory).\r\n" );
				return _tRetType();
			}
			tRet->Shader() = m_ssmShaderMan.GetShader( spCopy );
			if ( !tRet->Shader().Valid() ) { return _tRetType(); }

			tRet->TexRect().SetNormalizedDimensions( _st2pTex, LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );

			return tRet;
		}

		/**
		 * Adds a texture and associates it with an ID and properties.
		 *
		 * \param _st2pTex The texture to add.
		 * \param _ui64Id ID of the texture to add.
		 * \param _tpProps Properties to associate with the texture.
		 * \return Returns true if all associations were made.  False indicates a memory error.
		 */
		LSE_INLINE LSBOOL LSE_CALL				AddTexture( CSharedTexture2dPtr &_st2pTex, uint64_t _ui64Id, const LSR_TEXTURE_PROPS &_tpProps );

		/**
		 * Draws all the sprites currently in the current vertex buffer.
		 */
		void LSE_CALL							Flush();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Creates an animated sprite given an image file name.
	 *
	 * \param _pcPath Path to the file to load.
	 * \param _spProps Sprite properties.
	 * \return Returns a shared sprite pointer which will be invalid upon failure (file not found etc.)
	 */
	LSE_INLINE CSharedAnimatedSpritePtr LSE_CALL CSpriteManager::CreateAnimSprite( const char * _pcPath, const LSR_SPRITE_PROPERTIES &_spProps ) {
		return CreateSprite<CSharedAnimatedSpritePtr>( _pcPath, _spProps );
	}

	/**
	 * Creates an animated sprite given an image.
	 *
	 * \param _iImage Image for the sprite.
	 * \param _spProps Sprite properties.
	 * \return Returns a shared animated sprite pointer which will be invalid upon failure (file not found etc.)
	 */
	LSE_INLINE CSharedAnimatedSpritePtr LSE_CALL CSpriteManager::CreateAnimSprite( const CImage &_iImage, const LSR_SPRITE_PROPERTIES &_spProps ) {
		return CreateSprite<CSharedAnimatedSpritePtr>( _iImage, _spProps );
	}

	/**
	 * Gets a constant reference to the sprite sampler.  The sprite sampler uses clamping and a point filter.
	 *
	 * \return Returns a constant reference to the sprite sampler.
	 */
	LSE_INLINE CSampler & LSE_CALL CSpriteManager::Sampler() {
		return m_sSampler;
	}

	/**
	 * Gets the raster state.
	 *
	 * \return Returns the raster state commonly used for sprites.
	 */
	LSE_INLINE LSG_RASTERIZER_STATE * LSE_CALL CSpriteManager::RasterState() {
		return m_prsRasterState;
	}

	/**
	 * Gets the depth/stencil state.
	 *
	 * \return Returns the depth/stencil state commonly used for sprites.
	 */
	LSE_INLINE LSG_DEPTH_STENCIL_STATE * LSE_CALL CSpriteManager::DepthStencilState() {
		return m_pdssDepthState;
	}

	/**
	 * Gets the blend state for opaque sprites.
	 *
	 * \return Returns the blend state for opaque sprites commonly used for sprites.
	 */
	LSE_INLINE LSG_BLEND_STATE * LSE_CALL CSpriteManager::OpaqueBlendState() {
		return m_pbsNonAlphaBlendState;
	}

	/**
	 * Gets the blend state for translucent sprites.
	 *
	 * \return Returns the blend state for translucent sprites commonly used for sprites.
	 */
	LSE_INLINE LSG_BLEND_STATE * LSE_CALL CSpriteManager::AlphaBlendState() {
		return m_pbsAlphaBlendState;
	}

	/**
	 * Applies the raster, depth/stencil, and blend states commonly used for sprite rendering.
	 *
	 * \param _bAlpha If true, blending is enabled, otherwise it is not.
	 */
	LSE_INLINE void LSE_CALL CSpriteManager::SetStates( LSBOOL _bAlpha ) {
		CGfx::RSSetState( RasterState() );
		CGfx::OMSetDepthStencilState( DepthStencilState() );
		CGfx::OMSetBlendState( _bAlpha ? AlphaBlendState() : OpaqueBlendState() );
		Sampler().Activate( 0 );
		CGfx::SetWorldMatrix( CMatrix4x4::m_mIdentity );
	}

	/**
	 * Adds a texture and associates it with an ID and properties.
	 *
	 * \param _st2pTex The texture to add.
	 * \param _ui64Id ID of the texture to add.
	 * \param _tpProps Properties to associate with the texture.
	 * \return Returns true if all associations were made.  False indicates a memory error.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSpriteManager::AddTexture( CSharedTexture2dPtr &_st2pTex, uint64_t _ui64Id, const LSR_TEXTURE_PROPS &_tpProps ) {
		if ( !_st2pTex.Valid() ) { return false; }
		if ( m_lmIdsToTextures.ItemExists( _ui64Id ) ) {
			if ( !m_lmTexToProps.ItemExists( _st2pTex.Get() ) ) {
				m_lmTexToProps.Insert( _st2pTex.Get(), _tpProps );
			}
			return true;
		}

		if ( !m_lmIdsToTextures.Insert( _ui64Id, _st2pTex ) ) {
			m_lmTexToProps.Remove( _st2pTex.Get() );
			return false;
		}
		if ( !m_lmTexToProps.Insert( _st2pTex.Get(), _tpProps ) ) {
			m_lmIdsToTextures.Remove( _ui64Id );
			return false;
		}
		return true;
	}

}	// namespace lsr

#endif	// __LSR_SPRITEMANAGER_H__
