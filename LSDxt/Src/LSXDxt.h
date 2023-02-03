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
 * Description: The main class for the DXT Compressor.
 */


#ifndef __LSX_DXT_H__
#define __LSX_DXT_H__

// Primary headers.
#include "LSAMemLib.h"
using namespace ::lsa;

//#include "LSCCompressionLib.h"
//using namespace ::lsc;

#include "LSFFileLib.h"
using namespace ::lsf;

#include "LSIImageLib.h"
using namespace ::lsi;

#include "LSSTDStandardLib.h"
using namespace ::lsstd;

#include "LSTLib.h"
using namespace ::lstl;

// Secondary headers.
#include "FileStream/LSFFileStream.h"
#include "HighLevel/LSFFilesEx.h"
#include "Image/LSIImage.h"
#include "String/LSTLStringList.h"

namespace lsx {

	/**
	 * Class CDxt
	 * \brief The main class for the DXT Compressor.
	 *
	 * Description: The main class for the DXT Compressor.
	 */
	class CDxt {
	public :
		// == Enumerations.
		/** Image formats. */
		enum LSX_IMAGE_FORMATS {
			LSX_IF_DXT1C,												/**< DXT1, no alpha. */
			LSX_IF_DXT1A,												/**< DXT1, alpha. */
			LSX_IF_DXT2,												/**< DXT2. */
			LSX_IF_DXT3,												/**< DXT3. */
			LSX_IF_DXT4,												/**< DXT4. */
			LSX_IF_DXT5,												/**< DXT5. */
			LSX_IF_BC4,													/**< BC4. */
			LSX_IF_BC4S,												/**< BC4 Signed. */
			LSX_IF_BC5,													/**< BC5. */
			LSX_IF_BC5S,												/**< BC5 Signed. */
			LSX_IF_BC6H,												/**< BC6H. */
			LSX_IF_BC7,													/**< BC7. */
			LSX_IF_A8B8G8R8,											/**< A8B8G8R8. */
			LSX_IF_G16R16,												/**< G16R16. */
			LSX_IF_A2B10G10R10,											/**< A2B10G10R10. */
			LSX_IF_A1R5G5B5,											/**< A1R5G5B5. */
			LSX_IF_R5G6B5,												/**< R5G6B5. */
			LSX_IF_A8,													/**< A8. */
			LSX_IF_A8R8G8B8,											/**< A8R8G8B8. */
			LSX_IF_X8R8G8B8,											/**< X8R8G8B8. */
			LSX_IF_X8B8G8R8,											/**< X8B8G8R8. */
			LSX_IF_A2R10G10B10,											/**< A2R10G10B10. */
			LSX_IF_R8G8B8,												/**< R8G8B8. */
			LSX_IF_X1R5G5B5,											/**< X1R5G5B5. */
			LSX_IF_A4R4G4B4,											/**< A4R4G4B4. */
			LSX_IF_X4R4G4B4,											/**< X4R4G4B4. */
			LSX_IF_A8R3G3B2,											/**< A8R3G3B2. */
			LSX_IF_A8L8,												/**< A8L8. */
			LSX_IF_L16,													/**< L16. */
			LSX_IF_L8,													/**< L8. */
			LSX_IF_A4L4,												/**< A4L4. */
		};

		/** Quality settings. */
		enum LSX_QUALITY {
			LSX_Q_QUICK,												/**< Fastest compression. */
			LSX_Q_NORMAL,												/**< Normal quality. */
			LSX_Q_PRODUCTION,											/**< Production quality. */
			LSX_Q_BEST,													/**< Best quality. */
		};

		/** Power-of-2 rescale options. */
		enum LSX_PO2_OPTIONS {
			LSX_PO_NO_RESCALE,											/**< No power-of-2 rescaling. */
			LSX_PO_NEAREST,												/**< Rescaled to the nearest power-of-2 up or down. */
			LSX_PO_NEXT_LO,												/**< Rescaled to the nearest power-of-2 down. */
			LSX_PO_NEXT_HI,												/**< Rescaled to the nearest power-of-2 up. */
		};

