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


#ifndef __LSI_DDS_H__
#define __LSI_DDS_H__

#include "../LSIImageLib.h"
#include "../detex/detex.h"
#ifndef LSE_IPHONE
#include "../Ispc/ispc_texcomp.h"
#endif	// #ifndef LSE_IPHONE
#include "../Squish/squish.h"
#include "Vector/LSTLVector.h"

/** How many rows to do per-thread during a DXT conversion. */
#define LSI_DXT_ROWS					12

/** How many rows to do per-thread for BC6H and BC7 conversions. */
#define LSI_BC_ROWS						24

/** Maximum number of threads to run concurrently for BC conversions. */
#define LSI_BC_MAX_THREADS				16

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lsi {

	// == Enumerations.
	/**
	 * Pixel format flags.
	 */
	enum LSI_DDS_PIXEL_FORMAT_FLAGS {
		LSI_DPFF_ALPHAPIXELS			= 0x1,						/**< Texture contains alpha data; ui32RGBAlphaBitMask contains valid data. */
		LSI_DPFF_ALPHA					= 0x2,						/**< Used in some older DDS files for alpha channel only uncompressed data (ui32RGBBitCount contains the alpha channel bitcount; ui32ABitMask contains valid data). */
		LSI_DPFF_FOURCC					= 0x4,						/**< Texture contains compressed RGB data; ui32FourCC contains valid data. */
		LSI_DPFF_RGB					= 0x40,						/**< Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (ui32RBitMask, ui32RBitMask, ui32RBitMask) contain valid data. */
		LSI_DPFF_RGBA					= LSI_DPFF_RGB |
											LSI_DPFF_ALPHAPIXELS,	/**< Texture contains uncompressed RGB datawith alpha. */
		LSI_DPFF_YUV					= 0x200,					/**< Used in some older DDS files for YUV uncompressed data (ui32RGBBitCount contains the YUV bit count; ui32RBitMask contains the Y mask, ui32GBitMask contains the U mask, ui32BBitMask contains the V mask). */
		LSI_DPFF_LUMINANCE				= 0x20000,					/**< Used in some older DDS files for single channel color uncompressed data (ui32RGBBitCount contains the luminance channel bit count; ui32RBitMask contains the channel mask). Can be combined with LSI_DPFF_ALPHAPIXELS for a two-channel DDS file. */
	};

	/**
	 * DDS flags.
	 */
	enum LSI_DDS_FLAGS {
		LSI_DF_CAPS						= 0x1,						/**< Required in every .dds file. */
		LSI_DF_HEIGHT					= 0x2,						/**< Required in every .dds file. */
		LSI_DF_WIDTH					= 0x4,						/**< Required in every .dds file. */
		LSI_DF_PITCH					= 0x8,						/**< Required when pitch is provided for an uncompressed texture. */
		LSI_DF_PIXELFORMAT				= 0x1000,					/**< Required in every .dds file. */
		LSI_DF_MIPMAPCOUNT				= 0x20000,					/**< Required in a mipmapped texture. */
		LSI_DF_LINEARSIZE				= 0x80000,					/**< Required when pitch is provided for a compressed texture. */
		LSI_DF_DEPTH					= 0x800000,					/**< Required in a depth texture. */
	};

	/**
	 * Class CDds
	 * \brief Handles loading of .DDS files.
	 *
	 * Description: Handles loading of .DDS files.
	 */
	class CDds {
	public :
		// == Types.
		/** The DDS header pixel format structure. */
#pragma pack( push, 1 )
		typedef struct LSI_DDS_PIXELFORMAT {
			uint32_t					ui32Size;
			uint32_t					ui32Flags;
			uint32_t					ui32FourCC;
			uint32_t					ui32RGBBitCount;
			uint32_t					ui32RBitMask;
			uint32_t					ui32GBitMask;
			uint32_t					ui32BBitMask;
			uint32_t					ui32ABitMask;
		} * LPLSI_DDS_PIXELFORMAT, * const LPCLSI_DDS_PIXELFORMAT;
#pragma pack( pop )

		/** The DDS header. */
#pragma pack( push, 1 )
		typedef struct LSI_DDS_HEADER {
			uint32_t					ui32Size;
			uint32_t					ui32Flags;
			uint32_t					ui32Height;
			uint32_t					ui32Width;
			uint32_t					ui32PitchOrLinearSize;
			uint32_t					ui32Depth;
			uint32_t					ui32MipMapCount;
			uint32_t					ui32Reserved1[11];
			LSI_DDS_PIXELFORMAT			dpPixelFormat;
			uint32_t					ui32Caps;
			uint32_t					ui32Caps2;
			uint32_t					ui32Caps3;
			uint32_t					ui32Caps4;
			uint32_t					ui32Reserved2;
		} * LPLSI_DDS_HEADER, * const LPCLSI_DDS_HEADER;

		/** Extended header. */
		typedef struct LSI_DDS_HEADER_DXT10 {
			uint32_t					ui32DxgiFormat;
			uint32_t					ui32ResourceDimension;
			uint32_t					ui32MiscFlag;
			uint32_t					ui32ArraySize;
			uint32_t					ui32MiscFlags2;
		} * LPLSI_DDS_HEADER_DXT10, * const LPCLSI_DDS_HEADER_DXT10;
#pragma pack( pop )

		/** An RGBA color component. */
		typedef struct LSI_RGBA {
			float						fRgba[4];
		} * LPLSI_RGBA, * const LPCLSI_RGBA;

		/** Options for creating DXT file images. */
		typedef struct LSI_DXT_OPTIONS {
			/** Alpha cut-off for DXT1 images. */
			float							fAlphaThresh;

			/** Red weight. */
			float							fRedWeight;

			/** Green weight. */
			float							fGreenWeight;

			/** Blue weight. */
			float							fBlueWeight;
		} * LPLSI_DXT_OPTIONS, * const LPCLSI_DXT_OPTIONS;


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
		static LSBOOL LSE_CALL				LoadDds( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn,
			CVector<CImageLib::CTexelBuffer> &_vMipMaps );

		/**
		 * Returns the size, in pixels, of a block of a DDS file.
		 *
		 * \param _pfFormat The DXT format whose block size is to be obtained.
		 * \return Returns the size, in pixels, of a block of a DDS file.
		 */
		static uint32_t LSE_CALL			DxtBlockSize( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Returns the pixel format of the given four-character code.
		 *
		 * \param _ui32FourCc The FourCC to be converted into a pixel format.
		 * \return Returns the pixel format of the given four-character code.
		 */
		static LSI_PIXEL_FORMAT LSE_CALL	DxtPixelFormat( uint32_t _ui32FourCc );

		/**
		 * Gets the pitch of a FourCC image format.
		 *
		 * \param _ui32Width The width of the image.
		 * \param _pfFormat The format of the image.
		 * \return Returns the width, in bytes, of a scanline in the given DDS format.
		 */
		static uint32_t LSE_CALL			DxtScanlineWidth( uint32_t _ui32Width, LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Gets the total number of scanlines in a given DDS format.
		 *
		 * \param _ui32Height Height of the image.
		 * \return Returns the number of scanlines in the given DDS format.
		 */
		static uint32_t LSE_CALL			DxtTotalScanlines( uint32_t _ui32Height );

		/**
		 * Returns the total size of a compressed image given a factor and its width and height.
		 *
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _ui32Factor Multiplier.
		 * \return Returns the size of the compressed data.
		 */
		static uint32_t LSE_CALL			GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Factor );

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
		static LSBOOL LSE_CALL				Decompress_DXT1( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

		/**
		 * Decompresses a texel buffer from DXT2.
		 *
		 * \param _pfFormat The returned format of the decompresed data.
		 * \param _ptbSrc The source buffer of blocks.
		 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
		 * \return Returns true or false to indicate the success of the conversion.
		 */
		static LSBOOL LSE_CALL				Decompress_DXT2( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) { return Decompress_DXT3( _pfFormat, _ptbSrc, _ptbDst, _ui32Width, _ui32Height, _bSrgb ); }

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
		static LSBOOL LSE_CALL				Decompress_DXT3( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

		/**
		 * Decompresses a texel buffer from DXT4.
		 *
		 * \param _pfFormat The returned format of the decompresed data.
		 * \param _ptbSrc The source buffer of blocks.
		 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
		 * \return Returns true or false to indicate the success of the conversion.
		 */
		static LSBOOL LSE_CALL				Decompress_DXT4( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb ) { return Decompress_DXT5( _pfFormat, _ptbSrc, _ptbDst, _ui32Width, _ui32Height, _bSrgb ); }

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
		static LSBOOL LSE_CALL				Decompress_DXT5( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static LSBOOL LSE_CALL				Decompress_BC4U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static LSBOOL LSE_CALL				Decompress_BC4S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static LSBOOL LSE_CALL				Decompress_BC5U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static LSBOOL LSE_CALL				Decompress_BC5S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static LSBOOL LSE_CALL				Decompress_BC6U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static LSBOOL LSE_CALL				Decompress_BC6S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

		/**
		 * Decompresses a texel buffer from BC7U.
		 *
		 * \param _pfFormat The returned format of the decompresed data.
		 * \param _ptbSrc The source buffer of blocks.
		 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
		 * \return Returns true or false to indicate the success of the conversion.
		 */
		static LSBOOL LSE_CALL				Decompress_BC7U( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

		/**
		 * Decompresses a texel buffer from BC7S.
		 *
		 * \param _pfFormat The returned format of the decompresed data.
		 * \param _ptbSrc The source buffer of blocks.
		 * \param _ptbDst The destination buffer into which the blocks will be uncompressed.
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _bSrgb Indicates whether the texels should be considered sRGB or linear during decoding.
		 * \return Returns true or false to indicate the success of the conversion.
		 */
		static LSBOOL LSE_CALL				Decompress_BC7S( LSI_PIXEL_FORMAT &_pfFormat, const CImageLib::CTexelBuffer * _ptbSrc, CImageLib::CTexelBuffer * _ptbDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

		/**
		 * DXT1 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt1ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> DXT1 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _ui8DefaultAlphaThresh, unsigned _bSrgb>
		static bool LSE_CALL				Dxt1FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * DXT2 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt2ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr ) {
			Dxt3ToRgba32F( _pui8Src, _pui8Dst, _ui32Width, _ui32Height, _ui32Depth, _pvParms );
		}

		/**
		 * Generic RGBA32F -> DXT2 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt2FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * DXT3 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt3ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> DXT3 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt3FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * DXT4 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt4ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr ) {
			Dxt5ToRgba32F( _pui8Src, _pui8Dst, _ui32Width, _ui32Height, _ui32Depth, _pvParms );
		}

		/**
		 * Generic RGBA32F -> DXT4 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt4FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * DXT5 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt5ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> DXT5 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Dxt5FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * BC4U -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLum>
		static bool LSE_CALL				Bc4uToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> BC4U conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLum>
		static bool LSE_CALL				Bc4uFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * BC4S -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLum>
		static bool LSE_CALL				Bc4sToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> BC4S conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLum>
		static bool LSE_CALL				Bc4sFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * BC5U -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLumAlpha>
		static bool LSE_CALL				Bc5uToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> BC5U conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLumAlpha>
		static bool LSE_CALL				Bc5uFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * BC5S -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLumAlpha>
		static bool LSE_CALL				Bc5sToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> BC5S conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bLumAlpha>
		static bool LSE_CALL				Bc5sFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * BC6H -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Bc6hToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> BC6H conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Bc6hFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * BC7U -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Bc7uToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> BC7U conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Bc7uFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );


	protected :
		// == Types.
		/** Thread data for converting blocks to DXTn. */
		typedef struct LSI_DXT_THREAD_DATA {
			/** The block X starting point in the source data. */
			uint32_t						ui32SrcX;

			/** The block Y starting point in the source data. */
			uint32_t						ui32SrcY;

			/** The block Z starting point in the source data. */
			uint32_t						ui32SrcZ;

			/** The source image. */
			const uint8_t *					pui8Src;

			/** Image width. */
			uint32_t						ui32Width;

			/** Image height. */
			uint32_t						ui32Height;

			/** Image depth. */
			uint32_t						ui32Depth;

			/** Destination of the compressioned chunk. */
			uint8_t *						pui8Dest;

			/** Pixel format. */
			LSI_PIXEL_FORMAT				pfFormat;

			/** Treat BC5 as LATC2. */
			bool							bLatc;

			/** Indicates the thread has finished. */
			LSBOOL							bDone;

			/** The conversion options. */
			LSI_DXT_OPTIONS *				pdoOptions;
		} * LPLSI_DXT_THREAD_DATA, * const LPCLSI_DXT_THREAD_DATA;

		/** Surface and destination pair. */
		typedef struct LSI_SURFACE_AND_DST {
			/** The surface. */
			rgba_surface					sSurface;

			/** The destination. */
			uint8_t *						pui8Dst;
		} * LPLSI_SURFACE_AND_DST, * const LPCLSI_SURFACE_AND_DST;

		/** Thread data for BC7 conversions. */
		typedef struct LSI_BC7_THREAD_DATA {
			/** The settings. */
			bc7_enc_settings *				pbesSettings;

			/** The surface. */
			const rgba_surface *			psSurface;

			/** The destination. */
			uint8_t *						pui8Dst;

			/** Indicates the thread has finished. */
			LSBOOL							bDone;
		} * LPLSI_BC7_THREAD_DATA, * const LPCLSI_BC7_THREAD_DATA;

		/** Thread data for BC6 conversions. */
		typedef struct LSI_BC6_THREAD_DATA {
			/** The settings. */
			bc6h_enc_settings *				pbesSettings;

			/** The surface. */
			const rgba_surface *			psSurface;

			/** The destination. */
			uint8_t *						pui8Dst;

			/** Indicates the thread has finished. */
			LSBOOL							bDone;
		} * LPLSI_BC6_THREAD_DATA, * const LPCLSI_BC6_THREAD_DATA;

		/** A block of texels for DDS encoding. */
		typedef union LSI_BLOCK {
			struct LSI_COLOR {
				float						fR, fG, fB, fA;
			}								s;
			float							fValues[4];


			// == Operators.
			/**
			 * The less-than operator.
			 *
			 * \param _bOther The object against which to test.
			 * \return Returns true if this object is less than the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL		operator < ( const LSI_BLOCK &_bOther ) const;

			/**
			 * The equality operator.
			 *
			 * \param _bOther The object against which to test.
			 * \return Returns true if this object is equal to the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL		operator == ( const LSI_BLOCK &_bOther ) const;

			/**
			 * The inequality operator.
			 *
			 * \param _bOther The object against which to test.
			 * \return Returns true if this object is not equal to the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL		operator != ( const LSI_BLOCK &_bOther ) const;
		} * LPLSI_BLOCK, * const LPCLSI_BLOCK;


		// == Members.
		/** Determines the mode for decoding DXT textures.  Most implementations use standard math, which creates an ideal decoding of DXT1 (and friends) data,
		 *	but this flag can be used to switch to using the algorithm that NVIDIA GPU's use: http://www.ludicon.com/castano/blog/2009/03/gpu-dxt-decompression/
		 */
		static LSBOOL						m_bUseNVidiaDecode;



		// == Functions.
		/**
		 * Converts a 16-bit R5G6B5 end point into an LSI_RGBA color value.
		 *
		 * \param _ui16Point The end point to convert.
		 * \param _rRgba The output for the converted color.
		 */
		template <unsigned _bSrgb>
		static LSE_INLINE void LSE_CALL		Rgb565ToRgbaF32( uint16_t _ui16Point, LSI_RGBA &_rRgba );

		/**
		 * Decodes a single block of DXT1.
		 *
		 * \param _ui64Block The block to decode.
		 * \param _prPalette The created palette (contains 4 entries).
		 */
		template <unsigned _bSrgb>
		static void LSE_CALL				DecodeDXT1( uint64_t _ui64Block, LSI_RGBA * _prPalette );

		/**
		 * Decodes a single block of DXT3.
		 *
		 * \param _ui64Block The block to decode.
		 * \param _prPalette The created palette (contains 4 entries).
		 */
		template <unsigned _bSrgb>
		static void LSE_CALL				DecodeDXT3( uint64_t _ui64Block, LSI_RGBA * _prPalette );

		/**
		 * Decodes a single block of DXT3 alpha.
		 *
		 * \param _ui64Block The block to decode.
		 * \param _pfPalette The created palette (contains 16 entries).
		 */
		static void LSE_CALL				DecodeDXT3_Alpha( uint64_t _ui64Block, float * _pfPalette );

		/**
		 * Decodes a single block of BC4U.
		 *
		 * \param _ui64Block The block to decode.
		 * \param _pfPalette The created palette (contains 8 entries).
		 */
		static void LSE_CALL				DecodeBC4U( uint64_t _ui64Block, float * _pfPalette );

		/**
		 * Decodes a single block of BC4S.
		 *
		 * \param _ui64Block The block to decode.
		 * \param _pfPalette The created palette (contains 8 entries).
		 */
		static void LSE_CALL				DecodeBC4S( uint64_t _ui64Block, float * _pfPalette );

		/**
		 * Gets the indices from a DXT1 block. 
		 *
		 * \param _ui64Block The block to decode.
		 * \param _pui8Indices The 16 indices as extracted from the block.
		 */
		static void LSE_CALL				Dxt1Indices( uint64_t _ui64Block, uint8_t * _pui8Indices );

		/**
		 * Gets the indices from a BC4 block. 
		 *
		 * \param _ui64Block The block to decode.
		 * \param _pui8Indices The 16 indices as extracted from the block.
		 */
		static void LSE_CALL				Bc4Indices( uint64_t _ui64Block, uint8_t * _pui8Indices );

		/**
		 * Thread function for converting a block of texels to DXTn formats.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_DXT_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		template <unsigned _bSrgb>
		static uint32_t LSH_CALL			DxtThread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to the BC6 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_BC6_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Bc6Thread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to the BC7 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_BC7_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Bc7Thread( void * _lpParameter );

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
		static void LSE_CALL				DivideSurface( LSI_SURFACE_AND_DST &_sadSurface, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t _ui32RowsPerSection, uint32_t _ui32ThisSectionIdx, LSI_PIXEL_FORMAT _pfFormat );

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
		static bool LSE_CALL				DivideSurface( CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> &_vSurfaces, uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t _ui32RowsPerSection, LSI_PIXEL_FORMAT _pfFormat );
		
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
		static bool LSE_CALL				ExpandImageRgbaF32( const LSI_RGBA * _prSrc, CVectorPoD<LSI_RGBA, uint32_t> &_vDst, uint32_t &_ui32Width, uint32_t &_ui32Height, bool _bSrgb );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * The less-than operator.
	 *
	 * \param _bOther The object against which to test.
	 * \return Returns true if this object is less than the given object.
	 */
	LSE_INLINE LSBOOL LSE_CALL CDds::LSI_BLOCK::operator < ( const LSI_BLOCK &_bOther ) const {
		return fValues[0] < _bOther.fValues[0] && fValues[1] < _bOther.fValues[1] &&
			fValues[2] < _bOther.fValues[2] && fValues[3] < _bOther.fValues[3];
	}

	 /**
	 * The equality operator.
	 *
	 * \param _bOther The object against which to test.
	 * \return Returns true if this object is equal to the given object.
	 */
	LSE_INLINE LSBOOL LSE_CALL CDds::LSI_BLOCK::operator == ( const LSI_BLOCK &_bOther ) const {
		return fValues[0] == _bOther.fValues[0] && fValues[1] == _bOther.fValues[1] &&
			fValues[2] == _bOther.fValues[2] && fValues[3] == _bOther.fValues[3];
	}

	 /**
	 * The inequality operator.
	 *
	 * \param _bOther The object against which to test.
	 * \return Returns true if this object is not equal to the given object.
	 */
	LSE_INLINE LSBOOL LSE_CALL CDds::LSI_BLOCK::operator != ( const LSI_BLOCK &_bOther ) const {
		return fValues[0] != _bOther.fValues[0] || fValues[1] != _bOther.fValues[1] ||
			fValues[2] != _bOther.fValues[2] || fValues[3] != _bOther.fValues[3];
	}

	// == Functions.
	/**
	 * DXT1 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt1ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_DXT1_BLOCK {
			uint64_t ui64Rgb;
		};
		const LSI_DXT1_BLOCK * pbbBlocks = reinterpret_cast<const LSI_DXT1_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_DXT1_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		LSI_RGBA fPaletteRgb[4];
		uint8_t ui8IndicesRgb[16];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X].ui64Rgb;
					DecodeDXT1<_bSrgb>( ui64ThisBlock, fPaletteRgb );
					Dxt1Indices( ui64ThisBlock, ui8IndicesRgb );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_R];
							(*prgbaRow0).fTexels[1] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_G];
							(*prgbaRow0).fTexels[2] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_B];
							(*prgbaRow0).fTexels[3] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_A];
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> DXT1 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _ui8DefaultAlphaThresh, unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt1FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = _ui8DefaultAlphaThresh / 255.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_DXT1;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_DXT1 ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> DXT2 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt2FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_DXT2;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_DXT2 ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}
		return true;
	}

	/**
	 * DXT3 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt3ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_DXT3_BLOCK {
			uint64_t ui64Alpha;
			uint64_t ui64Rgb;
		};
		const LSI_DXT3_BLOCK * pbbBlocks = reinterpret_cast<const LSI_DXT3_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_DXT3_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		LSI_RGBA fPaletteRgb[4];
		float fAlphaPalette[16];
		uint8_t ui8IndicesRgb[16];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X].ui64Rgb;
					DecodeDXT3<_bSrgb>( ui64ThisBlock, fPaletteRgb );
					Dxt1Indices( ui64ThisBlock, ui8IndicesRgb );

					DecodeDXT3_Alpha( pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X].ui64Alpha, fAlphaPalette );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_R];
							(*prgbaRow0).fTexels[1] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_G];
							(*prgbaRow0).fTexels[2] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_B];
							(*prgbaRow0).fTexels[3] = fAlphaPalette[I];
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> DXT3 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt3FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_DXT3;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_DXT3 ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> DXT4 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt4FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_DXT4;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_DXT4 ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}
		return true;
	}

	/**
	 * DXT5 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt5ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_DXT5_BLOCK {
			uint64_t ui64Alpha;
			uint64_t ui64Rgb;
		};
		const LSI_DXT5_BLOCK * pbbBlocks = reinterpret_cast<const LSI_DXT5_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_DXT5_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		LSI_RGBA fPaletteRgb[4];
		float fAlphaPalette[8];
		uint8_t ui8IndicesRgb[16];
		uint8_t ui8IndicesA[16];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X].ui64Rgb;
					DecodeDXT3<_bSrgb>( ui64ThisBlock, fPaletteRgb );
					Dxt1Indices( ui64ThisBlock, ui8IndicesRgb );

					ui64ThisBlock = pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X].ui64Alpha;
					DecodeBC4U( ui64ThisBlock, fAlphaPalette );
					Bc4Indices( ui64ThisBlock, ui8IndicesA );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_R];
							(*prgbaRow0).fTexels[1] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_G];
							(*prgbaRow0).fTexels[2] = fPaletteRgb[ui8IndicesRgb[I]].fRgba[LSI_PC_B];
							(*prgbaRow0).fTexels[3] = fAlphaPalette[ui8IndicesA[I]];
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> DXT5 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Dxt5FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_DXT5;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_DXT5 ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}

		return true;
	}

	/**
	 * BC4U -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLum>
	bool LSE_CALL CDds::Bc4uToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		const uint64_t * pui64Blocks = reinterpret_cast<const uint64_t *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		float fPalette[8];
		uint8_t ui8Indices[16];
		// Size per slice.
		uint32_t ui32SrcSliceSize = ui32BlocksW * ui32BlocksH;
		uint32_t ui32SliceSize = _ui32Width * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pui64Blocks[Z*ui32SrcSliceSize+Y*ui32BlocksW+X];
					DecodeBC4U( ui64ThisBlock, fPalette );
					Bc4Indices( ui64ThisBlock, ui8Indices );
					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32SliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPalette[ui8Indices[I]];
							(*prgbaRow0).fTexels[1] = _bLum ? fPalette[ui8Indices[I]] : 0.0f;
							(*prgbaRow0).fTexels[2] = _bLum ? fPalette[ui8Indices[I]] : 0.0f;
							(*prgbaRow0).fTexels[3] = 1.0f;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> BC4U conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLum>
	bool LSE_CALL CDds::Bc4uFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_BC4U;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].bLatc = _bLum;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_BC4U ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}

		return true;
	}

	/**
	 * BC4S -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLum>
	bool LSE_CALL CDds::Bc4sToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		const uint64_t * pui64Blocks = reinterpret_cast<const uint64_t *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		float fPalette[8];
		uint8_t ui8Indices[16];
		// Size per slice.
		uint32_t ui32SrcSliceSize = ui32BlocksW * ui32BlocksH;
		uint32_t ui32SliceSize = _ui32Width * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pui64Blocks[Z*ui32SrcSliceSize+Y*ui32BlocksW+X];
					DecodeBC4S( ui64ThisBlock, fPalette );
					Bc4Indices( ui64ThisBlock, ui8Indices );
					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32SliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPalette[ui8Indices[I]];
							(*prgbaRow0).fTexels[1] = _bLum ? fPalette[ui8Indices[I]] : 0.0f;
							(*prgbaRow0).fTexels[2] = _bLum ? fPalette[ui8Indices[I]] : 0.0f;
							(*prgbaRow0).fTexels[3] = 1.0f;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> BC4S conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLum>
	bool LSE_CALL CDds::Bc4sFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_BC4S;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].bLatc = _bLum;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_BC4S ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}

		return true;
	}

	/**
	 * BC5U -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLumAlpha>
	bool LSE_CALL CDds::Bc5uToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_BC5_BLOCK {
			uint64_t ui64Red;
			uint64_t ui64Green;
		};
		const LSI_BC5_BLOCK * pbbBlocks = reinterpret_cast<const LSI_BC5_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		float fPaletteR[8];
		uint8_t ui8IndicesR[16];
		float fPaletteG[8];
		uint8_t ui8IndicesG[16];
		// Size per slice.
		uint32_t ui32SrcSliceSize = ui32BlocksW * ui32BlocksH;
		uint32_t ui32SliceSize = _ui32Width * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pbbBlocks[Z*ui32SrcSliceSize+Y*ui32BlocksW+X].ui64Red;
					DecodeBC4U( ui64ThisBlock, fPaletteR );
					Bc4Indices( ui64ThisBlock, ui8IndicesR );

					ui64ThisBlock = pbbBlocks[Z*ui32SrcSliceSize+Y*ui32BlocksW+X].ui64Green;
					DecodeBC4U( ui64ThisBlock, fPaletteG );
					Bc4Indices( ui64ThisBlock, ui8IndicesG );
					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32SliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							if ( _bLumAlpha ) {
								(*prgbaRow0).fTexels[0] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[1] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[2] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[3] = fPaletteG[ui8IndicesG[I]];
							}
							else {
								(*prgbaRow0).fTexels[0] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[1] = fPaletteG[ui8IndicesG[I]];
								(*prgbaRow0).fTexels[2] = 0.0f;
								(*prgbaRow0).fTexels[3] = 1.0f;
							}
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> BC5U conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLumAlpha>
	bool LSE_CALL CDds::Bc5uFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_BC5U;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].bLatc = _bLumAlpha;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_BC5U ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}

		return true;
	}

	/**
	 * BC5S -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLumAlpha>
	bool LSE_CALL CDds::Bc5sToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_BC5_BLOCK {
			uint64_t ui64Red;
			uint64_t ui64Green;
		};
		const LSI_BC5_BLOCK * pbbBlocks = reinterpret_cast<const LSI_BC5_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		float fPaletteR[8];
		uint8_t ui8IndicesR[16];
		float fPaletteG[8];
		uint8_t ui8IndicesG[16];
		// Size per slice.
		uint32_t ui32SrcSliceSize = ui32BlocksW * ui32BlocksH;
		uint32_t ui32SliceSize = _ui32Width * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					uint64_t ui64ThisBlock = pbbBlocks[Z*ui32SrcSliceSize+Y*ui32BlocksW+X].ui64Red;
					DecodeBC4S( ui64ThisBlock, fPaletteR );
					Bc4Indices( ui64ThisBlock, ui8IndicesR );

					ui64ThisBlock = pbbBlocks[Z*ui32SrcSliceSize+Y*ui32BlocksW+X].ui64Green;
					DecodeBC4S( ui64ThisBlock, fPaletteG );
					Bc4Indices( ui64ThisBlock, ui8IndicesG );
					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32SliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							if ( _bLumAlpha ) {
								(*prgbaRow0).fTexels[0] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[1] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[2] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[3] = fPaletteG[ui8IndicesG[I]];
							}
							else {
								(*prgbaRow0).fTexels[0] = fPaletteR[ui8IndicesR[I]];
								(*prgbaRow0).fTexels[1] = fPaletteG[ui8IndicesG[I]];
								(*prgbaRow0).fTexels[2] = 0.0f;
								(*prgbaRow0).fTexels[3] = 1.0f;
							}
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> BC5S conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bLumAlpha>
	bool LSE_CALL CDds::Bc5sFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms ) {
		LSI_DXT_OPTIONS doOptions;
		if ( _pvParms ) {
			doOptions = (*reinterpret_cast<LSI_DXT_OPTIONS *>(_pvParms));
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[6];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
			dtdData[I].bDone = true;
		}

		// Number of blocks along the width.
		uint32_t ui32Width = (_ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (_ui32Height + 3) >> 2;
		// Size per slice.
		uint32_t ui32SliceSize = ui32Width * ui32TotalScanLines * sizeof( uint64_t );
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint8_t * pui8Dst = &_pui8Dst[Z*ui32SliceSize];
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].ui32SrcZ = Z;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = LSI_PF_BC5S;
						dtdData[I].pui8Src = _pui8Src;
						dtdData[I].ui32Width = _ui32Width;
						dtdData[I].ui32Height = _ui32Height;
						dtdData[I].ui32Depth = _ui32Depth;
						dtdData[I].bLatc = _bLumAlpha;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread<_bSrgb>, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], nullptr );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					//CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( LSI_PF_BC5S ) * ui32Width * LSI_DXT_ROWS;
			}
		}

		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::WaitForThread( tThreads[I], nullptr );
			CThreadLib::CloseThread( tThreads[I] );
		}

		return true;
	}

	/**
	 * BC6H -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Bc6hToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_BC6_BLOCK {
			uint64_t ui64Block0;
			uint64_t ui64Block1;
		};
		const LSI_BC6_BLOCK * pbbBlocks = reinterpret_cast<const LSI_BC6_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_BC6_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		CFloat16 fPaletteRgbF16[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockBPTC_FLOAT( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), DETEX_MODE_MASK_ALL_MODES_BPTC_FLOAT,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbF16) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = fPaletteRgbF16[I*4+LSI_PC_R];
							(*prgbaRow0).fTexels[1] = fPaletteRgbF16[I*4+LSI_PC_G];
							(*prgbaRow0).fTexels[2] = fPaletteRgbF16[I*4+LSI_PC_B];
							(*prgbaRow0).fTexels[3] = 1.0f;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> BC6H conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Bc6hFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#ifndef LSE_IPHONE
		uint32_t ui32SliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;
		uint32_t ui32DstSliceSize = CDds::GetCompressedSize( _ui32Width, _ui32Height, CDds::DxtBlockSize( LSI_PF_BC6U ) );

#ifdef LSI_BC_ROWS
		CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> vSurfaces;
		uint8_t * pui8Surf;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_BC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_BC6_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
#else
		rgba_surface sSurface;
		sSurface.width = LSE_ROUND_UP( _ui32Width, 4 );
		sSurface.height = LSE_ROUND_UP( _ui32Height, 4 );
		sSurface.stride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, sSurface.width );
#endif	// #ifdef LSI_BC_ROWS

#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))
		
		CVectorPoD<LSI_RGBA, uint32_t> vExpanded;

		bc6h_enc_settings besSettings;
		::GetProfile_bc6h_veryslow( &besSettings );
		//::GetProfile_bc6h_veryfast( &besSettings );
		// Have to convert to 16-bit floats.
		CVectorPoD<float, uint32_t> vHalfBuffer;
		if ( !vHalfBuffer.Resize( LSE_ROUND_UP( _ui32Width, 4 ) * LSE_ROUND_UP( _ui32Height, 4 ) * 4 ) ) { return false; }
				
		CFloat16 fZero = 0.0f;
		uint16_t ui16Zero = fZero.GetInt();
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
#ifdef LSI_BC_ROWS
			pui8Surf = LSI_CAST( _pui8Src );
#else
			sSurface.ptr = LSI_CAST( _pui8Src );
#endif	// ##ifdef LSI_BC_ROWS

			uint32_t ui32W = _ui32Width;
			uint32_t ui32H = _ui32Height;
			uint8_t * pui8ConvertedSrc = LSI_CAST( _pui8Src );
			if ( (_ui32Width % 4) != 0 || (_ui32Height % 4) != 0 ) {
				if ( !ExpandImageRgbaF32( reinterpret_cast<const LSI_RGBA *>(_pui8Src), vExpanded, ui32W, ui32H, false ) ) { return false; }
				pui8ConvertedSrc = LSI_CAST( &vExpanded[0] );
			}

			for ( uint32_t Y = ui32H; Y--; ) {
				uint32_t ui32Row = Y * ui32W * sizeof( LSI_RGBA );
				uint32_t ui32DstRow = Y * ui32W * 4;
				for ( uint32_t X = ui32W; X--; ) {
					uint32_t ui32Index = ui32Row + X * sizeof( float ) * 4;
					CFloat16 fR = reinterpret_cast<float *>(&pui8ConvertedSrc[ui32Index])[LSI_PC_R];
					CFloat16 fG = reinterpret_cast<float *>(&pui8ConvertedSrc[ui32Index])[LSI_PC_G];
					CFloat16 fB = reinterpret_cast<float *>(&pui8ConvertedSrc[ui32Index])[LSI_PC_B];
					vHalfBuffer[ui32DstRow+X*4+0] = fR.GetInt();
					vHalfBuffer[ui32DstRow+X*4+1] = fG.GetInt();
					vHalfBuffer[ui32DstRow+X*4+2] = fB.GetInt();
					vHalfBuffer[ui32DstRow+X*4+3] = ui16Zero;
				}
			}
#ifdef LSI_BC_ROWS
			if ( !DivideSurface( vSurfaces, LSI_CAST( &vHalfBuffer[0] ), _pui8Dst, ui32W, ui32H, LSI_BC_ROWS, LSI_PF_BC6 ) ) { return false; }

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
							btdData[I].pbesSettings = &besSettings;
							if ( !CThreadLib::CreateThread( Bc6Thread, &btdData[I], &tThreads[I] ) ) {
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
			sSurface.ptr = LSI_CAST( &vHalfBuffer[0] );
			CompressBlocksBC6H( &sSurface, _pui8Dst, &besSettings );
#endif	// #ifdef LSI_BC_ROWS

			_pui8Src += ui32SliceSize;
			_pui8Dst += ui32DstSliceSize;
		}
#undef LSI_CAST
#endif	// #ifndef LSE_IPHONE
		return true;
	}

	/**
	 * BC7U -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Bc7uToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_DXT7_BLOCK {
			uint64_t ui64Block0;
			uint64_t ui64Block1;
		};
		const LSI_DXT7_BLOCK * pbbBlocks = reinterpret_cast<const LSI_DXT7_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_DXT7_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint8_t fPaletteRgbUi8[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockBPTC( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), DETEX_MODE_MASK_ALL_MODES_BPTC,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbUi8) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = _bSrgb ? CImageLib::SRgbToLinear( fPaletteRgbUi8[I*4+LSI_PC_R] / 255.0f ) : fPaletteRgbUi8[I*4+LSI_PC_R] / 255.0f;
							(*prgbaRow0).fTexels[1] = _bSrgb ? CImageLib::SRgbToLinear( fPaletteRgbUi8[I*4+LSI_PC_G] / 255.0f ) : fPaletteRgbUi8[I*4+LSI_PC_G] / 255.0f;
							(*prgbaRow0).fTexels[2] = _bSrgb ? CImageLib::SRgbToLinear( fPaletteRgbUi8[I*4+LSI_PC_B] / 255.0f ) : fPaletteRgbUi8[I*4+LSI_PC_B] / 255.0f;
							(*prgbaRow0).fTexels[3] = fPaletteRgbUi8[I*4+LSI_PC_A] / 255.0f;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA32F -> BC7U conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CDds::Bc7uFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#ifndef LSE_IPHONE
		uint32_t ui32SliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;
		uint32_t ui32DstSliceSize = CDds::GetCompressedSize( _ui32Width, _ui32Height, CDds::DxtBlockSize( LSI_PF_BC7 ) );

#ifdef LSI_BC_ROWS
		CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> vSurfaces;
		uint8_t * pui8Surf;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_BC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_BC7_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
#else
		rgba_surface sSurface;
		sSurface.width = LSE_ROUND_UP( _ui32Width, 4 );
		sSurface.height = LSE_ROUND_UP( _ui32Height, 4 );
		sSurface.stride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, sSurface.width );
#endif	// #ifdef LSI_BC_ROWS
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))

		struct LSI_RGBA32 {
			float fTexels[4];
		};
		CVectorPoD<LSI_RGBA32, uint32_t> vTmp;
		if ( _bSrgb != 0 ) {
			if ( !vTmp.Resize( _ui32Width * _ui32Height ) ) { return false; }
		}

		bc7_enc_settings besSettings;
		::GetProfile_alpha_slow( &besSettings );

		CVectorPoD<LSI_RGBA, uint32_t> vExpanded;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
#ifdef LSI_BC_ROWS
			pui8Surf = LSI_CAST( _pui8Src );
#else
			sSurface.ptr = LSI_CAST( _pui8Src );
#endif	// ##ifdef LSI_BC_ROWS

			uint32_t ui32W = _ui32Width;
			uint32_t ui32H = _ui32Height;
			bool bDidsRGB = false;
			if ( (_ui32Width % 4) != 0 || (_ui32Height % 4) != 0 ) {
				if ( !ExpandImageRgbaF32( reinterpret_cast<const LSI_RGBA *>(_pui8Src), vExpanded, ui32W, ui32H, _bSrgb ) ) { return false; }
#ifdef LSI_BC_ROWS
				pui8Surf = LSI_CAST( &vExpanded[0] );
#else
				sSurface.ptr = LSI_CAST( &vExpanded[0] );
#endif	// ##ifdef LSI_BC_ROWS
				bDidsRGB = true;
			}


			if ( _bSrgb != 0 && !bDidsRGB ) {
				for ( uint32_t Y = 0; Y < ui32H; ++Y ) {
					for ( uint32_t X = 0; X < ui32W; ++X ) {
						LSI_RGBA32 & rDst = vTmp[Y*ui32W+X];
						const LSI_RGBA32 & rSrc = reinterpret_cast<const LSI_RGBA32 *>(_pui8Src)[Y*ui32W+X];
						rDst.fTexels[LSI_PC_R] = CImageLib::LinearToSRgb( rSrc.fTexels[LSI_PC_R] );
						rDst.fTexels[LSI_PC_G] = CImageLib::LinearToSRgb( rSrc.fTexels[LSI_PC_G] );
						rDst.fTexels[LSI_PC_B] = CImageLib::LinearToSRgb( rSrc.fTexels[LSI_PC_B] );
						rDst.fTexels[LSI_PC_A] = rSrc.fTexels[LSI_PC_A];
					}
				}
#ifdef LSI_BC_ROWS
				pui8Surf = LSI_CAST( &vTmp[0] );
#else
				sSurface.ptr = LSI_CAST( &vTmp[0] );
#endif	// ##ifdef LSI_BC_ROWS
			}
#ifdef LSI_BC_ROWS
			if ( !DivideSurface( vSurfaces, pui8Surf, _pui8Dst, ui32W, ui32H, LSI_BC_ROWS, LSI_PF_BC7 ) ) { return false; }

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
							btdData[I].pbesSettings = &besSettings;
							if ( !CThreadLib::CreateThread( Bc7Thread, &btdData[I], &tThreads[I] ) ) {
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
			CompressBlocksBC7( &sSurface, _pui8Dst, &besSettings );
#endif	// #ifdef LSI_BC_ROWS

			_pui8Src += ui32SliceSize;
			_pui8Dst += ui32DstSliceSize;
		}
#undef LSI_CAST
#endif	// #ifndef LSE_IPHONE
		return true;
	}

	/**
	 * Converts a 16-bit R5G6B5 end point into an LSI_RGBA color value.
	 *
	 * \param _ui16Point The end point to convert.
	 * \param _rRgba The output for the converted color.
	 */
	template <unsigned _bSrgb>
	LSE_INLINE void LSE_CALL CDds::Rgb565ToRgbaF32( uint16_t _ui16Point, LSI_RGBA &_rRgba ) {
		const uint32_t ui32Mask5 = (1 << 5) - 1;
		const uint32_t ui32Mask6 = (1 << 6) - 1;
		if ( m_bUseNVidiaDecode ) {
			uint32_t uiR = _ui16Point >> 11;
			uint32_t uiG = ((_ui16Point >> 5) & ui32Mask6);
			uint32_t uiB = _ui16Point & ui32Mask5;

			_rRgba.fRgba[LSI_PC_B] = ((3 * uiB * 22) / 8) / 255.0f;
			_rRgba.fRgba[LSI_PC_G] = ((uiG << 2) | (uiG >> 4)) / 255.0f;
			_rRgba.fRgba[LSI_PC_R] = ((3 * uiR * 22) / 8) / 255.0f;
		}
		else {
			
			_rRgba.fRgba[LSI_PC_R] = (_ui16Point >> 11) / static_cast<float>(ui32Mask5);
			_rRgba.fRgba[LSI_PC_G] = ((_ui16Point >> 5) & ui32Mask6) / static_cast<float>(ui32Mask6);
			_rRgba.fRgba[LSI_PC_B] = (_ui16Point & ui32Mask5) / static_cast<float>(ui32Mask5);
		}
		if ( _bSrgb != 0 ) {
			_rRgba.fRgba[LSI_PC_B] = CImageLib::SRgbToLinear( _rRgba.fRgba[LSI_PC_B] );
			_rRgba.fRgba[LSI_PC_G] = CImageLib::SRgbToLinear( _rRgba.fRgba[LSI_PC_G] );
			_rRgba.fRgba[LSI_PC_R] = CImageLib::SRgbToLinear( _rRgba.fRgba[LSI_PC_R] );
		}
		_rRgba.fRgba[LSI_PC_A] = 1.0f;
	}

	/**
	 * Decodes a single block of DXT1.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _prPalette The created palette (contains 4 entries).
	 */
	template <unsigned _bSrgb>
	void LSE_CALL CDds::DecodeDXT1( uint64_t _ui64Block, LSI_RGBA * _prPalette ) {
		uint16_t ui16Tmp0 = _ui64Block & 0xFFFF;
		uint16_t ui16Tmp1 = (_ui64Block >> 16) & 0xFFFF;
		Rgb565ToRgbaF32<false>( ui16Tmp0, _prPalette[0] );
		Rgb565ToRgbaF32<false>( ui16Tmp1, _prPalette[1] );

		if ( m_bUseNVidiaDecode ) {
			const uint32_t ui32Mask5 = (1 << 5) - 1;
			uint32_t uiR0 = ui16Tmp0 >> 11;
			uint32_t uiB0 = ui16Tmp0 & ui32Mask5;
			uint32_t uiR1 = ui16Tmp1 >> 11;
			uint32_t uiB1 = ui16Tmp1 & ui32Mask5;


			uint32_t uiFullG0 = static_cast<uint32_t>(std::round( _prPalette[0].fRgba[LSI_PC_G] * 255.0f ));
			uint32_t uiFullG1 = static_cast<uint32_t>(std::round( _prPalette[1].fRgba[LSI_PC_G] * 255.0f ));
			int32_t iDiff = uiFullG1 - uiFullG0;


			if ( ui16Tmp0 > ui16Tmp1 ) {
				_prPalette[2].fRgba[LSI_PC_R] = (((2 * uiR0 + uiR1) * 22) / 8) / 255.0f;
				_prPalette[2].fRgba[LSI_PC_G] = ((256 * uiFullG0 + iDiff / 4 + 128 + iDiff * 80) / 256) / 255.0f;
				_prPalette[2].fRgba[LSI_PC_B] = (((2 * uiB0 + uiB1) * 22) / 8) / 255.0f;
				_prPalette[2].fRgba[LSI_PC_A] = 1.0f;

				_prPalette[3].fRgba[LSI_PC_R] = (((2 * uiR1 + uiR0) * 22) / 8) / 255.0f;
				_prPalette[3].fRgba[LSI_PC_G] = ((256 * uiFullG1 - iDiff / 4 + 128 - iDiff * 80) / 256) / 255.0f;
				_prPalette[3].fRgba[LSI_PC_B] = (((2 * uiB1 + uiB0) * 22) / 8) / 255.0f;
				_prPalette[3].fRgba[LSI_PC_A] = 1.0f;
			}
			else {
				_prPalette[2].fRgba[LSI_PC_R] = (((uiR0 + uiR1) * 33) / 8) / 255.0f;
				_prPalette[2].fRgba[LSI_PC_G] = ((256 * uiFullG0 + iDiff / 4 + 128 + iDiff * 128) / 256) / 255.0f;
				_prPalette[2].fRgba[LSI_PC_B] = (((uiB0 + uiB1) * 33) / 8) / 255.0f;
				_prPalette[2].fRgba[LSI_PC_A] = 1.0f;

				_prPalette[3].fRgba[LSI_PC_R] = 0.0f;
				_prPalette[3].fRgba[LSI_PC_G] = 0.0f;
				_prPalette[3].fRgba[LSI_PC_B] = 0.0f;
				_prPalette[3].fRgba[LSI_PC_A] = 0.0f;
			}
		}
		else {

			if ( ui16Tmp0 > ui16Tmp1 ) {
				_prPalette[2].fRgba[LSI_PC_R] = (_prPalette[0].fRgba[LSI_PC_R] * 2.0f + _prPalette[1].fRgba[LSI_PC_R]) / 3.0f;
				_prPalette[2].fRgba[LSI_PC_G] = (_prPalette[0].fRgba[LSI_PC_G] * 2.0f + _prPalette[1].fRgba[LSI_PC_G]) / 3.0f;
				_prPalette[2].fRgba[LSI_PC_B] = (_prPalette[0].fRgba[LSI_PC_B] * 2.0f + _prPalette[1].fRgba[LSI_PC_B]) / 3.0f;
				_prPalette[2].fRgba[LSI_PC_A] = 1.0f;

				_prPalette[3].fRgba[LSI_PC_R] = (_prPalette[0].fRgba[LSI_PC_R] + _prPalette[1].fRgba[LSI_PC_R] * 2.0f) / 3.0f;
				_prPalette[3].fRgba[LSI_PC_G] = (_prPalette[0].fRgba[LSI_PC_G] + _prPalette[1].fRgba[LSI_PC_G] * 2.0f) / 3.0f;
				_prPalette[3].fRgba[LSI_PC_B] = (_prPalette[0].fRgba[LSI_PC_B] + _prPalette[1].fRgba[LSI_PC_B] * 2.0f) / 3.0f;
				_prPalette[3].fRgba[LSI_PC_A] = 1.0f;
			}
			else {
				_prPalette[2].fRgba[LSI_PC_R] = (_prPalette[0].fRgba[LSI_PC_R] + _prPalette[1].fRgba[LSI_PC_R]) * 0.5f;
				_prPalette[2].fRgba[LSI_PC_G] = (_prPalette[0].fRgba[LSI_PC_G] + _prPalette[1].fRgba[LSI_PC_G]) * 0.5f;
				_prPalette[2].fRgba[LSI_PC_B] = (_prPalette[0].fRgba[LSI_PC_B] + _prPalette[1].fRgba[LSI_PC_B]) * 0.5f;
				_prPalette[2].fRgba[LSI_PC_A] = 1.0f;

				_prPalette[3].fRgba[LSI_PC_R] = 0.0f;
				_prPalette[3].fRgba[LSI_PC_G] = 0.0f;
				_prPalette[3].fRgba[LSI_PC_B] = 0.0f;
				_prPalette[3].fRgba[LSI_PC_A] = 0.0f;
			}
		}
		if ( _bSrgb != 0 ) {
			for ( uint32_t I = 0; I < 4; ++I ) {
				_prPalette[I].fRgba[LSI_PC_R] = CImageLib::SRgbToLinear( _prPalette[I].fRgba[LSI_PC_R] );
				_prPalette[I].fRgba[LSI_PC_G] = CImageLib::SRgbToLinear( _prPalette[I].fRgba[LSI_PC_G] );
				_prPalette[I].fRgba[LSI_PC_B] = CImageLib::SRgbToLinear( _prPalette[I].fRgba[LSI_PC_B] );
			}
		}
	}

	/**
	 * Decodes a single block of DXT3.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _prPalette The created palette (contains 4 entries).
	 */
	template <unsigned _bSrgb>
	void LSE_CALL CDds::DecodeDXT3( uint64_t _ui64Block, LSI_RGBA * _prPalette ) {
		uint16_t ui16Tmp0 = _ui64Block & 0xFFFF;
		uint16_t ui16Tmp1 = (_ui64Block >> 16) & 0xFFFF;
		Rgb565ToRgbaF32<false>( ui16Tmp0, _prPalette[0] );
		Rgb565ToRgbaF32<false>( ui16Tmp1, _prPalette[1] );

		if ( m_bUseNVidiaDecode ) {
			const uint32_t ui32Mask5 = (1 << 5) - 1;
			uint32_t uiR0 = ui16Tmp0 >> 11;
			uint32_t uiB0 = ui16Tmp0 & ui32Mask5;
			uint32_t uiR1 = ui16Tmp1 >> 11;
			uint32_t uiB1 = ui16Tmp1 & ui32Mask5;


			uint32_t uiFullG0 = static_cast<uint32_t>(std::round( _prPalette[0].fRgba[LSI_PC_G] * 255.0f ));
			uint32_t uiFullG1 = static_cast<uint32_t>(std::round( _prPalette[1].fRgba[LSI_PC_G] * 255.0f ));
			int32_t iDiff = uiFullG1 - uiFullG0;


			_prPalette[2].fRgba[LSI_PC_R] = (((2 * uiR0 + uiR1) * 22) / 8) / 255.0f;
			_prPalette[2].fRgba[LSI_PC_G] = ((256 * uiFullG0 + iDiff / 4 + 128 + iDiff * 80) / 256) / 255.0f;
			_prPalette[2].fRgba[LSI_PC_B] = (((2 * uiB0 + uiB1) * 22) / 8) / 255.0f;
			_prPalette[2].fRgba[LSI_PC_A] = 1.0f;

			_prPalette[3].fRgba[LSI_PC_R] = (((2 * uiR1 + uiR0) * 22) / 8) / 255.0f;
			_prPalette[3].fRgba[LSI_PC_G] = ((256 * uiFullG1 - iDiff / 4 + 128 - iDiff * 80) / 256) / 255.0f;
			_prPalette[3].fRgba[LSI_PC_B] = (((2 * uiB1 + uiB0) * 22) / 8) / 255.0f;
			_prPalette[3].fRgba[LSI_PC_A] = 1.0f;
		}
		else {

			_prPalette[2].fRgba[LSI_PC_R] = (_prPalette[0].fRgba[LSI_PC_R] * 2.0f + _prPalette[1].fRgba[LSI_PC_R]) / 3.0f;
			_prPalette[2].fRgba[LSI_PC_G] = (_prPalette[0].fRgba[LSI_PC_G] * 2.0f + _prPalette[1].fRgba[LSI_PC_G]) / 3.0f;
			_prPalette[2].fRgba[LSI_PC_B] = (_prPalette[0].fRgba[LSI_PC_B] * 2.0f + _prPalette[1].fRgba[LSI_PC_B]) / 3.0f;
			_prPalette[2].fRgba[LSI_PC_A] = 1.0f;

			_prPalette[3].fRgba[LSI_PC_R] = (_prPalette[0].fRgba[LSI_PC_R] + _prPalette[1].fRgba[LSI_PC_R] * 2.0f) / 3.0f;
			_prPalette[3].fRgba[LSI_PC_G] = (_prPalette[0].fRgba[LSI_PC_G] + _prPalette[1].fRgba[LSI_PC_G] * 2.0f) / 3.0f;
			_prPalette[3].fRgba[LSI_PC_B] = (_prPalette[0].fRgba[LSI_PC_B] + _prPalette[1].fRgba[LSI_PC_B] * 2.0f) / 3.0f;
			_prPalette[3].fRgba[LSI_PC_A] = 1.0f;
		}

		if ( _bSrgb != 0 ) {
			for ( uint32_t I = 0; I < 4; ++I ) {
				_prPalette[I].fRgba[LSI_PC_R] = CImageLib::SRgbToLinear( _prPalette[I].fRgba[LSI_PC_R] );
				_prPalette[I].fRgba[LSI_PC_G] = CImageLib::SRgbToLinear( _prPalette[I].fRgba[LSI_PC_G] );
				_prPalette[I].fRgba[LSI_PC_B] = CImageLib::SRgbToLinear( _prPalette[I].fRgba[LSI_PC_B] );
			}
		}
	}

	/**
	 * Thread function for converting a block of texels to DXTn formats.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_DXT_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	template <unsigned _bSrgb>
	uint32_t LSH_CALL CDds::DxtThread( void * _lpParameter ) {
		LSI_DXT_THREAD_DATA * pdtdData = static_cast<LSI_DXT_THREAD_DATA *>(_lpParameter);


		LSI_BLOCK bColors[4][4];
		uint32_t ui32Width = (pdtdData->ui32Width + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (pdtdData->ui32Height + 3) >> 2;

		uint32_t ui32SrcStride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, pdtdData->ui32Width );
		const uint8_t * pui8Src = pdtdData->pui8Src;
		pui8Src += (CImageLib::GetFormatSize( LSI_PF_R32G32B32A32F ) * pdtdData->ui32Width * pdtdData->ui32Height) * pdtdData->ui32SrcZ;
		pui8Src += (ui32SrcStride << 2) * pdtdData->ui32SrcY;

		uint32_t ui32BlockSize = CDds::DxtBlockSize( pdtdData->pfFormat );

		// Do LSI_DXT_ROWS rows at a time.
		uint32_t ui32Mask = 0;
		for ( uint32_t K = 0; K < LSI_DXT_ROWS; ++K ) {
			if ( pdtdData->ui32SrcY >= ui32TotalScanLines ) { break; }
			LSE_PREFETCH_LINE( pui8Src + (K + 1) * ui32SrcStride );

			for ( uint32_t X = pdtdData->ui32SrcX; X < ui32Width; ++X ) {
				uint32_t ui32SrcX = X << 4;							// 4 texels per block wide, 4 channels per texel.
				// Get the 4-by-4 block.
				//float fAlphaHigh = 0.0f, fAlphaLow = 1.0f;
				for ( uint32_t J = 0; J < 4; ++J ) {				// Down the block height.
					const float * pfThisRowStart = reinterpret_cast<const float *>(pui8Src + J * ui32SrcStride);
					if ( J + (pdtdData->ui32SrcY << 2) >= pdtdData->ui32Height ) {
						for ( uint32_t I = 0; I < 4; ++I ) {		// Along the block width.
							for ( uint32_t C = 0; C < 4; ++C ) {	// For each channel in this texel.
								bColors[J][I].fValues[C] = 0.0f;
							}
						}
					}
					else {
						// The row is valid.
						for ( uint32_t I = 0; I < 4; ++I ) {		// Along the block width.
							// We are on a single texel now.  But it might be beyond the width of the image.
							if ( I + (X << 2) >= pdtdData->ui32Width ) {
								for ( uint32_t C = 0; C < 4; ++C ) {// For each channel in this texel.
									bColors[J][I].fValues[C] = 0.0f;
								}
							}
							else {
								const float * pfSrcTexel = pfThisRowStart + ui32SrcX + (I << 2);
								// It is inside the image, so add it to an entry in the block.
								bColors[J][I].s.fR = pfSrcTexel[LSI_PC_R];
								bColors[J][I].s.fG = pfSrcTexel[LSI_PC_G];
								bColors[J][I].s.fB = pfSrcTexel[LSI_PC_B];
								bColors[J][I].s.fA = pfSrcTexel[LSI_PC_A];
								/*fAlphaHigh = CStd::Max<float>( fAlphaHigh, bColors[J][I].s.fA );
								fAlphaLow = CStd::Min<float>( fAlphaLow, bColors[J][I].s.fA );*/
								ui32Mask |= 1 << ((J << 2) + I);
							}
						}
					}
				}

				// The local block is filled.  Send it to the filter to determine the best high and low colors to use.
				int iFlags = squish::kColorMetricPerceptual | squish::kColorIterativeClusterFit | squish::kWeightColorByAlpha;
				
				squish::SquishConfig scConfig = {
					(*pdtdData->pdoOptions).fRedWeight,
					(*pdtdData->pdoOptions).fGreenWeight,
					(*pdtdData->pdoOptions).fBlueWeight,
					(*pdtdData->pdoOptions).fAlphaThresh,
				};
#ifdef SQUISH_USE_FLOATS
				float * pfBlock = bColors[0][0].fValues;
#else
				squish::u8 u8Block[4][4][4];
				for ( uint32_t Y = 0; Y < 4; ++Y ) {			// Block height.
					for ( uint32_t X = 0; X < 4; ++X ) {		// Block width.
						for ( uint32_t I = 0; I < 4; ++I ) {	// Colors.
							u8Block[Y][X][I] = static_cast<squish::u8>(bColors[Y][X].fValues[I] * 255.0f);
						}
					}
				}
#endif	// #ifdef SQUISH_USE_FLOATS
				switch ( pdtdData->pfFormat ) {
					case LSI_PF_DXT1 : {
						iFlags |= squish::kDxt1;
						break;
					}
					case LSI_PF_DXT2 : {
						// Premultiply the alpha in the block and fall through.
						for ( uint32_t G = 0; G < 4; ++G ) {
							for ( uint32_t H = 0; H < 4; ++H ) {
								float fAlpha = bColors[G][H].s.fA;
								bColors[G][H].s.fR *= fAlpha;
								bColors[G][H].s.fG *= fAlpha;
								bColors[G][H].s.fB *= fAlpha;
							}
						}
					}
					case LSI_PF_DXT3 : {
						iFlags |= squish::kDxt3;
						break;
					}
					case LSI_PF_DXT4 : {
						// Premultiply the alpha in the block and fall through.
						for ( uint32_t G = 0; G < 4; ++G ) {
							for ( uint32_t H = 0; H < 4; ++H ) {
								float fAlpha = bColors[G][H].s.fA;
								bColors[G][H].s.fR *= fAlpha;
								bColors[G][H].s.fG *= fAlpha;
								bColors[G][H].s.fB *= fAlpha;
							}
						}
					}
					case LSI_PF_DXT5 : {
						iFlags |= squish::kDxt5;
						break;
					}
					case LSI_PF_BC4S : {
						iFlags |= squish::kSigned;
						// Fall through.
					}
					case LSI_PF_BC4 : {
						iFlags |= squish::kBc4;
						if ( pdtdData->bLatc ) {
							// Only channel R is used for color, so update channel R with the combined weights of every RGB channel.
							for ( uint32_t G = 0; G < 4; ++G ) {
								for ( uint32_t H = 0; H < 4; ++H ) {
									bColors[G][H].s.fR = bColors[G][H].s.fR * pdtdData->pdoOptions->fRedWeight +
										bColors[G][H].s.fG * pdtdData->pdoOptions->fGreenWeight +
										bColors[G][H].s.fB * pdtdData->pdoOptions->fBlueWeight;
								}
							}
						}
						break;
					}
					case LSI_PF_BC5S : {
						iFlags |= squish::kSigned;
						// Fall through.
					}
					case LSI_PF_BC5 : {
						iFlags |= squish::kBc5;
						if ( pdtdData->bLatc ) {
							iFlags |= squish::kBc5IsLatc2;
							// Only channel R is used for color, so update channel R with the combined weights of every RGB channel.
							for ( uint32_t G = 0; G < 4; ++G ) {
								for ( uint32_t H = 0; H < 4; ++H ) {
									bColors[G][H].s.fR = bColors[G][H].s.fR * pdtdData->pdoOptions->fRedWeight +
										bColors[G][H].s.fG * pdtdData->pdoOptions->fGreenWeight +
										bColors[G][H].s.fB * pdtdData->pdoOptions->fBlueWeight;
								}
							}
						}
						break;
					}
					default : {}
				}
				if ( _bSrgb != 0 ) {
					// Apply sRGB (importantly done after pre-multiply of alpha.
					for ( uint32_t G = 0; G < 4; ++G ) {
						for ( uint32_t H = 0; H < 4; ++H ) {
							bColors[G][H].s.fR = CImageLib::LinearToSRgb( bColors[G][H].s.fR );
							bColors[G][H].s.fG = CImageLib::LinearToSRgb( bColors[G][H].s.fG );
							bColors[G][H].s.fB = CImageLib::LinearToSRgb( bColors[G][H].s.fB );
						}
					}
				}
				squish::CompressMasked(
#ifdef SQUISH_USE_FLOATS
					pfBlock,
#else
					reinterpret_cast<squish::u8 *>(u8Block),
#endif	// #ifdef SQUISH_USE_FLOATS
					ui32Mask,
					pdtdData->pui8Dest, iFlags, scConfig );

				pdtdData->pui8Dest += ui32BlockSize;
			}
			pui8Src += (ui32SrcStride << 2);
			++pdtdData->ui32SrcY;
		}

		pdtdData->bDone = true;
		return 0;
	}

}	// namespace lsi

#pragma warning( pop )

#endif	// __LSI_DDS_H__
