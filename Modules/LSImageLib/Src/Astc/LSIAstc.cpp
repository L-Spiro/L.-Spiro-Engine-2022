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
 * Description: Handles conversion of ASTC formats.
 */

#include "LSIAstc.h"


namespace lsi {

	// == Functions.
	/**
	 * Gets the size of blocks given an ASTC pixel format.
	 *
	 * \param _pfFormat The format for which to retrieve the block dimensions.
	 * \param _ui32X Holds the X block dimensions.
	 * \param _ui32Y Holds the Y block dimensions.
	 */
	void LSE_CALL CAstc::GetBlockSizes( LSI_PIXEL_FORMAT _pfFormat, uint32_t &_ui32X, uint32_t &_ui32Y ) {
		switch ( _pfFormat ) {
			case LSI_PF_ASTC_4_4 : { _ui32X = 4; _ui32Y = 4; return; }
			case LSI_PF_ASTC_5_4 : { _ui32X = 5; _ui32Y = 4; return; }
			case LSI_PF_ASTC_5_5 : { _ui32X = 5; _ui32Y = 5; return; }
			case LSI_PF_ASTC_6_5 : { _ui32X = 6; _ui32Y = 5; return; }
			case LSI_PF_ASTC_6_6 : { _ui32X = 6; _ui32Y = 6; return; }
			case LSI_PF_ASTC_8_5 : { _ui32X = 8; _ui32Y = 5; return; }
			case LSI_PF_ASTC_8_6 : { _ui32X = 8; _ui32Y = 6; return; }
			case LSI_PF_ASTC_8_8 : { _ui32X = 8; _ui32Y = 8; return; }
			case LSI_PF_ASTC_10_5 : { _ui32X = 10; _ui32Y = 5; return; }
			case LSI_PF_ASTC_10_6 : { _ui32X = 10; _ui32Y = 6; return; }
			case LSI_PF_ASTC_10_8 : { _ui32X = 10; _ui32Y = 8; return; }
			case LSI_PF_ASTC_10_10 : { _ui32X = 10; _ui32Y = 10; return; }
			case LSI_PF_ASTC_12_10 : { _ui32X = 12; _ui32Y = 10; return; }
			case LSI_PF_ASTC_12_12 : { _ui32X = 12; _ui32Y = 12; return; }
			default : { _ui32X = 0; _ui32Y = 0; }
		}
	}

	/**
	 * Gets the total number of scanlines in a given ASTC format.
	 *
	 * \param _ui32Height Height of the image.
	 * \return Returns the number of scanlines in the given ASTC format.
	 */
	uint32_t LSE_CALL CAstc::AstcTotalScanlines( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Height ) {
		uint32_t ui32X, ui32Y;
		GetBlockSizes( _pfFormat, ui32X, ui32Y );
		return ((_ui32Height + (ui32Y - 1)) / ui32Y);
	}