		/** ETC types. */
		enum LSX_ETC_TYPES {
			LSX_ET_ETC1,												/**< ETC1. */
			LSX_ET_ETC2,												/**< ETC2. */
			LSX_ET_NONE,												/**< Not ETC. */
		};

		/** ETC format. */
		enum LSX_ETC_FORMAT {
			LSE_EF_R,													/**< R8. */
			LSE_EF_RG,													/**< RG8. */
			LSE_EF_SIGNED_R,											/**< Signed R8. */
			LSE_EF_SIGNED_RG,											/**< Signed RG8. */
			LSE_EF_RGB,													/**< RGB8. */
			LSE_EF_RGB_A1,												/**< RGB8A1. */
			LSE_EF_RGB_A8,												/**< RGB8A8. */
			LSE_EF_SRGB,												/**< sRGB RGB8. */
			LSE_EF_SRGB_A1,												/**< sRGB RGB8A1. */
			LSE_EF_SRGB_A8,												/**< sRGB RGB8A8. */
		};

		/** ETC error metric. */
		enum LSX_ETC_ERROR {
			LSX_E_PERCEPTUAL,											/**< Perceptual (nicest). */
			LSX_E_NONPERCEPTUAL,										/**< Nonperceptual (highest PSNR). */
		};

		/** PNG compression. */
		enum LSX_PNG_COMPRESSION {
			LSX_PC_BEST_SPEED,											/**< Use Zlib level 1 compression when saving PNG files. */
			LSX_PC_DEFAULT_COMPRESSION,									/**< Use Zlib level 6 compression when saving PNG files. */
			LSX_PC_BEST_COMPRESSION,									/**< Use Zlib level 9 compression when saving PNG files. */
			LSX_PC_NO_COMPRESSION,										/**< No Zlib compression when saving PNG files. */
		};


		// == Types.
		/** Options for compression. */
		typedef struct LSX_OPTIONS {
			/** List of input files in UTF-8 format. */
			CStringList							slInputs;

			/** List of output files in UTF-8 format. */
			CStringList							slOutputs;

			/** Output format. */
			LSX_IMAGE_FORMATS					ifOutFormat;

			/** Compression quality. */
			LSX_QUALITY							qQuality;

			/** Rescale width or 0 if not supplied. */
			uint32_t							ui32RescaleWidth;

			/** Rescale height or 0 if not supplied. */
			uint32_t							ui32RescaleHeight;

			/** Power-of-2 rescale options. */
			LSX_PO2_OPTIONS						poPo2Options;

			/** Relative rescale width. */
			float								fRelScaleWidth;

			/** Relative rescale height. */
			float								fRelScaleHeight;

			/** Rescale filter to use. */
			LSI_FILTER							fFilter;

			/** Mipmap filter to use. */
			LSI_FILTER							fMipFilter;

			/** Address mode. */
			CResampler::LSI_ADDRESS_MODE		amAddressMode;

			/** Clamp on the image width or 0 for no restrictions. */
			uint32_t							ui32WidthClamp;

			/** Clamp on the image height or 0 for no restrictions. */
			uint32_t							ui32HeightClamp;

			/** Relative clamp on the width or 0. */
			float								fRelWidthClamp;

			/** Relative clamp on the height or 0. */
			float								fRelHeightClamp;

			/** If true, mipmaps are generated. */
			LSBOOL								bMipMaps;

			/** Maximum number of mipmaps to generate. */
			uint32_t							ui32Mips;

			/** Image is in RGBe space. */
			LSBOOL								bRgbE;

			/** Flips the image vertically. */
			LSBOOL								bFlip;

			/** If true, R and B are swapped. */
			LSBOOL								bSwapRB;

			/** If true, mipmaps are normalized. */
			LSBOOL								bNormalizeMips;

			/** Normal-map creation filter size or 0 for none. */
			uint32_t							ui32NormalMapKernelSize;

			/** Normal-map channel.  One of CImageLib::LSI_PIXEL_COMPONENTS or CImage::LSI_CHANNEL_ACCESS. */
			uint32_t							ui32NormalMapChannel;

			/** Strength of the normal map. */
			float								fNormalMapStr;

			/** The red weighting. */
			float								fRedWeight;

			/** The green weighting. */
			float								fGreenWeight;

			/** The blue weighting. */
			float								fBlueWeight;

			/** Gamma during filtering. */
			float								fFilterGamma;

			/** Alpha cut-off. */
			uint8_t								ui8AlphaThresh;

			/** If true, alpha is ignored. */
			LSBOOL								bIgnoreAlpha;

			/** If true, the tool pauses after the conversion. */
			LSBOOL								bPause;

			/** If true, the conversion time is shown after the conversion. */
			LSBOOL								bShowTime;

			/** The KTX1 format, if any. */
			LSI_KTX_INTERNAL_FORMAT				kifFormat;

			/** The ETC type if any. */
			LSX_ETC_TYPES						etEtcType;

			/** The ETC format, if any. */
			LSX_ETC_FORMAT						efEtcFormat;

			/** The ETC error metric. */
			LSX_ETC_ERROR						eeErrorMetric;

			/** The BMP bit-depth. */
			uint8_t								ui8BmpBits;

			/** The PNG compression. */
			LSX_PNG_COMPRESSION					pcPngCompress;

			/** Interlaced PNG. */
			bool								bPngInterlaced;
		} * LPLSX_OPTIONS, * const LPCLSX_OPTIONS;

