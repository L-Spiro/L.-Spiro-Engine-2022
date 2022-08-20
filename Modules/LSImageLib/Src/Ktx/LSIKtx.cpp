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
 * Description: Handles loading of .DDS files.
 */

#include "LSIKtx.h"
#include "../Astc/LSIAstc.h"
#include "../Dds/LSIDds.h"
#include "../Etc/LSIEtc.h"
#include "../KTX-Software/lib/gl_format.h"
#include "../Pvrtc/LSIPvrtc.h"

#ifdef LSI_PRINT_TABLE
#include <cstring>
#endif	// #ifdef LSI_PRINT_TABLE

namespace lsi {

	// == Members.
	/** The data for internal formats. */
	const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA CKtx::m_kifdInternalFormats[] = {
#if 0
#ifdef RGB
#undef RGB
#endif	// RGB
#define LSI_KTX_IF_ENTRY( NAME, BASE, TEXELSIZE, SRGB )				{ LSI_KIF_GL_ ## NAME, "GL_" #NAME, LSI_KBIF_GL_ ## BASE, TEXELSIZE, false, false, SRGB }
		LSI_KTX_IF_ENTRY( R8, RED, 1, false ),
		LSI_KTX_IF_ENTRY( R16, RED, 2, false ),
		LSI_KTX_IF_ENTRY( RG8, RG, 2, false ),
		LSI_KTX_IF_ENTRY( RG16, RG, 4, false ),
		LSI_KTX_IF_ENTRY( R16F, RED, 2, false ),
		LSI_KTX_IF_ENTRY( R32F, RED, 4, false ),
		LSI_KTX_IF_ENTRY( RG16F, RG, 4, false ),
		LSI_KTX_IF_ENTRY( RG32F, RG, 8, false ),
		LSI_KTX_IF_ENTRY( R8I, RED, 1, false ),
		LSI_KTX_IF_ENTRY( R8UI, RED, 1, false ),
		LSI_KTX_IF_ENTRY( R16I, RED, 2, false ),
		LSI_KTX_IF_ENTRY( R16UI, RED, 2, false ),
		LSI_KTX_IF_ENTRY( R32I, RED, 4, false ),
		LSI_KTX_IF_ENTRY( R32UI, RED, 4, false ),
		LSI_KTX_IF_ENTRY( RG8I, RG, 2, false ),
		LSI_KTX_IF_ENTRY( RG8UI, RG, 2, false ),
		LSI_KTX_IF_ENTRY( RG16I, RG, 4, false ),
		LSI_KTX_IF_ENTRY( RG16UI, RG, 4, false ),
		LSI_KTX_IF_ENTRY( RG32I, RG, 8, false ),
		LSI_KTX_IF_ENTRY( RG32UI, RG, 8, false ),
		
		LSI_KTX_IF_ENTRY( R8_SNORM, RED, 1, false ),
		LSI_KTX_IF_ENTRY( RG8_SNORM, RG, 2, false ),
		LSI_KTX_IF_ENTRY( RGB8_SNORM, RGB, 3, false ),
		LSI_KTX_IF_ENTRY( RGBA8_SNORM, RGBA, 4, false ),
		LSI_KTX_IF_ENTRY( R16_SNORM, RED, 2, false ),
		LSI_KTX_IF_ENTRY( RG16_SNORM, RG, 4, false ),
		LSI_KTX_IF_ENTRY( RGB16_SNORM, RGB, 6, false ),
		LSI_KTX_IF_ENTRY( RGBA16_SNORM, RGBA, 8, false ),

		LSI_KTX_IF_ENTRY( R3_G3_B2, RGB, 1, false ),
		LSI_KTX_IF_ENTRY( RGB4, RGB, 2, false ),
		LSI_KTX_IF_ENTRY( RGB5, RGB, 2, false ),
		LSI_KTX_IF_ENTRY( RGB8, RGB, 3, false ),
		LSI_KTX_IF_ENTRY( RGB10, RGB, 4, false ),
		LSI_KTX_IF_ENTRY( RGB12, RGB, 5, false ),
		LSI_KTX_IF_ENTRY( RGB16, RGB, 6, false ),
		LSI_KTX_IF_ENTRY( RGBA2, RGBA, 1, false ),
		LSI_KTX_IF_ENTRY( RGBA4, RGBA, 2, false ),
		LSI_KTX_IF_ENTRY( RGB5_A1, RGBA, 2, false ),
		LSI_KTX_IF_ENTRY( RGBA8, RGBA, 4, false ),
		LSI_KTX_IF_ENTRY( RGB10_A2, RGBA, 4, false ),
		LSI_KTX_IF_ENTRY( RGBA12, RGBA, 6, false ),
		LSI_KTX_IF_ENTRY( RGBA16, RGBA, 8, false ),

		LSI_KTX_IF_ENTRY( RGB10_A2UI, RGBA, 8, false ),

		LSI_KTX_IF_ENTRY( RGB565, RGB, 2, false ),

		//LSI_KTX_IF_ENTRY( LUMINANCE4_EXT, RGB, 1, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE8_EXT, RGB, 1, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE12_EXT, RGB, 2, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE16_EXT, RGB, 2, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE4_ALPHA4_EXT, RGBA, 1, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE6_ALPHA2_EXT, RGBA, 1, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE8_ALPHA8_EXT, RGBA, 2, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE12_ALPHA4_EXT, RGBA, 2, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE12_ALPHA12_EXT, RGBA, 3, false ),
		//LSI_KTX_IF_ENTRY( LUMINANCE16_ALPHA16_EXT, RGBA, 4, false ),

		//LSI_KTX_IF_ENTRY( SRGB_EXT, RGB, 3, true ),
		LSI_KTX_IF_ENTRY( SRGB8, RGB, 3, true ),
		//LSI_KTX_IF_ENTRY( SRGB_ALPHA_EXT, RGBA, 4, true ),
		LSI_KTX_IF_ENTRY( SRGB8_ALPHA8, RGBA, 4, true ),
		//LSI_KTX_IF_ENTRY( SLUMINANCE_ALPHA_EXT, RGBA, 2, false ),
		//LSI_KTX_IF_ENTRY( SLUMINANCE8_ALPHA8_EXT, RGBA, 2, false ),
		//LSI_KTX_IF_ENTRY( SLUMINANCE_EXT, RGB, 1, false ),
		//LSI_KTX_IF_ENTRY( SLUMINANCE8_EXT, RGB, 1, false ),
		//LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_EXT, RGB, 0, true ),
		//LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_EXT, RGBA, 0, true ),
		//LSI_KTX_IF_ENTRY( COMPRESSED_SLUMINANCE_EXT, RGB, 0, false ),
		//LSI_KTX_IF_ENTRY( COMPRESSED_SLUMINANCE_ALPHA_EXT, RGBA, 0, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_S3TC_DXT1_EXT, RGB, 8, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, RGBA, 8, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, RGBA, 16, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, RGBA, 16, true ),

		LSI_KTX_IF_ENTRY( RGBA32F, RGBA, 4 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB32F, RGB, 4 * 3, false ),
		LSI_KTX_IF_ENTRY( RGBA16F, RGBA, 2 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB16F, RGB, 2 * 3, false ),

		LSI_KTX_IF_ENTRY( R11F_G11F_B10F, RGB, 4, false ),
		LSI_KTX_IF_ENTRY( RGB9_E5, RGB, 4, false ),

		LSI_KTX_IF_ENTRY( RGBA32UI, RGBA, 4 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB32UI, RGB, 4 * 3, false ),
		LSI_KTX_IF_ENTRY( RGBA16UI, RGBA, 2 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB16UI, RGB, 2 * 3, false ),
		LSI_KTX_IF_ENTRY( RGBA8UI, RGBA, 1 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB8UI, RGB, 1 * 3, false ),
		LSI_KTX_IF_ENTRY( RGBA32I, RGBA, 4 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB32I, RGB, 4 * 3, false ),
		LSI_KTX_IF_ENTRY( RGBA16I, RGBA, 2 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB16I, RGB, 2 * 3, false ),
		LSI_KTX_IF_ENTRY( RGBA8I, RGBA, 1 * 4, false ),
		LSI_KTX_IF_ENTRY( RGB8I, RGB, 1 * 3, false ),

		LSI_KTX_IF_ENTRY( DEPTH_COMPONENT16, DEPTH_COMPONENT, 2, false ),
		LSI_KTX_IF_ENTRY( DEPTH_COMPONENT24, DEPTH_COMPONENT, 3, false ),
		LSI_KTX_IF_ENTRY( DEPTH_COMPONENT32, DEPTH_COMPONENT, 4, false ),
		LSI_KTX_IF_ENTRY( DEPTH_COMPONENT32F, DEPTH_COMPONENT, 4, false ),
		LSI_KTX_IF_ENTRY( DEPTH24_STENCIL8, DEPTH_STENCIL, 4, false ),
		LSI_KTX_IF_ENTRY( DEPTH32F_STENCIL8, DEPTH_STENCIL, 5, false ),
		LSI_KTX_IF_ENTRY( STENCIL_INDEX1, STENCIL_INDEX, 1, false ),
		LSI_KTX_IF_ENTRY( STENCIL_INDEX4, STENCIL_INDEX, 1, false ),
		LSI_KTX_IF_ENTRY( STENCIL_INDEX8, STENCIL_INDEX, 1, false ),
		LSI_KTX_IF_ENTRY( STENCIL_INDEX16, STENCIL_INDEX, 2, false ),

#undef LSI_KTX_IF_ENTRY
#define LSI_KTX_IF_ENTRY( NAME, BASE, TEXELSIZE, SRGB )				{ LSI_KIF_GL_ ## NAME, "GL_" #NAME, LSI_KBIF_GL_ ## BASE, TEXELSIZE, true, false, SRGB }

		LSI_KTX_IF_ENTRY( COMPRESSED_RED, RED, 0, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RG, RG, 0, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGB, RGB, 0, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA, RGBA, 0, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB, RGB, 0, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA, RGBA, 0, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RED_RGTC1, RED, 64 / 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SIGNED_RED_RGTC1, RED, 64 / 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RG_RGTC2, RG, 128 / 8, false ),
		
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_BPTC_UNORM, RGBA, 16, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_BPTC_UNORM, RGBA, 16, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGB_BPTC_SIGNED_FLOAT, RGB, 16, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, RGB, 16, false ),

		LSI_KTX_IF_ENTRY( ETC1_RGB8_OES, RGB, 64 / 8, false ),

		LSI_KTX_IF_ENTRY( COMPRESSED_RGB8_ETC2, RGB, 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ETC2, RGB, 8, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, RGB, 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, RGB, 8, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA8_ETC2_EAC, RGBA, 16, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, RGBA, 16, true ),
		LSI_KTX_IF_ENTRY( COMPRESSED_R11_EAC, RED, 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SIGNED_R11_EAC, RED, 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RG11_EAC, RG, 16, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SIGNED_RG11_EAC, RG, 16, false ),

		LSI_KTX_IF_ENTRY( COMPRESSED_LUMINANCE_LATC1_EXT, RGB, 64 / 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT, RGB, 64 / 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, RGBA, 128 / 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT, RGBA, 128 / 8, false ),

		LSI_KTX_IF_ENTRY( COMPRESSED_RGB_S3TC_DXT1_EXT, RGB, 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_S3TC_DXT1_EXT, RGBA, 8, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_S3TC_DXT3_EXT, RGBA, 16, false ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_S3TC_DXT5_EXT, RGBA, 16, false ),

#undef LSI_KTX_IF_ENTRY
#define LSI_KTX_IF_ENTRY( NAME, BASE, TEXELSIZE, SRGB )				{ LSI_KIF_GL_ ## NAME, "GL_" #NAME, LSI_KBIF_GL_ ## BASE, TEXELSIZE, false, true, SRGB }

