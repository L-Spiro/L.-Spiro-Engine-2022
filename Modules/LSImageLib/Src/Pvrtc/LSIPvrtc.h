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


#ifndef __LSI_PVRTC_H__
#define __LSI_PVRTC_H__

#include "../LSIImageLib.h"
#include "../PVRTexTool/PVRTexLib.hpp"


namespace lsi {

	/**
	 * Class CPvrtc
	 * \brief Handles PVRTC formats.
	 *
	 * Description: Handles PVRTC formats.
	 */
	class CPvrtc {
	public :
		// == Types.
		/** An RGBAF32 color component. */
		typedef struct LSI_RGBAF32 {
			union {
				float						fRgba[4];
				uint32_t					ui32Rgba[4];
			};
		} * LPLSI_RGBAF32, * const LPCLSI_RGBAF32;
		

		// == Functions.
		/**
		 * Converts from the image library's format to a PowerVR library format.
		 *
		 * \param _pfFormat The image library's format.
		 * \return Returns the PVRTexLib format.
		 */
		static PVRTexLibPixelFormat LSE_CALL
											FormatToPvrFormat( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Gets the size of blocks given an ASTC pixel format.
		 *
		 * \param _pfFormat The format for which to retrieve the block dimensions.
		 * \param _ui32X Holds the X block dimensions.
		 * \param _ui32Y Holds the Y block dimensions.
		 */
		static void LSE_CALL				GetBlockSizes( LSI_PIXEL_FORMAT _pfFormat, uint32_t &_ui32X, uint32_t &_ui32Y );

		/**
		 * PVRTC -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _pfFormat, unsigned _bSrgb>
		static bool LSE_CALL				PvrtcToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> PVRTC conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _pfFormat, unsigned _bSrgb>
		static bool LSE_CALL				PvrtcFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );


	protected :
		// == Functions.
		/**
		 * Allocates an RGBA8 buffer to act as an intermediate storage for decoding ASTC texture before converting to RGBAF32.
		 *
		 * \param _pvDst The destination for the converted texels, which will be allocated.
		 * \param _ui32Width The width of the image data, in texels.
		 * \param _ui32Height The height of the image data, in texels.
		 * \return Returns true if there is no issue allocating the new image memory.
		 */
		static bool LSE_CALL				CreateExpandedRgba8( CVectorPoD<LSI_RGBAF32, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Allocates an RGBA8 buffer such that the width and height have been rounded up by 4 or 8 texels and returns the rounded-up width and height.
		 *
		 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
		 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
		 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
		 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
		 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
		 * \return Returns true if there is no issue allocating the new image memory.
		 */
		static LSE_INLINE bool LSE_CALL		CreateExpandedRgba8( CVectorPoD<LSI_RGBAF32, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, LSI_PIXEL_FORMAT _pfFormat );

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
		static bool LSE_CALL				CreateExpandedRgbaF32FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBAF32, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb, LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Returns the total size of a PVRTC image given a factor and its width and height.
		 *
		 * \param _pfFormat Format.
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \return Returns the size of the compressed data.
		 */
		static LSE_INLINE uint32_t LSE_CALL	GetCompressedSizePvrtc( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * PVRTC -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _pfFormat, unsigned _bSrgb>
	bool LSE_CALL CPvrtc::PvrtcToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		uint32_t ui32DstRow = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width );
		uint32_t ui32DstSliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;

		pvrtexlib::PVRTextureHeader thTexHeader( FormatToPvrFormat( static_cast<LSI_PIXEL_FORMAT>(_pfFormat) ),
			_ui32Width, _ui32Height, _ui32Depth,
			1U, 1U, 1U,
			_bSrgb ? PVRTLCS_sRGB : PVRTLCS_Linear,
			PVRTLVT_UnsignedByteNorm );
		pvrtexlib::PVRTexture tTex( thTexHeader, _pui8Src );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			if ( tTex.Transcode( PVRTGENPIXELID4( 'r', 'g', 'b', 'a', 32, 32, 32, 32 ),
				PVRTLVT_SignedFloat,
				PVRTLCS_Linear,
				PVRTLCQ_PVRTCBest ) ) {
				uint8_t * pui8Data = static_cast<uint8_t *>(tTex.GetTextureDataPointer( 0U, 0U, 0U, Z ));
				uint8_t * pui8Dst = _pui8Dst;
				if ( pui8Data ) {
					for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
						CStd::MemCpy( pui8Dst, pui8Data, ui32DstRow );

						pui8Dst += ui32DstRow;
						pui8Data += ui32DstRow;
					}
				}
				else { return false; }
			}
			_pui8Dst += ui32DstSliceSize;
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> PVRTC conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _pfFormat, unsigned _bSrgb>
	bool LSE_CALL CPvrtc::PvrtcFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		uint32_t ui32SliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;
		uint32_t ui32DstSliceSize = GetCompressedSizePvrtc( static_cast<LSI_PIXEL_FORMAT>(_pfFormat), _ui32Width, _ui32Height );
		CVectorPoD<LSI_RGBAF32, uint32_t> vExpanded;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint32_t ui32W = _ui32Width;
			uint32_t ui32H = _ui32Height;
			if ( !CreateExpandedRgbaF32FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vExpanded, _ui32Width, _ui32Height, ui32W, ui32H,
				false, static_cast<LSI_PIXEL_FORMAT>(_pfFormat) ) ) { return false; }

