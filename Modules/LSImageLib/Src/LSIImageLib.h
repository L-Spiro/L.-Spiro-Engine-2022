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
 * Description: The heart include file for the L. Spiro Image Library.
 */


#ifndef __LSI_IMAGELIB_H__
#define __LSI_IMAGELIB_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the compression library.
#include "LSCCompressionLib.h"
using namespace ::lsc;

// Include the file library.
#include "LSFFileLib.h"
using namespace ::lsf;

// Include the thread library.
#include "LSHThreadLib.h"
using namespace ::lsh;

// Include the math library.
#include "LSMMathLib.h"
using namespace ::lsm;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the standard library.
#include "LSTLib.h"
using namespace ::lstl;

#include "Float16/LSMFloat16.h"
#include "Vector/LSTLVectorPoD.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*#define LSI_R_WEIGHT								0.299f			// The values are from the YIQ color-conversion equation and not correct in linear color space.
#define LSI_G_WEIGHT								0.587f
#define LSI_B_WEIGHT								0.114f*/
#define LSI_R_WEIGHT								0.212639005871510f
#define LSI_G_WEIGHT								0.715168678767756f
#define LSI_B_WEIGHT								0.072192315360734f
/*#define LSI_R_WEIGHT								0.3086f
#define LSI_G_WEIGHT								0.6094f
#define LSI_B_WEIGHT								0.082f*/

/**
 * Makes a 32-bit RGBA value matching the component order defined by this library.
 */
#define LSI_MAKE_RGBA( R, G, B, A )					static_cast<uint32_t>(((R) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |		\
													((G) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G ))	|							\
													((B) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |							\
													((A) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A )))

/**
 * Makes a 32-bit RGBX value matching the component order defined by this library.  Alpha is set to 0xFF.
 */
#define LSI_MAKE_RGBX( R, G, B )					static_cast<uint32_t>(((R) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |		\
													((G) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |							\
													((B) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |							\
													(0xFF << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A )))
													
/**
 * Makes a 24-bit RGBX value matching the component order defined by this library.  Alpha is set to 0xFF.
 */
#define LSI_MAKE_RGB24( R, G, B )					static_cast<uint32_t>(((R) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_R )) |		\
													((G) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_G )) |								\
													((B) << CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_B )))

/**
 * Gets the R value out of a 32-bit color value.
 */
#define LSI_R( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8A8, LSI_PC_R )) - 1))
/**
 * Gets the G value out of a 32-bit color value.
 */
#define LSI_G( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8A8, LSI_PC_G )) - 1))
/**
 * Gets the B value out of a 32-bit color value.
 */
#define LSI_B( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8A8, LSI_PC_B )) - 1))
/**
 * Gets the A value out of a 32-bit color value.
 */
#define LSI_A( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8A8, LSI_PC_A )) - 1))
/**
 * Gets the R value out of a 24-bit color value.
 */
#define LSI_R24( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_R )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_R )) - 1))
/**
 * Gets the G value out of a 24-bit color value.
 */
#define LSI_G24( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_G )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_G )) - 1))
/**
 * Gets the B value out of a 24-bit color value.
 */
#define LSI_B24( VAL )								(((VAL) >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_B )) & ((1 << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_B )) - 1))


#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

// warning C4293: '>>' : shift count negative or too big, undefined behavior
#pragma warning( disable : 4293 )



namespace lsi {

	// == Enumerations.
	/**
	 * Pixel formats we support natively.
	 */
	enum LSI_PIXEL_FORMAT {
		LSI_PF_A8,									/**< 8-bit alpha channel. */
		LSI_PF_R3G3B2,								/**< 8-bit no alpha. */

		LSI_PF_R5G6B5,								/**< 16-bit no alpha. */
		LSI_PF_R4G4B4A4,							/**< 16-bit with full alpha. */
		LSI_PF_R5G5B5A1,							/**< 16-bit with basic alpha. */

		LSI_PF_R8G8B8,								/**< 24-bit no alpha. */

		LSI_PF_R8G8B8A8,							/**< 32-bit with full alpha. */

		LSI_PF_R16G16B16A16,						/**< 64-bit with full alpha. */

		LSI_PF_TOTAL_INT,							/**< Total integer formats. */

		LSI_PF_R16G16B16A16F = LSI_PF_TOTAL_INT,	/**< 64-bit floating-point format. */
		LSI_PF_R32G32B32A32F,						/**< 128-bit floating-point format. */

		LSI_PF_TOTAL,								/**< Total formats. */

		LSI_PF_DXT1 = 100,							/**< DTX-1 format. Special-case format. */
		LSI_PF_DXT2 = 101,							/**< DTX-2 format. Special-case format. */
		LSI_PF_DXT3 = 102,							/**< DTX-3 format. Special-case format. */
		LSI_PF_DXT4 = 103,							/**< DTX-4 format. Special-case format. */
		LSI_PF_DXT5 = 104,							/**< DTX-5 format. Special-case format. */
		LSI_PF_BC1 = LSI_PF_DXT1,					/**< BC1 format. Special-case format. */
		LSI_PF_BC2 = LSI_PF_DXT3,					/**< BC2 format. Special-case format. */
		LSI_PF_BC3 = LSI_PF_DXT5,					/**< BC3 format. Special-case format. */
		LSI_PF_BC4 = 105,							/**< BC4 format. Special-case format. */
		LSI_PF_BC4U = LSI_PF_BC4,					/**< BC4 format. Special-case format. */
		LSI_PF_BC4S = 106,							/**< BC4 signed format. Special-case format. */
		LSI_PF_BC5 = 107,							/**< BC5 format. Special-case format. */
		LSI_PF_BC5U = LSI_PF_BC5,					/**< BC5 format. Special-case format. */
		LSI_PF_BC5S = 108,							/**< BC5 signed format. Special-case format. */
		LSI_PF_BC6 = 109,							/**< BC6 format. Special-case format. */
		LSI_PF_BC6U = LSI_PF_BC6,					/**< BC6 format. Special-case format. */
		LSI_PF_BC6S = 110,							/**< BC6 signed format. Special-case format. */
		LSI_PF_BC7 = 111,							/**< BC7 format. Special-case format. */
		LSI_PF_BC7U = LSI_PF_BC7,					/**< BC7 format. Special-case format. */
		LSI_PF_BC7_SRGB = 112,						/**< BC7 sRGB format. Special-case format. */
		LSI_PF_DTX_END,
		LSI_PF_DTX_START = LSI_PF_DXT1,

		LSI_PF_ETC1 = 120,							/**< ETC1 RGB8 format. Special-case format. */
		LSI_PF_ETC2,								/**< ETC2 RGB8 format. Special-case format. */
		LSI_PF_ETC2_PT_ALPHA1,						/**< ETC2 RGB8 with punch-through A1 format. Special-case format. */
		LSI_PF_ETC2_EAC,							/**< ETC2 RGBA8 EAC format. Special-case format. */
		LSI_PF_ETC2_SRGB,							/**< ETC2 RGB8 format. Special-case format. */
		LSI_PF_ETC2_PT_ALPHA1_SRGB,					/**< ETC2 RGB8 with punch-through A1 format. Special-case format. */
		LSI_PF_ETC2_EAC_SRGB,						/**< ETC2 RGBA8 EAC format. Special-case format. */
		LSI_PF_R11_EAC,								/**< R11 EAC format. Special-case format. */
		LSI_PF_RG11_EAC,							/**< RG11 EAC format. Special-case format. */
		LSI_PF_R11_EACS,							/**< Signed R11 EAC format. Special-case format. */
		LSI_PF_RG11_EACS,							/**< Signed RG11 EAC format. Special-case format. */
		LSI_PF_ETC_END,
		LSI_PF_ETC_START = LSI_PF_ETC1,