		LSI_KTX_IF_ENTRY( PALETTE4_RGB8_OES, RGB, 4, false ),		// Bytes changed to bits.
		LSI_KTX_IF_ENTRY( PALETTE4_RGBA8_OES, RGBA, 4, false ),
		LSI_KTX_IF_ENTRY( PALETTE4_R5_G6_B5_OES, RGB, 4, false ),
		LSI_KTX_IF_ENTRY( PALETTE4_RGBA4_OES, RGBA, 4, false ),
		LSI_KTX_IF_ENTRY( PALETTE4_RGB5_A1_OES, RGBA, 4, false ),
		LSI_KTX_IF_ENTRY( PALETTE8_RGB8_OES, RGB, 8, false ),		// Bytes changed to bits.
		LSI_KTX_IF_ENTRY( PALETTE8_RGBA8_OES, RGBA, 8, false ),
		LSI_KTX_IF_ENTRY( PALETTE8_R5_G6_B5_OES, RGB, 8, false ),
		LSI_KTX_IF_ENTRY( PALETTE8_RGBA8_OES, RGBA, 8, false ),
		LSI_KTX_IF_ENTRY( PALETTE8_RGB5_A1_OES, RGBA, 8, false ),

#undef LSI_KTX_IF_ENTRY
#define LSI_KTX_IF_ENTRY( NAME, BASE, TEXELSIZE, SRGB, FUNC )		{ LSI_KIF_GL_ ## NAME, "GL_" #NAME, LSI_KBIF_GL_ ## BASE, TEXELSIZE, true, false, SRGB, FUNC }

		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_4x4_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<4, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_5x4_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<5, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_5x5_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<5, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_6x5_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<5, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_6x6_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<6, 6>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_8x5_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<8, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_8x6_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<8, 6>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_8x8_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<8, 8>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_10x5_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<10, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_10x6_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<10, 6>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_10x8_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<10, 8>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_10x10_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<10, 10>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_12x10_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<12, 10>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_ASTC_12x12_KHR, RGBA, 128 / 8, false, &(CKtx::GetCompressedSizeAstc<12, 12>) ),

		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<4, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<5, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<5, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<6, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<6, 6>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<8, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<8, 6>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<8, 8>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<10, 5>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<10, 6>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<10, 8>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<10, 10>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<12, 10>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, RGBA, 128 / 8, true, &(CKtx::GetCompressedSizeAstc<12, 12>) ),

		LSI_KTX_IF_ENTRY( COMPRESSED_RGB_PVRTC_4BPPV1_IMG, RGB, 8, false, &(GetCompressedSizePvrtc<4, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGB_PVRTC_2BPPV1_IMG, RGB, 8, false, &(GetCompressedSizePvrtc<8, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, RGBA, 8, false, &(GetCompressedSizePvrtc<4, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, RGBA, 8, false, &(GetCompressedSizePvrtc<8, 4>) ),

		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_PVRTC_2BPPV1_EXT, RGB, 8, true, &(GetCompressedSizePvrtc<8, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_PVRTC_4BPPV1_EXT, RGB, 8, true, &(GetCompressedSizePvrtc<4, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT, RGBA, 8, true, &(GetCompressedSizePvrtc<8, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT, RGBA, 8, true, &(GetCompressedSizePvrtc<4, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG, RGBA, 8, true, &(GetCompressedSizePvrtc<8, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG, RGBA, 8, true, &(GetCompressedSizePvrtc<4, 4>) ),

		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, RGBA, 8, true, &(GetCompressedSizePvrtc<8, 4>) ),
		LSI_KTX_IF_ENTRY( COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, RGBA, 8, true, &(GetCompressedSizePvrtc<4, 4>) ),
		
#undef LSI_KTX_IF_ENTRY
#else
#define LSI_TBITS( R, G, B, A )													R, G, B, A	// Just helps to label template parameters.
#define LSI_TSHIFTS( R, G, B, A )												R, G, B, A	// Just helps to label template parameters.
#define LSI_LBITS( L, A )														L, A		// Just helps to label template parameters.
#define LSI_LSHIFTS( L, A )														L, A		// Just helps to label template parameters.
#define LSI_IBITS( I )															I			// Just helps to label template parameters.
#define LSI_CONV_I( TTO, TFROM, TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )	(TTO<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>), (TFROM<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>)
#define LSI_CONV_F( TTO, TFROM, TBITS, TSHIFTS, TSIZE )							(TTO<TBITS, TSHIFTS, TSIZE>), (TFROM<TBITS, TSHIFTS, TSIZE>)
#define LSI_CONV_L( TTO, TFROM, TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )	(CKtx::LumAlphaToRgba32F<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>), (CKtx::LumAlphaFromRgba32F<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>)
#define LSI_CONV_IN( TTO, TFROM, TBITS, TSIZE, SIGNED, NORM, TFLOAT )			(TTO<TBITS, TSIZE, SIGNED, NORM, TFLOAT>), (TFROM<TBITS, TSIZE, SIGNED, NORM, TFLOAT>)
#define LSI_GEN_INT( TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )				LSI_CONV_I( CKtx::StdIntToRgba32F, CKtx::StdIntFromRgba32F, TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )

		{ LSI_KIF_GL_R8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_RG8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 0, 0), LSI_TSHIFTS(0, 8, 0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_RGB8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ LSI_KIF_GL_RGBA8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ LSI_KIF_GL_R8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_RED, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 1, true, true, false ) },
		{ LSI_KIF_GL_RG8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_RG, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 0, 0), LSI_TSHIFTS(0, 8, 0, 0), 2, true, true, false ) },
		{ LSI_KIF_GL_RGB8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, true, true, false ) },
		{ LSI_KIF_GL_RGBA8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, true, true, false ) },
		{ LSI_KIF_GL_R8UI, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 1, false, false, false ) },
		{ LSI_KIF_GL_RG8UI, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 0, 0), LSI_TSHIFTS(0, 8, 0, 0), 2, false, false, false ) },
		{ LSI_KIF_GL_RGB8UI, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, false, false ) },
		{ LSI_KIF_GL_RGBA8UI, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, false, false, false ) },
		{ LSI_KIF_GL_R8I, LSI_KT_GL_BYTE, LSI_KBIF_GL_RED, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 1, true, false, false ) },
		{ LSI_KIF_GL_RG8I, LSI_KT_GL_BYTE, LSI_KBIF_GL_RG, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 0, 0), LSI_TSHIFTS(0, 8, 0, 0), 2, true, false, false ) },
		{ LSI_KIF_GL_RGB8I, LSI_KT_GL_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, true, false, false ) },
		{ LSI_KIF_GL_RGBA8I, LSI_KT_GL_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, true, false, false ) },
		{ LSI_KIF_GL_SR8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 1, false, true, true ) },
		{ LSI_KIF_GL_SRG8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 0, 0), LSI_TSHIFTS(0, 8, 0, 0), 2, false, true, true ) },
		{ LSI_KIF_GL_SRGB8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, true, true ) },
		{ LSI_KIF_GL_SRGB8_ALPHA8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, false, true, true ) },
		{ LSI_KIF_GL_R16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RED, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_RG16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RG, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 0, 0), LSI_TSHIFTS(0, 16, 0, 0), 4, false, true, false ) },
		{ LSI_KIF_GL_RGB16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGB, 0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ LSI_KIF_GL_RGBA16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGBA, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 16), LSI_TSHIFTS(0, 16, 32, 48), 8, false, true, false ) },
		{ LSI_KIF_GL_R16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_RED, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 2, true, true, false ) },
		{ LSI_KIF_GL_RG16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_RG, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 0, 0), LSI_TSHIFTS(0, 16, 0, 0), 4, true, true, false ) },
		{ LSI_KIF_GL_RGB16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_RGB, 0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6, true, true, false ) },
		{ LSI_KIF_GL_RGBA16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_RGBA, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 16), LSI_TSHIFTS(0, 16, 32, 48), 8, true, true, false ) },
		{ LSI_KIF_GL_R16UI, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RED, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 2, false, false, false ) },
		{ LSI_KIF_GL_RG16UI, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RG, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 0, 0), LSI_TSHIFTS(0, 16, 0, 0), 4, false, false, false ) },
		{ LSI_KIF_GL_RGB16UI, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGB, 0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6, false, false, false ) },
		{ LSI_KIF_GL_RGBA16UI, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGBA, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 16), LSI_TSHIFTS(0, 16, 32, 48), 8, false, false, false ) },
		{ LSI_KIF_GL_R16I, LSI_KT_GL_SHORT, LSI_KBIF_GL_RED, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 2, true, false, false ) },
		{ LSI_KIF_GL_RG16I, LSI_KT_GL_SHORT, LSI_KBIF_GL_RG, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 0, 0), LSI_TSHIFTS(0, 16, 0, 0), 4, true, false, false ) },
		{ LSI_KIF_GL_RGB16I, LSI_KT_GL_SHORT, LSI_KBIF_GL_RGB, 0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6, true, false, false ) },
		{ LSI_KIF_GL_RGBA16I, LSI_KT_GL_SHORT, LSI_KBIF_GL_RGBA, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 16), LSI_TSHIFTS(0, 16, 32, 48), 8, true, false, false ) },
		{ LSI_KIF_GL_R16F, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_RED, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F16ToRgba32F, CKtx::F16FromRgba32F, LSI_TBITS(16, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 2 ) },
		{ LSI_KIF_GL_RG16F, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_RG, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F16ToRgba32F, CKtx::F16FromRgba32F, LSI_TBITS(16, 16, 0, 0), LSI_TSHIFTS(0, 16, 0, 0), 4 ) },
		{ LSI_KIF_GL_RGB16F, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_RGB, 0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F16ToRgba32F, CKtx::F16FromRgba32F, LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6 ) },
		{ LSI_KIF_GL_RGBA16F, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_RGBA, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F16ToRgba32F, CKtx::F16FromRgba32F, LSI_TBITS(16, 16, 16, 16), LSI_TSHIFTS(0, 16, 32, 48), 8 ) },
		{ LSI_KIF_GL_R32UI, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_RED, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 4, false, false, false ) },
		{ LSI_KIF_GL_RG32UI, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_RG, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 32, 0, 0), LSI_TSHIFTS(0, 32, 0, 0), 8, false, false, false ) },
		{ LSI_KIF_GL_RGB32UI, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_RGB, 0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 32, 32, 0), LSI_TSHIFTS(0, 32, 64, 0), 12, false, false, false ) },
		{ LSI_KIF_GL_RGBA32UI, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_RGBA, 0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 32, 32, 32), LSI_TSHIFTS(0, 32, 64, 96), 16, false, false, false ) },
		{ LSI_KIF_GL_R32I, LSI_KT_GL_INT, LSI_KBIF_GL_RED, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 4, true, false, false ) },
		{ LSI_KIF_GL_RG32I, LSI_KT_GL_INT, LSI_KBIF_GL_RG, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 32, 0, 0), LSI_TSHIFTS(0, 32, 0, 0), 8, true, false, false ) },
		{ LSI_KIF_GL_RGB32I, LSI_KT_GL_INT, LSI_KBIF_GL_RGB, 0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 32, 32, 0), LSI_TSHIFTS(0, 32, 64, 0), 12, true, false, false ) },
		{ LSI_KIF_GL_RGBA32I, LSI_KT_GL_INT, LSI_KBIF_GL_RGBA, 0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_I( CKtx::Int128ToRgba32F, CKtx::Int128FromRgba32F, LSI_TBITS(32, 32, 32, 32), LSI_TSHIFTS(0, 32, 64, 96), 16, true, false, false ) },
		{ LSI_KIF_GL_R32F, LSI_KT_GL_FLOAT, LSI_KBIF_GL_RED, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F32ToRgba32F, CKtx::F32FromRgba32F, LSI_TBITS(32, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 4 ) },
		{ LSI_KIF_GL_RG32F, LSI_KT_GL_FLOAT, LSI_KBIF_GL_RG, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F32ToRgba32F, CKtx::F32FromRgba32F, LSI_TBITS(32, 32, 0, 0), LSI_TSHIFTS(0, 32, 0, 0), 8 ) },
		{ LSI_KIF_GL_RGB32F, LSI_KT_GL_FLOAT, LSI_KBIF_GL_RGB, 0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F32ToRgba32F, CKtx::F32FromRgba32F, LSI_TBITS(32, 32, 32, 0), LSI_TSHIFTS(0, 32, 64, 0), 12 ) },
		{ LSI_KIF_GL_RGBA32F, LSI_KT_GL_FLOAT, LSI_KBIF_GL_RGBA, 0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::F32ToRgba32F, CKtx::F32FromRgba32F, LSI_TBITS(32, 32, 32, 32), LSI_TSHIFTS(0, 32, 64, 96), 16 ) },
		{ LSI_KIF_GL_R3_G3_B2, LSI_KT_GL_UNSIGNED_BYTE_2_3_3_REV, LSI_KBIF_GL_RGB, 0x01, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(3, 3, 2, 0), LSI_TSHIFTS(0, 3, 6, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_RGB4, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x01, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ LSI_KIF_GL_RGB5, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x01, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ LSI_KIF_GL_RGB565, LSI_KT_GL_UNSIGNED_SHORT_5_6_5, LSI_KBIF_GL_RGB, 0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(5, 6, 5, 0), LSI_TSHIFTS(11, 5, 0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_RGB10, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGB, 0x01, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ LSI_KIF_GL_RGB12, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGB, 0x01, 0, 48, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 0), LSI_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ LSI_KIF_GL_RGBA2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ LSI_KIF_GL_RGBA4, LSI_KT_GL_UNSIGNED_SHORT_4_4_4_4, LSI_KBIF_GL_RGBA, 0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(4, 4, 4, 4), LSI_TSHIFTS(12, 8, 4, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_RGBA12, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_RGBA, 0x01, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(16, 16, 16, 16), LSI_TSHIFTS(0, 16, 32, 48), 8, false, true, false ) },
		{ LSI_KIF_GL_RGB5_A1, LSI_KT_GL_UNSIGNED_SHORT_5_5_5_1, LSI_KBIF_GL_RGBA, 0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(5, 5, 5, 1), LSI_TSHIFTS(11, 6, 1, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_RGB10_A2, LSI_KT_GL_UNSIGNED_INT_2_10_10_10_REV, LSI_KBIF_GL_RGBA, 0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(10, 10, 10, 2), LSI_TSHIFTS(0, 10, 20, 30), 4, false, true, false ) },
		{ LSI_KIF_GL_RGB10_A2UI, LSI_KT_GL_UNSIGNED_INT_2_10_10_10_REV, LSI_KBIF_GL_RGBA, 0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(10, 10, 10, 2), LSI_TSHIFTS(0, 10, 20, 30), 4, false, false, false ) },
		{ LSI_KIF_GL_R11F_G11F_B10F, LSI_KT_GL_UNSIGNED_INT_10F_11F_11F_REV, LSI_KBIF_GL_RGB, 0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::R11G11B10FToRgba32F, CKtx::R11G11B10FFromRgba32F },
		{ LSI_KIF_GL_RGB9_E5, LSI_KT_GL_UNSIGNED_INT_5_9_9_9_REV, LSI_KBIF_GL_RGB, 0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::RGB9E5ToRgba32F, CKtx::RGB9E5FromRgba32F },
		{ LSI_KIF_GL_ALPHA4, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_ALPHA, 0x00, 0, 4, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 8), LSI_LSHIFTS(0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_ALPHA8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_ALPHA, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 8), LSI_LSHIFTS(0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_ALPHA8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_ALPHA, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 8), LSI_LSHIFTS(0, 0), 1, true, true, false ) },
		{ LSI_KIF_GL_ALPHA8UI_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_ALPHA_INTEGER, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 8), LSI_LSHIFTS(0, 0), 1, false, false, false ) },
		{ LSI_KIF_GL_ALPHA8I_EXT, LSI_KT_GL_BYTE, LSI_KBIF_GL_ALPHA_INTEGER, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 8), LSI_LSHIFTS(0, 0), 1, true, false, false ) },
		{ LSI_KIF_GL_ALPHA12, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_ALPHA, 0x00, 0, 12, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 16), LSI_LSHIFTS(0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_ALPHA16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 16), LSI_LSHIFTS(0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_ALPHA16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 16), LSI_LSHIFTS(0, 0), 2, true, true, false ) },
		{ LSI_KIF_GL_ALPHA16UI_EXT, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_ALPHA_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 16), LSI_LSHIFTS(0, 0), 2, false, false, false ) },
		{ LSI_KIF_GL_ALPHA16I_EXT, LSI_KT_GL_SHORT, LSI_KBIF_GL_ALPHA_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 16), LSI_LSHIFTS(0, 0), 2, true, false, false ) },
		{ LSI_KIF_GL_ALPHA16F_ARB, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::LumAlphaFToRgba32F, CKtx::LumAlphaFFromRgba32F, LSI_LBITS(0, 16), LSI_LSHIFTS(0, 0), 2 ) },
		{ LSI_KIF_GL_ALPHA32UI_EXT, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_ALPHA_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 32), LSI_LSHIFTS(0, 0), 4, false, false, false ) },
		{ LSI_KIF_GL_ALPHA32I_EXT, LSI_KT_GL_INT, LSI_KBIF_GL_ALPHA_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(0, 32), LSI_LSHIFTS(0, 0), 4, true, false, false ) },
		{ LSI_KIF_GL_ALPHA32F_ARB, LSI_KT_GL_FLOAT, LSI_KBIF_GL_ALPHA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::LumAlphaFToRgba32F, CKtx::LumAlphaFFromRgba32F, LSI_LBITS(0, 32), LSI_LSHIFTS(0, 0), 4 ) },
		{ LSI_KIF_GL_LUMINANCE4, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 4, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, true, true, false ) },
		{ LSI_KIF_GL_SLUMINANCE8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, false, true, true ) },
		{ LSI_KIF_GL_LUMINANCE8UI_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, false, false, false ) },
		{ LSI_KIF_GL_LUMINANCE8I_EXT, LSI_KT_GL_BYTE, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, true, false, false ) },
		{ LSI_KIF_GL_LUMINANCE12, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 12, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 0), LSI_LSHIFTS(0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 0), LSI_LSHIFTS(0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 0), LSI_LSHIFTS(0, 0), 2, true, true, false ) },
		{ LSI_KIF_GL_LUMINANCE16UI_EXT, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 0), LSI_LSHIFTS(0, 0), 2, false, false, false ) },
		{ LSI_KIF_GL_LUMINANCE16I_EXT, LSI_KT_GL_SHORT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 0), LSI_LSHIFTS(0, 0), 2, true, false, false ) },
		{ LSI_KIF_GL_LUMINANCE16F_ARB, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::LumAlphaFToRgba32F, CKtx::LumAlphaFFromRgba32F, LSI_LBITS(16, 0), LSI_LSHIFTS(0, 0), 2 ) },
		{ LSI_KIF_GL_LUMINANCE32UI_EXT, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(32, 0), LSI_LSHIFTS(0, 0), 4, false, false, false ) },
		{ LSI_KIF_GL_LUMINANCE32I_EXT, LSI_KT_GL_INT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(32, 0), LSI_LSHIFTS(0, 0), 4, true, false, false ) },
		{ LSI_KIF_GL_LUMINANCE32F_ARB, LSI_KT_GL_FLOAT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::LumAlphaFToRgba32F, CKtx::LumAlphaFFromRgba32F, LSI_LBITS(32, 0), LSI_LSHIFTS(0, 0), 4 ) },
		{ LSI_KIF_GL_LUMINANCE4_ALPHA4, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE6_ALPHA2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, true, false ) },	// Weird result.
		{ LSI_KIF_GL_LUMINANCE8_ALPHA8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE8_ALPHA8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, true, true, false ) },
		{ LSI_KIF_GL_SLUMINANCE8_ALPHA8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, true, true ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA8UI_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, false, false ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA8I_EXT, LSI_KT_GL_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, true, false, false ) },
		{ LSI_KIF_GL_LUMINANCE12_ALPHA4, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE12_ALPHA12, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE16_ALPHA16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4, false, true, false ) },
		{ LSI_KIF_GL_LUMINANCE16_ALPHA16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4, true, true, false ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA16UI_EXT, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE_ALPHA_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4, false, false, false ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA16I_EXT, LSI_KT_GL_SHORT, LSI_KBIF_GL_LUMINANCE_ALPHA_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4, true, false, false ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA16F_ARB, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::LumAlphaFToRgba32F, CKtx::LumAlphaFFromRgba32F, LSI_LBITS(16, 16), LSI_LSHIFTS(0, 16), 4 ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA32UI_EXT, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_LUMINANCE_ALPHA_INTEGER, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(32, 32), LSI_LSHIFTS(0, 32), 8, false, false, false ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA32I_EXT, LSI_KT_GL_INT, LSI_KBIF_GL_LUMINANCE_ALPHA_INTEGER, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(32, 32), LSI_LSHIFTS(0, 32), 8, true, false, false ) },
		{ LSI_KIF_GL_LUMINANCE_ALPHA32F_ARB, LSI_KT_GL_FLOAT, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_F( CKtx::LumAlphaFToRgba32F, CKtx::LumAlphaFFromRgba32F, LSI_LBITS(32, 32), LSI_LSHIFTS(0, 32), 8 ) },
		{ LSI_KIF_GL_INTENSITY4, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 4, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(8), 1, false, true, false ) },
		{ LSI_KIF_GL_INTENSITY8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(8), 1, false, true, false ) },
		{ LSI_KIF_GL_INTENSITY8_SNORM, LSI_KT_GL_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(8), 1, true, true, false ) },
		{ LSI_KIF_GL_INTENSITY8UI_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(8), 1, false, false, false ) },
		{ LSI_KIF_GL_INTENSITY8I_EXT, LSI_KT_GL_BYTE, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(8), 1, true, false, false ) },
		{ LSI_KIF_GL_INTENSITY12, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 12, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(16), 2, false, true, false ) },
		{ LSI_KIF_GL_INTENSITY16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(16), 2, false, true, false ) },
		{ LSI_KIF_GL_INTENSITY16_SNORM, LSI_KT_GL_SHORT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(16), 2, true, true, false ) },
		{ LSI_KIF_GL_INTENSITY16UI_EXT, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(16), 2, false, false, false ) },
		{ LSI_KIF_GL_INTENSITY16I_EXT, LSI_KT_GL_SHORT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(16), 2, true, false, false ) },
		{ LSI_KIF_GL_INTENSITY16F_ARB, LSI_KT_GL_HALF_FLOAT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(16), 2, false, true, true ) },
		{ LSI_KIF_GL_INTENSITY32UI_EXT, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(32), 4, false, false, false ) },
		{ LSI_KIF_GL_INTENSITY32I_EXT, LSI_KT_GL_INT, LSI_KBIF_GL_LUMINANCE_INTEGER, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(32), 4, true, false, false ) },
		{ LSI_KIF_GL_INTENSITY32F_ARB, LSI_KT_GL_FLOAT, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(32), 4, false, true, true ) },
		{ LSI_KIF_GL_COMPRESSED_RED, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 0, 0, 0), LSI_TSHIFTS(0, 0, 0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_ALPHA, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_ALPHA, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(0, 0, 0, 8), LSI_TSHIFTS(0, 0, 0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_LUMINANCE, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_SLUMINANCE, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 0), LSI_LSHIFTS(0, 0), 1, false, true, true ) },
		{ LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_SLUMINANCE_ALPHA, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE_ALPHA, 0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_CONV_L( CKtx::LumAlphaToRgba32F, CKtx::LumAlphaFromRgba32F, LSI_LBITS(8, 8), LSI_LSHIFTS(0, 8), 2, false, true, true ) },
		{ LSI_KIF_GL_COMPRESSED_INTENSITY, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_LUMINANCE, 0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_CONV_IN( CKtx::IntensityToRgba32F, CKtx::IntensityFromRgba32F, LSI_IBITS(8), 1, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_RG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 0, 0), LSI_TSHIFTS(0, 8, 0, 0), 2, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_RGB, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_RGBA, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ LSI_KIF_GL_COMPRESSED_SRGB, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x00, 0, 24, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 0), LSI_TSHIFTS(0, 8, 16, 0), 3, false, true, true ) },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, nullptr, LSI_GEN_INT( LSI_TBITS(8, 8, 8, 8), LSI_TSHIFTS(0, 8, 16, 24), 4, false, true, true ) },
		{ LSI_KIF_GL_COMPRESSED_RGB_FXT1_3DFX, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_FXT1_3DFX, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGB_S3TC_DXT1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Dxt1ToRgba32F<false>, CDds::Dxt1FromRgba32F<0, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Dxt1ToRgba32F<false>, CDds::Dxt1FromRgba32F<128, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Dxt3ToRgba32F<false>, CDds::Dxt3FromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Dxt5ToRgba32F<false>, CDds::Dxt5FromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, nullptr, CDds::Dxt1ToRgba32F<true>, CDds::Dxt1FromRgba32F<0, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, nullptr, CDds::Dxt1ToRgba32F<true>, CDds::Dxt1FromRgba32F<128, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, nullptr, CDds::Dxt3ToRgba32F<true>, CDds::Dxt3FromRgba32F<true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, nullptr, CDds::Dxt5ToRgba32F<true>, CDds::Dxt5FromRgba32F<true> },
		{ LSI_KIF_GL_COMPRESSED_LUMINANCE_LATC1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc4uToRgba32F<false, true>, CDds::Bc4uFromRgba32F<false, true> },
		{ LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc5uToRgba32F<false, true>, CDds::Bc5uFromRgba32F<false, true> },
		{ LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc4sToRgba32F<false, true>, CDds::Bc4sFromRgba32F<false, true> },
		{ LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc5sToRgba32F<false, true>, CDds::Bc5sFromRgba32F<false, true> },
		{ LSI_KIF_GL_COMPRESSED_RED_RGTC1, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc4uToRgba32F<false, false>, CDds::Bc4uFromRgba32F<false, false> },
		{ LSI_KIF_GL_COMPRESSED_RG_RGTC2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc5uToRgba32F<false, false>, CDds::Bc5uFromRgba32F<false, false> },
		{ LSI_KIF_GL_COMPRESSED_SIGNED_RED_RGTC1, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc4sToRgba32F<false, false>, CDds::Bc4sFromRgba32F<false, false> },
		{ LSI_KIF_GL_COMPRESSED_SIGNED_RG_RGTC2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc5sToRgba32F<false, false>, CDds::Bc5sFromRgba32F<false, false> },
		{ LSI_KIF_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, LSI_KT_GL_FLOAT, LSI_KBIF_GL_RGB, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, /**/nullptr, nullptr /*CDds::Bc6hToRgba32F<false>, CDds::Bc6hFromRgba32F<false>*/ },
		{ LSI_KIF_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, LSI_KT_GL_FLOAT, LSI_KBIF_GL_RGB, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc6hToRgba32F<false>, CDds::Bc6hFromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_BPTC_UNORM, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CDds::Bc7uToRgba32F<false>, CDds::Bc7uFromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, nullptr, CDds::Bc7uToRgba32F<true>, CDds::Bc7uFromRgba32F<true> },
		{ LSI_KIF_GL_ETC1_RGB8_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc1ToRgba32F, CEtc::Etc1FromRgba32F },
		{ LSI_KIF_GL_COMPRESSED_RGB8_ETC2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc2ToRgba32F<false>, CEtc::Etc2FromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc2A1ToRgba32F<false>, CEtc::Etc2A1FromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA8_ETC2_EAC, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc2EacToRgba32F<false>, CEtc::Etc2EacFromRgba32F<false> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ETC2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, nullptr, CEtc::Etc2ToRgba32F<true>, CEtc::Etc2FromRgba32F<true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, nullptr, CEtc::Etc2A1ToRgba32F<true>, CEtc::Etc2A1FromRgba32F<true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, nullptr, CEtc::Etc2EacToRgba32F<true>, CEtc::Etc2EacFromRgba32F<true> },
		{ LSI_KIF_GL_COMPRESSED_R11_EAC, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::R11EacToRgba32F<false, false>, CEtc::R11EacFromRgba32F<false, false> },
		{ LSI_KIF_GL_COMPRESSED_RG11_EAC, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::RG11EacToRgba32F<false, false>, CEtc::RG11EacFromRgba32F<false, false> },
		{ LSI_KIF_GL_COMPRESSED_SIGNED_R11_EAC, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::R11EacToRgba32F<false, true>, CEtc::R11EacFromRgba32F<false, true> },
		{ LSI_KIF_GL_COMPRESSED_SIGNED_RG11_EAC, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::RG11EacToRgba32F<false, true>, CEtc::RG11EacFromRgba32F<false, true> },
		{ LSI_KIF_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 8, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_2_RGB_V1, false>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_2_RGB_V1, false> },
		{ LSI_KIF_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_4_RGB_V1, false>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_4_RGB_V1, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_2_RGBA_V1, false>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_2_RGBA_V1, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_4_RGBA_V1, false>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_4_RGBA_V1, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 1, 1, false, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_2_RGBA_V2, false>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_2_RGBA_V2, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_4_RGBA_V2, false>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_4_RGBA_V2, false> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 8, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_2_RGB_V1, true>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_2_RGB_V1, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_4_RGB_V1, true>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_4_RGB_V1, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_2_RGBA_V1, true>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_2_RGBA_V1, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_4_RGBA_V1, true>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_4_RGBA_V1, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 1, 1, true, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_2_RGBA_V2, true>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_2_RGBA_V2, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<LSI_PF_PCRTC_4_RGBA_V2, true>, CPvrtc::PvrtcFromRgba32F<LSI_PF_PCRTC_4_RGBA_V2, true> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<4, 4>), CAstc::AstcToRgba32F<LSI_PF_ASTC_4_4, 4, 4, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_4_4, 4, 4, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x4_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<5, 4>), CAstc::AstcToRgba32F<LSI_PF_ASTC_5_4, 5, 4, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_5_4, 5, 4, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<5, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_5_5, 5, 5, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_5_5, 5, 5, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<6, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_6_5, 6, 5, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_6_5, 6, 5, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<6, 6>), CAstc::AstcToRgba32F<LSI_PF_ASTC_6_6, 6, 6, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_6_6, 6, 6, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 8, 5, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<8, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_8_5, 8, 5, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_8_5, 8, 5, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x6_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 8, 6, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<8, 6>), CAstc::AstcToRgba32F<LSI_PF_ASTC_8_6, 8, 6, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_8_6, 8, 6, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x8_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 8, 8, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<8, 8>), CAstc::AstcToRgba32F<LSI_PF_ASTC_8_8, 8, 8, false>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_8_8, 8, 8, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 5, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<10, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_5, 10, 5, false>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_5, 10, 5, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x6_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 6, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<10, 6>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_6, 10, 6, false>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_6, 10, 6, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x8_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 8, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<10, 8>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_8, 10, 8, false>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_8, 10, 8, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x10_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 10, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<10, 10>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_10, 10, 10, false>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_10, 10, 10, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_12x10_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 12, 10, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<12, 10>), CAstc::AstcToRgba32F<LSI_PF_ASTC_12_10, 12, 10, false>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_12_10, 12, 10, false> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_12x12_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 12, 12, 1, 1, 1, false, true, false, &(CKtx::GetCompressedSizeAstc<12, 12>), CAstc::AstcToRgba32F<LSI_PF_ASTC_12_12, 12, 12, false>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_12_12, 12, 12, false> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<4, 4>), CAstc::AstcToRgba32F<LSI_PF_ASTC_4_4, 4, 4, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_4_4, 4, 4, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<5, 4>), CAstc::AstcToRgba32F<LSI_PF_ASTC_5_4, 5, 4, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_5_4, 5, 4, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<5, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_5_5, 5, 5, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_5_5, 5, 5, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<6, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_6_5, 6, 5, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_6_5, 6, 5, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<6, 6>), CAstc::AstcToRgba32F<LSI_PF_ASTC_6_6, 6, 6, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_6_6, 6, 6, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 8, 5, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<8, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_8_5, 8, 5, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_8_5, 8, 5, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 8, 6, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<8, 6>), CAstc::AstcToRgba32F<LSI_PF_ASTC_8_6, 8, 6, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_8_6, 8, 6, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 8, 8, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<8, 8>), CAstc::AstcToRgba32F<LSI_PF_ASTC_8_8, 8, 8, true>, CAstc::AstcFromRgba32F<LSI_PF_ASTC_8_8, 8, 8, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 5, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<10, 5>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_5, 10, 5, true>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_5, 10, 5, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 6, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<10, 6>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_6, 10, 6, true>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_6, 10, 6, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 8, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<10, 8>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_8, 10, 8, true>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_8, 10, 8, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 10, 10, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<10, 10>), CAstc::AstcToRgba32F<LSI_PF_ASTC_10_10, 10, 10, true>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_10_10, 10, 10, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 12, 10, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<12, 10>), CAstc::AstcToRgba32F<LSI_PF_ASTC_12_10, 12, 10, true>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_12_10, 12, 10, true> },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 12, 12, 1, 1, 1, true, true, false, &(CKtx::GetCompressedSizeAstc<12, 12>), CAstc::AstcToRgba32F<LSI_PF_ASTC_12_12, 12, 12, true>, CAstc::Astc2FromRgba32F<LSI_PF_ASTC_12_12, 12, 12, true> },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_3x3x3_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 3, 3, 3, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x3x3_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 3, 3, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4x3_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 3, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4x4_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 4, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x4x4_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 4, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5x4_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 4, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5x5_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 5, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x5x5_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 5, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6x5_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 5, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6x6_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 6, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 3, 3, 3, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 3, 3, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 3, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 4, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 4, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 4, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 5, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 5, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 5, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 6, 1, 1, true, true, false, nullptr },
		{ LSI_KIF_GL_ATC_RGB_AMD, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr },
		{ LSI_KIF_GL_PALETTE4_RGB8_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x04, 384, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE4_RGBA8_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x04, 512, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE4_R5_G6_B5_OES, LSI_KT_GL_UNSIGNED_SHORT_5_6_5, LSI_KBIF_GL_RGB, 0x04, 256, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE4_RGBA4_OES, LSI_KT_GL_UNSIGNED_SHORT_4_4_4_4, LSI_KBIF_GL_RGBA, 0x04, 256, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE4_RGB5_A1_OES, LSI_KT_GL_UNSIGNED_SHORT_5_5_5_1, LSI_KBIF_GL_RGBA, 0x04, 256, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE8_RGB8_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGB, 0x04, 6144, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE8_RGBA8_OES, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_RGBA, 0x04, 8192, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE8_R5_G6_B5_OES, LSI_KT_GL_UNSIGNED_SHORT_5_6_5, LSI_KBIF_GL_RGB, 0x04, 4096, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE8_RGBA4_OES, LSI_KT_GL_UNSIGNED_SHORT_4_4_4_4, LSI_KBIF_GL_RGBA, 0x04, 4096, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_PALETTE8_RGB5_A1_OES, LSI_KT_GL_UNSIGNED_SHORT_5_5_5_1, LSI_KBIF_GL_RGBA, 0x04, 4096, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		{ LSI_KIF_GL_COLOR_INDEX1_EXT, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COLOR_INDEX2_EXT, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COLOR_INDEX4_EXT, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COLOR_INDEX8_EXT, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COLOR_INDEX12_EXT, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_COLOR_INDEX16_EXT, LSI_KT_0, LSI_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		{ LSI_KIF_GL_DEPTH_COMPONENT16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth16ToRgba32F, CKtx::Depth16FromRgba32F },
		{ LSI_KIF_GL_DEPTH_COMPONENT24, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth24ToRgba32F, CKtx::Depth24FromRgba32F },
		{ LSI_KIF_GL_DEPTH_COMPONENT32, LSI_KT_GL_UNSIGNED_INT, LSI_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth32ToRgba32F, CKtx::Depth32FromRgba32F },
		{ LSI_KIF_GL_DEPTH_COMPONENT32F, LSI_KT_GL_FLOAT, LSI_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth32FToRgba32F, CKtx::Depth32FFromRgba32F },
		{ LSI_KIF_GL_DEPTH_COMPONENT32F_NV, LSI_KT_GL_FLOAT, LSI_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth32FToRgba32F, CKtx::Depth32FFromRgba32F },
		{ LSI_KIF_GL_STENCIL_INDEX1, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_STENCIL_INDEX, 0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::StencilXToRgba32F<uint8_t, 1>, CKtx::StencilXFromRgba32F<uint8_t, 1> },
		{ LSI_KIF_GL_STENCIL_INDEX4, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_STENCIL_INDEX, 0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::StencilXToRgba32F<uint8_t, 4>, CKtx::StencilXFromRgba32F<uint8_t, 4> },
		{ LSI_KIF_GL_STENCIL_INDEX8, LSI_KT_GL_UNSIGNED_BYTE, LSI_KBIF_GL_STENCIL_INDEX, 0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::StencilXToRgba32F<uint8_t, 8>, CKtx::StencilXFromRgba32F<uint8_t, 8> },
		{ LSI_KIF_GL_STENCIL_INDEX16, LSI_KT_GL_UNSIGNED_SHORT, LSI_KBIF_GL_STENCIL_INDEX, 0x10, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::StencilXToRgba32F<uint16_t, 16>, CKtx::StencilXFromRgba32F<uint16_t, 16> },
		{ LSI_KIF_GL_DEPTH24_STENCIL8, LSI_KT_GL_UNSIGNED_INT_24_8, LSI_KBIF_GL_DEPTH_STENCIL, 0x18, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth24S8ToRgba32F, CKtx::Depth24S8FromRgba32F },
		{ LSI_KIF_GL_DEPTH32F_STENCIL8, LSI_KT_GL_FLOAT_32_UNSIGNED_INT_24_8_REV, LSI_KBIF_GL_DEPTH_STENCIL, 0x18, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth32FS8ToRgba32F, CKtx::Depth32FS8FromRgba32F },
		{ LSI_KIF_GL_DEPTH32F_STENCIL8_NV, LSI_KT_GL_FLOAT_32_UNSIGNED_INT_24_8_REV, LSI_KBIF_GL_DEPTH_STENCIL, 0x18, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, CKtx::Depth32FS8ToRgba32F, CKtx::Depth32FS8FromRgba32F },
		
#undef LSI_GEN_INT
#undef LSI_CONV_IN
#undef LSI_CONV_F
#undef LSI_CONV_I
#undef LSI_IBITS
#undef LSI_LBITS
#undef LSI_LSHIFTS
#undef LSI_TSHIFTS
#undef LSI_TBITS
#endif
	};

	// == Functions.
	/**
	 * Loads a KTX 1 file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _ktGlType The OpenGL type of the image.
	 * \param _kfFormat The OpenGL format of the image.
	 * \param _kifInternalFormat The OpenGL internal format of the image.
	 * \param _kbifInternalBaseFormat The OpenGL internal base format of the image.
	 * \param _tbReturn The returned data.
	 * \param _vMipMaps The returned mipmap data, if any.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CKtx::LoadKtx1( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		LSI_KTX_TYPE &_ktGlType, LSI_KTX_FORMAT &_kfFormat, LSI_KTX_INTERNAL_FORMAT &_kifInternalFormat, LSI_KTX_BASE_INTERNAL_FORMAT &_kbifInternalBaseFormat,
		CImageLib::CTexelBuffer &_tbReturn,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {
		//PrintTable();
#ifdef LSI_USE_KHRONOS
		{
			CKtxTexture<ktxTexture1> kt1Tex;
			KTX_error_code ecErr = ::ktxTexture1_CreateFromMemory( _pui8FileData, _ui32DataLen, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
				kt1Tex.HandlePointer() );
			
			if ( KTX_SUCCESS == ecErr && kt1Tex.Handle() ) {
				if ( (*kt1Tex).numDimensions <= 2 &&		// 1D and 2D textures only for now.
					(*kt1Tex).baseDepth == 1 &&				// Second check on not being a 3D texture.
					(*kt1Tex).numLayers == 1 &&				// No arrays.
					(*kt1Tex).numFaces == 1 ) {				// No cube textures.
					// Must have valid data.
					if ( (*kt1Tex).pData ) {
						LSI_KTX_IMAGE_LOAD kilImgDst;
						kilImgDst.ptbReturn = &_tbReturn;
						kilImgDst.pvMipMaps = &_vMipMaps;
						if ( (*kt1Tex).numLevels > 1 ) {
							if ( !_vMipMaps.Resize( (*kt1Tex).numLevels - 1 ) ) { return false; }
						}
						if ( KTX_SUCCESS == ::ktxTexture_IterateLevelFaces( ktxTexture( kt1Tex.Handle() ), KtxImageLoad, &kilImgDst ) ) {
							_ui32Width = (*kt1Tex).baseWidth;
							_ui32Height = (*kt1Tex).baseHeight;
							_pfFormat = LSI_PF_KTX1;

							_ktGlType = static_cast<LSI_KTX_TYPE>((*kt1Tex).glType);
							_kfFormat = static_cast<LSI_KTX_FORMAT>((*kt1Tex).glFormat);
							_kifInternalFormat = static_cast<LSI_KTX_INTERNAL_FORMAT>((*kt1Tex).glInternalformat);
							_kbifInternalBaseFormat = static_cast<LSI_KTX_BASE_INTERNAL_FORMAT>((*kt1Tex).glBaseInternalformat);
							return true;
						}
					}
				}
			}
		}
		
		return false;
#else
		if ( _ui32DataLen < sizeof( LSI_KTX_HEADER ) ) { return false; }
		const uint8_t ui8Ident[] = {
			0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
		};
		const LSI_KTX_HEADER * pkhHeader = reinterpret_cast<const LSI_KTX_HEADER *>(_pui8FileData);
		if ( !CStd::MemCmpF( ui8Ident, pkhHeader->ui8Identifier, sizeof( ui8Ident ) ) ) { return false; }
		bool bFixEndian = 0x04030201 != pkhHeader->ui32Endienness;
#define LSI_GET_VAL( NAME )		(bFixEndian ? CStd::ByteSwap32( pkhHeader->NAME ) : pkhHeader->NAME)
		if ( LSI_GET_VAL( ui32PixelDepth ) ) {
			// Can't support 3D textures just yet.
			return false;
		}
		_ktGlType = static_cast<LSI_KTX_TYPE>(LSI_GET_VAL( ui32GlType ));
		uint32_t ui32GlTypeSize = static_cast<uint32_t>(LSI_GET_VAL( ui32GlTypeSize ));
		_kfFormat = static_cast<LSI_KTX_FORMAT>(LSI_GET_VAL( ui32GlFormat ));
		_kifInternalFormat = static_cast<LSI_KTX_INTERNAL_FORMAT>(LSI_GET_VAL( ui32GlInternalFormat ));
		_kbifInternalBaseFormat = static_cast<LSI_KTX_BASE_INTERNAL_FORMAT>(LSI_GET_VAL( ui32GlBaseInternalFormat ));

		_ui32Width = LSI_GET_VAL( ui32PixelWidth );
		_ui32Height = CStd::Max<uint32_t>( LSI_GET_VAL( ui32PixelHeight ), 1U );
		_pfFormat = LSI_PF_KTX1;

		uint32_t ui32Mips = CStd::Max<uint32_t>( LSI_GET_VAL( ui32NumberofMipmapLevels ), 1U );
		uint32_t ui32Arrays = CStd::Max<uint32_t>( LSI_GET_VAL( ui32NumberofArrayElements ), 1U );
		uint32_t ui32Faces = LSI_GET_VAL( ui32NumberofFaces );
		if ( static_cast<uint16_t>(_kifInternalFormat) >= static_cast<uint16_t>(LSI_KIF_GL_PALETTE4_RGB8_OES) && static_cast<uint16_t>(_kifInternalFormat) <= static_cast<uint16_t>(LSI_KIF_GL_PALETTE8_RGB5_A1_OES) ) {
			if ( ui32Faces != 1 ) { return false; }
			ui32Mips = 1;
		}
		if ( ui32Faces != 1 || ui32Arrays != 1 ) {
			// Can't currently load multi-faced images.
			return false;
		}
		uint32_t ui32Data = sizeof( LSI_KTX_HEADER ) + LSI_GET_VAL( ui32BytesOfKeyValueData );
#define LSI_MOVE_TO_4_BYTE		ui32Data = ((ui32Data + 3) / 4) * 4
		
		//uint32_t ui32SizeRemaining = _ui32DataLen - ui32Data;
#undef LSI_GET_VAL
#define LSI_GET_VAL( NAME )		(bFixEndian ? CStd::ByteSwap32( NAME ) : NAME)

		if ( ui32Mips > 1 ) {
			if ( !_vMipMaps.Resize( ui32Mips - 1 ) ) { return false; }
		}

		for ( uint32_t M = 0; M < ui32Mips; ++M ) {
			LSI_MOVE_TO_4_BYTE;
			if ( ui32Data + sizeof( uint32_t ) > _ui32DataLen ) { return false; }
			uint32_t ui32Size = LSI_GET_VAL( (*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32Data])) );
			ui32Data += sizeof( uint32_t );
			if ( ui32Data + ui32Size > _ui32DataLen ) { return false; }
			//uint32_t ui32SizePerRow = ui32Size / _ui32Height;
			CImageLib::CTexelBuffer * ptbTarget = (M == 0 ? & _tbReturn : &_vMipMaps[M-1]);
			if ( !ptbTarget->Resize( ui32Size ) ) { return false; }
			for ( uint32_t A = 0; A < ui32Arrays; ++A ) {
				for ( uint32_t F = 0; F < ui32Faces; ++F ) {
					CStd::MemCpy( &(*ptbTarget)[0], &_pui8FileData[ui32Data], ui32Size );
					if ( bFixEndian ) {
						switch ( ui32GlTypeSize ) {
							case sizeof( uint16_t ) : {
								uint16_t * pui16SwapMe = reinterpret_cast<uint16_t *>(&(*ptbTarget)[0]);
								for ( uint32_t J = ui32Size / ui32GlTypeSize; J--; ) {
									pui16SwapMe[J] = CStd::ByteSwap16( pui16SwapMe[J] );
								}
								break;
							}
							/*case 3 : {
								struct LSI_KTX_BS {
									uint8_t		ui8Vals[3];
								};
								LSI_KTX_BS * pui32SwapMe = reinterpret_cast<LSI_KTX_BS *>(&(*ptbTarget)[0]);
								for ( uint32_t J = ui32Size / ui32GlTypeSize; J--; ) {
									CStd::Swap( pui32SwapMe[0], pui32SwapMe[2] );
								}
								break;
							}*/
							case sizeof( uint32_t ) : {
								uint32_t * pui32SwapMe = reinterpret_cast<uint32_t *>(&(*ptbTarget)[0]);
								for ( uint32_t J = ui32Size / ui32GlTypeSize; J--; ) {
									pui32SwapMe[J] = CStd::ByteSwap32( pui32SwapMe[J] );
								}
								break;
							}
							case sizeof( uint64_t ) : {
								uint64_t * pui64SwapMe = reinterpret_cast<uint64_t *>(&(*ptbTarget)[0]);
								for ( uint32_t J = ui32Size / ui32GlTypeSize; J--; ) {
									pui64SwapMe[J] = CStd::ByteSwap64( pui64SwapMe[J] );
								}
								break;
							}
						}
					}
					break;
				}
				break;
			}


			ui32Data += ui32Size;
		}

