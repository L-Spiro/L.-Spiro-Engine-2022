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


#ifndef __LSI_ETC_H__
#define __LSI_ETC_H__

#include "../LSIImageLib.h"
#include "../detex/detex.h"
#ifndef LSE_IPHONE
#include "../Ispc/ispc_texcomp.h"
#endif	// #ifndef LSE_IPHONE

/** Use the "detex" decoder instead of ETCPACK. */
#define LSI_USE_DETEX_FOR_ETC


typedef unsigned char uint8;
#if !defined( LSI_USE_DETEX_FOR_ETC )
typedef unsigned int uint;

extern void decompressBlockETC2c_(unsigned int block_part1, unsigned int block_part2, uint8 *img, int width, int height, int startx, int starty, int channels);
extern void decompressBlockETC21BitAlphaC_(unsigned int block_part1, unsigned int block_part2, uint8 *img, uint8* alphaimg, int width, int height, int startx, int starty, int channelsRGB);
extern void decompressBlockAlpha_(uint8* data,uint8* img,int width,int height,int ix,int iy);
extern void decompressBlockAlpha16bit_(uint8* data,uint8* img,int width,int height,int ix,int iy);

#endif	// #if !defined( LSI_USE_DETEX_FOR_ETC )


extern void compressBlockETC2ExhaustivePerceptual(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockETC2FastPerceptual(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockETC2Exhaustive(uint8 *img, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockETC2Fast(uint8 *img, uint8* alphaimg, uint8 *imgdec,int width,int height,int startx,int starty, unsigned int &compressed1, unsigned int &compressed2);
extern void compressBlockAlphaSlow(uint8* data, int ix, int iy, int width, int /*height*/, uint8* returnData);
extern void compressBlockAlphaFast(uint8 * data, int ix, int iy, int width, int /*height*/, uint8* returnData);
extern void compressBlockAlpha16(uint8* data, int ix, int iy, int width, int height, uint8* returnData);
extern int format;
extern int formatSigned_;
extern bool readCompressParams(void);
extern void setupAlphaTableAndValtab();
enum {ETC1_RGB_NO_MIPMAPS,ETC2PACKAGE_RGB_NO_MIPMAPS,ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD,ETC2PACKAGE_RGBA_NO_MIPMAPS,ETC2PACKAGE_RGBA1_NO_MIPMAPS,ETC2PACKAGE_R_NO_MIPMAPS,ETC2PACKAGE_RG_NO_MIPMAPS,ETC2PACKAGE_R_SIGNED_NO_MIPMAPS,ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS,ETC2PACKAGE_sRGB_NO_MIPMAPS,ETC2PACKAGE_sRGBA_NO_MIPMAPS,ETC2PACKAGE_sRGBA1_NO_MIPMAPS};
enum {SPEED_SLOW, SPEED_FAST, SPEED_MEDIUM};
enum {METRIC_PERCEPTUAL, METRIC_NONPERCEPTUAL};
enum {CODEC_ETC, CODEC_ETC2};

/** How many rows to do per-thread for ETC conversions. */
#define LSI_ETC_ROWS					24

/** Maximum number of threads to run concurrently for ETC conversions. */
#define LSI_ETC_MAX_THREADS				16

namespace lsi {

	/**
	 * Class CEtc
	 * \brief Handles conversion of ETC formats.
	 *
	 * Description: Handles conversion of ETC formats.
	 */
	class CEtc {
	public :
		// == Types.
		/** An RGBAF32 color component. */
		typedef struct LSI_RGBAF32 {
			float							fRgba[4];
		} * LPLSI_RGBAF32, * const LPCLSI_RGBAF32;

		/** An RGBA8 color component. */
		typedef struct LSI_RGBA8 {
			uint8_t							ui8fRgba[4];
		} * LPLSI_RGBA8, * const LPCLSI_RGBA8;

		/** An RGB8 color component. */
		typedef struct LSI_RGB8 {
			uint8_t							ui8fRgba[3];
		} * LPLSI_RGB8, * const LPCLSI_RGB8;


		// == Functions.
		/**
		 * Returns the size, in pixels, of a block.
		 *
		 * \param _pfFormat The ETC format whose block size is to be obtained.
		 * \return Returns the size, in pixels, of a block of ETC.
		 */
		static uint32_t LSE_CALL			EtcBlockSize( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Gets the pitch of an ETC format.
		 *
		 * \param _ui32Width The width of the image.
		 * \param _pfFormat The format of the image.
		 * \return Returns the width, in bytes, of a scanline in the given ETC format.
		 */
		static uint32_t LSE_CALL			EtcScanlineWidth( uint32_t _ui32Width, LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Gets the total number of scanlines in a given ETC format.
		 *
		 * \param _ui32Height Height of the image.
		 * \return Returns the number of scanlines in the given ETC format.
		 */
		static uint32_t LSE_CALL			EtcTotalScanlines( uint32_t _ui32Height );

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
		 * ETC1 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		static bool LSE_CALL				Etc1ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

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
		static bool LSE_CALL				Etc1FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * ETC2 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Etc2ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> ETC2 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Etc2FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * ETC2 EAC -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Etc2EacToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> ETC2 EAC conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Etc2EacFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * ETC2 A1 -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Etc2A1ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> ETC2 A1 conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb>
		static bool LSE_CALL				Etc2A1FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * R11 EAC -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bSigned>
		static bool LSE_CALL				R11EacToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> R11 EAC conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bSigned>
		static bool LSE_CALL				R11EacFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * RG11 EAC -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bSigned>
		static bool LSE_CALL				RG11EacToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> RG11 EAC conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _bSrgb, unsigned _bSigned>
		static bool LSE_CALL				RG11EacFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );


	protected :
		// == Types.
		/** Surface and destination pair. */
		typedef struct LSI_SURFACE_AND_DST {
			/** The surface. */
			rgba_surface					sSurface;

			/** The destination. */
			uint8_t *						pui8Dst;
		} * LPLSI_SURFACE_AND_DST, * const LPCLSI_SURFACE_AND_DST;

		/** Thread data for ETC1 conversions. */
		typedef struct LSI_ETC1_THREAD_DATA {
			/** The settings. */
			etc_enc_settings *				peesSettings;

			/** The surface. */
			const rgba_surface *			psSurface;

			/** The destination. */
			uint8_t *						pui8Dst;

			/** Indicates the thread has finished. */
			LSBOOL							bDone;
		} * LPLSI_ETC1_THREAD_DATA, * const LPCLSI_ETC1_THREAD_DATA;

		/** Structure for holding the rows of a texture divided into parts to send to threads. */
		typedef struct LSI_THREAD_ROWS {
			/** The starting row. */
			uint32_t						ui32Start;

			/** The total rows. */
			uint32_t						ui32Total;
		} * LPLSI_THREAD_ROWS, * const LPCLSI_THREAD_ROWS;

		/** Thread data for ETC2 conversions. */
		typedef struct LSI_ETC2_THREAD_DATA {
			/** Encoder source. */
			uint8 *							pui8EncoderSrc;

			/** Encoder alpha source (optional). */
			uint8 *							pui8EncoderSrcAlpha;

			/** Scratch destination (optional). */
			uint8 *							pui8ScratchDst;

			/** Destination. */
			uint8 *							pui8Dst;

			/** Extended image width. */
			uint32_t						ui32ExtWidth;

			/** Extended image height. */
			uint32_t						ui32ExtHeight;

			/** Blocks wide. */
			uint32_t						ui32BlockW;

			/** The start and total rows to convert. */
			LSI_THREAD_ROWS *				ptrRows;

			/** Indicates the thread has finished. */
			LSBOOL							bDone;
		} * LPLSI_ETC2_THREAD_DATA, * const LPCLSI_ETC2_THREAD_DATA;


		// == Functions.
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
		static bool LSE_CALL				CreateExpandedRgba8( CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight );

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
		static bool LSE_CALL				CreatedExtendedRgb8( CVectorPoD<LSI_RGB8, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight );

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
		static bool LSE_CALL				CreatedExtendedA8( CVectorPoD<uint8_t, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight );

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
		static bool LSE_CALL				CreatedExtendedA11( CVectorPoD<uint16_t, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight );

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
		static bool LSE_CALL				CreatedExtendedRgba8FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb );

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
		static bool LSE_CALL				CreatedExtendedRgba8FromRgbaF32( const LSI_RGBAF32 * _prSrc,
			CVectorPoD<LSI_RGB8, uint32_t> &_pvDstRgb, CVectorPoD<uint8_t, uint32_t> &_pvDstA,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb );

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
		static void LSE_CALL				CopyRgba8ToRgbaF32( const CVectorPoD<LSI_RGBA8, uint32_t> &_pvSrc, LSI_RGBAF32 * _prDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb, bool _bNoAlpha );

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
		static bool LSE_CALL				CreatedExtendedRgb8FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGB8, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb );

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
		static bool LSE_CALL				CreatedExtendedA11FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<uint16_t, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb, bool _bSigned );

		/**
		 * Converts an RGB8 buffer into the destination RGBAF32 buffer, optionally applying an sRGB -> linear conversion as well.
		 *
		 * \param _pvSrc The source RGB8 buffer.
		 * \param _prDst The destination RGBAF32 buffer.
		 * \param _ui32Width The width of the image data, in texels, to which _prDst points.
		 * \param _ui32Height The height of the image data, in texels, to which _prDst points.
		 * \param _bSrgb If true, an sRGB -> linear conversion is done.
		 */
		static void LSE_CALL				CopyRgb8ToRgbaF32( const CVectorPoD<LSI_RGB8, uint32_t> &_pvSrc, LSI_RGBAF32 * _prDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static void LSE_CALL				CopyRgb8A8ToRgbaF32( const CVectorPoD<LSI_RGB8, uint32_t> &_vSrcRgb, const CVectorPoD<uint8_t, uint32_t> &_pvSrcA, LSI_RGBAF32 * _prDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb );

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
		static void LSE_CALL				CopyR16G16ToRgbaF32( const CVectorPoD<uint16_t, uint32_t> &_vSrcR8, const CVectorPoD<uint16_t, uint32_t> * _pvSrcG8, LSI_RGBAF32 * _prDst,
			uint32_t _ui32Width, uint32_t _ui32Height, bool _bSrgb, bool _bSigned );

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
		static bool LSE_CALL				ExpandImageRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBAF32, uint32_t> &_vDst, uint32_t &_ui32Width, uint32_t &_ui32Height, bool _bSrgb );

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
		 * Thread function for converting a block of texels to the ETC1 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC1_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Etc1Thread( void * _lpParameter );

		/**
		 * Divides an image into rows of blocks to be handle by threads later.
		 *
		 * \param _vRows Vector to hold the rows.
		 * \param _ui32BlocksH Total rows of blocks in the image.
		 * \param _ui32MinRows Number of rows to pass to a thread.
		 * \return Returns false only if the vector could not be allocated.
		 */
		static bool LSE_CALL				DivideByRows( CVectorPoD<LSI_THREAD_ROWS, uint32_t> &_vRows, uint32_t _ui32BlocksH, uint32_t _ui32MinRows );

		/**
		 * Thread function for converting a block of texels to the ETC2 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Etc2Thread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to the ETC2 EAC format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Etc2EacThread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to the ETC2 A1 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Etc2A1Thread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to the ETC2 R11 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Etc2R11Thread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to the ETC2 RG11 format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ETC2_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			Etc2RG11Thread( void * _lpParameter );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * ETC2 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CEtc::Etc2ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#if defined( LSI_USE_DETEX_FOR_ETC )
		struct LSI_ETC2_BLOCK {
			uint64_t ui64Block0;
		};
		const LSI_ETC2_BLOCK * pbbBlocks = reinterpret_cast<const LSI_ETC2_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_ETC2_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint8_t fPaletteRgbUi8[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockETC2( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]),
						DETEX_MODE_MASK_ALL_MODES_ETC2,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbUi8) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_R], _bSrgb );
							(*prgbaRow0).fTexels[1] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_G], _bSrgb );
							(*prgbaRow0).fTexels[2] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_B], _bSrgb );
							(*prgbaRow0).fTexels[3] = 1.0f;
						}
					}
				}
			}
		}
#else
		CVectorPoD<LSI_RGB8, uint32_t> vTmp;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreatedExtendedRgb8( vTmp, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }

		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		
		const uint64_t * pui64Src = reinterpret_cast<const uint64_t *>(_pui8Src);
		uint8_t * pui8DecodeDst = reinterpret_cast<uint8_t *>(&vTmp[0]);
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					uint32_t ui32Block1 = CStd::ByteSwap32( static_cast<uint32_t>(pui64Src[Y*ui32BlockW+X]) );
					uint32_t ui32Block2 = CStd::ByteSwap32( static_cast<uint32_t>(pui64Src[Y*ui32BlockW+X] >> 32ULL) );
					::decompressBlockETC2c_( ui32Block1, ui32Block2, pui8DecodeDst, ui32ExtW, ui32ExtH, X << 2, Y << 2, 3 );
				}
			}
			CopyRgb8ToRgbaF32( vTmp, reinterpret_cast<LSI_RGBAF32 *>(_pui8Dst), _ui32Width, _ui32Height, _bSrgb );
			pui64Src += ui32BlockW * ui32BlockH;
			_pui8Dst += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
#endif	// #if defined( LSI_USE_DETEX_FOR_ETC )
		return true;
	}

	/**
	 * Generic RGBA32F -> ETC2 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CEtc::Etc2FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		CVectorPoD<LSI_RGB8, uint32_t> vTmp, vScratch;
		uint32_t ui32ExtW, ui32ExtH;
		
		if ( !CreatedExtendedRgb8( vScratch, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }
		::readCompressParams();
		format = ETC2PACKAGE_RGB_NO_MIPMAPS;
		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(_pui8Dst);
		
		uint8_t * pui8ScratchDst = reinterpret_cast<uint8_t *>(&vScratch[0]);

#ifdef LSI_ETC_ROWS
		CVectorPoD<LSI_THREAD_ROWS, uint32_t> vRows;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ETC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ETC2_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
		if ( !DivideByRows( vRows, ui32BlockH, LSI_ETC_ROWS ) ) { return false; }
#endif	// #ifdef LSI_ETC_ROWS

		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			if ( !CreatedExtendedRgb8FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vTmp, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH, _bSrgb ) ) { return false; }
			uint8_t * pui8EncodeSrc = reinterpret_cast<uint8_t *>(&vTmp[0]);
#ifdef LSI_ETC_ROWS
			for ( uint32_t ui32Index = 0; ui32Index < vRows.Length(); ++ui32Index ) {
				bool bFound = false;
				while ( !bFound ) {
					// Find a free thread.
					for ( uint32_t I = 0; I < LSE_ELEMENTS( tThreads ); ++I ) {
						if ( btdData[I].bDone ) {
							// Thread stopped or never started.
							CThreadLib::CloseThread( tThreads[I] );
						
							btdData[I].bDone = false;
							btdData[I].ptrRows = &vRows[ui32Index];
							btdData[I].pui8EncoderSrc = pui8EncodeSrc;
							btdData[I].pui8Dst = _pui8Dst;
							btdData[I].pui8ScratchDst = pui8ScratchDst;
							btdData[I].ui32BlockW = ui32BlockW;
							btdData[I].ui32ExtHeight = ui32ExtH;
							btdData[I].ui32ExtWidth = ui32ExtW;
							if ( !CThreadLib::CreateThread( Etc2Thread, &btdData[I], &tThreads[I] ) ) {
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
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					uint32_t ui32Block1, ui32Block2;
					::compressBlockETC2ExhaustivePerceptual( pui8EncodeSrc, pui8ScratchDst,
						ui32ExtW, ui32ExtH, X << 2, Y << 2, ui32Block1, ui32Block2 );
					/*::compressBlockETC2FastPerceptual( pui8EncodeSrc, pui8ScratchDst,
						ui32ExtW, ui32ExtH, X << 2, Y << 2, ui32Block1, ui32Block2 );*/
					uint64_t & ui64Dst = pui64Dst[Y*ui32BlockW+X];
					ui64Dst = CStd::ByteSwap32( ui32Block1 ) | (static_cast<uint64_t>(CStd::ByteSwap32( ui32Block2 )) << 32ULL);
				}
			}
#endif	// #ifdef LSI_ETC_ROWS
			pui64Dst += ui32BlockW * ui32BlockH;
			_pui8Src += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
		return true;
	}

	/**
	 * ETC2 EAC -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CEtc::Etc2EacToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#if defined( LSI_USE_DETEX_FOR_ETC )
		struct LSI_ETC2_BLOCK {
			uint64_t ui64Block0;
			uint64_t ui64Block1;
		};
		const LSI_ETC2_BLOCK * pbbBlocks = reinterpret_cast<const LSI_ETC2_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_ETC2_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint8_t fPaletteRgbUi8[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockETC2_EAC( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]),
						DETEX_MODE_MASK_ALL_MODES_ETC2,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbUi8) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_R], _bSrgb );
							(*prgbaRow0).fTexels[1] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_G], _bSrgb );
							(*prgbaRow0).fTexels[2] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_B], _bSrgb );
							(*prgbaRow0).fTexels[3] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_A], false );
						}
					}
				}
			}
		}
#else
		CVectorPoD<LSI_RGB8, uint32_t> vTmpRgb;
		CVectorPoD<uint8_t, uint32_t> vTmpA;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreatedExtendedRgb8( vTmpRgb, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }
		if ( !CreatedExtendedA8( vTmpA, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }

		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
			uint64_t ui64Block2;
		};
		
		const LSI_ETC_EAC * peeSrc = reinterpret_cast<const LSI_ETC_EAC *>(_pui8Src);
		uint8_t * pui8DecodeDst = reinterpret_cast<uint8_t *>(&vTmpRgb[0]);
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					uint32_t ui32Block1 = CStd::ByteSwap32( static_cast<uint32_t>(peeSrc[Y*ui32BlockW+X].ui64Block2) );
					uint32_t ui32Block2 = CStd::ByteSwap32( static_cast<uint32_t>(peeSrc[Y*ui32BlockW+X].ui64Block2 >> 32ULL) );
					::decompressBlockETC2c_( ui32Block1, ui32Block2, pui8DecodeDst, ui32ExtW, ui32ExtH, X << 2, Y << 2, 3 );

					::decompressBlockAlpha_( const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&peeSrc[Y*ui32BlockW+X].ui64Block1)),
						&vTmpA[0],
						ui32ExtW, ui32ExtH,
						X << 2, Y << 2 );
				}
			}
			CopyRgb8A8ToRgbaF32( vTmpRgb, vTmpA, reinterpret_cast<LSI_RGBAF32 *>(_pui8Dst), _ui32Width, _ui32Height, _bSrgb );
			peeSrc += ui32BlockW * ui32BlockH;
			_pui8Dst += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
#endif	// #if defined( LSI_USE_DETEX_FOR_ETC )
		return true;
	}

	/**
	 * Generic RGBA32F -> ETC2 EAC conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CEtc::Etc2EacFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		CVectorPoD<LSI_RGB8, uint32_t> vTmpRgb;
		CVectorPoD<uint8_t, uint32_t> vTmpA;
		CVectorPoD<LSI_RGBA8, uint32_t> vScratch;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreateExpandedRgba8( vScratch, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }

		
		::readCompressParams();
		::setupAlphaTableAndValtab();
		format = ETC2PACKAGE_RGBA_NO_MIPMAPS;
		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
			uint64_t ui64Block2;
		};
		LSI_ETC_EAC * peeDst = reinterpret_cast<LSI_ETC_EAC *>(_pui8Dst);
		uint8_t * pui8ScratchDst = reinterpret_cast<uint8_t *>(&vScratch[0]);
#ifdef LSI_ETC_ROWS
		CVectorPoD<LSI_THREAD_ROWS, uint32_t> vRows;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ETC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ETC2_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
		if ( !DivideByRows( vRows, ui32BlockH, LSI_ETC_ROWS ) ) { return false; }
#endif	// #ifdef LSI_ETC_ROWS
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			if ( !CreatedExtendedRgba8FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vTmpRgb, vTmpA, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH, _bSrgb ) ) { return false; }
			uint8_t * pui8EncodeSrcRgb = reinterpret_cast<uint8_t *>(&vTmpRgb[0]);
			uint8_t * pui8EncodeSrcA = reinterpret_cast<uint8_t *>(&vTmpA[0]);
#ifdef LSI_ETC_ROWS
			for ( uint32_t ui32Index = 0; ui32Index < vRows.Length(); ++ui32Index ) {
				bool bFound = false;
				while ( !bFound ) {
					// Find a free thread.
					for ( uint32_t I = 0; I < LSE_ELEMENTS( tThreads ); ++I ) {
						if ( btdData[I].bDone ) {
							// Thread stopped or never started.
							CThreadLib::CloseThread( tThreads[I] );
						
							btdData[I].bDone = false;
							btdData[I].ptrRows = &vRows[ui32Index];
							btdData[I].pui8EncoderSrc = pui8EncodeSrcRgb;
							btdData[I].pui8EncoderSrcAlpha = pui8EncodeSrcA;
							btdData[I].pui8Dst = _pui8Dst;
							btdData[I].pui8ScratchDst = pui8ScratchDst;
							btdData[I].ui32BlockW = ui32BlockW;
							btdData[I].ui32ExtHeight = ui32ExtH;
							btdData[I].ui32ExtWidth = ui32ExtW;
							if ( !CThreadLib::CreateThread( Etc2EacThread, &btdData[I], &tThreads[I] ) ) {
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
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					LSI_ETC_EAC & eeDst = peeDst[Y*ui32BlockW+X];
					uint32_t ui32Block1, ui32Block2;
					::compressBlockETC2ExhaustivePerceptual( pui8EncodeSrcRgb, pui8ScratchDst,
						ui32ExtW, ui32ExtH, X << 2, Y << 2, ui32Block1, ui32Block2 );
					/*::compressBlockETC2Fast( pui8EncodeSrcRgb, pui8EncodeSrcA,
						pui8ScratchDst,
						ui32ExtW, ui32ExtH, X << 2, Y << 2, ui32Block1, ui32Block2 );*/
					eeDst.ui64Block2 = CStd::ByteSwap32( ui32Block1 ) | (static_cast<uint64_t>(CStd::ByteSwap32( ui32Block2 )) << 32ULL);
					::compressBlockAlphaSlow( pui8EncodeSrcA, X << 2, Y << 2, ui32ExtW, ui32ExtH, reinterpret_cast<uint8 *>(&eeDst.ui64Block1) );

				}
			}
