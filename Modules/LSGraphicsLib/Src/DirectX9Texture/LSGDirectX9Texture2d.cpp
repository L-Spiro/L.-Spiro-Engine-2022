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
 * Description: A 2D Direct3D 9 texture.
 */

#include "LSGDirectX9Texture2d.h"

#ifdef LSG_DX9

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9Texture2d::CDirectX9Texture2d() :
		m_pd3dtTexture( NULL ) {
	}
	LSE_CALLCTOR CDirectX9Texture2d::~CDirectX9Texture2d() {
		Reset();
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void LSE_CALL CDirectX9Texture2d::OnDeviceLost() {
		/*CCriticalSection::CLocker lLock( m_csBaseCrit );
		for ( uint32_t I = LSG_MAX_TEXTURE_UNITS; I--; ) {
			if ( m_ui32LastTextures[I] == m_ui32Id ) {
				m_ui32LastTextures[I] = 0UL;
			}
		}*/
		if ( !m_bResourceCanBeLost || !m_pd3dtTexture ) { return; }

		CDirectX9::SafeRelease( m_pd3dtTexture );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	LSBOOL LSE_CALL CDirectX9Texture2d::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		return CreateApiTexture( NULL, 0 );
	}

	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL CDirectX9Texture2d::Reset() {
		CDirectX9::SafeRelease( m_pd3dtTexture );

		m_bResourceCanBeLost = false;
		m_iOrigTexels.Reset();
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
	LSBOOL LSE_CALL CDirectX9Texture2d::SetAsColorRenderTarget( uint32_t _ui32Mipmap, uint32_t /*_ui32Slice*/, uint32_t _ui32Slot ) const {
		if ( !m_pd3dtTexture ) { return false; }
		IDirect3DSurface9 * pd3ds9Surface = NULL;
		if ( FAILED( m_pd3dtTexture->GetSurfaceLevel( _ui32Mipmap, &pd3ds9Surface ) ) ) { return false; }
		CDirectX9::m_rsCurRenderState.prtRenderTargets[_ui32Slot] = pd3ds9Surface;
		CDirectX9::m_rsCurRenderState.ui8TotalRenderTargets = CStd::Max<uint8_t>( CDirectX9::m_rsCurRenderState.ui8TotalRenderTargets, uint8_t( _ui32Slot + 1 ) );
		CDirectX9::SafeRelease( pd3ds9Surface );
		return true;
	}

	/**
	 * Creates a DirectX 9 texture and fills it with our texel data.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL CDirectX9Texture2d::CreateApiTexture( const CImage * _piImages, uint32_t /*_ui32Total*/ ) {
		// _ui32Total and m_ui16ArraySize will always be the same values.
		if ( m_ui16ArraySize != 1 ) { return false; }
		// Usage flags must not contradict.
		if ( (m_ui32Usage & (LSG_TUT_SETONLY | LSG_TUT_DYNAMIC)) == (LSG_TUT_SETONLY | LSG_TUT_DYNAMIC) ) { return false; }

		LSBOOL bUseHardware = (CDirectX9::GetCreationParms().BehaviorFlags & (D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING)) != 0;
		DWORD dwUsage = m_ui32Usage;
		dwUsage |= !bUseHardware ? D3DUSAGE_SOFTWAREPROCESSING : 0;

		// If the resource is being created for the first time, _piImages will not be NULL.
		//	If _piImages is NULL, it means we are recovering from a lost device.
		//	So if it is not NULL, make a copy of the image and perform creation-only manipulation on it.
		if ( _piImages ) {
			m_iOrigTexels = (*_piImages);

			uint32_t ui32NewWidth = m_iOrigTexels.GetWidth();
			uint32_t ui32NewHeight = m_iOrigTexels.GetHeight();

			// Do textures need to be a power-of-2?
			if ( !CGfx::GetMetrics().bNonPo2 ) {
				ui32NewWidth = CStd::GetLowestPo2( ui32NewWidth );
				ui32NewHeight = CStd::GetLowestPo2( ui32NewHeight );
			}
			// Do textures need to be square?
			if ( CDirectX9::GetDeviceCaps().TextureCaps & D3DPTEXTURECAPS_SQUAREONLY ) {
				ui32NewWidth = CStd::Max( ui32NewWidth, ui32NewHeight );
				ui32NewHeight = ui32NewWidth;
			}
			// Have we made the texture too big?
			ui32NewWidth = CStd::Min<uint32_t>( ui32NewWidth, CGfx::GetMetrics().ui32MaxTextureWidth );
			ui32NewHeight = CStd::Min<uint32_t>( ui32NewHeight, CGfx::GetMetrics().ui32MaxTextureHeight );


			if ( ui32NewWidth != m_iOrigTexels.GetWidth() || ui32NewHeight != m_iOrigTexels.GetHeight() ) {
				// Resampling is required.
				LSI_FILTER fFilter = (ui32NewWidth >= m_iOrigTexels.GetWidth() || ui32NewHeight >= m_iOrigTexels.GetHeight()) ?
					LSI_F_BILINEAR_FILTER : LSI_F_KAISER_FILTER;
				if ( !m_iOrigTexels.ResampleInPlace( ui32NewWidth, ui32NewHeight, fFilter, m_iOrigTexels.IsSRgb() ? -2.2f : 1.0f ) ) {
					return false;
				}
			}
		}

		D3DPOOL pPool = Direct3DPool();
		m_bResourceCanBeLost = pPool == D3DPOOL_DEFAULT;

		// Create the resource.
		HRESULT hRes = CDirectX9::GetDirectX9Device()->CreateTexture( m_iOrigTexels.GetWidth(), m_iOrigTexels.GetHeight(),
			m_ui16MipMaps,
			dwUsage,
			Direct3DFormat( m_iOrigTexels.GetFormat() ), pPool,
			&m_pd3dtTexture, NULL );
		if ( FAILED( hRes ) ) { return false; }


		// If the texture is set-only, create a staging texture to fill its texel data.
		if ( pPool == D3DPOOL_DEFAULT ) {
			IDirect3DTexture9 * pd3dtStaging;
			if ( FAILED( CDirectX9::GetDirectX9Device()->CreateTexture( m_iOrigTexels.GetWidth(), m_iOrigTexels.GetHeight(),
				m_ui16MipMaps,
				0UL,
				Direct3DFormat( m_iOrigTexels.GetFormat() ), D3DPOOL_SYSTEMMEM,
				&pd3dtStaging, NULL ) ) ) {
				return false;
			}

			if ( !GenerateMipMaps( pd3dtStaging, m_iOrigTexels ) ) {
				CDirectX9::SafeRelease( pd3dtStaging );
				return false;
			}

			LSBOOL bRet = SUCCEEDED( CDirectX9::GetDirectX9Device()->UpdateTexture( pd3dtStaging, m_pd3dtTexture ) );

			CDirectX9::SafeRelease( pd3dtStaging );
			if ( !bRet ) {
				return false;
			}
		}
		else {
			// We can fill the texel data directly.
			if ( !GenerateMipMaps( m_pd3dtTexture, m_iOrigTexels ) ) { return false; }
		}

		if ( !m_bResourceCanBeLost ) {
			// Don't need a copy of the texel data.
			m_iOrigTexels.Reset();
		}

		return true;
	}

	/**
	 * Activates the texture in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
	 * \return Returns true if the texture was activated in the given slot.
	 */
	LSBOOL LSE_CALL CDirectX9Texture2d::ActivateApi( uint32_t _ui32Slot ) {
		CDirectX9::m_rsCurRenderState.bSRgb[_ui32Slot] = m_bSRgb;
		return true;
	}

	/**
	 * Handles mipmap generation for the given texture, which must be lockable.  Layer 0 (the non-scaled level) is also handled by this
	 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
	 *
	 * \param _pd3dtTexture The texture for which to generate mipmaps.
	 * \param _iImage The image to use for the mipmapping process.
	 * \return Returns true if there were no failures.  Failure implies the resource was not lockable.
	 */
	LSBOOL LSE_CALL CDirectX9Texture2d::GenerateMipMaps( IDirect3DTexture9 * _pd3dtTexture, const CImage &_iImage ) {
		// Reroute DXT textures.
		switch ( _iImage.GetFormat() ) {
			case LSI_PF_DXT1 : {}
			case LSI_PF_DXT2 : {}
			case LSI_PF_DXT3 : {}
			case LSI_PF_DXT4 : {}
			case LSI_PF_DXT5 : {
				return GenerateDxtMipMaps( _pd3dtTexture, _iImage );
			}
		}

		uint32_t ui32Width = _iImage.GetWidth(), ui32Height = _iImage.GetHeight();
		CImage iMipMap;
		for ( uint32_t I = 0; I < m_ui16MipMaps; ++I ) {
			// If the mipmap is not part of the original data, generate it.
			const CImageLib::CTexelBuffer * ptbUseMe;
			if ( I >= _iImage.TotalMipLevels() ) {
				if ( !_iImage.Resample( ui32Width, ui32Height, LSI_F_KAISER_FILTER, iMipMap, _iImage.IsSRgb() ? -2.2f : 1.0f ) ) {
					// Memory failure or DXT format.
					return false;
				}
				ptbUseMe = &iMipMap.GetMipMapBuffers( 0 );
			}
			else {
				ptbUseMe = &_iImage.GetMipMapBuffers( I );
			}
			IDirect3DSurface9 * pd3ds9Surface;
			if ( FAILED( _pd3dtTexture->GetSurfaceLevel( I, &pd3ds9Surface ) ) ) {
				// Lock failure.
				return false;
			}

			if ( !GenerateMipMaps( pd3ds9Surface, (*ptbUseMe), _iImage.GetFormat(),
				ui32Width, ui32Height ) ) {
				CDirectX9::SafeRelease( pd3ds9Surface );
				return false;
			}
			CDirectX9::SafeRelease( pd3ds9Surface );

			ui32Width = CStd::Max<uint32_t>( 1, ui32Width >> 1 );
			ui32Height = CStd::Max<uint32_t>( 1, ui32Height >> 1 );
		}
		return true;
	}

	/**
	 * Handles mipmap generation for the given texture, which must be lockable.  Layer 0 (the non-scaled level) is also handled by this
	 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
	 *
	 * \param _pd3dtTexture The texture for which to generate mipmaps.
	 * \param _iImage The image to use for the mipmapping process.
	 * \return Returns true if there were no failures.  Failure implies the resource was not lockable or _iImage does not
	 *	contain the proper number of mipmap levels.
	 */
	LSBOOL LSE_CALL CDirectX9Texture2d::GenerateDxtMipMaps( IDirect3DTexture9 * _pd3dtTexture, const CImage &_iImage ) {
		// Can't account for missing mipmaps so ensure the number in _iImage matches the number we need.
		if ( m_ui16MipMaps != _iImage.TotalMipLevels() ) { return false; }
		uint32_t ui32Width = _iImage.GetWidth(), ui32Height = _iImage.GetHeight();
		for ( uint32_t I = 0; I < m_ui16MipMaps; ++I ) {
			IDirect3DSurface9 * pd3ds9Surface;
			LSBOOL bRet = false;
			if ( SUCCEEDED( _pd3dtTexture->GetSurfaceLevel( I, &pd3ds9Surface ) ) ) {
				// The texture is not in the default pool so we can lock and use it directly.
				// Get the full rectangle.
				D3DLOCKED_RECT lrLocked;
				if ( SUCCEEDED( pd3ds9Surface->LockRect( &lrLocked, NULL, 0 ) ) ) {
					uint32_t ui32SrcScanlineSize = CDds::DxtScanlineWidth( ui32Width, _iImage.GetFormat() );
					const uint8_t * pui8Src = &_iImage.GetMipMapBuffers( I )[0];
					uint8_t * pui8Dst = static_cast<uint8_t *>(lrLocked.pBits);

					for ( uint32_t Y = CDds::DxtTotalScanlines( ui32Height ); Y--; ) {
						CStd::MemCpy( pui8Dst, pui8Src, ui32SrcScanlineSize );
						pui8Src += ui32SrcScanlineSize;
						pui8Dst += lrLocked.Pitch;
					}
					bRet = true;
					pd3ds9Surface->UnlockRect();
				}

				CDirectX9::SafeRelease( pd3ds9Surface );
			}

			if ( !bRet ) { return false; }


			ui32Width = CStd::Max<uint32_t>( 1, ui32Width >> 1 );
			ui32Height = CStd::Max<uint32_t>( 1, ui32Height >> 1 );
		}
		return true;
	}

	/**
	 * Handles mipmap generation for the given texture, which must be lockable.  Layer 0 (the non-scaled level) is also handled by this
	 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
	 *
	 * \param _pd3dsSurface The surface to which to copy our texel data.
	 * \param _tbTexels The texel data to copy.
	 * \param _pfFormat Format of the data to copy.
	 * \param _ui32Width Width of the image data.
	 * \param _ui32Height Height of the image data.
	 * \return Returns true if there were no failures.  Failure implies the resource was not lockable.
	 */
	LSBOOL LSE_CALL CDirectX9Texture2d::GenerateMipMaps( IDirect3DSurface9 * _pd3dsSurface,
		const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height ) {
		// The texture is not in the default pool so we can lock and use it directly.
		// Get the full rectangle.
		D3DLOCKED_RECT lrLocked;
		if ( FAILED( _pd3dsSurface->LockRect( &lrLocked, NULL, 0 ) ) ) { return false; }

		uint32_t ui32DestTexelSize = CImageLib::GetFormatSize( _pfFormat );
		uint32_t ui32Pitch = lrLocked.Pitch;
		uint32_t ui32YPitchOff;

		uint64_t ui64AlphaRgbShift = CImageLib::GetComponentBits( _pfFormat, LSI_PC_A );
		uint64_t ui64AlphaMask = ~((1ULL << ui64AlphaRgbShift) - 1ULL);
		uint64_t ui64AlphaShift = (CImageLib::GetFormatSize( _pfFormat ) * 8ULL) - ui64AlphaRgbShift;
		uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(lrLocked.pBits);
		uint32_t ui32SrcRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
		uint32_t ui32SrcTexelSize = CImageLib::GetFormatSize( _pfFormat );
		switch ( ui32DestTexelSize ) {
			case 1 : {
				for ( uint32_t Y = 0UL; Y < _ui32Height; ++Y ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = 0UL; X < _ui32Width; ++X ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						// The only one-byte format is A8, so the alpha channel does not need to be adjusted.
						uint64_t ui64This = GetTexelAt( _tbTexels, _pfFormat, _pfFormat, ui32SrcRowWidth, ui32SrcTexelSize, X, Y );

						(*reinterpret_cast<uint8_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint8_t>(ui64This);
					}
				}
				break;
			}
			case 2 : {
				for ( uint32_t Y = _ui32Height; Y--; ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = _ui32Width; X--; ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						// DirectX stores alpha on the wrong side for some reason.
						uint64_t ui64This = GetTexelAt( _tbTexels, _pfFormat, _pfFormat, ui32SrcRowWidth, ui32SrcTexelSize, X, Y );
						uint64_t ui64Alpha = ui64This & ~ui64AlphaMask;
						ui64This = (ui64This & ui64AlphaMask) >> ui64AlphaRgbShift;
						ui64This |= (ui64Alpha << ui64AlphaShift);

						(*reinterpret_cast<uint16_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint16_t>(ui64This);
					}
				}
				break;
			}
			case 3 : {
				// R8G8B8 only.
				// Special case where the destination has an extra channel.
				++ui32DestTexelSize;
				for ( uint32_t Y = _ui32Height; Y--; ) {
					ui32YPitchOff = Y * ui32Pitch;

					for ( uint32_t X = _ui32Width; X--; ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						// DirectX stores alpha on the wrong side for some reason.
						uint32_t ui32This = static_cast<uint32_t>(GetTexelAt( _tbTexels, _pfFormat, _pfFormat, ui32SrcRowWidth, ui32SrcTexelSize, X, Y ));

						(*reinterpret_cast<uint32_t *>(&pui8Dest[ui32SurfIndex])) = ui32This;
					}
				}
				break;
			}
			case 4 : {
				uint32_t Y, ui32SurfIndex;
				int32_t X;
				int32_t i32Width = static_cast<int32_t>(_ui32Width);
				volatile uint64_t ui64This, ui64Alpha;	// DirectX Release has a problem with this if it is not volatile.
//#pragma omp parallel private( Y, X, ui32YPitchOff, ui32SurfIndex, ui64This, ui64Alpha )
				for ( Y = _ui32Height; Y--; ) {
					ui32YPitchOff = Y * ui32Pitch;
//#pragma omp for schedule( dynamic )
					for ( X = 0; X < i32Width; ++X ) {
						ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

						// DirectX stores alpha on the wrong side for some reason.
						ui64This = GetTexelAt( _tbTexels, _pfFormat, _pfFormat, ui32SrcRowWidth, ui32SrcTexelSize, X, Y );
						ui64Alpha = ui64This & ~ui64AlphaMask;
						ui64This = (ui64This & ui64AlphaMask) >> ui64AlphaRgbShift;
						ui64This |= (ui64Alpha << ui64AlphaShift);

						(*reinterpret_cast<uint32_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint32_t>(ui64This);
					}
				}
				break;
			}
			case 8 : {
				switch ( _pfFormat ) {
					case LSI_PF_R16G16B16A16F : {
						CFloat16 fRgba[4];
						for ( uint32_t Y = _ui32Height; Y--; ) {
							ui32YPitchOff = Y * ui32Pitch;
							for ( uint32_t X = _ui32Width; X--; ) {
								uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;
								GetTexelAt( _tbTexels, ui32SrcRowWidth, ui32SrcTexelSize, X, Y, fRgba );
								// Move everything down one and put alpha at the front.
								// Swap R and B.
								/*CStd::Swap( fRgba[LSI_PC_R], fRgba[LSI_PC_B] );
								CFloat16 fTemp = fRgba[3];
								fRgba[3] = fRgba[2];
								fRgba[2] = fRgba[1];
								fRgba[1] = fRgba[0];
								fRgba[0] = fTemp;*/

								(*reinterpret_cast<uint64_t *>(&pui8Dest[ui32SurfIndex])) = (*reinterpret_cast<uint64_t *>(fRgba));
							}
						}
						break;
					}
					default : {
						// Since this is ABGR, we also have to swap the R and B channels.
						uint64_t ui64RedMask = CImageLib::GetComponentBits( _pfFormat, LSI_PC_R );
						ui64RedMask = ((1ULL << ui64RedMask) - 1ULL);
						uint64_t ui64RedShift = CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R );
						uint64_t ui64RemoveRed = ~(ui64RedMask << ui64RedShift);

						uint64_t ui64BlueMask = CImageLib::GetComponentBits( _pfFormat, LSI_PC_B );
						ui64BlueMask = ((1ULL << ui64BlueMask) - 1ULL);
						uint64_t ui64BlueShift = CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B );
						uint64_t ui64RemoveBlue = ~(ui64BlueMask << ui64BlueShift);
						for ( uint32_t Y = _ui32Height; Y--; ) {
							ui32YPitchOff = Y * ui32Pitch;
							for ( uint32_t X = _ui32Width; X--; ) {
								uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;

								// DirectX stores alpha on the wrong side for some reason.
								uint64_t ui64This = GetTexelAt( _tbTexels, _pfFormat, _pfFormat, ui32SrcRowWidth, ui32SrcTexelSize, X, Y );
								// Swap red and green.
								uint64_t ui64Red = (ui64This >> ui64RedShift) & ui64RedMask;
								uint64_t ui64Blue = (ui64This >> ui64BlueShift) & ui64BlueMask;
								ui64This &= ui64RemoveRed;
								ui64This &= ui64RemoveBlue;

								ui64This |= ui64Red << ui64BlueShift;
								ui64This |= ui64Blue << ui64RedShift;

								// Move alpha over.
								uint64_t ui64Alpha = ui64This & ~ui64AlphaMask;
								ui64This = (ui64This & ui64AlphaMask) >> ui64AlphaRgbShift;
								ui64This |= (ui64Alpha << ui64AlphaShift);

								(*reinterpret_cast<uint64_t *>(&pui8Dest[ui32SurfIndex])) = static_cast<uint64_t>(ui64This);
							}
						}
					}
				}
				break;
			}
			case 16 : {
				float fRgba[4];
				for ( uint32_t Y = _ui32Height; Y--; ) {
					ui32YPitchOff = Y * ui32Pitch;
					for ( uint32_t X = _ui32Width; X--; ) {
						uint32_t ui32SurfIndex = ui32YPitchOff + X * ui32DestTexelSize;
						GetTexelAt( _tbTexels, ui32SrcRowWidth, ui32SrcTexelSize, X, Y, fRgba );
						// Move everything down one and put alpha at the front.
						// Swap R and B.
						/*CStd::Swap( fRgba[LSI_PC_R], fRgba[LSI_PC_B] );
						float fTemp = fRgba[3];
						fRgba[3] = fRgba[2];
						fRgba[2] = fRgba[1];
						fRgba[1] = fRgba[0];
						fRgba[0] = fTemp;*/

						const uint64_t * pui64Src = reinterpret_cast<uint64_t *>(fRgba);
						uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(&pui8Dest[ui32SurfIndex]);
						(*pui64Dst++) = (*pui64Src++);
						(*pui64Dst) = (*pui64Src);
					}
				}
				break;
			}
		}
		_pd3dsSurface->UnlockRect();
		return true;
	}

	/**
	 * Gets the pool of the texture based on m_ui32Usage.
	 *
	 * \return Returns the pool of the texture based on m_ui32Usage.
	 */
	D3DPOOL LSE_CALL CDirectX9Texture2d::Direct3DPool() const {
		// D3DUSAGE_RENDERTARGET can only be used with D3DPOOL_DEFAULT.
		if ( m_ui32Usage & LSG_TUT_RENDERTARGET ) { return D3DPOOL_DEFAULT; }
		// Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty.
		if ( m_ui32Usage & LSG_TUT_SETONLY ) { return D3DPOOL_DEFAULT; }
		// D3DUSAGE_DYNAMIC and D3DPOOL_MANAGED are incompatible and should not be used together.
		if ( m_ui32Usage & LSG_TUT_DYNAMIC ) { return D3DPOOL_SYSTEMMEM; }
		// Applications should use D3DPOOL_MANAGED for most static resources because this saves the application from having to deal with lost devices. 
		return D3DPOOL_MANAGED;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
