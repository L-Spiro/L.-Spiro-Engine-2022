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
#include "../Kernel/LSIKernel.h"
#include "../Ktx/LSIKtx.h"
#include "../Palette/LSIPaletteDatabase.h"
#include "../Resampler/LSIResampler.h"
#include "Map/LSTLLinearMap.h"
#include "Map/LSTLStaticMap.h"
#include "Misc/LSCMisc.h"
#include "Vector/LSMVector4F.h"
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
		LSI_F_LANCZOS2_FILTER,						/**< A Lanczos filter with 2 sampling points. */
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
		/** Compression types. */
		enum LSI_COMPRESSIONS {
			LSI_C_RAWCOMPRESSION,									/**< The texel data is simply compressed without being rearranged. */
			LSI_C_CHANNELCOMPRESSION,								/**< Channels are stripped and compressed together. */
			LSI_C_RLECOMPRESSION,									/**< Per-channel RLE with compression. */
			LSI_C_INDEXCOMPRESSION,									/**< Indexed data with compression. */
			LSI_C_DTXCOMPRESSION,									/**< DTX compression. */
		};

		/** Channel access. */
		enum LSI_CHANNEL_ACCESS {
			LSI_CA_AVERAGE							= 4,			/**< Average of the RGB channels. */
			LSI_CA_WEIGHTED_AVERAGE					= 5,			/**< Weighted average of the RGB channels using LSI_*_WEIGHT. */
			LSI_CA_MAX								= 6,			/**< Maximum between the RGB channels. */
		};


		// == Types.
		/** A table entry for single-color DXT values. */
		typedef struct LSI_DXT_TABLE_4 {
			/** The 4 colors. */
			float									fColors[4];
		} * LPLSI_DXT_TABLE_4, * const LPCLSI_DXT_TABLE_4;

		/** Options for creating DXT file images. */
		typedef struct LSI_DXT_OPTIONS {
			/** Alpha cut-off for DXT1 images. */
			float									fAlphaThresh;
			/** Red weight. */
			float									fRedWeight;
			/** Green weight. */
			float									fGreenWeight;
			/** Blue weight. */
			float									fBlueWeight;
		} * LPLSI_DXT_OPTIONS, * const LPCLSI_DXT_OPTIONS;

		/** Options for creating ETC file images. */
		typedef struct LSI_ETC_OPTIONS {
			/** Speed. */
			uint32_t								ui32Speed;		// SPEED_SLOW, SPEED_FAST, SPEED_MEDIUM

			/** Error metric. */
			uint32_t								ui32Error;		// METRIC_PERCEPTUAL, METRIC_NONPERCEPTUAL

			/** Codec. */
			uint32_t								ui32Codec;		// CODEC_ETC, CODEC_ETC2

			/** Format. */
			uint32_t								ui32Format;		// ETC1_RGB_NO_MIPMAPS,ETC2PACKAGE_RGB_NO_MIPMAPS,ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD,ETC2PACKAGE_RGBA_NO_MIPMAPS,ETC2PACKAGE_RGBA1_NO_MIPMAPS,ETC2PACKAGE_R_NO_MIPMAPS,ETC2PACKAGE_RG_NO_MIPMAPS,ETC2PACKAGE_R_SIGNED_NO_MIPMAPS,ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS,ETC2PACKAGE_sRGB_NO_MIPMAPS,ETC2PACKAGE_sRGBA_NO_MIPMAPS,ETC2PACKAGE_sRGBA1_NO_MIPMAPS
		} * LPLSI_ETC_OPTIONS, * const LPCLSI_ETC_OPTIONS;

		/** KTX 1 OpenGL values. */
		typedef struct LSI_KTX1_VALUES {
			/** The OpenGL type. */
			LSI_KTX_TYPE							ktGlType;
			/** The OpenGL format. */
			LSI_KTX_FORMAT							kfGlFormat;
			/** The OpenGL internal format. */
			LSI_KTX_INTERNAL_FORMAT					kifGlInternalFormat;
			/** The OpenGL internal base format. */
			LSI_KTX_BASE_INTERNAL_FORMAT			kbifGlInternalBaseFormat;
		} * LPLSI_KTX1_VALUES, * const LPCLSI_KTX1_VALUES;

		/** KTX 2 Vulkan values. */
		typedef struct LSI_KTX2_VALUES {
			/** The Vulkan format. */
			LSI_VKFORMAT							vkFormat;
		} * LPLSI_KTX2_VALUES, * const LPCLSI_KTX2_VALUES;


		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _iOther The object to copy.
		 * \return Returns the copied object.
		 */
		CImage & LSE_CALL							operator = ( const CImage &_iOther );

		/**
		 * Move-assignment operator.
		 *
		 * \param _iOther The object to copy.
		 * \return Returns the copied object.
		 */
		CImage & LSE_CALL							operator = ( CImage &&_iOther );


		// == Functions.
		/**
		 * Sets the allocator.  Causes the object to be fully reset.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator.
		 */
		void LSE_CALL								SetAllocator( CAllocator * _paAllocator );

		/**
		 * Reset the image entirely, leaving it ready to be used again for a new image.
		 */
		void LSE_CALL								Reset();

		/**
		 * Gets the image width.
		 *
		 * \return Returns the image width.
		 */
		LSE_INLINE uint32_t LSE_CALL				GetWidth() const;

		/**
		 * Gets the image height.
		 *
		 * \return Returns the image height.
		 */
		LSE_INLINE uint32_t LSE_CALL				GetHeight() const;

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
		LSE_INLINE const void * LSE_CALL			GetBufferData() const;

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
		LSE_INLINE uint32_t LSE_CALL				TotalMipLevels() const;

		/**
		 * Gets a mipmap level such that 0 is the main image buffer and the remaining are from the mipmap chain.
		 *
		 * \param _ui32Level Level of the buffer to get.
		 * \return Returns a mipmap image buffer, such that index 0 is the main image buffer.
		 */
		LSE_INLINE const CImageLib::CTexelBuffer & LSE_CALL
													GetMipMapBuffers( uint32_t _ui32Level ) const;

		/**
		 * Creates a mipmap chain up to the given maximum level or 0 to go all the way down to 1x1.
		 *
		 * \param _ui32Levels Maximum levels to generate or 0 to make a full mipmap chain.  The number includes the
		 *	base image, so a value of 1 will not create more mipmap levels beyond the main image.
		 * \param _fFilter The filter to use for resampling.
		 * \param _amAddressMode The texture address mode to use for resampling.
		 * \return Returns true if all mipmaps were created.
		 */
		LSBOOL LSE_CALL								GenerateMipMaps( uint32_t _ui32Levels = 0, LSI_FILTER _fFilter = LSI_F_KAISER_FILTER,
			CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT );

		/**
		 * Create an image from a file already loaded to memory.
		 *
		 * \param _pui8FileData Pointer to the loaded file.
		 * \param _ui32DataLen Length of the loaded file data.
		 * \param _ppdPalettes An optional palette database.
		 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
		 */
		LSBOOL LSE_CALL								LoadFileFromMemory( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CPaletteDatabase * _ppdPalettes = NULL );

		/**
		 * Create an image from a file.  Path must be in UTF-8 format.
		 *
		 * \param _pcFile UTF-8-formatted string representing the path to the file to load.
		 * \param _ppdPalettes An optional palette database.
		 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
		 */
		LSBOOL LSE_CALL								LoadFile( const char * _pcFile, const CPaletteDatabase * _ppdPalettes = NULL );

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
		LSBOOL LSE_CALL								CreateBlank( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t _ui32MipMaps = 1, LSI_GAMMA _gGamma = LSI_G_LINEAR );

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
		 * Creates a blank KTX 1 image.
		 *
		 * \param _kifFormat Format of the image to create.
		 * \param _ui32Width Width of the image to create.
		 * \param _ui32Height Height of the image to create.
		 * \param _ui32MipMaps Number of mipmaps to generate, including the top level.  Specify 0 to generate a full chain.
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								CreateBlankKtx1( LSI_KTX_INTERNAL_FORMAT _kifFormat, uint32_t _ui32Width, uint32_t _ui32Height,
			uint32_t _ui32MipMaps = 1 );

		/**
		 * Gets a texel in the given format.  Integer formats only are accepted.
		 *
		 * \param _pfFormat Form in which to return the texel.  Function is fast if the format matches the format of the image.
		 *	Must be an integer format.
		 * \param _ui32X X location of the texel to get.
		 * \param _ui32Y Y location of the texel to get.
		 * \return Returns the texel in the requested format.
		 */
		LSE_INLINE uint64_t LSE_CALL				GetTexelAt( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32X, uint32_t _ui32Y ) const;

		/**
		 * Sets a texel at a given location.  Input format can be any valid format.
		 *
		 * \param _ui64Texel The texel value to set at the given location.
		 * \param _pfFormat Format of the incoming texel value.
		 * \param _ui32X X coordinate of the texel to modify.
		 * \param _ui32Y Y coordinate of the texel to modify.
		 */
		LSE_INLINE void LSE_CALL					SetTexelAt( uint64_t _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32X, uint32_t _ui32Y );

		/**
		 * Sets a texel at a given location.  Input format can be any valid format.
		 *
		 * \param _ui64Texel The texel value to set at the given location.
		 * \param _pfFormat Format of the incoming texel value.
		 * \param _ui32X X coordinate of the texel to modify.
		 * \param _ui32Y Y coordinate of the texel to modify.
		 * \param _ui32MipLevel Mipmap level at which to set the texel.
		 */
		LSE_INLINE void LSE_CALL					SetTexelAt( uint64_t _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32MipLevel );

		/**
		 * Gets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to get.
		 * \param _ui32Y Y location of the texel to get.
		 * \param _pfReturn Must point to an array of at least 4 float's.  Upon return, this holds the requested texels.
		 */
		LSE_INLINE void LSE_CALL					GetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, float * _pfReturn ) const;

		/**
		 * Gets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to get.
		 * \param _ui32Y Y location of the texel to get.
		 * \param _pfReturn Must point to an array of at least 4 float's.  Upon return, this holds the requested texels.
		 * \param _ui32MipLevel Mipmap level from which to get the texel.
		 */
		LSE_INLINE void LSE_CALL					GetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, float * _pfReturn, uint32_t _ui32MipLevel ) const;

		/**
		 * Sets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to set.
		 * \param _ui32Y Y location of the texel to set.
		 * \param _pfValue Must point to an array of at least 4 float's which will be used to set the texel at the given coordinates.
		 */
		LSE_INLINE void LSE_CALL					SetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, const float * _pfValue );

		/**
		 * Sets a texel in 32-byte-per-component floating-point format.
		 *
		 * \param _ui32X X location of the texel to set.
		 * \param _ui32Y Y location of the texel to set.
		 * \param _pfValue Must point to an array of at least 4 float's which will be used to set the texel at the given coordinates.
		 * \param _ui32MipLevel Mipmap level at which to set the texel.
		 */
		LSE_INLINE void LSE_CALL					SetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, const float * _pfValue, uint32_t _ui32MipLevel );

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
		void LSE_CALL								Flip();

		/**
		 * Sets the color space.
		 *
		 * \param _gColorSpace The color space to set.
		 */
		LSE_INLINE void LSE_FCALL					SetColorSpace( LSI_GAMMA _gColorSpace );

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
		LSBOOL LSE_CALL								IsChannelSetTo( LSI_PIXEL_COMPONENTS _pcComponent, float _fValue ) const;

		/**
		 * Swaps 2 channels.
		 *
		 * \param _pcCom0 Channel 0.
		 * \param _pcCom1 Channel 1.
		 */
		void LSE_CALL								SwapChannels( LSI_PIXEL_COMPONENTS _pcCom0, LSI_PIXEL_COMPONENTS _pcCom1 );

		/**
		 * Converts to a given format and return a new image in that format.
		 *
		 * \param _pfFormat Format to which to convert this image.
		 * \param _iDest Destination image.
		 * \param _pvParm Optional parameters to pass to the conversion routine.
		 * \param _gTargetGamma If not LSI_G_NONE, an sRGB <-> Linear conversion will take place if necessary.
		 */
		void LSE_CALL								ConvertToFormat( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDest, void * _pvParm = nullptr, LSI_GAMMA _gTargetGamma = LSI_G_NONE ) const;

		/**
		 * Converts this image in-place to a given format.
		 *
		 * \param _pfFormat Format to which to convert this image.
		 * \param _pvParm Optional parameters to pass to the conversion routine.
		 * \return Returns the converted image.
		 */
		CImage & LSE_CALL							ConvertToFormatInPlace( LSI_PIXEL_FORMAT _pfFormat, void * _pvParm = nullptr );

		/**
		 * Performs a copy of this image to another while optionally converting to either sRGB or linear, depending on this image's
		 *	color space and the desired target color space.  If a conversion is required then the source texture must support GetTexelAt() and the destination image must support SetTexelAt().
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */

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
		LSBOOL LSE_CALL								Resample( uint32_t _ui32NewWidth, uint32_t _ui32NewHeight, LSI_FILTER _fFilter, CImage &_iDest,
			float _fSrcGamma = 2.2f, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, float _fFilterScale = 1.0f ) const;

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
		LSBOOL LSE_CALL								Resample( uint32_t _ui32NewWidth, uint32_t _ui32NewHeight, LSI_FILTER _fFilter, CImageLib::CTexelBuffer &_tbDest,
			float _fSrcGamma = 2.2f, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, float _fFilterScale = 1.0f ) const;

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
		LSBOOL LSE_CALL								ResampleInPlace( uint32_t _ui32NewWidth, uint32_t _ui32NewHeight, LSI_FILTER _fFilter,
			float _fSrcGamma = 2.2f, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, float _fFilterScale = 1.0f );

		/**
		 * Calculates and returns the standard 32-bit CRC for this image.
		 *
		 * \return Returns the standard 32-bit CRC for this image.
		 */
		uint32_t LSE_CALL							GetCrc32() const;

		/**
		 * Calculates and returns the alternative 32-bit CRC for this image.
		 *
		 * \return Returns the alternative 32-bit CRC for this image.
		 */
		uint32_t LSE_CALL							GetCrc32Alt() const;

		/**
		 * Converts the image to a normal map using average color intensities from each channel of the image.
		 *	Normally images are greyscale when converted to normal maps.
		 *
		 * \param _fStr The strength of the normals.  Must not be 0.
		 * \param _bWrapX If true, the normal map is calculated with wrapping at the horizontal borders.
		 * \param _bWrapY If true, the normal map is calculated with wrapping at the vertical borders.
		 */
		void LSE_CALL								ConvertToNormalMap( float _fStr, LSBOOL _bWrapX = true, LSBOOL _bWrapY = true );

		/**
		 * Converts the image to a normal map given the kernels.
		 *
		 * \param _kU U kernel.
		 * \param _kV V kernel.
		 * \param _fStr Strength of the normals.  Must not be 0.
		 * \param _vWeights Height weights.
		 * \param _amAddressMode Address mode.
		 * \param _bUseMaxes If true, weights are ignored and the maximum component is used.
		 */
		void LSE_CALL								ConvertToNormalMap( const CKernel &_kU, const CKernel &_kV, float _fStr,
			const CVector4F &_vWeights, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, LSBOOL _bUseMaxes = false );

		/**
		 * Converts the image to DXT format.
		 *
		 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
		 * \param _iDst The destination image to which this image is converted.
		 * \param _pdoOptions Conversion options.
		 * \return Returns true if there are no resource problems.
		 */
		LSBOOL LSE_CALL								ConvertToDxt( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDst, const LSI_DXT_OPTIONS * _pdoOptions = NULL ) const;

		/**
		 * Converts the image to DXT format in-place.
		 *
		 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
		 * \param _pdoOptions Conversion options.
		 * \return Returns true if there are no resource problems.
		 */
		LSBOOL LSE_CALL								ConvertToDxtInPlace( LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS * _pdoOptions = NULL );

		/**
		 * Converts the image to KTX 1 format.
		 *
		 * \param _kifFormat The OpenGL internal format to which to convert.
		 * \param _iDst The destination image to which this image is converted.
		 * \param _pvParm Optional parameters to pass to the conversion routine.
		 * \return Returns true if there are no resource problems and the conversion is supported.
		 */
		LSBOOL LSE_CALL								ConvertToKtx1( LSI_KTX_INTERNAL_FORMAT _kifFormat, CImage &_iDst, void * _pvParm = nullptr ) const;

		/**
		 * Converts the image to KTX 1 format.
		 *
		 * \param _kifFormat The OpenGL internal format to which to convert.
		 * \param _pvParm Optional parameters to pass to the conversion routine.
		 * \return Returns true if there are no resource problems and the conversion is supported.
		 */
		LSBOOL LSE_CALL								ConvertToKtx1InPlace( LSI_KTX_INTERNAL_FORMAT _kifFormat, void * _pvParm = nullptr );

		/**
		 * Pre-multiplies the alpha in-place.  This should not be done more than once on a single image for correct results.
		 */
		void LSE_CALL								PreMultiplyAlpha();

		/**
		 * Adds a normal-map channel to the Z component.
		 */
		void LSE_CALL								AddNormalMapZ();

		/**
		 * Normalizes the image in-place.  This should be done for normal maps.
		 */
		void LSE_CALL								Normalize();

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

		/**
		 * Applies a given kernel to a single texel in the image.
		 *
		 * \param _ui32X X coordinate of the texel to which to apply the kernel.
		 * \param _ui32Y Y coordinate of the texel to which to apply the kernel.
		 * \param _kKernel Kernel to apply.
		 * \param _ui32Channel The channel to which to apply the kernel.
		 * \param _amAddressMode Address mode.
		 * \param _ui32Mip The mipmap level to sample.
		 * \return Returns the filtered value.
		 */
		float LSE_CALL								ApplyKernel( uint32_t _ui32X, uint32_t _ui32Y, const CKernel &_kKernel,
			uint32_t _ui32Channel, CResampler::LSI_ADDRESS_MODE _amAddressMode = CResampler::LSI_AM_REPEAT, uint32_t _ui32Mip = 0 ) const;

		/**
		 * Applies a given kernel to a single texel in the image.
		 *
		 * \param _ui32X X coordinate of the texel to which to apply the kernel.
		 * \param _ui32Y Y coordinate of the texel to which to apply the kernel.
		 * \param _kKernel Kernel to apply.
		 * \param _ui32Channel The channel to which to apply the kernel.
		 * \param _amAddressMode Address mode.
		 * \param _ui32Mip The mipmap level to sample.
		 * \param _ui32Width Width of the mipmap level to sample.
		 * \param _ui32Height Height of the mipmap level to sample.
		 * \return Returns the filtered value.
		 */
		float LSE_CALL								ApplyKernel( uint32_t _ui32X, uint32_t _ui32Y, const CKernel &_kKernel,
			uint32_t _ui32Channel, CResampler::LSI_ADDRESS_MODE _amAddressMode, uint32_t _ui32Mip,
			uint32_t _ui32Width, uint32_t _ui32Height ) const;

		/*
		 * Gets the KTX1 information.
		 *
		 * \return Returns the KTX1 information.
		 */
		LSE_INLINE const LSI_KTX1_VALUES & LSE_CALL	GetKtx1Values() const;


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
			uint32_t								ui32NewWidth;

			/** New height. */
			uint32_t								ui32NewHeight;

			/** Filter. */
			LSI_FILTER								fFilter;

			/** Destination texel buffer. */
			CImageLib::CTexelBuffer *				ptbDst;

			/** Gamma. */
			float									fGamma;

			/** Texture addressing mode. */
			CResampler::LSI_ADDRESS_MODE			amAddressMode;

			/** Gamma scale. */
			float									fGammaScale;
		} * LPLSI_MIPMAP_DATA, * const LPCLSI_MIPMAP_DATA;

		/** For sorting pixels.  Used when creating index tables. */
		typedef struct LSI_SORTED_PIXEL {
			/** The actual pixel value. */
			union LSI_COL {
				uint8_t								ui8Rgb[16];

				uint64_t							ui64Rgb64;
			}										u;

			/** Length of this pixel. */
			uint32_t								ui32Length;


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
				float								fR, fG, fB, fA;
			}										s;
			float									fValues[4];


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
			uint32_t								ui32SrcX;

			/** The block Y starting point in the source data. */
			uint32_t								ui32SrcY;

			/** The source image. */
			const CImage *							piSrcImage;

			/** Destination of the compressioned chunk. */
			uint8_t *								pui8Dest;

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
		uint32_t									m_ui32Width;

		/** The height of the image in texels. */
		uint32_t									m_ui32Height;

		/** The pixel format. */
		LSI_PIXEL_FORMAT							m_pfFormat;

		/** The width of rows, in pixels. */
		uint32_t									m_ui32RowWidth;

		/** Texel size in bytes. */
		uint32_t									m_ui32TexelSize;

		/** Gamma encoding. */
		LSI_GAMMA									m_gColorSpace;

		/** KTX 1 data. */
		LSI_KTX1_VALUES								m_kvKtx1;

		/** KTX 2 data.*/
		LSI_KTX2_VALUES								m_kvKtx2;


		// == Functions.
		/**
		 * Called after a successful load.  Sets various values based on the loaded data.
		 */
		void LSE_CALL								PostLoad();

		/**
		 * Compress this texture by grouping the reds, greens, blues, and alphas together
		 *	and compressing them separately.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressChannels( CMisc::LSC_BUFFER &_bBuffer, uint32_t _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress this texture by just performing a raw compression of the data.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressRaw( CMisc::LSC_BUFFER &_bBuffer, uint32_t _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress this texture by using RLE encoding.  The RLE data is also compressed.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressRle( CMisc::LSC_BUFFER &_bBuffer, uint32_t _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Compress this texture by creating indexed data.  The index table and indexed data are
		 *	then further compressed.
		 *
		 * \param _bBuffer The output buffer where the compressed data will be stored.
		 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL								CompressIndex( CMisc::LSC_BUFFER &_bBuffer, uint32_t _ui32MaxSize = LSE_MAXU32 );

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
		LSBOOL LSE_CALL								LoadCompressedChannels( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut );

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
		LSBOOL LSE_CALL								LoadCompressedRaw( uint8_t _ui8MipLevels, const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
			CVector<CImageLib::CTexelBuffer> &_vMipMaps );

		/**
		 * Loads an LSI image that was compressed via bitwise RLE compression.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadCompressedBitwiseRle( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut );

		/**
		 * Loads an LSI image that was compressed via index compression.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _tbOut Buffer where to store the decompressed image data.
		 * \return Returns true if there are no memory failures and the given data is valid. 
		 */
		LSBOOL LSE_CALL								LoadCompressedIndices( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut );

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
		LSBOOL LSE_CALL								LoadDxt( uint8_t _ui8MipLevels, const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
			CVector<CImageLib::CTexelBuffer> &_vMipMaps );

		/**
		 * Loads an LSI file from memory.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL								LoadLsi( const uint8_t * _pui8FileData, uint32_t _ui32DataLen );

		/**
		 * Loads an image via the FreeImage library.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL								LoadFreeImage( const uint8_t * _pui8FileData, uint32_t _ui32DataLen );

		/**
		 * A thread function for resampling from a source image to a destination texel buffer.
		 *
		 * \param _lpParameter Pointer to an LSI_MIPMAP_DATA structure.
		 * \return Returns the result of the call to Resample() on the created thread.
		 */
		static uint32_t LSH_CALL					ResampleThread( void * _lpParameter );

		/**
		 * Thread function for converting a block of texels to DXTn format.
		 *
		 * \param _lpParameter The thread information, stored as a pointer to a LSI_DXT_THREAD_DATA structure.
		 * \return Returns 0.
		 */
		static uint32_t LSH_CALL					DxtThread( void * _lpParameter );

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
	LSE_INLINE uint32_t LSE_CALL CImage::GetWidth() const {
		return m_ui32Width;
	}

	/**
	 * Gets the image height.
	 *
	 * \return Returns the image height.
	 */
	LSE_INLINE uint32_t LSE_CALL CImage::GetHeight() const {
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
	LSE_INLINE const void * LSE_CALL CImage::GetBufferData() const {
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
	LSE_INLINE const CImageLib::CTexelBuffer & LSE_CALL CImage::GetMipMapBuffers( uint32_t _ui32Level ) const {
		return _ui32Level == 0 ? GetBufferDataAsVector() : GetMipMapBuffers()[_ui32Level-1];
	}

	/**
	 * Gets the total mipmap levels including the base level.
	 *
	 * \return Returns the total mipmap levels including the base level.
	 */
	LSE_INLINE uint32_t LSE_CALL CImage::TotalMipLevels() const {
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
	LSE_INLINE uint64_t LSE_CALL CImage::GetTexelAt( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32X, uint32_t _ui32Y ) const {
		assert( _pfFormat < LSI_PF_TOTAL_INT );
		return CImageLib::IntegerTexelToIntegerTexel( GetFormat(), _pfFormat, (*reinterpret_cast<const uint64_t *>(&m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize])), LSI_G_NONE );
	}

	/**
	 * Sets a texel at a given location.  Input format can be any valid format.
	 *
	 * \param _ui64Texel The texel value to set at the given location.
	 * \param _pfFormat Format of the incoming texel value.
	 * \param _ui32X X coordinate of the texel to modify.
	 * \param _ui32Y Y coordinate of the texel to modify.
	 */
	LSE_INLINE void LSE_CALL CImage::SetTexelAt( uint64_t _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32X, uint32_t _ui32Y ) {
		assert( _pfFormat < LSI_PF_TOTAL_INT );
		uint64_t ui64Converted = CImageLib::IntegerTexelToIntegerTexel( _pfFormat, GetFormat(), _ui64Texel, LSI_G_NONE );

		static const uint64_t ui64Mask[] = {
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
		static const uint64_t ui32Sizes[] = {
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
		//uint32_t ui32Size = CImageLib::GetFormatSize( GetFormat() );

		uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(&m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize]);
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
	LSE_INLINE void LSE_CALL CImage::SetTexelAt( uint64_t _ui64Texel, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32MipLevel ) {
		assert( _pfFormat < LSI_PF_TOTAL_INT );
		uint64_t ui64Converted = CImageLib::IntegerTexelToIntegerTexel( _pfFormat, GetFormat(), _ui64Texel, LSI_G_NONE );

		static const uint64_t ui64Mask[] = {
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
		static const uint64_t ui32Sizes[] = {
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
		//uint32_t ui32Size = CImageLib::GetFormatSize( GetFormat() );
		uint32_t ui32RowWidth = CImageLib::GetRowSize( GetFormat(), CStd::Max<uint32_t>( GetWidth() >> _ui32MipLevel, 1 ) );
		uint8_t * pui8Buffer = const_cast<uint8_t *>(&GetMipMapBuffers( _ui32MipLevel )[0]);
		uint64_t * pui64Dst = reinterpret_cast<uint64_t *>(&pui8Buffer[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize]);
		(*pui64Dst) = ((*pui64Dst) & ui64Mask[m_ui32TexelSize]) | (ui64Converted & ui32Sizes[m_ui32TexelSize]);
	}

	/**
	 * Gets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to get.
	 * \param _ui32Y Y location of the texel to get.
	 * \param _pfReturn Must point to an array of at least 4 float's.  Upon return, this holds the requested texels.
	 */
	LSE_INLINE void LSE_CALL CImage::GetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, float * _pfReturn ) const {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			CImageLib::BatchIntegerTexelToFloat32Texel( GetFormat(), &m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize],
				_pfReturn, 1, LSI_G_NONE );
		}
		else if ( GetFormat() >= LSI_PF_R16G16B16A16F ) {
			CImageLib::BatchFloatTexelToFloatTexel( GetFormat(), &m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize],
				LSI_PF_R32G32B32A32F, _pfReturn, 1, LSI_G_NONE );
		}
	}

	/**
	 * Gets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to get.
	 * \param _ui32Y Y location of the texel to get.
	 * \param _pfReturn Must point to an array of at least 4 float's.  Upon return, this holds the requested texels.
	 * \param _ui32MipLevel Mipmap level from which to get the texel.
	 */
	LSE_INLINE void LSE_CALL CImage::GetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, float * _pfReturn, uint32_t _ui32MipLevel ) const {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		uint32_t ui32RowWidth = CImageLib::GetRowSize( GetFormat(), CStd::Max<uint32_t>( GetWidth() >> _ui32MipLevel, 1 ) );
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			CImageLib::BatchIntegerTexelToFloat32Texel( GetFormat(), &GetMipMapBuffers( _ui32MipLevel )[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize],
				_pfReturn, 1, LSI_G_NONE );
		}
		else if ( GetFormat() >= LSI_PF_R16G16B16A16F ) {
			CImageLib::BatchFloatTexelToFloatTexel( GetFormat(), &GetMipMapBuffers( _ui32MipLevel )[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize],
				LSI_PF_R32G32B32A32F, _pfReturn, 1, LSI_G_NONE );
		}
	}

	/**
	 * Sets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to set.
	 * \param _ui32Y Y location of the texel to set.
	 * \param _pfValue Must point to an array of at least 4 float's which will be used to set the texel at the given coordinates.
	 */
	LSE_INLINE void LSE_CALL CImage::SetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, const float * _pfValue ) {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			uint64_t ui64Temp = CImageLib::Float32TexelToIntegerTexel( GetFormat(), _pfValue, LSI_G_NONE );
			SetTexelAt( ui64Temp, GetFormat(), _ui32X, _ui32Y );
		}
		else {
			// Float-to-float.
			CImageLib::BatchFloatTexelToFloatTexel( LSI_PF_R32G32B32A32F, _pfValue,
				GetFormat(), &m_tbBuffer[_ui32Y*m_ui32RowWidth+_ui32X*m_ui32TexelSize], 1, LSI_G_NONE );
		}
	}

	/**
	 * Sets a texel in 32-byte-per-component floating-point format.
	 *
	 * \param _ui32X X location of the texel to set.
	 * \param _ui32Y Y location of the texel to set.
	 * \param _pfValue Must point to an array of at least 4 float's which will be used to set the texel at the given coordinates.
	 * \param _ui32MipLevel Mipmap level at which to set the texel.
	 */
	LSE_INLINE void LSE_CALL CImage::SetTexelAt( uint32_t _ui32X, uint32_t _ui32Y, const float * _pfValue, uint32_t _ui32MipLevel ) {
		if ( GetFormat() >= LSI_PF_TOTAL ) { return; }	// Can't do anything.
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			// Integer-to-float.
			uint64_t ui64Temp = CImageLib::Float32TexelToIntegerTexel( GetFormat(), _pfValue, LSI_G_NONE );
			SetTexelAt( ui64Temp, GetFormat(), _ui32X, _ui32Y, _ui32MipLevel );
		}
		else {
			uint32_t ui32RowWidth = CImageLib::GetRowSize( GetFormat(), CStd::Max<uint32_t>( GetWidth() >> _ui32MipLevel, 1 ) );
			uint8_t * pui8Buffer = const_cast<uint8_t *>(&GetMipMapBuffers( _ui32MipLevel )[0]);
			// Float-to-float.
			CImageLib::BatchFloatTexelToFloatTexel( LSI_PF_R32G32B32A32F, _pfValue,
				GetFormat(), &pui8Buffer[_ui32Y*ui32RowWidth+_ui32X*m_ui32TexelSize], 1, LSI_G_NONE );
		}
	}

	/**
	 * Sets the color space.
	 *
	 * \param _gColorSpace The color space to set.
	 */
	LSE_INLINE void LSE_FCALL CImage::SetColorSpace( LSI_GAMMA _gColorSpace ) {
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

	/*
	 * Gets the KTX1 information.
	 *
	 * \return Returns the KTX1 information.
	 */
	LSE_INLINE const CImage::LSI_KTX1_VALUES & LSE_CALL CImage::GetKtx1Values() const {
		return m_kvKtx1;
	}

}	// namespace lsi

#endif	// __LSI_IMAGE_H__
