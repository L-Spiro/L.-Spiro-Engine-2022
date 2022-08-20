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

#include "LSRStaticLayer.h"
#include "../Camera/LSRCamera2d.h"


 // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 // MACROS
 // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** Size of chunks. */
#define LSR_CHUNK_SIZE	512

namespace lsr {

	// == Members.
	/** The shader for creating chunks. */
	CSharedShaderPtr CStaticLayer::m_sspChunkShader;

	/** Tracks how many instances of this class there are so that the chunk shader is only created once. */
	CAtomic::ATOM CStaticLayer::m_aInstances = 0;

	/** The default sprite sampler. */
	CSampler CStaticLayer::m_sSampler;

	/** The sprite raster mode. */
	LSG_RASTERIZER_STATE * CStaticLayer::m_prsRasterState = NULL;

	/** The depth mode. */
	LSG_DEPTH_STENCIL_STATE * CStaticLayer::m_pdssDepthState = NULL;

	/** The blend state for alpha objects. */
	LSG_BLEND_STATE * CStaticLayer::m_pbsAlphaBlendState = NULL;

	// == Members.
	/** NDC corners. */
	CVector4 CStaticLayer::m_vCorners[4] = {
		CVector4( -1.0f, -1.0f, 0.0f, 1.0f ),
		CVector4( 1.0f, 1.0f, 0.0f, 1.0f ),
		CVector4( 1.0f, -1.0f, 0.0f, 1.0f ),
		CVector4( -1.0f, 1.0f, 0.0f, 1.0f ),
	};