#undef LSI_GET_VAL
#undef LSI_MOVE_TO_4_BYTE

		return true;
#endif	// LSI_USE_KHRONOS
	}

	/**
	 * Loads a KTX 2 file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _vfFormatnternalBaseFormat The Vulkan format of the image.
	 * \param _tbReturn The returned data.
	 * \param _vMipMaps The returned mipmap data, if any.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CKtx::LoadKtx2( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		LSI_VKFORMAT &_vfFormatnternalBaseFormat,
		CImageLib::CTexelBuffer &_tbReturn,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {
#ifdef LSI_USE_KHRONOS
		{
			CKtxTexture<ktxTexture2> kt2Tex;
			KTX_error_code ecErr = ::ktxTexture2_CreateFromMemory( _pui8FileData, _ui32DataLen, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
				kt2Tex.HandlePointer() );
			if ( KTX_SUCCESS == ecErr && kt2Tex.Handle() ) {
			}
		}
#endif	// #ifdef LSI_USE_KHRONOS
		return false;
	}

	/**
	 * Finds the details for a given internal format.
	 *
	 * \param _kifInternalFormat The OpenGL internal format whose details are to be found.
	 * \return Returns the details for the given internal format or nullptr.
	 */
	const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * LSE_CALL CKtx::FindInternalFormatData( LSI_KTX_INTERNAL_FORMAT _kifInternalFormat ) {
		for ( size_t I = 0; I < LSE_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].kifInternalFormat == _kifInternalFormat ) {
				return &m_kifdInternalFormats[I];
			}
		}
		return nullptr;
	}

	/**
	 * Gets an internal format entry by index.
	 *
	 * \param _stIndex Index of the entry to obtain.
	 * \return Returns nullptr if the entry index is out of bounds, otherwise the requested entry.
	 */
	const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * LSE_CALL CKtx::GetInternalFormatData( size_t _stIndex ) {
		if ( _stIndex >= LSE_ELEMENTS( m_kifdInternalFormats ) ) { return nullptr; }
		return &m_kifdInternalFormats[_stIndex];
	}

	/**
	 * Gets the total entries in the internal format data list.
	 *
	 * \return Returns the total entries in the internal format data list.
	 */
	size_t LSE_CALL CKtx::GetInternalFormatDataTotal() {
		return LSE_ELEMENTS( m_kifdInternalFormats );
	}

	/**
	 * Returns the total size of a compressed image given a factor and its width and height.
	 *
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _ui32Factor Multiplier.
	 * \return Returns the size of the compressed data.
	 */
	uint32_t LSE_CALL CKtx::GetCompressedSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Factor ) {
		return ((_ui32Width + 3) >> 2) * ((_ui32Height + 3) >> 2) * _ui32Factor;
	}

	/**
	 * Returns the size of a texture of the given width, height, and depth based off its LSI_KTX_INTERNAL_FORMAT_DATA representation.
	 *
	 * \param _ui32Width Width in pixels.
	 * \param _ui32Height Height in pixels.
	 * \param _ui32Depth Depth in pixels.
	 * \param _pkifdData Format descriptor.
	 * \return Returns the size in bytes needed for the texture data.
	 */
	uint32_t LSE_CALL CKtx::GetTextureSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const LSI_KTX_INTERNAL_FORMAT_DATA * _pkifdData ) {
		if ( !_pkifdData ) { return 0; }
		if ( _pkifdData->bCompressed ) {
			if ( _ui32Depth != 1 ) { return 0; }
			if ( _pkifdData->pfCompSizeFunc ) {
				return _pkifdData->pfCompSizeFunc( _ui32Width, _ui32Height, ((_pkifdData->ui32BlockSizeInBits + 7) / 8) );
			}
			return CKtx::GetCompressedSize( _ui32Width, _ui32Height, ((_pkifdData->ui32BlockSizeInBits + 7) / 8) );
		}
		uint32_t ui32RowSize = LSE_ROUND_UP( ((_pkifdData->ui32BlockSizeInBits + 7) / 8) * _ui32Width, 4 );
		return ui32RowSize * _ui32Height * _ui32Depth;
	}

	/**
	 * Converts a LSI_KTX_TYPE value to a string.
	 *
	 * \param _tType The type to convert to a string.
	 * \return Returns the string form of a given LSI_KTX_TYPE value.
	 */
	const char * LSE_CALL LSE_CALL CKtx::glTypeToString( LSI_KTX_TYPE _tType ) {
#define LSI_TS( VAL )		case LSI_KT_ ## VAL : { return #VAL; }
		switch ( _tType ) {
			LSI_TS( GL_BYTE )
			LSI_TS( GL_UNSIGNED_BYTE )
			LSI_TS( GL_SHORT )
			LSI_TS( GL_UNSIGNED_SHORT )
			LSI_TS( GL_INT )
			LSI_TS( GL_UNSIGNED_INT )
			LSI_TS( GL_INT64 )
			LSI_TS( GL_UNSIGNED_INT64 )
			LSI_TS( GL_HALF_FLOAT )
			LSI_TS( GL_HALF_FLOAT_OES )
			LSI_TS( GL_FLOAT )
			LSI_TS( GL_DOUBLE )
			LSI_TS( GL_UNSIGNED_BYTE_3_3_2 )
			LSI_TS( GL_UNSIGNED_BYTE_2_3_3_REV )
			LSI_TS( GL_UNSIGNED_SHORT_5_6_5 )
			LSI_TS( GL_UNSIGNED_SHORT_5_6_5_REV )
			LSI_TS( GL_UNSIGNED_SHORT_4_4_4_4 )
			LSI_TS( GL_UNSIGNED_SHORT_4_4_4_4_REV )
			LSI_TS( GL_UNSIGNED_SHORT_5_5_5_1 )
			LSI_TS( GL_UNSIGNED_SHORT_1_5_5_5_REV )
			LSI_TS( GL_UNSIGNED_INT_8_8_8_8 )
			LSI_TS( GL_UNSIGNED_INT_8_8_8_8_REV )
			LSI_TS( GL_UNSIGNED_INT_10_10_10_2 )
			LSI_TS( GL_UNSIGNED_INT_2_10_10_10_REV )
			LSI_TS( GL_UNSIGNED_INT_10F_11F_11F_REV )
			LSI_TS( GL_UNSIGNED_INT_5_9_9_9_REV )
			LSI_TS( GL_UNSIGNED_INT_24_8 )
			LSI_TS( GL_FLOAT_32_UNSIGNED_INT_24_8_REV )
		}
#undef LSI_TS
		return "";
	}

	/**
	 * Converts a LSI_KTX_FORMAT value to a string.
	 *
	 * \param _fFormat The format to convert to a string.
	 * \return Returns the string form of a given LSI_KTX_FORMAT value.
	 */
	const char * LSE_CALL CKtx::glFormatToString( LSI_KTX_FORMAT _fFormat ) {
#define LSI_TS( VAL )		case LSI_KF_ ## VAL : { return #VAL; }
		switch ( _fFormat ) {
			LSI_TS( GL_RED )
			LSI_TS( GL_GREEN )
			LSI_TS( GL_BLUE )
			LSI_TS( GL_ALPHA )
			LSI_TS( GL_LUMINANCE )
			LSI_TS( GL_SLUMINANCE )
			LSI_TS( GL_LUMINANCE_ALPHA )
			LSI_TS( GL_SLUMINANCE_ALPHA )
			LSI_TS( GL_INTENSITY )
			LSI_TS( GL_RG )
			LSI_TS( GL_RGB )
			LSI_TS( GL_BGR )
			LSI_TS( GL_RGBA )
			LSI_TS( GL_BGRA )
			LSI_TS( GL_RED_INTEGER )
			LSI_TS( GL_GREEN_INTEGER )
			LSI_TS( GL_BLUE_INTEGER )
			LSI_TS( GL_ALPHA_INTEGER )
			LSI_TS( GL_LUMINANCE_INTEGER )
			LSI_TS( GL_LUMINANCE_ALPHA_INTEGER )
			LSI_TS( GL_RG_INTEGER )
			LSI_TS( GL_RGB_INTEGER )
			LSI_TS( GL_BGR_INTEGER )
			LSI_TS( GL_RGBA_INTEGER )
			LSI_TS( GL_BGRA_INTEGER )
			LSI_TS( GL_COLOR_INDEX )
			LSI_TS( GL_STENCIL_INDEX )
			LSI_TS( GL_DEPTH_COMPONENT )
			LSI_TS( GL_DEPTH_STENCIL )
		}
#undef LSI_TS
		return "";
	}

	/**
	 * Converts a LSI_KTX_INTERNAL_FORMAT value to a string.
	 *
	 * \param _ifInternalFormat The type to convert to a string.
	 * \return Returns the string form of a given LSI_KTX_INTERNAL_FORMAT value.
	 */
	const char * LSE_CALL CKtx::glInternalFormatToString( LSI_KTX_INTERNAL_FORMAT _ifInternalFormat ) {
#define LSI_TS( VAL )		case LSI_KIF_ ## VAL : { return #VAL; }
		switch ( _ifInternalFormat ) {
			//
			// 8 bits per component
			//
			LSI_TS( GL_R8 )
			LSI_TS( GL_RG8 )
			LSI_TS( GL_RGB8 )
			LSI_TS( GL_RGBA8 )
			LSI_TS( GL_R8_SNORM )
			LSI_TS( GL_RG8_SNORM )
			LSI_TS( GL_RGB8_SNORM )
			LSI_TS( GL_RGBA8_SNORM )
			LSI_TS( GL_R8UI )
			LSI_TS( GL_RG8UI )
			LSI_TS( GL_RGB8UI )
			LSI_TS( GL_RGBA8UI )
			LSI_TS( GL_R8I )
			LSI_TS( GL_RG8I )
			LSI_TS( GL_RGB8I )
			LSI_TS( GL_RGBA8I )
			LSI_TS( GL_SR8 )
			LSI_TS( GL_SRG8 )
			LSI_TS( GL_SRGB8 )
			LSI_TS( GL_SRGB8_ALPHA8 )
			//
			// 16 bits per component
			//
			LSI_TS( GL_R16 )
			LSI_TS( GL_RG16 )
			LSI_TS( GL_RGB16 )
			LSI_TS( GL_RGBA16 )
			LSI_TS( GL_R16_SNORM )
			LSI_TS( GL_RG16_SNORM )
			LSI_TS( GL_RGB16_SNORM )
			LSI_TS( GL_RGBA16_SNORM )
			LSI_TS( GL_R16UI )
			LSI_TS( GL_RG16UI )
			LSI_TS( GL_RGB16UI )
			LSI_TS( GL_RGBA16UI )
			LSI_TS( GL_R16I )
			LSI_TS( GL_RG16I )
			LSI_TS( GL_RGB16I )
			LSI_TS( GL_RGBA16I )
			LSI_TS( GL_R16F )
			LSI_TS( GL_RG16F )
			LSI_TS( GL_RGB16F )
			LSI_TS( GL_RGBA16F )
			//
			// 32 bits per component
			//
			LSI_TS( GL_R32UI )
			LSI_TS( GL_RG32UI )
			LSI_TS( GL_RGB32UI )
			LSI_TS( GL_RGBA32UI )
			LSI_TS( GL_R32I )
			LSI_TS( GL_RG32I )
			LSI_TS( GL_RGB32I )
			LSI_TS( GL_RGBA32I )
			LSI_TS( GL_R32F )
			LSI_TS( GL_RG32F )
			LSI_TS( GL_RGB32F )
			LSI_TS( GL_RGBA32F )
			//
			// Packed
			//
			LSI_TS( GL_R3_G3_B2 )
			LSI_TS( GL_RGB4 )
			LSI_TS( GL_RGB5 )
			LSI_TS( GL_RGB565 )
			LSI_TS( GL_RGB10 )
			LSI_TS( GL_RGB12 )
			LSI_TS( GL_RGBA2 )
			LSI_TS( GL_RGBA4 )
			LSI_TS( GL_RGBA12 )
			LSI_TS( GL_RGB5_A1 )
			LSI_TS( GL_RGB10_A2 )
			LSI_TS( GL_RGB10_A2UI )
			LSI_TS( GL_R11F_G11F_B10F )
			LSI_TS( GL_RGB9_E5 )
			//
			// Alpha
			//
			LSI_TS( GL_ALPHA4 )
			LSI_TS( GL_ALPHA8 )
			LSI_TS( GL_ALPHA8_SNORM )
			LSI_TS( GL_ALPHA8UI_EXT )
			LSI_TS( GL_ALPHA8I_EXT )
			LSI_TS( GL_ALPHA12 )
			LSI_TS( GL_ALPHA16 )
			LSI_TS( GL_ALPHA16_SNORM )
			LSI_TS( GL_ALPHA16UI_EXT )
			LSI_TS( GL_ALPHA16I_EXT )
			LSI_TS( GL_ALPHA16F_ARB )
			LSI_TS( GL_ALPHA32UI_EXT )
			LSI_TS( GL_ALPHA32I_EXT )
			LSI_TS( GL_ALPHA32F_ARB )
			//
			// Luminance
			//
			LSI_TS( GL_LUMINANCE4 )
			LSI_TS( GL_LUMINANCE8 )
			LSI_TS( GL_LUMINANCE8_SNORM )
			LSI_TS( GL_SLUMINANCE8 )
			LSI_TS( GL_LUMINANCE8UI_EXT )
			LSI_TS( GL_LUMINANCE8I_EXT )
			LSI_TS( GL_LUMINANCE12 )
			LSI_TS( GL_LUMINANCE16 )
			LSI_TS( GL_LUMINANCE16_SNORM )
			LSI_TS( GL_LUMINANCE16UI_EXT )
			LSI_TS( GL_LUMINANCE16I_EXT )
			LSI_TS( GL_LUMINANCE16F_ARB )
			LSI_TS( GL_LUMINANCE32UI_EXT )
			LSI_TS( GL_LUMINANCE32I_EXT )
			LSI_TS( GL_LUMINANCE32F_ARB )
			//
			// Luminance/Alpha
			//
			LSI_TS( GL_LUMINANCE4_ALPHA4 )
			LSI_TS( GL_LUMINANCE6_ALPHA2 )
			LSI_TS( GL_LUMINANCE8_ALPHA8 )
			LSI_TS( GL_LUMINANCE8_ALPHA8_SNORM )
			LSI_TS( GL_SLUMINANCE8_ALPHA8 )
			LSI_TS( GL_LUMINANCE_ALPHA8UI_EXT )
			LSI_TS( GL_LUMINANCE_ALPHA8I_EXT )
			LSI_TS( GL_LUMINANCE12_ALPHA4 )
			LSI_TS( GL_LUMINANCE12_ALPHA12 )
			LSI_TS( GL_LUMINANCE16_ALPHA16 )
			LSI_TS( GL_LUMINANCE16_ALPHA16_SNORM )
			LSI_TS( GL_LUMINANCE_ALPHA16UI_EXT )
			LSI_TS( GL_LUMINANCE_ALPHA16I_EXT )
			LSI_TS( GL_LUMINANCE_ALPHA16F_ARB )
			LSI_TS( GL_LUMINANCE_ALPHA32UI_EXT )
			LSI_TS( GL_LUMINANCE_ALPHA32I_EXT )
			LSI_TS( GL_LUMINANCE_ALPHA32F_ARB )
			//
			// Intensity
			//
			LSI_TS( GL_INTENSITY4 )
			LSI_TS( GL_INTENSITY8 )
			LSI_TS( GL_INTENSITY8_SNORM )
			LSI_TS( GL_INTENSITY8UI_EXT )
			LSI_TS( GL_INTENSITY8I_EXT )
			LSI_TS( GL_INTENSITY12 )
			LSI_TS( GL_INTENSITY16 )
			LSI_TS( GL_INTENSITY16_SNORM )
			LSI_TS( GL_INTENSITY16UI_EXT )
			LSI_TS( GL_INTENSITY16I_EXT )
			LSI_TS( GL_INTENSITY16F_ARB )
			LSI_TS( GL_INTENSITY32UI_EXT )
			LSI_TS( GL_INTENSITY32I_EXT )
			LSI_TS( GL_INTENSITY32F_ARB )
			//
			// Generic compression
			//
			LSI_TS( GL_COMPRESSED_RED )
			LSI_TS( GL_COMPRESSED_ALPHA )
			LSI_TS( GL_COMPRESSED_LUMINANCE )
			LSI_TS( GL_COMPRESSED_SLUMINANCE )
			LSI_TS( GL_COMPRESSED_LUMINANCE_ALPHA )
			LSI_TS( GL_COMPRESSED_SLUMINANCE_ALPHA )
			LSI_TS( GL_COMPRESSED_INTENSITY )
			LSI_TS( GL_COMPRESSED_RG )
			LSI_TS( GL_COMPRESSED_RGB )
			LSI_TS( GL_COMPRESSED_RGBA )
			LSI_TS( GL_COMPRESSED_SRGB )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA )
			//
			// FXT1
			//
			LSI_TS( GL_COMPRESSED_RGB_FXT1_3DFX )
			LSI_TS( GL_COMPRESSED_RGBA_FXT1_3DFX )
			//
			// S3TC/DXT/BC
			//
			LSI_TS( GL_COMPRESSED_RGB_S3TC_DXT1_EXT )
			LSI_TS( GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
			LSI_TS( GL_COMPRESSED_RGBA_S3TC_DXT3_EXT )
			LSI_TS( GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_S3TC_DXT1_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT )
			LSI_TS( GL_COMPRESSED_LUMINANCE_LATC1_EXT )
			LSI_TS( GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT )
			LSI_TS( GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT )
			LSI_TS( GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT )
			LSI_TS( GL_COMPRESSED_RED_RGTC1 )
			LSI_TS( GL_COMPRESSED_RG_RGTC2 )
			LSI_TS( GL_COMPRESSED_SIGNED_RED_RGTC1 )
			LSI_TS( GL_COMPRESSED_SIGNED_RG_RGTC2 )
			LSI_TS( GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT )
			LSI_TS( GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT )
			LSI_TS( GL_COMPRESSED_RGBA_BPTC_UNORM )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM )
			//
			// ETC
			//
			LSI_TS( GL_ETC1_RGB8_OES )
			LSI_TS( GL_COMPRESSED_RGB8_ETC2 )
			LSI_TS( GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 )
			LSI_TS( GL_COMPRESSED_RGBA8_ETC2_EAC )
			LSI_TS( GL_COMPRESSED_SRGB8_ETC2 )
			LSI_TS( GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC )
			LSI_TS( GL_COMPRESSED_R11_EAC )
			LSI_TS( GL_COMPRESSED_RG11_EAC )
			LSI_TS( GL_COMPRESSED_SIGNED_R11_EAC )
			LSI_TS( GL_COMPRESSED_SIGNED_RG11_EAC )
			//
			// PVRTC
			//
			LSI_TS( GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG )
			LSI_TS( GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG )
			LSI_TS( GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG )
			LSI_TS( GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG )

			LSI_TS( GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG )
			LSI_TS( GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG )

			LSI_TS( GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT )

			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG )
			LSI_TS( GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG )
			//
			// ASTC
			//
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_4x4_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_5x4_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_5x5_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_6x5_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_6x6_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_8x5_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_8x6_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_8x8_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_10x5_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_10x6_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_10x8_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_10x10_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_12x10_KHR )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_12x12_KHR )

			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR )

			LSI_TS( GL_COMPRESSED_RGBA_ASTC_3x3x3_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_4x3x3_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_4x4x3_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_4x4x4_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_5x4x4_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_5x5x4_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_5x5x5_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_6x5x5_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_6x6x5_OES )
			LSI_TS( GL_COMPRESSED_RGBA_ASTC_6x6x6_OES )

			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES )
			LSI_TS( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES )
			//
			// ATC
			//
			LSI_TS( GL_ATC_RGB_AMD )
			LSI_TS( GL_ATC_RGBA_EXPLICIT_ALPHA_AMD )
			LSI_TS( GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD )
			//
			// Palletized (combined palette)
			//
			LSI_TS( GL_PALETTE4_RGB8_OES )
			LSI_TS( GL_PALETTE4_RGBA8_OES )
			LSI_TS( GL_PALETTE4_R5_G6_B5_OES )
			LSI_TS( GL_PALETTE4_RGBA4_OES )
			LSI_TS( GL_PALETTE4_RGB5_A1_OES )
			LSI_TS( GL_PALETTE8_RGB8_OES )
			LSI_TS( GL_PALETTE8_RGBA8_OES )
			LSI_TS( GL_PALETTE8_R5_G6_B5_OES )
			LSI_TS( GL_PALETTE8_RGBA4_OES )
			LSI_TS( GL_PALETTE8_RGB5_A1_OES )
			//
			// Palletized (separate palette)
			//
			LSI_TS( GL_COLOR_INDEX1_EXT )
			LSI_TS( GL_COLOR_INDEX2_EXT )
			LSI_TS( GL_COLOR_INDEX4_EXT )
			LSI_TS( GL_COLOR_INDEX8_EXT )
			LSI_TS( GL_COLOR_INDEX12_EXT )
			LSI_TS( GL_COLOR_INDEX16_EXT )
			//
			// Depth/stencil
			//
			LSI_TS( GL_DEPTH_COMPONENT16 )
			LSI_TS( GL_DEPTH_COMPONENT24 )
			LSI_TS( GL_DEPTH_COMPONENT32 )
			LSI_TS( GL_DEPTH_COMPONENT32F )
			LSI_TS( GL_DEPTH_COMPONENT32F_NV )
			LSI_TS( GL_STENCIL_INDEX1 )
			LSI_TS( GL_STENCIL_INDEX4 )
			LSI_TS( GL_STENCIL_INDEX8 )
			LSI_TS( GL_STENCIL_INDEX16 )
			LSI_TS( GL_DEPTH24_STENCIL8 )
			LSI_TS( GL_DEPTH32F_STENCIL8 )
			LSI_TS( GL_DEPTH32F_STENCIL8_NV )
		}