		LSI_PF_ASTC_4_4 = 150,						/**< 4-by-4 ASTC format. Special-case format. */
		LSI_PF_ASTC_5_4,							/**< 5-by-4 ASTC format. Special-case format. */
		LSI_PF_ASTC_5_5,							/**< 5-by-5 ASTC format. Special-case format. */
		LSI_PF_ASTC_6_5,							/**< 6-by-5 ASTC format. Special-case format. */
		LSI_PF_ASTC_6_6,							/**< 6-by-6 ASTC format. Special-case format. */
		LSI_PF_ASTC_8_5,							/**< 8-by-5 ASTC format. Special-case format. */
		LSI_PF_ASTC_8_6,							/**< 8-by-6 ASTC format. Special-case format. */
		LSI_PF_ASTC_8_8,							/**< 8-by-8 ASTC format. Special-case format. */
		LSI_PF_ASTC_10_5,							/**< 10-by-5 ASTC format. Special-case format. */
		LSI_PF_ASTC_10_6,							/**< 10-by-6 ASTC format. Special-case format. */
		LSI_PF_ASTC_10_8,							/**< 10-by-8 ASTC format. Special-case format. */
		LSI_PF_ASTC_10_10,							/**< 10-by-10 ASTC format. Special-case format. */
		LSI_PF_ASTC_12_10,							/**< 12-by-10 ASTC format. Special-case format. */
		LSI_PF_ASTC_12_12,							/**< 12-by-12 ASTC format. Special-case format. */
		LSI_PF_ASTC_END,
		LSI_PF_ASTC_START = LSI_PF_ASTC_4_4,

		LSI_PF_PCRTC_2_RGB_V1 = 180,				/**< 2BPP RGB, version 1. Special-case format. */
		LSI_PF_PCRTC_2_RGBA_V1,						/**< 2BPP RGBA, version 1. Special-case format. */
		LSI_PF_PCRTC_4_RGB_V1,						/**< 4BPP RGB, version 1. Special-case format. */
		LSI_PF_PCRTC_4_RGBA_V1,						/**< 4BPP RGBA, version 1. Special-case format. */
		LSI_PF_PCRTC_2_RGBA_V2,						/**< 2BPP RGBA, version 2. Special-case format. */
		LSI_PF_PCRTC_4_RGBA_V2,						/**< 4BPP RGBA, version 2. Special-case format. */
		LSI_PF_PCRTC_6_HDR_V1,						/**< 6BPP HDR, version 1. Special-case format. */
		LSI_PF_PCRTC_8_HDR_V1,						/**< 8BPP HDR, version 1. Special-case format. */
		LSI_PF_PCRTC_6_HDR_V2,						/**< 6BPP HDR, version 2. Special-case format. */
		LSI_PF_PCRTC_8_HDR_V2,						/**< 8BPP HDR, version 2. Special-case format. */
		LSI_PF_PCRTC_END,
		LSI_PF_PCRTC_START = LSI_PF_PCRTC_2_RGB_V1,

		LSI_PF_KTX1 = 350,							/**< Any contents of a KTX (version 1/OpenGL) file. Special-case format. */
		LSI_PF_KTX2 = 351,							/**< Any contents of a KTX (version 2/Vulkan) file. Special-case format. */

		LSI_PF_ANY = 10000,							/**< Whatever format is in the file. */
		LSI_PF_HIGHEST_INT = LSI_PF_R16G16B16A16,	/**< The highest-quality integer format. */
		
	};

	/**
	 * Component order.
	 */
	enum LSI_PIXEL_COMPONENTS {
		LSI_PC_R,									/**< Red. */
		LSI_PC_G,									/**< Green. */
		LSI_PC_B,									/**< Blue. */
		LSI_PC_A,									/**< Alpha. */
		LSI_PC_INVALID,								/**< Invalid. */
	};

	/** Gamma encoding. */
	enum LSI_GAMMA {
		LSI_G_sRGB,									/**< Image is in sRGB color space. */
		LSI_G_LINEAR,								/**< Image is in linear color space. */
		LSI_G_NONE = -1,							/**< Used for conversions to specify no conversions between sRGB and linear are to take place. */
	};


	/**
	 * Class CImageLib
	 * \brief Image-related functions.
	 *
	 * Description: Generalized image-related functions and template functions.
	 */
	class CImageLib {
	public :
		// == Types.
		/**
		 * A loaded pixel buffer in which each row is tightly packed and padded to 4 bytes.
		 */
		typedef CVectorPoD<uint8_t, uint32_t>		CTexelBuffer;


		// == Functions.
		/**
		 * Initializes the image library.
		 */
		static void LSE_CALL						InitImageLibrary();

		/**
		 * Shuts down the image library.
		 */
		static void LSE_CALL						ShutDownImageLibrary();

		/**
		 * Gets the number of bits for a component of the given format.
		 *
		 * \param _pfFormat The texel format.
		 * \param _ppComp The component whose bit count is to be gotten.
		 * \return Returns the number of bits for a component of the given format.
		 */
		static LSE_INLINE const uint32_t LSE_FCALL	GetComponentBits( LSI_PIXEL_FORMAT _pfFormat, LSI_PIXEL_COMPONENTS _ppComp );

		/**
		 * Gets the offset, in bits, for a component of the given format.
		 *
		 * \param _pfFormat The texel format.
		 * \param _ppComp The component whose bit offset is to be gotten.
		 * \return Returns the offset, in bits, for a component of the given format.
		 */
		static LSE_INLINE const uint32_t LSE_FCALL	GetComponentOffset( LSI_PIXEL_FORMAT _pfFormat, LSI_PIXEL_COMPONENTS _ppComp );

