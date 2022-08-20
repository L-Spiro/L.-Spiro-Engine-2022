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
 * Description: A static layer, typically used for backgrounds.  Being static means that it stores a static
 *	non-animated tile-map, although the layer as a whole can move, scroll, etc.  This is basically like the
 *	background in Super Mario World.  All tiles must be on the same texture.
 */


#ifndef __LSR_STATICLAYER_H__
#define __LSR_STATICLAYER_H__

#include "../LSRSpriteLib.h"
#include "Framebuffer/LSGFramebuffer.h"
#include "OwnedOrBorrowed/LSTLOwnedOrBorrowedPointer.h"
#include "Sampler/LSGSampler.h"
#include "ShaderLanguage/LSGShader.h"
#include "Texture/LSGTexture2d.h"
#include "VertexBuffer/LSGNBufferVertexBuffer.h"

namespace lsr {

	// == Forward Declarations.
	class										CCamera2d;

	/**
	 * Class CStaticLayer
	 * \brief A static layer, typically used for backgrounds.
	 *
	 * Description: A static layer, typically used for backgrounds.  Being static means that it stores a static
	 *	non-animated tile-map, although the layer as a whole can move, scroll, etc.  This is basically like the
	 *	background in Super Mario World.  All tiles must be on the same texture.
	 */
	class CStaticLayer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CStaticLayer();
		LSE_CALLCTOR							~CStaticLayer();


		// == Functions.
		/**
		 * Creates a static layer.
		 *
		 * \param _st2pTileMap The tile map image to use.
		 * \param _ui32Width Width of the layer in blocks.
		 * \param _ui32Height Height of the layer in blocks.
		 * \param _pui16Blocks Pointer to an array of _ui32Width * _ui32Height unsigned shorts describing the tile layout.
		 * \param _bTakeOwnership If true, the _pui16Blocks array is copied into the layer object, otherwise _pui16Blocks is
		 *	used directly for all operations the layer makes.  This means that the data to which _pui16Blocks points must
		 *	remain in memory as long as the layer needs to use it.
		 * \param _ui16BlockWidth Width of each block in pixels.
		 * \param _ui16BlockHeight Height of each block in pixels.
		 * \param _ui16StaggerX X stagger for isometric and hexagonal tile layouts.
		 * \param _ui16StaggerY Y stagger for isometric and hexagonal tile layouts.
		 * \param _ui16StrideX Total space between blocks horizontally.  0 uses the block width.
		 * \param _ui16StrideY Total space between blocks vertically.  0 uses the block height.
		 * \return Returns true if the layer was created.  False indicates a memory error or an invalid tile sheet.
		 */
		LSBOOL LSE_CALL							CreateStaticLayer( const CSharedTexture2dPtr &_st2pTileMap,
			LSUINT32 _ui32Width, LSUINT32 _ui32Height, const LSUINT16 * _pui16Blocks, LSBOOL _bTakeOwnership,
			LSUINT16 _ui16BlockWidth, LSUINT16 _ui16BlockHeight,
			LSUINT16 _ui16StaggerX = 0, LSUINT16 _ui16StaggerY = 0,
			LSUINT16 _ui16StrideX = 0, LSUINT16 _ui16StrideY = 0 );

		/**
		 * Renders the background using the given camera.
		 *
		 * \param _cCam The camera to use to render the layer.
		 */
		LSVOID LSE_CALL							Render( const CCamera2d &_cCam );

		/**
		 * Converts a block index into a pixel position.
		 *
		 * \param _ui32X X coordinate of the block.
		 * \param _ui32Y Y coordinate of the block.
		 * \param _ui32PixelX Returned pixel X coordinate.
		 * \param _ui32PixelY Returned pixel Y coordinate.
		 */
		LSE_INLINE LSVOID LSE_FCALL				BlockToPixel( LSUINT32 _ui32X, LSUINT32 _ui32Y, LSUINT32 &_ui32PixelX, LSUINT32 &_ui32PixelY ) const;

		/**
		 * Resets the layer to scratch.
		 */
		LSVOID LSE_CALL							Reset();

		/**
		 * Gets the block index at the given coodinrates. -1 indicates no block.
		 *
		 * \param _ui32X X coordinate of the block.
		 * \param _ui32Y Y coordinate of the block.
		 * \return Returns the block index at the given coodinrates. -1 indicates no block.
		 */
		LSE_INLINE LSINT32 LSE_FCALL			BlockAt( LSUINT32 _ui32X, LSUINT32 _ui32Y ) const;


	protected :
		// == Types.
		/** The vertex type for rendering chunks. */
#pragma pack( push, 1 )
		typedef struct LSR_CHUNK_VERTS {
			LSFLOAT								fPos[2];								/**< Vertex position. */
			LSFLOAT								fUv[2];									/**< UV coordinates. */
		} * LPLSR_CHUNK_VERTS, * const LPCLSR_CHUNK_VERTS;
#pragma pack( pop )

