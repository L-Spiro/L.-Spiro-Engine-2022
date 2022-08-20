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
 * Description: The main class that holds loaded image information.
 */


#ifndef __LSI_IMAGE_H__
#define __LSI_IMAGE_H__

#include "../LSIImageLib.h"
#include "../Dds/LSIDds.h"
#include "../Resampler/LSIResampler.h"
#include "Map/LSTLLinearMap.h"
#include "Map/LSTLStaticMap.h"
#include "Misc/LSCMisc.h"
#include "Vector/LSTLVectorPoD.h"
#include "Vector/LSTLVector.h"

namespace lsi {

	// == Enumerations.
	/** Resample filters. */
	enum LSI_FILTER {
		LSI_F_BOX_FILTER,							/**< A box filter. */
		LSI_F_BILINEAR_FILTER,						/**< A bilinear (tent) filter.  Good for scaling up images that are intended to be rescaled back down. */
		LSI_F_QUADRATIC_FILTER,						/**< A quadratic filter. */
		LSI_F_KAISER_FILTER,						/**< A Kaiser filter.  Good for scaling down images.  Best for mipmaps. */
		LSI_F_LANCZOS3_FILTER,						/**< A Lanczos filter with 3 sampling points. */
		LSI_F_LANCZOS4_FILTER,						/**< A Lanczos filter with 4 sampling points. */
		LSI_F_LANCZOS6_FILTER,						/**< A Lanczos filter with 6 sampling points. */
		LSI_F_LANCZOS8_FILTER,						/**< A Lanczos filter with 8 sampling points. */
		LSI_F_LANCZOS12_FILTER,						/**< A Lanczos filter with 12 sampling points. */
		LSI_F_LANCZOS64_FILTER,						/**< A Lanczos filter with 64 sampling points. */
		LSI_F_MITCHELL_FILTER,						/**< A Mitchell filter.  The best filter for scaling up, unless the image is intended to be rescaled back down to its original size. */
		LSI_F_CATMULLROM_FILTER,					/**< A Catmull-Rom filter. */
		LSI_F_BSPLINE_FILTER,						/**< A B-Spline filter. */
		LSI_F_BLACKMAN_FILTER,						/**< A Blackman filter. */
		LSI_F_GAUSSIAN_FILTER,						/**< A Gaussian filter. */
		LSI_F_BELL_FILTER,							/**< A Bell filter. */
		LSI_F_ANY,									/**< Any filter. */
	};

	/** Gamma encoding. */
	enum LSI_GAMMA {
		LSI_G_sRGB,									/**< Image is in sRGB color space. */
		LSI_G_LINEAR,								/**< Image is in linear color space. */
	};

	/**
	 * Class CImage
	 * \brief The main class that holds loaded image information.
	 *
	 * Description: The main class that holds loaded image information.
	 */
	class CImage {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CImage();
		LSE_CALLCTOR								CImage( const CImage &_iOther );


		// == Enumerations.
		/**
		 * Compression types.
		 */
		enum LSI_COMPRESSIONS {
			LSI_C_RAWCOMPRESSION,					/**< The texel data is simply compressed without being rearranged. */
			LSI_C_CHANNELCOMPRESSION,				/**< Channels are stripped and compressed together. */
			LSI_C_RLECOMPRESSION,					/**< Per-channel RLE with compression. */
			LSI_C_INDEXCOMPRESSION,					/**< Indexed data with compression. */
			LSI_C_DTXCOMPRESSION,					/**< DTX compression. */
		};


		// == Types.
		/** A table entry for single-color DXT values. */
		typedef struct LSI_DXT_TABLE_4 {
			/** The 4 colors. */
			LSFLOAT									fColors[4];
		} * LPLSI_DXT_TABLE_4, * const LPCLSI_DXT_TABLE_4;

		/** Options for creating DXT file images. */
		typedef struct LSI_DXT_OPTIONS {
			/** Alpha cut-off for DXT1 images. */
			LSFLOAT									fAlphaThresh;

			/** Red weight. */
			LSFLOAT									fRedWeight;

			/** Green weight. */
			LSFLOAT									fGreenWeight;

			/** Blue weight. */
			LSFLOAT									fBlueWeight;

			/** If true, DXT1 ignores alpha. */
			LSBOOL									bIgnoreDxt1Alpha;

			/** The number of second-stage iterations.  0 = low quality, higher = more quality. */
			LSUINT32								ui32SecondStageIterations;

			/** The number of 3rd-stage iterations. */
			LSUINT32								ui32ThirdStageIterations[3];
		} * LPLSI_DXT_OPTIONS, * const LPCLSI_DXT_OPTIONS;


		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _iOther The object to copy.
		 * \return Returns the copied object.
		 */
		CImage & LSE_CALL							operator = ( const CImage &_iOther );


		// == Functions.
		/**
		 * Sets the allocator.  Causes the object to be fully reset.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator.
		 */
		LSVOID LSE_CALL								SetAllocator( CAllocator * _paAllocator );

		/**
		 * Reset the image entirely, leaving it ready to be used again for a new image.
		 */
		LSVOID LSE_CALL								Reset();

		/**
		 * Gets the image width.
		 *
		 * \return Returns the image width.
		 */
		LSE_INLINE LSUINT32 LSE_CALL				GetWidth() const;

		/**
		 * Gets the image height.
		 *
		 * \return Returns the image height.
		 */
		LSE_INLINE LSUINT32 LSE_CALL				GetHeight() const;

		/**
		 * Gets the texel format.
		 *
		 * \return Returns the texel format.
		 */
		LSE_INLINE LSI_PIXEL_FORMAT LSE_CALL		GetFormat() const;

		/**
		 * Gets a read-only pointer to the raw buffer of texel data.
		 *
		 * \return Returns a read-only pointer to the raw buffer of texel data.
		 */
		LSE_INLINE const LSVOID * LSE_CALL			GetBufferData() const;

		/**
		 * Gets a read-only reference to the raw buffer of texel data as a vector.
		 *
		 * \return Returns a read-only reference to the raw buffer of texel data as a vector.
		 */
		LSE_INLINE const CImageLib::CTexelBuffer & LSE_CALL
													GetBufferDataAsVector() const;

		/**
		 * Gets the mipmaps of this image, if any.
		 *
		 * \return Returns the optional mipmap chain in this image.
		 */
		LSE_INLINE const CVector<CImageLib::CTexelBuffer> & LSE_CALL
													GetMipMapBuffers() const;

		/**
		 * Gets the total mipmap levels including the base level.
		 *
		 * \return Returns the total mipmap levels including the base level.
		 */
		LSE_INLINE LSUINT32 LSE_CALL				TotalMipLevels() const;

		/**
		 * Gets a mipmap level such that 0 is the main image buffer and the remaining are from the mipmap chain.
		 *
		 * \param _ui32Level Level of the buffer to get.
		 * \return Returns a mipmap image buffer, such that index 0 is the main image buffer.
		 */
		LSE_INLINE const CImageLib::CTexelBuffer & LSE_CALL
													GetMipMapBuffers( LSUINT32 _ui32Level ) const;

		/**
		 * Creates a mipmap chain up to the given maximum level or 0 to go all the way down to 1x1.
		 *
		 * \param _ui32Levels Maximum levels to generate or 0 to make a full mipmap chain.  The number includes the
		 *	base image, so a value of 1 will not create more mipmap levels beyond the main image.
		 * \param _fFilter The filter to use for resampling.
		 * \param _amAddressMode The texture address mode to use for resampling.
		 * \return Returns true if all mipmaps were created.
		 */
		LSBOOL LSE_CALL								GenerateMipMaps( LSUINT32 _ui32Levels = 0, LSI_FILTER _fFilter = LSI_F_KAISER_FILTER,
			CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT );

		/**
		 * Create an image from a file already loaded to memory.
		 *
		 * \param _pui8FileData Pointer to the loaded file.
		 * \param _ui32DataLen Length of the loaded file data.
		 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
		 */
		LSBOOL LSE_CALL								LoadFileFromMemory( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen );

		/**
		 * Create an image from a file.  Path must be in UTF-8 format.
		 *
		 * \param _pcFile UTF-8-formatted string representing the path to the file to load.
		 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
		 */
		LSBOOL LSE_CALL								LoadFile( const LSCHAR * _pcFile );

