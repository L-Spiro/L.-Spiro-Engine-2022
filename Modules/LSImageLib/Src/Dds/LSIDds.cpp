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
 * Description: Handles loading of .DDS files.
 */

#include "LSIDds.h"


namespace lsi {

	// == Members.
	/** Determines the mode for decoding DXT textures.  Most implementations use standard math, which creates an ideal decoding of DXT1 (and friends) data,
	 *	but this flag can be used to switch to using the algorithm that NVIDIA GPU's use: http://www.ludicon.com/castano/blog/2009/03/gpu-dxt-decompression/
	 */
	LSBOOL CDds::m_bUseNVidiaDecode = true;

	// == Functions.
	/**
	 * Loads a DDS file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \param _vMipMaps The returned mipmap data, if any.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CDds::LoadDds( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {
		// The size must be at least the header and magic number.
		if ( _ui32DataLen < sizeof( uint32_t ) + sizeof( LSI_DDS_HEADER ) ) { return false; }

		// Read the magic number.
		if ( (*reinterpret_cast<const uint32_t *>(_pui8FileData)) != 0x20534444 ) { return false; }
		_pui8FileData += sizeof( uint32_t );
		_ui32DataLen -= sizeof( uint32_t );

		// Get the header.
		const LSI_DDS_HEADER * pdhHeader = reinterpret_cast<const LSI_DDS_HEADER *>(_pui8FileData);
		if ( pdhHeader->dpPixelFormat.ui32Size != sizeof( pdhHeader->dpPixelFormat ) ) { return false; }
		
		if ( (pdhHeader->ui32Flags & (LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT)) !=
			(LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT) ) {
			if ( (pdhHeader->ui32Flags & (LSI_DF_HEIGHT | LSI_DF_WIDTH)) !=
				(LSI_DF_HEIGHT | LSI_DF_WIDTH) ) {
				return false;
			}
		}
		_ui32Width = pdhHeader->ui32Width;
		_ui32Height = pdhHeader->ui32Height;

		uint32_t ui32RemainingDataSize = _ui32DataLen - sizeof( LSI_DDS_HEADER );

		if ( pdhHeader->dpPixelFormat.ui32Flags & LSI_DPFF_FOURCC ) {
			switch ( pdhHeader->dpPixelFormat.ui32FourCC ) {
				case LSE_MAKEFOURCC( 'D', 'X', '1', '0' ) : {}
				case LSE_MAKEFOURCC( 'D', 'X', 'T', '1' ) : {}
				case LSE_MAKEFOURCC( 'D', 'X', 'T', '2' ) : {}
				case LSE_MAKEFOURCC( 'D', 'X', 'T', '3' ) : {}
				case LSE_MAKEFOURCC( 'D', 'X', 'T', '4' ) : {}
				case LSE_MAKEFOURCC( 'D', 'X', 'T', '5' ) : {}
				case LSE_MAKEFOURCC( 'A', 'T', 'I', '1' ) : {}
				case LSE_MAKEFOURCC( 'A', 'T', 'I', '2' ) : {
					const LSI_DDS_HEADER_DXT10 * pdhHeaderExt = NULL;
					const uint8_t * pui8Read = NULL;
					if ( pdhHeader->dpPixelFormat.ui32FourCC == LSE_MAKEFOURCC( 'D', 'X', '1', '0' ) ) {
						_pui8FileData += sizeof( LSI_DDS_HEADER );
						_ui32DataLen -= sizeof( LSI_DDS_HEADER );
						ui32RemainingDataSize -= sizeof( LSI_DDS_HEADER_DXT10 );
						pdhHeaderExt = reinterpret_cast<const LSI_DDS_HEADER_DXT10 *>(_pui8FileData);
						pui8Read = reinterpret_cast<const uint8_t *>(&pdhHeaderExt[1]);
						_pfFormat = DxtPixelFormat( pdhHeaderExt->ui32DxgiFormat );
					}
					else {
						pui8Read = reinterpret_cast<const uint8_t *>(&pdhHeader[1]);
						_pfFormat = DxtPixelFormat( pdhHeader->dpPixelFormat.ui32FourCC );
					}

					//uint32_t ui32ScanLine = CDds::DxtScanlineWidth( _ui32Width, _pfFormat );

					uint32_t ui32Size = GetCompressedSize( _ui32Width, _ui32Height, DxtBlockSize( _pfFormat ) );
					if ( ui32RemainingDataSize < ui32Size ) { return false; }
					if ( !_tbReturn.Resize( ui32Size ) ) { return false; }
					CStd::MemCpy( &_tbReturn[0], pui8Read, ui32Size );

					if ( pdhHeader->ui32Flags & LSI_DF_MIPMAPCOUNT ) {
						uint32_t ui32Total = pdhHeader->ui32MipMapCount - 1;
						if ( !_vMipMaps.Resize( ui32Total ) ) { return false; }
						const uint8_t * pui8Src = pui8Read;
						// Mipmapped texture.
						
						ui32RemainingDataSize -= ui32Size;
						uint32_t ui32W = _ui32Width >> 1;
						uint32_t ui32H = _ui32Height >> 1;
						for ( uint32_t I = 0; I < ui32Total; ++I ) {
							pui8Src += ui32Size;
							ui32Size = GetCompressedSize( ui32W, ui32H, DxtBlockSize( _pfFormat ) );

							if ( ui32RemainingDataSize < ui32Size ) { return false; }
							if ( !_vMipMaps[I].Resize( ui32Size ) ) { return false; }
							CStd::MemCpy( &_vMipMaps[I][0], pui8Src, ui32Size );

							ui32RemainingDataSize -= ui32Size;
							ui32W >>= 1;
							ui32H >>= 1;
							ui32W = CStd::Max<uint32_t>( ui32W, 1 );
							ui32H = CStd::Max<uint32_t>( ui32H, 1 );
						}
					}
					
					return true;
				}
				default : {
					// Unsupported FourCC format.
					return false;
				}
			}
		}

		return false;
	}

	/**
	 * Returns the size, in pixels, of a block of a DDS file.
	 *
	 * \param _pfFormat The DXT format whose block size is to be obtained.
	 * \return Returns the size, in pixels, of a block of a DDS file.
	 */
	uint32_t LSE_CALL CDds::DxtBlockSize( LSI_PIXEL_FORMAT _pfFormat ) {
		return (_pfFormat == LSI_PF_DXT1 || _pfFormat == LSI_PF_BC4
			|| _pfFormat == LSI_PF_BC4S) ? 8 : 16;
	}