#endif	// #ifdef LSI_ETC_ROWS
			peeDst += ui32BlockW * ui32BlockH;
			_pui8Src += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
		return true;
	}

	/**
	 * ETC2 A1 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CEtc::Etc2A1ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#if defined( LSI_USE_DETEX_FOR_ETC )
		struct LSI_ETC2A1_BLOCK {
			uint64_t ui64Block0;
		};
		const LSI_ETC2A1_BLOCK * pbbBlocks = reinterpret_cast<const LSI_ETC2A1_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_ETC2A1_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint8_t fPaletteRgbUi8[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockETC2_PUNCHTHROUGH( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]),
						DETEX_MODE_MASK_ALL_MODES_ETC2_PUNCHTHROUGH,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbUi8) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).fTexels[0] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_R], _bSrgb );
							(*prgbaRow0).fTexels[1] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_G], _bSrgb );
							(*prgbaRow0).fTexels[2] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_B], _bSrgb );
							(*prgbaRow0).fTexels[3] = CImageLib::Uint8ToUNorm( fPaletteRgbUi8[I*4+LSI_PC_A], false );
						}
					}
				}
			}
		}
#else 
		CVectorPoD<LSI_RGB8, uint32_t> vTmp;
		CVectorPoD<uint8_t, uint32_t> vAlphaTmp;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreatedExtendedRgb8( vTmp, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }
		if ( !CreatedExtendedA8( vAlphaTmp, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }

		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		
		const uint64_t * pui64Src = reinterpret_cast<const uint64_t *>(_pui8Src);
		uint8_t * pui8DecodeDstRgb = reinterpret_cast<uint8_t *>(&vTmp[0]);
		uint8_t * pui8DecodeDstA = reinterpret_cast<uint8_t *>(&vAlphaTmp[0]);
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					uint32_t ui32Block1 = CStd::ByteSwap32( static_cast<uint32_t>(pui64Src[Y*ui32BlockW+X]) );
					uint32_t ui32Block2 = CStd::ByteSwap32( static_cast<uint32_t>(pui64Src[Y*ui32BlockW+X] >> 32ULL) );
					::decompressBlockETC21BitAlphaC_( ui32Block1, ui32Block2, pui8DecodeDstRgb, pui8DecodeDstA, ui32ExtW, ui32ExtH, X << 2, Y << 2, 3 );
				}
			}
			CopyRgb8A8ToRgbaF32( vTmp, vAlphaTmp, reinterpret_cast<LSI_RGBAF32 *>(_pui8Dst), _ui32Width, _ui32Height, _bSrgb );
			pui64Src += ui32BlockW * ui32BlockH;
			_pui8Dst += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
#endif	// #if defined( LSI_USE_DETEX_FOR_ETC )
		return true;
	}

	/**
	 * Generic RGBA32F -> ETC2 A1 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb>
	bool LSE_CALL CEtc::Etc2A1FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		CVectorPoD<LSI_RGB8, uint32_t> vTmpRgb;
		CVectorPoD<uint8_t, uint32_t> vTmpA;
		CVectorPoD<LSI_RGBA8, uint32_t> vScratch;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreateExpandedRgba8( vScratch, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return true; }
		//::readCompressParams();
		format = ETC2PACKAGE_RGBA1_NO_MIPMAPS;
		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(_pui8Dst);
		uint8_t * pui8ScratchDst = reinterpret_cast<uint8_t *>(&vScratch[0]);
#ifdef LSI_ETC_ROWS
		CVectorPoD<LSI_THREAD_ROWS, uint32_t> vRows;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ETC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ETC2_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
		if ( !DivideByRows( vRows, ui32BlockH, LSI_ETC_ROWS ) ) { return false; }
#endif	// #ifdef LSI_ETC_ROWS
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			if ( !CreatedExtendedRgba8FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vTmpRgb, vTmpA, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH, _bSrgb ) ) { return false; }
			uint8_t * pui8EncodeSrcRgb = reinterpret_cast<uint8_t *>(&vTmpRgb[0]);
			uint8_t * pui8EncodeSrcA = reinterpret_cast<uint8_t *>(&vTmpA[0]);
#ifdef LSI_ETC_ROWS
			for ( uint32_t ui32Index = 0; ui32Index < vRows.Length(); ++ui32Index ) {
				bool bFound = false;
				while ( !bFound ) {
					// Find a free thread.
					for ( uint32_t I = 0; I < LSE_ELEMENTS( tThreads ); ++I ) {
						if ( btdData[I].bDone ) {
							// Thread stopped or never started.
							CThreadLib::CloseThread( tThreads[I] );
						
							btdData[I].bDone = false;
							btdData[I].ptrRows = &vRows[ui32Index];
							btdData[I].pui8EncoderSrc = pui8EncodeSrcRgb;
							btdData[I].pui8EncoderSrcAlpha = pui8EncodeSrcA;
							btdData[I].pui8Dst = _pui8Dst;
							btdData[I].pui8ScratchDst = pui8ScratchDst;
							btdData[I].ui32BlockW = ui32BlockW;
							btdData[I].ui32ExtHeight = ui32ExtH;
							btdData[I].ui32ExtWidth = ui32ExtW;
							if ( !CThreadLib::CreateThread( Etc2A1Thread, &btdData[I], &tThreads[I] ) ) {
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
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					uint32_t ui32Block1, ui32Block2;
					::compressBlockETC2Fast( pui8EncodeSrcRgb, pui8EncodeSrcA,
						pui8ScratchDst,
						ui32ExtW, ui32ExtH, X << 2, Y << 2, ui32Block1, ui32Block2 );
					uint64_t & ui64Dst = pui64Dst[Y*ui32BlockW+X];
					ui64Dst = CStd::ByteSwap32( ui32Block1 ) | (static_cast<uint64_t>(CStd::ByteSwap32( ui32Block2 )) << 32ULL);
				}
			}
#endif	// #ifdef LSI_ETC_ROWS
			pui64Dst += ui32BlockW * ui32BlockH;
			_pui8Src += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
		return true;
	}

	/**
	 * R11 EAC -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bSigned>
	bool LSE_CALL CEtc::R11EacToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#if defined( LSI_USE_DETEX_FOR_ETC )
		struct LSI_R11_BLOCK {
			uint64_t ui64Block0;
		};
		const LSI_R11_BLOCK * pbbBlocks = reinterpret_cast<const LSI_R11_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_R11_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint16_t fPaletteRgbUi16[16];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					if ( _bSigned ) {
						::detexDecompressBlockEAC_SIGNED_R11( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), 0,
							0, reinterpret_cast<uint8_t *>(fPaletteRgbUi16) );
					}
					else {
						::detexDecompressBlockEAC_R11( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), 0,
							0, reinterpret_cast<uint8_t *>(fPaletteRgbUi16) );
					}

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							if ( _bSigned ) {
								int16_t i16Tmp = fPaletteRgbUi16[I+LSI_PC_R];
								(*prgbaRow0).fTexels[0] = CImageLib::Int16ToSNorm( i16Tmp, _bSrgb );
							}
							else {
								(*prgbaRow0).fTexels[0] = CImageLib::Uint16ToUNorm( fPaletteRgbUi16[I+LSI_PC_R], _bSrgb );
							}
							(*prgbaRow0).fTexels[1] = 0.0f;
							(*prgbaRow0).fTexels[2] = 0.0f;
							(*prgbaRow0).fTexels[3] = 1.0f;
						}
					}
				}
			}
		}
#else 
		CVectorPoD<uint16_t, uint32_t> vTmpA;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreatedExtendedA11( vTmpA, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return false; }

		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
		};
		formatSigned_ = _bSigned;
		
		const LSI_ETC_EAC * peeSrc = reinterpret_cast<const LSI_ETC_EAC *>(_pui8Src);
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					::decompressBlockAlpha16bit_( const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&peeSrc[Y*ui32BlockW+X].ui64Block1)),
						reinterpret_cast<uint8 *>(&vTmpA[0]),
						ui32ExtW, ui32ExtH,
						X << 2, Y << 2 );
				}
			}
			CopyR16G16ToRgbaF32( vTmpA, nullptr, reinterpret_cast<LSI_RGBAF32 *>(_pui8Dst), _ui32Width, _ui32Height, _bSrgb, _bSigned );
			peeSrc += ui32BlockW * ui32BlockH;
			_pui8Dst += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
#endif	// #if defined( LSI_USE_DETEX_FOR_ETC )
		return true;
	}

	/**
	 * Generic RGBA32F -> R11 EAC conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bSigned>
	bool LSE_CALL CEtc::R11EacFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		CVectorPoD<uint16_t, uint32_t> vTmpA;
		uint32_t ui32ExtW = LSE_ROUND_UP( _ui32Width, 4 ), ui32ExtH = LSE_ROUND_UP( _ui32Height, 4 );
		
		::readCompressParams();
		::setupAlphaTableAndValtab();
		format = ETC2PACKAGE_R_NO_MIPMAPS;
		formatSigned_ = _bSigned;
		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
		};
		LSI_ETC_EAC * peeDst = reinterpret_cast<LSI_ETC_EAC *>(_pui8Dst);
#ifdef LSI_ETC_ROWS
		CVectorPoD<LSI_THREAD_ROWS, uint32_t> vRows;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ETC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ETC2_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
		if ( !DivideByRows( vRows, ui32BlockH, LSI_ETC_ROWS ) ) { return false; }
#endif	// #ifdef LSI_ETC_ROWS
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			if ( !CreatedExtendedA11FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vTmpA, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH, _bSrgb, _bSigned ) ) { return false; }
			uint8_t * pui8EncodeSrcA = reinterpret_cast<uint8_t *>(&vTmpA[0]);
#ifdef LSI_ETC_ROWS
			for ( uint32_t ui32Index = 0; ui32Index < vRows.Length(); ++ui32Index ) {
				bool bFound = false;
				while ( !bFound ) {
					// Find a free thread.
					for ( uint32_t I = 0; I < LSE_ELEMENTS( tThreads ); ++I ) {
						if ( btdData[I].bDone ) {
							// Thread stopped or never started.
							CThreadLib::CloseThread( tThreads[I] );
						
							btdData[I].bDone = false;
							btdData[I].ptrRows = &vRows[ui32Index];
							btdData[I].pui8EncoderSrcAlpha = pui8EncodeSrcA;
							btdData[I].pui8Dst = _pui8Dst;
							btdData[I].ui32BlockW = ui32BlockW;
							btdData[I].ui32ExtHeight = ui32ExtH;
							btdData[I].ui32ExtWidth = ui32ExtW;
							if ( !CThreadLib::CreateThread( Etc2R11Thread, &btdData[I], &tThreads[I] ) ) {
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
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					LSI_ETC_EAC & eeDst = peeDst[Y*ui32BlockW+X];
					::compressBlockAlpha16( pui8EncodeSrcA,
						X << 2, Y << 2,
						ui32ExtW, ui32ExtH, reinterpret_cast<uint8 *>(&eeDst.ui64Block1) );

				}
			}
#endif	// #ifdef LSI_ETC_ROWS
			peeDst += ui32BlockW * ui32BlockH;
			_pui8Src += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
		return true;
	}

	/**
	 * RG11 EAC -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bSigned>
	bool LSE_CALL CEtc::RG11EacToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#if defined( LSI_USE_DETEX_FOR_ETC )
		struct LSI_R11_BLOCK {
			uint64_t ui64Block0;
			uint64_t ui64Block1;
		};
		const LSI_R11_BLOCK * pbbBlocks = reinterpret_cast<const LSI_R11_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( LSI_R11_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		LSI_RGBA32 * prgbaTexels = reinterpret_cast<LSI_RGBA32 *>(_pui8Dst);
		uint16_t fPaletteRgbUi16[16*2];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					if ( _bSigned ) {
						::detexDecompressBlockEAC_SIGNED_RG11( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), 0,
							0, reinterpret_cast<uint8_t *>(fPaletteRgbUi16) );
					}
					else {
						::detexDecompressBlockEAC_RG11( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), 0,
							0, reinterpret_cast<uint8_t *>(fPaletteRgbUi16) );
					}

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							LSI_RGBA32 * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							if ( _bSigned ) {
								int16_t i16Tmp = fPaletteRgbUi16[I*2+LSI_PC_R];
								(*prgbaRow0).fTexels[0] = CImageLib::Int16ToSNorm( i16Tmp, _bSrgb );
								i16Tmp = fPaletteRgbUi16[I*2+LSI_PC_G];
								(*prgbaRow0).fTexels[1] = CImageLib::Int16ToSNorm( i16Tmp, _bSrgb );
							}
							else {
								(*prgbaRow0).fTexels[0] = CImageLib::Uint16ToUNorm( fPaletteRgbUi16[I*2+LSI_PC_R], _bSrgb );
								(*prgbaRow0).fTexels[1] = CImageLib::Uint16ToUNorm( fPaletteRgbUi16[I*2+LSI_PC_G], _bSrgb );
							}
							(*prgbaRow0).fTexels[2] = 0.0f;
							(*prgbaRow0).fTexels[3] = 1.0f;
						}
					}
				}
			}
		}
#else 
		CVectorPoD<uint16_t, uint32_t> vTmpA;
		CVectorPoD<uint16_t, uint32_t> vTmpG;
		uint32_t ui32ExtW, ui32ExtH;
		if ( !CreatedExtendedA11( vTmpA, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return; }
		if ( !CreatedExtendedA11( vTmpG, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH ) ) { return; }

		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
			uint64_t ui64Block2;
		};
		formatSigned_ = _bSigned;
		
		const LSI_ETC_EAC * peeSrc = reinterpret_cast<const LSI_ETC_EAC *>(_pui8Src);
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					::decompressBlockAlpha16bit_( const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&peeSrc[Y*ui32BlockW+X].ui64Block1)),
						reinterpret_cast<uint8 *>(&vTmpA[0]),
						ui32ExtW, ui32ExtH,
						X << 2, Y << 2 );
					::decompressBlockAlpha16bit_( const_cast<uint8 *>(reinterpret_cast<const uint8 *>(&peeSrc[Y*ui32BlockW+X].ui64Block2)),
						reinterpret_cast<uint8 *>(&vTmpG[0]),
						ui32ExtW, ui32ExtH,
						X << 2, Y << 2 );
				}
			}
			CopyR16G16ToRgbaF32( vTmpA, &vTmpG, reinterpret_cast<LSI_RGBAF32 *>(_pui8Dst), _ui32Width, _ui32Height, _bSrgb, _bSigned );
			peeSrc += ui32BlockW * ui32BlockH;
			_pui8Dst += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
#endif	// #if defined( LSI_USE_DETEX_FOR_ETC )
		return true;
	}

	/**
	 * Generic RGBA32F -> RG11 EAC conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _bSrgb, unsigned _bSigned>
	bool LSE_CALL CEtc::RG11EacFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		CVectorPoD<uint16_t, uint32_t> vTmpR;
		CVectorPoD<uint16_t, uint32_t> vTmpG;
		uint32_t ui32ExtW = LSE_ROUND_UP( _ui32Width, 4 ), ui32ExtH = LSE_ROUND_UP( _ui32Height, 4 );
		
		::readCompressParams();
		::setupAlphaTableAndValtab();
		format = ETC2PACKAGE_R_NO_MIPMAPS;
		formatSigned_ = _bSigned;
		uint32_t ui32BlockW = ui32ExtW >> 2, ui32BlockH = ui32ExtH >> 2;
		struct LSI_ETC_EAC {
			uint64_t ui64Block1;
			uint64_t ui64Block2;
		};
		LSI_ETC_EAC * peeDst = reinterpret_cast<LSI_ETC_EAC *>(_pui8Dst);
#ifdef LSI_ETC_ROWS
		CVectorPoD<LSI_THREAD_ROWS, uint32_t> vRows;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ETC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ETC2_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
		if ( !DivideByRows( vRows, ui32BlockH, LSI_ETC_ROWS ) ) { return false; }
#endif	// #ifdef LSI_ETC_ROWS
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			if ( !CreatedExtendedA11FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vTmpR, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH, _bSrgb, _bSigned ) ) { return false; }
			uint8_t * pui8EncodeSrcR = reinterpret_cast<uint8_t *>(&vTmpR[0]);
			if ( !CreatedExtendedA11FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src + sizeof( float )), vTmpG, _ui32Width, _ui32Height, ui32ExtW, ui32ExtH, _bSrgb, _bSigned ) ) { return false; }
			uint8_t * pui8EncodeSrcG = reinterpret_cast<uint8_t *>(&vTmpG[0]);
#ifdef LSI_ETC_ROWS
			for ( uint32_t ui32Index = 0; ui32Index < vRows.Length(); ++ui32Index ) {
				bool bFound = false;
				while ( !bFound ) {
					// Find a free thread.
					for ( uint32_t I = 0; I < LSE_ELEMENTS( tThreads ); ++I ) {
						if ( btdData[I].bDone ) {
							// Thread stopped or never started.
							CThreadLib::CloseThread( tThreads[I] );
						
							btdData[I].bDone = false;
							btdData[I].ptrRows = &vRows[ui32Index];
							btdData[I].pui8EncoderSrc = pui8EncodeSrcR;
							btdData[I].pui8EncoderSrcAlpha = pui8EncodeSrcG;
							btdData[I].pui8Dst = _pui8Dst;
							btdData[I].ui32BlockW = ui32BlockW;
							btdData[I].ui32ExtHeight = ui32ExtH;
							btdData[I].ui32ExtWidth = ui32ExtW;
							if ( !CThreadLib::CreateThread( Etc2RG11Thread, &btdData[I], &tThreads[I] ) ) {
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
			for ( uint32_t Y = 0; Y < ui32BlockH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlockW; ++X ) {
					LSI_ETC_EAC & eeDst = peeDst[Y*ui32BlockW+X];
					::compressBlockAlpha16( pui8EncodeSrcR,
						X << 2, Y << 2,
						ui32ExtW, ui32ExtH, reinterpret_cast<uint8 *>(&eeDst.ui64Block1) );
					::compressBlockAlpha16( pui8EncodeSrcG,
						X << 2, Y << 2,
						ui32ExtW, ui32ExtH, reinterpret_cast<uint8 *>(&eeDst.ui64Block2) );
				}
			}
#endif	// #ifdef LSI_ETC_ROWS
			peeDst += ui32BlockW * ui32BlockH;
			_pui8Src += sizeof( LSI_RGBAF32 ) * _ui32Width * _ui32Height;
		}
		return true;
	}

}	// namespace lsi

#endif	// __LSI_ETC_H__