		/** A buffered/rendered-to tile. */
		typedef struct LSR_BUFFERED_TILE_CHUNK {
			/** The texture for this chunk. Points to a texture in the pool. */
			CSharedTexture2dPtr					st2pTexture;
		} * LPLSR_BUFFERED_TILE_CHUNK, * const LPCLSR_BUFFERED_TILE_CHUNK;

		// == Members.
		/** The tilemap texture. */
		CSharedTexture2dPtr						m_st2pTileMap;

		/** The tile map. */
		COwnedOrBorrowedPointer<LSUINT16>		m_oobpBuffer;

		/** The map expressed in chunks. */
		CVector<LSR_BUFFERED_TILE_CHUNK, LSUINT32>
												m_vChunks;

		/** Temprary chunk buffer. */
		mutable CVector<LSUINT32, LSUINT32, 8>
												m_vTempChunks;

		/** Pool of allocated chunk textures.  When textures are in chunks, they are not in this array.
		 *	They are swapped back and forth. */
		CVector<CSharedTexture2dPtr>			m_vUnusedChunkTextures;

		/** The framebuffer for rendering to the chunks. */
		CFramebuffer							m_fChunkFramebuffer;

		/** The vertex buffer(s) for rendering to chunks. */
		CNBufferVertexBuffer<4>					m_nbvbChunkVertBuffer;

		/** Map width in tiles. */
		LSUINT32								m_ui32MapWidth;

		/** Map height in tiles. */
		LSUINT32								m_ui32MapHeight;

		/** Block width. */
		LSUINT16								m_ui16BlockWidth;

		/** Block height. */
		LSUINT16								m_ui16BlockHeight;

		/** Horizontal stride. */
		LSUINT16								m_ui16StrideX;

		/** Vertical stride. */
		LSUINT16								m_ui16StrideY;

		/** Stagger X. */
		LSUINT16								m_ui16StaggerX;

		/** Stagger Y. */
		LSUINT16								m_ui16StaggerY;

		/** X size of chunks in pixels. */
		LSUINT16								m_ui16ChunkSizeX;

		/** Y size of chunks in pixels. */
		LSUINT16								m_ui16ChunkSizeY;

		/** How many chunks wide the layer is. */
		LSUINT16								m_ui16ChunksX;

		/** How many chunks tall the layer is. */
		LSUINT16								m_ui16ChunksY;

		/** NDC corners. */
		static CVector4							m_vCorners[4];

		/** The shader for creating chunks. */
		static CSharedShaderPtr					m_sspChunkShader;

		/** Tracks how many instances of this class there are so that the chunk shader is only created once. */
		static CAtomic::ATOM					m_aInstances;

		/** The default sprite sampler. */
		static CSampler							m_sSampler;

		/** The sprite raster mode. */
		static LSG_RASTERIZER_STATE *			m_prsRasterState;

		/** The depth mode. */
		static LSG_DEPTH_STENCIL_STATE *		m_pdssDepthState;

		/** The blend state for alpha objects. */
		static LSG_BLEND_STATE *				m_pbsAlphaBlendState;


		// == Functions.
		/**
		 * Gets a chunk based off its X and Y grid indices.  Coordinates wrap.
		 *
		 * \param _i32X X coordinate.
		 * \param _i32Y Y coordinate.
		 * \return Returns the requested chunk.
		 */
		LSE_INLINE LSR_BUFFERED_TILE_CHUNK & LSE_CALL
												GetChunk( LSINT32 _i32X, LSINT32 _i32Y );

		/**
		 * Wraps the given coordinates.  The returned coordinates are always unsigned and valid.
		 *
		 * \param _i32X X coordinate.
		 * \param _i32Y Y coordinate.
		 */
		LSE_INLINE LSVOID LSE_CALL				WrapCoordinates( LSINT32 &_i32X, LSINT32 &_i32Y );

		/**
		 * Gets the maximum possible number of tiles in a chunk.
		 *
		 * \return Returns the maximum possible number of tiles in a chunk.
		 */
		LSE_INLINE LSUINT32 LSE_CALL			MaxTilesInChunk();

		/**
		 * Renders a chunk.
		 *
		 * \param _ui32X X index of the chunk to render.
		 * \param _ui32Y Y index of the chunk to render.
		 */

		/**
		 * Generates the vertices for a given chunk.
		 *
		 * \param _ui32X X index of the chunk for whom to generate vertices.
		 * \param _ui32Y Y index of the chunk for whom to generate vertices.
		 */
		LSVOID LSE_CALL							GenerateVertices( LSUINT32 _ui32X, LSUINT32 _ui32Y );

