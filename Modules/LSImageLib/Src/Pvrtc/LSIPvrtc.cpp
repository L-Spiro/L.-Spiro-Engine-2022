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
 * Description: Handles conversion of PVRTC formats.
 */

#include "LSIPvrtc.h"


namespace lsi {

	// == Functions.
	/**
	 * Converts from the image library's format to a PowerVR library format.
	 *
	 * \param _pfFormat The image library's format.
	 * \return Returns the PVRTexLib format.
	 */
	PVRTexLibPixelFormat LSE_CALL CPvrtc::FormatToPvrFormat( LSI_PIXEL_FORMAT _pfFormat ) {
		switch ( _pfFormat ) {
			case LSI_PF_PCRTC_2_RGB_V1 : { return PVRTLPF_PVRTCI_2bpp_RGB; }
			case LSI_PF_PCRTC_2_RGBA_V1 : { return PVRTLPF_PVRTCI_2bpp_RGBA; }
			case LSI_PF_PCRTC_4_RGB_V1 : { return PVRTLPF_PVRTCI_4bpp_RGB; }
			case LSI_PF_PCRTC_4_RGBA_V1 : { return PVRTLPF_PVRTCI_4bpp_RGBA; }
			case LSI_PF_PCRTC_2_RGBA_V2 : { return PVRTLPF_PVRTCII_2bpp; }
			case LSI_PF_PCRTC_4_RGBA_V2 : { return PVRTLPF_PVRTCII_4bpp; }
			case LSI_PF_PCRTC_6_HDR_V1 : { return PVRTLPF_PVRTCI_HDR_6bpp; }
			case LSI_PF_PCRTC_8_HDR_V1 : { return PVRTLPF_PVRTCI_HDR_8bpp; }
			case LSI_PF_PCRTC_6_HDR_V2 : { return PVRTLPF_PVRTCII_HDR_6bpp; }
			case LSI_PF_PCRTC_8_HDR_V2 : { return PVRTLPF_PVRTCII_HDR_8bpp; }
			default : { return PVRTLPF_NumCompressedPFs; }
		}
	}

	/**
	 * Gets the size of blocks given an ASTC pixel format.
	 *
	 * \param _pfFormat The format for which to retrieve the block dimensions.
	 * \param _ui32X Holds the X block dimensions.
	 * \param _ui32Y Holds the Y block dimensions.
	 */
	void LSE_CALL CPvrtc::GetBlockSizes( LSI_PIXEL_FORMAT _pfFormat, uint32_t &_ui32X, uint32_t &_ui32Y ) {
		switch ( _pfFormat ) {
			case LSI_PF_PCRTC_2_RGB_V1 : {}
			case LSI_PF_PCRTC_2_RGBA_V1 : {}
			case LSI_PF_PCRTC_2_RGBA_V2 : { _ui32X = 8; _ui32Y = 4; return; }
			case LSI_PF_PCRTC_4_RGB_V1 : {}
			case LSI_PF_PCRTC_4_RGBA_V1 : {}
			case LSI_PF_PCRTC_4_RGBA_V2 : { _ui32X = 4; _ui32Y = 4; return; }
			case LSI_PF_PCRTC_6_HDR_V1 : {}
			case LSI_PF_PCRTC_6_HDR_V2 : { _ui32X = 8; _ui32Y = 4; return; }
			case LSI_PF_PCRTC_8_HDR_V1 : {}
			case LSI_PF_PCRTC_8_HDR_V2 : { _ui32X = 4; _ui32Y = 4; return; }
		}
	}

	/**
	 * Allocates an RGBA8 buffer to act as an intermediate storage for decoding ASTC texture before converting to RGBAF32.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated.
	 * \param _ui32Width The width of the image data, in texels.
	 * \param _ui32Height The height of the image data, in texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	bool LSE_CALL CPvrtc::CreateExpandedRgba8( CVectorPoD<LSI_RGBAF32, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height ) {
		if ( !_pvDst.Resize( _ui32Width * _ui32Height ) ) { return false; }
		return true;
	}

	/**
	 * Converts RGBAF32 into RGBAF32 while padding out the width and height to be multiples of 4 or 8.
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
	bool LSE_CALL CPvrtc::CreateExpandedRgbaF32FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBAF32, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb, LSI_PIXEL_FORMAT _pfFormat ) {
		if ( !CreateExpandedRgba8( _pvDst, _ui32Width, _ui32Height,
			_ui32ExpandedWidth, _ui32ExpandedHeight, _pfFormat ) ) { return false; }
		for ( uint32_t Y = 0; Y < _ui32ExpandedHeight; ++Y ) {
			uint32_t ui32ClampY = CStd::Min<uint32_t>( Y, _ui32Height - 1 );
			for ( uint32_t X = 0; X < _ui32ExpandedWidth; ++X ) {
				uint32_t ui32ClampX = CStd::Min<uint32_t>( X, _ui32Width - 1 );
				LSI_RGBAF32 & rDst = _pvDst[Y*_ui32ExpandedWidth+X];
				const LSI_RGBAF32 & rSrc = _prSrc[ui32ClampY*_ui32Width+ui32ClampX];
				rDst.fRgba[0] = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[0] ) : rSrc.fRgba[0];
				rDst.fRgba[1] = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[1] ) : rSrc.fRgba[1];
				rDst.fRgba[2] = _bSrgb ? CImageLib::LinearToSRgb( rSrc.fRgba[2] ) : rSrc.fRgba[2];
				rDst.fRgba[3] = rSrc.fRgba[3];
			}
		}
		return true;
	}

}	// namespace lsi