	// == Various constructors.
	LSE_CALLCTOR CStaticLayer::CStaticLayer() :
		m_ui32MapWidth( 0 ),
		m_ui32MapHeight( 0 ),
		m_ui16BlockWidth( 0 ),
		m_ui16BlockHeight( 0 ),
		m_ui16StrideX( 0 ),
		m_ui16StrideY( 0 ),
		m_ui16StaggerX( 0 ),
		m_ui16StaggerY( 0 ),
		m_ui16ChunkSizeX( 0 ),
		m_ui16ChunkSizeY( 0 ),
		m_ui16ChunksX( 0 ),
		m_ui16ChunksY( 0 ) {
		if ( CAtomic::InterlockedIncrement( m_aInstances ) == 1 ) {
			m_sspChunkShader.New();
			if ( m_sspChunkShader->CompileFromFiles( "LSRSpriteStaticLayerChunkVS.lssl", "LSRSpriteStaticLayerChunkPS.lssl" ) ) {
				m_sspChunkShader.Reset();
			}

			m_sSampler.SetAddressMode( LSG_TAM_CLAMP, LSG_TAM_CLAMP, LSG_TAM_CLAMP );
			m_sSampler.SetMaxAnisotropy( 1 );
			m_sSampler.SetFilter( LSG_F_MIN_MAG_MIP_POINT );

			LSG_RASTERIZER_DESC rdRastrDesc;
			rdRastrDesc = CGfx::DefaultRasterizerDesc( rdRastrDesc );
			rdRastrDesc.CullMode = CStd::ImpCast( rdRastrDesc.CullMode, LSG_CM_NONE );
			rdRastrDesc.DepthClipEnable = false;
			CGfx::CreateRasterizerState( rdRastrDesc, &m_prsRasterState );

			LSG_DEPTH_STENCIL_DESC dsdDepdthDesc;
			dsdDepdthDesc = CGfx::DefaultDepthStencilDesc( dsdDepdthDesc );
			dsdDepdthDesc.StencilEnable = false;
			dsdDepdthDesc.DepthEnable = false;
			CGfx::CreateDepthStencilState( dsdDepdthDesc, &m_pdssDepthState );

			LSG_BLEND_DESC bdBlendDec;
			bdBlendDec = CGfx::DefaultBlendDesc( bdBlendDec );
			bdBlendDec.RenderTarget[0].BlendEnable = true;
			bdBlendDec.RenderTarget[0].SrcBlend = CStd::ImpCast( bdBlendDec.RenderTarget[0].SrcBlend, LSG_BM_SRC_ALPHA );
			bdBlendDec.RenderTarget[0].DestBlend = CStd::ImpCast( bdBlendDec.RenderTarget[0].DestBlend, LSG_BM_INV_SRC_ALPHA );
			bdBlendDec.RenderTarget[0].SrcBlendAlpha = CStd::ImpCast( bdBlendDec.RenderTarget[0].SrcBlendAlpha, LSG_BM_SRC_ALPHA );
			bdBlendDec.RenderTarget[0].DestBlendAlpha = CStd::ImpCast( bdBlendDec.RenderTarget[0].DestBlendAlpha, LSG_BM_ONE );
			CGfx::CreateBlendState( bdBlendDec, &m_pbsAlphaBlendState );
		}
	}
	LSE_CALLCTOR CStaticLayer::~CStaticLayer() {
		Reset();
		if ( CAtomic::InterlockedDecrement( m_aInstances ) == 0 ) {
			m_sspChunkShader.Reset();
			m_sSampler.Reset();
			CGfx::ReleaseState( m_prsRasterState );
			CGfx::ReleaseState( m_pdssDepthState );
			CGfx::ReleaseState( m_pbsAlphaBlendState );
		}
	}

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
	LSBOOL LSE_CALL CStaticLayer::CreateStaticLayer( const CSharedTexture2dPtr &_st2pTileMap,
		LSUINT32 _ui32Width, LSUINT32 _ui32Height, const LSUINT16 * _pui16Blocks, LSBOOL _bTakeOwnership,
		LSUINT16 _ui16BlockWidth, LSUINT16 _ui16BlockHeight,
		LSUINT16 _ui16StaggerX, LSUINT16 _ui16StaggerY,
		LSUINT16 _ui16StrideX, LSUINT16 _ui16StrideY ) {
		Reset();
		if ( !_ui32Width || !_ui32Height ) {
			CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Layer must have both width and height.\r\n" );
			Reset();
			return false;
		}
		if ( !_st2pTileMap.Valid() ) {
			CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Tile sheet is invalid.\r\n" );
			Reset();
			return false;
		}
		_ui16StrideX = _ui16StrideX ? _ui16StrideX : _ui16BlockWidth;
		_ui16StrideY = _ui16StrideY ? _ui16StrideY : _ui16BlockHeight;
		LSUINT32 ui32Size = _ui32Width * _ui32Height;
		if ( _bTakeOwnership ) {
			if ( !m_oobpBuffer.Create( ui32Size ) ) {
				CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Not enough memory to copy the tile map.\r\n" );
				Reset();
				return false;
			}
			CStd::MemCpy( &m_oobpBuffer[0], _pui16Blocks, ui32Size );
		}
		else {
			m_oobpBuffer.Borrow( _pui16Blocks, ui32Size );
		}

		// Copy all the data.
		m_st2pTileMap = _st2pTileMap;
		if ( !m_st2pTileMap.Valid() ) {
			CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Not enough memory to copy the tile texture.\r\n" );
			Reset();
			return false;
		}

		m_ui32MapWidth = _ui32Width;
		m_ui32MapHeight = _ui32Height;
		m_ui16BlockWidth = _ui16BlockWidth;
		m_ui16BlockHeight = _ui16BlockHeight;
		m_ui16StrideX = _ui16StrideX;
		m_ui16StrideY = _ui16StrideY;
		m_ui16StaggerX = _ui16StaggerX;
		m_ui16StaggerY = _ui16StaggerY;

		m_ui16ChunkSizeX = LSUINT16( LSE_ROUND_UP_ANY( LSR_CHUNK_SIZE, m_ui16StrideX ) );
		m_ui16ChunkSizeY = LSUINT16( LSE_ROUND_UP_ANY( LSR_CHUNK_SIZE, m_ui16StrideY << 1 ) );
		LSUINT32 ui32PixelWidth = m_ui16StrideX * m_ui32MapWidth;
		LSUINT32 ui32PixelHeight = m_ui16StrideY * m_ui32MapHeight;
		LSUINT32 ui32PixelWidthChunk = m_ui16ChunkSizeX;
		LSUINT32 ui32PixelHeightChunk = m_ui16ChunkSizeY;
		m_ui16ChunksX = LSUINT16( ui32PixelWidth / ui32PixelWidthChunk );
		if ( ui32PixelWidth % ui32PixelWidthChunk ) {
			++m_ui16ChunksX;
		}
		m_ui16ChunksY = LSUINT16( ui32PixelHeight / ui32PixelHeightChunk );
		if ( ui32PixelHeight % ui32PixelHeightChunk ) {
			++m_ui16ChunksY;
		}
		if ( !m_vChunks.Resize( m_ui16ChunksX * m_ui16ChunksY ) ) {
			CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Not enough memory to allocate layer chunks.\r\n" );
			Reset();
			return false;
		}

		// Make the vertex buffers.
		LSUINT32 ui32TotalVerts = MaxTilesInChunk() * 6;
		static LSG_VERTEX_ELEMENT_DESC vedVertDesc[] = {
			{
				LSG_VE_R32G32_F,
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
		for ( LSUINT32 I = m_nbvbChunkVertBuffer.TotalBuffers(); I--; ) {
			if ( !m_nbvbChunkVertBuffer.GetBuffer( I ).CreateVertexBuffer( NULL, vedVertDesc, 2, false,
				sizeof( LSR_CHUNK_VERTS ), ui32TotalVerts, LSG_VBUT_DYNAMIC,
				LSG_PT_TRIANGLELIST ) ) {
				CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Failed to create chunk vertex buffers.\r\n" );
				Reset();
				return false;
			}
		}

		

		return true;
	}

	/**
	 * Renders the background using the given camera.
	 *
	 * \param _cCam The camera to use to render the layer.
	 */
	LSVOID LSE_CALL CStaticLayer::Render( const CCamera2d &_cCam ) {
		if ( !m_sspChunkShader.Valid() ) {
			CStd::DebugPrintA( "CStaticLayer::Render(): Chunk-rendering shader is invalid.\r\n" );
			return;
		}
		// Determine the left, right, top, and bottom coverage of the view area.
		CMatrix4x4 mInvProj = _cCam.ViewProjMatrix().Inverse();
		CVector4 vPoints[4];
		CMatrix4x4::MultiplyVec4Batch( mInvProj, m_vCorners, 4, vPoints );
		// TODO: Apply offsets to allow BG scrolling.

		LSREAL fLeft = vPoints[3].x;
		LSREAL fRight = vPoints[3].x;
		LSREAL fBottom = vPoints[3].y;
		LSREAL fTop = vPoints[3].y;
		for ( LSUINT32 I = 3; I--; ) {
			fLeft = CStd::Min( fLeft, vPoints[I].x );
			fRight = CStd::Max( fRight, vPoints[I].x );
			fBottom = CStd::Max( fBottom, vPoints[I].y );
			fTop = CStd::Min( fTop, vPoints[I].y );
		}
		GatherChunks( fLeft, fTop, fRight, fBottom );	// Fills m_vTempChunks.
		for ( LSUINT32 I = m_vTempChunks.Length(); I--; ) {
			LSUINT32 ui32X = m_vTempChunks[I] & 0xFFFF;
			LSUINT32 ui32Y = m_vTempChunks[I] >> 16;
			LSR_BUFFERED_TILE_CHUNK & btcChunk = GetChunk( ui32X, ui32Y );
			if ( !btcChunk.st2pTexture.Valid() ) {
				GenerateVertices( ui32X, ui32Y );
			}
		}
	}

	/**
	 * Resets the layer to scratch.
	 */
	LSVOID LSE_CALL CStaticLayer::Reset() {
		m_vUnusedChunkTextures.Reset();
		m_vChunks.Reset();
		m_oobpBuffer.Reset();
		m_st2pTileMap.Reset();

		m_ui32MapWidth = 0;
		m_ui32MapHeight = 0;
		m_ui16BlockWidth = 0;
		m_ui16BlockHeight = 0;
		m_ui16StrideX = 0;
		m_ui16StrideY = 0;
		m_ui16StaggerX = 0;
		m_ui16StaggerY = 0;
		m_ui16ChunkSizeX = 0;
		m_ui16ChunkSizeY = 0;
		m_ui16ChunksX = 0;
		m_ui16ChunksY = 0;

		m_nbvbChunkVertBuffer.Reset();
	}

	/**
	 * Generates the vertices for a given chunk.
	 *
	 * \param _ui32X X index of the chunk for whom to generate vertices.
	 * \param _ui32Y Y index of the chunk for whom to generate vertices.
	 */
	LSVOID LSE_CALL CStaticLayer::GenerateVertices( LSUINT32 _ui32X, LSUINT32 _ui32Y ) {
		LSG_VERTEX_BUFFER_MAP vbmMap = m_nbvbChunkVertBuffer.Cur().Map( LSG_VBMA_WRITE_DISCARD );
		LSR_CHUNK_VERTS * pcvDst = reinterpret_cast<LSR_CHUNK_VERTS *>(vbmMap.pvData);
		LSINT32 i32ChunkX = LSINT32( _ui32X * m_ui16ChunkSizeX );
		LSINT32 i32ChunkY = LSINT32( _ui32Y * m_ui16ChunkSizeY );
		LSUINT32 ui32BlocksPerChunkX = m_ui16ChunkSizeX / m_ui16StrideX;
		LSUINT32 ui32BlocksPerChunkY = m_ui16ChunkSizeY / m_ui16StrideY;
		LSUINT32 ui32EvenBlockStartX = _ui32X * ui32BlocksPerChunkX;
		LSUINT32 ui32EvenBlockStartY = _ui32Y * ui32BlocksPerChunkY;
		LSUINT32 ui32OddBlockStartX = ui32EvenBlockStartX;
		LSUINT32 ui32OddBlockStartY = ui32EvenBlockStartY + 1;
		LSUINT32 ui32EvenRowX, ui32EvenRowY;
		LSUINT32 ui32OddRowX, ui32OddRowY;
		BlockToPixel( ui32EvenBlockStartX, ui32EvenBlockStartY, ui32EvenRowX, ui32EvenRowY );
		BlockToPixel( ui32OddBlockStartX, ui32OddBlockStartY, ui32OddRowX, ui32OddRowY );
		
		LSINT32 i32EvenLeft = static_cast<LSINT32>(ui32EvenRowX) - i32ChunkX;
		LSINT32 i32EvenTop = static_cast<LSINT32>(ui32EvenRowY) - i32ChunkY;

		LSINT32 i32OddLeft = static_cast<LSINT32>(ui32OddRowX) - i32ChunkX;
		LSINT32 i32OddTop = static_cast<LSINT32>(ui32OddRowY) - i32ChunkY;

		// Even blocks are staggered.
		// If staggering vertically, blocks from the above tile should come into this tile.
		if ( m_ui16StaggerY ) {
			++ui32BlocksPerChunkY;	// Even blocks hanging off the bottom.
		}
		if ( m_ui16StaggerX ) {
			++ui32BlocksPerChunkX;	// Even blocks hanging off the right edge.
		}
		// UV stuff.
		LSREAL fUvWidthConv = LSM_ONE / m_st2pTileMap->GetWidth();
		LSREAL fUvHeightConv = LSM_ONE / m_st2pTileMap->GetHeight();
		LSUINT32 ui32BlocksInTileSheetX = m_st2pTileMap->GetWidth() / m_ui16BlockWidth;
		LSREAL fBlockWidth = m_ui16BlockWidth * fUvWidthConv;
		LSREAL fBlockHeight = m_ui16BlockHeight * fUvHeightConv;

		LSUINT32 ui32YBlockIndex = ui32EvenBlockStartY;
		for ( LSUINT32 Y = 0; Y < ui32BlocksPerChunkY; ++Y, ++ui32YBlockIndex ) {
			//if ( ui32YBlockIndex >= m_ui32MapHeight ) { break; }	// Passed the end of the tile map.
			LSUINT32 ui32XBlockIndex = ui32EvenBlockStartX;
			LSINT32 i32X, i32Y;
			if ( !(Y & 1) ) {
				// Use even coordinates for this row.
				i32X = i32EvenLeft;
				i32Y = i32EvenTop;
				i32EvenTop += m_ui16StrideY << 1;
			}
			else {
				// Use odd coordinates for this row.
				i32X = i32OddLeft;
				i32Y = i32OddTop;
				i32OddTop += m_ui16StrideY << 1;
			}

			

			// Vertex stuff.
			LSREAL fBottom = LSREAL( i32Y + m_ui16BlockHeight ) / m_ui16ChunkSizeY;
			LSREAL fTop = LSREAL( i32Y ) / m_ui16ChunkSizeY;
			fBottom = fBottom * LSM_TWO - LSM_ONE;
			fTop = fTop * LSM_TWO - LSM_ONE;
			fBottom = -fBottom;
			fTop = -fTop;
			//LSREAL fLeft = LSREAL( i32X ) / m_ui16ChunkSizeX;
			//LSREAL fRight = LSREAL( i32X + m_ui16BlockWidth ) / m_ui16ChunkSizeX;
			
			/*fLeft = fLeft * LSM_TWO - LSM_ONE;
			fRight = fRight * LSM_TWO - LSM_ONE;*/
			for ( LSUINT32 X = 0; X < ui32BlocksPerChunkX; ++X, i32X += m_ui16StrideX, ++ui32XBlockIndex ) {
				LSREAL fLeft = LSREAL( i32X ) / m_ui16ChunkSizeX;
				LSREAL fRight = LSREAL( i32X + m_ui16BlockWidth ) / m_ui16ChunkSizeX;
				fLeft = fLeft * LSM_TWO - LSM_ONE;
				fRight = fRight * LSM_TWO - LSM_ONE;

				LSINT32 iBlockIndex = BlockAt( ui32XBlockIndex, ui32YBlockIndex );

				LSREAL fLeftUv, fTopUv, fRightUv, fBottomUv;
				if ( iBlockIndex >= 0 ) {
					LSUINT32 ui32BlockUvX = iBlockIndex % ui32BlocksInTileSheetX;
					LSUINT32 ui32BlockUvY = iBlockIndex / ui32BlocksInTileSheetX;
					fLeftUv = ui32BlockUvX * m_ui16BlockWidth * fUvWidthConv; 
					fTopUv = ui32BlockUvY * m_ui16BlockHeight * fUvHeightConv;
					fRightUv = fLeftUv + fBlockWidth;
					fBottomUv = fTopUv + fBlockHeight;
				}
				else {
					fLeftUv = fTopUv = fRightUv = fBottomUv = -LSM_ONE;
				}
				// ============
				// 1st Triangle
				// ============
				// Upper-left.
				pcvDst->fPos[0] = fLeft;
				pcvDst->fPos[1] = fTop;
				pcvDst->fUv[0] = fLeftUv;
				pcvDst->fUv[1] = fTopUv;
				++pcvDst;
				// Upper-right.
				pcvDst->fPos[0] = fRight;
				pcvDst->fPos[1] = fTop;
				pcvDst->fUv[0] = fRightUv;
				pcvDst->fUv[1] = fTopUv;
				++pcvDst;
				// Bottom-left.
				pcvDst->fPos[0] = fLeft;
				pcvDst->fPos[1] = fBottom;
				pcvDst->fUv[0] = fLeftUv;
				pcvDst->fUv[1] = fBottomUv;
				++pcvDst;
				// ============
				// 2nd Triangle
				// ============
				// Upper-right.
				pcvDst->fPos[0] = fRight;
				pcvDst->fPos[1] = fTop;
				pcvDst->fUv[0] = fRightUv;
				pcvDst->fUv[1] = fTopUv;
				++pcvDst;
				// Bottom-right.
				pcvDst->fPos[0] = fRight;
				pcvDst->fPos[1] = fBottom;
				pcvDst->fUv[0] = fRightUv;
				pcvDst->fUv[1] = fBottomUv;
				++pcvDst;
				// Bottom-left.
				pcvDst->fPos[0] = fLeft;
				pcvDst->fPos[1] = fBottom;
				pcvDst->fUv[0] = fLeftUv;
				pcvDst->fUv[1] = fBottomUv;
				++pcvDst;

				/*fLeft = fRight;
				fRight = LSREAL( i32X + m_ui16BlockWidth ) / m_ui16ChunkSizeX;
				fRight = fRight * LSM_TWO - LSM_ONE;*/
			}
		}
		pcvDst = reinterpret_cast<LSR_CHUNK_VERTS *>(vbmMap.pvData);

		m_nbvbChunkVertBuffer.Cur().Unmap( vbmMap );
		

		// Prepare to render.
		LSR_BUFFERED_TILE_CHUNK btcChunk = GetChunk( _ui32X, _ui32Y );
		if ( !btcChunk.st2pTexture.Valid() ) {
			btcChunk.st2pTexture.New();
			if ( !btcChunk.st2pTexture.Valid() ) { return; }
			CImage iTemp;
			iTemp.CreateBlank( LSI_PF_R8G8B8A8, m_ui16ChunkSizeX, m_ui16ChunkSizeY );
			if ( !btcChunk.st2pTexture->CreateTexture( &iTemp, 1, LSG_TUT_RENDERTARGET ) ) { return; }
		}
		m_fChunkFramebuffer.Texture2DColorAttach( btcChunk.st2pTexture.Get(), 0, 0 );
		CFramebuffer * pfPrev = CGfx::GetFramebuffer();
		CGfx::SetFramebuffer( &m_fChunkFramebuffer );
		CGfx::SetViewport( LSM_ZERO, LSM_ZERO, LSREAL( btcChunk.st2pTexture->GetWidth() ), LSREAL( btcChunk.st2pTexture->GetHeight() ) );
		m_sSampler.Activate( 0 );
		m_st2pTileMap->Activate( 0 );
		CGfx::RSSetState( m_prsRasterState );
		CGfx::OMSetDepthStencilState( m_pdssDepthState );
		CGfx::OMSetBlendState( m_pbsAlphaBlendState );
		CGfx::SetShader( m_sspChunkShader.Get() );
		m_nbvbChunkVertBuffer.Cur().SetStream( 0 );
		CGfx::Render( NULL,
			0, ui32BlocksPerChunkY * ui32BlocksPerChunkX * 2 );
		CGfx::SetFramebuffer( pfPrev );
		m_nbvbChunkVertBuffer.Swap();
	}

	/**
	 * Gathers chunks in the given frame.
	 *
	 * \param _fLeft Left of the frame.
	 * \param _fTop Top of the frame.
	 * \param _fRight Right of the frame.
	 * \param _fBottom Bottom of the frame.
	 */
	LSVOID LSE_CALL CStaticLayer::GatherChunks( LSREAL _fLeft, LSREAL _fTop, LSREAL _fRight, LSREAL _fBottom ) {
		m_vTempChunks.ResetNoDealloc();
		LSINT32 i32LeftIndex = LSINT32( _fLeft / m_ui16ChunkSizeX );
		LSINT32 i32RightIndex = LSINT32( (_fRight - LSM_ZERO) / m_ui16ChunkSizeX );

		LSINT32 i32TopIndex = LSINT32( _fTop / m_ui16ChunkSizeY );
		LSINT32 i32BottomIndex = LSINT32( (_fBottom - LSM_ZERO) / m_ui16ChunkSizeY );
		for ( LSINT32 Y = i32TopIndex; Y <= i32BottomIndex; ++Y ) {
			for ( LSINT32 X = i32LeftIndex; X <= i32RightIndex; ++X ) {
				LSINT32 i32X = X;
				LSINT32 i32Y = Y;
				WrapCoordinates( i32X, i32Y );
				LSUINT32 ui16X = i32X;
				LSUINT32 ui16Y = i32Y;
				m_vTempChunks.Push( ui16X | (ui16Y << 16) );
			}
		}
	}

}	// namespace lsr

#undef LSR_CHUNK_SIZE