		/** A floating-point vector. */
		typedef struct LSX_VECTOR : public CVector3Base<float, LSX_VECTOR> {
			// == Various constructors.
			LSE_CALLCTOR						LSX_VECTOR( float _fX, float _fY, float _fZ );


		private :
			typedef CVector3Base<float, LSX_VECTOR>
												Parent;
		} * LPLSX_VECTOR, * const LPCLSX_VECTOR;

		/** In-memory file. */
		typedef CVector<uint8_t, uint32_t, 1024 * 1024 * 32>
												CMemFile;

		/** Conversion table for compressed formats. */
		struct LSX_FORMAT_TABLE {
			/** Image format by our enumerated value. */
			LSX_IMAGE_FORMATS					ifFormat;

			/** Associated L. Spiro Engine format. */
			LSI_PIXEL_FORMAT					pfEngineFormat;

			/** FourCC code. */
			uint32_t							ui32FourCc;
		};

		/** Conversion table for uncompressed formats. */
		struct LSX_UNCOMPRESSED_TABLE {
				/** Image format by our enumerated value. */
				LSX_IMAGE_FORMATS				ifFormat;

				/** The dwFlags value. */
				uint32_t						ui32Flags;

				/** The dwRGBBitCount value. */
				uint32_t						ui32RgbBitCount;

				/** The dwRBitMask value. */
				uint32_t						ui32RBitMask;

				/** The dwGBitMask value. */
				uint32_t						ui32GBitMask;

				/** The dwBBitMask value. */
				uint32_t						ui32BBitMask;

				/** Image dwABitMask value. */
				uint32_t						ui32ABitMask;
			};



		// == Functions.
		/**
		 * Executes the program.  Takes an LSX_OPTIONS structure on input and performs
		 *	all requested operations, ultimately generating the resulting .DDS file(s).
		 *
		 * \param _oOptions The input options for processing.
		 * \return Returns an error code determining the result of the operation.
		 */
		static LSSTD_ERRORS LSE_CALL			Process( const LSX_OPTIONS &_oOptions );

		/**
		 * Prints the KTX 1 format list for the Help file.
		 */
		static void LSE_CALL					PrintKtx1Help();

