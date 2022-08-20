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


#ifndef __LSI_ASTC_H__
#define __LSI_ASTC_H__

#include "../LSIImageLib.h"
#include "../astc-encoder/astc_codec_internals.h"
#ifndef LSE_IPHONE
#include "../Ispc/ispc_texcomp.h"
#endif	// #ifndef LSE_IPHONE

/** How many rows to do per-thread for ASTC conversions. */
#define LSI_ASTC_ROWS						24

/** Maximum number of threads to run concurrently for ASTC conversions. */
#define LSI_ASTC_MAX_THREADS				16

extern void encode_astc_image( const astc_codec_image * input_image,
	astc_codec_image * output_image,
	int xdim,
	int ydim,
	int zdim,
	const error_weighting_params * ewp, astc_decode_mode decode_mode, swizzlepattern swz_encode, swizzlepattern swz_decode, uint8_t * buffer, int pack_and_unpack, int threadcount );


namespace lsi {

	/**
	 * Class CAstc
	 * \brief Handles ASTC formats.
	 *
	 * Description: Handles ASTC formats.
	 */
	class CAstc {
	public :
		// == Types.
		/** An RGBAF32 color component. */
		typedef struct LSI_RGBAF32 {
			float							fRgba[4];
		} * LPLSI_RGBAF32, * const LPCLSI_RGBAF32;

		/** An RGBA8 color component. */
		typedef struct LSI_RGBA8 {
			uint8_t							ui8Rgba[4];
		} * LPLSI_RGBA8, * const LPCLSI_RGBA8;


		// == Functions.
		/**
		 * Gets the size of blocks given an ASTC pixel format.
		 *
		 * \param _pfFormat The format for which to retrieve the block dimensions.
		 * \param _ui32X Holds the X block dimensions.
		 * \param _ui32Y Holds the Y block dimensions.
		 */
		static void LSE_CALL				GetBlockSizes( LSI_PIXEL_FORMAT _pfFormat, uint32_t &_ui32X, uint32_t &_ui32Y );

		/**
		 * Returns the size, in pixels, of a block.
		 *
		 * \param _pfFormat The ASTC format whose block size is to be obtained.
		 * \return Returns the size, in pixels, of a block of ASTC.
		 */
		static uint32_t LSE_CALL			AstcBlockSize( LSI_PIXEL_FORMAT /*_pfFormat*/ ) { return AstcBlockSize(); }

		/**
		 * Returns the size, in pixels, of a block.
		 *
		 * \return Returns the size, in pixels, of a block of ASTC.
		 */
		static uint32_t LSE_CALL			AstcBlockSize() { return 16; }

		/**
		 * Gets the pitch of an ASTC format.
		 *
		 * \param _ui32Width The width of the image.
		 * \param _pfFormat The format of the image.
		 * \return Returns the width, in bytes, of a scanline in the given ASTC format.
		 */
		static uint32_t LSE_CALL			AstcScanlineWidth( uint32_t _ui32Width, LSI_PIXEL_FORMAT _pfFormat ) {
			uint32_t ui32X, ui32Y;
			GetBlockSizes( _pfFormat, ui32X, ui32Y );
			return ((_ui32Width + (ui32X - 1)) / ui32X) * AstcBlockSize( _pfFormat );
		}

		/**
		 * Gets the total number of scanlines in a given ASTC format.
		 *
		 * \param _ui32Height Height of the image.
		 * \return Returns the number of scanlines in the given ASTC format.
		 */
		static uint32_t LSE_CALL			AstcTotalScanlines( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Height );

		/**
		 * Returns the total size of a compressed image given a factor and its width and height.
		 *
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _pfFormat The format.
		 * \return Returns the size of the compressed data.
		 */
		static uint32_t LSE_CALL			GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Returns the total size of a compressed image given its block sizes and its width and height.
		 *
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _ui32BlockW Width of blocks.
		 * \param _ui32BlockH Height of blocks.
		 * \return Returns the size of the compressed data.
		 */
		static uint32_t LSE_CALL			GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32BlockW, uint32_t _ui32BlockH );

