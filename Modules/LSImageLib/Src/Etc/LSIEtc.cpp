/**
 * Copyright L. Spiro 2021
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Handles conversion of ETC formats.
 */

#include "LSIEtc.h"

/** How many rows to do per-thread for ETC1 conversions. */
#define LSI_ETC1_ROWS						24

/** Maximum number of threads to run concurrently for ETC1 conversions. */
#define LSI_ETC1_MAX_THREADS				16


namespace lsi {

	// == Functions.
	/**
	 * Returns the size, in pixels, of a block.
	 *
	 * \param _pfFormat The ETC format whose block size is to be obtained.
	 * \return Returns the size, in pixels, of a block of ETC.
	 */
	uint32_t LSE_CALL CEtc::EtcBlockSize( LSI_PIXEL_FORMAT _pfFormat ) {
		switch ( _pfFormat ) {
			case LSI_PF_ETC2_EAC : {}
			case LSI_PF_ETC2_EAC_SRGB : {}
			case LSI_PF_RG11_EAC : {}
			case LSI_PF_RG11_EACS : { return 128 / 8; }
			default : { return 64 / 8; }
		}
	}

	/**
	 * Gets the pitch of an ETC format.
	 *
	 * \param _ui32Width The width of the image.
	 * \param _pfFormat The format of the image.
	 * \return Returns the width, in bytes, of a scanline in the given ETC format.
	 */
	uint32_t LSE_CALL CEtc::EtcScanlineWidth( uint32_t _ui32Width, LSI_PIXEL_FORMAT _pfFormat ) {
		return ((_ui32Width + 3) >> 2) * EtcBlockSize( _pfFormat );
	}

	/**
	 * Gets the total number of scanlines in a given ETC format.
	 *
	 * \param _ui32Height Height of the image.
	 * \return Returns the number of scanlines in the given ETC format.
	 */
	uint32_t LSE_CALL CEtc::EtcTotalScanlines( uint32_t _ui32Height ) {
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
	uint32_t LSE_CALL CEtc::GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Factor ) {
		return ((_ui32Width + 3) >> 2) * ((_ui32Height + 3) >> 2) * _ui32Factor;
	}

