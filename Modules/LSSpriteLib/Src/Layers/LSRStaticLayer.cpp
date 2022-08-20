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

	// == Various constructors.
	LSE_CALLCTOR CStaticLayer::CStaticLayer() :
		m_ui32TotalPrims( 0 ),
		m_ui32MapWidth( 0 ),
		m_ui32MapHeight( 0 ),
		m_ui16BlockWidth( 0 ),
		m_ui16BlockHeight( 0 ),
		m_ui16StrideX( 0 ),
		m_ui16StrideY( 0 ),
		m_ui16StaggerX( 0 ),
		m_ui16StaggerY( 0 ) {
		m_ui32EntityType |= LSR_ET_STATICLAYER;

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
		uint32_t _ui32Width, uint32_t _ui32Height, const uint16_t * _pui16Blocks, LSBOOL _bTakeOwnership,
		uint16_t _ui16BlockWidth, uint16_t _ui16BlockHeight,
		uint16_t _ui16StaggerX, uint16_t _ui16StaggerY,
		uint16_t _ui16StrideX, uint16_t _ui16StrideY ) {
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
		uint32_t ui32Size = _ui32Width * _ui32Height;
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
		
		// Make the vertex buffers.
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
		uint32_t ui32TotalVerts = m_ui32MapWidth * m_ui32MapHeight * 6;
		CVectorPoD<LSR_CHUNK_VERTS, uint32_t> vBuffer;
		if ( !vBuffer.Resize( ui32TotalVerts ) ) { return false; }
		// Generate vertices.
		uint32_t ui32EvenRowX, ui32EvenRowY;
		uint32_t ui32OddRowX, ui32OddRowY;
		BlockToPixel( 0, 0, ui32EvenRowX, ui32EvenRowY );
		BlockToPixel( 0, 1, ui32OddRowX, ui32OddRowY );
		// UV stuff.
		// Have to offset the UV a tiny bit.
		LSREAL fUvOffsetX = LSM_ONE / LSREAL( 64.0 ) / m_st2pTileMap->GetWidth();
		LSREAL fUvOffsetY = LSM_ONE / LSREAL( 64.0 ) / m_st2pTileMap->GetHeight();
		uint32_t ui32BlocksInTileSheetX = m_st2pTileMap->GetWidth() / m_ui16BlockWidth;
		LSR_CHUNK_VERTS * pcvDst = reinterpret_cast<LSR_CHUNK_VERTS *>(&vBuffer[0]);
		m_ui32TotalPrims = 0;
		for ( uint32_t Y = 0; Y < m_ui32MapHeight; ++Y ) {
			// Vertex stuff.
			LSREAL fTop = LSREAL( Y * m_ui16BlockHeight );
			LSREAL fBottom = LSREAL( Y * m_ui16BlockHeight + m_ui16BlockHeight );
			for ( uint32_t X = 0; X < m_ui32MapWidth; ++X ) {
				LSREAL fLeftUv, fTopUv, fRightUv, fBottomUv;
				int32_t iBlockIndex = BlockAt( X, Y );
				if ( iBlockIndex < 0 ) {
					continue;
					/*fLeftUv = fTopUv = fRightUv = fBottomUv = -LSM_ONE;
					while ( ++X < m_ui32MapWidth ) {
						if ( BlockAt( X, Y ) >= 0 ) { break; }
					}
					--X;*/
				}
				//else {
					uint32_t ui32BlockUvX = iBlockIndex % ui32BlocksInTileSheetX;
					uint32_t ui32BlockUvY = iBlockIndex / ui32BlocksInTileSheetX;
					// Division redundant for accuracy.
					fLeftUv = ui32BlockUvX * m_ui16BlockWidth / LSREAL( m_st2pTileMap->GetWidth() ); 
					fTopUv = ui32BlockUvY * m_ui16BlockHeight / LSREAL( m_st2pTileMap->GetHeight() );
					fRightUv = (ui32BlockUvX * m_ui16BlockWidth + m_ui16BlockWidth) / LSREAL( m_st2pTileMap->GetWidth() );
					fBottomUv = (ui32BlockUvY * m_ui16BlockHeight + m_ui16BlockHeight) / LSREAL( m_st2pTileMap->GetHeight() );
					fLeftUv += fUvOffsetX;
					//fRightUv += fUvOffsetX;
					fTopUv += fUvOffsetY;
					//fBottomUv += fUvOffsetY;
				//}
				LSREAL fLeft = LSREAL( X * m_ui16BlockWidth );
				LSREAL fRight = LSREAL( X * m_ui16BlockWidth + m_ui16BlockWidth );
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

				m_ui32TotalPrims += 2;
			}
		}
		pcvDst = reinterpret_cast<LSR_CHUNK_VERTS *>(&vBuffer[0]);
		if ( !m_vbVertexBuffer.CreateVertexBuffer( pcvDst, vedVertDesc, 2, true,
			sizeof( LSR_CHUNK_VERTS ), m_ui32TotalPrims * 3, LSG_VBUT_SETONLY,
			LSG_PT_TRIANGLELIST ) ) {
			CStd::DebugPrintA( "CStaticLayer::CreateStaticLayer(): Failed to create vertex buffer.\r\n" );
			Reset();
			return false;
		}



		return true;
	}

	/**
	 * Resets the layer to scratch.
	 */
	void LSE_CALL CStaticLayer::Reset() {
		m_oobpBuffer.Reset();
		m_st2pTileMap.Reset();
		m_vbVertexBuffer.Reset();

		m_ui32TotalPrims = 0;
		m_ui32MapWidth = 0;
		m_ui32MapHeight = 0;
		m_ui16BlockWidth = 0;
		m_ui16BlockHeight = 0;
		m_ui16StrideX = 0;
		m_ui16StrideY = 0;
		m_ui16StaggerX = 0;
		m_ui16StaggerY = 0;
	}

	/**
	 * Renders the background using the given camera.
	 *
	 * \param _cCam The camera to use to render the layer.
	 */
	void LSE_CALL CStaticLayer::Render( const CCamera2d &_cCam ) {
		if ( !m_vbVertexBuffer.Elements() ) {
			CStd::DebugPrintA( "CStaticLayer::Render(): Vertex buffer is invalid.\r\n" );
			return;
		}

		m_sSampler.Activate( 0 );
		m_st2pTileMap->Activate( 0 );
		CGfx::RSSetState( m_prsRasterState );
		CGfx::OMSetDepthStencilState( m_pdssDepthState );
		CGfx::OMSetBlendState( m_pbsAlphaBlendState );
		CGfx::SetShader( m_sspChunkShader.Get() );
		m_vbVertexBuffer.SetStream( 0 );
		CGfx::SetWorldMatrix( WorldTrans() );
		CGfx::SetViewMatrix( _cCam.ViewMatrix() );
		CGfx::SetProjMatrix( _cCam.ProjMatrix() );
		CGfx::Render( NULL,
			0, m_ui32TotalPrims );
	}

}	// namespace lsr