		/**
		 * ASTC -> RGBA32F conversion.
		 *
		 * \param _pui8Src Source texels.
		 * \param _pui8Dst Destination texels known to be in RGBA32F format.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _pfFormat, unsigned _uiBlockW, unsigned _uiBlockH, unsigned _bSrgb, unsigned _uiBlockD = 1>
		static bool LSE_CALL				AstcToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> ASTC conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _pfFormat, unsigned _uiBlockW, unsigned _uiBlockH, unsigned _bSrgb>
		static bool LSE_CALL				AstcFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );

		/**
		 * Generic RGBA32F -> ASTC (above 8x8) conversion.
		 *
		 * \param _pui8Src Source texels known to be in RGBA32F format.
		 * \param _pui8Dst Destination texels.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Depth Depth of the image.
		 * \param _pvParms Optional parameters for the conversion.
		 */
		template <unsigned _pfFormat, unsigned _uiBlockW, unsigned _uiBlockH, unsigned _bSrgb, unsigned _uiBlockD = 1>
		static bool LSE_CALL				Astc2FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * _pvParms = nullptr );


	protected :
		// == Types.
		/** Surface and destination pair. */
		typedef struct LSI_SURFACE_AND_DST {
			/** The surface. */
			rgba_surface					sSurface;

			/** The destination. */
			uint8_t *						pui8Dst;
		} * LPLSI_SURFACE_AND_DST, * const LPCLSI_SURFACE_AND_DST;

		/** Thread data for ASTC conversions. */
		typedef struct LSI_ASTC_THREAD_DATA {
			/** The settings. */
			astc_enc_settings *				paesSettings;

			/** The surface. */
			const rgba_surface *			psSurface;

			/** The destination. */
			uint8_t *						pui8Dst;

			/** Indicates the thread has finished. */
			LSBOOL							bDone;
		} * LPLSI_ASTC_THREAD_DATA, * const LPCLSI_ASTC_THREAD_DATA;


		// == Functions.
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
		static bool LSE_CALL				ExpandImageRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBAF32, uint32_t> &_vDst, uint32_t &_ui32Width, uint32_t &_ui32Height, bool _bSrgb );

		/**
		 * Thread function for converting a block of texels to the ASTC format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_ASTC_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL			AstcThread( void * _lpParameter );

		/**
		 * Allocates an RGBA8 buffer to act as an intermediate storage for decoding ASTC texture before converting to RGBAF32.
		 *
		 * \param _pvDst The destination for the converted texels, which will be allocated.
		 * \param _ui32Width The width of the image data, in texels.
		 * \param _ui32Height The height of the image data, in texels.
		 * \return Returns true if there is no issue allocating the new image memory.
		 */
		static bool LSE_CALL				CreateExpandedRgba8( CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height );

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
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, LSI_PIXEL_FORMAT _pfFormat );

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
		static bool LSE_CALL				CreateExpandedRgba8FromRgbaF32( const LSI_RGBAF32 * _prSrc, CVectorPoD<LSI_RGBA8, uint32_t> &_pvDst,
			uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t &_ui32ExpandedWidth, uint32_t &_ui32ExpandedHeight, bool _bSrgb, LSI_PIXEL_FORMAT _pfFormat );

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
		static void LSE_CALL				CopyFloat16ToFloat32( uint16_t *** _pppui16Src, LSI_RGBAF32 * _prDst, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, bool _bSrgb );

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
		static void LSE_CALL				CopyFloat32ToFloat16( uint16_t *** _pppui16Dst, const LSI_RGBAF32 * _prSrc, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, bool _bSrgb );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * ASTC -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _pfFormat, unsigned _uiBlockW, unsigned _uiBlockH, unsigned _bSrgb, unsigned _uiBlockD>
	bool LSE_CALL CAstc::AstcToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		uint32_t ui32BlocksW = (_ui32Width + (_uiBlockW - 1)) / _uiBlockW;
		uint32_t ui32BlocksH = (_ui32Height + (_uiBlockH - 1)) / _uiBlockH;
		uint32_t ui32BlocksD = _uiBlockD == 1 ? _uiBlockD : (_ui32Depth + (_uiBlockD - 1)) / _uiBlockD;
		CVectorPoD<LSI_RGBA8, uint32_t> vExpanded;
		::prepare_angular_tables();
		::build_quantization_mode_table();
		swizzlepattern spSwizzle = { 0, 1, 2, 3 };
		astc_codec_image * paciImage = ::allocate_image( 16, _ui32Width, _ui32Height, _ui32Depth, 0 );
		::initialize_image( paciImage );
		imageblock ibImBlock;
		uint8_t * pui8OriginalDst = _pui8Dst;
		for ( uint32_t Z = 0; Z < ui32BlocksD; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					int iOffset = (((Z * ui32BlocksH + Y) * ui32BlocksW) + X) * 16;
					const uint8_t * pui8Block = _pui8Src + iOffset;
					physical_compressed_block pcbCompBlock = (*reinterpret_cast<const physical_compressed_block *>(pui8Block));
					symbolic_compressed_block scbSymbBlock;
					::physical_to_symbolic( _uiBlockW, _uiBlockH, _uiBlockD, pcbCompBlock, &scbSymbBlock );
					::decompress_symbolic_block( DECODE_HDR, _uiBlockW, _uiBlockH, _uiBlockD, X * _uiBlockW, Y * _uiBlockH, Z * 1, &scbSymbBlock, &ibImBlock );
					::write_imageblock( paciImage, &ibImBlock, _uiBlockW, _uiBlockH, _uiBlockD, X * _uiBlockW, Y * _uiBlockH, Z * 1, spSwizzle );
				}
			}
		}
		CopyFloat16ToFloat32( paciImage->imagedata16, reinterpret_cast<LSI_RGBAF32 *>(pui8OriginalDst), _ui32Width, _ui32Height, _ui32Depth, _bSrgb );
		::destroy_image( paciImage );
		return true;
	}

	/**
	 * Generic RGBA32F -> ASTC conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _pfFormat, unsigned _uiBlockW, unsigned _uiBlockH, unsigned _bSrgb>
	bool LSE_CALL CAstc::AstcFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
#ifndef LSE_IPHONE
		uint32_t ui32SliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;
		uint32_t ui32DstSliceSize = CAstc::GetCompressedSize( _ui32Width, _ui32Height, _uiBlockW, _uiBlockH );

#ifdef LSI_ASTC_ROWS
		CVectorPoD<LSI_SURFACE_AND_DST, uint32_t> vSurfaces;
		uint8_t * pui8Surf;

		// Create X number of threads as we go along the blocks.
		LSH_THREAD tThreads[LSI_ASTC_MAX_THREADS];
		for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
			CThreadLib::MakeInvalidHandle( tThreads[I] );
		}
		LSI_ASTC_THREAD_DATA btdData[LSE_ELEMENTS( tThreads )];
		for ( uint32_t I = LSE_ELEMENTS( btdData ); I--; ) {
			btdData[I].bDone = true;
		}
#else
		rgba_surface sSurface;
		sSurface.width = LSE_ROUND_UP_ANY( _ui32Width, _uiBlockW );
		sSurface.height = LSE_ROUND_UP_ANY( _ui32Height, _uiBlockH );
		sSurface.stride = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, sSurface.width );
#endif	// #ifdef LSI_ASTC_ROWS
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))

		astc_enc_settings aesSettings;
		::GetProfile_astc_alpha_slow( &aesSettings, _uiBlockW, _uiBlockH );

		CVectorPoD<LSI_RGBA8, uint32_t> vExpanded;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
#ifdef LSI_ASTC_ROWS
			pui8Surf = LSI_CAST( _pui8Src );
#else
			sSurface.ptr = LSI_CAST( _pui8Src );
#endif	// ##ifdef LSI_ASTC_ROWS

			uint32_t ui32W = _ui32Width;
			uint32_t ui32H = _ui32Height;
			if ( !CreateExpandedRgba8FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vExpanded, _ui32Width, _ui32Height, ui32W, ui32H,
				_bSrgb, static_cast<LSI_PIXEL_FORMAT>(_pfFormat) ) ) { return false; }
#ifdef LSI_ASTC_ROWS
			pui8Surf = LSI_CAST( &vExpanded[0] );
#else
			sSurface.ptr = LSI_CAST( &vExpanded[0] );
#endif	// ##ifdef LSI_ASTC_ROWS

#ifdef LSI_ASTC_ROWS
			if ( !DivideSurface( vSurfaces, pui8Surf, _pui8Dst, ui32W, ui32H, LSE_ROUND_UP_ANY( LSI_ASTC_ROWS, _uiBlockH ), static_cast<LSI_PIXEL_FORMAT>(_pfFormat) ) ) { return false; }

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
							btdData[I].paesSettings = &aesSettings;
							if ( !CThreadLib::CreateThread( AstcThread, &btdData[I], &tThreads[I] ) ) {
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
			CompressBlocksASTC( &sSurface, _pui8Dst, &aesSettings );
#endif	// #ifdef LSI_ASTC_ROWS

			_pui8Src += ui32SliceSize;
			_pui8Dst += ui32DstSliceSize;
		}
#undef LSI_CAST
#endif	// #ifndef LSE_IPHONE
		return true;
	}

	/**
	 * Generic RGBA32F -> ASTC (above 8x8) conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	template <unsigned _pfFormat, unsigned _uiBlockW, unsigned _uiBlockH, unsigned _bSrgb, unsigned _uiBlockD>
	bool LSE_CALL CAstc::Astc2FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		::prepare_angular_tables();
		::build_quantization_mode_table();

		swizzlepattern spSwizzle = { 0, 1, 2, 3 };
		astc_codec_image * paciImage = ::allocate_image( 16, _ui32Width, _ui32Height, _ui32Depth, 0 );
		::initialize_image( paciImage );

		CopyFloat32ToFloat16( paciImage->imagedata16, reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src),
			_ui32Width, _ui32Height, _ui32Depth, _bSrgb );

		error_weighting_params ewpWeightParms = { 0 };
		ewpWeightParms.rgb_power = 1.0f;
		ewpWeightParms.alpha_power = 1.0f;
		ewpWeightParms.rgb_base_weight = 1.0f;
		ewpWeightParms.alpha_base_weight = 1.0f;
		ewpWeightParms.rgb_mean_weight = 0.0f;
		ewpWeightParms.rgb_stdev_weight = 0.0f;
		ewpWeightParms.alpha_mean_weight = 0.0f;
		ewpWeightParms.alpha_stdev_weight = 0.0f;

		ewpWeightParms.rgb_mean_and_stdev_mixing = 0.0f;
		ewpWeightParms.mean_stdev_radius = 0;
		ewpWeightParms.enable_rgb_scale_with_alpha = 0;
		ewpWeightParms.alpha_radius = 0;

		ewpWeightParms.block_artifact_suppression = 0.0f;
		ewpWeightParms.rgba_weights[0] = 1.0f;
		ewpWeightParms.rgba_weights[1] = 1.0f;
		ewpWeightParms.rgba_weights[2] = 1.0f;
		ewpWeightParms.rgba_weights[3] = 1.0f;
		ewpWeightParms.ra_normal_angular_scale = 0;

		ewpWeightParms.max_refinement_iters = 4;
		ewpWeightParms.block_mode_cutoff = 100.0f / 100.0f;
		ewpWeightParms.partition_1_to_2_limit = 1000.0f;
		ewpWeightParms.lowest_correlation_cutoff = 0.99f;
		ewpWeightParms.partition_search_limit = PARTITION_COUNT;
		float fMaxColorCompWeight = MAX( MAX( ewpWeightParms.rgba_weights[0], ewpWeightParms.rgba_weights[1] ),
			MAX( ewpWeightParms.rgba_weights[2], ewpWeightParms.rgba_weights[3] ) );
		ewpWeightParms.rgba_weights[0] = MAX( ewpWeightParms.rgba_weights[0], fMaxColorCompWeight / 1000.0f );
		ewpWeightParms.rgba_weights[1] = MAX( ewpWeightParms.rgba_weights[1], fMaxColorCompWeight / 1000.0f );
		ewpWeightParms.rgba_weights[2] = MAX( ewpWeightParms.rgba_weights[2], fMaxColorCompWeight / 1000.0f );
		ewpWeightParms.rgba_weights[3] = MAX( ewpWeightParms.rgba_weights[3], fMaxColorCompWeight / 1000.0f );
		ewpWeightParms.texel_avg_error_limit = 999;

		::expand_block_artifact_suppression( _uiBlockW, _uiBlockH, _ui32Depth, &ewpWeightParms );
		if ( ewpWeightParms.rgb_mean_weight != 0.0f || ewpWeightParms.rgb_stdev_weight != 0.0f || ewpWeightParms.alpha_mean_weight != 0.0f || ewpWeightParms.alpha_stdev_weight != 0.0f ) {
			::compute_averages_and_variances( paciImage, ewpWeightParms.rgb_power, ewpWeightParms.alpha_power, ewpWeightParms.mean_stdev_radius, ewpWeightParms.alpha_radius, spSwizzle );
		}

		::encode_astc_image( paciImage, nullptr, _uiBlockW, _uiBlockH, _uiBlockD, &ewpWeightParms, DECODE_HDR,
			spSwizzle, spSwizzle, _pui8Dst, false, 8 );

		::destroy_image( paciImage );
#if 0
		uint32_t ui32SliceSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, _ui32Width ) * _ui32Height;
		uint32_t ui32DstSliceSize = CAstc::GetCompressedSize( _ui32Width, _ui32Height, _uiBlockW, _uiBlockH );
		uint32_t ui32BlocksW = (_ui32Width + (_uiBlockW - 1)) / _uiBlockW;
		uint32_t ui32BlocksH = (_ui32Height + (_uiBlockH - 1)) / _uiBlockH;
		struct LSI_RGBA32 {
			float fTexels[4];
		};
		CVectorPoD<LSI_RGBA8, uint32_t> vExpanded;
		swizzlepattern swz_decode = { 0, 1, 2, 3 };
		astc_codec_image *img = ::allocate_image( 8, _ui32Width, _ui32Height, _ui32Depth, 0 );
		::initialize_image( img );
		imageblock pb;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			uint32_t ui32W = _ui32Width;
			uint32_t ui32H = _ui32Height;
			if ( !CreateExpandedRgba8FromRgbaF32( reinterpret_cast<const LSI_RGBAF32 *>(_pui8Src), vExpanded, _ui32Width, _ui32Height, ui32W, ui32H,
				_bSrgb, static_cast<LSI_PIXEL_FORMAT>(_pfFormat) ) ) { return false; }
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					/*int offset = (((Z * ui32BlocksH + Y) * ui32BlocksW) + X) * 16;
					uint8_t *bp = _pui8Src + offset;
					::fetch_imageblock(input_image, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_encode);
					symbolic_compressed_block scb;
					::compress_symbolic_block(input_image, decode_mode, xdim, ydim, zdim, ewpWeightParms, &pb, &scb);*/
					/*
					int offset = (((Z * ui32BlocksH + Y) * ui32BlocksW) + X) * 16;
					uint8_t *bp = _pui8Src + offset;
					physical_compressed_block pcb = *(physical_compressed_block *) bp;
					symbolic_compressed_block scb;
					::physical_to_symbolic(_ui32Width, _ui32Height, _ui32Depth, pcb, &scb);
					::decompress_symbolic_block(decode_mode, _ui32Width, _ui32Height, _ui32Depth, X * _uiBlockW, Y * _uiBlockH, Z * 1, &scb, &pb);
					::write_imageblock(img, &pb, _ui32Width, _ui32Height, _ui32Depth, X * _uiBlockW, Y * _uiBlockH, Z * 1, swz_decode);
					*/
				}
			}
		}
		::destroy_image( img );
#endif //
		return true;
	}

}	// namespace lsi

#endif	// __LSI_ASTC_H__