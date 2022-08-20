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
 * Description: A 2D Direct3D 11 texture.
 */

#include "LSGDirectX11Texture2d.h"

#ifdef LSG_DX11

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11Texture2d::CDirectX11Texture2d() :
		m_ptTexture( NULL ),
		m_psrvShaderView( NULL ) {
	}
	LSE_CALLCTOR CDirectX11Texture2d::~CDirectX11Texture2d() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL CDirectX11Texture2d::Reset() {
		CDirectX11::SafeRelease( m_psrvShaderView );
		for ( uint32_t I = m_vRenderTargetViews.Length(); I--; ) {
			CDirectX11::SafeRelease( m_vRenderTargetViews[I] );
		}
		m_vRenderTargetViews.Reset();
		CDirectX11::SafeRelease( m_ptTexture );

		Parent::Reset();
	}

	/**
	* Activates the object as a color render target.
	*
	* \param _ui32Mipmap The mipmap to set.
	* \param _ui32Slice The slice in a 3D texture to set.
	* \param _ui32Slot The color slot into which to activate the render target.
	* \return Returns true if it is ready to be used as a render target.
	*/
	LSBOOL LSE_CALL CDirectX11Texture2d::SetAsColorRenderTarget( uint32_t _ui32Mipmap, uint32_t _ui32Slice, uint32_t _ui32Slot ) const {
		uint32_t ui32Index = _ui32Slice * m_ui16MipMaps + _ui32Mipmap;
		if ( !m_vRenderTargetViews[ui32Index] ) { return false; }
		CDirectX11::m_rsCurRenderState.prtRenderTargets[_ui32Slot] = m_vRenderTargetViews[ui32Index];
		CDirectX11::m_rsCurRenderState.ui8TotalRenderTargets = CStd::Max<uint8_t>( CDirectX11::m_rsCurRenderState.ui8TotalRenderTargets, uint8_t( _ui32Slot + 1 ) );
		return true;
	}

	/**
	 * Creates a DirectX 11 texture and fills it with our texel data.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL CDirectX11Texture2d::CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total ) {
		/*for ( uint32_t I = DXGI_FORMAT_UNKNOWN; I <= DXGI_FORMAT_BC7_UNORM_SRGB; ++I ) {
			UINT uiSup;
			CDirectX11::GetDirectX11Device()->CheckFormatSupport( static_cast<DXGI_FORMAT>(I), &uiSup );
			char szFormat[64];
			CStd::SPrintF( szFormat, LSE_ELEMENTS( szFormat ), "%u: %.4X\r\n", I, uiSup & (D3D11_FORMAT_SUPPORT_BUFFER | D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET) );
			CStd::DebugPrintA( szFormat );
		}*/
		// Verify format.
		/*UINT uiSup;
		CDirectX11::GetDirectX11Device()->CheckFormatSupport( static_cast<DXGI_FORMAT>(Direct3DFormat( _piImages[0].GetFormat() )), &uiSup );
		if ( !(uiSup & D3D11_FORMAT_SUPPORT_TEXTURE2D) ) {
			CStd::DebugPrintA( "Not supported!\r\n" );
		}*/

		// Create the texture.
		D3D11_TEXTURE2D_DESC tdDec = { 0 };
		tdDec.Width = _piImages[0].GetWidth();
		tdDec.Height = _piImages[0].GetHeight();
		tdDec.MipLevels = _piImages[0].TotalMipLevels();
		tdDec.ArraySize = _ui32Total;
		tdDec.Format = Direct3DFormat( _piImages[0].GetFormat(), _piImages[0].IsSRgb() );
		tdDec.SampleDesc.Count = m_ui16MultiSampleCount;
		tdDec.SampleDesc.Quality = m_ui16MultiSampleQuality;
		tdDec.Usage = static_cast<D3D11_USAGE>(m_ui32Usage & 0xFF);
		tdDec.BindFlags = D3D11_BIND_SHADER_RESOURCE | ((m_ui32Usage & LSG_TUT_RENDERTARGET) ? D3D11_BIND_RENDER_TARGET : 0);
		HRESULT hRes = CDirectX11::GetDirectX11Device()->CreateTexture2D( &tdDec, NULL, &m_ptTexture );
		if ( FAILED( hRes ) ) { return false; }


		// Create the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvdShaderViewDesc;		
        srvdShaderViewDesc.Format = tdDec.Format;
		srvdShaderViewDesc.ViewDimension = tdDec.ArraySize == 1 ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvdShaderViewDesc.Texture2D.MipLevels = tdDec.MipLevels;
        srvdShaderViewDesc.Texture2D.MostDetailedMip = 0;
		if ( FAILED( CDirectX11::GetDirectX11Device()->CreateShaderResourceView( m_ptTexture, &srvdShaderViewDesc, &m_psrvShaderView ) ) ) { return false; }

		// Create the render target view.
		if ( m_ui32Usage & LSG_TUT_RENDERTARGET ) {
			D3D11_RENDER_TARGET_VIEW_DESC rtvdRenderTargetViewDesc;
			rtvdRenderTargetViewDesc.Format = tdDec.Format;
			rtvdRenderTargetViewDesc.ViewDimension = tdDec.ArraySize == 1 ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			if ( !m_vRenderTargetViews.Resize( m_ui16MipMaps * m_ui16ArraySize ) ) { return false; }
			for ( uint32_t I = 0; I < m_ui16ArraySize; ++I ) {
				for ( uint32_t J = 0; J < m_ui16MipMaps; ++J ) {
					switch ( rtvdRenderTargetViewDesc.ViewDimension ) {
						case D3D11_RTV_DIMENSION_TEXTURE2D : {
							rtvdRenderTargetViewDesc.Texture2DArray.MipSlice = J;
							rtvdRenderTargetViewDesc.Texture2DArray.FirstArraySlice = I;
							rtvdRenderTargetViewDesc.Texture2DArray.ArraySize = 1;
							break;
						}
						default : {
							rtvdRenderTargetViewDesc.Texture2D.MipSlice = J;
						}

					}
					
					if ( FAILED( CDirectX11::GetDirectX11Device()->CreateRenderTargetView( m_ptTexture, &rtvdRenderTargetViewDesc, &m_vRenderTargetViews[I*m_ui16MipMaps+J] ) ) ) { return false; }
				}
			}
		}


		// Copy the image data into the texture.
		if ( tdDec.Usage == D3D11_USAGE_DEFAULT ) {
			// Have to create a staging texture.
			ID3D11Texture2D * ptStaging = NULL;
			tdDec.Usage = D3D11_USAGE_STAGING;
			tdDec.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			tdDec.BindFlags = 0;
			if ( FAILED( CDirectX11::GetDirectX11Device()->CreateTexture2D( &tdDec, NULL, &ptStaging ) ) ) { return false; }

			if ( !CreateMips( ptStaging, _piImages, _ui32Total ) ) {
				CDirectX11::SafeRelease( ptStaging );
				return false;
			}

			CDirectX11::GetDirectX11Context()->CopyResource( m_ptTexture, ptStaging );
			CDirectX11::SafeRelease( ptStaging );
			return true;
		}
		if ( !CreateMips( m_ptTexture, _piImages, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Constructs a full mipmap chain using the given image array.
	 *
	 * \param _ptTexture The texture for which to construct a full mipmap chain(s).
	 * \param _piTexels The images from which to copy the full mipmap chain(s).
	 * \param _ui32Total Total images to which _piTexels points.
	 * \return Returns true if the all mipmaps were copied.
	 */
	LSBOOL LSE_CALL CDirectX11Texture2d::CreateMips( ID3D11Texture2D * _ptTexture, const CImage * _piTexels, uint32_t _ui32Total ) {
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {							// For every texture.
			uint32_t ui32Width = _piTexels[I].GetWidth();
			uint32_t ui32Height = _piTexels[I].GetHeight();

			for ( uint32_t M = 0; M < _piTexels[I].TotalMipLevels(); ++M ) {	// For every mipmap.
				D3D11_MAPPED_SUBRESOURCE msMap;
				if ( FAILED( CDirectX11::GetDirectX11Context()->Map( _ptTexture, ::D3D11CalcSubresource( M, I, _piTexels[I].TotalMipLevels() ), D3D11_MAP_WRITE, 0UL, &msMap ) ) ) {
					return false;
				}
				CreateMips( &msMap,
					_piTexels[I].GetMipMapBuffers( M ), ui32Width, ui32Height, _piTexels[I].GetFormat() );
				CDirectX11::GetDirectX11Context()->Unmap( _ptTexture, ::D3D11CalcSubresource( M, I, _piTexels[I].TotalMipLevels() ) );
				
				ui32Width = CStd::Max<uint32_t>( 1, ui32Width >> 1 );
				ui32Height = CStd::Max<uint32_t>( 1, ui32Height >> 1 );
			}
		}
		return true;
	}

	/**
	 * Constructs a full mipmap chain using the given image.
	 *
	 * \param _pmsSurface The texture for which to construct a full mipmap chain.
	 * \param _tbTexels The image from which to copy the full mipmap chain.
	 * \param _ui32Width The image width.
	 * \param _ui32Height The image height.
	 * \param _pFormat The format of the texels in _tbTexels.
	 */
	void LSE_CALL CDirectX11Texture2d::CreateMips( D3D11_MAPPED_SUBRESOURCE * _pmsSurface,
		const CImageLib::CTexelBuffer &_tbTexels, uint32_t _ui32Width, uint32_t _ui32Height, LSI_PIXEL_FORMAT _pFormat ) {
		if ( CImageLib::IsCompressed( _pFormat ) ) {
			// DXT/BC formats.
			uint32_t ui32DstPitch = _pmsSurface->RowPitch;
			uint32_t ui32SrcPitch = CDds::DxtScanlineWidth( _ui32Width, _pFormat );
			uint8_t * pui8Dst = reinterpret_cast<uint8_t *>(_pmsSurface->pData);
			const uint8_t * pui8Src = reinterpret_cast<const uint8_t *>(&_tbTexels[0]);
			for ( uint32_t Y = CDds::DxtTotalScanlines( _ui32Height ); Y--; ) {
				CStd::MemCpy( pui8Dst, pui8Src, ui32SrcPitch );
				pui8Dst += ui32DstPitch;
				pui8Src += ui32SrcPitch;
			}
			return;
		}

		// Standard formats.
		uint32_t ui32DestTexelSize = CImageLib::GetFormatSize( _pFormat );
		uint32_t ui32SrcTexelSize = ui32DestTexelSize;
		uint32_t ui32Pitch = _pmsSurface->RowPitch;
		uint32_t ui32YPitchOff;
		uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(_pmsSurface->pData);
		uint32_t ui32RowSize = CImageLib::GetRowSize( _pFormat, _ui32Width );

		// Shifts for swapping red and blue.
		uint64_t ui64RedMask = CImageLib::GetComponentBits( _pFormat, LSI_PC_R );
		ui64RedMask = ((1ULL << ui64RedMask) - 1ULL);
		uint64_t ui64RedShift = CImageLib::GetComponentOffset( _pFormat, LSI_PC_R );
		uint64_t ui64RemoveRed = ~(ui64RedMask << ui64RedShift);

		uint64_t ui64BlueMask = CImageLib::GetComponentBits( _pFormat, LSI_PC_B );
		ui64BlueMask = ((1ULL << ui64BlueMask) - 1ULL);
		uint64_t ui64BlueShift = CImageLib::GetComponentOffset( _pFormat, LSI_PC_B );
		uint64_t ui64RemoveBlue = ~(ui64BlueMask << ui64BlueShift);

		LSBOOL bInvertRgb = Direct3DInvertFormat( _pFormat );
#define LSG_TEXEL_AT( X, Y )			(*reinterpret_cast<const uint64_t *>(&_tbTexels[Y*ui32RowSize+X*ui32SrcTexelSize]))
		switch ( ui32DestTexelSize ) {
			case 1 : {
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						// The only one-byte format is A8, so the alpha channel does not need to be adjusted.
						uint64_t ui64This = LSG_TEXEL_AT( X, Y );

						(*reinterpret_cast<uint8_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint8_t>(ui64This);
					}
				}
				break;
			}
			case 2 : {
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						uint64_t ui64This = LSG_TEXEL_AT( X, Y );

						// Swap if necessary.
						if ( bInvertRgb ) {
							uint64_t ui64Red = (ui64This >> ui64RedShift) & ui64RedMask;
							uint64_t ui64Blue = (ui64This >> ui64BlueShift) & ui64BlueMask;
							ui64This &= ui64RemoveRed;
							ui64This &= ui64RemoveBlue;

							ui64This |= ui64Red << ui64BlueShift;
							ui64This |= ui64Blue << ui64RedShift;
						}

						(*reinterpret_cast<uint16_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint16_t>(ui64This);
					}
				}
				break;
			}
			case 3 : {
				// R8G8B8 only.
				// Special case where the destination has an extra channel.
				++ui32DestTexelSize;
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						uint64_t ui64This = LSG_TEXEL_AT( X, Y );

						// Swap if necessary.
						if ( bInvertRgb ) {
							uint64_t ui64Red = (ui64This >> ui64RedShift) & ui64RedMask;
							uint64_t ui64Blue = (ui64This >> ui64BlueShift) & ui64BlueMask;
							ui64This &= ui64RemoveRed;
							ui64This &= ui64RemoveBlue;

							ui64This |= ui64Red << ui64BlueShift;
							ui64This |= ui64Blue << ui64RedShift;
						}

						uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&pui8Dest[ui32SurfIndex]);
						uint32_t ui32Mix = static_cast<uint32_t>((ui64This & 0x00FFFFFF) | (*pui32Dst) & 0xFF000000);
						(*pui32Dst) = ui32Mix;
					}
				}
				break;
			}
			case 4 : {
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						uint64_t ui64This = LSG_TEXEL_AT( X, Y );
						if ( _pFormat == LSI_PF_R8G8B8A8 ) {
							ui64This = CStd::ByteSwap32( static_cast<uint32_t>(ui64This) );
						}

						// Swap if necessary.
						if ( bInvertRgb ) {
							uint64_t ui64Red = (ui64This >> ui64RedShift) & ui64RedMask;
							uint64_t ui64Blue = (ui64This >> ui64BlueShift) & ui64BlueMask;
							ui64This &= ui64RemoveRed;
							ui64This &= ui64RemoveBlue;

							ui64This |= ui64Red << ui64BlueShift;
							ui64This |= ui64Blue << ui64RedShift;
						}

						(*reinterpret_cast<uint32_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint32_t>(ui64This);
					}
				}
				break;
			}
			case 8 : {
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						uint64_t ui64This = LSG_TEXEL_AT( X, Y );

						if ( _pFormat == LSI_PF_R16G16B16A16 ) {
							ui64This = CStd::ByteSwap64( ui64This );
						}

						// Swap if necessary.
						if ( bInvertRgb ) {
							uint64_t ui64Red = (ui64This >> ui64RedShift) & ui64RedMask;
							uint64_t ui64Blue = (ui64This >> ui64BlueShift) & ui64BlueMask;
							ui64This &= ui64RemoveRed;
							ui64This &= ui64RemoveBlue;

							ui64This |= ui64Red << ui64BlueShift;
							ui64This |= ui64Blue << ui64RedShift;
						}

						(*reinterpret_cast<uint64_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint64_t>(ui64This);
					}
				}
				break;
			}
			case 16 : {
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						const float * pfThis = reinterpret_cast<const float *>(&_tbTexels[Y*ui32RowSize+X*ui32SrcTexelSize]);

						
						// Swap if necessary.
						if ( bInvertRgb ) {
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*0])) = pfThis[2];
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*1])) = pfThis[1];
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*2])) = pfThis[0];
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*3])) = pfThis[3];
						}
						else {
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*0])) = pfThis[0];
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*1])) = pfThis[1];
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*2])) = pfThis[2];
							(*reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex+sizeof( float )*3])) = pfThis[3];
						}
					}
				}
				break;
			}
		}
#undef LSG_TEXEL_AT
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11