#undef LSI_TS
		return "";
	}

	/**
	 * Converts a LSI_KTX_BASE_INTERNAL_FORMAT value to a string.
	 *
	 * \param _bifBaseInternalFormat The type to convert to a string.
	 * \return Returns the string form of a given LSI_KTX_BASE_INTERNAL_FORMAT value.
	 */
	const char * LSE_CALL CKtx::glBaseInternalFormatToString( LSI_KTX_BASE_INTERNAL_FORMAT _bifBaseInternalFormat ) {
#define LSI_TS( VAL )		case LSI_KBIF_ ## VAL : { return #VAL; }
		switch ( _bifBaseInternalFormat ) {
			LSI_TS( GL_RED )
			LSI_TS( GL_GREEN )
			LSI_TS( GL_BLUE )
			LSI_TS( GL_ALPHA )
			LSI_TS( GL_LUMINANCE )
			LSI_TS( GL_SLUMINANCE )
			LSI_TS( GL_LUMINANCE_ALPHA )
			LSI_TS( GL_SLUMINANCE_ALPHA )
			LSI_TS( GL_INTENSITY )
			LSI_TS( GL_RG )
			LSI_TS( GL_RGB )
			LSI_TS( GL_BGR )
			LSI_TS( GL_RGBA )
			LSI_TS( GL_BGRA )
			LSI_TS( GL_RED_INTEGER )
			LSI_TS( GL_GREEN_INTEGER )
			LSI_TS( GL_BLUE_INTEGER )
			LSI_TS( GL_ALPHA_INTEGER )
			LSI_TS( GL_LUMINANCE_INTEGER )
			LSI_TS( GL_LUMINANCE_ALPHA_INTEGER )
			LSI_TS( GL_RG_INTEGER )
			LSI_TS( GL_RGB_INTEGER )
			LSI_TS( GL_BGR_INTEGER )
			LSI_TS( GL_RGBA_INTEGER )
			LSI_TS( GL_BGRA_INTEGER )
			LSI_TS( GL_COLOR_INDEX )
			LSI_TS( GL_STENCIL_INDEX )
			LSI_TS( GL_DEPTH_COMPONENT )
			LSI_TS( GL_DEPTH_STENCIL )
		}
#undef LSI_TS
		return "";
	}

	/**
	 * Gets the size of _ktType in bytes.
	 *
	 * \param _ktType The type whose size is to be returned.
	 * \return the size of _ktType in bytes.
	 */
	uint32_t LSE_CALL CKtx::glTypeToSize( LSI_KTX_TYPE _ktType ) {
		switch ( _ktType ) {
			case LSI_KT_GL_BYTE : {}
			case LSI_KT_GL_UNSIGNED_BYTE : { return 1; }
			case LSI_KT_GL_SHORT: {}
			case LSI_KT_GL_UNSIGNED_SHORT : { return 2; }
			case LSI_KT_GL_INT : {}
			case LSI_KT_GL_UNSIGNED_INT : { return 4; }
			case LSI_KT_GL_INT64 : {}
			case LSI_KT_GL_UNSIGNED_INT64 : { return 8; }
			case LSI_KT_GL_HALF_FLOAT : {}
			case LSI_KT_GL_HALF_FLOAT_OES : { return 2; }
			case LSI_KT_GL_FLOAT : { return 4; }
			case LSI_KT_GL_DOUBLE : { return 8; }
			case LSI_KT_GL_UNSIGNED_BYTE_3_3_2 : { return 1; }
			case LSI_KT_GL_UNSIGNED_BYTE_2_3_3_REV : { return 1; }
			case LSI_KT_GL_UNSIGNED_SHORT_5_6_5 : { return 2; }
			case LSI_KT_GL_UNSIGNED_SHORT_5_6_5_REV : { return 2; }
			case LSI_KT_GL_UNSIGNED_SHORT_4_4_4_4 : { return 2; }
			case LSI_KT_GL_UNSIGNED_SHORT_4_4_4_4_REV : { return 2; }
			case LSI_KT_GL_UNSIGNED_SHORT_5_5_5_1 : { return 2; }
			case LSI_KT_GL_UNSIGNED_SHORT_1_5_5_5_REV : { return 2; }
			case LSI_KT_GL_UNSIGNED_INT_8_8_8_8 : { return 4; }
			case LSI_KT_GL_UNSIGNED_INT_8_8_8_8_REV : { return 4; }
			case LSI_KT_GL_UNSIGNED_INT_10_10_10_2 : { return 4; }
			case LSI_KT_GL_UNSIGNED_INT_2_10_10_10_REV : { return 4; }
			case LSI_KT_GL_UNSIGNED_INT_10F_11F_11F_REV : { return 4; }
			case LSI_KT_GL_UNSIGNED_INT_5_9_9_9_REV : { return 4; }
			case LSI_KT_GL_UNSIGNED_INT_24_8 : { return 4; }
			case LSI_KT_GL_FLOAT_32_UNSIGNED_INT_24_8_REV : { return 4; }
		}
		assert( 0 );
		return 0;
	}