		/**
		 * Gets the number of components in the given format. 
		 *
		 * \param _pfFormat The texel format.
		 * \return Returns the number of components in the given format. 
		 */
		static LSE_INLINE const uint32_t LSE_FCALL	GetTotalComponents( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * If the give format a compressed format?
		 *
		 * \param _pfFormat Format to check for being compressed.
		 * \return Returns true if the given format is a compressed format.
		 */
		static LSE_INLINE LSBOOL LSE_FCALL			IsCompressed( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Converts the component index to a component type.  For example, for the LSI_PF_R8G8B8A8 format, index 0 is translated to
		 *	LSI_PC_A because the alpha component comes first in memory.
		 *
		 * \param _pfFormat The texel format.
		 * \param _ui32Index Index of the component to translate to a component type.
		 * \return Returns the component type of the given component index.
		 */
		static LSE_INLINE LSI_PIXEL_COMPONENTS LSE_FCALL
													ComponentIndexToComponentType( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Index );

		/**
		 * Converts from any integer component to any other integer component.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pfDstFormat The destination texel format.
		 * \param _ppComp The index of the component to convert.
		 * \param _ui32Component The component to convert.
		 * \param _gGammConv If _gGammConv is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 * \return Returns the converted component.
		 */
		static LSE_INLINE const uint32_t LSE_FCALL	ConvComponent( LSI_PIXEL_FORMAT _pfSrcFormat, LSI_PIXEL_FORMAT _pfDstFormat,
			LSI_PIXEL_COMPONENTS _ppComp, uint32_t _ui32Component, LSI_GAMMA _gGammConv );

		/**
		 * Converts a single float value from sRGB space to linear space.  Performs an accurate conversion.
		 *
		 * \param _fVal The value to convert.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE float LSE_FCALL			SRgbToLinear( float _fVal );

		/**
		 * Converts a single float value from linear space to sRGB space.  Performs an accurate conversion.
		 *
		 * \param _fVal The value to convert.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE float LSE_FCALL			LinearToSRgb( float _fVal );

		/**
		 * Converts a single double value from sRGB space to linear space.  Performs an accurate conversion.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE double LSE_FCALL			SRgbToLinear( double _dVal );

		/**
		 * Converts a single double value from linear space to sRGB space.  Performs an accurate conversion.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE double LSE_FCALL			LinearToSRgb( double _dVal );

		/**
		 * Gets the size, in bytes, of a texel format.
		 *
		 * \param _pfFormat The texel format.
		 * \return Returns the size of the given format in bytes.
		 */
		static LSE_INLINE const uint32_t LSE_FCALL	GetFormatSize( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Gets the width of a row of texels in bytes.
		 *
		 * \param _pfFormat Format of the texel data.
		 * \param _ui32Total Number of texels in a row.
		 * \return Returns the number of bytes in a row of tightly packed texel data.
		 */
		static LSE_INLINE uint32_t LSE_FCALL		GetRowSize( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Total );

		/**
		 * Gets the total number of bytes that need to be allocated for an image buffer with the given dimensions. 
		 *	Acccounts for the 8-bit overflow area that is necessary for conversion functions.
		 *
		 * \param _pfFormat Format of the texel data.
		 * \param _ui32Width Number of texels wide the image is.
		 * \param _ui32Height Number of texels tall the image is.
		 * \return Returns the total number of bytes that need to be allocated to safely hold the buffered image with the
		 *	given dimensions and format.
		 */
		static uint32_t LSE_FCALL					GetBufferSizeWithPadding( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Gets the next-best upward promotion of a format.  Returns LSI_PF_ANY if no higher lossless format conversion exists.
		 *
		 * \param _pfFormat The format whose lossless upward promotion is to be obtained.
		 * \return Returns the next format above the given format in which a lossless conversion can be made or LSI_PF_ANY if none can be made.
		 */
		static LSE_INLINE LSI_PIXEL_FORMAT LSE_FCALL
													GetPromotion( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Converts any integer format to any other integer format.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pfDstFormat The destination texel format.
		 * \param _ui64Color The texel to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 * \return Returns the converted texel.  The number of bytes within the return that are valid is
		 *	equal to the number of bytes in the destination format.
		 */
		static uint64_t LSE_FCALL					IntegerTexelToIntegerTexel( LSI_PIXEL_FORMAT _pfSrcFormat, LSI_PIXEL_FORMAT _pfDstFormat, uint64_t _ui64Color, LSI_GAMMA _gGamma );

		/**
		 * Converts any integer format to a 128-bit floating-point format with 32 bits in each component.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _ui64Color The texel to convert.
		 * \param _fReturn Holds the converted color value.  Must be a 4-element array.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static void LSE_FCALL						IntegerTexelToFloat32Texel( LSI_PIXEL_FORMAT _pfSrcFormat, uint64_t _ui64Color, float _fReturn[4], LSI_GAMMA _gGamma );

		/**
		 * Converts from a 32-bit-per-component floating-point format to any integer format.
		 *
		 * \param _pfDstFormat The destination texel format.
		 * \param _fValue The 4-element array of texels in 32-bit floating format to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 * \return Returns the converted texel.  The number of bytes within the return that are valid is
		 *	equal to the number of bytes in the destination format.
		 */
		static uint64_t LSE_FCALL					Float32TexelToIntegerTexel( LSI_PIXEL_FORMAT _pfDstFormat, const float _fValue[4], LSI_GAMMA _gGamma );

		/**
		 * Converts any integer format to a 64-bit floating-point format with 16 bits in each component.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _ui64Color The texel to convert.
		 * \param _f16Return Holds the converted color value.  Must be a 4-element array.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static void LSE_FCALL						IntegerTexelToFloat16Texel( LSI_PIXEL_FORMAT _pfSrcFormat, uint64_t _ui64Color, CFloat16 _f16Return[4], LSI_GAMMA _gGamma );

		/**
		 * Converts from a 16-bit-per-component floating-point format to any integer format.
		 *
		 * \param _pfDstFormat The destination texel format.
		 * \param _f16Value The 4-element array of texels in 16-bit floating format to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 * \return Returns the converted texel.  The number of bytes within the return that are valid is
		 *	equal to the number of bytes in the destination format.
		 */
		static uint64_t LSE_FCALL					Float16TexelToIntegerTexel( LSI_PIXEL_FORMAT _pfDstFormat, const CFloat16 _f16Value[4], LSI_GAMMA _gGamma );
		
		/**
		 * Converts from a 16-bit-per-component floating-point format to 32-bit-per-component floating-point format.
		 *
		 * \param _f16Value The 4-element array of texels in 16-bit floating format to convert.
		 * \param _fReturn Holds the converted color value.  Must be a 4-element array.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static void LSE_FCALL						Float16TexelToFloat32Texel( const CFloat16 _f16Value[4], float _fReturn[4], LSI_GAMMA _gGamma );

		/**
		 * Converts from 4 floating-point values to any integer format.
		 *
		 * \param _pfDstFormat The destination texel format.
		 * \param _fR The red component.
		 * \param _fG The green component.
		 * \param _fB The blue component.
		 * \param _fA The alpha component.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 * \return Returns the converted texel.  The number of bytes within the return that are valid is
		 *	equal to the number of bytes in the destination format.
		 */
		static uint64_t LSE_FCALL					Float32TexelToIntegerTexel( LSI_PIXEL_FORMAT _pfDstFormat,
			float _fR, float _fG, float _fB, float _fA, LSI_GAMMA _gGamma );

		/**
		 * Converts a batch of texels from any integer format to any other integer format.  The destination buffer must be large enough
		 *	to hold the converted texels plus 8 more bytes.  Texels are assumed to be tightly packed in the source buffer, and will be tightly packed in
		 *	the output buffer.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
		 * \param _pfDstFormat The destination texel format.
		 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchIntegerTexelToIntegerTexel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
			LSI_PIXEL_FORMAT _pfDstFormat, void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts a batch of texels from any integer format to a 32-bit-per-component floating-point format.  The destination buffer must be large enough
		 *	to hold the converted texels.  Texels are assumed to be tightly packed in the source buffer, and will be tightly packed in
		 *	the output buffer.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
		 * \param _pvDstBuffer The destination buffer.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchIntegerTexelToFloat32Texel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
			void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts a batch of texels from any integer format to a 16-bit-per-component floating-point format.  The destination buffer must be large enough
		 *	to hold the converted texels.  Texels are assumed to be tightly packed in the source buffer, and will be tightly packed in
		 *	the output buffer.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
		 * \param _pvDstBuffer The destination buffer.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchIntegerTexelToFloat16Texel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
			void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts from a 32-bit-per-component floating-point format to any integer format.  The destination buffer must be large enough
		 *	to hold the converted texels, and the source buffer must be over-allocated by 8 bytes.  Texels are assumed to be tightly packed
		 *	in the source buffer, and will be tightly packed in the output buffer.
		 *
		 * \param _pvSrcBuffer The source buffer.
		 * \param _pfDstFormat The destination texel format.
		 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchFloat32TexelToIntegerTexel( const void * _pvSrcBuffer, LSI_PIXEL_FORMAT _pfDstFormat,
			void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts from a 16-bit-per-component floating-point format to any integer format.  The destination buffer must be large enough
		 *	to hold the converted texels, and the source buffer must be over-allocated by 8 bytes.  Texels are assumed to be tightly packed
		 *	in the source buffer, and will be tightly packed in the output buffer.
		 *
		 * \param _pvSrcBuffer The source buffer.
		 * \param _pfDstFormat The destination texel format.
		 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchFloat16TexelToIntegerTexel( const void * _pvSrcBuffer, LSI_PIXEL_FORMAT _pfDstFormat,
			void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts from a floating-point format to another floating-point format.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
		 * \param _pfDstFormat The destination texel format.
		 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchFloatTexelToFloatTexel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
			LSI_PIXEL_FORMAT _pfDstFormat, void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts from any format to any other format.  Both buffers should be over-allocated by 8 bytes.  Texels are assumed to be tightly
		 *	packed in the source buffer, and will be tightly packed in the output buffer.
		 *
		 * \param _pfSrcFormat The source texel format.
		 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
		 * \param _pfDstFormat The destination texel format.
		 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
		 * \param _ui32Total The total number of texels to convert.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 */
		static LSE_INLINE void LSE_CALL				BatchAnyTexelToAnytexel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
			LSI_PIXEL_FORMAT _pfDstFormat, void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		/**
		 * Converts a 16-bit uint16_t such that 0 = 0.0f and 65535 (0xFFFF) = 1.0f.  For UNORM formats.
		 *
		 * \param _ui16Val The value to convert.
		 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE float LSE_CALL			Uint16ToUNorm( uint16_t _ui16Val, bool _bSrgb );

		/**
		 * Converts a 16-bit uint16_t such that 1 = -1.0f, 32768 = 0.0f, and 65535 (0xFFFF) = 1.0f.  For SNORM formats.
		 *
		 * \param _ui16Val The value to convert.
		 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE float LSE_CALL			Uint16ToSNorm( uint16_t _ui16Val, bool _bSrgb );

		/**
		 * Converts a 16-bit int16_t.  For SNORM formats.
		 *
		 * \param _i16Val The value to convert.
		 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE float LSE_CALL			Int16ToSNorm( int16_t _i16Val, bool _bSrgb );

		/**
		 * Converts an 8-bit uint8_t such that 0 = 0.0f and 255 (0xFF) = 1.0f.  For UNORM formats.
		 *
		 * \param _ui8Val The value to convert.
		 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
		 * \return Returns the converted value.
		 */
		static LSE_INLINE float LSE_CALL			Uint8ToUNorm( uint8_t _ui8Val, bool _bSrgb );

		/**
		 * Performs an accurate conversion from one Uint*UNorm format to another, optionally doing an sRGB <-> Linear conversion.
		 *	Does not use bit replication because that breaks down with source bits 6 and below.
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */




	protected :
		// == Functions.
		/**
		 * Converts a single component from one integer form to another, optionally doing an sRGB <-> Linear conversion.
		 *
		 * \param _ui32Src The source component.
		 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
		 * \return Returns the normalized converted component.
		 */
		template <unsigned _tSrcBits, unsigned _tDstBits, unsigned _tTreat0BitsAsFF>
		static LSE_INLINE uint32_t LSE_FCALL		ConvComp( uint32_t _ui32Src, LSI_GAMMA _gGamma );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the number of bits for a component of the given format.
	 *
	 * \param _pfFormat The texel format.
	 * \param _ppComp The component whose bit count is to be gotten.
	 * \return Returns the number of bits for a component of the given format.
	 */
	LSE_INLINE const uint32_t LSE_FCALL CImageLib::GetComponentBits( LSI_PIXEL_FORMAT _pfFormat, LSI_PIXEL_COMPONENTS _ppComp ) {
		static const uint32_t ui32Bits[LSI_PF_TOTAL_INT][4] = {
			//  LSI_PC_R	LSI_PC_G	LSI_PC_B	LSI_PC_A
			{	0,			0,			0,			8		},	// LSI_PF_A8
			{	3,			3,			2,			0		},	// LSI_PF_R3G3B2
			{	5,			6,			5,			0		},	// LSI_PF_R5G6B5
			{	4,			4,			4,			4		},	// LSI_PF_R4G4B4A4
			{	5,			5,			5,			1		},	// LSI_PF_R5G5B5A1
			{	8,			8,			8,			0		},	// LSI_PF_R8G8B8
			{	8,			8,			8,			8		},	// LSI_PF_R8G8B8A8
			{	16,			16,			16,			16		},	// LSI_PF_R16G16B16A16
		};
		return ui32Bits[_pfFormat][_ppComp];
	}

	/**
	 * Gets the offset, in bits, for a component of the given format.
	 *
	 * \param _pfFormat The texel format.
	 * \param _ppComp The component whose bit offset is to be gotten.
	 * \return Returns the offset, in bits, for a component of the given format.
	 */
	LSE_INLINE const uint32_t LSE_FCALL CImageLib::GetComponentOffset( LSI_PIXEL_FORMAT _pfFormat, LSI_PIXEL_COMPONENTS _ppComp ) {
		static const uint32_t ui32Offsets[LSI_PF_TOTAL_INT][4] = {
			//  LSI_PC_R	LSI_PC_G	LSI_PC_B	LSI_PC_A
			{	0,			0,			0,			0		},	// LSI_PF_A8
			{	5,			2,			0,			0		},	// LSI_PF_R3G3B2
			{	11,			5,			0,			0		},	// LSI_PF_R5G6B5
			{	12,			8,			4,			0		},	// LSI_PF_R4G4B4A4
			{	11,			6,			1,			0		},	// LSI_PF_R5G5B5A1
			{	16,			8,			0,			0		},	// LSI_PF_R8G8B8
			{	24,			16,			8,			0		},	// LSI_PF_R8G8B8A8
			{	48,			32,			16,			0		},	// LSI_PF_R16G16B16A16
		};
		return ui32Offsets[_pfFormat][_ppComp];
	}

	/**
	 * Gets the number of components in the given format. 
	 *
	 * \param _pfFormat The texel format.
	 * \return Returns the number of components in the given format. 
	 */
	LSE_INLINE const uint32_t LSE_FCALL	CImageLib::GetTotalComponents( LSI_PIXEL_FORMAT _pfFormat ) {
		static const uint32_t ui32Comp[LSI_PF_TOTAL] = {
			1,	// LSI_PF_A8
			3,	// LSI_PF_R3G3B2
			3,	// LSI_PF_R5G6B5
			4,	// LSI_PF_R4G4B4A4
			4,	// LSI_PF_R5G5B5A1
			3,	// LSI_PF_R8G8B8
			4,	// LSI_PF_R8G8B8A8
			4,	// LSI_PF_R16G16B16A16
			4,	// LSI_PF_R16G16B16A16F
			4,	// LSI_PF_R32G32B32A32F
		};
		return ui32Comp[_pfFormat];
	}

	/**
	 * If the give format a compressed format?
	 *
	 * \param _pfFormat Format to check for being compressed.
	 * \return Returns true if the given format is a compressed format.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CImageLib::IsCompressed( LSI_PIXEL_FORMAT _pfFormat ) {
		return (_pfFormat >= LSI_PF_DTX_START && _pfFormat < LSI_PF_DTX_END) ||
			(_pfFormat >= LSI_PF_ETC_START && _pfFormat < LSI_PF_ETC_END) ||
			(_pfFormat >= LSI_PF_ASTC_START && _pfFormat < LSI_PF_ASTC_END) ||
			(_pfFormat >= LSI_PF_PCRTC_START && _pfFormat < LSI_PF_PCRTC_END);
	}

	/**
	 * Converts the component index to a component type.  For example, for the LSI_PF_R8G8B8A8 format, index 0 is translated to
	 *	LSI_PC_A because the alpha component comes first in memory.
	 *
	 * \param _pfFormat The texel format.
	 * \param _ui32Index Index of the component to translate to a component type.
	 * \return Returns the component type of the given component index.
	 */
	LSE_INLINE LSI_PIXEL_COMPONENTS LSE_FCALL CImageLib::ComponentIndexToComponentType( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Index ) {
		static const LSI_PIXEL_COMPONENTS pcTable[LSI_PF_TOTAL][4] = {
			//  0				1				2				3
			{	LSI_PC_A,		LSI_PC_INVALID,	LSI_PC_INVALID,	LSI_PC_INVALID	},	// LSI_PF_A8
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R3G3B2
			{	LSI_PC_B,		LSI_PC_G,		LSI_PC_R,		LSI_PC_INVALID	},	// LSI_PF_R5G6B5
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R4G4B4A4
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R5G5B5A1
			{	LSI_PC_B,		LSI_PC_G,		LSI_PC_R,		LSI_PC_INVALID	},	// LSI_PF_R8G8B8
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R8G8B8A8
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R16G16B16A16
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R16G16B16A16F
			{	LSI_PC_A,		LSI_PC_B,		LSI_PC_G,		LSI_PC_R		},	// LSI_PF_R32G32B32A32F
		};
		return pcTable[_pfFormat][_ui32Index];
	}

	/**
	 * Converts from any integer component to any other integer component.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _pfDstFormat The destination texel format.
	 * \param _ppComp The index of the component to convert.
	 * \param _ui32Component The component to convert.
	 * \param _gGammConv If _gGammConv is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 * \return Returns the converted component.
	 */
	LSE_INLINE const uint32_t LSE_FCALL	CImageLib::ConvComponent( LSI_PIXEL_FORMAT _pfSrcFormat, LSI_PIXEL_FORMAT _pfDstFormat,
		LSI_PIXEL_COMPONENTS _ppComp, uint32_t _ui32Component, LSI_GAMMA _gGammConv ) {
		typedef uint32_t (LSE_FCALL * PfConverterFunc)( uint32_t _ui32Src, LSI_GAMMA _gGamma );
		static const PfConverterFunc pfTable[LSI_PF_TOTAL_INT][LSI_PF_TOTAL_INT][4] = {
			{	// LSI_PF_A8
				{	// LSI_PF_A8 -> LSI_PF_A8
					CImageLib::ConvComp<0, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<0, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<0, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<0, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<0, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<0, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<0, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_A8 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<0, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<0, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<0, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R3G3B2
				{	// LSI_PF_R3G3B2 -> LSI_PF_A8
					CImageLib::ConvComp<3, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<3, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<3, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<3, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<3, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<3, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<3, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R3G3B2 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<3, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<3, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<2, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R5G6B5
				{	// LSI_PF_R5G6B5 -> LSI_PF_A8
					CImageLib::ConvComp<5, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<5, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<5, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G6B5 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<5, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<6, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R4G4B4A4
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_A8
					CImageLib::ConvComp<4, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<4, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<4, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<4, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<4, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<4, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<4, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R4G4B4A4 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<4, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<4, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<4, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<4, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R5G5B5A1
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_A8
					CImageLib::ConvComp<5, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<5, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<5, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R5G5B5A1 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<5, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<5, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<5, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<1, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R8G8B8
				{	// LSI_PF_R8G8B8 -> LSI_PF_A8
					CImageLib::ConvComp<8, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<8, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<8, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<8, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<0, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R8G8B8A8
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_A8
					CImageLib::ConvComp<8, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<8, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<8, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R8G8B8A8 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<8, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<8, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<8, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<8, 16, true>,	// LSI_PP_A
				},
			},
			{	// LSI_PF_R16G16B16A16
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_A8
					CImageLib::ConvComp<16, 0, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 0, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 0, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R3G3B2
					CImageLib::ConvComp<16, 3, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 3, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 2, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R5G6B5
					CImageLib::ConvComp<16, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 6, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R4G4B4A4
					CImageLib::ConvComp<16, 4, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 4, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 4, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 4, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R5G5B5A1
					CImageLib::ConvComp<16, 5, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 5, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 5, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 1, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R8G8B8
					CImageLib::ConvComp<16, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 0, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R8G8B8A8
					CImageLib::ConvComp<16, 8, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 8, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 8, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 8, true>,	// LSI_PP_A
				},
				{	// LSI_PF_R16G16B16A16 -> LSI_PF_R16G16B16A16
					CImageLib::ConvComp<16, 16, false>,	// LSI_PP_R
					CImageLib::ConvComp<16, 16, false>,	// LSI_PP_G
					CImageLib::ConvComp<16, 16, false>,	// LSI_PP_B
					CImageLib::ConvComp<16, 16, true>,	// LSI_PP_A
				},
			},
		};
		return pfTable[_pfSrcFormat][_pfDstFormat][_ppComp]( _ui32Component, _gGammConv );
	}

	/**
	 * Converts a single float value from sRGB space to linear space.  Performs an accurate conversion.
	 *
	 * \param _fVal The value to convert.
	 * \return Returns the converted value.
	 */
	LSE_INLINE float LSE_FCALL CImageLib::SRgbToLinear( float _fVal ) {
		return _fVal <= 0.04045f ?
			_fVal * (1.0f / 12.92f) :
			std::powf( (_fVal + 0.055f) * (1.0f / 1.055f), 2.4f );
	}

	/**
	 * Converts a single float value from linear space to sRGB space.  Performs an accurate conversion.
	 *
	 * \param _fVal The value to convert.
	 * \return Returns the converted value.
	 */
	LSE_INLINE float LSE_FCALL CImageLib::LinearToSRgb( float _fVal ) {
		return _fVal <= 0.0031308f ?
			_fVal * 12.92f :
			1.055f * std::powf( _fVal, 1.0f / 2.4f ) - 0.055f;
	}

	/**
	 * Converts a single double value from sRGB space to linear space.  Performs an accurate conversion.
	 *
	 * \param _dVal The value to convert.
	 * \return Returns the converted value.
	 */
	LSE_INLINE double LSE_FCALL CImageLib::SRgbToLinear( double _dVal ) {
		return _dVal <= 0.04045 ?
			_dVal * (1.0 / 12.92) :
			std::pow( (_dVal + 0.055) * (1.0 / 1.055), 2.4 );
	}

	/**
	 * Converts a single double value from linear space to sRGB space.  Performs an accurate conversion.
	 *
	 * \param _dVal The value to convert.
	 * \return Returns the converted value.
	 */
	LSE_INLINE double LSE_FCALL CImageLib::LinearToSRgb( double _dVal ) {
		return _dVal <= 0.0031308 ?
			_dVal * 12.92 :
			1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
	}

	/**
	 * Gets the size, in bytes, of a texel format.
	 *
	 * \param _pfFormat The texel format.
	 * \return Returns the size of the given format in bytes.
	 */
	LSE_INLINE const uint32_t LSE_FCALL	CImageLib::GetFormatSize( LSI_PIXEL_FORMAT _pfFormat ) {
		static const uint32_t ui32Sizes[LSI_PF_TOTAL] = {
			1,	// LSI_PF_A8
			1,	// LSI_PF_R3G3B2
			2,	// LSI_PF_R5G6B5
			2,	// LSI_PF_R4G4B4A4
			2,	// LSI_PF_R5G5B5A1
			3,	// LSI_PF_R8G8B8
			4,	// LSI_PF_R8G8B8A8
			8,	// LSI_PF_R16G16B16A16
			8,	// LSI_PF_R16G16B16A16F
			16,	// LSI_PF_R32G32B32A32F
		};

		return ui32Sizes[_pfFormat];
	}

	/**
	 * Gets the width of a row of texels in bytes.
	 *
	 * \param _pfFormat Format of the texel data.
	 * \param _ui32Total Number of texels in a row.
	 * \return Returns the number of bytes in a row of tightly packed texel data.
	 */
	LSE_INLINE uint32_t LSE_FCALL CImageLib::GetRowSize( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Total ) {
		return _ui32Total * GetFormatSize( _pfFormat );
	}

	/**
	 * Gets the next-best upward promotion of a format.  Returns LSI_PF_ANY if no higher lossless format conversion exists.
	 *
	 * \param _pfFormat The format whose lossless upward promotion is to be obtained.
	 * \return Returns the next format above the given format in which a lossless conversion can be made or LSI_PF_ANY if none can be made.
	 */
	LSE_INLINE LSI_PIXEL_FORMAT LSE_FCALL CImageLib::GetPromotion( LSI_PIXEL_FORMAT _pfFormat ) {
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) {
			static const LSI_PIXEL_FORMAT pfTable[LSI_PF_TOTAL] = {
				LSI_PF_R8G8B8A8,			// LSI_PF_A8
				LSI_PF_R5G6B5,				// LSI_PF_R3G3B2
				LSI_PF_R8G8B8,				// LSI_PF_R5G6B5
				LSI_PF_R8G8B8A8,			// LSI_PF_R4G4B4A4
				LSI_PF_R8G8B8A8,			// LSI_PF_R5G5B5A1
				LSI_PF_R8G8B8A8,			// LSI_PF_R8G8B8
				LSI_PF_R16G16B16A16,		// LSI_PF_R8G8B8A8
				LSI_PF_R32G32B32A32F,		// LSI_PF_R16G16B16A16
				LSI_PF_R32G32B32A32F,		// LSI_PF_R16G16B16A16F
				LSI_PF_ANY,					// LSI_PF_R32G32B32A32F
			};

			return pfTable[_pfFormat];
		}
		return LSI_PF_ANY;
	}

	/**
	 * Converts a batch of texels from any integer format to any other integer format.  The destination buffer must be large enough
	 *	to hold the converted texels plus 8 more bytes.  Texels are assumed to be tightly packed in the source buffer, and will be tightly packed in
	 *	the output buffer.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
	 * \param _pfDstFormat The destination texel format.
	 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchIntegerTexelToIntegerTexel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
		LSI_PIXEL_FORMAT _pfDstFormat, void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		if ( _pfSrcFormat == _pfDstFormat && _gGamma == LSI_G_NONE ) {
			// Just copy the buffer.
			CStd::MemCpy( _pvDstBuffer, _pvSrcBuffer, GetFormatSize( _pfSrcFormat ) * _ui32Total );
			return;
		}
		// Convert texel-by-texel.
		const uint8_t * pui8Src = static_cast<const uint8_t *>(_pvSrcBuffer);
		uint8_t * pui8Dst = static_cast<uint8_t *>(_pvDstBuffer);
		uint32_t ui32SrcSize = GetFormatSize( _pfSrcFormat );
		uint32_t ui32DstSize = GetFormatSize( _pfDstFormat );

		for ( ; _ui32Total--; ) {
			// ARM devices have problems reading from misaligned data.
#ifdef LSE_IPHONE
			uint64_t ui64Src = (static_cast<uint64_t>(pui8Src[7]) << 56ULL) |
				(static_cast<uint64_t>(pui8Src[6]) << 48ULL) |
				(static_cast<uint64_t>(pui8Src[5]) << 40ULL) |
				(static_cast<uint64_t>(pui8Src[4]) << 32ULL) |
				(static_cast<uint64_t>(pui8Src[3]) << 24ULL) |
				(static_cast<uint64_t>(pui8Src[2]) << 16ULL) |
				(static_cast<uint64_t>(pui8Src[1]) << 8ULL) |
				(static_cast<uint64_t>(pui8Src[0]) << 0ULL);
			uint64_t ui64Dst = IntegerTexelToIntegerTexel( _pfSrcFormat, _pfDstFormat, ui64Src, _gGamma );
			pui8Dst[7] = static_cast<uint8_t>((ui64Dst >> 56ULL) & 0xFF);
			pui8Dst[6] = static_cast<uint8_t>((ui64Dst >> 48ULL) & 0xFF);
			pui8Dst[5] = static_cast<uint8_t>((ui64Dst >> 40ULL) & 0xFF);
			pui8Dst[4] = static_cast<uint8_t>((ui64Dst >> 32ULL) & 0xFF);
			pui8Dst[3] = static_cast<uint8_t>((ui64Dst >> 24ULL) & 0xFF);
			pui8Dst[2] = static_cast<uint8_t>((ui64Dst >> 16ULL) & 0xFF);
			pui8Dst[1] = static_cast<uint8_t>((ui64Dst >> 8ULL) & 0xFF);
			pui8Dst[0] = static_cast<uint8_t>((ui64Dst >> 0ULL) & 0xFF);
#else
			uint64_t ui64Src = (*reinterpret_cast<const uint64_t *>(pui8Src));
			uint64_t ui64Dst = IntegerTexelToIntegerTexel( _pfSrcFormat, _pfDstFormat, ui64Src, _gGamma );
			(*reinterpret_cast<uint64_t *>(pui8Dst)) = ui64Dst;
#endif	// #ifdef LSE_IPHONE
			pui8Src += ui32SrcSize;
			pui8Dst += ui32DstSize;
		}
	}

	/**
	 * Converts a batch of texels from any integer format to a 32-bit-per-component floating-point format.  The destination buffer must be large enough
	 *	to hold the converted texels.  Texels are assumed to be tightly packed in the source buffer, and will be tightly packed in
	 *	the output buffer.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
	 * \param _pvDstBuffer The destination buffer.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchIntegerTexelToFloat32Texel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
		void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		// Convert texel-by-texel.
		const uint8_t * pui8Src = static_cast<const uint8_t *>(_pvSrcBuffer);
		float * pfDst = static_cast<float *>(_pvDstBuffer);
		uint32_t ui32SrcSize = GetFormatSize( _pfSrcFormat );

		for ( ; _ui32Total--; ) {
#ifdef LSE_IPHONE
			uint64_t ui64Src = (static_cast<uint64_t>(pui8Src[7]) << 56ULL) |
				(static_cast<uint64_t>(pui8Src[6]) << 48ULL) |
				(static_cast<uint64_t>(pui8Src[5]) << 40ULL) |
				(static_cast<uint64_t>(pui8Src[4]) << 32ULL) |
				(static_cast<uint64_t>(pui8Src[3]) << 24ULL) |
				(static_cast<uint64_t>(pui8Src[2]) << 16ULL) |
				(static_cast<uint64_t>(pui8Src[1]) << 8ULL) |
				(static_cast<uint64_t>(pui8Src[0]) << 0ULL);
#else
			uint64_t ui64Src = (*reinterpret_cast<const uint64_t *>(pui8Src));
#endif	// #ifdef LSE_IPHONE
			IntegerTexelToFloat32Texel( _pfSrcFormat, ui64Src, pfDst, _gGamma );
			pfDst += 4;
			pui8Src += ui32SrcSize;
		}
	}

	/**
	 * Converts a batch of texels from any integer format to a 16-bit-per-component floating-point format.  The destination buffer must be large enough
	 *	to hold the converted texels.  Texels are assumed to be tightly packed in the source buffer, and will be tightly packed in
	 *	the output buffer.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
	 * \param _pvDstBuffer The destination buffer.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchIntegerTexelToFloat16Texel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
		void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		// Convert texel-by-texel.
		const uint8_t * pui8Src = static_cast<const uint8_t *>(_pvSrcBuffer);
		CFloat16 * pf16Dst = static_cast<CFloat16 *>(_pvDstBuffer);
		uint32_t ui32SrcSize = GetFormatSize( _pfSrcFormat );

		for ( ; _ui32Total--; ) {
#ifdef LSE_IPHONE
			uint64_t ui64Src = (static_cast<uint64_t>(pui8Src[7]) << 56ULL) |
				(static_cast<uint64_t>(pui8Src[6]) << 48ULL) |
				(static_cast<uint64_t>(pui8Src[5]) << 40ULL) |
				(static_cast<uint64_t>(pui8Src[4]) << 32ULL) |
				(static_cast<uint64_t>(pui8Src[3]) << 24ULL) |
				(static_cast<uint64_t>(pui8Src[2]) << 16ULL) |
				(static_cast<uint64_t>(pui8Src[1]) << 8ULL) |
				(static_cast<uint64_t>(pui8Src[0]) << 0ULL);
#else
			uint64_t ui64Src = (*reinterpret_cast<const uint64_t *>(pui8Src));
#endif	// #ifdef LSE_IPHONE
			IntegerTexelToFloat16Texel( _pfSrcFormat, ui64Src, pf16Dst, _gGamma );
			pf16Dst += 4;
			pui8Src += ui32SrcSize;
		}
	}

	/**
	 * Converts from a 32-bit-per-component floating-point format to any integer format.  The destination buffer must be large enough
	 *	to hold the converted texels, and the source buffer must be over-allocated by 8 bytes.  Texels are assumed to be tightly packed
	 *	in the source buffer, and will be tightly packed in the output buffer.
	 *
	 * \param _pvSrcBuffer The source buffer.
	 * \param _pfDstFormat The destination texel format.
	 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchFloat32TexelToIntegerTexel( const void * _pvSrcBuffer, LSI_PIXEL_FORMAT _pfDstFormat,
		void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		// Convert texel-by-texel.
		const float * pfSrc = static_cast<const float *>(_pvSrcBuffer);
		uint8_t * pui8Dst = static_cast<uint8_t *>(_pvDstBuffer);
		uint32_t ui32DstSize = GetFormatSize( _pfDstFormat );

		for ( ; _ui32Total--; ) {
			(*reinterpret_cast<uint64_t *>(pui8Dst)) = Float32TexelToIntegerTexel( _pfDstFormat, pfSrc, _gGamma );
			pfSrc += 4;
			pui8Dst += ui32DstSize;
		}
	}

	/**
	 * Converts from a 16-bit-per-component floating-point format to any integer format.  The destination buffer must be large enough
	 *	to hold the converted texels, and the source buffer must be over-allocated by 8 bytes.  Texels are assumed to be tightly packed
	 *	in the source buffer, and will be tightly packed in the output buffer.
	 *
	 * \param _pvSrcBuffer The source buffer.
	 * \param _pfDstFormat The destination texel format.
	 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchFloat16TexelToIntegerTexel( const void * _pvSrcBuffer, LSI_PIXEL_FORMAT _pfDstFormat,
		void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		// Convert texel-by-texel.
		const CFloat16 * pf16Src = static_cast<const CFloat16 *>(_pvSrcBuffer);
		uint8_t * pui8Dst = static_cast<uint8_t *>(_pvDstBuffer);
		uint32_t ui32DstSize = GetFormatSize( _pfDstFormat );

		for ( ; _ui32Total--; ) {
			(*reinterpret_cast<uint64_t *>(pui8Dst)) = Float16TexelToIntegerTexel( _pfDstFormat, pf16Src, _gGamma );
			pf16Src += 4;
			pui8Dst += ui32DstSize;
		}
	}

	/**
	 * Converts from a floating-point format to another floating-point format.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
	 * \param _pfDstFormat The destination texel format.
	 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchFloatTexelToFloatTexel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
		LSI_PIXEL_FORMAT _pfDstFormat, void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		if ( _pfSrcFormat == _pfDstFormat && _gGamma == LSI_G_NONE ) {
			// Just copy the buffer.
			CStd::MemCpy( _pvDstBuffer, _pvSrcBuffer, GetFormatSize( _pfSrcFormat ) * _ui32Total );
			return;
		}
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				if ( _pfSrcFormat == LSI_PF_R16G16B16A16F && _pfDstFormat == LSI_PF_R16G16B16A16F ) {
					const CFloat16 * pf16Src = static_cast<const CFloat16 *>(_pvSrcBuffer);
					CFloat16 * pf16Dst = static_cast<CFloat16 *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pf16Dst++) = LinearToSRgb( (*pf16Src++) );
						(*pf16Dst++) = LinearToSRgb( (*pf16Src++) );
						(*pf16Dst++) = LinearToSRgb( (*pf16Src++) );
						(*pf16Dst++) = (*pf16Src++);
					}
				}
				if ( _pfSrcFormat == LSI_PF_R32G32B32A32F && _pfDstFormat == LSI_PF_R16G16B16A16F ) {
					const float * pfSrc = static_cast<const float *>(_pvSrcBuffer);
					CFloat16 * pf16Dst = static_cast<CFloat16 *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pf16Dst++) = LinearToSRgb( (*pfSrc++) );
						(*pf16Dst++) = LinearToSRgb( (*pfSrc++) );
						(*pf16Dst++) = LinearToSRgb( (*pfSrc++) );
						(*pf16Dst++) = (*pfSrc++);
					}
				}
				if ( _pfSrcFormat == LSI_PF_R16G16B16A16F && _pfDstFormat == LSI_PF_R32G32B32A32F ) {
					const CFloat16 * pf16Src = static_cast<const CFloat16 *>(_pvSrcBuffer);
					float * pfDst = static_cast<float *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pfDst++) = LinearToSRgb( (*pf16Src++) );
						(*pfDst++) = LinearToSRgb( (*pf16Src++) );
						(*pfDst++) = LinearToSRgb( (*pf16Src++) );
						(*pfDst++) = (*pf16Src++);
					}
				}
				if ( _pfSrcFormat == LSI_PF_R32G32B32A32F && _pfDstFormat == LSI_PF_R32G32B32A32F ) {
					const float * pfSrc = static_cast<const float *>(_pvSrcBuffer);
					float * pfDst = static_cast<float *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pfDst++) = LinearToSRgb( (*pfSrc++) );
						(*pfDst++) = LinearToSRgb( (*pfSrc++) );
						(*pfDst++) = LinearToSRgb( (*pfSrc++) );
						(*pfDst++) = (*pfSrc++);
					}
				}
				return;
			}
			case LSI_G_LINEAR : {
				if ( _pfSrcFormat == LSI_PF_R16G16B16A16F && _pfDstFormat == LSI_PF_R16G16B16A16F ) {
					const CFloat16 * pf16Src = static_cast<const CFloat16 *>(_pvSrcBuffer);
					CFloat16 * pf16Dst = static_cast<CFloat16 *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pf16Dst++) = SRgbToLinear( (*pf16Src++) );
						(*pf16Dst++) = SRgbToLinear( (*pf16Src++) );
						(*pf16Dst++) = SRgbToLinear( (*pf16Src++) );
						(*pf16Dst++) = (*pf16Src++);
					}
				}
				if ( _pfSrcFormat == LSI_PF_R32G32B32A32F && _pfDstFormat == LSI_PF_R16G16B16A16F ) {
					const float * pfSrc = static_cast<const float *>(_pvSrcBuffer);
					CFloat16 * pf16Dst = static_cast<CFloat16 *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pf16Dst++) = SRgbToLinear( (*pfSrc++) );
						(*pf16Dst++) = SRgbToLinear( (*pfSrc++) );
						(*pf16Dst++) = SRgbToLinear( (*pfSrc++) );
						(*pf16Dst++) = (*pfSrc++);
					}
				}
				if ( _pfSrcFormat == LSI_PF_R16G16B16A16F && _pfDstFormat == LSI_PF_R32G32B32A32F ) {
					const CFloat16 * pf16Src = static_cast<const CFloat16 *>(_pvSrcBuffer);
					float * pfDst = static_cast<float *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pfDst++) = SRgbToLinear( (*pf16Src++) );
						(*pfDst++) = SRgbToLinear( (*pf16Src++) );
						(*pfDst++) = SRgbToLinear( (*pf16Src++) );
						(*pfDst++) = (*pf16Src++);
					}
				}
				if ( _pfSrcFormat == LSI_PF_R32G32B32A32F && _pfDstFormat == LSI_PF_R32G32B32A32F ) {
					const float * pfSrc = static_cast<const float *>(_pvSrcBuffer);
					float * pfDst = static_cast<float *>(_pvDstBuffer);
					while ( _ui32Total-- ) {
						(*pfDst++) = SRgbToLinear( (*pfSrc++) );
						(*pfDst++) = SRgbToLinear( (*pfSrc++) );
						(*pfDst++) = SRgbToLinear( (*pfSrc++) );
						(*pfDst++) = (*pfSrc++);
					}
				}
				return;
			}
		}
		// _gGamma == LSI_G_NONE, which means we came here because of a pair different floating types.

		// Convert texel-by-texel.
		if ( _pfSrcFormat == LSI_PF_R16G16B16A16F ) {
			// From 16 to 32.
			const CFloat16 * pf16Src = static_cast<const CFloat16 *>(_pvSrcBuffer);
			float * pfDst = static_cast<float *>(_pvDstBuffer);

			for ( _ui32Total <<= 2; _ui32Total--; ) {
				(*pfDst++) = (*pf16Src++);
			}
		}
		else {
			const float * pfSrc = static_cast<const float *>(_pvSrcBuffer);
			CFloat16 * pf16Dst = static_cast<CFloat16 *>(_pvDstBuffer);
			for ( _ui32Total <<= 2; _ui32Total--; ) {
				(*pf16Dst++) = (*pfSrc++);
			}
		}
	}

	/**
	 * Converts from any format to any other format.  Both buffers should be over-allocated by 8 bytes.  Texels are assumed to be tightly
	 *	packed in the source buffer, and will be tightly packed in the output buffer.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _pvSrcBuffer The source buffer.  Must be over-allocated by 8 bytes.
	 * \param _pfDstFormat The destination texel format.
	 * \param _pvDstBuffer The destination buffer.  Must be over-allocated by 8 bytes.
	 * \param _ui32Total The total number of texels to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	LSE_INLINE void LSE_CALL CImageLib::BatchAnyTexelToAnytexel( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
		LSI_PIXEL_FORMAT _pfDstFormat, void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma ) {
		// Same format?
		if ( _pfSrcFormat == _pfDstFormat && _gGamma == LSI_G_NONE ) {
			// Just copy.
			CStd::MemCpy( _pvDstBuffer, _pvSrcBuffer, _ui32Total * GetFormatSize( _pfSrcFormat ) );
			return;
		}

		// Both integer types?
		if ( _pfSrcFormat < LSI_PF_TOTAL_INT && _pfDstFormat < LSI_PF_TOTAL_INT ) {
			BatchIntegerTexelToIntegerTexel( _pfSrcFormat, _pvSrcBuffer, _pfDstFormat, _pvDstBuffer, _ui32Total, _gGamma );
			return;
		}

		// Both floating types?
		if ( _pfSrcFormat >= LSI_PF_TOTAL_INT && _pfDstFormat >= LSI_PF_TOTAL_INT ) {
			BatchFloatTexelToFloatTexel( _pfSrcFormat, _pvSrcBuffer, _pfDstFormat, _pvDstBuffer, _ui32Total, _gGamma );
			return;
		}


		// One side is an integer format and the other side is a float format.
		if ( _pfSrcFormat < LSI_PF_TOTAL_INT ) {
			// Int to float.
			typedef void (LSE_CALL * PfConvMe)( LSI_PIXEL_FORMAT _pfSrcFormat, const void * _pvSrcBuffer,
				void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

			static const PfConvMe pfConvFuncs[2] = {
				BatchIntegerTexelToFloat16Texel,
				BatchIntegerTexelToFloat32Texel
			};
			pfConvFuncs[_pfDstFormat-LSI_PF_TOTAL_INT]( _pfSrcFormat, _pvSrcBuffer, _pvDstBuffer, _ui32Total, _gGamma );
			return;
		}
		// Float to int.
		typedef void (LSE_CALL * PfConvMe)( const void * _pvSrcBuffer, LSI_PIXEL_FORMAT _pfDstFormat,
			void * _pvDstBuffer, uint32_t _ui32Total, LSI_GAMMA _gGamma );

		static const PfConvMe pfConvFuncs[2] = {
			BatchFloat16TexelToIntegerTexel,
			BatchFloat32TexelToIntegerTexel
		};
		pfConvFuncs[_pfSrcFormat-LSI_PF_TOTAL_INT]( _pvSrcBuffer, _pfDstFormat, _pvDstBuffer, _ui32Total, _gGamma );
	}

	/**
	 * Converts a 16-bit uint16_t such that 0 = 0.0f and 65535 (0xFFFF) = 1.0f.  For UNORM formats.
	 *
	 * \param _ui16Val The value to convert.
	 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
	 * \return Returns the converted value.
	 */
	LSE_INLINE float LSE_CALL CImageLib::Uint16ToUNorm( uint16_t _ui16Val, bool _bSrgb ) {
		return _bSrgb ? SRgbToLinear( _ui16Val / static_cast<float>(0xFFFF) ) : _ui16Val / static_cast<float>(0xFFFF);
	}

	/**
	 * Converts a 16-bit uint16_t such that 1 = -1.0f, 32768 = 0.0f, and 65535 (0xFFFF) = 1.0f.  For SNORM formats.
	 *
	 * \param _ui16Val The value to convert.
	 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
	 * \return Returns the converted value.
	 */
	LSE_INLINE float LSE_CALL CImageLib::Uint16ToSNorm( uint16_t _ui16Val, bool _bSrgb ) {
		const float fMaxS = 0xFFFF - 1;
		float fTmp = ((CStd::Clamp<uint16_t>( _ui16Val, 1, 0xFFFF ) - 1) / fMaxS) * 2.0f - 1.0f;
		return _bSrgb ? SRgbToLinear( fTmp ) : fTmp;
	}

	/**
	 * Converts a 16-bit int16_t.  For SNORM formats.
	 *
	 * \param _i16Val The value to convert.
	 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
	 * \return Returns the converted value.
	 */
	LSE_INLINE float LSE_CALL CImageLib::Int16ToSNorm( int16_t _i16Val, bool _bSrgb ) {
		return _bSrgb ? SRgbToLinear( CStd::Clamp( _i16Val / static_cast<float>(0x7FFF), -1.0f, 1.0f ) ) : CStd::Clamp( _i16Val / static_cast<float>(0x7FFF), -1.0f, 1.0f );
	}

	/**
	 * Converts an 8-bit uint8_t such that 0 = 0.0f and 255 (0xFF) = 1.0f.  For UNORM formats.
	 *
	 * \param _ui8Val The value to convert.
	 * \param _bSrgb If true, an sRGB -> linear conversion is applied.
	 * \return Returns the converted value.
	 */
	LSE_INLINE float LSE_CALL CImageLib::Uint8ToUNorm( uint8_t _ui8Val, bool _bSrgb ) {
		return _bSrgb ? SRgbToLinear( _ui8Val / static_cast<float>(0xFF) ) : _ui8Val / static_cast<float>(0xFF);
	}

	/**
	 * Converts a single component from one integer form to another.
	 *
	 * \param _ui32Src The source component.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 * \return Returns the normalized converted component.
	 */
	template <unsigned _tSrcBits, unsigned _tDstBits, unsigned _tTreat0BitsAsFF>
	LSE_INLINE uint32_t LSE_FCALL CImageLib::ConvComp( uint32_t _ui32Src, LSI_GAMMA _gGamma ) {
		// If the destination bits are 0, return 0.
		if ( _tDstBits == 0 ) { return 0; }
		// If the source bits are 0, return either 0 or the highest value possible
		//	for the destination.
		if ( _tSrcBits == 0 ) {
			if ( _tTreat0BitsAsFF ) {
				return (1 << _tDstBits) - 1;
			}
			return 0;
		}
		const double dMaxSrc = (1 << _tSrcBits) - 1.0;
		const double dMaxDst = (1 << _tDstBits) - 1.0;
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				double dVal = _ui32Src / dMaxSrc;
				return static_cast<uint32_t>(std::round( LinearToSRgb( dVal ) * dMaxDst ));
			}
			case LSI_G_LINEAR : {
				double dVal = _ui32Src / dMaxSrc;
				return static_cast<uint32_t>(std::round( SRgbToLinear( dVal ) * dMaxDst ));
			}
			default : {
				// Because this is a template function, if statements will be fully removed when compiled since
				//	they will all resolve to constant expressions.

				// If the component bits are the same, no conversion takes place.
				if ( _tSrcBits == _tDstBits ) { return _ui32Src; }
				
				// The below implementation causes a slight loss of accuracy when shifting down, plus attempts to save time by skipping std::round(),
				//	which could cause some values to be truncated.
				// Just do a full conversion, emphasis on accuracy.
				double dVal = _ui32Src / dMaxSrc;
				return static_cast<uint32_t>(std::round( dVal * dMaxDst ));
#if 0
				// Both the source and the destination have some bits in them and they are not the same number of
				//	bits.  One will be higher than the other and this changes which shift we have to use.
				if ( _tSrcBits > _tDstBits ) {
					// Shifting down (>>).
					return _ui32Src >> (_tSrcBits - _tDstBits);
				}
				// Destination bits are greater than the source bits.  Normalization is necessary.
				// That - (1 - !_tSrcBits) at the end should be just - 1, but the compiler barfs if _tSrcBits is 0 because it tries
				//	to divide by zero during compile-time.  During run-time, this can never happen, since the function will never get
				//	here if _tSrcBits is 0, but the compiler still tries to resolve the expression below to a constant and it will
				//	divide by zero in doing so.
				// "- (1 - !_tSrcBits)" is necessary to avoid this.
				return static_cast<uint32_t>(_ui32Src * static_cast<double>(((1 << _tDstBits) - 1) / static_cast<double>((1 << _tSrcBits) - (1 - !_tSrcBits))));
#endif	// #if 0
			}
		}
	}

}	// namespace lsi

#pragma warning( pop )

#endif	// __LSI_IMAGELIB_H__
