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
 * Description: A 2D OpenGL ES texture.
 */

#include "LSGOpenGlEsTexture2d.h"

#ifdef LSG_OGLES

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsTexture2d::COpenGlEsTexture2d() :
		m_uiTexture( 0 ) {
	}
	LSE_CALLCTOR COpenGlEsTexture2d::~COpenGlEsTexture2d() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL COpenGlEsTexture2d::Reset() {
		COpenGlEs::DeleteTexture( m_uiTexture );

		Parent::Reset();
	}

	/**
	 * Creates a OpenGL texture and fills it with our texel data.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL COpenGlEsTexture2d::CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total ) {
		// We need the error flag.
		CCriticalSection::CLocker lGetError( COpenGlEs::m_csCrit );
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.

		const CImage * piUseMe = _piImages;
		CImage iTemp;
		
		uint32_t ui32NewW = _piImages->GetWidth();
		uint32_t ui32NewH = _piImages->GetHeight();
		
		if ( !CGfx::GetMetrics().bNonPo2 ) {
			ui32NewW = CStd::GetLowestPo2( ui32NewW );
			ui32NewH = CStd::GetLowestPo2( ui32NewH );
		}
		
		ui32NewW = CStd::Min<uint32_t>( ui32NewW, CGfx::GetMetrics().ui32MaxTextureWidth );
		ui32NewH = CStd::Min<uint32_t>( ui32NewH, CGfx::GetMetrics().ui32MaxTextureHeight );
		
		if ( ui32NewW != piUseMe->GetWidth() || ui32NewH != piUseMe->GetHeight() ) {
			// Resampling is required.
			LSI_FILTER fFilter = (ui32NewW >= piUseMe->GetWidth() || ui32NewH >= piUseMe->GetHeight()) ?
				LSI_F_BILINEAR_FILTER : LSI_F_KAISER_FILTER;
			if ( !piUseMe->Resample( ui32NewW, ui32NewH, fFilter, iTemp, piUseMe->IsSRgb() ? -2.2f : 1.0f ) ) {
				return false;
			}
			piUseMe = &iTemp;
		}
		

		m_uiTexture = COpenGlEs::GenTexture();
		COpenGlEs::CTempTexture2dBinder tt2bBinder( m_uiTexture );
		if ( !GenerateMipMaps( piUseMe, _ui32Total ) ) { return false; }

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
	LSBOOL LSE_CALL COpenGlEsTexture2d::GenerateMipMaps( const CImage * _piImages, uint32_t _ui32Total ) {
		uint32_t ui32Width = _piImages[0].GetWidth();
		uint32_t ui32Height = _piImages[0].GetHeight();
		for ( uint32_t M = 0; M < _piImages[0].TotalMipLevels(); ++M ) {
			if ( !GenerateMipMaps( M, _piImages[0].GetMipMapBuffers( M ),
				_piImages[0].GetFormat(), ui32Width, ui32Height, _piImages[0].IsSRgb() ) ) { return false; }

			ui32Width = CStd::Max<uint32_t>( 1, ui32Width >> 1 );
			ui32Height = CStd::Max<uint32_t>( 1, ui32Height >> 1 );
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
	 * \param _bsRgb Specifies if the image is sRGB or not.
	 * \return Returns true if there were no failures.
	 */
	LSBOOL LSE_CALL COpenGlEsTexture2d::GenerateMipMaps( GLint _iMipLevel,
		const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height, LSBOOL _bsRgb ) {
		
		const uint8_t * pui8Src = &_tbTexels[0];
		CImageLib::CTexelBuffer tbTemp;
		
		uint32_t ui32SrcRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
		if ( _pfFormat == LSI_PF_R8G8B8 ) {
			tbTemp = _tbTexels;
			pui8Src = &tbTemp[0];
			uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(&tbTemp[0]);
			
			for ( uint32_t Y = _ui32Height; Y--; ) {
				uint32_t ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( uint32_t X = _ui32Width; X--; ) {
					uint32_t ui32SurfIndex = ui32YPitchOff + X * 3 /* Texel size. */;

					uint8_t * pui8This = reinterpret_cast<uint8_t *>(&pui8Dest[ui32SurfIndex]);
					CStd::Swap( pui8This[0], pui8This[2] );
				}
			}
		}
		else if ( _pfFormat == LSI_PF_R8G8B8A8 ) {
			tbTemp = _tbTexels;
			pui8Src = &tbTemp[0];
			uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(&tbTemp[0]);
			
			for ( uint32_t Y = _ui32Height; Y--; ) {
				uint32_t ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( uint32_t X = _ui32Width; X--; ) {
					uint32_t ui32SurfIndex = ui32YPitchOff + X * 4 /* Texel size. */;

					uint32_t * pui32This = reinterpret_cast<uint32_t *>(&pui8Dest[ui32SurfIndex]);
					(*pui32This) = CStd::ByteSwap32( (*pui32This) );
				}
			}
		}
		else if ( _pfFormat == LSI_PF_R16G16B16A16F ) {
			tbTemp = _tbTexels;
			pui8Src = &tbTemp[0];
			uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(&tbTemp[0]);
			
			for ( uint32_t Y = _ui32Height; Y--; ) {
				uint32_t ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( uint32_t X = _ui32Width; X--; ) {
					uint32_t ui32SurfIndex = ui32YPitchOff + X * 8 /* Texel size. */;

					uint16_t * pui16This = reinterpret_cast<uint16_t *>(&pui8Dest[ui32SurfIndex]);
					CStd::Swap( pui16This[0], pui16This[3] );
					CStd::Swap( pui16This[1], pui16This[2] );
				}
			}
		}
		else if ( _pfFormat == LSI_PF_R32G32B32A32F ) {
			tbTemp = _tbTexels;
			pui8Src = &tbTemp[0];
			uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(&tbTemp[0]);
			
			for ( uint32_t Y = _ui32Height; Y--; ) {
				uint32_t ui32YPitchOff = Y * ui32SrcRowWidth;

				for ( uint32_t X = _ui32Width; X--; ) {
					uint32_t ui32SurfIndex = ui32YPitchOff + X * 16 /* Texel size. */;

					float * pfThis = reinterpret_cast<float *>(&pui8Dest[ui32SurfIndex]);
					CStd::Swap( pfThis[0], pfThis[3] );
					CStd::Swap( pfThis[1], pfThis[2] );
				}
			}
		}
		::glTexImage2D( GL_TEXTURE_2D, _iMipLevel, COpenGlEsTextureConstants::GetOpenGlInternalFormatStandard( _pfFormat, _bsRgb ),
			_ui32Width, _ui32Height, 0, COpenGlEsTextureConstants::GetOpenGlFormatStandard( _pfFormat, _bsRgb ),
			COpenGlEsTextureConstants::GetOpenGlTypeStandard( _pfFormat ), pui8Src );
		return glWarnErrorAlways( "Error uploading texture data" ) == false;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES
