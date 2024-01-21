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

#include "LSXDxt.h"
#include "Bmp/LSIBmp.h"
#include "Dds/LSIDds.h"
#include "Etc/LSIEtc.h"
#include "FileStream/LSFFileStream.h"
#include "FreeImage.h"
#include "Palette/LSIPaletteDatabase.h"
#include "String/LSTLWString.h"
#include "Time/LSSTDTime.h"
#include "Vector/LSMVector3Base.h"
#include <iostream>


/**
 * The main entrypoint for this program.
 *
 * \param _i32Args Number of strings to which _pwcArgv points.
 * \param _pwcArgv Pointer to an array of strings composing the command line passed to this application.
 * \return Returns 0 in the case of success, non-zero otherwise.  The error returned will be one of the
 *	LSSTD_E_* enumerated values and can be printed via lsstd::PrintError().
 */
int32_t LSE_CCALL wmain( int32_t _i32Args, LSUTFX * _pwcArgv[] ) {
	--_i32Args;
	++_pwcArgv;

	if ( _i32Args == 0 ) {
		return LSSTD_E_INVALIDCALL;
	}

	CMemLib::Init( 0x10000, true );
	CFilesEx::InitFilesEx();
	CImageLib::InitImageLibrary();
	{
		/*lsx::CDxt::PrintKtx1Help();
		lsx::CDxt::PrintKtx1Parse();*/
#define return									CImageLib::ShutDownImageLibrary(); CFilesEx::ShutDownFilesEx(); CMemLib::Destroy(); return
#define LSX_ERROR( CODE )						oOptions.slInputs.Reset();						\
												oOptions.slOutputs.Reset();						\
												oOptions.slPalDir.Reset();						\
												CStd::PrintError( CODE );						\
												CStd::DebugPrintA( "\r\n" );					\
												if ( oOptions.bPause ) { ::system( "pause" ); }	\
												return CODE

		// Get the command-line options and fill out the options structure.
		lsx::CDxt::LSX_OPTIONS oOptions = {
			CStringList(),									// slInputs
			CStringList(),									// slOutputs
			CStringList(),									// slPalDir
			lsx::CDxt::LSX_IF_DXT3,							// ifOutFormat
			lsx::CDxt::LSX_Q_NORMAL,						// qQuality
			0,												// ui32RescaleWidth
			0,												// ui32RescaleHeight
			lsx::CDxt::LSX_PO_NO_RESCALE,					// poPo2Options
			1.0f,											// fRelScaleWidth
			1.0f,											// fRelScaleHeight
			LSI_F_BOX_FILTER,								// fFilter
			LSI_F_KAISER_FILTER,							// fMipFilter
			CResampler::LSI_AM_CLAMP,						// amAddressMode
			0,												// ui32WidthClamp
			0,												// ui32HeightClamp
			0.0f,											// fRelWidthClamp
			0.0f,											// fRelHeightClamp
			true,											// bMipMaps
			32UL,											// ui32Mips
			false,											// bRgbE
			false,											// bFlip
			false,											// bSwapRB
			false,											// bNormalizeMips
			0,												// ui32NormalMapKernelSize
			CImage::LSI_CA_MAX,								// ui32NormalMapChannel
			1.0f,											// fNormalMapStr
			LSI_R_WEIGHT,									// fRedWeight
			LSI_G_WEIGHT,									// fGreenWeight
			LSI_B_WEIGHT,									// fBlueWeight
			-2.2f,											// fFilterGamma
			0,												// ui8AlphaThresh
			false,											// bIgnoreAlpha
			false,											// bPremultiplyAlpha
			false,											// bPause
			false,											// bShowTime
			static_cast<LSI_KTX_INTERNAL_FORMAT>(0),		// kifFormat
			lsx::CDxt::LSX_ET_NONE,							// etEtcType
			lsx::CDxt::LSE_EF_RGB_A8,						// efEtcFormat
			lsx::CDxt::LSX_E_PERCEPTUAL,					// eeErrorMetric
			0,												// ui8BmpBits
			lsx::CDxt::LSX_PC_DEFAULT_COMPRESSION,			// pcPngCompress
			false,											// bPngInterlaced
		};

		uint32_t ui32CreateNormalMapArg = LSE_MAXU32;
		for ( int32_t I = 0; I < _i32Args; ++I ) {
#define LSX_VERIFY_INPUT( STRING, ARGS )		CStd::WStrICmp( &_pwcArgv[I][1], L ## #STRING ) == 0 && (I + (ARGS)) < _i32Args
			if ( _pwcArgv[I][0] == L'-' ) {
				// Input files.
				if ( LSX_VERIFY_INPUT( file, 1 ) ) {
					if ( !oOptions.slInputs.Push( CString::CStringFromUtfX( _pwcArgv[++I] ) ) ) {
						LSX_ERROR( LSSTD_E_OUTOFMEMORY );
					}
				}
				// Output files.
				else if ( LSX_VERIFY_INPUT( outfile, 1 ) ) {
					// Make sure the output list has 1 fewer entries than the input list.
					if ( oOptions.slOutputs.Length() >= oOptions.slInputs.Length() ) {
						// Too many outputs have already been submitted.
						CStd::DebugPrintA( "Too many outputs for the given number of inputs.\r\n" );
						LSX_ERROR( LSSTD_E_INVALIDCALL );
					}
					for ( uint32_t J = oOptions.slOutputs.Length(); oOptions.slOutputs.Length() < oOptions.slInputs.Length() - 1; ++J ) {
						// Make a string by copying the input string and changing the extension.
						char szPath[LSF_MAX_PATH];
						CStd::StrNCpy( szPath, oOptions.slInputs[I].CStr(), LSF_MAX_PATH );
						const char * pcExt = ".dds";
						if ( oOptions.kifFormat != static_cast<LSI_KTX_INTERNAL_FORMAT>(0) ) {
							pcExt = ".ktx";
						}
						CStd::StrNCat( CFileLib::NoExtension( szPath ), pcExt, LSF_MAX_PATH - CStd::StrLen( szPath ) );
						if ( !oOptions.slOutputs.Push( szPath ) ) {
							LSX_ERROR( LSSTD_E_OUTOFMEMORY );
						}
					}
					if ( !oOptions.slOutputs.Push( CString::CStringFromUtfX( _pwcArgv[++I] ) ) ) {
						LSX_ERROR( LSSTD_E_OUTOFMEMORY );
					}
				}
				// Palette directories.
				if ( LSX_VERIFY_INPUT( pal_dir, 1 ) || LSX_VERIFY_INPUT( paldir, 1 ) || LSX_VERIFY_INPUT( palette_dir, 1 ) ) {
					if ( !oOptions.slPalDir.Push( CString::CStringFromUtfX( _pwcArgv[++I] ) ) ) {
						LSX_ERROR( LSSTD_E_OUTOFMEMORY );
					}
					continue;
				}
				// Output format.
				if ( LSX_VERIFY_INPUT( dxt1c, 0 ) || LSX_VERIFY_INPUT( bc1, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_DXT1C;
					continue;
				}
				if ( LSX_VERIFY_INPUT( dxt1a, 0 ) || LSX_VERIFY_INPUT( bc1a, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_DXT1A;
					continue;
				}
				if ( LSX_VERIFY_INPUT( dxt2, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_DXT2;
					continue;
				}
				if ( LSX_VERIFY_INPUT( dxt3, 0 ) || LSX_VERIFY_INPUT( bc2, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_DXT3;
					continue;
				}
				if ( LSX_VERIFY_INPUT( dxt4, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_DXT4;
					continue;
				}
				if ( LSX_VERIFY_INPUT( dxt5, 0 ) || LSX_VERIFY_INPUT( bc3, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_DXT5;
					continue;
				}
				if ( LSX_VERIFY_INPUT( bc4, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_BC4;
					continue;
				}
				if ( LSX_VERIFY_INPUT( bc5, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_BC5;
					continue;
				}
				if ( LSX_VERIFY_INPUT( bc6, 0 ) || LSX_VERIFY_INPUT( bc6h, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_BC6H;
					continue;
				}
				if ( LSX_VERIFY_INPUT( bc7, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_BC7;
					continue;
				}
#define LSX_GET_IMAGE( TYPE )	if ( LSX_VERIFY_INPUT( TYPE, 0 ) ) {		\
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_ ## TYPE;		\
					continue;												\
				}
				LSX_GET_IMAGE( A8B8G8R8 )
				LSX_GET_IMAGE( G16R16 )
				LSX_GET_IMAGE( A2B10G10R10 )
				LSX_GET_IMAGE( A1R5G5B5 )
				LSX_GET_IMAGE( R5G6B5 )
				LSX_GET_IMAGE( A8 )
				LSX_GET_IMAGE( A8R8G8B8 )
				LSX_GET_IMAGE( X8R8G8B8 )
				LSX_GET_IMAGE( X8B8G8R8 )
				LSX_GET_IMAGE( A2R10G10B10 )
				LSX_GET_IMAGE( R8G8B8 )
				LSX_GET_IMAGE( X1R5G5B5 )
				LSX_GET_IMAGE( A4R4G4B4 )
				LSX_GET_IMAGE( X4R4G4B4 )
				LSX_GET_IMAGE( A8R3G3B2 )
				LSX_GET_IMAGE( A8L8 )
				LSX_GET_IMAGE( L16 )
				LSX_GET_IMAGE( L8 )
				LSX_GET_IMAGE( A4L4 )
#undef LSX_GET_IMAGE
				if ( LSX_VERIFY_INPUT( u1555, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_A1R5G5B5;
					continue;
				}
				if ( LSX_VERIFY_INPUT( u4444, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_A4R4G4B4;
					continue;
				}
				if ( LSX_VERIFY_INPUT( u565, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_R5G6B5;
					continue;
				}
				if ( LSX_VERIFY_INPUT( u8888, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_A8R8G8B8;
					continue;
				}
				if ( LSX_VERIFY_INPUT( u888, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_R8G8B8;
					continue;
				}
				if ( LSX_VERIFY_INPUT( u555, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_X1R5G5B5;
					continue;
				}
				if ( LSX_VERIFY_INPUT( L8, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_L8;
					continue;
				}
				if ( LSX_VERIFY_INPUT( A8, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_A8;
					continue;
				}
				if ( LSX_VERIFY_INPUT( A8L8, 0 ) ) {
					oOptions.ifOutFormat = lsx::CDxt::LSX_IF_A8L8;
					continue;
				}
				// Quality settings.
				if ( LSX_VERIFY_INPUT( quick, 0 ) ) {
					oOptions.qQuality = lsx::CDxt::LSX_Q_QUICK;
					continue;
				}
				if ( LSX_VERIFY_INPUT( quality_normal, 0 ) ) {
					oOptions.qQuality = lsx::CDxt::LSX_Q_NORMAL;
					continue;
				}
				if ( LSX_VERIFY_INPUT( quality_production, 0 ) ) {
					oOptions.qQuality = lsx::CDxt::LSX_Q_PRODUCTION;
					continue;
				}
				if ( LSX_VERIFY_INPUT( quality_highest, 0 ) ) {
					oOptions.qQuality = lsx::CDxt::LSX_Q_BEST;
					continue;
				}
				// ETCPACK "s"peed setting.
				if ( LSX_VERIFY_INPUT( s, 1 ) ) {
					++I;
					if ( CStd::WStrICmp( _pwcArgv[I], L"fast" ) == 0 ) {
						oOptions.qQuality = lsx::CDxt::LSX_Q_QUICK;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"slow" ) == 0 ) {
						oOptions.qQuality = lsx::CDxt::LSX_Q_BEST;
						continue;
					}
				}
				// ETCPACK "e"rror-metric setting.
				if ( LSX_VERIFY_INPUT( e, 1 ) ) {
					++I;
					if ( CStd::WStrICmp( _pwcArgv[I], L"perceptual" ) == 0 ) {
						oOptions.eeErrorMetric = lsx::CDxt::LSX_E_PERCEPTUAL;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"nonperceptual" ) == 0 ) {
						oOptions.eeErrorMetric = lsx::CDxt::LSX_E_NONPERCEPTUAL;
						continue;
					}
				}
				// Rescale filters/ETCPACK "f"ormat setting.
				if ( LSX_VERIFY_INPUT( f, 1 ) ) {
					++I;
					if ( CStd::WStrICmp( _pwcArgv[I], L"box" ) == 0 ) {
						oOptions.fFilter = LSI_F_BOX_FILTER;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"quadratic" ) == 0 ) {
						oOptions.fFilter = LSI_F_QUADRATIC_FILTER;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"bspline" ) == 0 ) {
						oOptions.fFilter = LSI_F_BSPLINE_FILTER;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"mitchell" ) == 0 ) {
						oOptions.fFilter = LSI_F_MITCHELL_FILTER;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"lanczos" ) == 0 ) {
						oOptions.fFilter = LSI_F_LANCZOS64_FILTER;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"kaiser" ) == 0 ) {
						oOptions.fFilter = LSI_F_KAISER_FILTER;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_R;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_RG;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R_signed" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_SIGNED_R;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG_signed" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_SIGNED_RG;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_RGB;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA1" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_RGB_A1;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA8" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_RGB_A8;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"sRGB" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_SRGB;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"sRGBA1" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_SRGB_A1;
						continue;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"sRGBA8" ) == 0 ) {
						oOptions.efEtcFormat = lsx::CDxt::LSE_EF_SRGB_A8;
						continue;
					}
				}
				// ETCPACK "c"ompression type setting.
				if ( LSX_VERIFY_INPUT( c, 1 ) ) {
					++I;
					if ( CStd::WStrICmp( _pwcArgv[I], L"etc1" ) == 0 ) {
						oOptions.etEtcType = lsx::CDxt::LSX_ET_ETC1;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"etc2" ) == 0 ) {
						oOptions.etEtcType = lsx::CDxt::LSX_ET_ETC2;
						continue;
					}
				}
				// BMP bit-depth.
				if ( LSX_VERIFY_INPUT( bmp, 1 ) ) {
					oOptions.ui8BmpBits = uint8_t( CStd::WtoI32( _pwcArgv[++I] ) );
					continue;
				}
				// PNG compression level.
				if ( LSX_VERIFY_INPUT( png, 1 ) ) {
					++I;
					if ( CStd::WStrICmp( _pwcArgv[I], L"fast" ) == 0 ) {
						oOptions.pcPngCompress = lsx::CDxt::LSX_PC_BEST_SPEED;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"small" ) == 0 ) {
						oOptions.pcPngCompress = lsx::CDxt::LSX_PC_BEST_COMPRESSION;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"none" ) == 0 ) {
						oOptions.pcPngCompress = lsx::CDxt::LSX_PC_NO_COMPRESSION;
						continue;
					}
				}
				// PNG interlace.
				if ( LSX_VERIFY_INPUT( png_interlaced, 0 ) ) {
					oOptions.bPngInterlaced = true;
					continue;
				}
				// Rescaling.
				if ( LSX_VERIFY_INPUT( prescale, 2 ) ) {
					oOptions.ui32RescaleWidth = CStd::WtoI32( _pwcArgv[++I] );
					oOptions.ui32RescaleHeight = CStd::WtoI32( _pwcArgv[++I] );
					continue;
				}
				// Rescaling by power-of-2.
				if ( LSX_VERIFY_INPUT( rescale, 1 ) ) {
					++I;
					if ( CStd::WStrICmp( _pwcArgv[I], L"nearest" ) == 0 ) {
						oOptions.poPo2Options = lsx::CDxt::LSX_PO_NEAREST;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"hi" ) == 0 ) {
						oOptions.poPo2Options = lsx::CDxt::LSX_PO_NEXT_HI;
						continue;
					}
					else if ( CStd::WStrICmp( _pwcArgv[I], L"lo" ) == 0 || CStd::WStrICmp( _pwcArgv[I], L"next_lo" ) == 0 ) {
						oOptions.poPo2Options = lsx::CDxt::LSX_PO_NEXT_LO;
						continue;
					}
					else {
						LSX_ERROR( LSSTD_E_INVALIDCALL );
					}
				}
				// Rescaling by ratio.
				if ( LSX_VERIFY_INPUT( rel_scale, 2 ) ) {
					oOptions.fRelScaleWidth = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
					oOptions.fRelScaleHeight = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleBox, 0 ) ) {
					oOptions.fFilter = LSI_F_BOX_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleTent, 0 ) ) {
					oOptions.fFilter = LSI_F_BILINEAR_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleQuadratic, 0 ) ) {
					oOptions.fFilter = LSI_F_QUADRATIC_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleKaiser, 0 ) ) {
					oOptions.fFilter = LSI_F_KAISER_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos2, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS2_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos3, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS3_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos4, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS4_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos6, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS6_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos8, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS8_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos12, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS12_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleLanczos64, 0 ) ) {
					oOptions.fFilter = LSI_F_LANCZOS64_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleMitchell, 0 ) ) {
					oOptions.fFilter = LSI_F_MITCHELL_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleCatrom, 0 ) ) {
					oOptions.fFilter = LSI_F_CATMULLROM_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleBSpline, 0 ) ) {
					oOptions.fFilter = LSI_F_BSPLINE_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleBlackman, 0 ) ) {
					oOptions.fFilter = LSI_F_BLACKMAN_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleGaussian, 0 ) ) {
					oOptions.fFilter = LSI_F_GAUSSIAN_FILTER;
					continue;
				}
				if ( LSX_VERIFY_INPUT( RescaleBell, 0 ) ) {
					oOptions.fFilter = LSI_F_BELL_FILTER;
					continue;
				}
				// Mipmap filters.
				else if ( LSX_VERIFY_INPUT( Box, 0 ) ) {
					oOptions.fMipFilter = LSI_F_BOX_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Tent, 0 ) ) {
					oOptions.fMipFilter = LSI_F_BILINEAR_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Quadratic, 0 ) ) {
					oOptions.fMipFilter = LSI_F_QUADRATIC_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Kaiser, 0 ) ) {
					oOptions.fMipFilter = LSI_F_KAISER_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos2, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS2_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos3, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS3_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos4, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS4_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos6, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS6_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos8, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS8_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos12, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS12_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Lanczos64, 0 ) ) {
					oOptions.fMipFilter = LSI_F_LANCZOS64_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Mitchell, 0 ) ) {
					oOptions.fMipFilter = LSI_F_MITCHELL_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Catrom, 0 ) ) {
					oOptions.fMipFilter = LSI_F_CATMULLROM_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( BSpline, 0 ) ) {
					oOptions.fMipFilter = LSI_F_BSPLINE_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Blackman, 0 ) ) {
					oOptions.fMipFilter = LSI_F_BLACKMAN_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Gaussian, 0 ) ) {
					oOptions.fMipFilter = LSI_F_GAUSSIAN_FILTER;
				}
				else if ( LSX_VERIFY_INPUT( Bell, 0 ) ) {
					oOptions.fMipFilter = LSI_F_BELL_FILTER;
				}
				// Clamping.
				else if ( LSX_VERIFY_INPUT( clamp, 2 ) ) {
					oOptions.ui32WidthClamp = CStd::WtoI32( _pwcArgv[++I] );
					oOptions.ui32HeightClamp = CStd::WtoI32( _pwcArgv[++I] );
				}
				// Scaled clamping.
				else if ( LSX_VERIFY_INPUT( clampScale, 2 ) ) {
					oOptions.fRelWidthClamp = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
					oOptions.fRelHeightClamp = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
				}
				// Mipmaps off.
				else if ( LSX_VERIFY_INPUT( nomipmap, 0 ) || LSX_VERIFY_INPUT( nomipmaps, 0 ) || LSX_VERIFY_INPUT( nomips, 0 ) ) {
					oOptions.bMipMaps = false;
				}
				// Number of mipmaps to generate.
				else if ( LSX_VERIFY_INPUT( nmips, 1 ) ) {
					oOptions.ui32Mips = CStd::WtoI32( _pwcArgv[++I] );
				}
				// RGBe mode.
				else if ( LSX_VERIFY_INPUT( rgbe, 0 ) ) {
					oOptions.bRgbE = true;
					oOptions.fFilterGamma = 0.0f;	// Already in linear mode so don't gamma-correct during filtering.
				}
				// Flipping.
				else if ( LSX_VERIFY_INPUT( flip, 0 ) ) {
					oOptions.bFlip = true;
				}
				// Swap R and B.
				else if ( LSX_VERIFY_INPUT( swap, 0 ) ) {
					oOptions.bSwapRB = true;
				}
				// Normalize mipmaps.
				else if ( (LSX_VERIFY_INPUT( norm, 0 )) || LSX_VERIFY_INPUT( normalize, 0 ) ) {
					oOptions.bNormalizeMips = true;
				}
				// Generate a normal map.
				else if ( (LSX_VERIFY_INPUT( n3x3, 0 )) ) {
					oOptions.ui32NormalMapKernelSize = 3;
					ui32CreateNormalMapArg = I;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( n5x5, 0 )) ) {
					oOptions.ui32NormalMapKernelSize = 5;
					ui32CreateNormalMapArg = I;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( n7x7, 0 )) ) {
					oOptions.ui32NormalMapKernelSize = 7;
					ui32CreateNormalMapArg = I;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( n9x9, 0 )) ) {
					oOptions.ui32NormalMapKernelSize = 9;
					ui32CreateNormalMapArg = I;
					oOptions.bNormalizeMips = true;
				}
				// Select the normal-map source channel.
				else if ( (LSX_VERIFY_INPUT( alpha, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = LSI_PC_A;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( rgb, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = CImage::LSI_CA_AVERAGE;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( red, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = LSI_PC_R;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( green, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = LSI_PC_G;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( blue, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = LSI_PC_B;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( max, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = CImage::LSI_CA_MAX;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( colorspace, 0 )) ) {
					/*if ( ui32CreateNormalMapArg != I - 1 ) {
					}*/
					oOptions.ui32NormalMapChannel = CImage::LSI_CA_WEIGHTED_AVERAGE;
					oOptions.bNormalizeMips = true;
				}
				else if ( (LSX_VERIFY_INPUT( scale, 1 )) ) {
					oOptions.fNormalMapStr = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
				}
				// Color weights.
				else if ( (LSX_VERIFY_INPUT( weight, 3 )) || (LSX_VERIFY_INPUT( weights, 3 )) ) {
					oOptions.fRedWeight = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
					oOptions.fGreenWeight = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
					oOptions.fBlueWeight = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
				}
				// Gamma.
				else if ( LSX_VERIFY_INPUT( gamma, 1 ) || LSX_VERIFY_INPUT( g, 1 ) ) {
					oOptions.fFilterGamma = static_cast<float>(CStd::WtoF( _pwcArgv[++I] ));
				}
				// Addressing mode.
				else if ( LSX_VERIFY_INPUT( clamp, 0 ) ) {
					oOptions.amAddressMode = CResampler::LSI_AM_CLAMP;
				}
				else if ( LSX_VERIFY_INPUT( repeat, 0 ) || LSX_VERIFY_INPUT( wrap, 0 ) ) {
					oOptions.amAddressMode = CResampler::LSI_AM_REPEAT;
				}
				else if ( LSX_VERIFY_INPUT( mirror, 0 ) || LSX_VERIFY_INPUT( reflect, 0 ) ) {
					oOptions.amAddressMode = CResampler::LSI_AM_MIRROR;
				}
				// Alpha threshold.
				else if ( LSX_VERIFY_INPUT( alpha_threshold, 1 ) || LSX_VERIFY_INPUT( alpha_thresh, 1 ) ) {
					oOptions.ui8AlphaThresh = static_cast<uint8_t>(CStd::Clamp<uint32_t>( CStd::WtoI32( _pwcArgv[++I] ), 0, 255UL ));
				}
				// Ignore alpha.
				else if ( LSX_VERIFY_INPUT( ignore_alpha, 0 ) || LSX_VERIFY_INPUT( ia, 0 ) ) {
					oOptions.bIgnoreAlpha = true;
				}
				// Pre-multiply alpha.
				else if ( LSX_VERIFY_INPUT( premultiply_alpha, 0 ) || LSX_VERIFY_INPUT( premult_alpha, 0 ) ) {
					oOptions.bPremultiplyAlpha = true;
				}
				// Pausing.
				else if ( LSX_VERIFY_INPUT( pause, 0 ) ) {
					oOptions.bPause = true;
				}
				// Conversion time.
				else if ( LSX_VERIFY_INPUT( time, 0 ) || LSX_VERIFY_INPUT( showtime, 0 ) ) {
					oOptions.bShowTime = true;
				}
				// KTX 1.
				else if ( LSX_VERIFY_INPUT( ktx1format, 1 ) ) {
					++I;
					bool bFound = false;
					if ( CStd::WStrICmp( _pwcArgv[I], L"R8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R8UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R8UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG8UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG8UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB8UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB8UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA8UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA8UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R8I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R8I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG8I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG8I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB8I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB8I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA8I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA8I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"SR8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_SR8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"SRG8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_SRG8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"SRGB8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_SRGB8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"SRGB8_ALPHA8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_SRGB8_ALPHA8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R16UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R16UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG16UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG16UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB16UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB16UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA16UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA16UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R16I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R16I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG16I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG16I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB16I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB16I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA16I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA16I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R16F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R16F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG16F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG16F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB16F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB16F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA16F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA16F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R32UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R32UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG32UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG32UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB32UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB32UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA32UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA32UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R32I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R32I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG32I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG32I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB32I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB32I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA32I" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA32I;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R32F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R32F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RG32F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RG32F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB32F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB32F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA32F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA32F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R3_G3_B2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R3_G3_B2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB5" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB5;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB565" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB565;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB10" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB10;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB12" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB12;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGBA12" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGBA12;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB5_A1" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB5_A1;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB10_A2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB10_A2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB10_A2UI" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB10_A2UI;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"R11F_G11F_B10F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_R11F_G11F_B10F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"RGB9_E5" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_RGB9_E5;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA8UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA8UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA8I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA8I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA12" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA12;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA16UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA16UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA16I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA16I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA16F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA16F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA32UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA32UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA32I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA32I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ALPHA32F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ALPHA32F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"SLUMINANCE8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_SLUMINANCE8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE8UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE8UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE8I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE8I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE12" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE12;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE32UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE32UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE32I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE32I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE32F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE32F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE4_ALPHA4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE4_ALPHA4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE6_ALPHA2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE6_ALPHA2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE8_ALPHA8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE8_ALPHA8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE8_ALPHA8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE8_ALPHA8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"SLUMINANCE8_ALPHA8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_SLUMINANCE8_ALPHA8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA8UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA8UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA8I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA8I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE12_ALPHA4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE12_ALPHA4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE12_ALPHA12" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE12_ALPHA12;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16_ALPHA16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16_ALPHA16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE16_ALPHA16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE16_ALPHA16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA16UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA16UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA16I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA16I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA16F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA16F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA32UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA32UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA32I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA32I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"LUMINANCE_ALPHA32F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_LUMINANCE_ALPHA32F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY8_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY8_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY8UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY8UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY8I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY8I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY12" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY12;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY16_SNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY16_SNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY16UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY16UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY16I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY16I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY16F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY16F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY32UI_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY32UI_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY32I_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY32I_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"INTENSITY32F_ARB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_INTENSITY32F_ARB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RED" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RED;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_ALPHA" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_ALPHA;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_LUMINANCE" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_LUMINANCE;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SLUMINANCE" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SLUMINANCE;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_LUMINANCE_ALPHA" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SLUMINANCE_ALPHA" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SLUMINANCE_ALPHA;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_INTENSITY" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_INTENSITY;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB_S3TC_DXT1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_S3TC_DXT1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_S3TC_DXT3_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_S3TC_DXT5_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_S3TC_DXT1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_LUMINANCE_LATC1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_LUMINANCE_LATC1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RED_RGTC1" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RED_RGTC1;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RG_RGTC2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RG_RGTC2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SIGNED_RED_RGTC1" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SIGNED_RED_RGTC1;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SIGNED_RG_RGTC2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SIGNED_RG_RGTC2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_BPTC_UNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_BPTC_UNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_BPTC_UNORM" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"ETC1_RGB8_OES" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_ETC1_RGB8_OES;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB8_ETC2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB8_ETC2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA8_ETC2_EAC" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA8_ETC2_EAC;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ETC2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ETC2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ETC2_EAC" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_R11_EAC" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_R11_EAC;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RG11_EAC" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RG11_EAC;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SIGNED_R11_EAC" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SIGNED_R11_EAC;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SIGNED_RG11_EAC" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SIGNED_RG11_EAC;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB_PVRTC_2BPPV1_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGB_PVRTC_4BPPV1_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_PVRTC_2BPPV1_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_PVRTC_4BPPV1_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_PVRTC_2BPPV2_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_PVRTC_4BPPV2_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_PVRTC_2BPPV1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_PVRTC_4BPPV1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_4x4_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_5x4_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_5x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_6x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_6x6_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_8x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_8x6_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_8x8_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_10x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_10x6_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_10x8_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_10x10_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_12x10_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_RGBA_ASTC_12x12_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH_COMPONENT16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH_COMPONENT16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH_COMPONENT24" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH_COMPONENT24;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH_COMPONENT32" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH_COMPONENT32;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH_COMPONENT32F" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH_COMPONENT32F;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH_COMPONENT32F_NV" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH_COMPONENT32F_NV;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"STENCIL_INDEX1" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_STENCIL_INDEX1;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"STENCIL_INDEX4" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_STENCIL_INDEX4;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"STENCIL_INDEX8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_STENCIL_INDEX8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"STENCIL_INDEX16" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_STENCIL_INDEX16;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH24_STENCIL8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH24_STENCIL8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH32F_STENCIL8" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH32F_STENCIL8;
						bFound = true;
					}
					if ( CStd::WStrICmp( _pwcArgv[I], L"DEPTH32F_STENCIL8_NV" ) == 0 ) {
						oOptions.kifFormat = lsi::LSI_KIF_GL_DEPTH32F_STENCIL8_NV;
						bFound = true;
					}
					if ( !bFound ) { LSX_ERROR( LSSTD_E_INVALIDCALL ); }
				}
			}
			else {
				LSX_ERROR( LSSTD_E_INVALIDCALL );
			}
#undef LSX_VERIFY_INPUT
		}

		// Ensure the same number of outputs as there are inputs.
		for ( uint32_t J = oOptions.slOutputs.Length(); oOptions.slOutputs.Length() < oOptions.slInputs.Length(); ++J ) {
			// Make a string by copying the input string and changing the extension.
			char szPath[LSF_MAX_PATH];
			CStd::StrNCpy( szPath, oOptions.slInputs[J].CStr(), LSF_MAX_PATH );
			CStd::StrNCat( CFileLib::NoExtension( szPath ), ".dds", LSF_MAX_PATH - CStd::StrLen( szPath ) );
			if ( !oOptions.slOutputs.Push( szPath ) ) {
				LSX_ERROR( LSSTD_E_OUTOFMEMORY );
			}
		}

		CTime tTime;
		LSSTD_ERRORS eError = lsx::CDxt::Process( oOptions );
		if ( eError != LSSTD_E_SUCCESS ) {
			LSX_ERROR( eError );
		}
		tTime.Update( false );
		char szPrintfMe[512];
		::sprintf( szPrintfMe, "Total time: %.13f seconds.\r\n", tTime.GetCurMicros() / 1000000.0 );
		CStd::DebugPrintA( szPrintfMe );
		if ( oOptions.bShowTime ) {
			::printf( "Total time: %.13f seconds.\r\n", tTime.GetCurMicros() / 1000000.0 );
		}
		if ( oOptions.bPause ) {
			::system( "pause" );
		}
	}

	return LSSTD_E_SUCCESS;
#undef LSX_ERROR
#undef return	// Hacky so sue me.
}


namespace lsx {

	const CDxt::LSX_FORMAT_TABLE CDxt::m_ftTable[] = {
		//ifFormat							pfEngineFormat		ui32FourCc
		{ LSX_IF_DXT1C,						LSI_PF_DXT1,		LSE_MAKEFOURCC( 'D', 'X', 'T', '1' ), },
		{ LSX_IF_DXT1A,						LSI_PF_DXT1,		LSE_MAKEFOURCC( 'D', 'X', 'T', '1' ), },
		{ LSX_IF_DXT2,						LSI_PF_DXT2,		LSE_MAKEFOURCC( 'D', 'X', 'T', '2' ), },
		{ LSX_IF_DXT3,						LSI_PF_DXT3,		LSE_MAKEFOURCC( 'D', 'X', 'T', '3' ), },
		{ LSX_IF_DXT4,						LSI_PF_DXT4,		LSE_MAKEFOURCC( 'D', 'X', 'T', '4' ), },
		{ LSX_IF_DXT5,						LSI_PF_DXT5,		LSE_MAKEFOURCC( 'D', 'X', 'T', '5' ), },
		{ LSX_IF_BC4,						LSI_PF_BC4,			LSE_MAKEFOURCC( 'A', 'T', 'I', '1' ), },
		{ LSX_IF_BC4S,						LSI_PF_BC4S,		LSE_MAKEFOURCC( 'B', 'C', '4', 'S' ), },
		{ LSX_IF_BC5,						LSI_PF_BC5,			LSE_MAKEFOURCC( 'A', 'T', 'I', '2' ), },
		{ LSX_IF_BC5S,						LSI_PF_BC5S,		LSE_MAKEFOURCC( 'B', 'C', '5', 'S' ), },
		{ LSX_IF_BC6H,						LSI_PF_BC6,			LSE_MAKEFOURCC( 'D', 'X', '1', '0' ), },
		{ LSX_IF_BC7,						LSI_PF_BC7,			LSE_MAKEFOURCC( 'D', 'X', '1', '0' ), },
	};

	const CDxt::LSX_UNCOMPRESSED_TABLE CDxt::m_utTable[] = {
			//ifFormat							ui32Flags			ui32RgbBitCount	ui32RBitMask	ui32GBitMask	ui32BBitMask	ui32ABitMask
			{ LSX_IF_A8B8G8R8,					LSI_DPFF_RGBA,		32UL,			0xFF,			0xFF00,			0xFF0000,		0xFF000000 },
			{ LSX_IF_G16R16,					LSI_DPFF_RGBA,		32UL,			0xFFFF,			0xFFFF0000,		0x00,			0x00 },
			{ LSX_IF_A2B10G10R10,				LSI_DPFF_RGBA,		32UL,			0x3FF,			0xFFC00,		0x3FF00000,		0xC0000000 },
			{ LSX_IF_A1R5G5B5,					LSI_DPFF_RGBA,		16UL,			0x7C00,			0x3E0,			0x1F,			0x8000 },
			{ LSX_IF_R5G6B5,					LSI_DPFF_RGB,		16UL,			0xF800,			0x7E0,			0x1F,			0x00 },
			{ LSX_IF_A8,						LSI_DPFF_ALPHA,		8UL,			0x00,			0x00,			0x00,			0xFF },
			{ LSX_IF_A8R8G8B8,					LSI_DPFF_RGBA,		32UL,			0xFF,			0xFF00,			0xFF0000,		0xFF000000 },
			{ LSX_IF_X8R8G8B8,					LSI_DPFF_RGB,		32UL,			0xFF,			0xFF00,			0xFF0000,		0x00 },
			{ LSX_IF_X8B8G8R8,					LSI_DPFF_RGB,		32UL,			0xFF,			0xFF00,			0xFF0000,		0x00 },
			{ LSX_IF_A2R10G10B10,				LSI_DPFF_RGBA,		32UL,			0x3FF00000,		0xFFC00,		0x3FF,			0xC0000000 },
			{ LSX_IF_R8G8B8,					LSI_DPFF_RGB,		24UL,			0xFF0000,		0xFF00,			0xFF,			0x00 },
			{ LSX_IF_X1R5G5B5,					LSI_DPFF_RGB,		16UL,			0x7C00,			0x3E0,			0x1F,			0x00 },
			{ LSX_IF_A4R4G4B4,					LSI_DPFF_RGBA,		16UL,			0xF00,			0xF0,			0xF,			0xF000 },
			{ LSX_IF_X4R4G4B4,					LSI_DPFF_RGB,		16UL,			0xF00,			0xF0,			0xF,			0x00 },
			{ LSX_IF_A8R3G3B2,					LSI_DPFF_RGBA,		16UL,			0xE0,			0x1C,			0x3,			0xFF00 },
			{ LSX_IF_A8L8,						LSI_DPFF_LUMINANCE,	16UL,			0xFF,			0x00,			0x00,			0xFF00 },
			{ LSX_IF_L16,						LSI_DPFF_LUMINANCE,	16UL,			0xFFFF,			0x00,			0x00,			0x00 },
			{ LSX_IF_L8,						LSI_DPFF_LUMINANCE,	8UL,			0xFF,			0x00,			0x00,			0x00 },
			{ LSX_IF_A4L4,						LSI_DPFF_LUMINANCE,	8UL,			0xF,			0x00,			0x00,			0xF0 },
		};

	// == Various constructors.
	LSE_CALLCTOR CDxt::LSX_VECTOR::LSX_VECTOR( float _fX, float _fY, float _fZ ) :
		Parent( _fX, _fY, _fZ ) {

	}

	// == Functions.
	/**
	 * Executes the program.  Takes an LSX_OPTIONS structure on input and performs
	 *	all requested operations, ultimately generating the resulting .DDS file(s).
	 *
	 * \param _oOptions The input options for processing.
	 * \return Returns an error code determining the result of the operation.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::Process( const LSX_OPTIONS &_oOptions ) {
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;
		CPaletteDatabase pdPalettes;
		// Load/find any palettes necessary.
		for ( uint32_t I = 0; I < _oOptions.slPalDir.Length(); ++I ) {
			if ( CFileLib::Exists( _oOptions.slPalDir[I].CStr() ) ) {
				if ( !pdPalettes.LoadPalette( _oOptions.slPalDir[I].CStr() ) ) {
					eError = LSSTD_E_PARTIALFAILURE;
					::printf( "Failed to load palette file %s.\r\n", _oOptions.slPalDir[I].CStr() );
					return eError;
				}
			}
			else {
				CStringList slTmp;
				CFilesEx::GetFilesInDir( CWString::FromUtf8( reinterpret_cast<const LSUTF8 *>(_oOptions.slPalDir[I].CStr()) ).CStr(), L"*.pal", false, slTmp );
				CFilesEx::GetFilesInDir( CWString::FromUtf8( reinterpret_cast<const LSUTF8 *>(_oOptions.slPalDir[I].CStr()) ).CStr(), L"*.ppl", false, slTmp );
				for ( uint32_t J = 0; J < slTmp.Length(); ++J ) {
					if ( !pdPalettes.LoadPalette( slTmp[J].CStr() ) ) {
						eError = LSSTD_E_PARTIALFAILURE;
						::printf( "Failed to load palette file %s.\r\n", slTmp[J].CStr() );
					}
				}
			}
		}

		// Process the files in order.
		for ( uint32_t I = 0; I < _oOptions.slInputs.Length(); ++I ) {
			// Load the image.
			CImage iImage;
			if ( !iImage.LoadFile( _oOptions.slInputs[I].CStr(), &pdPalettes ) ) {
				// We can continue to the next file.
				eError = LSSTD_E_PARTIALFAILURE;
				::printf( "Failed to open image %s.\r\n", _oOptions.slInputs[I].CStr() );
				continue;
			}

			// File loaded.

			// Configure the color space.
			// If it is sRGB then the color space was set by the file.
			iImage.SetColorSpace( _oOptions.bRgbE ? LSI_G_LINEAR :							// Forced to linear because of command line.
				(iImage.AutoColorSpace() ? iImage.GetColorSpace() :							// If set by the file, use that.
					(_oOptions.fFilterGamma == 0 ? LSI_G_LINEAR : LSI_G_sRGB)) );			// Use _oOptions.fFilterGamma as a hint.

			// Should it be flipped?
			if ( _oOptions.bFlip ) {
				iImage.Flip();
			}

			// Should the R and B be swapped?
			if ( _oOptions.bSwapRB ) {
				for ( uint32_t Y = iImage.GetHeight(); Y--; ) {
					for ( uint32_t X = iImage.GetWidth(); X--; ) {
						uint64_t ui64Color = iImage.GetTexelAt( LSI_PF_R16G16B16A16, X, Y );
						uint64_t ui64RedMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_R )) - 1ULL;
						uint64_t ui64Red = (ui64Color >> CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_R )) & ui64RedMask;

						uint64_t ui64BlueMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_B )) - 1ULL;
						uint64_t ui64Blue = (ui64Color >> CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_B )) & ui64BlueMask;

						// Remove them from the color.
						ui64Color &= ~((ui64RedMask << CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_R )) |
							(ui64BlueMask << CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_B )));

						// Add them back.
						ui64Color |= (ui64Red << CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_B )) |
							(ui64Blue << CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_R ));

						// Put them back into the image.
						iImage.SetTexelAt( ui64Color, LSI_PF_R16G16B16A16, X, Y );
					}
				}
			}

			// If we are ignoring alpha, set alpha to 1.
			if ( _oOptions.bIgnoreAlpha ) {
				for ( uint32_t Y = iImage.GetHeight(); Y--; ) {
					for ( uint32_t X = iImage.GetWidth(); X--; ) {
						// Get the color.
						uint64_t ui64Color = iImage.GetTexelAt( LSI_PF_R16G16B16A16, X, Y );

						// Get the alpha mask.
						uint64_t ui64AlphaMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_A )) - 1ULL;

						// Shift into position.
						ui64AlphaMask <<= CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_A );

						// Full alpha means just setting all the alpha bits, which is what the alpha mask is.
						ui64Color |= (ui64AlphaMask << CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_A ));

						// Put them back into the image.
						iImage.SetTexelAt( ui64Color, LSI_PF_R16G16B16A16, X, Y );
					}
				}
			}

			// Should we pre-multiply alpha?
			if ( _oOptions.bPremultiplyAlpha ) {
				iImage.PreMultiplyAlpha();
			}

			// Determine the resampling size.
			uint32_t ui32NewWidth = _oOptions.ui32RescaleWidth;
			uint32_t ui32NewHeight = _oOptions.ui32RescaleHeight;
			if ( !ui32NewWidth || !ui32NewHeight ) {
				ui32NewWidth = iImage.GetWidth();
				ui32NewHeight = iImage.GetHeight();
			}

			// If relative scaling is applied, apply it.
			ui32NewWidth = static_cast<uint32_t>(ui32NewWidth * _oOptions.fRelScaleWidth);
			ui32NewHeight = static_cast<uint32_t>(ui32NewHeight * _oOptions.fRelScaleHeight);

			// Rounding to any powers of 2?
			switch ( _oOptions.poPo2Options ) {
				case CDxt::LSX_PO_NO_RESCALE : { break; }
				case CDxt::LSX_PO_NEAREST : {
					uint32_t ui32NearestLowW = CStd::GetLowestPo2( ui32NewWidth ) >> 1;
					uint32_t ui32NearestLowH = CStd::GetLowestPo2( ui32NewHeight ) >> 1;

					uint32_t ui32NearestHiW = CStd::GetLowestPo2( ui32NewWidth );
					uint32_t ui32NearestHiH = CStd::GetLowestPo2( ui32NewHeight );

					ui32NewWidth = (ui32NearestHiW - ui32NewWidth) < (ui32NewWidth - ui32NearestLowW) ? ui32NearestHiW : ui32NearestLowW;
					ui32NewHeight = (ui32NearestHiH - ui32NewHeight) < (ui32NewHeight - ui32NearestLowH) ? ui32NearestHiH : ui32NearestLowH;
					break;
				}
				case CDxt::LSX_PO_NEXT_HI : {
					ui32NewWidth = CStd::GetLowestPo2( ui32NewWidth );
					ui32NewHeight = CStd::GetLowestPo2( ui32NewHeight );
					break;
				}
				case CDxt::LSX_PO_NEXT_LO : {
					uint32_t ui32TempW = CStd::GetLowestPo2( ui32NewWidth );
					uint32_t ui32TempH = CStd::GetLowestPo2( ui32NewHeight );
					ui32NewWidth = ui32TempW == ui32NewWidth ? ui32NewWidth : ui32TempW >> 1;
					ui32NewHeight = ui32TempH == ui32NewHeight ? ui32NewHeight : ui32TempH >> 1;
					break;
				}
			}

			// Apply clamps.
			if ( _oOptions.ui32WidthClamp ) {
				ui32NewWidth = CStd::Min( _oOptions.ui32WidthClamp, ui32NewWidth );
			}
			if ( _oOptions.ui32HeightClamp ) {
				ui32NewHeight = CStd::Min( _oOptions.ui32HeightClamp, ui32NewHeight );
			}

			// Resize the image in-place.
			if ( iImage.GetWidth() != ui32NewWidth || iImage.GetHeight() != ui32NewHeight ) {
				if ( iImage.GetFormat() >= LSI_PF_TOTAL ) {
					iImage.ConvertToFormatInPlace( LSI_PF_R16G16B16A16 );
				}
				if ( !iImage.ResampleInPlace( ui32NewWidth, ui32NewHeight, _oOptions.fFilter, _oOptions.fFilterGamma, _oOptions.amAddressMode, 1.0f ) ) {
					eError = LSSTD_E_PARTIALFAILURE;
					::printf( "Failed to resize image %s.\r\n", _oOptions.slInputs[I].CStr() );
					continue;
				}
			}

			// If converting to a normal map, do it now.
			if ( _oOptions.ui32NormalMapKernelSize ) {
				CKernel kU, kV;
				switch ( _oOptions.ui32NormalMapKernelSize ) {
					case 5 : {
						kU.CreateSobel5x5();
						break;
					}
					case 7 : {
						kU.CreateSobel7x7();
						break;
					}
					case 9 : {
						kU.CreateSobel9x9();
						break;
					}
					default : {
						kU.CreateSobel3x3();
						break;
					}
				}
				kV = kU;
				kV.Transpose();
				CVector4F vWeights = CVector4F( _oOptions.fRedWeight, _oOptions.fGreenWeight, _oOptions.fBlueWeight, 1.0f );
				switch ( _oOptions.ui32NormalMapChannel ) {
					case LSI_PC_R : {
						vWeights = CVector4F( 1.0f, 0.0f, 0.0f, 0.0f );
						break;
					}
					case LSI_PC_G : {
						vWeights = CVector4F( 0.0f, 1.0f, 0.0f, 0.0f );
						break;
					}
					case LSI_PC_B : {
						vWeights = CVector4F( 0.0f, 0.0f, 1.0f, 0.0f );
						break;
					}
					case LSI_PC_A : {
						vWeights = CVector4F( 0.0f, 0.0f, 0.0f, 1.0f );
						break;
					}
					case CImage::LSI_CA_AVERAGE : {
						vWeights = CVector4F( 1.0f, 1.0f, 1.0f, 0.0f );
						break;
					}
				}
				iImage.ConvertToNormalMap( kU, kV, _oOptions.fNormalMapStr, vWeights, _oOptions.amAddressMode, _oOptions.ui32NormalMapChannel == CImage::LSI_CA_MAX );
			}

			const char * pcExtension = CFileLib::GetExtension( _oOptions.slOutputs[I].CStr() );

			// Create an image of the final file in memory.
			CMemFile mfFileImage;
			if ( _oOptions.kifFormat != static_cast<LSI_KTX_INTERNAL_FORMAT>(0) ) {
				eError = CreateKtx1( _oOptions, iImage, mfFileImage, I );
				if ( eError != LSSTD_E_SUCCESS ) {
					continue;
				}
			}
			else if ( pcExtension && CStd::StrICmp( pcExtension, "bmp" ) == 0 ) {
				eError = CreateBmp( _oOptions, iImage, mfFileImage, I );
				if ( eError != LSSTD_E_SUCCESS ) {
					continue;
				}
			}
			else if ( pcExtension && CStd::StrICmp( pcExtension, "ico" ) == 0 ) {
				eError = CreateIco( _oOptions, iImage, mfFileImage, I );
				if ( eError != LSSTD_E_SUCCESS ) {
					continue;
				}
			}
			else if ( pcExtension && CStd::StrICmp( pcExtension, "png" ) == 0 ) {
				eError = CreatePng( _oOptions, iImage, mfFileImage, I );
				if ( eError != LSSTD_E_SUCCESS ) {
					continue;
				}
			}
			else if ( pcExtension && CStd::StrICmp( pcExtension, "qoi" ) == 0 ) {
				eError = CreateQoi( _oOptions, iImage, mfFileImage, I );
				if ( eError != LSSTD_E_SUCCESS ) {
					continue;
				}
			}
			else {
				eError = CreateDds( _oOptions, iImage, mfFileImage, I );
				if ( eError != LSSTD_E_SUCCESS ) {
					continue;
				}
			}

			


			// Save the file.
			CFileStream fsStream( _oOptions.slOutputs[I].CStr(), LSF_OF_WRITE, true );
			::printf( "Size: %u.\r\n", mfFileImage.Length() );
			if ( fsStream.WriteBytes( &mfFileImage[0], mfFileImage.Length() ) == CStream::LSSTD_SE_ERROR ) {
				eError = LSSTD_E_PARTIALFAILURE;
				::printf( "Failed to create file %s.\r\n", _oOptions.slOutputs[I].CStr() );
				continue;
			}
			if ( CFileLib::Exists( _oOptions.slOutputs[I].CStr() ) ) {
				::printf( "Saved to %s.\r\n", _oOptions.slOutputs[I].CStr() );
				CStd::DebugPrintA( _oOptions.slOutputs[I].CStr() );
				CStd::DebugPrintA( "\r\n" );
			}
			else {
				::printf( "Huh? %s.\r\n", _oOptions.slOutputs[I].CStr() );
			}

		}
		return eError;
	}

	/**
	 * Prints the KTX 1 format list for the Help file.
	 */
	void LSE_CALL CDxt::PrintKtx1Help() {
		::OutputDebugStringA( "-ktx1format\r\n" );
		for ( size_t I = 0; I < CKtx::GetInternalFormatDataTotal(); ++I ) {
			const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdData = CKtx::GetInternalFormatData( I );
			const char * pcName = CKtx::glInternalFormatToString( pkifdData->kifInternalFormat );
			if ( !pkifdData->pfToRgba32F && !pkifdData->pfFromRgba32F ) { continue; }
			//::OutputDebugStringA( "\t" );
			::OutputDebugStringA( &pcName[3] );
			if ( !pkifdData->pfToRgba32F || !pkifdData->pfFromRgba32F ) {
				::OutputDebugStringA( " (" );
			}
			if ( !pkifdData->pfToRgba32F ) {
				::OutputDebugStringA( "No Conversion From" );
				if ( !pkifdData->pfFromRgba32F ) {
					::OutputDebugStringA( ", " );
				}
			}
			if ( !pkifdData->pfFromRgba32F ) {
				::OutputDebugStringA( "No Conversion To" );
			}
			if ( !pkifdData->pfToRgba32F || !pkifdData->pfFromRgba32F ) {
				::OutputDebugStringA( ")" );
			}
			::OutputDebugStringA( "\r\n" );
		}
	}

	/**
	 * Prints the KTX 1 format list for the parsing inputs.
	 */
	void LSE_CALL CDxt::PrintKtx1Parse() {
		::OutputDebugStringA( "else if ( LSX_VERIFY_INPUT( ktx1format, 1 ) ) {\r\n\t++I;\r\n\tbool bFound = false;\r\n" );
		for ( size_t I = 0; I < CKtx::GetInternalFormatDataTotal(); ++I ) {
			const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdData = CKtx::GetInternalFormatData( I );
			const char * pcName = CKtx::glInternalFormatToString( pkifdData->kifInternalFormat );
			if ( !pkifdData->pfToRgba32F && !pkifdData->pfFromRgba32F ) { continue; }
			::OutputDebugStringA( "\tif ( CStd::WStrICmp( _pwcArgv[I], L\"" );
			::OutputDebugStringA( &pcName[3] );
			::OutputDebugStringA( "\" ) == 0 ) {\r\n" );
			::OutputDebugStringA( "\t\toOptions.kifFormat = lsi::LSI_KIF_" );
			::OutputDebugStringA( pcName );
			::OutputDebugStringA( ";\r\n\t\tbFound = true;\r\n" );
			::OutputDebugStringA( "\t}\r\n" );
		}
		::OutputDebugStringA( "\tif ( !bFound ) { LSX_ERROR( LSSTD_E_INVALIDCALL ); }\r\n}\r\n" );
	}

	/**
	 * Creates a DDS file.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreateDds( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		// Convert it to DXT.
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;
		LSI_PIXEL_FORMAT pfFormat = LSI_PF_DXT3;
		uint32_t ui32Match = 0, ui32UnMatch = 0;
		for ( uint32_t J = LSE_ELEMENTS( m_ftTable ); J--; ) {
			if ( m_ftTable[J].ifFormat == _oOptions.ifOutFormat ) {
				pfFormat = m_ftTable[J].pfEngineFormat;
				ui32Match = J;
				break;
			}
		}
		for ( uint32_t J = LSE_ELEMENTS( m_utTable ); J--; ) {
			if ( m_utTable[J].ifFormat == _oOptions.ifOutFormat ) {
				ui32UnMatch = J;
				break;
			}
		}

		CImage iDxt;
		CImage::LSI_DXT_OPTIONS doOptions = OptionsToDssOptions( _oOptions );


		LSBOOL bDxt = false;
		switch ( _oOptions.ifOutFormat ) {
			case LSX_IF_DXT1A : {}
			case LSX_IF_DXT1C : {}
			case LSX_IF_DXT2 : {}
			case LSX_IF_DXT3 : {}
			case LSX_IF_DXT4 : {}
			case LSX_IF_DXT5 : {}
			case LSX_IF_BC4 : {}
			case LSX_IF_BC4S : {}
			case LSX_IF_BC5 : {}
			case LSX_IF_BC5S : {}
			case LSX_IF_BC6H : {}
			case LSX_IF_BC7 : {
				bDxt = true;
				::printf( "Converting %s to DXT.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
				if ( !_iImage.ConvertToDxt( pfFormat, iDxt, &doOptions ) ) {
					eError = LSSTD_E_PARTIALFAILURE;
					::printf( "Failed to create DXT compression for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
					return eError;
				}
				break;
			}
		}

		eError = CreateDdsHeader( _oOptions, iDxt, _mfFile,
			_ui32FileIndex, ui32Match, ui32UnMatch );
		if ( eError != LSSTD_E_SUCCESS ) {
			return eError;
		}
		if ( bDxt ) {
			if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(iDxt.GetBufferData()), iDxt.GetBufferDataAsVector().Length() ) ) {
				eError = LSSTD_E_PARTIALFAILURE;
				::printf( "Failed to save DXT level-1 surface for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
				return eError;
			}
		}
		else {
			// Convert manually.
			::printf( "Converting %s to uncompressed format.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			uint64_t ui64RShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32RBitMask );
			uint64_t ui64GShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32GBitMask );
			uint64_t ui64BShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32BBitMask );
			uint64_t ui64AShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32ABitMask );

			uint64_t ui64RFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_R ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32RBitMask );
			uint64_t ui64GFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_G ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32GBitMask );
			uint64_t ui64BFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_B ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32BBitMask );
			uint64_t ui64AFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_A ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32ABitMask );
#define LSX_MASK( COMP ) ((1ULL << CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, COMP )) - 1ULL)
			uint64_t ui64RMask = LSX_MASK( LSI_PC_R );
			uint64_t ui64GMask = LSX_MASK( LSI_PC_G );
			uint64_t ui64BMask = LSX_MASK( LSI_PC_B );
			uint64_t ui64AMask = LSX_MASK( LSI_PC_A );
			uint64_t ui64OffsetR = CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_R );
			uint64_t ui64OffsetG = CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_G );
			uint64_t ui64OffsetB = CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_B );
			uint64_t ui64OffsetA = CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, LSI_PC_A );
			for ( uint32_t Y = 0; Y < _iImage.GetHeight(); ++Y ) {
				for ( uint32_t X = 0; X < _iImage.GetWidth(); ++X ) {
					uint64_t ui64This = _iImage.GetTexelAt( LSI_PF_R16G16B16A16, X, Y );
					// Extract each 16-bit component.


					uint64_t ui64R = (ui64This >> ui64OffsetR) & ui64RMask;
					uint64_t ui64G = (ui64This >> ui64OffsetG) & ui64GMask;
					uint64_t ui64B = (ui64This >> ui64OffsetB) & ui64BMask;
					uint64_t ui64A = (ui64This >> ui64OffsetA) & ui64AMask;
#undef LSX_MASK
					// Convert to the bit size of the destination format.
					ui64R >>= ui64RFix;
					ui64G >>= ui64GFix;
					ui64B >>= ui64BFix;
					ui64A >>= ui64AFix;

					// Create the new texel.
					uint64_t ui64Final = (ui64R << ui64RShift) |
						(ui64G << ui64GShift) |
						(ui64B << ui64BShift) |
						(ui64A << ui64AShift);
					// Write the value to the file.
					uint32_t ui32Total = m_utTable[ui32UnMatch].ui32RgbBitCount >> 3UL;
					if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui64Final), ui32Total ) ) {
						eError = LSSTD_E_PARTIALFAILURE;
						::printf( "Failed to save DXT level-1 uncompressed surface for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
						continue;
					}
				}
			}
		}

		// Generate mipmaps.
		uint32_t ui32MipW = _iImage.GetWidth() >> 1;
		uint32_t ui32MipH = _iImage.GetHeight() >> 1;
		//LSBOOL bErrored = false;
		uint32_t ui32ActualMips = GetActualMips( iDxt, _oOptions );
		for ( uint32_t J = 1; J < ui32ActualMips; ++J ) {
			// Increase the quality since the image is smaller.

			CImage iTemp;
			::printf( "Generating mipmap %u (%u by %u).\r\n", J, ui32MipW, ui32MipH );
			if ( !_iImage.Resample( ui32MipW, ui32MipH, _oOptions.fMipFilter, iTemp, _oOptions.fFilterGamma, _oOptions.amAddressMode, 1.0f ) ) {
				eError = LSSTD_E_PARTIALFAILURE;
				::printf( "Failed to create DXT mipmap %u for %s.\r\n", J, _oOptions.slInputs[_ui32FileIndex].CStr() );
				//bErrored = true;
				break;
			}
			if ( _oOptions.bNormalizeMips ) {
				iTemp.Normalize();
			}
			if ( bDxt ) {
				if ( !iTemp.ConvertToDxt( pfFormat, iDxt, &doOptions ) ) {
					eError = LSSTD_E_PARTIALFAILURE;
					::printf( "Failed to create DXT level-%u surface for %s.\r\n", J, _oOptions.slInputs[_ui32FileIndex].CStr() );
					//bErrored = true;
					break;
				}
				if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(iDxt.GetBufferData()), iDxt.GetBufferDataAsVector().Length() ) ) {
					eError = LSSTD_E_PARTIALFAILURE;
					::printf( "Failed to save DXT level-%u surface for %s.\r\n", J, _oOptions.slInputs[_ui32FileIndex].CStr() );
					//bErrored = true;
					break;
				}
			}
			else {
				// Convert manually.
				uint64_t ui64RShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32RBitMask );
				uint64_t ui64GShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32GBitMask );
				uint64_t ui64BShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32BBitMask );
				uint64_t ui64AShift = CStd::Count0Bits( m_utTable[ui32UnMatch].ui32ABitMask );

				uint64_t ui64RFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_R ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32RBitMask );
				uint64_t ui64GFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_G ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32GBitMask );
				uint64_t ui64BFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_B ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32BBitMask );
				uint64_t ui64AFix = CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, LSI_PC_A ) - CStd::CountBits( m_utTable[ui32UnMatch].ui32ABitMask );
				for ( uint32_t Y = 0; Y < iTemp.GetHeight(); ++Y ) {
					for ( uint32_t X = 0; X < iTemp.GetWidth(); ++X ) {
						uint64_t ui64This = iTemp.GetTexelAt( LSI_PF_R16G16B16A16, X, Y );
						// Extract each 16-bit component.
#define LSX_MASK( COMP ) ((1ULL << CImageLib::GetComponentBits( LSI_PF_R16G16B16A16, COMP )) - 1ULL)
#define LSX_GETCOMP( COMP )	(ui64This >> CImageLib::GetComponentOffset( LSI_PF_R16G16B16A16, COMP )) & LSX_MASK( COMP )
						uint64_t ui64R = LSX_GETCOMP( LSI_PC_R );
						uint64_t ui64G = LSX_GETCOMP( LSI_PC_G );
						uint64_t ui64B = LSX_GETCOMP( LSI_PC_B );
						uint64_t ui64A = LSX_GETCOMP( LSI_PC_A );
#undef LSX_GETCOMP
#undef LSX_MASK
						// Convert to the bit size of the destination format.
						ui64R >>= ui64RFix;
						ui64G >>= ui64GFix;
						ui64B >>= ui64BFix;
						ui64A >>= ui64AFix;

						// Create the new texel.
						uint64_t ui64Final = (ui64R << ui64RShift) |
							(ui64G << ui64GShift) |
							(ui64B << ui64BShift) |
							(ui64A << ui64AShift);
						// Write the value to the file.
						volatile uint32_t ui32Total = m_utTable[ui32UnMatch].ui32RgbBitCount >> 3UL;
						if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui64Final), ui32Total ) ) {
							eError = LSSTD_E_PARTIALFAILURE;
							::printf( "Failed to save DXT level-%u uncompressed surface for %s.\r\n", J, _oOptions.slInputs[_ui32FileIndex].CStr() );
							continue;
						}
					}
				}
			}
				

			ui32MipW >>= 1;
			ui32MipH >>= 1;
			ui32MipW = CStd::Max<uint32_t>( ui32MipW, 1 );
			ui32MipH = CStd::Max<uint32_t>( ui32MipH, 1 );
		}
		return eError;
	}

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
	LSSTD_ERRORS LSE_CALL CDxt::CreateDdsHeader( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex, uint32_t _ui32Match, uint32_t _ui32UnMatch ) {
		// Magic number.
		uint32_t ui32Magic = 0x20534444;
		if ( !_mfFile.Append( reinterpret_cast<uint8_t *>(&ui32Magic), sizeof( ui32Magic ) ) ) {
			::printf( "Failed to create DXT header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}

		// Add the header.
		CDds::LSI_DDS_HEADER dhHeader = { sizeof( CDds::LSI_DDS_HEADER ) };	// Must be 124 though.
		dhHeader.ui32Flags = LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT;
			

		LSBOOL bDxt = false;
		switch ( _oOptions.ifOutFormat ) {
			case LSX_IF_DXT1A : {}
			case LSX_IF_DXT1C : {}
			case LSX_IF_DXT2 : {}
			case LSX_IF_DXT3 : {}
			case LSX_IF_DXT4 : {}
			case LSX_IF_DXT5 : {}
			case LSX_IF_BC4 : {}
			case LSX_IF_BC4S : {}
			case LSX_IF_BC5 : {}
			case LSX_IF_BC5S : {}
			case LSX_IF_BC6H : {}
			case LSX_IF_BC7 : {
				bDxt = true;
				dhHeader.ui32Flags |= LSI_DF_LINEARSIZE;
				if ( m_ftTable[_ui32Match].ui32FourCc == LSE_MAKEFOURCC( 'D', 'X', '1', '0' ) ) {
					dhHeader.ui32Flags &= ~LSI_DF_PIXELFORMAT;
				}
				break;
			}
			default : {
				dhHeader.ui32Flags |= LSI_DF_PITCH;
			}
		}
		if ( _oOptions.bMipMaps && _oOptions.ui32Mips >= 1 && _iImage.GetWidth() > 1 && _iImage.GetHeight() > 1 ) {
			dhHeader.ui32Flags |= LSI_DF_MIPMAPCOUNT;
		}
		dhHeader.ui32Height = _iImage.GetHeight();
		dhHeader.ui32Width = _iImage.GetWidth();
		if ( bDxt ) {
			dhHeader.ui32PitchOrLinearSize = CDds::DxtScanlineWidth( _iImage.GetWidth(), _iImage.GetFormat() );
		}
		else {
			dhHeader.ui32PitchOrLinearSize = (_iImage.GetWidth() * m_utTable[_ui32UnMatch].ui32RgbBitCount + 7UL) / 8UL;
		}
		dhHeader.ui32Depth = 0;
		dhHeader.ui32MipMapCount = GetActualMips( _iImage, _oOptions );
		dhHeader.dpPixelFormat.ui32Size = sizeof( dhHeader.dpPixelFormat );	// Must be 32 though.
		if ( bDxt ) {
			dhHeader.dpPixelFormat.ui32Flags = LSI_DPFF_FOURCC;
			dhHeader.dpPixelFormat.ui32FourCC = m_ftTable[_ui32Match].ui32FourCc;
			dhHeader.dpPixelFormat.ui32RGBBitCount = 0;
			dhHeader.dpPixelFormat.ui32RBitMask = 0;
			dhHeader.dpPixelFormat.ui32GBitMask = 0;
			dhHeader.dpPixelFormat.ui32BBitMask = 0;
			dhHeader.dpPixelFormat.ui32ABitMask = 0;
		}
		else {
			dhHeader.dpPixelFormat.ui32Flags = m_utTable[_ui32UnMatch].ui32Flags;
			dhHeader.dpPixelFormat.ui32RGBBitCount = m_utTable[_ui32UnMatch].ui32RgbBitCount;
			dhHeader.dpPixelFormat.ui32RBitMask = m_utTable[_ui32UnMatch].ui32RBitMask;
			dhHeader.dpPixelFormat.ui32GBitMask = m_utTable[_ui32UnMatch].ui32GBitMask;
			dhHeader.dpPixelFormat.ui32BBitMask = m_utTable[_ui32UnMatch].ui32BBitMask;
			dhHeader.dpPixelFormat.ui32ABitMask = m_utTable[_ui32UnMatch].ui32ABitMask;
		}

		dhHeader.ui32Caps = 0x1000;					// Texture.
		if ( dhHeader.ui32Flags & LSI_DF_MIPMAPCOUNT ) {
			dhHeader.ui32Caps |= 0x8 | 0x400000;	// Complex and mipmaps.
		}

		if ( !_mfFile.Append( reinterpret_cast<uint8_t *>(&dhHeader), sizeof( dhHeader ) ) ) {
			::printf( "Failed to create DXT header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}


		if ( bDxt ) {
			if ( m_ftTable[_ui32Match].ui32FourCc == LSE_MAKEFOURCC( 'D', 'X', '1', '0' ) ) {
				// Add the extended header.
				CDds::LSI_DDS_HEADER_DXT10 dhdExtHeader = { 0 };
				enum {
					LSX_BC6H_UF16				= 95,
					LSX_BC6H_SF16				= 96,
					LSX_BC7_UNORM				= 98,
					LSX_BC7_UNORM_SRGB			= 99,
				};
				// Because we currently only support BC6H_UF16 and BC7_UNORM.
				dhdExtHeader.ui32DxgiFormat = _oOptions.ifOutFormat == LSX_IF_BC6H ? LSX_BC6H_UF16 : LSX_BC7_UNORM;
				dhdExtHeader.ui32ArraySize = 1;
				dhdExtHeader.ui32ResourceDimension = 3; // D3D10_RESOURCE_DIMENSION_TEXTURE2D
				dhdExtHeader.ui32MiscFlag = 0;			// 0x4 = cube.
				dhdExtHeader.ui32MiscFlags2 = 1;

				if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&dhdExtHeader), sizeof( dhdExtHeader ) ) ) {
					::printf( "Failed to save DXT extended header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
					return LSSTD_E_PARTIALFAILURE;
				}
			}
		}
		return LSSTD_E_SUCCESS;
	}

	/**
	 * Creates a KTX file.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreateKtx1( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;
		const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdData = CKtx::FindInternalFormatData( _oOptions.kifFormat );
		if ( !pkifdData ) {
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to create KTX1 file: Internal data error.\r\n" );
			return eError;
		}
		void * pvOptions = nullptr;
		CImage::LSI_DXT_OPTIONS doDxtOptions;
		CImage::LSI_ETC_OPTIONS eoOptions;
		if ( IsDxt( _oOptions.kifFormat ) ) {
			doDxtOptions = OptionsToDssOptions( _oOptions );
			pvOptions = &doDxtOptions;
		}
		else if ( IsEtc( _oOptions.kifFormat ) ) {
			eoOptions = OptionsToEtcOptions( _oOptions );
			pvOptions = &eoOptions;
		}

		CImage iKtx;

		ktxTexture1 * ptTexture = nullptr;
		ktxTextureCreateInfo tciCreateInfo = { 0 };
		tciCreateInfo.glInternalformat = _oOptions.kifFormat;
		tciCreateInfo.baseWidth = _iImage.GetWidth();
		tciCreateInfo.baseHeight = _iImage.GetHeight();
		tciCreateInfo.baseDepth = 1;
		tciCreateInfo.numDimensions = 2;
		tciCreateInfo.numLevels = GetActualMips( _iImage, _oOptions );
		tciCreateInfo.numLayers = 1;
		tciCreateInfo.numFaces = 1;
		tciCreateInfo.isArray = KTX_FALSE;
		tciCreateInfo.generateMipmaps = KTX_FALSE;

		KTX_error_code ecResult = ::ktxTexture1_Create( &tciCreateInfo,
			KTX_TEXTURE_CREATE_ALLOC_STORAGE,
			&ptTexture );
		if ( KTX_SUCCESS != ecResult ) {
			eError = KtxErrorToError( ecResult );
			::printf( "Failed to create KTX1 file: %s.\r\n", ::ktxErrorString( ecResult ) );
			return eError;
		}

		if ( !_iImage.ConvertToKtx1( _oOptions.kifFormat, iKtx, pvOptions ) ) {
			ktxTexture_Destroy( ktxTexture( ptTexture ) );
			eError = LSSTD_E_PARTIALFAILURE;
			::printf( "Failed to create KTX1 for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		uint32_t ui32Size = CKtx::GetTextureSize( iKtx.GetWidth(), iKtx.GetHeight(), 1, pkifdData );
		ktxTexture_SetImageFromMemory( ktxTexture( ptTexture ),
			0, 0, 0,
			static_cast<const ktx_uint8_t *>(iKtx.GetBufferData()), ui32Size );

		// Generate mipmaps.
		uint32_t ui32MipW = _iImage.GetWidth() >> 1;
		uint32_t ui32MipH = _iImage.GetHeight() >> 1;
		bool bErrored = false;
		CImage iTemp;
		for ( uint32_t J = 1; J < tciCreateInfo.numLevels; ++J ) {
			::printf( "Generating mipmap %u (%u by %u).\r\n", J, ui32MipW, ui32MipH );
			if ( !_iImage.Resample( ui32MipW, ui32MipH, _oOptions.fMipFilter, iTemp, _oOptions.fFilterGamma, _oOptions.amAddressMode, 1.0f ) ) {
				eError = LSSTD_E_PARTIALFAILURE;
				::printf( "Failed to create KTX1 mipmap %u for %s.\r\n", J, _oOptions.slInputs[_ui32FileIndex].CStr() );
				bErrored = true;
				break;
			}
			if ( _oOptions.bNormalizeMips ) {
				iTemp.Normalize();
			}

			if ( !iTemp.ConvertToKtx1( _oOptions.kifFormat, iKtx, pvOptions ) ) {
				eError = LSSTD_E_PARTIALFAILURE;
				::printf( "Failed to create KTX1 level-%u surface for %s.\r\n", J, _oOptions.slInputs[_ui32FileIndex].CStr() );
				bErrored = true;
				break;
			}
			ui32Size = CKtx::GetTextureSize( iKtx.GetWidth(), iKtx.GetHeight(), 1, pkifdData );
			ktxTexture_SetImageFromMemory( ktxTexture( ptTexture ),
				J, 0, 0,
				static_cast<const ktx_uint8_t *>(iKtx.GetBufferData()), ui32Size );


			ui32MipW >>= 1;
			ui32MipH >>= 1;
			ui32MipW = CStd::Max<uint32_t>( ui32MipW, 1 );
			ui32MipH = CStd::Max<uint32_t>( ui32MipH, 1 );
		}
		// Free memory for the write process.
		iKtx.Reset();
		iTemp.Reset();

		
		ktx_uint8_t * puiBuffer = nullptr;
		ktx_size_t sBufferLen = 0;
		if ( !bErrored ) {
			ecResult = ktxTexture_WriteToMemory( ktxTexture( ptTexture ),
				&puiBuffer, &sBufferLen );

			if ( KTX_SUCCESS != ecResult ) {
				ktxTexture_Destroy( ktxTexture( ptTexture ) );
				std::free( puiBuffer );
				eError = KtxErrorToError( ecResult );
				::printf( "Failed to create KTX1 file: %s.\r\n", ::ktxErrorString( ecResult ) );
				return eError;
			}

			do {
				if ( !_mfFile.Append( puiBuffer, uint32_t( sBufferLen ) ) ) {
					eError = LSSTD_E_OUTOFMEMORY;
					::printf( "Failed to create file image %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
					//bErrored = true;
					break;
				}
			} while ( 0 );
		}


		ktxTexture_Destroy( ktxTexture( ptTexture ) );
		std::free( puiBuffer );

		return eError;
#if 0
		// Header.
		// identifier.
		uint8_t ui8FileIdentifier[12] = {
		   0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
		};
		if ( !_mfFile.Append( ui8FileIdentifier, sizeof( ui8FileIdentifier ) ) ) {
			::printf( "Failed to create KTX1 header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}
		// endianness.
		const uint32_t ui32End = 0x04030201;
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui32End), sizeof( ui32End ) ) ) {
			::printf( "Failed to create KTX1 header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}
		// glType.
		// For compressed textures, glType must equal 0.
		const uint32_t ui32Zero = 0;
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui32Zero), sizeof( ui32Zero ) ) ) {
			::printf( "Failed to create KTX1 header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}
		// glTypeSize.
		// For ptTexture data which does not depend on platform endianness, including compressed ptTexture data, glTypeSize must equal 1.
		const uint32_t ui32One = 1;
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui32One), sizeof( ui32One ) ) ) {
			::printf( "Failed to create KTX1 header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}

		// glFormat.
		// For compressed textures, glFormat must equal 0.
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui32Zero), sizeof( ui32Zero ) ) ) {
			::printf( "Failed to create KTX1 header for %s.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return LSSTD_E_PARTIALFAILURE;
		}

		// glInternalFormat.
		// For compressed textures, glInternalFormat must equal the compressed internal format, usually one of the values from table 8.14 of the OpenGL 4.4 specification [OPENGL44].
#endif	// #if 0
	}

	/**
	 * Creates a BMP file.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreateBmp( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;

		CImage iImage;
		_iImage.ConvertToFormat( LSI_PF_R8G8B8A8, iImage );

		LSI_PIXEL_FORMAT pfDstFormat = LSI_PF_R8G8B8A8;
		switch ( _oOptions.ui8BmpBits ) {
			case 0 : {}
			case 32 : {
				pfDstFormat = LSI_PF_R8G8B8A8;
				break;
			}
			case 24 : {
				pfDstFormat = LSI_PF_R8G8B8;
				break;
			}
			case 16 : {
				pfDstFormat = LSI_PF_R5G6B5;
				//pfDstFormat = LSI_PF_R5G5B5A1;
				break;
			}
			default : {
				eError = LSSTD_E_BADFORMAT;
				::printf( "Failed to create BMP file (%s): Format must be 16-, 24-, or 32- bit.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
				return eError;
			}
		}

		uint32_t ui32FormatBytes = CImageLib::GetFormatSize( pfDstFormat );
		uint32_t ui32Stride = ui32FormatBytes * iImage.GetWidth();
		if ( ui32Stride & 0x3 ) {
			ui32Stride = (ui32Stride & ~3) + 4;
		}

		CBmp::LSI_BITMAPFILEHEADER bmfhHeader = { 0x4D42 };
		bmfhHeader.ui32Offset = sizeof( CBmp::LSI_BITMAPFILEHEADER ) + sizeof( CBmp::LSI_BITMAPINFOHEADER );
		bmfhHeader.ui32Size = bmfhHeader.ui32Offset + ui32Stride * iImage.GetHeight() + 2;
		CBmp::LSI_BITMAPINFOHEADER bihInfo = { sizeof( CBmp::LSI_BITMAPINFOHEADER ) };
		bihInfo.ui32Width = iImage.GetWidth();
		bihInfo.ui32Height = iImage.GetHeight();
		bihInfo.ui16Planes = 1;
		bihInfo.ui32ImageSize = ui32Stride * iImage.GetHeight() + 2;
		bihInfo.ui32PixelsPerMeterX = static_cast<uint32_t>(std::round( 96.0 * 39.37007874015748096 ));
		bihInfo.ui32PixelsPerMeterY = static_cast<uint32_t>(std::round( 96.0 * 39.37007874015748096 ));
		
		bihInfo.ui16BitsPerPixel = uint16_t( ui32FormatBytes * 8 );
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&bmfhHeader), sizeof( bmfhHeader ) ) ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to create BMP file (%s): Out of memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&bihInfo), sizeof( bihInfo ) ) ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to create BMP file (%s): Out of memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		
		CVectorPoD<uint8_t, uint32_t, 0x10000> vRow;
		if ( !vRow.Resize( ui32Stride ) ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to create BMP file (%s): Out of memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		
		const CBmp::LSI_BITMAPCOLORMASK * lpbcmMask = nullptr;
		if ( bihInfo.ui32Compression == BI_BITFIELDS ) {
			//lpbcmMask = reinterpret_cast<const CBmp::LSI_BITMAPCOLORMASK *>(&_pui8FileData[sizeof( CBmp::LSI_BITMAPFILEHEADER )+sizeof( CBmp::LSI_BITMAPINFOHEADER )]);
		}
		else {
			static const CBmp::LSI_BITMAPCOLORMASK bcmDefaultMask32 = {
				0x00FF0000,
				0x0000FF00,
				0x000000FF,
				0xFF000000,
			};
			static const CBmp::LSI_BITMAPCOLORMASK bcmDefaultMask24 = {
				0x00FF0000,
				0x0000FF00,
				0x000000FF,
				0x00000000,
			};
			static const CBmp::LSI_BITMAPCOLORMASK bcmDefaultMask16 = {
				0x00007C00,
				0x000003E0,
				0x0000001F,
				0x00000000,
			};
			if ( bihInfo.ui16BitsPerPixel == 16 ) {
				lpbcmMask = &bcmDefaultMask16;
			}
			else if ( bihInfo.ui16BitsPerPixel == 32 ) {
				lpbcmMask = &bcmDefaultMask32;
			}
			else {
				lpbcmMask = &bcmDefaultMask24;
			}
		}
		// Determine how much we need to shift each channel to get the values we expect.
		uint32_t ui32RShift = 0;
		uint32_t ui32GShift = 0;
		uint32_t ui32BShift = 0;
		uint32_t ui32AShift = 0;
		while ( ui32RShift < 32 && !(lpbcmMask->ui32Red & (1 << ui32RShift)) ) { ++ui32RShift; }
		while ( ui32GShift < 32 && !(lpbcmMask->ui32Green & (1 << ui32GShift)) ) { ++ui32GShift; }
		while ( ui32BShift < 32 && !(lpbcmMask->ui32Blue & (1 << ui32BShift)) ) { ++ui32BShift; }
		while ( ui32AShift < 32 && !(lpbcmMask->ui32Alpha & (1 << ui32AShift)) ) { ++ui32AShift; }

		// Also get the number of bits per component.
		uint32_t ui32RBits = 0;
		uint32_t ui32GBits = 0;
		uint32_t ui32BBits = 0;
		uint32_t ui32ABits = 0;
		while ( (lpbcmMask->ui32Red & (1 << (ui32RShift + ui32RBits)) ) ) { ++ui32RBits; }
		while ( (lpbcmMask->ui32Green & (1 << (ui32GShift + ui32GBits)) ) ) { ++ui32GBits; }
		while ( (lpbcmMask->ui32Blue & (1 << (ui32BShift + ui32BBits)) ) ) { ++ui32BBits; }
		while ( (lpbcmMask->ui32Alpha & (1 << (ui32AShift + ui32ABits)) ) ) { ++ui32ABits; }

		uint32_t ui32RMask = (1 << ui32RBits) - 1;
		uint32_t ui32GMask = (1 << ui32GBits) - 1;
		uint32_t ui32BMask = (1 << ui32BBits) - 1;
		uint32_t ui32AMask = (1 << ui32ABits) - 1;

		for ( uint32_t Y = 0; Y < bihInfo.ui32Height; ++Y ) {
			CStd::MemSet( &vRow[0], 0, vRow.Length() );
			uint8_t * ui8Dst = &vRow[0];
			for ( uint32_t X = 0; X < bihInfo.ui32Width; ++X ) {
				uint64_t ui64Texel = iImage.GetTexelAt( LSI_PF_R8G8B8A8, X, (bihInfo.ui32Height - 1) - Y );
				uint8_t ui8A = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ) );
				uint8_t ui8R = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R ) );
				uint8_t ui8G = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G ) );
				uint8_t ui8B = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B ) );
				if ( bihInfo.ui16BitsPerPixel == 16 ) {
					uint16_t * pui16Dst = reinterpret_cast<uint16_t *>(ui8Dst);
					(*pui16Dst) = uint16_t(
						(((ui8A >> (8 - ui32ABits)) & ui32AMask) << ui32AShift) |
						(((ui8R >> (8 - ui32RBits)) & ui32RMask) << ui32RShift) |
						(((ui8G >> (8 - ui32GBits)) & ui32GMask) << ui32GShift) |
						(((ui8B >> (8 - ui32BBits)) & ui32BMask) << ui32BShift) );
				}
				else if ( bihInfo.ui16BitsPerPixel == 32 ) {
					uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(ui8Dst);
					(*pui32Dst) = uint32_t( ((ui8A & ui32AMask) << ui32AShift) |
						((ui8R & ui32RMask) << ui32RShift) |
						((ui8G & ui32GMask) << ui32GShift) |
						((ui8B & ui32BMask) << ui32BShift) );
				}
				else {
					ui8Dst[ui32RShift>>3] = ui8R;
					ui8Dst[ui32GShift>>3] = ui8G;
					ui8Dst[ui32BShift>>3] = ui8B;
				}

				ui8Dst += ui32FormatBytes;
			}
			if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&vRow[0]), vRow.Length() ) ) {
				eError = LSSTD_E_OUTOFMEMORY;
				::printf( "Failed to create BMP file (%s): Out of memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
				return eError;
			}
		}

		uint16_t ui16Post = 0;
		if ( !_mfFile.Append( reinterpret_cast<const uint8_t *>(&ui16Post), sizeof( ui16Post ) ) ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to create BMP file (%s): Out of memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		
		return eError;
	}

	/**
	 * Creates a BMP file using FreeImage.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreateBmp_FI( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;

		CImage iImage;
		_iImage.ConvertToFormat( LSI_PF_R8G8B8A8, iImage );
		FIBITMAP * pbmBitmap = ::FreeImage_Allocate( iImage.GetWidth(), iImage.GetHeight(), 32 );
		if ( !pbmBitmap ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to allocate bitmap structure for BMP file (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		for ( uint32_t Y = 0; Y < iImage.GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < iImage.GetWidth(); ++X ) {
				uint64_t ui64Texel = iImage.GetTexelAt( LSI_PF_R8G8B8A8, X, (iImage.GetWidth() - 1) - Y );
				RGBQUAD rqQuad;
				rqQuad.rgbReserved = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ) );
				rqQuad.rgbRed = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R ) );
				rqQuad.rgbGreen = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G ) );
				rqQuad.rgbBlue = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B ) );
				::FreeImage_SetPixelColor( pbmBitmap, X, Y, &rqQuad );
			}
		}

		FIMEMORY * pmMemory = ::FreeImage_OpenMemory();
		if ( nullptr == pmMemory ) {
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to memory stream for BMP file (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		if ( !::FreeImage_SaveToMemory( FIF_BMP, pbmBitmap, pmMemory, BMP_DEFAULT ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to save BMP file (%s) to memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		BYTE * pbData;
		DWORD dwSize;
		if ( !::FreeImage_AcquireMemory( pmMemory, &pbData, &dwSize ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to save BMP file (%s) to memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		if ( !_mfFile.Append( pbData, dwSize ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to copy BMP file (%s) memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		::FreeImage_CloseMemory( pmMemory );
		::FreeImage_Unload( pbmBitmap );

		return eError;
	}

	/**
	 * Creates an ICO file.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreateIco( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;

		CImage iImage;
		_iImage.ConvertToFormat( LSI_PF_R8G8B8A8, iImage );
		FIBITMAP * pbmBitmap = ::FreeImage_Allocate( iImage.GetWidth(), iImage.GetHeight(), 32 );
		if ( !pbmBitmap ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to allocate bitmap structure for ICO file (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		for ( uint32_t Y = 0; Y < iImage.GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < iImage.GetWidth(); ++X ) {
				uint64_t ui64Texel = iImage.GetTexelAt( LSI_PF_R8G8B8A8, X, (iImage.GetWidth() - 1) - Y );
				RGBQUAD rqQuad;
				rqQuad.rgbReserved = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ) );
				rqQuad.rgbRed = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R ) );
				rqQuad.rgbGreen = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G ) );
				rqQuad.rgbBlue = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B ) );
				::FreeImage_SetPixelColor( pbmBitmap, X, Y, &rqQuad );
			}
		}

		FIMEMORY * pmMemory = ::FreeImage_OpenMemory();
		if ( nullptr == pmMemory ) {
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to memory stream for ICO file (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		if ( !::FreeImage_SaveToMemory( FIF_ICO, pbmBitmap, pmMemory, ICO_DEFAULT ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to save ICO file (%s) to memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		BYTE * pbData;
		DWORD dwSize;
		if ( !::FreeImage_AcquireMemory( pmMemory, &pbData, &dwSize ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to save ICO file (%s) to memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		if ( !_mfFile.Append( pbData, dwSize ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to copy ICO file (%s) memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		::FreeImage_CloseMemory( pmMemory );
		::FreeImage_Unload( pbmBitmap );

		return eError;
	}

	/**
	 * Creates a PNG file.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreatePng( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;

		int iFlags = 0;
		switch ( _oOptions.pcPngCompress ) {
			case LSX_PC_BEST_SPEED : {
				iFlags |= PNG_Z_BEST_SPEED;
				break;
			}
			case LSX_PC_DEFAULT_COMPRESSION : {
				iFlags |= PNG_Z_DEFAULT_COMPRESSION;
				break;
			}
			case LSX_PC_BEST_COMPRESSION : {
				iFlags |= PNG_Z_BEST_COMPRESSION;
				break;
			}
			case LSX_PC_NO_COMPRESSION : {
				iFlags |= PNG_Z_NO_COMPRESSION;
				break;
			}
		}
		if ( _oOptions.bPngInterlaced ) {
			iFlags |= PNG_INTERLACED;
		}

		CImage iImage;
		_iImage.ConvertToFormat( LSI_PF_R8G8B8A8, iImage );
		FIBITMAP * pbmBitmap = ::FreeImage_Allocate( iImage.GetWidth(), iImage.GetHeight(), 32 );
		if ( !pbmBitmap ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to allocate bitmap structure for PNG file (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		for ( uint32_t Y = 0; Y < iImage.GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < iImage.GetWidth(); ++X ) {
				uint64_t ui64Texel = iImage.GetTexelAt( LSI_PF_R8G8B8A8, X, (iImage.GetHeight() - 1) - Y );
				RGBQUAD rqQuad;
				rqQuad.rgbReserved = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ) );
				rqQuad.rgbRed = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R ) );
				rqQuad.rgbGreen = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G ) );
				rqQuad.rgbBlue = uint8_t( ui64Texel >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B ) );
				::FreeImage_SetPixelColor( pbmBitmap, X, Y, &rqQuad );
			}
		}

		FIMEMORY * pmMemory = ::FreeImage_OpenMemory();
		if ( nullptr == pmMemory ) {
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to memory stream for PNG file (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		if ( !::FreeImage_SaveToMemory( FIF_PNG, pbmBitmap, pmMemory, iFlags ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to save PNG file (%s) to memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		BYTE * pbData;
		DWORD dwSize;
		if ( !::FreeImage_AcquireMemory( pmMemory, &pbData, &dwSize ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_INTERNALERROR;
			::printf( "Failed to save PNG file (%s) to memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		if ( !_mfFile.Append( pbData, dwSize ) ) {
			::FreeImage_CloseMemory( pmMemory );
			::FreeImage_Unload( pbmBitmap );
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Failed to copy PNG file (%s) memory.\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		::FreeImage_CloseMemory( pmMemory );
		::FreeImage_Unload( pbmBitmap );

		return eError;
	}

	/**
	 * Creates a QOI file.
	 *
	 * \param _oOptions Conversion options.
	 * \param _iImage The image to save.
	 * \param _mfFile The in-memory file to which to write the file data.
	 * \param _ui32FileIndex Index of the file being converted.
	 * \return Returns an error code indicating successor failure.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::CreateQoi( const LSX_OPTIONS &_oOptions, const CImage &_iImage, CMemFile &_mfFile,
		uint32_t _ui32FileIndex ) {
		constexpr uint32_t QOI_OP_INDEX = 0x00;		/* 00xxxxxx */
		constexpr uint32_t QOI_OP_DIFF = 0x40;		/* 01xxxxxx */
		constexpr uint32_t QOI_OP_LUMA = 0x80;		/* 10xxxxxx */
		constexpr uint32_t QOI_OP_RUN = 0xC0;		/* 11xxxxxx */
		constexpr uint32_t QOI_OP_RGB = 0xFE;		/* 11111110 */
		constexpr uint32_t QOI_OP_RGBA = 0xFF;		/* 11111111 */


#define QOI_COLOR_HASH( C )							(C.rgba.r * 3 + C.rgba.g * 5 + C.rgba.b * 7 + C.rgba.a * 11)
#define QOI_ZEROARR( a )							CStd::MemSet( (a), 0, sizeof( a ) )
		
		constexpr uint32_t QOI_MAGIC = (uint32_t( 'q' ) << 24) |
			(uint32_t( 'o' ) << 16) |
			(uint32_t( 'i' ) <<  8) |
			uint32_t( 'f' );
		constexpr uint32_t QOI_HEADER_SIZE = 14;
		/* 2GB is the max file size that this implementation can safely handle. We guard
		against anything larger than that, assuming the worst case with 5 bytes per
		pixel, rounded down to a nice clean value. 400 million pui8Pixels ought to be
		enough for anybody. */
		constexpr uint32_t QOI_PIXELS_MAX = 400000000;
		constexpr uint32_t QOI_SRGB = 0;
		constexpr uint32_t QOI_LINEAR = 1;

		typedef union {
			struct { uint8_t r, g, b, a; } rgba;
			uint32_t v;
		} QOI_RGBA;

		static const unsigned char qoi_padding[8] = {0,0,0,0,0,0,0,1};

		auto Write32 = [=](uint8_t *pui8Bytes, uint32_t *ui32Pos, uint32_t v) {
			pui8Bytes[(*ui32Pos)++] = uint8_t( (0xFF000000 & v) >> 24 );
			pui8Bytes[(*ui32Pos)++] = uint8_t( (0x00FF0000 & v) >> 16 );
			pui8Bytes[(*ui32Pos)++] = uint8_t( (0x0000FF00 & v) >> 8 );
			pui8Bytes[(*ui32Pos)++] = uint8_t( 0x000000FF & v );
		};

		LSSTD_ERRORS eError = LSSTD_E_SUCCESS;

		uint32_t ui32Channels = CImageLib::GetTotalComponents( _iImage.GetFormat() );
		uint32_t ui32ColorSpace = _iImage.IsSRgb() ? QOI_SRGB : QOI_LINEAR;
		CImage iImage;
		_iImage.ConvertToFormat( ui32Channels == 4 ? LSI_PF_R8G8B8A8 : LSI_PF_R8G8B8, iImage );
		iImage.SwapChannels( LSI_PC_R, LSI_PC_B );
		const uint8_t * pui8Pixels = reinterpret_cast<const uint8_t *>(iImage.GetBufferData());



		QOI_RGBA rIndex[64];
		QOI_RGBA rPx, rPxPrev;

		if ( iImage.GetWidth() == 0 || iImage.GetHeight() == 0 ) {
			eError = LSSTD_E_INVALIDDATA;
			::printf( "QOI width and height must not be 0: (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		if ( ui32Channels < 3 || ui32Channels > 4 ) {
			eError = LSSTD_E_INVALIDDATA;
			::printf( "QOI files must have either 3 or 4 channels: (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		if ( iImage.GetHeight() >= QOI_PIXELS_MAX / iImage.GetWidth() ) {
			eError = LSSTD_E_INVALIDDATA;
			::printf( "QOI file size too large: (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}

		uint32_t ui32MaxSize =
			iImage.GetWidth() * iImage.GetHeight() * (ui32Channels + 1) +
			QOI_HEADER_SIZE + sizeof( qoi_padding );

		uint32_t ui32Pos = 0;
		if ( !_mfFile.Resize( ui32MaxSize ) ) {
			eError = LSSTD_E_OUTOFMEMORY;
			::printf( "Unable to allocate memory for QOI files (%s).\r\n", _oOptions.slInputs[_ui32FileIndex].CStr() );
			return eError;
		}
		uint8_t * pui8Bytes = &_mfFile[0];

		Write32( pui8Bytes, &ui32Pos, QOI_MAGIC );
		Write32( pui8Bytes, &ui32Pos, iImage.GetWidth() );
		Write32( pui8Bytes, &ui32Pos, iImage.GetHeight() );
		pui8Bytes[ui32Pos++] = uint8_t( ui32Channels );
		pui8Bytes[ui32Pos++] = uint8_t( ui32ColorSpace );


		QOI_ZEROARR( rIndex );

		uint32_t ui32Run = 0;
		rPxPrev.rgba.r = 0;
		rPxPrev.rgba.g = 0;
		rPxPrev.rgba.b = 0;
		rPxPrev.rgba.a = 255;
		rPx = rPxPrev;

		uint32_t ui32PxLen = iImage.GetWidth() * iImage.GetHeight() * ui32Channels;
		uint32_t ui32PxEnd = ui32PxLen - ui32Channels;

		for ( uint32_t px_pos = 0; px_pos < ui32PxLen; px_pos += ui32Channels ) {
			rPx.rgba.r = pui8Pixels[px_pos+0];
			rPx.rgba.g = pui8Pixels[px_pos+1];
			rPx.rgba.b = pui8Pixels[px_pos+2];

			if ( ui32Channels == 4 ) {
				rPx.rgba.a = pui8Pixels[px_pos+3];
			}

			if ( rPx.v == rPxPrev.v ) {
				ui32Run++;
				if ( ui32Run == 62 || px_pos == ui32PxEnd ) {
					pui8Bytes[ui32Pos++] = uint8_t( QOI_OP_RUN | (ui32Run - 1) );
					ui32Run = 0;
				}
			}
			else {
				int32_t i32IdxPos;

				if ( ui32Run > 0 ) {
					pui8Bytes[ui32Pos++] = uint8_t( QOI_OP_RUN | (ui32Run - 1) );
					ui32Run = 0;
				}

				i32IdxPos = QOI_COLOR_HASH( rPx ) % 64;

				if ( rIndex[i32IdxPos].v == rPx.v ) {
					pui8Bytes[ui32Pos++] = uint8_t( QOI_OP_INDEX | i32IdxPos );
				}
				else {
					rIndex[i32IdxPos] = rPx;

					if ( rPx.rgba.a == rPxPrev.rgba.a ) {
						int8_t i8Vr = rPx.rgba.r - rPxPrev.rgba.r;
						int8_t i8Vg = rPx.rgba.g - rPxPrev.rgba.g;
						int8_t i8Vb = rPx.rgba.b - rPxPrev.rgba.b;

						int8_t i8VgR = i8Vr - i8Vg;
						int8_t i8VgB = i8Vb - i8Vg;

						if ( i8Vr > -3 && i8Vr < 2 &&
							i8Vg > -3 && i8Vg < 2 &&
							i8Vb > -3 && i8Vb < 2 ) {
							pui8Bytes[ui32Pos++] = QOI_OP_DIFF | (i8Vr + 2) << 4 | (i8Vg + 2) << 2 | (i8Vb + 2);
						}
						else if ( i8VgR > -9 && i8VgR < 8 &&
							i8Vg > -33 && i8Vg < 32 &&
							i8VgB > -9 && i8VgB < 8 ) {
							pui8Bytes[ui32Pos++] = QOI_OP_LUMA | (i8Vg   + 32);
							pui8Bytes[ui32Pos++] = (i8VgR + 8) << 4 | (i8VgB +  8);
						}
						else {
							pui8Bytes[ui32Pos++] = QOI_OP_RGB;
							pui8Bytes[ui32Pos++] = rPx.rgba.r;
							pui8Bytes[ui32Pos++] = rPx.rgba.g;
							pui8Bytes[ui32Pos++] = rPx.rgba.b;
						}
					}
					else {
						pui8Bytes[ui32Pos++] = QOI_OP_RGBA;
						pui8Bytes[ui32Pos++] = rPx.rgba.r;
						pui8Bytes[ui32Pos++] = rPx.rgba.g;
						pui8Bytes[ui32Pos++] = rPx.rgba.b;
						pui8Bytes[ui32Pos++] = rPx.rgba.a;
					}
				}
			}
			rPxPrev = rPx;
		}

		for ( size_t I = 0; I < sizeof( qoi_padding ); I++ ) {
			pui8Bytes[ui32Pos++] = qoi_padding[I];
		}

		_mfFile.Resize( ui32Pos );

		return eError;

#undef QOI_ZEROARR
#undef QOI_COLOR_HASH
	}

	/**
	 * Gets the actual mipmap count.
	 *
	 * \param _iImage The source image.
	 * \param _oOptions The options.
	 * \return Returns the total actual mipmap levels.
	 */
	uint32_t LSE_CALL CDxt::GetActualMips( const CImage &_iImage, const LSX_OPTIONS &_oOptions ) {
		LSBOOL bMips = _oOptions.bMipMaps && _oOptions.ui32Mips >= 1 && _iImage.GetWidth() > 1 && _iImage.GetHeight() > 1;
		uint32_t ui32Mips = 1;
		if ( bMips ) {
			ui32Mips = 0;
			uint32_t ui32CopyW = _iImage.GetWidth();
			uint32_t ui32CopyH = _iImage.GetHeight();
			while ( ui32CopyW || ui32CopyH ) {
				++ui32Mips;
				ui32CopyW >>= 1;
				ui32CopyH >>= 1;
			}
			ui32Mips = CStd::Min( ui32Mips, _oOptions.ui32Mips );
		}
		return ui32Mips;
	}

	/**
	 * Converts a given KTX error into one of our error codes.
	 *
	 * \param _eError The KTX error code.
	 * \return Returns one of our error codes.
	 */
	LSSTD_ERRORS LSE_CALL CDxt::KtxErrorToError( KTX_error_code _eError ) {
		switch ( _eError ) {
			case KTX_SUCCESS : {
				return LSSTD_E_SUCCESS;
			}
			case KTX_FILE_OVERFLOW : {
				return LSSTD_E_FILEOVERFLOW;
			}
			case KTX_FILE_WRITE_ERROR : {
				return LSSTD_E_FILEWRITEERROR;
			}
			case KTX_INVALID_OPERATION : {
				return LSSTD_E_INTERNALERROR;
			}
			case KTX_INVALID_VALUE : {
				return LSSTD_E_INVALIDDATA;
			}
			case KTX_OUT_OF_MEMORY : {
				return LSSTD_E_OUTOFMEMORY;
			}
			case KTX_UNSUPPORTED_TEXTURE_TYPE : {
				return LSSTD_E_BADVERSION;
			}
		}
		assert( 0 );
	}

	/**
	 * Converts from our options to a CImage::LSI_DXT_OPTIONS structure.
	 *
	 * \param _oOptions The options from which to derive the values for the returned CImage::LSI_DXT_OPTIONS object.
	 * \return Returns an CImage::LSI_DXT_OPTIONS object created based off values set in _oOptions.
	 */
	CImage::LSI_DXT_OPTIONS LSE_CALL CDxt::OptionsToDssOptions( const LSX_OPTIONS &_oOptions ) {
		CImage::LSI_DXT_OPTIONS doOptions;
		doOptions.fAlphaThresh = _oOptions.ui8AlphaThresh / 255.0f;
		LSX_VECTOR vWeights( _oOptions.fRedWeight, _oOptions.fGreenWeight, _oOptions.fBlueWeight );
		vWeights *= 1.0f / (vWeights.x + vWeights.y + vWeights.z);	// Normalize to add up to 1.

		doOptions.fRedWeight = vWeights.x;
		doOptions.fGreenWeight = vWeights.y;
		doOptions.fBlueWeight = vWeights.z;

		return doOptions;
	}

	/**
	 * Converts from our options to a CImage::LSI_ETC_OPTIONS structure.
	 *
	 * \param _oOptions The options from which to derive the values for the returned CImage::LSI_ETC_OPTIONS object.
	 * \return Returns an CImage::LSI_ETC_OPTIONS object created based off values set in _oOptions.
	 */
	CImage::LSI_ETC_OPTIONS LSE_CALL CDxt::OptionsToEtcOptions( const LSX_OPTIONS &_oOptions ) {
		CImage::LSI_ETC_OPTIONS eoOptions;
		switch ( _oOptions.eeErrorMetric ) {
			case CDxt::LSX_E_PERCEPTUAL : {
				eoOptions.ui32Error = METRIC_PERCEPTUAL;
				break;
			}
			case CDxt::LSX_E_NONPERCEPTUAL : {
				eoOptions.ui32Error = METRIC_NONPERCEPTUAL;
				break;
			}
		}
		switch ( _oOptions.efEtcFormat ) {
			case CDxt::LSE_EF_R : {
				eoOptions.ui32Format = ETC2PACKAGE_R_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_RG : {
				eoOptions.ui32Format = ETC2PACKAGE_RG_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_SIGNED_R : {
				eoOptions.ui32Format = ETC2PACKAGE_R_SIGNED_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_SIGNED_RG : {
				eoOptions.ui32Format = ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_RGB : {
				eoOptions.ui32Format = ETC2PACKAGE_RGB_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_RGB_A1 : {
				eoOptions.ui32Format = ETC2PACKAGE_RGBA1_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_RGB_A8 : {
				eoOptions.ui32Format = ETC2PACKAGE_RGBA_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_SRGB : {
				eoOptions.ui32Format = ETC2PACKAGE_sRGB_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_SRGB_A1 : {
				eoOptions.ui32Format = ETC2PACKAGE_sRGBA1_NO_MIPMAPS;
				break;
			}
			case CDxt::LSE_EF_SRGB_A8 : {
				eoOptions.ui32Format = ETC2PACKAGE_sRGBA_NO_MIPMAPS;
				break;
			}
		}
		switch ( _oOptions.etEtcType ) {
			case CDxt::LSX_ET_ETC1 : {
				eoOptions.ui32Codec = CODEC_ETC;
				break;
			}
			case CDxt::LSX_ET_ETC2 : {
				eoOptions.ui32Codec = CODEC_ETC2;
				break;
			}
		}
		switch ( _oOptions.qQuality ) {
			case CDxt::LSX_Q_QUICK : {
				eoOptions.ui32Speed = SPEED_FAST;
				break;
			}
			case CDxt::LSX_Q_NORMAL : {
				eoOptions.ui32Speed = SPEED_MEDIUM;
				break;
			}
			case LSX_Q_BEST : {}
			case CDxt::LSX_Q_PRODUCTION : {
				eoOptions.ui32Speed = SPEED_SLOW;
				break;
			}
		}
		return eoOptions;
	}

	/**
	 * Returns true if the given KTX1 format is any of the DXT/BC textures (uses the LSI_DXT_OPTIONS structure).
	 *
	 * \param _kifFormat Thge format to check.
	 * \return Returns true if the given KTX1 format is any of the DXT/BC textures (uses the LSI_DXT_OPTIONS structure).
	 */
	bool LSE_CALL CDxt::IsDxt( LSI_KTX_INTERNAL_FORMAT _kifFormat ) {
		switch ( _kifFormat ) {
			case LSI_KIF_GL_COMPRESSED_RGB_S3TC_DXT1_EXT : {}
			case LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : {}
			case LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : {}
			case LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : {}
			case LSI_KIF_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT : {}
			case LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT : {}
			case LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT : {}
			case LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : {}
			case LSI_KIF_GL_COMPRESSED_LUMINANCE_LATC1_EXT : {}
			case LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT : {}
			case LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT : {}
			case LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT : {}
			case LSI_KIF_GL_COMPRESSED_RED_RGTC1 : {}
			case LSI_KIF_GL_COMPRESSED_RG_RGTC2 : {}
			case LSI_KIF_GL_COMPRESSED_SIGNED_RED_RGTC1 : {}
			case LSI_KIF_GL_COMPRESSED_SIGNED_RG_RGTC2 : {}
			case LSI_KIF_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT : {}
			case LSI_KIF_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT : {}
			case LSI_KIF_GL_COMPRESSED_RGBA_BPTC_UNORM : {}
			case LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM : { return true; }
		}
		return false;
	}

	/**
	 * Returns true if the given KTX1 format is any of the ETC textures (uses the LSI_ETC_OPTIONS structure).
	 *
	 * \param _kifFormat Thge format to check.
	 * \return Returns true if the given KTX1 format is any of the ETC textures (uses the LSI_ETC_OPTIONS structure).
	 */
	bool LSE_CALL CDxt::IsEtc( LSI_KTX_INTERNAL_FORMAT _kifFormat ) {
		switch ( _kifFormat ) {
			case LSI_KIF_GL_ETC1_RGB8_OES : {}
			case LSI_KIF_GL_COMPRESSED_RGB8_ETC2 : {}
			case LSI_KIF_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 : {}
			case LSI_KIF_GL_COMPRESSED_RGBA8_ETC2_EAC : {}
			case LSI_KIF_GL_COMPRESSED_SRGB8_ETC2 : {}
			case LSI_KIF_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 : {}
			case LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC : {}
			case LSI_KIF_GL_COMPRESSED_R11_EAC : {}
			case LSI_KIF_GL_COMPRESSED_RG11_EAC : {}
			case LSI_KIF_GL_COMPRESSED_SIGNED_R11_EAC : {}
			case LSI_KIF_GL_COMPRESSED_SIGNED_RG11_EAC : { return true; }
		}
		return false;
	}

}	// namespace lsx