#ifdef LSI_PRINT_TABLE
	/**
	 * TMP
	 */
	void CKtx::PrintTable() {
		const uint32_t uiTable[] = {
			LSI_KIF_GL_R8,
			LSI_KIF_GL_RG8,
			LSI_KIF_GL_RGB8,
			LSI_KIF_GL_RGBA8,
			LSI_KIF_GL_R8_SNORM,
			LSI_KIF_GL_RG8_SNORM,
			LSI_KIF_GL_RGB8_SNORM,
			LSI_KIF_GL_RGBA8_SNORM,
			LSI_KIF_GL_R8UI,
			LSI_KIF_GL_RG8UI,
			LSI_KIF_GL_RGB8UI,
			LSI_KIF_GL_RGBA8UI,
			LSI_KIF_GL_R8I,
			LSI_KIF_GL_RG8I,
			LSI_KIF_GL_RGB8I,
			LSI_KIF_GL_RGBA8I,
			LSI_KIF_GL_SR8,
			LSI_KIF_GL_SRG8,
			LSI_KIF_GL_SRGB8,
			LSI_KIF_GL_SRGB8_ALPHA8,
			//
			// 16 bits per component
			//
			LSI_KIF_GL_R16,
			LSI_KIF_GL_RG16,
			LSI_KIF_GL_RGB16,
			LSI_KIF_GL_RGBA16,
			LSI_KIF_GL_R16_SNORM,
			LSI_KIF_GL_RG16_SNORM,
			LSI_KIF_GL_RGB16_SNORM,
			LSI_KIF_GL_RGBA16_SNORM,
			LSI_KIF_GL_R16UI,
			LSI_KIF_GL_RG16UI,
			LSI_KIF_GL_RGB16UI,
			LSI_KIF_GL_RGBA16UI,
			LSI_KIF_GL_R16I,
			LSI_KIF_GL_RG16I,
			LSI_KIF_GL_RGB16I,
			LSI_KIF_GL_RGBA16I,
			LSI_KIF_GL_R16F,
			LSI_KIF_GL_RG16F,
			LSI_KIF_GL_RGB16F,
			LSI_KIF_GL_RGBA16F,
			//
			// 32 bits per component
			//
			LSI_KIF_GL_R32UI,
			LSI_KIF_GL_RG32UI,
			LSI_KIF_GL_RGB32UI,
			LSI_KIF_GL_RGBA32UI,
			LSI_KIF_GL_R32I,
			LSI_KIF_GL_RG32I,
			LSI_KIF_GL_RGB32I,
			LSI_KIF_GL_RGBA32I,
			LSI_KIF_GL_R32F,
			LSI_KIF_GL_RG32F,
			LSI_KIF_GL_RGB32F,
			LSI_KIF_GL_RGBA32F,
			//
			// Packed
			//
			LSI_KIF_GL_R3_G3_B2,
			LSI_KIF_GL_RGB4,
			LSI_KIF_GL_RGB5,
			LSI_KIF_GL_RGB565,
			LSI_KIF_GL_RGB10,
			LSI_KIF_GL_RGB12,
			LSI_KIF_GL_RGBA2,
			LSI_KIF_GL_RGBA4,
			LSI_KIF_GL_RGBA12,
			LSI_KIF_GL_RGB5_A1,
			LSI_KIF_GL_RGB10_A2,
			LSI_KIF_GL_RGB10_A2UI,
			LSI_KIF_GL_R11F_G11F_B10F,
			LSI_KIF_GL_RGB9_E5,
			//
			// Alpha
			//
			LSI_KIF_GL_ALPHA4,
			LSI_KIF_GL_ALPHA8,
			LSI_KIF_GL_ALPHA8_SNORM,
			LSI_KIF_GL_ALPHA8UI_EXT,
			LSI_KIF_GL_ALPHA8I_EXT,
			LSI_KIF_GL_ALPHA12,
			LSI_KIF_GL_ALPHA16,
			LSI_KIF_GL_ALPHA16_SNORM,
			LSI_KIF_GL_ALPHA16UI_EXT,
			LSI_KIF_GL_ALPHA16I_EXT,
			LSI_KIF_GL_ALPHA16F_ARB,
			LSI_KIF_GL_ALPHA32UI_EXT,
			LSI_KIF_GL_ALPHA32I_EXT,
			LSI_KIF_GL_ALPHA32F_ARB,
			//
			// Luminance
			//
			LSI_KIF_GL_LUMINANCE4,
			LSI_KIF_GL_LUMINANCE8,
			LSI_KIF_GL_LUMINANCE8_SNORM,
			LSI_KIF_GL_SLUMINANCE8,
			LSI_KIF_GL_LUMINANCE8UI_EXT,
			LSI_KIF_GL_LUMINANCE8I_EXT,
			LSI_KIF_GL_LUMINANCE12,
			LSI_KIF_GL_LUMINANCE16,
			LSI_KIF_GL_LUMINANCE16_SNORM,
			LSI_KIF_GL_LUMINANCE16UI_EXT,
			LSI_KIF_GL_LUMINANCE16I_EXT,
			LSI_KIF_GL_LUMINANCE16F_ARB,
			LSI_KIF_GL_LUMINANCE32UI_EXT,
			LSI_KIF_GL_LUMINANCE32I_EXT,
			LSI_KIF_GL_LUMINANCE32F_ARB,
			//
			// Luminance/Alpha
			//
			LSI_KIF_GL_LUMINANCE4_ALPHA4,
			LSI_KIF_GL_LUMINANCE6_ALPHA2,
			LSI_KIF_GL_LUMINANCE8_ALPHA8,
			LSI_KIF_GL_LUMINANCE8_ALPHA8_SNORM,
			LSI_KIF_GL_SLUMINANCE8_ALPHA8,
			LSI_KIF_GL_LUMINANCE_ALPHA8UI_EXT,
			LSI_KIF_GL_LUMINANCE_ALPHA8I_EXT,
			LSI_KIF_GL_LUMINANCE12_ALPHA4,
			LSI_KIF_GL_LUMINANCE12_ALPHA12,
			LSI_KIF_GL_LUMINANCE16_ALPHA16,
			LSI_KIF_GL_LUMINANCE16_ALPHA16_SNORM,
			LSI_KIF_GL_LUMINANCE_ALPHA16UI_EXT,
			LSI_KIF_GL_LUMINANCE_ALPHA16I_EXT,
			LSI_KIF_GL_LUMINANCE_ALPHA16F_ARB,
			LSI_KIF_GL_LUMINANCE_ALPHA32UI_EXT,
			LSI_KIF_GL_LUMINANCE_ALPHA32I_EXT,
			LSI_KIF_GL_LUMINANCE_ALPHA32F_ARB,
			//
			// Intensity
			//
			LSI_KIF_GL_INTENSITY4,
			LSI_KIF_GL_INTENSITY8,
			LSI_KIF_GL_INTENSITY8_SNORM,
			LSI_KIF_GL_INTENSITY8UI_EXT,
			LSI_KIF_GL_INTENSITY8I_EXT,
			LSI_KIF_GL_INTENSITY12,
			LSI_KIF_GL_INTENSITY16,
			LSI_KIF_GL_INTENSITY16_SNORM,
			LSI_KIF_GL_INTENSITY16UI_EXT,
			LSI_KIF_GL_INTENSITY16I_EXT,
			LSI_KIF_GL_INTENSITY16F_ARB,
			LSI_KIF_GL_INTENSITY32UI_EXT,
			LSI_KIF_GL_INTENSITY32I_EXT,
			LSI_KIF_GL_INTENSITY32F_ARB,
			//
			// Generic compression
			//
			LSI_KIF_GL_COMPRESSED_RED,
			LSI_KIF_GL_COMPRESSED_ALPHA,
			LSI_KIF_GL_COMPRESSED_LUMINANCE,
			LSI_KIF_GL_COMPRESSED_SLUMINANCE,
			LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA,
			LSI_KIF_GL_COMPRESSED_SLUMINANCE_ALPHA,
			LSI_KIF_GL_COMPRESSED_INTENSITY,
			LSI_KIF_GL_COMPRESSED_RG,
			LSI_KIF_GL_COMPRESSED_RGB,
			LSI_KIF_GL_COMPRESSED_RGBA,
			LSI_KIF_GL_COMPRESSED_SRGB,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA,
			//
			// FXT1
			//
			LSI_KIF_GL_COMPRESSED_RGB_FXT1_3DFX,
			LSI_KIF_GL_COMPRESSED_RGBA_FXT1_3DFX,
			//
			// S3TC/DXT/BC
			//
			LSI_KIF_GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
			LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
			LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			LSI_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
			LSI_KIF_GL_COMPRESSED_LUMINANCE_LATC1_EXT,
			LSI_KIF_GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,
			LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT,
			LSI_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT,
			LSI_KIF_GL_COMPRESSED_RED_RGTC1,
			LSI_KIF_GL_COMPRESSED_RG_RGTC2,
			LSI_KIF_GL_COMPRESSED_SIGNED_RED_RGTC1,
			LSI_KIF_GL_COMPRESSED_SIGNED_RG_RGTC2,
			LSI_KIF_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
			LSI_KIF_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
			LSI_KIF_GL_COMPRESSED_RGBA_BPTC_UNORM,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
			//
			// ETC
			//
			LSI_KIF_GL_ETC1_RGB8_OES,
			LSI_KIF_GL_COMPRESSED_RGB8_ETC2,
			LSI_KIF_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
			LSI_KIF_GL_COMPRESSED_RGBA8_ETC2_EAC,
			LSI_KIF_GL_COMPRESSED_SRGB8_ETC2,
			LSI_KIF_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
			LSI_KIF_GL_COMPRESSED_R11_EAC,
			LSI_KIF_GL_COMPRESSED_RG11_EAC,
			LSI_KIF_GL_COMPRESSED_SIGNED_R11_EAC,
			LSI_KIF_GL_COMPRESSED_SIGNED_RG11_EAC,
			//
			// PVRTC
			//
			LSI_KIF_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
			LSI_KIF_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
			LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
			LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,

			LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,
			LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,

			LSI_KIF_GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,

			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG,
			LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG,
			//
			// ASTC
			//
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_12x12_KHR,

			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,

			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_3x3x3_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x3x3_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4x3_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4x4_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x4x4_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5x4_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_5x5x5_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x5x5_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6x5_OES,
			LSI_KIF_GL_COMPRESSED_RGBA_ASTC_6x6x6_OES,

			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,
			LSI_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
			//
			// ATC
			//
			LSI_KIF_GL_ATC_RGB_AMD,
			LSI_KIF_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,
			LSI_KIF_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,
			//
			// Palletized (combined palette)
			//
			LSI_KIF_GL_PALETTE4_RGB8_OES,
			LSI_KIF_GL_PALETTE4_RGBA8_OES,
			LSI_KIF_GL_PALETTE4_R5_G6_B5_OES,
			LSI_KIF_GL_PALETTE4_RGBA4_OES,
			LSI_KIF_GL_PALETTE4_RGB5_A1_OES,
			LSI_KIF_GL_PALETTE8_RGB8_OES,
			LSI_KIF_GL_PALETTE8_RGBA8_OES,
			LSI_KIF_GL_PALETTE8_R5_G6_B5_OES,
			LSI_KIF_GL_PALETTE8_RGBA4_OES,
			LSI_KIF_GL_PALETTE8_RGB5_A1_OES,
			//
			// Palletized (separate palette)
			//
			LSI_KIF_GL_COLOR_INDEX1_EXT,
			LSI_KIF_GL_COLOR_INDEX2_EXT,
			LSI_KIF_GL_COLOR_INDEX4_EXT,
			LSI_KIF_GL_COLOR_INDEX8_EXT,
			LSI_KIF_GL_COLOR_INDEX12_EXT,
			LSI_KIF_GL_COLOR_INDEX16_EXT,
			//
			// Depth/stencil
			//
			LSI_KIF_GL_DEPTH_COMPONENT16,
			LSI_KIF_GL_DEPTH_COMPONENT24,
			LSI_KIF_GL_DEPTH_COMPONENT32,
			LSI_KIF_GL_DEPTH_COMPONENT32F,
			LSI_KIF_GL_DEPTH_COMPONENT32F_NV,
			LSI_KIF_GL_STENCIL_INDEX1,
			LSI_KIF_GL_STENCIL_INDEX4,
			LSI_KIF_GL_STENCIL_INDEX8,
			LSI_KIF_GL_STENCIL_INDEX16,
			LSI_KIF_GL_DEPTH24_STENCIL8,
			LSI_KIF_GL_DEPTH32F_STENCIL8,
			LSI_KIF_GL_DEPTH32F_STENCIL8_NV,
		};

		char szBuffer[1024];
		for ( uint32_t I = 0; I < LSE_ELEMENTS( uiTable ); ++I ) {
			ktxFormatSize fsSize;
			glGetFormatSize( uiTable[I], &fsSize );
			const char * pcIntForm = glInternalFormatToString( static_cast<LSI_KTX_INTERNAL_FORMAT>(uiTable[I]) );
			bool bSrgb = false;
			
			if ( std::strstr( pcIntForm, "SRGB" ) ) {
				bSrgb = true;
			}

			const char * pcType = glTypeToString( static_cast<LSI_KTX_TYPE>(glGetTypeFromInternalFormat( uiTable[I] )) );
			if ( pcType[0] == '\0' ) {
				pcType = "0";
			}
			const char * pcFormat = glBaseInternalFormatToString( static_cast<LSI_KTX_BASE_INTERNAL_FORMAT>(glGetFormatFromInternalFormat( uiTable[I] )) );
			if ( pcFormat[0] == '\0' ) {
				pcFormat = "0";
			}
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "{ LSI_KIF_%s, LSI_KT_%s, LSI_KBIF_%s, 0x%.2X, %u, %u, %u, %u, %u, %u, %u, %s, %s, %s, nullptr },\r\n",
				pcIntForm,
				pcType,
				pcFormat,
				fsSize.flags, fsSize.paletteSizeInBits, fsSize.blockSizeInBits,
				fsSize.blockWidth, fsSize.blockHeight, fsSize.blockDepth,
				fsSize.minBlocksX, fsSize.minBlocksY,
				bSrgb ? "true" : "false",
				(fsSize.blockWidth > 1 || fsSize.blockHeight > 1 || fsSize.blockDepth > 1) ? "true" : "false",
				fsSize.paletteSizeInBits ? "true" : "false" );
			::OutputDebugStringA( szBuffer );

		}
	}