	/**
	 * ETC1 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CEtc::Etc1ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_ETC1_BLOCK {
			uint64_t ui64Block0;
		};
		const LSI_ETC1_BLOCK * pbbBlocks = reinterpret_cast<const LSI_ETC1_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_ETC1_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint8_t fPaletteRgbUi8[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockETC1( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), DETEX_MODE_MASK_ALL_MODES_ETC1,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbUi8) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPaletteRgbUi8[I*4+LSI_PC_R] / 255.0f;
							(*prgbaRow0).fTexels[1] = fPaletteRgbUi8[I*4+LSI_PC_G] / 255.0f;
							(*prgbaRow0).fTexels[2] = fPaletteRgbUi8[I*4+LSI_PC_B] / 255.0f;
							(*prgbaRow0).fTexels[3] = fPaletteRgbUi8[I*4+LSI_PC_A] / 255.0f;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> ETC1 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CEtc::Etc1FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#ifndef LSE_IPHONE
		uint32_t ui32SliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;
		uint32_t ui32DstSliceSize = CEtc::GetCompressedSize( _ui32Width, _ui32Height, CEtc::EtcBlockSize( LSI_PF_ETC1 ) );

#ifdef LSI_ETC1_ROWS
		CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> vSurfaces;
		uint8_t * pui8Surf;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ETC1_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ETC1_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
#else
		rgba_surface sSurface;
		sSurface.width = LSE_ROUND_UP( _ui32Width, 4 );
		sSurface.height = LSE_ROUND_UP( _ui32Height, 4 );
		sSurface.stride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, sSurface.width );
#endif	// #ifdef LSI_ETC1_ROWS
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))

		struct LSI_RGBA32 {
			float fTexels[4];
		};
		

		etc_enc_settings eesSettings;
		::GetProfile_etc_slow( &eesSettings );

		CVectorPoD<LSI_RGBAF32, uint32_t> vExpanded;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
#ifdef LSI_ETC1_ROWS
			pui8Surf = LSI_CAST( _pui8Src );
#else
			sSurface.ptr = LSI_CAST( _pui8Src );
#endif	// ##ifdef LSI_ETC1_ROWS

			uint32_t ui32W = _ui32Width;
			uint32_t ui32H = _ui32Height;
			if ( (_ui32Width % 4) != 0 || (_ui32Height % 4) != 0 ) {
				if ( !ExpandImageRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vExpanded, ui32W, ui32H, false ) ) { return false; }
#ifdef LSI_ETC1_ROWS
				pui8Surf = LSI_CAST( &vExpanded[0] );
#else
				sSurface.ptr = LSI_CAST( &vExpanded[0] );
#endif	// ##ifdef LSI_ETC1_ROWS
			}

#ifdef LSI_ETC1_ROWS
			if ( !DivideSurface( vSurfaces, pui8Surf, _pui8Dst, ui32W, ui32H, LSI_ETC1_ROWS, LSI_PF_ETC1 ) ) { return false; }

			for ( uint32_t ui32Index = 0; ui32Index < vSurfaces.Length(); ++ui32Index ) {
				bool bFound = false;
				while ( !bFound ) {
					// Find a free thread.
					for ( uint32_t I = 0; I < LSE_ELEMENTS( tThreads ); ++I ) {
						if ( btdData[I].bDone ) {
							// Thread stopped or never started.
							CThreadLib::CloseThread( tThreads[I] );
						
							btdData[I].bDone = false;
							btdData[I].psSurface = &vSurfaces[ui32Index].sSurface;
							btdData[I].pui8Dst = vSurfaces[ui32Index].pui8Dst;
							btdData[I].peesSettings = &eesSettings;
							if ( !CThreadLib::CreateThread( Etc1Thread, &btdData[I], &tThreads[I] ) ) {
								for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
									CThreadLib::WaitForThread( tThreads[I], nullptr );
									CThreadLib::CloseThread( tThreads[I] );
								}
								return false;
							}
							bFound = true;
							break;
						}
						//CStd::Sleep( 10 );
					}
				}
			}
			for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
				CThreadLib::WaitForThread( tThreads[I], nullptr );
				CThreadLib::CloseThread( tThreads[I] );
			}
#else
			CompressBlocksETC1( &sSurface, _pui8Dst, &eesSettings );
#endif	// #ifdef LSI_ETC1_ROWS

			_pui8Src += ui32SliceSize;
			_pui8Dst += ui32DstSliceSize;
		}
#undef LSI_CAST
#endif	// #ifndef LSE_IPHONE
		return true;
	}

	/**
	 * Allocates an RGBA8 buffer such that the width and height have been rounded up by 4 texels and returns the rounded-up width and height.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreateExpandedRgba8( CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight ) {
		_ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		_ui32ExpandedHeight = LSE_ROUND_UP( _ui32Height, 4 );
		if ( !_pvDst.Resize( _ui32ExpandedWidth * _ui32ExpandedHeight ) ) { return false; }
		return true;
	}

	/**
	 * Allocates an RGB8 buffer such that the width and height have been rounded up by 4 texels and returns the rounded-up width and height.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedRgb8( CVectorPoD<LSI_RGB8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight ) {
		_ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		_ui32ExpandedHeight = LSE_ROUND_UP( _ui32Height, 4 );
		if ( !_pvDst.Resize( _ui32ExpandedWidth * _ui32ExpandedHeight ) ) { return false; }
		return true;
	}

	/**
	 * Allocates an A8 buffer such that the width and height have been rounded up by 4 texels and returns the rounded-up width and height.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedA8( CVectorPoD<uint8_t, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight ) {
		_ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		_ui32ExpandedHeight = LSE_ROUND_UP( _ui32Height, 4 );
		if ( !_pvDst.Resize( _ui32ExpandedWidth * _ui32ExpandedHeight ) ) { return false; }
		return true;
	}

	/**
	 * Allocates an A11 buffer such that the width and height have been rounded up by 4 texels and returns the rounded-up width and height.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedA11( CVectorPoD<uint16_t, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight ) {
		_ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		_ui32ExpandedHeight = LSE_ROUND_UP( _ui32Height, 4 );
		if ( !_pvDst.Resize( _ui32ExpandedWidth * _ui32ExpandedHeight ) ) { return false; }
		return true;
	}

	/**
	 * Converts RGBAF32 into RGBA8 while padding out the width and height to be multiples of 4.
	 *
	 * \param _prSrc The RGBAF32 source buffer.
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _bSrgb If true, a linear -> sRGB conversion is done.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedRgba8FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb ) {
		if ( !CreateExpandedRgba8( _pvDst, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight ) ) { return false; }
		for ( uint32_t Y = 0; Y < _ui32ExpandedHeight; ++Y ) {
			uint32_t ui32ClampY = CStd::Min<uint32_t>( Y, _ui32Height - 1 );
			for ( uint32_t X = 0; X < _ui32ExpandedWidth; ++X ) {
				uint32_t ui32ClampX = CStd::Min<uint32_t>( X, _ui32Width - 1 );
				LSI_RGBA8 & rDst = _pvDst[Y*_ui32ExpandedWidth+X];
				const LSI_RGBAF32 & rSrc = _prSrc[ui32ClampY*_ui32Width+ui32ClampX];
					
				rDst.ui8fRgba[0] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[0] ) * 255.0f ) : std::round( rSrc.fRgba[0] * 255.0f ));
				rDst.ui8fRgba[1] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[1] ) * 255.0f ) : std::round( rSrc.fRgba[1] * 255.0f ));
				rDst.ui8fRgba[2] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[2] ) * 255.0f ) : std::round( rSrc.fRgba[2] * 255.0f ));
				rDst.ui8fRgba[3] = static_cast<uint8_t>(std::round( rSrc.fRgba[3] * 255.0f ));
			}
		}
		return true;
	}

	/**
	 * Converts RGBAF32 into RGB8 and A8 while padding out the width and height to be multiples of 4.
	 *
	 * \param _prSrc The RGBAF32 source buffer.
	 * \param _pvDstRgb The destination for the RGB converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _pvDstA The destination for the A converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _bSrgb If true, a linear -> sRGB conversion is done.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedRgba8FromRgbaF32( const LSI_RGBAF32 * _prSrc,
		CVectorPoD<LSI_RGB8, uint32_t> &_pvDstRgb, CVectorPoD<uint8_t, uint32_t> &_pvDstA,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb ) {

		if ( !CreatedExtendedRgb8( _pvDstRgb, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight ) ) { return false; }
		if ( !CreatedExtendedA8( _pvDstA, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight ) ) { return false; }
		for ( uint32_t Y = 0; Y < _ui32ExpandedHeight; ++Y ) {
			uint32_t ui32ClampY = CStd::Min<uint32_t>( Y, _ui32Height - 1 );
			for ( uint32_t X = 0; X < _ui32ExpandedWidth; ++X ) {
				uint32_t ui32ClampX = CStd::Min<uint32_t>( X, _ui32Width - 1 );
				LSI_RGB8 & rDstRgb = _pvDstRgb[Y*_ui32ExpandedWidth+X];
				uint8_t & rDstA = _pvDstA[Y*_ui32ExpandedWidth+X];
				const LSI_RGBAF32 & rSrc = _prSrc[ui32ClampY*_ui32Width+ui32ClampX];
					
				rDstRgb.ui8fRgba[0] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[0] ) * 255.0f ) : std::round( rSrc.fRgba[0] * 255.0f ));
				rDstRgb.ui8fRgba[1] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[1] ) * 255.0f ) : std::round( rSrc.fRgba[1] * 255.0f ));
				rDstRgb.ui8fRgba[2] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[2] ) * 255.0f ) : std::round( rSrc.fRgba[2] * 255.0f ));

				rDstA = static_cast<uint8_t>(std::round( rSrc.fRgba[3] * 255.0f ));
			}
		}
		return true;
	}

	/**
	 * Converts an RGBA8 buffer into the destination RGBAF32 buffer, optionally applying an sRGB -> linear conversion as well.
	 *
	 * \param _pvSrc The source RGBA8 buffer.
	 * \param _prDst The destination RGBAF32 buffer.
	 * \param _ui32Width The width of the image data, in texels, to which _prDst points.
	 * \param _ui32Height The height of the image data, in texels, to which _prDst points.
	 * \param _bSrgb If true, an sRGB -> linear conversion is done.
	 * \param _bNoAlpha If true, alpha is set to 1.0f in the output image.
	 */
	void LSE_CALL CEtc::CopyRgba8ToRgbaF32( const CVectorPoD<LSI_RGBA8, uint32_t> &_pvSrc, LSI_RGBAF32 * _prDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb, bool _bNoAlpha ) {
		uint32_t ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		//uint32_t ui32ExpandedHeight = LSE_ROUND_UP( _ui32Height, 4 );
		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				const LSI_RGBA8 & rSrc = _pvSrc[Y*ui32ExpandedWidth+X];
				LSI_RGBAF32 & rDst = _prDst[Y*_ui32Width+X];
				rDst.fRgba[0] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[0] / 255.0f ) : rSrc.ui8fRgba[0] / 255.0f;
				rDst.fRgba[1] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[1] / 255.0f ) : rSrc.ui8fRgba[1] / 255.0f;
				rDst.fRgba[2] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[2] / 255.0f ) : rSrc.ui8fRgba[2] / 255.0f;
				rDst.fRgba[3] = _bNoAlpha ? 1.0f : rSrc.ui8fRgba[3] / 255.0f;
			}
		}
	}

	/**
	 * Converts RGBAF32 into RGB8 while padding out the width and height to be multiples of 4.
	 *
	 * \param _prSrc The RGBAF32 source buffer.
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _bSrgb If true, a linear -> sRGB conversion is done.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedRgb8FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGB8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb ) {
		if ( !CreatedExtendedRgb8( _pvDst, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight ) ) { return false; }
		for ( uint32_t Y = 0; Y < _ui32ExpandedHeight; ++Y ) {
			uint32_t ui32ClampY = CStd::Min<uint32_t>( Y, _ui32Height - 1 );
			for ( uint32_t X = 0; X < _ui32ExpandedWidth; ++X ) {
				uint32_t ui32ClampX = CStd::Min<uint32_t>( X, _ui32Width - 1 );
				LSI_RGB8 & rDst = _pvDst[Y*_ui32ExpandedWidth+X];
				const LSI_RGBAF32 & rSrc = _prSrc[ui32ClampY*_ui32Width+ui32ClampX];
				rDst.ui8fRgba[0] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[0] ) * 255.0f ) : std::round( rSrc.fRgba[0] * 255.0f ));
				rDst.ui8fRgba[1] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[1] ) * 255.0f ) : std::round( rSrc.fRgba[1] * 255.0f ));
				rDst.ui8fRgba[2] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[2] ) * 255.0f ) : std::round( rSrc.fRgba[2] * 255.0f ));
			}
		}
		return true;
	}

	/**
	 * Converts RGBAF32 into R11 while padding out the width and height to be multiples of 4.
	 *
	 * \param _prSrc The RGBAF32 source buffer.
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _bSrgb If true, a linear -> sRGB conversion is done.
	 * \param _bSigned Determines whether to convert to signed values or not.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CEtc::CreatedExtendedA11FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<uint16_t, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb, bool _bSigned ) {
		if ( !CreatedExtendedA11( _pvDst, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight ) ) { return false; }
		for ( uint32_t Y = 0; Y < _ui32ExpandedHeight; ++Y ) {
			uint32_t ui32ClampY = CStd::Min<uint32_t>( Y, _ui32Height - 1 );
			for ( uint32_t X = 0; X < _ui32ExpandedWidth; ++X ) {
				uint32_t ui32ClampX = CStd::Min<uint32_t>( X, _ui32Width - 1 );
				uint16_t & ui16Dst = _pvDst[Y*_ui32ExpandedWidth+X];
				const LSI_RGBAF32 & rSrc = _prSrc[ui32ClampY*_ui32Width+ui32ClampX];
				float fVal = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[0] ) : rSrc.fRgba[0];
				if ( _bSigned ) {
					const uint16_t ui16Max = 0xFFFF - 1;
					ui16Dst = static_cast<uint16_t>(std::round( ((CStd::Clamp( fVal, -1.0f, 1.0f ) * 0.5f) + 0.5f) * ui16Max )) + 1;
				}
				else {
					ui16Dst = static_cast<uint16_t>(std::round( CStd::Clamp( fVal, 0.0f, 1.0f ) * 0xFFFF ));
				}
				ui16Dst = CStd::ByteSwap16( ui16Dst );
			}
		}
		return true;
	}

	/**
	 * Converts an RGB8 buffer into the destination RGBAF32 buffer, optionally applying an sRGB -> linear conversion as well.
	 *
	 * \param _pvSrc The source RGB8 buffer.
	 * \param _prDst The destination RGBAF32 buffer.
	 * \param _ui32Width The width of the image data, in texels, to which _prDst points.
	 * \param _ui32Height The height of the image data, in texels, to which _prDst points.
	 * \param _bSrgb If true, an sRGB -> linear conversion is done.
	 */
	void LSE_CALL CEtc::CopyRgb8ToRgbaF32( const CVectorPoD<LSI_RGB8, uint32_t> &_pvSrc, LSI_RGBAF32 * _prDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		uint32_t ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				const LSI_RGB8 & rSrc = _pvSrc[Y*ui32ExpandedWidth+X];
				LSI_RGBAF32 & rDst = _prDst[Y*_ui32Width+X];
				rDst.fRgba[0] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[0] / 255.0f ) : rSrc.ui8fRgba[0] / 255.0f;
				rDst.fRgba[1] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[1] / 255.0f ) : rSrc.ui8fRgba[1] / 255.0f;
				rDst.fRgba[2] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[2] / 255.0f ) : rSrc.ui8fRgba[2] / 255.0f;
				rDst.fRgba[3] = 1.0f;
			}
		}
	}

	/**
	 * Converts an RGB8 buffer with alpha coming from an A8 buffer into the destination RGBAF32 buffer, optionally applying an sRGB -> linear conversion as well.
	 *
	 * \param _vSrcRgb The source RGB8 buffer.
	 * \param _pvSrcA The source A8 buffer.
	 * \param _prDst The destination RGBAF32 buffer.
	 * \param _ui32Width The width of the image data, in texels, to which _prDst points.
	 * \param _ui32Height The height of the image data, in texels, to which _prDst points.
	 * \param _bSrgb If true, an sRGB -> linear conversion is done.
	 */
	void LSE_CALL CEtc::CopyRgb8A8ToRgbaF32( const CVectorPoD<LSI_RGB8, uint32_t> &_vSrcRgb, const CVectorPoD<uint8_t, uint32_t> &_pvSrcA, LSI_RGBAF32 * _prDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) {
		uint32_t ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				const LSI_RGB8 & rSrc = _vSrcRgb[Y*ui32ExpandedWidth+X];
				LSI_RGBAF32 & rDst = _prDst[Y*_ui32Width+X];
				rDst.fRgba[0] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[0] / 255.0f ) : rSrc.ui8fRgba[0] / 255.0f;
				rDst.fRgba[1] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[1] / 255.0f ) : rSrc.ui8fRgba[1] / 255.0f;
				rDst.fRgba[2] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui8fRgba[2] / 255.0f ) : rSrc.ui8fRgba[2] / 255.0f;
				rDst.fRgba[3] = _pvSrcA[Y*ui32ExpandedWidth+X] / 255.0f;
			}
		}
	}

	/**
	 * Converts an R16 buffer with an optional G16 buffer into the destination RGBAF32 buffer, optionally applying an sRGB -> linear conversion as well.
	 *
	 * \param _vSrcR8 The source R8 buffer.
	 * \param _pvSrcG8 The optional source G8 buffer.
	 * \param _prDst The destination RGBAF32 buffer.
	 * \param _ui32Width The width of the image data, in texels, to which _prDst points.
	 * \param _ui32Height The height of the image data, in texels, to which _prDst points.
	 * \param _bSrgb If true, an sRGB -> linear conversion is done.
	 * \param _bSigned Indicates that the 16-bit values are signed.
	 */
	void LSE_CALL CEtc::CopyR16G16ToRgbaF32( const CVectorPoD<uint16_t, uint32_t> &_vSrcR8, const CVectorPoD<uint16_t, uint32_t> * _pvSrcG8, LSI_RGBAF32 * _prDst,
		uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb, bool _bSigned ) {
		uint32_t ui32ExpandedWidth = LSE_ROUND_UP( _ui32Width, 4 );
		for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
			for ( uint32_t X = 0; X < _ui32Width; ++X ) {
				uint16_t ui16Src = CStd::ByteSwap16( _vSrcR8[Y*ui32ExpandedWidth+X] );
				LSI_RGBAF32 & rDst = _prDst[Y*_ui32Width+X];
				float fTmp = _bSigned ? CImageLib::Uint16ToSNorm( ui16Src, _bSrgb ) : CImageLib::Uint16ToUNorm( ui16Src, _bSrgb );
				rDst.fRgba[0] = fTmp;
				if ( _pvSrcG8 ) {
					uint16_t ui16SrcG = CStd::ByteSwap16( (*_pvSrcG8)[Y*ui32ExpandedWidth+X] );
					fTmp = _bSigned ? CImageLib::Uint16ToSNorm( ui16SrcG, _bSrgb ) : CImageLib::Uint16ToUNorm( ui16SrcG, _bSrgb );
					rDst.fRgba[1] = fTmp;
				}
				else {
					rDst.fRgba[1] = 0.0f;
				}
				
				rDst.fRgba[2] = 0.0f;
				rDst.fRgba[3] = 1.0f;
			}
		}
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
	bool LSE_CALL CEtc::ExpandImageRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBAF32, uint32_t> &_vDst, uint32_t &_ui32Width, uint32_t &_ui32Height, bool _bSrgb ) {
		uint32_t ui32ExpWidth = LSE_ROUND_UP( _ui32Width, 4 );
		uint32_t ui32ExpHeight = LSE_ROUND_UP( _ui32Height, 4 );
		if ( !_vDst.Resize( ui32ExpWidth * ui32ExpHeight ) ) { return false; }
		for ( uint32_t Y = 0; Y < ui32ExpHeight; ++Y ) {
			for ( uint32_t X = 0; X < ui32ExpWidth; ++X ) {
				const LSI_RGBAF32 & prThis = _prSrc[CStd::Min(Y,_ui32Height-1)*_ui32Width+CStd::Min(X,_ui32Width-1)];
				LSI_RGBAF32 & rDst = _vDst[Y*ui32ExpWidth+X];
				if ( _bSrgb ) {
					rDst.fRgba[LSI_PC_R] = CImageLib::LinearToSRgb( rDst.fRgba[LSI_PC_R] );
					rDst.fRgba[LSI_PC_G] = CImageLib::LinearToSRgb( rDst.fRgba[LSI_PC_G] );
					rDst.fRgba[LSI_PC_B] = CImageLib::LinearToSRgb( rDst.fRgba[LSI_PC_B] );
					rDst.fRgba[LSI_PC_A] = rDst.fRgba[LSI_PC_A];
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
	void LSE_CALL CEtc::DivideSurface( LSI_SURFACE_AND_DST &_sadSurface, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t _ui32RowsPerSection, uint32_t _ui32ThisSectionIdx, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32SrcStride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width );
		uint32_t ui32DstStride = CEtc::EtcScanlineWidth( _ui32Width, _pfFormat );
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
	bool LSE_CALL CEtc::DivideSurface( CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> &_vSurfaces, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
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
	 * Thread function for converting a block of texels to the ETC1 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC1_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CEtc::Etc1Thread( void * _lpParameter ) {
		LSI_ETC1_THREAD_DATA * pbtdData = static_cast<LSI_ETC1_THREAD_DATA *>(_lpParameter);
		CompressBlocksETC1( pbtdData->psSurface, pbtdData->pui8Dst, pbtdData->peesSettings );
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Divides an image into rows of blocks to be handle by threads later.
	 *
	 * \param _vRows Vector to hold the rows.
	 * \param _ui32BlocksH Total rows of blocks in the image.
	 * \param _ui32MinRows Number of rows to pass to a thread.
	 * \return Returns false only if the vector could not be allocated.
	 */
	bool LSE_CALL CEtc::DivideByRows( CVectorPoD<LSI_THREAD_ROWS, uint32_t> &_vRows, uint32_t _ui32BlocksH, uint32_t _ui32MinRows ) {
		_vRows.ResetNoDealloc();
		uint32_t ui32Rows = 0;
		for ( uint32_t I = 0; I < _ui32BlocksH; I += ui32Rows ) {
			ui32Rows = CStd::Min( _ui32BlocksH - I, _ui32MinRows );
			LSI_THREAD_ROWS trThis;
			trThis.ui32Start = I;
			trThis.ui32Total = ui32Rows;
			if ( !_vRows.Push( trThis ) ) { return false; }
		}
		return true;
	}

	/**
	 * Thread function for converting a block of texels to the ETC2 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CEtc::Etc2Thread( void * _lpParameter ) {
		LSI_ETC2_THREAD_DATA * pbtdData = static_cast<LSI_ETC2_THREAD_DATA *>(_lpParameter);

		for ( uint32_t Y = 0; Y < pbtdData->ptrRows->ui32Total; ++Y ) {
			uint32_t ui32Y = Y + pbtdData->ptrRows->ui32Start;
			for ( uint32_t X = 0; X < pbtdData->ui32BlockW; ++X ) {
				uint32_t ui32Block1, ui32Block2;
				::compressBlockETC2ExhaustivePerceptual( pbtdData->pui8EncoderSrc, pbtdData->pui8ScratchDst,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, X << 2, ui32Y << 2, ui32Block1, ui32Block2 );
				/*::compressBlockETC2FastPerceptual( pui8EncodeSrc, pui8ScratchDst,
					ui32ExtW, ui32ExtH, X << 2, ui32Y << 2, ui32Block1, ui32Block2 );*/
				uint64_t & ui64Dst = reinterpret_cast<uint64_t *>(pbtdData->pui8Dst)[ui32Y*pbtdData->ui32BlockW+X];
				ui64Dst = CStd::ByteSwap32( ui32Block1 ) | (static_cast<uint64_t>(CStd::ByteSwap32( ui32Block2 )) << 32ULL);
			}
		}
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Thread function for converting a block of texels to the ETC2 EAC format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CEtc::Etc2EacThread( void * _lpParameter ) {
		LSI_ETC2_THREAD_DATA * pbtdData = static_cast<LSI_ETC2_THREAD_DATA *>(_lpParameter);
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
			uint64_t ui64Block2;
		};
		LSI_ETC_EAC * peeDst = reinterpret_cast<LSI_ETC_EAC *>(pbtdData->pui8Dst);
		for ( uint32_t Y = 0; Y < pbtdData->ptrRows->ui32Total; ++Y ) {
			uint32_t ui32Y = Y + pbtdData->ptrRows->ui32Start;
			for ( uint32_t X = 0; X < pbtdData->ui32BlockW; ++X ) {
				LSI_ETC_EAC & eeDst = peeDst[ui32Y*pbtdData->ui32BlockW+X];
				uint32_t ui32Block1, ui32Block2;
				::compressBlockETC2ExhaustivePerceptual( pbtdData->pui8EncoderSrc, pbtdData->pui8ScratchDst,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, X << 2, ui32Y << 2, ui32Block1, ui32Block2 );
				/*::compressBlockETC2Fast( pbtdData->pui8EncoderSrc, pbtdData->pui8EncoderSrcAlpha,
					pbtdData->pui8ScratchDst,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, X << 2, ui32Y << 2, ui32Block1, ui32Block2 );*/
				eeDst.ui64Block2 = CStd::ByteSwap32( ui32Block1 ) | (static_cast<uint64_t>(CStd::ByteSwap32( ui32Block2 )) << 32ULL);
				::compressBlockAlphaSlow( pbtdData->pui8EncoderSrcAlpha, X << 2, ui32Y << 2, pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, reinterpret_cast<uint8 *>(&eeDst.ui64Block1) );

			}
		}
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Thread function for converting a block of texels to the ETC2 A1 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CEtc::Etc2A1Thread( void * _lpParameter ) {
		LSI_ETC2_THREAD_DATA * pbtdData = static_cast<LSI_ETC2_THREAD_DATA *>(_lpParameter);
		uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(pbtdData->pui8Dst);
		for ( uint32_t Y = 0; Y < pbtdData->ptrRows->ui32Total; ++Y ) {
			uint32_t ui32Y = Y + pbtdData->ptrRows->ui32Start;
			for ( uint32_t X = 0; X < pbtdData->ui32BlockW; ++X ) {
				uint32_t ui32Block1, ui32Block2;
				::compressBlockETC2Fast( pbtdData->pui8EncoderSrc, pbtdData->pui8EncoderSrcAlpha,
					pbtdData->pui8ScratchDst,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, X << 2, ui32Y << 2, ui32Block1, ui32Block2 );
				uint64_t & ui64Dst = pui64Dst[ui32Y*pbtdData->ui32BlockW+X];
				ui64Dst = CStd::ByteSwap32( ui32Block1 ) | (static_cast<uint64_t>(CStd::ByteSwap32( ui32Block2 )) << 32ULL);
			}
		}
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Thread function for converting a block of texels to the ETC2 R11 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CEtc::Etc2R11Thread( void * _lpParameter ) {
		LSI_ETC2_THREAD_DATA * pbtdData = static_cast<LSI_ETC2_THREAD_DATA *>(_lpParameter);
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
		};
		LSI_ETC_EAC * peeDst = reinterpret_cast<LSI_ETC_EAC *>(pbtdData->pui8Dst);
		for ( uint32_t Y = 0; Y < pbtdData->ptrRows->ui32Total; ++Y ) {
			uint32_t ui32Y = Y + pbtdData->ptrRows->ui32Start;
			for ( uint32_t X = 0; X < pbtdData->ui32BlockW; ++X ) {
				LSI_ETC_EAC & eeDst = peeDst[ui32Y*pbtdData->ui32BlockW+X];
				::compressBlockAlpha16( pbtdData->pui8EncoderSrcAlpha,
					X << 2, ui32Y << 2,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, reinterpret_cast<uint8 *>(&eeDst.ui64Block1) );
			}
		}
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Thread function for converting a block of texels to the ETC2 RG11 format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CEtc::Etc2RG11Thread( void * _lpParameter ) {
		LSI_ETC2_THREAD_DATA * pbtdData = static_cast<LSI_ETC2_THREAD_DATA *>(_lpParameter);
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
			uint64_t ui64Block2;
		};
		LSI_ETC_EAC * peeDst = reinterpret_cast<LSI_ETC_EAC *>(pbtdData->pui8Dst);
		for ( uint32_t Y = 0; Y < pbtdData->ptrRows->ui32Total; ++Y ) {
			uint32_t ui32Y = Y + pbtdData->ptrRows->ui32Start;
			for ( uint32_t X = 0; X < pbtdData->ui32BlockW; ++X ) {
				LSI_ETC_EAC & eeDst = peeDst[ui32Y*pbtdData->ui32BlockW+X];
				::compressBlockAlpha16( pbtdData->pui8EncoderSrc,
					X << 2, ui32Y << 2,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, reinterpret_cast<uint8 *>(&eeDst.ui64Block1) );
				::compressBlockAlpha16( pbtdData->pui8EncoderSrcAlpha,
					X << 2, ui32Y << 2,
					pbtdData->ui32ExtWidth, pbtdData->ui32ExtHeight, reinterpret_cast<uint8 *>(&eeDst.ui64Block2) );
			}
		}
		pbtdData->bDone = true;
		return 0;
	}

}	// namespace lsi