	/**
	 * Returns the total size of a compressed image given a factor and its width and height.
	 *
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _pfFormat The format.
	 * \return Returns the size of the compressed data.
	 */
	uint32_t LSE_CALL CAstc::GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32X, ui32Y;
		GetBlockSizes( _pfFormat, ui32X, ui32Y );
		return ((_ui32Width + (ui32X - 1)) / ui32X) * ((_ui32Height + (ui32Y - 1)) / ui32Y) * AstcBlockSize( _pfFormat );
	}

	/**
	 * Returns the total size of a compressed image given its block sizes and its width and height.
	 *
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _ui32BlockW Width of blocks.
	 * \param _ui32BlockH Height of blocks.
	 * \return Returns the size of the compressed data.
	 */
	uint32_t LSE_CALL CAstc::GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32BlockW, uint32_t _ui32BlockH ) {
		return ((_ui32Width + (_ui32BlockW - 1)) / _ui32BlockW) * ((_ui32Height + (_ui32BlockH - 1)) / _ui32BlockH) * AstcBlockSize();
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
	void LSE_CALL CAstc::DivideSurface( LSI_SURFACE_AND_DST &_sadSurface, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t _ui32RowsPerSection, uint32_t _ui32ThisSectionIdx, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32SrcStride = CImageLib::GetRowSize( LSI_PF_R8G8B8A8, _ui32Width );
		uint32_t ui32DstStride = CAstc::AstcScanlineWidth( _ui32Width, _pfFormat );
		uint32_t ui32TotalRowsAvailable = LSE_ROUND_UP_ANY( _ui32Height, _ui32RowsPerSection ) / _ui32RowsPerSection;
		uint32_t ui32X, ui32Y;
		GetBlockSizes( _pfFormat, ui32X, ui32Y );
		if ( _ui32ThisSectionIdx < ui32TotalRowsAvailable ) {
			uint32_t ui32RowStart = _ui32ThisSectionIdx * _ui32RowsPerSection;
			uint32_t ui32Rows = CStd::Min( _ui32Height - ui32RowStart, _ui32RowsPerSection );

			_sadSurface.sSurface.width = _ui32Width;
			_sadSurface.sSurface.height = ui32Rows;
			_sadSurface.sSurface.stride = ui32SrcStride;
			_sadSurface.sSurface.ptr = _pui8Src + (ui32SrcStride * ui32RowStart);
			_sadSurface.pui8Dst = _pui8Dst + (ui32DstStride * ui32RowStart / ui32Y);
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
	bool LSE_CALL CAstc::DivideSurface( CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> &_vSurfaces, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
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
	bool LSE_CALL CAstc::ExpandImageRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBAF32, uint32_t> &_vDst, uint32_t &_ui32Width, uint32_t &_ui32Height, bool _bSrgb ) {
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
	 * Thread function for converting a block of texels to the ASTC format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_ASTC_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CAstc::AstcThread( void * _lpParameter ) {
		LSI_ASTC_THREAD_DATA * pbtdData = static_cast<LSI_ASTC_THREAD_DATA *>(_lpParameter);
		CompressBlocksASTC( pbtdData->psSurface, pbtdData->pui8Dst, pbtdData->paesSettings );
		pbtdData->bDone = true;
		return 0;
	}

	/**
	 * Allocates an RGBA8 buffer to act as an intermediate storage for decoding ASTC texture before converting to RGBAF32.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated.
	 * \param _ui32Width The width of the image data, in texels.
	 * \param _ui32Height The height of the image data, in texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CAstc::CreateExpandedRgba8( CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height ) {
		if ( !_pvDst.Resize( _ui32Width * _ui32Height ) ) { return false; }
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
	bool LSE_CALL CAstc::CreateExpandedRgba8( CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32X, ui32Y;
		GetBlockSizes( _pfFormat, ui32X, ui32Y );
		_ui32ExpandedWidth = LSE_ROUND_UP_ANY( _ui32Width, ui32X );
		_ui32ExpandedHeight = LSE_ROUND_UP_ANY( _ui32Height, ui32Y );
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
	bool LSE_CALL CAstc::CreateExpandedRgba8FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb, LSI_PIXEL_FORMAT _pfFormat ) {
		if ( !CreateExpandedRgba8( _pvDst, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight, _pfFormat ) ) { return false; }
		for ( uint32_t Y = 0; Y < _ui32ExpandedHeight; ++Y ) {
			uint32_t ui32ClampY = CStd::Min<uint32_t>( Y, _ui32Height - 1 );
			for ( uint32_t X = 0; X < _ui32ExpandedWidth; ++X ) {
				uint32_t ui32ClampX = CStd::Min<uint32_t>( X, _ui32Width - 1 );
				LSI_RGBA8 & rDst = _pvDst[Y*_ui32ExpandedWidth+X];
				const LSI_RGBAF32 & rSrc = _prSrc[ui32ClampY*_ui32Width+ui32ClampX];
				rDst.ui8Rgba[0] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[0] ) * 255.0f ) : std::round( rSrc.fRgba[0] * 255.0f ));
				rDst.ui8Rgba[1] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[1] ) * 255.0f ) : std::round( rSrc.fRgba[1] * 255.0f ));
				rDst.ui8Rgba[2] = static_cast<uint8_t>(_bSrgb ? std::round( CImageLib::LinearToSRgb( rSrc.fRgba[2] ) * 255.0f ) : std::round( rSrc.fRgba[2] * 255.0f ));
				rDst.ui8Rgba[3] = static_cast<uint8_t>(std::round( rSrc.fRgba[3] * 255.0f ));
			}
		}
		return true;
	}

	/**
	 * Converts from a float16 (the format returned by ASTC-Encoder) to a float32.
	 *
	 * \param _pppui16Src The source buffer.
	 * \param _prDst The destination buffer.
	 * \param _ui32W The width of the image to copy.
	 * \param _ui32H The height of the image to copy.
	 * \param _ui32D The depth of the image to copy.
	 * \param _bSrgb If true, sRGB -> linear is applied.
	 */
	void LSE_CALL CAstc::CopyFloat16ToFloat32( uint16_t *** _pppui16Src, LSI_RGBAF32 * _prDst, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, bool _bSrgb ) {
		struct LSI_RGBAF16 {
			CFloat16	ui16Val[4];
		};
		uint32_t ui32SliceDst = _ui32W * _ui32H;
		for ( uint32_t Z = 0; Z < _ui32D; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32H; ++Y ) {
				for ( uint32_t X = 0; X < _ui32W; ++X ) {
					const LSI_RGBAF16 & rSrc = (*reinterpret_cast<const LSI_RGBAF16 *>(&_pppui16Src[Z][Y][X*4]));
					LSI_RGBAF32 & rDst = _prDst[Z*ui32SliceDst+Y*_ui32W+X];
					rDst.fRgba[LSI_PC_R] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui16Val[LSI_PC_R] ) : rSrc.ui16Val[LSI_PC_R];
					rDst.fRgba[LSI_PC_G] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui16Val[LSI_PC_G] ) : rSrc.ui16Val[LSI_PC_G];
					rDst.fRgba[LSI_PC_B] = _bSrgb ? CImageLib::SRgbToLinear( rSrc.ui16Val[LSI_PC_B] ) : rSrc.ui16Val[LSI_PC_B];
					rDst.fRgba[LSI_PC_A] = rSrc.ui16Val[LSI_PC_A];
				}
			}
		}
	}

	/**
	 * Converts from a float32 to a float16 (the format returned by ASTC-Encoder).
	 *
	 * \param _pppui16Dst The source buffer.
	 * \param _prSrc The destination buffer.
	 * \param _ui32W The width of the image to copy.
	 * \param _ui32H The height of the image to copy.
	 * \param _ui32D The depth of the image to copy.
	 * \param _bSrgb If true, linear -> sRGB is applied.
	 */
	void LSE_CALL CAstc::CopyFloat32ToFloat16( uint16_t *** _pppui16Dst, const LSI_RGBAF32 * _prSrc, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, bool _bSrgb ) {
		struct LSI_RGBAF16 {
			CFloat16	ui16Val[4];
		};
		uint32_t ui32SliceDst = _ui32W * _ui32H;
		for ( uint32_t Z = 0; Z < _ui32D; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32H; ++Y ) {
				for ( uint32_t X = 0; X < _ui32W; ++X ) {
					LSI_RGBAF16 & rDst = (*reinterpret_cast<LSI_RGBAF16 *>(&_pppui16Dst[Z][Y][X*4]));
					const LSI_RGBAF32 & rSrc = _prSrc[Z*ui32SliceDst+Y*_ui32W+X];
					rDst.ui16Val[LSI_PC_R] = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[LSI_PC_R] ) : rSrc.fRgba[LSI_PC_R];
					rDst.ui16Val[LSI_PC_G] = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[LSI_PC_G] ) : rSrc.fRgba[LSI_PC_G];
					rDst.ui16Val[LSI_PC_B] = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[LSI_PC_B] ) : rSrc.fRgba[LSI_PC_B];
					rDst.ui16Val[LSI_PC_A] = rSrc.fRgba[LSI_PC_A];
				}
			}
		}
	}

}	// namespace lsi