			pvrtexlib::PVRTextureHeader thTexHeader( PVRTGENPIXELID4( 'r', 'g', 'b', 'a', 32, 32, 32, 32 ),
				ui32W, ui32H, 1,
				1U, 1U, 1U, PVRTLCS_Linear, PVRTLVT_SignedFloat );
			pvrtexlib::PVRTexture tTex( thTexHeader, &vExpanded[0] );
			if ( tTex.Transcode( FormatToPvrFormat( static_cast<LSI_PIXEL_FORMAT>(_pfFormat) ),
				PVRTLVT_UnsignedByteNorm,
				_bSrgb ? PVRTLCS_sRGB : PVRTLCS_Linear,
				PVRTLCQ_PVRTCBest ) ) {
				void * pvData = tTex.GetTextureDataPointer( 0U, 0U, 0U, Z );
				if ( pvData ) {
					CStd::MemCpy( _pui8Dst, pvData, ui32DstSliceSize );
				}
			}
			

			_pui8Src += ui32SliceSize;
			_pui8Dst += ui32DstSliceSize;
		}
		return true;
	}

	/**
	 * Allocates an RGBA8 buffer such that the width and height have been rounded up by 4 or 8 texels and returns the rounded-up width and height.
	 *
	 * \param _pvDst The destination for the converted texels, which will be allocated and filled, and row-padded to 4 texels.
	 * \param _ui32Width The width of the image data, in texels, to which _prSrc points.
	 * \param _ui32Height The height of the image data, in texels, to which _prSrc points.
	 * \param _ui32ExpandedWidth Upon return, this holds the width of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \param _ui32ExpandedHeight Upon return, this holds the height of the image data in _pvDst, in texels, which will be padded up to 4 texels.
	 * \return Returns true if there is no issue allocating the new image memory.
	 */
	LSE_INLINE bool LSE_CALL CPvrtc::CreateExpandedRgba8( CVectorPoD<LSI_RGBAF32, uint32_t> &_pvDst,
		uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, LSI_PIXEL_FORMAT _pfFormat ) {
		uint32_t ui32X, ui32Y;
		GetBlockSizes( _pfFormat, ui32X, ui32Y );
		_ui32ExpandedWidth = _ui32Width;//LSE_ROUND_UP_ANY( _ui32Width, ui32X );
		_ui32ExpandedHeight = _ui32Height;//LSE_ROUND_UP_ANY( _ui32Height, ui32Y );
		if ( !_pvDst.Resize( _ui32ExpandedWidth * _ui32ExpandedHeight ) ) { return false; }
		return true;
	}

	/**
	 * Returns the total size of a PVRTC image given a factor and its width and height.
	 *
	 * \param _pfFormat Format.
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \return Returns the size of the compressed data.
	 */
	LSE_INLINE uint32_t LSE_CALL CPvrtc::GetCompressedSizePvrtc( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height ) {
		uint32_t ui32W, ui32H;
		GetBlockSizes( _pfFormat, ui32W, ui32H );
		return ((_ui32Width + (ui32W - 1)) / ui32W) * ((_ui32Height + (ui32H - 1)) / ui32H) * 8;
	}

}	// namespace lsi

#endif	// __LSI_PVRTC_H__