		/**
		 * Creates a blank image.
		 *
		 * \param _pfFormat Format of the image to create.
		 * \param _ui32Width Width of the image to create.
		 * \param _ui32Height Height of the image to create.
		 * \param _ui32MipMaps Number of mipmaps to generate, including the top level.  Specify 0 to generate a full chain.
		 * \param _gGamma Gamma encoding.
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								CreateBlank( LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
			LSUINT32 _ui32MipMaps = 1, LSI_GAMMA _gGamma = LSI_G_LINEAR );

		/**
		 * Create an image from an RGB and alpha source.  The created image is always in LSI_PF_R8G8B8A8 format.  If the alpha
		 *	source has no alpha channel, the intensities of its RGB channels are used to create the alpha.  The dimensions of
		 *	the created image are the the independant maximums of the widths and heights of the input images.
		 *
		 * \param _iRgb The image from which to extract the RGB components for the final image.
		 * \param _iAlpha The image from which to extract the alpha components for the final image.
		 * \return Returns true if there was enough memory to perform the combination.
		 */
		LSBOOL LSE_CALL								CreateFromRgbAndAlpha( const CImage &_iRgb, const CImage &_iAlpha );

		/**
		 * Gets a texel in the given format.  Integer formats only are accepted.
		 *
		 * \param _pfFormat Form in which to return the texel.  Function is fast if the format matches the format of the image.
		 *	Must be an integer format.
		 * \param _ui32X X location of the texel to get.
		 * \param _ui32Y Y location of the texel to get.
		 * \return Returns the texel in the requested format.
		 */
		LSE_INLINE LSUINT64 LSE_CALL				GetTexelAt( LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32X, LSUINT32 _ui32Y ) const;

		/**
		 * Sets a texel at a given location.  Input format can be any valid format.
		 *
		 * \param _ui64Texel The texel value to set at the given location.
		 * \param _pfFormat Format of the incoming texel value.
		 * \param _ui32X X coordinate of the texel to modify.
		 * \param _ui32Y Y coordinate of the texel to modify.
		 */
		LSE_INLINE LSVOID LSE_CALL					SetTexelAt( LSUINT64 _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32X, LSUINT32 _ui32Y );

		/**
		 * Sets a texel at a given location.  Input format can be any valid format.
		 *
		 * \param _ui64Texel The texel value to set at the given location.
		 * \param _pfFormat Format of the incoming texel value.
		 * \param _ui32X X coordinate of the texel to modify.
		 * \param _ui32Y Y coordinate of the texel to modify.
		 * \param _ui32MipLevel Mipmap level at which to set the texel.
		 */
		LSE_INLINE LSVOID LSE_CALL					SetTexelAt( LSUINT64 _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32X, LSUINT32 _ui32Y, LSUINT32 _ui32MipLevel );

		/**
		 * Gets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to get.
		 * \param _ui32Y Y location of the texel to get.
		 * \param _pfReturn Must point to an array of at least 4 LSFLOAT's.  Upon return, this holds the requested texels.
		 */
		LSE_INLINE LSVOID LSE_CALL					GetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, LSFLOAT * _pfReturn ) const;

