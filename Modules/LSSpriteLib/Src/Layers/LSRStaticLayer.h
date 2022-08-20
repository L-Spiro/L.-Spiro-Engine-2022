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
#include "../Entity/LSRActor2d.h"
#include "OwnedOrBorrowed/LSTLOwnedOrBorrowedPointer.h"
#include "Sampler/LSGSampler.h"
#include "ShaderLanguage/LSGShader.h"
#include "Texture/LSGTexture2d.h"
#include "VertexBuffer/LSGVertexBuffer.h"

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
	class CStaticLayer : public CActor2d {
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
			uint32_t _ui32Width, uint32_t _ui32Height, const uint16_t * _pui16Blocks, LSBOOL _bTakeOwnership,
			uint16_t _ui16BlockWidth, uint16_t _ui16BlockHeight,
			uint16_t _ui16StaggerX = 0, uint16_t _ui16StaggerY = 0,
			uint16_t _ui16StrideX = 0, uint16_t _ui16StrideY = 0 );

		/**
		 * Resets the layer to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Converts a block index into a pixel position.
		 *
		 * \param _ui32X X coordinate of the block.
		 * \param _ui32Y Y coordinate of the block.
		 * \param _ui32PixelX Returned pixel X coordinate.
		 * \param _ui32PixelY Returned pixel Y coordinate.
		 */
		LSE_INLINE void LSE_FCALL				BlockToPixel( uint32_t _ui32X, uint32_t _ui32Y, uint32_t &_ui32PixelX, uint32_t &_ui32PixelY ) const;

		/**
		 * Gets the block index at the given coordinates. -1 indicates no block.
		 *
		 * \param _ui32X X coordinate of the block.
		 * \param _ui32Y Y coordinate of the block.
		 * \return Returns the block index at the given coordinates. -1 indicates no block.
		 */
		LSE_INLINE int32_t LSE_FCALL			BlockAt( uint32_t _ui32X, uint32_t _ui32Y ) const;

		/**
		 * Renders the background using the given camera.
		 *
		 * \param _cCam The camera to use to render the layer.
		 */
		void LSE_CALL							Render( const CCamera2d &_cCam );


	protected :
		// == Types.
		/** The vertex type. */
#pragma pack( push, 1 )
		typedef struct LSR_CHUNK_VERTS {
			float								fPos[2];								/**< Vertex position. */
			float								fUv[2];									/**< UV coordinates. */
		} * LPLSR_CHUNK_VERTS, * const LPCLSR_CHUNK_VERTS;
#pragma pack( pop )

		// == Members.
		/** The tilemap texture. */
		CSharedTexture2dPtr						m_st2pTileMap;

		/** The tile map. */
		COwnedOrBorrowedPointer<uint16_t>		m_oobpBuffer;

		/** The vertex buffer. */
		CVertexBuffer							m_vbVertexBuffer;

		/** Total vertices in the vertex buffer. */
		uint32_t								m_ui32TotalPrims;

		/** Map width in tiles. */
		uint32_t								m_ui32MapWidth;

		/** Map height in tiles. */
		uint32_t								m_ui32MapHeight;

		/** Block width. */
		uint16_t								m_ui16BlockWidth;

		/** Block height. */
		uint16_t								m_ui16BlockHeight;

		/** Horizontal stride. */
		uint16_t								m_ui16StrideX;

		/** Vertical stride. */
		uint16_t								m_ui16StrideY;

		/** Stagger X. */
		uint16_t								m_ui16StaggerX;

		/** Stagger Y. */
		uint16_t								m_ui16StaggerY;

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
	LSE_INLINE void LSE_FCALL CStaticLayer::BlockToPixel( uint32_t _ui32X, uint32_t _ui32Y, uint32_t &_ui32PixelX, uint32_t &_ui32PixelY ) const {
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
	 * Gets the block index at the given coordinates. -1 indicates no block.
	 *
	 * \param _ui32X X coordinate of the block.
	 * \param _ui32Y Y coordinate of the block.
	 * \return Returns the block index at the given coordinates. -1 indicates no block.
	 */
	LSE_INLINE int32_t LSE_FCALL CStaticLayer::BlockAt( uint32_t _ui32X, uint32_t _ui32Y ) const {
		return int16_t( m_oobpBuffer[_ui32Y*m_ui32MapWidth+_ui32X] ) - 1;
	}

}	// namespace lsr

#endif	// __LSR_STATICLAYER_H__