		/**
		 * Prints the KTX 1 format list for the parsing inputs.
		 */
		static void LSE_CALL					PrintKtx1Parse();

	protected :
		// == Members.
		/** Compressed table. */
		static const LSX_FORMAT_TABLE			m_ftTable[];

		/** Uncompressed table. */
		static const LSX_UNCOMPRESSED_TABLE		m_utTable[];


		// == Functions.
		/**
		 * Creates a DDS file.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateDds( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Creates a DDS header given options, the image to write, and the destination in-memory buffer.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the header.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \param _ui32Match Index of the compressed format data.
		 * \param _ui32UnMatch Index of the uncompressed format data.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateDdsHeader( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex, uint32_t _ui32Match, uint32_t _ui32UnMatch );

		/**
		 * Creates a KTX file.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateKtx1( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Creates a BMP file.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateBmp( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Creates a BMP file using FreeImage.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateBmp_FI( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Creates an ICO file.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateIco( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Creates a PNG file.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreatePng( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Creates a QOI file.
		 *
		 * \param _oOptions Conversion options.
		 * \param _iImage The image to save.
		 * \param _mfFile The in-memory file to which to write the file data.
		 * \param _ui32FileIndex Index of the file being converted.
		 * \return Returns an error code indicating successor failure.
		 */
		static LSSTD_ERRORS LSE_CALL			CreateQoi( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
			uint32_t _ui32FileIndex );

		/**
		 * Gets the actual mipmap count.
		 *
		 * \param _iImage The source image.
		 * \param _oOptions The options.
		 * \return Returns the total actual mipmap levels.
		 */
		static uint32_t LSE_CALL				GetActualMips( const CImage &_iImage, const LSX_OPTIONS &_oOptions );

		/**
		 * Converts a given KTX error into one of our error codes.
		 *
		 * \param _eError The KTX error code.
		 * \return Returns one of our error codes.
		 */
		static LSSTD_ERRORS LSE_CALL			KtxErrorToError( KTX_error_code _eError );

		/**
		 * Converts from our options to a CImage::LSI_DXT_OPTIONS structure.
		 *
		 * \param _oOptions The options from which to derive the values for the returned CImage::LSI_DXT_OPTIONS object.
		 * \return Returns an CImage::LSI_DXT_OPTIONS object created based off values set in _oOptions.
		 */
		static CImage::LSI_DXT_OPTIONS LSE_CALL	OptionsToDssOptions( const LSX_OPTIONS &_oOptions );

		/**
		 * Converts from our options to a CImage::LSI_ETC_OPTIONS structure.
		 *
		 * \param _oOptions The options from which to derive the values for the returned CImage::LSI_ETC_OPTIONS object.
		 * \return Returns an CImage::LSI_ETC_OPTIONS object created based off values set in _oOptions.
		 */
		static CImage::LSI_ETC_OPTIONS LSE_CALL	OptionsToEtcOptions( const LSX_OPTIONS &_oOptions );

		/**
		 * Returns true if the given KTX1 format is any of the DXT/BC textures (uses the LSI_DXT_OPTIONS structure).
		 *
		 * \param _kifFormat Thge format to check.
		 * \return Returns true if the given KTX1 format is any of the DXT/BC textures (uses the LSI_DXT_OPTIONS structure).
		 */
		static bool LSE_CALL					IsDxt( LSI_KTX_INTERNAL_FORMAT _kifFormat );

		/**
		 * Returns true if the given KTX1 format is any of the ETC textures (uses the LSI_ETC_OPTIONS structure).
		 *
		 * \param _kifFormat Thge format to check.
		 * \return Returns true if the given KTX1 format is any of the ETC textures (uses the LSI_ETC_OPTIONS structure).
		 */
		static bool LSE_CALL					IsEtc( LSI_KTX_INTERNAL_FORMAT _kifFormat );
	};

}	// namespace lsx

#endif	// __LSX_DXT_H__