		/**
		 * Gathers chunks in the given frame.
		 *
		 * \param _fLeft Left of the frame.
		 * \param _fTop Top of the frame.
		 * \param _fRight Right of the frame.
		 * \param _fBottom Bottom of the frame.
		 */
		LSVOID LSE_CALL							GatherChunks( LSREAL _fLeft, LSREAL _fTop, LSREAL _fRight, LSREAL _fBottom );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Converts a block index into a pixel position.
	 *
	 * \param _ui32X X coordinate of the block.
	 * \param _ui32Y Y coordinate of the block.
	 * \param _ui32PixelX Returned pixel X coordinate.
	 * \param _ui32PixelY Returned pixel Y coordinate.
	 */
	LSE_INLINE LSVOID LSE_FCALL CStaticLayer::BlockToPixel( LSUINT32 _ui32X, LSUINT32 _ui32Y, LSUINT32 &_ui32PixelX, LSUINT32 &_ui32PixelY ) const {
		if ( m_ui16StaggerX || m_ui16StaggerY ) {
			_ui32PixelX = _ui32X * m_ui16StrideX;
			_ui32PixelY = (_ui32Y >> 1) * m_ui16StrideY;
			if ( !(_ui32Y & 1) ) {
				// Even tiles.
				_ui32PixelX -= m_ui16StaggerX;
				_ui32PixelY -= m_ui16StaggerY;
			}
		}
		else {
			_ui32PixelX = _ui32X * m_ui16StrideX;
			_ui32PixelY = _ui32Y * m_ui16StrideY;
		}
		//_ui32PixelX += m_ui16BlockWidth >> 1;
		//_ui32PixelY += m_ui16BlockHeight >> 1;
	}

	/**
	 * Gets the block index at the given coodinrates. -1 indicates no block.
	 *
	 * \param _ui32X X coordinate of the block.
	 * \param _ui32Y Y coordinate of the block.
	 * \return Returns the block index at the given coodinrates. -1 indicates no block.
	 */
	LSE_INLINE LSINT32 LSE_FCALL CStaticLayer::BlockAt( LSUINT32 _ui32X, LSUINT32 _ui32Y ) const {
		return (_ui32X < m_ui32MapWidth && _ui32Y < m_ui32MapHeight) ?
			LSINT16( m_oobpBuffer[_ui32Y*m_ui32MapWidth+_ui32X] ) - 1:
			-1;
	}

	/**
	 * Gets a chunk based off its X and Y grid indices.  Coordinates wrap.
	 *
	 * \param _i32X X coordinate.
	 * \param _i32Y Y coordinate.
	 * \return Returns the requested chunk.
	 */
	LSE_INLINE CStaticLayer::LSR_BUFFERED_TILE_CHUNK & LSE_CALL CStaticLayer::GetChunk( LSINT32 _i32X, LSINT32 _i32Y ) {
		WrapCoordinates( _i32X, _i32Y );
		return m_vChunks[_i32Y*m_ui16ChunksX+_i32X];
	}

	/**
	 * Wraps the given coordinates.  The returned coordinates are always unsigned and valid.
	 *
	 * \param _i32X X coordinate.
	 * \param _i32Y Y coordinate.
	 */
	LSE_INLINE LSVOID LSE_CALL CStaticLayer::WrapCoordinates( LSINT32 &_i32X, LSINT32 &_i32Y ) {
		if ( _i32X >= 0 ) { 
			_i32X %= m_ui16ChunksX;
		}
		else {
			_i32X = m_ui16ChunksX - ((-_i32X) % m_ui16ChunksX) - 1;
		}
		if ( _i32Y >= 0 ) { 
			_i32Y %= m_ui16ChunksY;
		}
		else {
			_i32Y = m_ui16ChunksY - ((-_i32Y) % m_ui16ChunksY) - 1;
		}
	}

	/**
	 * Gets the maximum possible number of tiles in a chunk.
	 *
	 * \return Returns the maximum possible number of tiles in a chunk.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CStaticLayer::MaxTilesInChunk() {
		// Chunks always begin on stride boundaries and are always a multiple of stride tiles wide and 2 strides tall.
		//	This means chunks are always an even number of rows tall.
		LSUINT32 ui32TilesAcross = m_ui16ChunkSizeX / m_ui16StrideX;
		LSUINT32 ui32TilesTall = m_ui16ChunkSizeY / m_ui16StrideY;
		LSUINT32 ui32Total = ui32TilesAcross * ui32TilesTall;
		if ( m_ui16StaggerX ) {
			ui32Total += ui32TilesAcross;
		}
		if ( m_ui16StaggerY ) {
			ui32Total += ui32TilesTall;
		}
		return ui32Total;
	}

}	// namespace lsr

#endif	// __LSR_STATICLAYER_H__