		/**
		 * Gets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to get.
		 * \param _ui32Y Y location of the texel to get.
		 * \param _pfReturn Must point to an array of at least 4 LSFLOAT's.  Upon return, this holds the requested texels.
		 * \param _ui32MipLevel Mipmap level from which to get the texel.
		 */
		LSE_INLINE LSVOID LSE_CALL					GetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, LSFLOAT * _pfReturn, LSUINT32 _ui32MipLevel ) const;

		/**
		 * Sets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to set.
		 * \param _ui32Y Y location of the texel to set.
		 * \param _pfValue Must point to an array of at least 4 LSFLOAT's which will be used to set the texel at the given coordinates.
		 */
		LSE_INLINE LSVOID LSE_CALL					SetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, const LSFLOAT * _pfValue );

		/**
		 * Sets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to set.
		 * \param _ui32Y Y location of the texel to set.
		 * \param _pfValue Must point to an array of at least 4 LSFLOAT's which will be used to set the texel at the given coordinates.
		 * \param _ui32MipLevel Mipmap level at which to set the texel.
		 */
		LSE_INLINE LSVOID LSE_CALL					SetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, const LSFLOAT * _pfValue, LSUINT32 _ui32MipLevel );

		/**
		 * Compress this texture to a buffer.  All compressions are tested and the smallest is used.  Also stores the
		 *	width, height, and format.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _bNoCompression If true, the image is not compressed when saved to the buffer.  This decreases the amount
		 *	of time it takes for the operation to complete.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressToBuffer( CMisc::LSC_BUFFER &_bBuffer, LSBOOL _bNoCompression = false );

		/**
		 * Flip the image vertically.
		 */
		LSVOID LSE_CALL								Flip();

		/**
		 * Sets the color space.
		 *
		 * \param _gColorSpace The color space to set.
		 */
		LSE_INLINE LSVOID LSE_FCALL					SetColorSpace( LSI_GAMMA _gColorSpace );

		/**
		 * Is the image sRGB?
		 *
		 * \return Returns true if the color space is set to LSI_G_sRGB.
		 */
		LSE_INLINE LSBOOL LSE_CALL					IsSRgb() const;

		/**
		 * Is the given channel entirely set to a given value?
		 *
		 * \param _pcComponent The channel to check.
		 * \param _fValue The reference value.
		 * \return Returns true if the given channel is entirely set to the given value.
		 */
		LSBOOL LSE_CALL								IsChannelSetTo( LSI_PIXEL_COMPONENTS _pcComponent, LSFLOAT _fValue ) const;

		/**
		 * Swaps 2 channels.
		 *
		 * \param _pcCom0 Channel 0.
		 * \param _pcCom1 Channel 1.
		 */
		LSVOID LSE_CALL								SwapChannels( LSI_PIXEL_COMPONENTS _pcCom0, LSI_PIXEL_COMPONENTS _pcCom1 );

		/**
		 * Converts to a given format and return a new image in that format.
		 *
		 * \param _pfFormat Format to which to convert this image.
		 * \param _iDest Destination image.
		 */
		LSVOID LSE_CALL								ConvertToFormat( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDest ) const;

		/**
		 * Converts this image in-place to a given format.
		 *
		 * \param _pfFormat Format to which to convert this image.
		 * \return Returns the converted image.
		 */
		CImage & LSE_CALL							ConvertToFormatInPlace( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Returns a new image instance that represents a resampled version of this image.
		 *
		 * \param _ui32NewWidth The width of the new image.
		 * \param _ui32NewHeight The height of the new image.
		 * \param _fFilter The filter to use for the resample.
		 * \param _iDest Destination image.
		 * \param _fSrcGamma Gamma-correction value.  Set to 1.0f to disable gamma correction.  Partial gamma correction produces
		 *	better mipmaps.  Set to any negative value to use standard-complient sRGB <-> Linear conversions.
		 * \param _amAddressMode Texture addressing mode.
		 * \param _fFilterScale Filter scale.  Values below 1.0f cause aliasing, but can produce better mipmaps.
		 * \return Returns false if there was not enough memory to perform the resample.
		 */
		LSBOOL LSE_CALL								Resample( LSUINT32 _ui32NewWidth, LSUINT32 _ui32NewHeight, LSI_FILTER _fFilter, CImage &_iDest,
			LSFLOAT _fSrcGamma = 2.2f, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, LSFLOAT _fFilterScale = 1.0f ) const;

		/**
		 * Resamples the current image into the given texel buffer.
		 *
		 * \param _ui32NewWidth The width of the new image.
		 * \param _ui32NewHeight The height of the new image.
		 * \param _fFilter The filter to use for the resample.
		 * \param _tbDest Destination texel buffer.
		 * \param _fSrcGamma Gamma-correction value.  Set to 1.0f to disable gamma correction.  Partial gamma correction produces
		 *	better mipmaps.  Set to any negative value to use standard-complient sRGB <-> Linear conversions.
		 * \param _amAddressMode Texture addressing mode.
		 * \param _fFilterScale Filter scale.  Values below 1.0f cause aliasing, but can produce better mipmaps.
		 * \return Returns false if there was not enough memory to perform the resample.
		 */
		LSBOOL LSE_CALL								Resample( LSUINT32 _ui32NewWidth, LSUINT32 _ui32NewHeight, LSI_FILTER _fFilter, CImageLib::CTexelBuffer &_tbDest,
			LSFLOAT _fSrcGamma = 2.2f, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, LSFLOAT _fFilterScale = 1.0f ) const;

		/**
		 * Resample this image in-place to a given size.
		 *
		 * \param _ui32NewWidth The width of the new image.
		 * \param _ui32NewHeight The height of the new image.
		 * \param _fFilter The filter to use for the resample.
		 * \param _fSrcGamma Gamma-correction value.  Set to 1.0f to disable gamma correction.  Partial gamma correction produces
		 *	better mipmaps.  Set to any negative value to use standard-complient sRGB <-> Linear conversions.
		 * \param _amAddressMode Texture addressing mode.
		 * \param _fFilterScale Filter scale.  Values below 1.0f cause aliasing, but can produce better mipmaps.
		 * \return Returns false if there was not enough memory to perform the resample.  In this case, the image is not lost.
		 */
		LSBOOL LSE_CALL								ResampleInPlace( LSUINT32 _ui32NewWidth, LSUINT32 _ui32NewHeight, LSI_FILTER _fFilter,
			LSFLOAT _fSrcGamma = 2.2f, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, LSFLOAT _fFilterScale = 1.0f );

		/**
		 * Calculates and returns the standard 32-bit CRC for this image.
		 *
		 * \return Returns the standard 32-bit CRC for this image.
		 */
		LSUINT32 LSE_CALL							GetCrc32() const;

		/**
		 * Calculates and returns the alternative 32-bit CRC for this image.
		 *
		 * \return Returns the alternative 32-bit CRC for this image.
		 */
		LSUINT32 LSE_CALL							GetCrc32Alt() const;

		/**
		 * Converts the image to a normal map using average color intensities from each channel of the image.
		 *	Normally images are greyscale when converted to normal maps.
		 *
		 * \param _fStr The strength of the normals.  Must not be 0.
		 * \param _bWrapX If true, the normal map is calculated with wrapping at the horizontal borders.
		 * \param _bWrapY If true, the normal map is calculated with wrapping at the vertical borders.
		 */
		LSVOID LSE_CALL								ConvertToNormalMap( LSFLOAT _fStr, LSBOOL _bWrapX = true, LSBOOL _bWrapY = true );

		/**
		 * Converts the image to DDS format.
		 *
		 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
		 * \param _iDst The destination image to which this image is converted.
		 * \param _pdoOptions Conversion options.
		 * \return Returns true if there are no resource problems.
		 */
		LSBOOL LSE_CALL								ConvertToDds( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDst, const LSI_DXT_OPTIONS * _pdoOptions = NULL );

		/**
		 * Converts the image to DDS format in-place.
		 *
		 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
		 * \param _pdoOptions Conversion options.
		 * \return Returns true if there are no resource problems.
		 */
		LSBOOL LSE_CALL								ConvertToDdsInPlace( LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS * _pdoOptions = NULL );

		/**
		 * Pre-multiplies the alpha in-place.  This should not be done more than once on a single image for correct results.
		 */
		LSVOID LSE_CALL								PreMultiplyAlpha();

		/**
		 * Adds a normal-map channel to the Z component.
		 */
		LSVOID LSE_CALL								AddNormalMapZ();

		/**
		 * Normalizes the image in-place.  This should be done for normal maps.
		 */
		LSVOID LSE_CALL								Normalize();

		/**
		 * Is this image black-and-white?
		 *
		 * \return Returns true if the image is in format LSI_PF_A8, or if it is in formats LSI_PF_R4G4B4A4, LSI_PF_R5G5B5A1,
		 *	LSI_PF_R8G8B8, LSI_PF_R8G8B8A8, LSI_PF_R16G16B16A16, LSI_PF_R16G16B16A16F, or LSI_PF_R32G32B32A32F, and the R, G,
		 *	and B channels are all the same.
		 */
		LSBOOL LSE_CALL								IsBlackAndWhite() const;

		/**
		 * Does this image have an alpha channel?
		 *
		 * \return Returns true if the format of the image includes an alpha channel.
		 */
		LSE_INLINE LSBOOL LSE_CALL					HasAlpha() const;

		/*
		 * Creates a fully mipmapped DDS-formatted .LSI 
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */


	protected :
		// == Enumerations.
		/** Resampling metrics. */
		enum {
			LSI_LINEAR_TO_SRGB_TABLE_SIZE			= 4096,			/**< Size of the table used to convert linear values to SRGB. */
		};


		// == Types.
		/** Data for generating mipmaps on threads. */
		typedef struct LSI_MIPMAP_DATA {
			/** Source data. */
			const CImage *							piSrc;

			/** New width. */
			LSUINT32								ui32NewWidth;

			/** New height. */
			LSUINT32								ui32NewHeight;

			/** Filter. */
			LSI_FILTER								fFilter;

			/** Destination texel buffer. */
			CImageLib::CTexelBuffer *				ptbDst;

			/** Gamma. */
			LSFLOAT									fGamma;

			/** Texture addressing mode. */
			CResampler::LSI_ADDRESS_MODE			amAddressMode;

			/** Gamma scale. */
			LSFLOAT									fGammaScale;
		} * LPLSI_MIPMAP_DATA, * const LPCLSI_MIPMAP_DATA;

		/** For sorting pixels.  Used when creating index tables. */
		typedef struct LSI_SORTED_PIXEL {
			/** The actual pixel value. */
			union LSI_COL {
				LSUINT8								ui8Rgb[16];

				LSUINT64							ui64Rgb64;
			}										u;

			/** Length of this pixel. */
			LSUINT32								ui32Length;


			// == Operators.
			/**
			 * Less-than comparison.
			 *
			 * \param _spOther Object against which to test.
			 * \return Returns true if this object is lexicographically less than the given object.
			 */
			LSBOOL LSE_CALL							operator < ( const LSI_SORTED_PIXEL &_spOther ) const;

			/**
			 * Equality comparison.
			 *
			 * \param _spOther Object against which to test.
			 * \return Returns true if this object is lexicographically equal to the given object.
			 */
			LSBOOL LSE_CALL							operator == ( const LSI_SORTED_PIXEL &_spOther ) const;
		} * LPLSI_SORTED_PIXEL, * const LPCLSI_SORTED_PIXEL;

		/** A block of texels for DDS encoding. */
		typedef union LSI_BLOCK {
			struct LSI_COLOR {
				LSFLOAT								fR, fG, fB, fA;
			}										s;
			LSFLOAT									fValues[4];


			// == Operators.
			/**
			 * The less-than operator.
			 *
			 * \param _bOther The object against which to test.
			 * \return Returns true if this object is less than the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL				operator < ( const LSI_BLOCK &_bOther ) const;

			/**
			 * The equality operator.
			 *
			 * \param _bOther The object against which to test.
			 * \return Returns true if this object is equal to the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL				operator == ( const LSI_BLOCK &_bOther ) const;

			/**
			 * The inequality operator.
			 *
			 * \param _bOther The object against which to test.
			 * \return Returns true if this object is not equal to the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL				operator != ( const LSI_BLOCK &_bOther ) const;
		} * LPLSI_BLOCK, * const LPCLSI_BLOCK;

		/** Thread data for converting blocks to DXTn. */
		typedef struct LSI_DXT_THREAD_DATA {
			/** The block X starting point in the source data. */
			LSUINT32								ui32SrcX;

			/** The block Y starting point in the source data. */
			LSUINT32								ui32SrcY;

			/** The source image. */
			const CImage *							piSrcImage;

			/** Destination of the compressioned chunk. */
			LSUINT8 *								pui8Dest;

			/** Pixel format. */
			LSI_PIXEL_FORMAT						pfFormat;

			/** Indicates the thread has finished. */
			LSBOOL									bDone;

			/** The conversion options. */
			CImage::LSI_DXT_OPTIONS *				pdoOptions;
		} * LPLSI_DXT_THREAD_DATA, * const LPCLSI_DXT_THREAD_DATA;


		// == Members.
		/** The actual texel buffer. */
		CImageLib::CTexelBuffer						m_tbBuffer;

		/** Optional precalculated mipmaps. */
		CVector<CImageLib::CTexelBuffer>			m_vMipMapBuffer;

		/** The width of the image in texels. */
		LSUINT32									m_ui32Width;

		/** The height of the image in texels. */
		LSUINT32									m_ui32Height;

		/** The pixel format. */
		LSI_PIXEL_FORMAT							m_pfFormat;

		/** The width of rows, in pixels. */
		LSUINT32									m_ui32RowWidth;

		/** Texel size in bytes. */
		LSUINT32									m_ui32TexelSize;

		/** Gamma encoding. */
		LSI_GAMMA									m_gColorSpace;

		/** Found best matches for blocks of a single color in a DXT compression. */
		static CLinearMap<LSUINT32, LSUINT32>		m_mSingleColorMatches;

		/** Critical section for accessing m_mSingleColorMatches. */
		static CCriticalSection						m_csDxtCrit;

		/** Single-color R/B 4-index DXT table. */
		static CVectorPoD<LSI_DXT_TABLE_4, LSUINT16>m_vDxtTableRb4;

		/** Single-color G 4-index DXT table. */
		static CVectorPoD<LSI_DXT_TABLE_4, LSUINT16>m_vDxtTableG4;


		// == Functions.
		/**
		 * Called after a successful load.  Sets various values based on the loaded data.
		 */
		LSVOID LSE_CALL								PostLoad();

		/**
		 * Compress this texture by grouping the reds, greens, blues, and alphas together
		 *	and compressing them separately.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressChannels( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress this texture by just performing a raw compression of the data.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressRaw( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress this texture by using RLE encoding.  The RLE data is also compressed.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressRle( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress this texture by creating indexed data.  The index table and indexed data are
		 *	then further compressed.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressIndex( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress an image that is already in DXT format.  Simply uses LZW on the compressed data.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressDxt( CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Loads an LSI image that was compressed via channel compression.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadCompressedChannels( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut );

		/**
		 * Loads an LSI image that was compressed via raw compression.
		 *
		 * \param _ui8MipLevels The number of mipmap levels in the file.  Always at least 1.
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the level-0 decompressed image data.
		 * \param _vMipMaps Buffers where to store the mipmap decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadCompressedRaw( LSUINT8 _ui8MipLevels, const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
			CVector<CImageLib::CTexelBuffer> &_vMipMaps );

		/**
		 * Loads an LSI image that was compressed via bitwise RLE compression.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadCompressedBitwiseRle( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut );

		/**
		 * Loads an LSI image that was compressed via index compression.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadCompressedIndices( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut );

		/**
		 * Loads an LSI image that was compressed via DXT compression.
		 *
		 * \param _ui8MipLevels The number of mipmap levels in the file.  Always at least 1.
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the level-0 decompressed image data.
		 * \param _vMipMaps Buffers where to store the mipmap decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadDxt( LSUINT8 _ui8MipLevels, const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
			CVector<CImageLib::CTexelBuffer> &_vMipMaps );

		/**
		 * Loads an LSI file from memory.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL								LoadLsi( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen );

		/**
		 * Loads an image via the FreeImage library.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL								LoadFreeImage( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen );

		/**
		 * A thread function for resampling from a source image to a destination texel buffer.
		 *
		 * \param _lpParameter Pointer to an LSI_MIPMAP_DATA structure.
		 * \return Returns the result of the call to Resample() on the created thread.
		 */
		static LSUINT32 LSH_CALL					ResampleThread( LSVOID * _lpParameter );

		/**
		 * Takes an array of 16 texels in floating-point format and determines the two colors best suited to represent
		 *	the given colors via simple interpolation.  In other words, it determines the two key colors for a DXT
		 *	block.
		 *
		 * \param _bBlock The input block of 4-by-4 texels.
		 * \param _bMax Upon return, this holds the computed maximum color value.
		 * \param _bMin Upon return, this holds the computed minimum color value.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _fAlphaHigh The highest alpha value in the block.
		 * \param _fAlphaLow The lowest alpha value in the block.
		 * \param _pfFormat The DXTn format to which the block is to be converted.
		 * \param _doOptions Options for the conversion.
		 */
		static LSVOID LSE_CALL						GetKeyColors( const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
			LSFLOAT _fAlphaHigh, LSFLOAT _fAlphaLow, LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Refines the given key colors iteratively, with each iteration bringing it closer to the most optimal solution.
		 *
		 * \param _bUniqueValues The unique values in each channel in the block, along with the number of times the value appears.
		 * \param _bUniqueColors The unique colors in the block, along with the number of times the value appears.
		 * \param _bBlock The input block of 4-by-4 texels.
		 * \param _bMax Upon return, this holds the computed maximum color value.
		 * \param _bMin Upon return, this holds the computed minimum color value.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _ui32Iters Iteration count for the 2nd phase.
		 * \param _fAlphaHigh The highest alpha value in the block.
		 * \param _fAlphaLow The lowest alpha value in the block.
		 * \param _pfFormat The DXTn format to which the block is to be converted.
		 * \param _doOptions Options for the conversion.
		 */
		static LSVOID LSE_CALL						RefineKeyColors( const CStaticMap<LSFLOAT, LSUINT32, 16> _bUniqueValues[4], const CStaticMap<LSI_BLOCK, LSUINT32, 16> &_bUniqueColors,
			const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
			LSUINT32 _ui32Iters, LSFLOAT _fAlphaHigh, LSFLOAT _fAlphaLow, LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Gets the best color pair for a single-color block.
		 *
		 * \param _bUniqueColors The unique colors in the block, along with the number of times the value appears.
		 * \param _bBlock The input block of 4-by-4 texels.
		 * \param _bMax Upon return, this holds the computed maximum color value.
		 * \param _bMin Upon return, this holds the computed minimum color value.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _bOmitLast If true, 3 colors are returned instead of 4.
		 * \param _doOptions Options for the conversion.
		 * \return Returns true if all channels have only one unique color.
		 */
		static LSBOOL LSE_CALL						GetBestSingleColor( const CStaticMap<LSI_BLOCK, LSUINT32, 16> &_bUniqueColors,
			const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
			LSBOOL _bOmitLast, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Fixes a 16-bit color pair by swapping some components to create a better match for the given input block.
		 *
		 * \param _bBlock The input block of 4-by-4 texels.
		 * \param _bMax Upon return, this holds the computed maximum color value.
		 * \param _bMin Upon return, this holds the computed minimum color value.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _bOmitLast If true, 3 colors are used instead of 4.
		 * \param _doOptions Options for the conversion.
		 */
		static LSVOID LSE_CALL						Finalize16BitColors( const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
			LSBOOL _bOmitLast, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Makes the 4 colors that can be created with the given block texels.
		 *
		 * \param _bMax The max color.
		 * \param _bMin The min color.
		 * \param _bRet The returned array of 4 colors.
		 * \param _bOmitLast If true, 3 colors are returned instead of 4.
		 */
		static LSVOID LSE_CALL						Get4Colors( const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin, LSI_BLOCK _bRet[4], LSBOOL _bOmitLast );

		/**
		 * Makes the 4 colors that can be created with the given block texels on one channel only.
		 *
		 * \param _ui32Channel The channel for which to derive the 4 possible values.
		 * \param _bMax The max color.
		 * \param _bMin The min color.
		 * \param _fRet The returned array of 4 colors along the given channel.
		 * \param _bOmitLast If true, 3 colors are returned instead of 4.
		 */
		static LSVOID LSE_CALL						Get4Colors( LSUINT32 _ui32Channel, const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin, LSFLOAT _fRet[4], LSBOOL _bOmitLast );

		/**
		 * Makes a DXT1 block.
		 *
		 * \param _bBlock Contains the 4-by-4 block of texels to be encoded.
		 * \param _fMax Holds the maximum values for each channel of the color to use.
		 * \param _fMin Holds the minimum values for each channel of the color to use.
		 * \param _fAlphaHigh The highest alpha value in the block.
		 * \param _fAlphaLow The lowest alpha value in the block.
		 * \param _doOptions Options for the conversion.
		 * \param _pui8Return Holds the returned compressed block.
		 */
		static LSVOID LSE_CALL						MakeDxt1Block( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
			LSFLOAT _fAlphaHigh, LSFLOAT _fAlphaLow,
			LSUINT8 * _pui8Return, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Makes a DXT3 block.
		 *
		 * \param _bBlock Contains the 4-by-4 block of texels to be encoded.
		 * \param _fMax Holds the maximum values for each channel of the color to use.
		 * \param _fMin Holds the minimum values for each channel of the color to use.
		 * \param _doOptions Options for the conversion.
		 * \param _pui8Return Holds the returned compressed block.
		 */
		static LSVOID LSE_CALL						MakeDxt3Block( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
			LSUINT8 * _pui8Return, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Makes a DXT5 block.
		 *
		 * \param _bBlock Contains the 4-by-4 block of texels to be encoded.
		 * \param _fMax Holds the maximum values for each channel of the color to use.
		 * \param _fMin Holds the minimum values for each channel of the color to use.
		 * \param _doOptions Options for the conversion.
		 * \param _pui8Return Holds the returned compressed block.
		 */
		static LSVOID LSE_CALL						MakeDxt5Block( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
			LSUINT8 * _pui8Return, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Finds the 2-bit DXT code that represents the given color's relationship between the two given key colors.
		 *	0	color0
		 *	1	color1
		 *	2	(2*color0 + color1) / 3
		 *	3	(color0 + 2*color1) / 3
		 *
		 * \param _bColor0 The first key color.
		 * \param _bColor1 The second key color.
		 * \param _bColor The color to test for being closer to one or the other key color.
		 * \param _fAlphaHigh The highest alpha value in the block.
		 * \param _fAlphaLow The lowest alpha value in the block.
		 * \param _doOptions Weights to use for the error calculation.
		 * \return Returns a 2-bit DXT code indicating the color's relationship to the given key colors.
		 */
		static LSUINT32 LSE_CALL					FindDxt1Match( const LSI_BLOCK &_bColor0, const LSI_BLOCK &_bColor1,
			const LSI_BLOCK &_bColor, LSFLOAT _fAlphaHigh, LSFLOAT _fAlphaLow, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Finds the 3-bit DXT code that represents the given alpha value for an alpha0 > alpha1 block in DXT5.
		 *
		 * \param _fAlpha0 The first key color.
		 * \param _fAlpha1 The second key color.
		 * \param _fAlpha The alpha value from 0 to 255.
		 * \return Returns the 3-bit code for the given alpha in relationship to the key alphas using alpha0 > alpha1
		 *	rules.
		 */
		static LSUINT32 LSE_CALL					FindDxt5AlphaMatch0( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha );

		/**
		 * Finds the 3-bit DXT code that represents the given alpha value for an alpha0 <= alpha1 block in DXT5.
		 *
		 * \param _fAlpha0 The first key color.
		 * \param _fAlpha1 The second key color.
		 * \param _fAlpha The alpha value from 0 to 255.
		 * \return Returns the 3-bit code for the given alpha in relationship to the key alphas using alpha0 <= alpha1
		 *	rules.
		 */
		static LSUINT32 LSE_CALL					FindDxt5AlphaMatch1( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha );

		/**
		 * Finds the key colors for DXT5 alpha.
		 *
		 * \param _ui16Color0 The returned left key color.
		 * \param _ui16Color1 The returned right key color.
		 * \param _bBlock Contains the 4-by-4 block of texels to be used to determine the key colors.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 */
		static LSVOID LSE_CALL						FindDxt5AlphaKeyColors( LSUINT16 &_ui16Color0, LSUINT16 &_ui16Color1,
			const LSI_BLOCK _bBlock[4][4], LSUINT32 _ui32Width, LSUINT32 _ui32Height );

		/**
		 * Gets the amount of error for a DXT5 alpha block using the alpha0 > alpha1 rules.
		 *
		 * \param _fAlpha0 alpha0.
		 * \param _fAlpha1 alpha1.
		 * \param _fAlpha The value whose error is to be calculated.
		 * \return Returns the minimum amount of error between the alpha points for the given alpha value.
		 */
		static LSFLOAT LSE_CALL						GetErrorDxt5Alpha0( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha );

		/**
		 * Gets the amount of non-normalized error for a DXT5 alpha block using the alpha0 > alpha1 rules.
		 *	Of note is that this is a special-case function which assumes the alpha values in the block have
		 *	already been set to the 0-255 range.
		 *
		 * \param _fAlpha0 alpha0.
		 * \param _fAlpha1 alpha1.
		 * \param _bBlock Contains the 4-by-4 block of texels to be used to determine the key colors.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \return Returns the minimum amount of error between the alpha points for the given block.
		 */
		static LSFLOAT LSE_CALL						GetErrorDxt5Alpha0( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1,
			const LSI_BLOCK _bBlock[4][4], LSUINT32 _ui32Width, LSUINT32 _ui32Height );

		/**
		 * Gets the amount of error for a DXT5 alpha block using the alpha0 <= alpha1 rules.
		 *
		 * \param _fAlpha0 alpha0.
		 * \param _fAlpha1 alpha1.
		 * \param _fAlpha The value whose error is to be calculated.
		 * \return Returns the minimum amount of error between the alpha points for the given alpha value.
		 */
		static LSFLOAT LSE_CALL						GetErrorDxt5Alpha1( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha );

		/**
		 * Gets the amount of non-normalized error for a DXT5 alpha block using the alpha0 <= alpha1 rules.
		 *	Of note is that this is a special-case function which assumes the alpha values in the block have
		 *	already been set to the 0-255 range.
		 *
		 * \param _fAlpha0 alpha0.
		 * \param _fAlpha1 alpha1.
		 * \param _bBlock Contains the 4-by-4 block of texels to be used to determine the key colors.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \return Returns the minimum amount of error between the alpha points for the given block.
		 */
		static LSFLOAT LSE_CALL						GetErrorDxt5Alpha1( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1,
			const LSI_BLOCK _bBlock[4][4], LSUINT32 _ui32Width, LSUINT32 _ui32Height );

		/**
		 * Gets the amount of error between 2 colors.
		 *
		 * \param _bColor0 Color 1.
		 * \param _bColor1 Color 2.
		 * \param _doOptions Weights to use for the error calculation.
		 * \return Returns the amount of error between 2 colors.
		 */
		static LSE_INLINE LSFLOAT LSE_CALL			GetError( const LSI_BLOCK &_bColor0, const LSI_BLOCK &_bColor1, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Gets the amount of error of an entire block between 2 colors.  The orders of the highs and lows in the channels does
		 *	not impact the result.
		 *
		 * \param _bUniqueColors The input block of 4-by-4 texels.
		 * \param _bMax The max color.
		 * \param _bMin The min color.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _bOmitLast If true, 3 colors are returned instead of 4.
		 * \param _fCurError The current amount of error.  Knowing this helps the routine early-out.
		 * \param _doOptions Weights to use for the error calculation.
		 * \return Returns the amount of error among the block given the 2 end colors.
		 */
		static LSFLOAT LSE_CALL						GetError( const CStaticMap<LSI_BLOCK, LSUINT32, 16> &_bUniqueColors, const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
			LSUINT32 _ui32Width, LSUINT32 _ui32Height, LSBOOL _bOmitLast, LSFLOAT _fCurError, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Gets the amount of error of an entire block between 2 colors.
		 *
		 * \param _bBlock The input block of 4-by-4 texels.
		 * \param _bMax The max color.
		 * \param _bMin The min color.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _bOmitLast If true, 3 colors are returned instead of 4.
		 * \param _doOptions Weights to use for the error calculation.
		 * \return Returns the amount of error among the block given the 2 end colors.
		 */
		static LSFLOAT LSE_CALL						GetErrorStrict( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
			LSUINT32 _ui32Width, LSUINT32 _ui32Height, LSBOOL _bOmitLast, const LSI_DXT_OPTIONS &_doOptions );

		/**
		 * Gets the amount of error of an entire block between 1 channel of 2 colors.
		 *
		 * \param _ui32Channel The channel for which to determine the amount of error.
		 * \param _bBlock The input block of 4-by-4 texels.
		 * \param _bMax The max color.
		 * \param _bMin The min color.
		 * \param _ui32Width Width of the block.
		 * \param _ui32Height Height of the block.
		 * \param _bOmitLast If true, 3 colors are returned instead of 4.
		 * \return Returns the amount of error among the block given the 2 end colors.
		 */
		static LSE_INLINE LSFLOAT LSE_CALL			GetError( LSUINT32 _ui32Channel,
			const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
			LSUINT32 _ui32Width, LSUINT32 _ui32Height, LSBOOL _bOmitLast );

		/**
		 * Gets the amount of error of a single color channel between 4 block colors.
		 *
		 * \param _ui32Channel The channel for which to determine the amount of error.
		 * \param _bBlocks The input array of 4 colors.
		 * \param _bSrc The color to test against the array of blocks to find the least amount of error.
		 * \param _ui32Index On return, this holds the index of the color that was the closest match.
		 * \param _bOmitLast If true, 3 colors are checked instead of 4.
		 * \return Returns the signed error between the input color and the array of colors.
		 */
		static LSE_INLINE LSFLOAT LSE_CALL			GetError( LSUINT32 _ui32Channel,
			const LSI_BLOCK _bBlocks[4], const LSI_BLOCK &_bSrc, LSUINT32 &_ui32Index, LSBOOL _bOmitLast );

		/**
		 * Gets the amount of error of a single color channel between 4 block colors.
		 *
		 * \param _ui32Channel The channel for which to determine the amount of error.
		 * \param _bBlocks The input array of 4 colors.
		 * \param _fValue The value to test against the array of blocks to find the least amount of error.
		 * \param _ui32Index On return, this holds the index of the color that was the closest match.
		 * \param _bOmitLast If true, 3 colors are checked instead of 4.
		 * \return Returns the signed error between the input color and the array of colors.
		 */
		static LSE_INLINE LSFLOAT LSE_CALL			GetError( LSUINT32 _ui32Channel,
			const LSI_BLOCK _bBlocks[4], LSFLOAT _fValue, LSUINT32 &_ui32Index, LSBOOL _bOmitLast );

		/**
		 * Converts a block texel to a 16-bit R5 G6 B5 value.
		 *
		 * \param _bColor The color to convert.
		 * \return Returns the converted 16-bit color.
		 */
		static LSE_INLINE LSUINT16 LSE_CALL			ConvertBlockTexelTo16Bit( const LSI_BLOCK &_bColor );

		/**
		 * Converts a 16-bit R5 G6 B5 value into a block texel.
		 *
		 * \param _ui16Color The value to convert.
		 * \return Returns the converted block texel.
		 */
		static LSE_INLINE LSI_BLOCK LSE_CALL		Convert16BitToBlockTexel( LSUINT16 _ui16Color );

		/**
		 * Offsets a component in a 16-bit R5 G6 B5 color by a given amount.  If false is returned, the component
		 *	can't be offset by the given amount.
		 *
		 * \param _ui32Channel The channel on the color to modify.
		 * \param _i32Amount The amount by which to modify the color.
		 * \param _ui16Color The color to modify.
		 * \return Returns true if the offset was applied.
		 */
		static LSE_INLINE LSBOOL LSE_CALL			Offset16BitColorChannel( LSUINT32 _ui32Channel, LSINT32 _i32Amount, LSUINT16 &_ui16Color );

		/**
		 * Gets the 2 end points that would allow the line segment between them pass through the given value.
		 *
		 * \param _fEndPoint The left end point.
		 * \param _fMidPoint The point through which to pass.
		 * \param _ui32EndIndex The index of the left point.
		 * \param _ui32MidIndex The index of the middle point.
		 * \param _ui32Points The total number of possible points.
		 * \param _fRes The resolution of the rounding.
		 * \param _fLow Holds the low end point on return.
		 * \param _fHigh Holds the high end point on return.
		 */
		static LSVOID LSE_CALL						FindEndPoints( LSFLOAT _fEndPoint, LSFLOAT _fMidPoint, LSUINT32 _ui32EndIndex, LSUINT32 _ui32MidIndex,
			LSUINT32 _ui32Points, LSFLOAT _fRes, LSFLOAT &_fLow, LSFLOAT &_fHigh );

		/**
		 * Rounds the given value off given a resolution for rounding.
		 *
		 * \param _fValue The value to round.
		 * \param _fRes The resolution of the rounding.
		 * \return Returns the rounded value.
		 */
		static LSE_INLINE LSFLOAT LSE_CALL			Round( LSFLOAT _fValue, LSFLOAT _fRes );

		/**
		 * Rounds off the colors in a block.
		 *
		 * \param _bColor The color to round.
		 */
		static LSE_INLINE LSVOID LSE_CALL			RoundOffBlock( LSI_BLOCK &_bColor );

		/**
		 * Thread function for converting a block of texels to DXTn format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_DXT_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static LSUINT32 LSH_CALL					DxtThread( LSVOID * _lpParameter );

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
	LSE_INLINE LSBOOL LSE_CALL CImage::LSI_BLOCK::operator < ( const LSI_BLOCK &_bOther ) const {
		return fValues[0] < _bOther.fValues[0] && fValues[1] < _bOther.fValues[1] &&
			fValues[2] < _bOther.fValues[2] && fValues[3] < _bOther.fValues[3];
	}

	 /**
	 * The equality operator.
	 *
	 * \param _bOther The object against which to test.
	 * \return Returns true if this object is equal to the given object.
	 */
	LSE_INLINE LSBOOL LSE_CALL CImage::LSI_BLOCK::operator == ( const LSI_BLOCK &_bOther ) const {
		return fValues[0] == _bOther.fValues[0] && fValues[1] == _bOther.fValues[1] &&
			fValues[2] == _bOther.fValues[2] && fValues[3] == _bOther.fValues[3];
	}

	 /**
	 * The inequality operator.
	 *
	 * \param _bOther The object against which to test.
	 * \return Returns true if this object is not equal to the given object.
	 */
	LSE_INLINE LSBOOL LSE_CALL CImage::LSI_BLOCK::operator != ( const LSI_BLOCK &_bOther ) const {
		return fValues[0] != _bOther.fValues[0] || fValues[1] != _bOther.fValues[1] ||
			fValues[2] != _bOther.fValues[2] || fValues[3] != _bOther.fValues[3];
	}

	// == Functions.
	/**
	 * Gets the image width.
	 *
	 * \return Returns the image width.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CImage::GetWidth() const {
		return m_ui32Width;
	}

	/**
	 * Gets the image height.
	 *
	 * \return Returns the image height.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CImage::GetHeight() const {
		return m_ui32Height;
	}

	/**
	 * Gets the texel format.
	 *
	 * \return Returns the texel format.
	 */
	LSE_INLINE LSI_PIXEL_FORMAT LSE_CALL CImage::GetFormat() const {
		return m_pfFormat;
	}

	/**
	 * Gets a read-only pointer to the raw buffer of texel data.
	 *
	 * \return Returns a read-only pointer to the raw buffer of texel data.
	 */
	LSE_INLINE const LSVOID * LSE_CALL CImage::GetBufferData() const {
		return m_tbBuffer.Length() ? &m_tbBuffer[0] : NULL;
	}

	/**
	 * Gets a read-only reference to the raw buffer of texel data as a vector.
	 *
	 * \return Returns a read-only reference to the raw buffer of texel data as a vector.
	 */
	LSE_INLINE const CImageLib::CTexelBuffer & LSE_CALL CImage::GetBufferDataAsVector() const {
		return m_tbBuffer;
	}
	/**
	 * Gets the mipmaps of this image, if any.
	 *
	 * \return Returns the optional mipmap chain in this image.
	 */
	LSE_INLINE const CVector<CImageLib::CTexelBuffer> & LSE_CALL CImage::GetMipMapBuffers() const {
		return m_vMipMapBuffer;
	}

	/**
	 * Gets a mipmap level such that 0 is the main image buffer and the remaining are from the mipmap chain.
	 *
	 * \param _ui32Level Level of the buffer to get.
	 * \return Returns a mipmap image buffer, such that index 0 is the main image buffer.
	 */
	LSE_INLINE const CImageLib::CTexelBuffer & LSE_CALL CImage::GetMipMapBuffers( LSUINT32 _ui32Level ) const {
		return _ui32Level == 0 ? GetBufferDataAsVector() : GetMipMapBuffers()[_ui32Level-1];
	}

	/**
	 * Gets the total mipmap levels including the base level.
	 *
	 * \return Returns the total mipmap levels including the base level.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CImage::TotalMipLevels() const {
		return GetMipMapBuffers().Length() + 1;
	}

	/**
	 * Gets a texel in the given format.  Integer formats only are accepted.
	 *
	 * \param _pfFormat Form in which to return the texel.  Function is fast if the format matches the format of the image.
	 *	Must be an integer format.
	 * \param _ui32X X location of the texel to get.
	 * \param _ui32Y Y location of the texel to get.
	 * \return Returns the texel in the requested format.
	 */
	LSE_INLINE LSUINT64 LSE_CALL CImage::GetTexelAt( LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32X, LSUINT32 _ui32Y ) const {
		assert( _pfFormat < LSI_PF_TOTAL_INT );
		return CImageLib::IntegerTexelToIntegerTexel( GetFormat(), _pfFormat, (*reinterpret_cast<const LSUINT64 *>(&m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize])) );
	}

	/**
	 * Sets a texel at a given location.  Input format can be any valid format.
	 *
	 * \param _ui64Texel The texel value to set at the given location.
	 * \param _pfFormat Format of the incoming texel value.
	 * \param _ui32X X coordinate of the texel to modify.
	 * \param _ui32Y Y coordinate of the texel to modify.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::SetTexelAt( LSUINT64 _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32X, LSUINT32 _ui32Y ) {
		assert( _pfFormat < LSI_PF_TOTAL_INT );
		LSUINT64 ui64Converted = CImageLib::IntegerTexelToIntegerTexel( _pfFormat, GetFormat(), _ui64Texel );

		static const LSUINT64 ui64Mask[] = {
			0xFFFFFFFFFFFFFFFFULL,
			0xFFFFFFFFFFFFFF00ULL,
			0xFFFFFFFFFFFF0000ULL,
			0xFFFFFFFFFF000000ULL,
			0xFFFFFFFF00000000ULL,
			0xFFFFFF0000000000ULL,
			0xFFFF000000000000ULL,
			0xFF00000000000000ULL,
			0x0000000000000000ULL
		};
		static const LSUINT64 ui32Sizes[] = {
			0x0000000000000000ULL,
			0x00000000000000FFULL,
			0x000000000000FFFFULL,
			0x0000000000FFFFFFULL,
			0x00000000FFFFFFFFULL,
			0x000000FFFFFFFFFFULL,
			0x0000FFFFFFFFFFFFULL,
			0x00FFFFFFFFFFFFFFULL,
			0xFFFFFFFFFFFFFFFFULL
		};
		//LSUINT32 ui32Size = CImageLib::GetFormatSize( GetFormat() );

		LSUINT64 * pui64Dst = reinterpret_cast<LSUINT64 *>(&m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize]);
		(*pui64Dst) = ((*pui64Dst) & ui64Mask[m_ui32TexelSize]) | (ui64Converted & ui32Sizes[m_ui32TexelSize]);
	}

	/**
	 * Sets a texel at a given location.  Input format can be any valid format.
	 *
	 * \param _ui64Texel The texel value to set at the given location.
	 * \param _pfFormat Format of the incoming texel value.
	 * \param _ui32X X coordinate of the texel to modify.
	 * \param _ui32Y Y coordinate of the texel to modify.
	 * \param _ui32MipLevel Mipmap level at which to set the texel.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::SetTexelAt( LSUINT64 _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32X, LSUINT32 _ui32Y, LSUINT32 _ui32MipLevel ) {
		assert( _pfFormat < LSI_PF_TOTAL_INT );
		LSUINT64 ui64Converted = CImageLib::IntegerTexelToIntegerTexel( _pfFormat, GetFormat(), _ui64Texel );

		static const LSUINT64 ui64Mask[] = {
			0xFFFFFFFFFFFFFFFFULL,
			0xFFFFFFFFFFFFFF00ULL,
			0xFFFFFFFFFFFF0000ULL,
			0xFFFFFFFFFF000000ULL,
			0xFFFFFFFF00000000ULL,
			0xFFFFFF0000000000ULL,
			0xFFFF000000000000ULL,
			0xFF00000000000000ULL,
			0x0000000000000000ULL
		};
		static const LSUINT64 ui32Sizes[] = {
			0x0000000000000000ULL,
			0x00000000000000FFULL,
			0x000000000000FFFFULL,
			0x0000000000FFFFFFULL,
			0x00000000FFFFFFFFULL,
			0x000000FFFFFFFFFFULL,
			0x0000FFFFFFFFFFFFULL,
			0x00FFFFFFFFFFFFFFULL,
			0xFFFFFFFFFFFFFFFFULL
		};
		//LSUINT32 ui32Size = CImageLib::GetFormatSize( GetFormat() );
		LSUINT32 ui32RowWidth = CImageLib::GetRowSize( GetFormat(), CStd::Max<LSUINT32>( GetWidth() >> _ui32MipLevel, 1 ) );
		LSUINT8 * pui8Buffer = const_cast<LSUINT8 *>(&GetMipMapBuffers( _ui32MipLevel )[0]);
		LSUINT64 * pui64Dst = reinterpret_cast<LSUINT64 *>(&pui8Buffer[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize]);
		(*pui64Dst) = ((*pui64Dst) & ui64Mask[m_ui32TexelSize]) | (ui64Converted & ui32Sizes[m_ui32TexelSize]);
	}

	/**
	 * Gets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to get.
	 * \param _ui32Y Y location of the texel to get.
	 * \param _pfReturn Must point to an array of at least 4 LSFLOAT's.  Upon return, this holds the requested texels.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::GetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, LSFLOAT * _pfReturn ) const {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			CImageLib::BatchIntegerTexelToFloat32Texel( GetFormat(), &m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize],
				_pfReturn, 1 );
		}
		else if ( GetFormat() >= LSI_PF_R16G16B16A16F ) {
			CImageLib::BatchFloatTexelToFloatTexel( GetFormat(), &m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize],
				LSI_PF_R32G32B32A32F, _pfReturn, 1 );
		}
	}

	/**
	 * Gets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to get.
	 * \param _ui32Y Y location of the texel to get.
	 * \param _pfReturn Must point to an array of at least 4 LSFLOAT's.  Upon return, this holds the requested texels.
	 * \param _ui32MipLevel Mipmap level from which to get the texel.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::GetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, LSFLOAT * _pfReturn, LSUINT32 _ui32MipLevel ) const {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		LSUINT32 ui32RowWidth = CImageLib::GetRowSize( GetFormat(), CStd::Max<LSUINT32>( GetWidth() >> _ui32MipLevel, 1 ) );
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			CImageLib::BatchIntegerTexelToFloat32Texel( GetFormat(), &GetMipMapBuffers( _ui32MipLevel )[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize],
				_pfReturn, 1 );
		}
		else if ( GetFormat() >= LSI_PF_R16G16B16A16F ) {
			CImageLib::BatchFloatTexelToFloatTexel( GetFormat(), &GetMipMapBuffers( _ui32MipLevel )[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize],
				LSI_PF_R32G32B32A32F, _pfReturn, 1 );
		}
	}

	/**
	 * Sets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to set.
	 * \param _ui32Y Y location of the texel to set.
	 * \param _pfValue Must point to an array of at least 4 LSFLOAT's which will be used to set the texel at the given coordinates.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::SetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, const LSFLOAT * _pfValue ) {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			LSUINT64 ui64Temp = CImageLib::Float32TexelToIntegerTexel( GetFormat(), _pfValue );
			SetTexelAt( ui64Temp, GetFormat(), _ui32X, _ui32Y );
		}
		else {
			// Float-to-float.
			CImageLib::BatchFloatTexelToFloatTexel( LSI_PF_R32G32B32A32F, _pfValue,
				GetFormat(), &m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize], 1 );
		}
	}

	/**
	 * Sets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to set.
	 * \param _ui32Y Y location of the texel to set.
	 * \param _pfValue Must point to an array of at least 4 LSFLOAT's which will be used to set the texel at the given coordinates.
	 * \param _ui32MipLevel Mipmap level at which to set the texel.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::SetTexelAt( LSUINT32 _ui32X, LSUINT32 _ui32Y, const LSFLOAT * _pfValue, LSUINT32 _ui32MipLevel ) {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			LSUINT64 ui64Temp = CImageLib::Float32TexelToIntegerTexel( GetFormat(), _pfValue );
			SetTexelAt( ui64Temp, GetFormat(), _ui32X, _ui32Y, _ui32MipLevel );
		}
		else {
			LSUINT32 ui32RowWidth = CImageLib::GetRowSize( GetFormat(), CStd::Max<LSUINT32>( GetWidth() >> _ui32MipLevel, 1 ) );
			LSUINT8 * pui8Buffer = const_cast<LSUINT8 *>(&GetMipMapBuffers( _ui32MipLevel )[0]);
			// Float-to-float.
			CImageLib::BatchFloatTexelToFloatTexel( LSI_PF_R32G32B32A32F, _pfValue,
				GetFormat(), &pui8Buffer[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize], 1 );
		}
	}

	/**
	 * Sets the color space.
	 *
	 * \param _gColorSpace The color space to set.
	 */
	LSE_INLINE LSVOID LSE_FCALL CImage::SetColorSpace( LSI_GAMMA _gColorSpace ) {
		m_gColorSpace = _gColorSpace;
	}

	/**
	 * Is the image sRGB?
	 *
	 * \return Returns true if the color space is set to LSI_G_sRGB.
	 */
	LSE_INLINE LSBOOL LSE_CALL CImage::IsSRgb() const {
		return m_gColorSpace == LSI_G_sRGB;
	}

	/**
	 * Does this image have an alpha channel?
	 *
	 * \return Returns true if the format of the image includes an alpha channel.
	 */
	LSE_INLINE LSBOOL LSE_CALL CImage::HasAlpha() const {
		return CImageLib::GetComponentBits( GetFormat(), LSI_PC_A ) != 0;
	}

	/**
	 * Gets the amount of error between 2 colors.
	 *
	 * \param _bColor0 Color 1.
	 * \param _bColor1 Color 2.
	 * \param _doOptions Weights to use for the error calculation.
	 * \return Returns the amount of error between 2 colors.
	 */
	LSE_INLINE LSFLOAT LSE_CALL CImage::GetError( const LSI_BLOCK &_bColor0, const LSI_BLOCK &_bColor1, const LSI_DXT_OPTIONS &_doOptions ) {
		/*LSFLOAT fR0 = _bColor0.s.fR * _bColor0.s.fR;//::powf( _bColor0.s.fR, 2.2f );
		LSFLOAT fR1 = _bColor1.s.fR * _bColor1.s.fR;//::powf( _bColor1.s.fR, 2.2f );

		LSFLOAT fG0 = _bColor0.s.fG * _bColor0.s.fG;//::powf( _bColor0.s.fG, 2.2f );
		LSFLOAT fG1 = _bColor1.s.fG * _bColor1.s.fG;//::powf( _bColor1.s.fG, 2.2f );

		LSFLOAT fB0 = _bColor0.s.fB * _bColor0.s.fB;//::powf( _bColor0.s.fB, 2.2f );
		LSFLOAT fB1 = _bColor1.s.fB * _bColor1.s.fB;//::powf( _bColor1.s.fB, 2.2f );

		return (fR0 - fR1) * (fR0 - fR1) * LSI_R_WEIGHT +
			(fG0 - fG1) * (fG0 - fG1) * LSI_G_WEIGHT +
			(fB0 - fB1) * (fB0 - fB1) * LSI_B_WEIGHT;*/
		return (_bColor0.s.fR - _bColor1.s.fR) * (_bColor0.s.fR - _bColor1.s.fR) * _doOptions.fRedWeight +
			(_bColor0.s.fG - _bColor1.s.fG) * (_bColor0.s.fG - _bColor1.s.fG) * _doOptions.fGreenWeight +
			(_bColor0.s.fB - _bColor1.s.fB) * (_bColor0.s.fB - _bColor1.s.fB) * _doOptions.fBlueWeight;
	}

	/**
	 * Gets the amount of error of an entire block between 1 channel of 2 colors.
	 *
	 * \param _ui32Channel The channel for which to determine the amount of error.
	 * \param _bBlock The input block of 4-by-4 texels.
	 * \param _bMax The max color.
	 * \param _bMin The min color.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _bOmitLast If true, 3 colors are returned instead of 4.
	 * \return Returns the amount of error among the block given the 2 end colors.
	 */
	LSE_INLINE LSFLOAT LSE_CALL CImage::GetError( LSUINT32 _ui32Channel,
		const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
		LSUINT32 _ui32Width, LSUINT32 _ui32Height, LSBOOL _bOmitLast ) {
		LSFLOAT fError = 0.0f;

		LSI_BLOCK bColors[4];
		Get4Colors( _bMax, _bMin, bColors, _bOmitLast );
		for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				LSFLOAT fChannelError = 10.0f;
				for ( LSUINT32 I = _bOmitLast ? 3 : 4; I--; ) {
					LSFLOAT fThisError = _bBlock[Y][X].fValues[_ui32Channel] - bColors[I].fValues[_ui32Channel];
					fChannelError = CStd::Min( fThisError, fChannelError );
				}
				fError += fChannelError;
			}
		}
		return fError * fError;
	}

	/**
	 * Gets the amount of error of a single color channel between 4 block colors.
	 *
	 * \param _ui32Channel The channel for which to determine the amount of error.
	 * \param _bBlocks The input array of 4 colors.
	 * \param _bSrc The color to test against the array of blocks to find the least amount of error.
	 * \param _ui32Index On return, this holds the index of the color that was the closest match.
	 * \param _bOmitLast If true, 3 colors are checked instead of 4.
	 * \return Returns the signed error between the input color and the array of colors.
	 */
	LSE_INLINE LSFLOAT LSE_CALL CImage::GetError( LSUINT32 _ui32Channel,
		const LSI_BLOCK _bBlocks[4], const LSI_BLOCK &_bSrc, LSUINT32 &_ui32Index, LSBOOL _bOmitLast ) {
		return GetError( _ui32Channel,
			_bBlocks, _bSrc.fValues[_ui32Channel], _ui32Index, _bOmitLast );
	}

	/**
	 * Gets the amount of error of a single color channel between 4 block colors.
	 *
	 * \param _ui32Channel The channel for which to determine the amount of error.
	 * \param _bBlocks The input array of 4 colors.
	 * \param _fValue The value to test against the array of blocks to find the least amount of error.
	 * \param _ui32Index On return, this holds the index of the color that was the closest match.
	 * \param _bOmitLast If true, 3 colors are checked instead of 4.
	 * \return Returns the signed error between the input color and the array of colors.
	 */
	LSE_INLINE LSFLOAT LSE_CALL CImage::GetError( LSUINT32 _ui32Channel,
		const LSI_BLOCK _bBlocks[4], LSFLOAT _fValue, LSUINT32 &_ui32Index, LSBOOL _bOmitLast ) {
		LSFLOAT fMinError = 2.0f;
		for ( LSUINT32 I = _bOmitLast ? 3 : 4; I--; ) {
			LSFLOAT fError = _bBlocks[I].fValues[_ui32Channel] - _fValue;
			if ( CMathLib::Abs( fError ) < CMathLib::Abs( fMinError ) ) {
				fMinError = fError;
				_ui32Index = I;
				if ( fError == 0.0f ) { break; }
			}
		}
		return fMinError;
	}

	/**
	 * Converts a block texel to a 16-bit R5 G6 B5 value.
	 *
	 * \param _bColor The color to convert.
	 * \return Returns the converted 16-bit color.
	 */
	LSE_INLINE LSUINT16 LSE_CALL CImage::ConvertBlockTexelTo16Bit( const LSI_BLOCK &_bColor ) {
		LSUINT16 ui16Ret = static_cast<LSUINT16>(static_cast<LSUINT16>(CStd::Clamp( _bColor.s.fR + (1.0f / 31.0f * 0.5f), 0.0f, 1.0f ) * 31.0f) << 11);
		ui16Ret |= static_cast<LSUINT16>(CStd::Clamp( _bColor.s.fG + (1.0f / 63.0f * 0.5f), 0.0f, 1.0f ) * 63.0f) << 5;
		ui16Ret |= static_cast<LSUINT16>(CStd::Clamp( _bColor.s.fB + (1.0f / 31.0f * 0.5f), 0.0f, 1.0f ) * 31.0f);
		return ui16Ret;
	}

	/**
	 * Converts a 16-bit R5 G6 B5 value into a block texel.
	 *
	 * \param _ui16Color The value to convert.
	 * \return Returns the converted block texel.
	 */
	LSE_INLINE CImage::LSI_BLOCK LSE_CALL CImage::Convert16BitToBlockTexel( LSUINT16 _ui16Color ) {
		LSI_BLOCK bColor;
		bColor.s.fR = (_ui16Color >> 11) * (1.0f / 31.0f);
		bColor.s.fG = ((_ui16Color >> 5) & 0x3F) * (1.0f / 63.0f);
		bColor.s.fB = (_ui16Color & 0x1F) * (1.0f / 31.0f);
		bColor.s.fA = 1.0f;
		return bColor;
	}

	/**
	 * Offsets a component in a 16-bit R5 G6 B5 color by a given amount.  If false is returned, the component
	 *	can't be offset by the given amount.
	 *
	 * \param _ui32Channel The channel on the color to modify.
	 * \param _i32Amount The amount by which to modify the color.
	 * \param _ui16Color The color to modify.
	 * \return Returns true if the offset was applied.
	 */
	LSE_INLINE LSBOOL LSE_CALL CImage::Offset16BitColorChannel( LSUINT32 _ui32Channel, LSINT32 _i32Amount, LSUINT16 &_ui16Color ) {
		static const LSUINT32 ui32Shifts[4] = {
			11,			// Red.
			5,			// Green.
			0,			// Blue.
			0,			// Alpha.
		};
		static const LSUINT32 ui32Bits[4] = {
			5,			// Red.
			6,			// Green.
			5,			// Blue.
			0,			// Alpha.
		};
		static const LSUINT32 ui32Masks[4] = {
			static_cast<LSUINT32>((1 << ui32Bits[0]) - 1),			// Red.
			static_cast<LSUINT32>((1 << ui32Bits[1]) - 1),			// Green.
			static_cast<LSUINT32>((1 << ui32Bits[2]) - 1),			// Blue.
			static_cast<LSUINT32>((1 << ui32Bits[3]) - 1),			// Alpha.
		};
		// Get the modified min.
		LSUINT32 ui32Mask = ui32Masks[_ui32Channel];
		LSUINT32 ui32Min = (_ui16Color >> ui32Shifts[_ui32Channel]) & ui32Mask;
		if ( static_cast<LSINT32>(ui32Min) + _i32Amount < 0 ||
			static_cast<LSINT32>(ui32Min) + _i32Amount > static_cast<LSINT32>(ui32Mask) ) {
			// Modifying by this amount would put the min out of its valid range.
			return false;
		}
		ui32Min += static_cast<LSUINT32>(_i32Amount);
		_ui16Color = static_cast<LSUINT16>(_ui16Color & ~(ui32Mask << ui32Shifts[_ui32Channel]));
		_ui16Color |= ui32Min << ui32Shifts[_ui32Channel];
		return true;
	}

	/**
	 * Rounds the given value off given a resolution for rounding.
	 *
	 * \param _fValue The value to round.
	 * \param _fRes The resolution of the rounding.
	 * \return Returns the rounded value.
	 */
	LSE_INLINE LSFLOAT LSE_CALL CImage::Round( LSFLOAT _fValue, LSFLOAT _fRes ) {
		//return static_cast<LSUINT32>(_fValue / _fRes + 0.5f) * _fRes;
		return ::floorf( _fValue / _fRes + 0.5f ) * _fRes;
	}

	/**
	 * Rounds off the colors in a block.
	 *
	 * \param _bColor The color to round.
	 */
	LSE_INLINE LSVOID LSE_CALL CImage::RoundOffBlock( LSI_BLOCK &_bColor ) {
		//_bColor = Convert16BitToBlockTexel( ConvertBlockTexelTo16Bit( _bColor ) );
		_bColor.s.fR = Round( _bColor.s.fR, 1.0f / 31.0f );
		_bColor.s.fG = Round( _bColor.s.fG, 1.0f / 63.0f );
		_bColor.s.fB = Round( _bColor.s.fB, 1.0f / 31.0f );
	}

}	// namespace lsi

#endif	// __LSI_IMAGE_H__
