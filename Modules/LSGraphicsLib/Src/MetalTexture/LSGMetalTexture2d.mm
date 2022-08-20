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
 * Description: A 2D Metal texture.
 */

#include "LSGMetalTexture2d.h"

#ifdef LSG_METAL

#include "../Gfx/LSGGfx.h"
#include "../Metal/LSGMetalObject.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CMetalTexture2d::CMetalTexture2d() :
		m_pvTexture( NULL ) {
	}
	LSE_CALLCTOR CMetalTexture2d::~CMetalTexture2d() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets everything to scratch.
	 */
	LSVOID LSE_CALL CMetalTexture2d::Reset() {
		if ( m_pvTexture ) {
			CFBridgingRelease( m_pvTexture );
			m_pvTexture = NULL;
		}

		Parent::Reset();
	}

	/**
	 * Creates a OpenGL texture and fills it with our texel data.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL CMetalTexture2d::CreateApiTexture( const CImage * _piImages, LSUINT32 _ui32Total ) {
		MTLPixelFormat pfFormat = static_cast<MTLPixelFormat>(CMetalTextureConstants::GetMetalInternalFormatStandard( _piImages->GetFormat(), _piImages->IsSRgb() ));
		if ( pfFormat == MTLPixelFormatInvalid ) { return false; }
		MTLResourceOptions roOptions = (m_ui32Usage & LSG_TUT_SETONLY) ? MTLResourceOptionCPUCacheModeDefault : MTLResourceOptionCPUCacheModeWriteCombined;
		
		MTLTextureDescriptor * ptTex = [MTLTextureDescriptor new];
		ptTex.textureType = _ui32Total > 1 ? MTLTextureType2DArray : MTLTextureType2D;
		ptTex.width = _piImages->GetWidth();
		ptTex.height = _piImages->GetHeight();
		ptTex.mipmapLevelCount = _piImages->TotalMipLevels();
		ptTex.sampleCount = m_ui16MultiSampleCount;
		ptTex.arrayLength = _ui32Total;
		ptTex.resourceOptions = roOptions;
		
		if ( m_ui16MultiSampleCount > 1 ) {
			ptTex.textureType = MTLTextureType2DMultisample;
			if ( ptTex.arrayLength != 1 ) {
				return false;
			}
		}


		id <MTLTexture> tTexture = [GetMetalDevice() newTextureWithDescriptor:ptTex];
		if ( tTexture == nil ) { return false; }
		m_pvTexture = CFBridgingRetain( tTexture );

		if ( !GenerateMipMaps( _piImages, _ui32Total )) {
			return false;
		}
		
		return true;
	}

	/**
	 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
	 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
	 *	It acts on the currently bound texture.
	 *
	 * \param _piImages The source image(s).
	 * \param _ui32Total Total images to which _piImages points.
	 * \return Returns true if there were no failures.
	 */
	LSBOOL LSE_CALL CMetalTexture2d::GenerateMipMaps( const CImage * _piImages, LSUINT32 _ui32Total ) {
		for ( LSUINT32 S = 0; S < _ui32Total; ++S ) {
			LSUINT32 ui32Width = _piImages[S].GetWidth();
			LSUINT32 ui32Height = _piImages[S].GetHeight();
			for ( LSUINT32 M = 0; M < _piImages[S].TotalMipLevels(); ++M ) {
				if ( !GenerateMipMaps( M, _piImages[S].GetMipMapBuffers( M ),
					_piImages[S].GetFormat(), ui32Width, ui32Height, S ) ) { return false; }

				ui32Width = CStd::Max<LSUINT32>( 1, ui32Width >> 1 );
				ui32Height = CStd::Max<LSUINT32>( 1, ui32Height >> 1 );
			}
		}
		return true;
	}

	/**
	 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
	 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
	 *	It acts on the currently bound texture.
	 *
	 * \param _iMipLevel The surface level to which to copy our texel data.
	 * \param _tbTexels The texel data to copy.
	 * \param _pfFormat Format of the data to copy.
	 * \param _ui32Width Width of the image data.
	 * \param _ui32Height Height of the image data.
	 * \param _ui32Slice Array slice to which to copy our texel data.
	 * \return Returns true if there were no failures.
	 */
	LSBOOL LSE_CALL CMetalTexture2d::GenerateMipMaps( LSUINT32 _iMipLevel,
		const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
		LSUINT32 _ui32Slice ) {
		
		const LSUINT8 * pui8Src = &_tbTexels[0];
		CImageLib::CTexelBuffer tbTemp;
		const CImageLib::CTexelBuffer * _ptbUseMe = &_tbTexels;
		
		LSUINT32 ui32SrcRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
		if ( _pfFormat == LSI_PF_R8G8B8 ) {
			tbTemp = _tbTexels;
			_ptbUseMe = &tbTemp;
			pui8Src = &tbTemp[0];
			LSUINT8 * pui8Dest = reinterpret_cast<LSUINT8 *>(&tbTemp[0]);
			
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				LSUINT32 ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( LSUINT32 X = _ui32Width; X--; ) {
					LSUINT32 ui32SurfIndex = ui32YPitchOff + X * 3 /* Texel size. */;

					LSUINT8 * pui8This = reinterpret_cast<LSUINT8 *>(&pui8Dest[ui32SurfIndex]);
					CStd::Swap( pui8This[0], pui8This[2] );
				}
			}
		}
		else if ( _pfFormat == LSI_PF_R8G8B8A8 ) {
			tbTemp = _tbTexels;
			_ptbUseMe = &tbTemp;
			pui8Src = &tbTemp[0];
			LSUINT8 * pui8Dest = reinterpret_cast<LSUINT8 *>(&tbTemp[0]);
			
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				LSUINT32 ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( LSUINT32 X = _ui32Width; X--; ) {
					LSUINT32 ui32SurfIndex = ui32YPitchOff + X * 4 /* Texel size. */;

					LSUINT32 * pui32This = reinterpret_cast<LSUINT32 *>(&pui8Dest[ui32SurfIndex]);
					(*pui32This) = CStd::ByteSwap32( (*pui32This) );
				}
			}
		}
		else if ( _pfFormat == LSI_PF_R16G16B16A16F ) {
			tbTemp = _tbTexels;
			_ptbUseMe = &tbTemp;
			pui8Src = &tbTemp[0];
			LSUINT8 * pui8Dest = reinterpret_cast<LSUINT8 *>(&tbTemp[0]);
			
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				LSUINT32 ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( LSUINT32 X = _ui32Width; X--; ) {
					LSUINT32 ui32SurfIndex = ui32YPitchOff + X * 8 /* Texel size. */;

					LSUINT16 * pui16This = reinterpret_cast<LSUINT16 *>(&pui8Dest[ui32SurfIndex]);
					CStd::Swap( pui16This[0], pui16This[3] );
					CStd::Swap( pui16This[1], pui16This[2] );
				}
			}
		}
		else if ( _pfFormat == LSI_PF_R32G32B32A32F ) {
			tbTemp = _tbTexels;
			_ptbUseMe = &tbTemp;
			pui8Src = &tbTemp[0];
			LSUINT8 * pui8Dest = reinterpret_cast<LSUINT8 *>(&tbTemp[0]);
			
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				LSUINT32 ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( LSUINT32 X = _ui32Width; X--; ) {
					LSUINT32 ui32SurfIndex = ui32YPitchOff + X * 16 /* Texel size. */;

					LSFLOAT * pfThis = reinterpret_cast<LSFLOAT *>(&pui8Dest[ui32SurfIndex]);
					CStd::Swap( pfThis[0], pfThis[3] );
					CStd::Swap( pfThis[1], pfThis[2] );
				}
			}
		}
		
		CopyTexels( (*_ptbUseMe), _pfFormat,
			_iMipLevel, _ui32Slice, _ui32Width, _ui32Height );

		return true;
	}
	
	/**
	 * Gets a texel from a buffer given the row width, X, Y, format, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _pfFormat Format of the texels in _tbTexels.
	 * \param _ui32Mip Mipmap level.
	 * \param _ui32Slice Target array slice.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 */
	LSVOID LSE_CALL CMetalTexture2d::CopyTexels( const CImageLib::CTexelBuffer &_tbTexels,
		LSI_PIXEL_FORMAT _pfFormat,
		LSUINT32 _ui32Mip, LSUINT32 _ui32Slice,
		LSUINT32 _ui32Width, LSUINT32 _ui32Height ) {
		id <MTLTexture> tTexture = (__bridge id <MTLTexture>)m_pvTexture;
		MTLRegion rReg = ::MTLRegionMake2D( 0, 0, _ui32Width, _ui32Height );
		const LSUINT8 * pui8UseMe = &_tbTexels[0];
		LSUINT32 ui32RowSize = CImageLib::GetRowSize( _pfFormat, _ui32Width );
		[tTexture replaceRegion:rReg
			mipmapLevel:_ui32Mip
			slice:_ui32Slice
			withBytes:pui8UseMe
			bytesPerRow:ui32RowSize
			bytesPerImage:ui32RowSize * _ui32Height];
	}
	
	// == Functions.
	/**
	 * Given an image format, return the Metal internal format for standard textures.
	 *
	 * \param _pfFormat Format to convert.
	 * \param _bsRgb If true, an sRGB format is selected, if possible.
	 * \return Returns the Metal internal format to use with the given image-library format.
	 */
	LSINT32 LSE_CALL CMetalTextureConstants::GetMetalInternalFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb ) {
		static const LSINT32 i32Table[] = {
			MTLPixelFormatA8Unorm,								// LSI_PF_A8
			MTLPixelFormatInvalid,								// LSI_PF_R3G3B2
			MTLPixelFormatB5G6R5Unorm,							// LSI_PF_R5G6B5
			MTLPixelFormatABGR4Unorm,							// LSI_PF_R4G4B4A4
			MTLPixelFormatA1BGR5Unorm,							// LSI_PF_R5G5B5A1
			MTLPixelFormatInvalid,								// LSI_PF_R8G8B8
			MTLPixelFormatRGBA8Unorm,							// LSI_PF_R8G8B8A8
			MTLPixelFormatRGBA16Unorm,							// LSI_PF_R16G16B16A16
			MTLPixelFormatRGBA16Float,							// LSI_PF_R16G16B16A16F
			MTLPixelFormatRGBA32Float,							// LSI_PF_R32G32B32A32F
		};
		static const LSINT32 i32TableSrgb[] = {
			MTLPixelFormatInvalid,								// LSI_PF_A8
			MTLPixelFormatInvalid,								// LSI_PF_R3G3B2
			MTLPixelFormatInvalid,								// LSI_PF_R5G6B5
			MTLPixelFormatInvalid,								// LSI_PF_R4G4B4A4
			MTLPixelFormatInvalid,								// LSI_PF_R5G5B5A1
			MTLPixelFormatInvalid,								// LSI_PF_R8G8B8
			MTLPixelFormatRGBA8Unorm_sRGB,						// LSI_PF_R8G8B8A8
			MTLPixelFormatInvalid,								// LSI_PF_R16G16B16A16
			MTLPixelFormatInvalid,								// LSI_PF_R16G16B16A16F
			MTLPixelFormatInvalid,								// LSI_PF_R32G32B32A32F
		};
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) {
			return _bsRgb ? i32TableSrgb[_pfFormat] : i32Table[_pfFormat];
		}
		return MTLPixelFormatInvalid;
	}

}	// namespace lsg

#endif	// #ifdef LSG_METAL