	/**
	 * Returns the pixel format of the given four-character code.
	 *
	 * \param _ui32FourCc The FourCC to be converted into a pixel format.
	 * \return Returns the pixel format of the given four-character code.
	 */
	LSI_PIXEL_FORMAT LSE_CALL CDds::DxtPixelFormat( uint32_t _ui32FourCc ) {
		switch ( _ui32FourCc ) {
			case LSE_MAKEFOURCC( 'D', 'X', 'T', '1' ) : { return LSI_PF_DXT1; }
			case LSE_MAKEFOURCC( 'D', 'X', 'T', '2' ) : { return LSI_PF_DXT2; }
			case LSE_MAKEFOURCC( 'D', 'X', 'T', '3' ) : { return LSI_PF_DXT3; }
			case LSE_MAKEFOURCC( 'D', 'X', 'T', '4' ) : { return LSI_PF_DXT4; }
			case LSE_MAKEFOURCC( 'D', 'X', 'T', '5' ) : { return LSI_PF_DXT5; }
			case LSE_MAKEFOURCC( 'A', 'T', 'I', '1' ) : { return LSI_PF_BC4; }
			case LSE_MAKEFOURCC( 'A', 'T', 'I', '2' ) : { return LSI_PF_BC5; }
			case 71 : { return LSI_PF_BC1; }				// DXGI_FORMAT_BC1_UNORM
			case 74 : { return LSI_PF_BC2; }				// DXGI_FORMAT_BC2_UNORM
			case 77 : { return LSI_PF_BC3; }				// DXGI_FORMAT_BC3_UNORM
			case 80 : { return LSI_PF_BC4U; }				// DXGI_FORMAT_BC4_UNORM
			case 81 : { return LSI_PF_BC4S; }				// DXGI_FORMAT_BC4_SNORM
			case 83 : { return LSI_PF_BC5U; }				// DXGI_FORMAT_BC5_UNORM
			case 84 : { return LSI_PF_BC5S; }				// DXGI_FORMAT_BC5_SNORM
			case 95 : { return LSI_PF_BC6U; }				// DXGI_FORMAT_BC6H_UF16
			case 96 : { return LSI_PF_BC6S; }				// DXGI_FORMAT_BC6H_SF16
			case 98 : { return LSI_PF_BC7; }				// DXGI_FORMAT_BC7_UNORM
			case 99 : { return LSI_PF_BC7_SRGB; }			// DXGI_FORMAT_BC7_UNORM_SRGB
			default : { return LSI_PF_ANY; }
		}
	}