#endif	// #ifdef LSI_PRINT_TABLE

#ifdef LSI_USE_KHRONOS
	/**
	 * Callback to load each face's data from a KTX file.
	 *
	 * \param _iMipLevel Mipmap level, starting from 0.
	 * \param _iFace_ Face index, usually 0, or 0-5 for cube maps.
	 * \param _iWidth Width of the image.
	 * \param _iHeight Height of the image.
	 * \param _iDepth Depth of the image.
	 * \param _ui64FaceLodSize Number of bytes to which _pvPixels points.
	 * \param _pvPixels The actual texture data.
	 * \param _pvUserdata User data passed to the callback for its own use.
	 * \return Returns an error code to indicate failure or success.
	 */
	KTX_error_code CKtx::KtxImageLoad( int _iMipLevel, int /*_iFace_*/,
		int /*_iWidth*/, int /*_iHeight*/, int /*_iDepth*/,
		ktx_uint64_t _ui64FaceLodSize,
		void * _pvPixels, void * _pvUserdata ) {
		if ( _ui64FaceLodSize > LSSTD_MAX_UINT32 ) { return KTX_OUT_OF_MEMORY; }

		LSI_KTX_IMAGE_LOAD * pkilLoad = reinterpret_cast<LSI_KTX_IMAGE_LOAD *>(_pvUserdata);
		if ( _iMipLevel == 0 ) {
			if ( !pkilLoad->ptbReturn->Resize( static_cast<uint32_t>(_ui64FaceLodSize) ) ) {
				return KTX_OUT_OF_MEMORY;
			}
			CStd::MemCpy( &(*pkilLoad->ptbReturn)[0], _pvPixels, static_cast<uintptr_t>(_ui64FaceLodSize) );
		}
		else {
			int iIdx = _iMipLevel - 1;
			if ( iIdx >= pkilLoad->pvMipMaps->Length() ) { return KTX_OUT_OF_MEMORY; }
			if ( !(*pkilLoad->pvMipMaps)[iIdx].Resize( static_cast<uint32_t>(_ui64FaceLodSize) ) ) {
				return KTX_OUT_OF_MEMORY;
			}
			CStd::MemCpy( &(*pkilLoad->pvMipMaps)[iIdx][0], _pvPixels, static_cast<uintptr_t>(_ui64FaceLodSize) );
		}
		return KTX_SUCCESS;
	}
#endif	// LSI_USE_KHRONOS

	/**
	 * Depth-16 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth16ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D16 {
			uint16_t ui16Val;
		};
		const uint64_t ui64RowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = LSE_ROUND_UP( sizeof( LSI_D16 ) * _ui32Width, 4ULL );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					LSI_RGBA32F & rgbaThis = reinterpret_cast<LSI_RGBA32F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_RGBA32F)]);
					const LSI_D16 * prSrc = reinterpret_cast<const LSI_D16 *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_D16)]);
					rgbaThis.fTexel[LSI_PC_R] = prSrc->ui16Val / 65535.0f;
					rgbaThis.fTexel[LSI_PC_G] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_B] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_A] = 1.0f;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA32F -> Depth-16 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth16FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D16 {
			uint16_t ui16Val;
		};
		const uint64_t ui64SrcRowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		const uint64_t ui64RowSize = LSE_ROUND_UP( sizeof( LSI_D16 ) * _ui32Width, 4ULL );
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const LSI_RGBA32F & rgbaThis = reinterpret_cast<const LSI_RGBA32F &>(_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_RGBA32F)]);
					LSI_D16 * prDst = reinterpret_cast<LSI_D16 *>(&_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_D16)]);
					double dLum = (rgbaThis.fTexel[LSI_PC_R] +
						rgbaThis.fTexel[LSI_PC_G] +
						rgbaThis.fTexel[LSI_PC_B]) / 3.0;
					prDst->ui16Val = static_cast<uint16_t>(dLum * 65535.5);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-24 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth24ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D24 {
			uint32_t ui32Val;
		};
		const uint64_t ui64RowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = LSE_ROUND_UP( sizeof( LSI_D24 ) * _ui32Width, 4ULL );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					LSI_RGBA32F & rgbaThis = reinterpret_cast<LSI_RGBA32F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_RGBA32F)]);
					const LSI_D24 * prSrc = reinterpret_cast<const LSI_D24 *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_D24)]);
					rgbaThis.fTexel[LSI_PC_R] = (prSrc->ui32Val >> 8) / 16777215.0f;
					rgbaThis.fTexel[LSI_PC_G] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_B] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_A] = 1.0f;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA32F -> Depth-24 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth24FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D24 {
			uint32_t ui32Val;
		};
		const uint64_t ui64SrcRowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		const uint64_t ui64RowSize = LSE_ROUND_UP( sizeof( LSI_D24 ) * _ui32Width, 4ULL );
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const LSI_RGBA32F & rgbaThis = reinterpret_cast<const LSI_RGBA32F &>(_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_RGBA32F)]);
					LSI_D24 * prDst = reinterpret_cast<LSI_D24 *>(&_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_D24)]);
					double dLum = (rgbaThis.fTexel[LSI_PC_R] +
						rgbaThis.fTexel[LSI_PC_G] +
						rgbaThis.fTexel[LSI_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 16777215.5) << 8;
				}
			}
		}
		return true;
	}

	/**
	 * Depth-32 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth32ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D32 {
			uint32_t ui32Val;
		};
		const uint64_t ui64RowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = LSE_ROUND_UP( sizeof( LSI_D32 ) * _ui32Width, 4ULL );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					LSI_RGBA32F & rgbaThis = reinterpret_cast<LSI_RGBA32F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_RGBA32F)]);
					const LSI_D32 * prSrc = reinterpret_cast<const LSI_D32 *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_D32)]);
					rgbaThis.fTexel[LSI_PC_R] = static_cast<float>(prSrc->ui32Val / 4294967295.0);
					rgbaThis.fTexel[LSI_PC_G] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_B] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_A] = 1.0f;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA32F -> Depth-32 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth32FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D32 {
			uint32_t ui32Val;
		};
		const uint64_t ui64SrcRowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		const uint64_t ui64RowSize = LSE_ROUND_UP( sizeof( LSI_D32 ) * _ui32Width, 4ULL );
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const LSI_RGBA32F & rgbaThis = reinterpret_cast<const LSI_RGBA32F &>(_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_RGBA32F)]);
					LSI_D32 * prDst = reinterpret_cast<LSI_D32 *>(&_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_D32)]);
					double dLum = (rgbaThis.fTexel[LSI_PC_R] +
						rgbaThis.fTexel[LSI_PC_G] +
						rgbaThis.fTexel[LSI_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 4294967295.5);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-32F -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth32FToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D32F {
			float fVal;
		};
		const uint64_t ui64RowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = LSE_ROUND_UP( sizeof( LSI_D32F ) * _ui32Width, 4ULL );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					LSI_RGBA32F & rgbaThis = reinterpret_cast<LSI_RGBA32F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_RGBA32F)]);
					const LSI_D32F * prSrc = reinterpret_cast<const LSI_D32F *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_D32F)]);
					rgbaThis.fTexel[LSI_PC_R] = prSrc->fVal;
					rgbaThis.fTexel[LSI_PC_G] = prSrc->fVal;
					rgbaThis.fTexel[LSI_PC_B] = prSrc->fVal;
					rgbaThis.fTexel[LSI_PC_A] = 1.0f;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA32F -> Depth-32F conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth32FFromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D32F {
			float fVal;
		};
		const uint64_t ui64SrcRowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		const uint64_t ui64RowSize = LSE_ROUND_UP( sizeof( LSI_D32F ) * _ui32Width, 4ULL );
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const LSI_RGBA32F & rgbaThis = reinterpret_cast<const LSI_RGBA32F &>(_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_RGBA32F)]);
					LSI_D32F * prDst = reinterpret_cast<LSI_D32F *>(&_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_D32F)]);
					float fLum = (rgbaThis.fTexel[LSI_PC_R] +
						rgbaThis.fTexel[LSI_PC_G] +
						rgbaThis.fTexel[LSI_PC_B]) / 3.0f;
					prDst->fVal = fLum;
				}
			}
		}
		return true;
	}

	/**
	 * Depth-24/Stencil-8 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth24S8ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D24 {
			uint32_t ui32Val;
		};
		const uint64_t ui64RowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = LSE_ROUND_UP( sizeof( LSI_D24 ) * _ui32Width, 4ULL );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					LSI_RGBA32F & rgbaThis = reinterpret_cast<LSI_RGBA32F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_RGBA32F)]);
					const LSI_D24 * prSrc = reinterpret_cast<const LSI_D24 *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_D24)]);
					rgbaThis.fTexel[LSI_PC_R] = (prSrc->ui32Val >> 8) / 16777215.0f;
					rgbaThis.fTexel[LSI_PC_G] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_B] = rgbaThis.fTexel[LSI_PC_R];
					rgbaThis.fTexel[LSI_PC_A] = static_cast<uint8_t>(prSrc->ui32Val) / 255.0f;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA32F -> Depth-24/Stencil-8 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth24S8FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D24 {
			uint32_t ui32Val;
		};
		const uint64_t ui64SrcRowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		const uint64_t ui64RowSize = LSE_ROUND_UP( sizeof( LSI_D24 ) * _ui32Width, 4ULL );
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const LSI_RGBA32F & rgbaThis = reinterpret_cast<const LSI_RGBA32F &>(_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_RGBA32F)]);
					LSI_D24 * prDst = reinterpret_cast<LSI_D24 *>(&_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_D24)]);
					double dLum = (rgbaThis.fTexel[LSI_PC_R] +
						rgbaThis.fTexel[LSI_PC_G] +
						rgbaThis.fTexel[LSI_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 16777215.5) << 8;
					prDst->ui32Val |= static_cast<uint8_t>(rgbaThis.fTexel[LSI_PC_A] * 255.5f);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-32F/Stencil-8 -> RGBA32F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA32F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth32FS8ToRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D32FS8 {
			float fVal;
			uint32_t ui32Stencil;
		};
		const uint64_t ui64RowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = LSE_ROUND_UP( sizeof( LSI_D32FS8 ) * _ui32Width, 4ULL );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					LSI_RGBA32F & rgbaThis = reinterpret_cast<LSI_RGBA32F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_RGBA32F)]);
					const LSI_D32FS8 * prSrc = reinterpret_cast<const LSI_D32FS8 *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_D32FS8)]);
					rgbaThis.fTexel[LSI_PC_R] = prSrc->fVal;
					rgbaThis.fTexel[LSI_PC_G] = prSrc->fVal;
					rgbaThis.fTexel[LSI_PC_B] = prSrc->fVal;
					rgbaThis.fTexel[LSI_PC_A] = static_cast<uint8_t>(prSrc->ui32Stencil) / 255.0f;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA32F -> Depth-32F/Stencil-8 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA32F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool LSE_CALL CKtx::Depth32FS8FromRgba32F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, void * /*_pvParms*/ ) {
		struct LSI_RGBA32F {
			float fTexel[4];
		};
		struct LSI_D32FS8 {
			float fVal;
			uint32_t ui32Stencil;
		};
		const uint64_t ui64SrcRowSize = sizeof( LSI_RGBA32F ) * _ui32Width;
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		const uint64_t ui64RowSize = LSE_ROUND_UP( sizeof( LSI_D32FS8 ) * _ui32Width, 4ULL );
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const LSI_RGBA32F & rgbaThis = reinterpret_cast<const LSI_RGBA32F &>(_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(LSI_RGBA32F)]);
					LSI_D32FS8 * prDst = reinterpret_cast<LSI_D32FS8 *>(&_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(LSI_D32FS8)]);
					float fLum = (rgbaThis.fTexel[LSI_PC_R] +
						rgbaThis.fTexel[LSI_PC_G] +
						rgbaThis.fTexel[LSI_PC_B]) / 3.0f;
					prDst->fVal = fLum;
					prDst->ui32Stencil = static_cast<uint8_t>(rgbaThis.fTexel[LSI_PC_A] * 255.5f);
				}
			}
		}
		return true;
	}

}	// namespace lsi