	/**
	 * Gets the pitch of a FourCC image format.
	 *
	 * \param _ui32Width The width of the image.
	 * \param _pfFormat The format of the image.
	 * \return Returns the width, in bytes, of a scanline in the given DDS format.
	 */
	uint32_t LSE_CALL CDds::DxtScanlineWidth( uint32_t _ui32Width, LSI_PIXEL_FORMAT _pfFormat ) {
		return ((_ui32Width + 3) >> 2) * DxtBlockSize( _pfFormat );
	}

	/**
	 * Gets the total number of scanlines in a given DDS format.
	 *
	 * \param _ui32Height Height of the image.
	 * \return Returns the number of scanlines in the given DDS format.
	 */
	uint32_t LSE_CALL CDds::DxtTotalScanlines( uint32_t _ui32Height ) {
		return ((_ui32Height + 3) >> 2);
	}

	/**
	 * Returns the total size of a compressed image given a factor and its width and height.
	 *
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _ui32Factor Multiplier.
	 * \return Returns the size of the compressed data.
	 */
	uint32_t LSE_CALL CDds::GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Factor ) {
		return ((_ui32Width + 3) >> 2) * ((_ui32Height + 3) >> 2) * _ui32Factor;
	}

	/**
	 * Decompresses a texel buffer from DXT1.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_DXT1( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		if ( !_ptbSrc || !_ptbDst ) { return false; }
		/*_pfFormat = LSI_PF_R8G8B8A8;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		squish::DecompressImage( reinterpret_cast<squish::u8 *>(&(*_ptbDst)[0]), _ui32Width, _ui32Height, &(*_ptbSrc)[0], squish::kDxt1 );
		*/

		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }

		_bSrgb ? Dxt1ToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1, nullptr ) : Dxt1ToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1, nullptr );
		return true;
	}

	/**
	 * Decompresses a texel buffer from DXT3.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_DXT3( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		/*_pfFormat = LSI_PF_R8G8B8A8;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		squish::DecompressImage( reinterpret_cast<squish::u8 *>(&(*_ptbDst)[0]), _ui32Width, _ui32Height, &(*_ptbSrc)[0], squish::kDxt3 );
		*/

		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }

		_bSrgb ? Dxt3ToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Dxt3ToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from DXT5.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_DXT5( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		/*_pfFormat = LSI_PF_R8G8B8A8;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		squish::DecompressImage( reinterpret_cast<squish::u8 *>(&(*_ptbDst)[0]), _ui32Width, _ui32Height, &(*_ptbSrc)[0], squish::kDxt5 );
		*/
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }

		_bSrgb ? Dxt5ToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Dxt5ToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC4U.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC4U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		
		_bSrgb ? Bc4uToRgba32F<true, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc4uToRgba32F<false, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC4S.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC4S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		_bSrgb ? Bc4sToRgba32F<true, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc4sToRgba32F<false, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC5U.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC5U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }

		_bSrgb ? Bc5uToRgba32F<true, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc5uToRgba32F<false, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC5S.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC5S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }

		_bSrgb ? Bc5sToRgba32F<true, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc5sToRgba32F<false, false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC6U.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC6U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		_bSrgb ? Bc6hToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc6hToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC6S.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC6S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		_bSrgb ? Bc6hToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc6hToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC6U.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC7U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		_bSrgb ? Bc7uToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc7uToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decompresses a texel buffer from BC6S.
	 *
	 * \param _pfFormat The returned format of the decompresed data.
	 * \param _ptbSrc The source buffer of blocks.
	 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
	 * \return Returns true or false to indicate the success of the conversion.
	 */
	LSBOOL LSE_CALL CDds::Decompress_BC7S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		_pfFormat = LSI_PF_R32G32B32A32F;
		if ( !_ptbDst->Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		_bSrgb ? Bc7uToRgba32F<true>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 ) : Bc7uToRgba32F<false>( &(*_ptbSrc)[0], &(*_ptbDst)[0], _ui32Width, _ui32Height, 1 );
		return true;
	}

	/**
	 * Decodes a single block of DXT3 alpha.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pfPalette The created palette (contains 16 entries).
	 */
	void LSE_CALL CDds::DecodeDXT3_Alpha( uint64_t _ui64Block, float * _pfPalette ) {
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pfPalette++) = (_ui64Block & 0xF) / 15.0f;
			_ui64Block >>= 4;
		}
	}

	/**
	 * Decodes a single block of BC4U.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pfPalette The created palette (contains 8 entries).
	 */
	void LSE_CALL CDds::DecodeBC4U( uint64_t _ui64Block, float * _pfPalette ) {
		_pfPalette[0] = ((_ui64Block >> 0) & 0xFF) / 255.0f;
		_pfPalette[1] = ((_ui64Block >> 8) & 0xFF) / 255.0f;
		if ( _pfPalette[0] > _pfPalette[1] ) {
			// 6 interpolated color values.
			_pfPalette[2] = (6.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 7.0f;	// Bit code 010.
			_pfPalette[3] = (5.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 7.0f;	// Bit code 011.
			_pfPalette[4] = (4.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 7.0f;	// Bit code 100.
			_pfPalette[5] = (3.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 7.0f;	// Bit code 101.
			_pfPalette[6] = (2.0f * _pfPalette[0] + 5.0f * _pfPalette[1]) / 7.0f;	// Bit code 110.
			_pfPalette[7] = (1.0f * _pfPalette[0] + 6.0f * _pfPalette[1]) / 7.0f;	// Bit code 111.
		}
		else {
			// 4 interpolated color values.
			_pfPalette[2] = (4.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 5.0f;	// Bit code 010.
			_pfPalette[3] = (3.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 5.0f;	// Bit code 011.
			_pfPalette[4] = (2.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 5.0f;	// Bit code 100.
			_pfPalette[5] = (1.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 5.0f;	// Bit code 101.
			_pfPalette[6] = 0.0f;													// Bit code 110.
			_pfPalette[7] = 1.0f;													// Bit code 111.
		}
	}

	/**
	 * Decodes a single block of BC4S.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pfPalette The created palette (contains 8 entries).
	 */
	void LSE_CALL CDds::DecodeBC4S( uint64_t _ui64Block, float * _pfPalette ) {
		int8_t i8C0 = static_cast<int8_t>(_ui64Block >> 0);
		int8_t i8C1 = static_cast<int8_t>(_ui64Block >> 8);
		_pfPalette[0] = i8C0 == -128 ? -1.0f : i8C0 / 127.0f;
		_pfPalette[1] = i8C1 == -128 ? -1.0f : i8C1 / 127.0f;
		if ( _pfPalette[0] > _pfPalette[1] ) {
			// 6 interpolated color values.
			_pfPalette[2] = (6.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 7.0f;	// Bit code 010.
			_pfPalette[3] = (5.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 7.0f;	// Bit code 011.
			_pfPalette[4] = (4.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 7.0f;	// Bit code 100.
			_pfPalette[5] = (3.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 7.0f;	// Bit code 101.
			_pfPalette[6] = (2.0f * _pfPalette[0] + 5.0f * _pfPalette[1]) / 7.0f;	// Bit code 110.
			_pfPalette[7] = (1.0f * _pfPalette[0] + 6.0f * _pfPalette[1]) / 7.0f;	// Bit code 111.
		}
		else {
			// 4 interpolated color values.
			_pfPalette[2] = (4.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 5.0f;	// Bit code 010.
			_pfPalette[3] = (3.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 5.0f;	// Bit code 011.
			_pfPalette[4] = (2.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 5.0f;	// Bit code 100.
			_pfPalette[5] = (1.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 5.0f;	// Bit code 101.
			_pfPalette[6] = -1.0f;													// Bit code 110.
			_pfPalette[7] = 1.0f;													// Bit code 111.
		}
	}

	/**
	 * Gets the indices from a DXT1 block. 
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pui8Indices The 16 indices as extracted from the block.
	 */
	void LSE_CALL CDds::Dxt1Indices( uint64_t _ui64Block, uint8_t * _pui8Indices ) {
		_ui64Block >>= 32;
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pui8Indices++) = _ui64Block & 0x3;
			_ui64Block >>= 2;
		}
	}

	/**
	 * Gets the indices from a BC4 block. 
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pui8Indices The 16 indices as extracted from the block.
	 */
	void LSE_CALL CDds::Bc4Indices( uint64_t _ui64Block, uint8_t * _pui8Indices ) {
		_ui64Block >>= 16;
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pui8Indices++) = _ui64Block & 0x7;
			_ui64Block >>= 3;
		}
	}

	/**
	 * Thread function for converting a block of texels to the BC6 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_BC6_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CDds::Bc6Thread( void * _lpParameter ) {
		LSI_BC6_THREAD_DATA * pbtdData = static_cast<LSI_BC6_THREAD_DATA *>(_lpParameter);
		CompressBlocksBC6H( pbtdData->psSurface, pbtdData->pui8Dst, pbtdData->pbesSettings );
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Thread function for converting a block of texels to the BC7 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_BC7_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CDds::Bc7Thread( void * _lpParameter ) {
		LSI_BC7_THREAD_DATA * pbtdData = static_cast<LSI_BC7_THREAD_DATA *>(_lpParameter);
		CompressBlocksBC7( pbtdData->psSurface, pbtdData->pui8Dst, pbtdData->pbesSettings );
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Divides a surface into sections that can be passed to a thread so that multiple threads can work on a section at once.
	 *
	 * \param _sadSurface Surface to update and the destination where the result should be stored.
	 * \param _pui8Src Pointer to the source image.
	 * \param _pui8Dst Pointer to the destination image.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32RowsPerSection Maximum number of rows to put into each section.
	 * \param _ui32ThisSectionIdx Index of this section.
	 * \param _pfFormat The target format.
	 */
	void LSE_CALL CDds::DivideSurface( LSI_SURFACE_AND_DST &_sadSurface, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t _ui32RowsPerSection, uint32_t _ui32ThisSectionIdx, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32SrcStride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width );
		uint32_t ui32DstStride = CDds::DxtScanlineWidth( _ui32Width, _pfFormat );
		uint32_t ui32TotalRowsAvailable = LSE_ROUND_UP_ANY( _ui32Height, _ui32RowsPerSection ) / _ui32RowsPerSection;
		if ( _ui32ThisSectionIdx < ui32TotalRowsAvailable ) {
			uint32_t ui32RowStart = _ui32ThisSectionIdx * _ui32RowsPerSection;
			uint32_t ui32Rows = CStd::Min( _ui32Height - ui32RowStart, _ui32RowsPerSection );

			_sadSurface.sSurface.width = _ui32Width;
			_sadSurface.sSurface.height = ui32Rows;
			_sadSurface.sSurface.stride = ui32SrcStride;
			_sadSurface.sSurface.ptr = _pui8Src + (ui32SrcStride * ui32RowStart);
			_sadSurface.pui8Dst = _pui8Dst + (ui32DstStride * ui32RowStart / 4);
		}
	}

	/**
	 * Divides a surface into sections that can be passed to a thread so that multiple threads can work on a section at once.
	 *
	 * \param _vSurfaces Holds the returned sections, each pointing to a different part of the image.
	 * \param _pui8Src Pointer to the source image.
	 * \param _pui8Dst Pointer to the destination image.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32RowsPerSection Maximum number of rows to put into each section.
	 * \param _pfFormat The target format.
	 * \return Returns true if the sections were allocated.
	 */
	bool LSE_CALL CDds::DivideSurface( CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> &_vSurfaces, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t _ui32RowsPerSection, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32TotalRowsAvailable = LSE_ROUND_UP_ANY( _ui32Height, _ui32RowsPerSection ) / _ui32RowsPerSection;
		_vSurfaces.ResetNoDealloc();
		for ( uint32_t I = 0; I < ui32TotalRowsAvailable; ++I ) {
			LSI_SURFACE_AND_DST sadSurface;
			DivideSurface( sadSurface, _pui8Src, _pui8Dst, _ui32Width, _ui32Height, _ui32RowsPerSection, I, _pfFormat );
			if ( !_vSurfaces.Push( sadSurface ) ) { return false; }
		}
		return true;
	}

	/**
	 * Expands an image by rounding its width and height up to 4.  Fills the extra texels with the left-most and bottom-most texels rather than black in order to send
	 *	fewer unique colors to the block comrpessor, helping it create better matches for the texels that are actually in the block.
	 *
	 * \param _prSrc The source image to expand.
	 * \param _vDst The destination to which the image will be expanded.  Failure to allocate this buffer is the only possibility for failure.
	 * \param _ui32Width The width of the image to expand.  Holds the expanded width on exit.
	 * \param _ui32Height The height of the image to expand.  Holds the expanded height on exit.
	 * \param _bSrgb If true, a linear -> sRGB conversion is made during the expansion.
	 * \return Returns true if the buffer for the expanded image was successfuly allocated.
	 */
	bool LSE_CALL CDds::ExpandImageRgbaF32( const LSI_RGBA * _prSrc, CVectorPoD<LSI_RGBA, uint32_t> &_vDst, uint32_t &_ui32Width, uint32_t &_ui32Height, bool _bSrgb ) {
		uint32_t ui32ExpWidth = LSE_ROUND_UP( _ui32Width, 4 );
		uint32_t ui32ExpHeight = LSE_ROUND_UP( _ui32Height, 4 );
		if ( !_vDst.Resize( ui32ExpWidth * ui32ExpHeight ) ) { return false; }
		for ( uint32_t Y = 0; Y < ui32ExpHeight; ++Y ) {
			for ( uint32_t X = 0; X < ui32ExpWidth; ++X ) {
				const LSI_RGBA & prThis = _prSrc[CStd::Min(Y,_ui32Height-1)*_ui32Width+CStd::Min(X,_ui32Width-1)];
				LSI_RGBA & rDst = _vDst[Y*ui32ExpWidth+X];
				if ( _bSrgb ) {
					rDst.fRgba[LSI_PC_R] = CImageLib::LinearToSRgb( prThis.fRgba[LSI_PC_R] );
					rDst.fRgba[LSI_PC_G] = CImageLib::LinearToSRgb( prThis.fRgba[LSI_PC_G] );
					rDst.fRgba[LSI_PC_B] = CImageLib::LinearToSRgb( prThis.fRgba[LSI_PC_B] );
					rDst.fRgba[LSI_PC_A] = prThis.fRgba[LSI_PC_A];
				}
				else {
					rDst = prThis;
				}
			}
		}
		_ui32Width = ui32ExpWidth;
		_ui32Height = ui32ExpHeight;
		return true;
	}

}	// namespace lsi

#undef LSI_DXT_ROWS
