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
 * Description: A 2D OpenGL texture.
 */

#include "LSGOpenGlTexture2d.h"

#ifdef LSG_OGL

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlTexture2d::COpenGlTexture2d() :
		m_uiTexture( 0 ),
		m_sArrayDepth( 0 ),
		m_eTextureType( GL_TEXTURE_2D ),
		m_eInternalFormatKtx1( GL_INVALID_ENUM ),
		m_eBaseInternalFormatKtx1( GL_INVALID_ENUM ),
		m_eFormatKtx1( GL_INVALID_ENUM ),
		m_eTypeKtx1( GL_INVALID_ENUM ) {
	}
	LSE_CALLCTOR COpenGlTexture2d::~COpenGlTexture2d() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL COpenGlTexture2d::Reset() {
		COpenGl::DeleteTexture( m_uiTexture );
		m_sArrayDepth = 0;
		m_eTextureType = GL_TEXTURE_2D;

		m_eInternalFormatKtx1 = GL_INVALID_ENUM;
		m_eBaseInternalFormatKtx1 = GL_INVALID_ENUM;
		m_eFormatKtx1 = GL_INVALID_ENUM;
		m_eTypeKtx1 = GL_INVALID_ENUM;

		Parent::Reset();
	}

	/**
	 * Creates a OpenGL texture and fills it with our texel data.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL COpenGlTexture2d::CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total ) {
		if ( _ui32Total != 1 ) {
			if ( !COpenGl::glTexStorage3D || !COpenGl::glTexSubImage3D ) {
				return false;
			}
		}
		// We need the error flag.
		CCriticalSection::CLocker lGetError( COpenGl::m_csCrit );
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.

		if ( _piImages->GetFormat() == LSI_PF_KTX1 ) {
			m_eInternalFormatKtx1 = static_cast<GLenum>(_piImages->GetKtx1Values().kifGlInternalFormat);
			m_eBaseInternalFormatKtx1 = static_cast<GLenum>(_piImages->GetKtx1Values().kbifGlInternalBaseFormat);
			m_eFormatKtx1 = static_cast<GLenum>(_piImages->GetKtx1Values().kfGlFormat);
			m_eTypeKtx1 = static_cast<GLenum>(_piImages->GetKtx1Values().ktGlType);
		}

		m_uiTexture = COpenGl::GenTexture();
		if ( _ui32Total == 1 ) {
			COpenGl::CTempTexture2dBinder tt2bBinder( m_uiTexture );
			if ( !GenerateMipMaps( _piImages, _ui32Total ) ) { return false; }
		}
		else {
			COpenGl::CTempTexture2dArrayBinder tt2abBinder( m_uiTexture );
			if ( !GenerateMipMaps( _piImages, _ui32Total ) ) { return false; }
		}

		m_sArrayDepth = static_cast<GLsizei>(_ui32Total);
		m_eTextureType = _ui32Total == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY;

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
	LSBOOL LSE_CALL COpenGlTexture2d::GenerateMipMaps( const CImage * _piImages, uint32_t _ui32Total ) {
		if ( _ui32Total > 1 ) {
			COpenGl::glTexStorage3D( GL_TEXTURE_2D_ARRAY, _ui32Total,
				COpenGlTextureConstants::GetOpenGlInternalFormatStandard( _piImages[0].GetFormat(), _piImages[0].IsSRgb() ),
				_piImages[0].GetWidth(),
				_piImages[0].GetHeight(), _ui32Total );
			for ( uint32_t I = 0; I < _ui32Total; ++I ) {
				uint32_t ui32Width = _piImages[I].GetWidth();
				uint32_t ui32Height = _piImages[I].GetHeight();
				for ( uint32_t M = 0; M < _piImages[I].TotalMipLevels(); ++M ) {
					if ( !GenerateMipMaps( M, I, _ui32Total,
						_piImages[I].GetMipMapBuffers( M ),
						_piImages[I].GetFormat(), ui32Width, ui32Height ) ) { return false; }

					ui32Width = CStd::Max<uint32_t>( 1, ui32Width >> 1 );
					ui32Height = CStd::Max<uint32_t>( 1, ui32Height >> 1 );
				}
			}
		}
		else {
			uint32_t ui32Width = _piImages[0].GetWidth();
			uint32_t ui32Height = _piImages[0].GetHeight();
			for ( uint32_t M = 0; M < _piImages[0].TotalMipLevels(); ++M ) {
				if ( !GenerateMipMaps( M, _piImages[0].GetMipMapBuffers( M ),
					_piImages[0].GetFormat(), ui32Width, ui32Height, _piImages[0].IsSRgb() ) ) { return false; }

				ui32Width = CStd::Max<uint32_t>( 1, ui32Width >> 1 );
				ui32Height = CStd::Max<uint32_t>( 1, ui32Height >> 1 );
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
	 * \param _bsRgb Specifies if the image is sRGB or not.
	 * \return Returns true if there were no failures.
	 */
	LSBOOL LSE_CALL COpenGlTexture2d::GenerateMipMaps( GLint _iMipLevel,
		const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height, LSBOOL _bsRgb ) {
		if ( _pfFormat >= LSI_PF_DXT1 && _pfFormat <= LSI_PF_BC7_SRGB ) {
			uint32_t ui32CompSize = CDds::GetCompressedSize( _ui32Width, _ui32Height, CDds::DxtBlockSize( _pfFormat ) );
			COpenGl::glCompressedTexImage2D( GL_TEXTURE_2D, _iMipLevel, COpenGlTextureConstants::GetOpenGlInternalFormatStandard( _pfFormat, _bsRgb ),
				_ui32Width, _ui32Height, 0,
				ui32CompSize,
				&_tbTexels[0] );

			return glWarnErrorAlways( "Error uploading DXT texture data" ) == false;
		}
		if ( _pfFormat == LSI_PF_KTX1 ) {
			const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdData = CKtx::FindInternalFormatData( static_cast<LSI_KTX_INTERNAL_FORMAT>(m_eInternalFormatKtx1) );
			if ( !pkifdData ) { return false; }
			if ( COpenGl::glGetInternalformativ ) {
				GLint iParam = 0;
				COpenGl::glGetInternalformativ( GL_TEXTURE_2D, m_eInternalFormatKtx1, GL_INTERNALFORMAT_SUPPORTED, 1, &iParam );
				if ( !iParam ) {
					char szBuffer[64];
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "Internal format unsupported: %.4X.\r\n", m_eInternalFormatKtx1 );
					CStd::DebugPrintA( szBuffer );
				}
			}

			if ( pkifdData->bCompressed ) {
				uint32_t ui32CompSize = pkifdData->pfCompSizeFunc ? pkifdData->pfCompSizeFunc( _ui32Width, _ui32Height, ((pkifdData->ui32BlockSizeInBits + 7) / 8) ) :
					CKtx::GetCompressedSize( _ui32Width, _ui32Height, ((pkifdData->ui32BlockSizeInBits + 7) / 8) );
				COpenGl::glCompressedTexImage2D( GL_TEXTURE_2D, _iMipLevel, m_eInternalFormatKtx1,
					_ui32Width, _ui32Height, 0,
					ui32CompSize,
					&_tbTexels[0] );
				return glWarnErrorAlways( "Error uploading compressed texture data" ) == false;
			}
			else {
				
				::glTexImage2D( GL_TEXTURE_2D, _iMipLevel, m_eInternalFormatKtx1,
					_ui32Width, _ui32Height, 0, m_eFormatKtx1,
					m_eTypeKtx1, &_tbTexels[0] );
				return glWarnErrorAlways( "Error uploading texture data" ) == false;
			}
		}
		const uint8_t * pui8UseMe = &_tbTexels[0];
		CImageLib::CTexelBuffer tbTemp;
		if ( _pfFormat == LSI_PF_R16G16B16A16 ) {
			tbTemp = _tbTexels;
			uint32_t ui32RowSize = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			uint32_t ui32SrcTexelSize = CImageLib::GetFormatSize( _pfFormat );
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					uint64_t ui64This = (*reinterpret_cast<uint64_t *>(&tbTemp[Y*ui32RowSize+X*ui32SrcTexelSize]));
					(*reinterpret_cast<uint64_t *>(&tbTemp[Y*ui32RowSize+X*ui32SrcTexelSize])) = CStd::ByteSwap64( ui64This );
				}
			}
			pui8UseMe = &tbTemp[0];
		}
		else if ( _pfFormat == LSI_PF_R8G8B8 ) {
			// Handling padding.
			tbTemp.Resize( CImageLib::GetBufferSizeWithPadding( LSI_PF_R8G8B8A8, _ui32Width, _ui32Height ) );
			uint32_t ui32SrcRowSize = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			uint32_t ui32SrcTexelSize = CImageLib::GetFormatSize( _pfFormat );

			uint32_t ui32DstRowSize = CImageLib::GetRowSize( LSI_PF_R8G8B8A8, _ui32Width );
			uint32_t ui32DstTexelSize = CImageLib::GetFormatSize( LSI_PF_R8G8B8A8 );
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					uint32_t ui32This = (*reinterpret_cast<const uint32_t *>(&_tbTexels[Y*ui32SrcRowSize+X*ui32SrcTexelSize]));
					uint32_t ui32R = (ui32This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_R )) & 0xFF;
					uint32_t ui32G = (ui32This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_G )) & 0xFF;
					uint32_t ui32B = (ui32This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_B )) & 0xFF;
					ui32This = (ui32R << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |
						(ui32G << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |
						(ui32B << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |
						(0xFF << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ));
					(*reinterpret_cast<uint32_t *>(&tbTemp[Y*ui32DstRowSize+X*ui32DstTexelSize])) = ui32This;
				}
			}
			pui8UseMe = &tbTemp[0];
			_pfFormat = LSI_PF_R8G8B8A8;
		}
		::glTexImage2D( GL_TEXTURE_2D, _iMipLevel, COpenGlTextureConstants::GetOpenGlInternalFormatStandard( _pfFormat, _bsRgb ),
			_ui32Width, _ui32Height, 0, COpenGlTextureConstants::GetOpenGlFormatStandard( _pfFormat ),
			COpenGlTextureConstants::GetOpenGlTypeStandard( _pfFormat ), pui8UseMe );
		return glWarnErrorAlways( "Error uploading texture data" ) == false;
	}

	/**
	 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
	 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
	 *	It acts on the currently bound texture.
	 *
	 * \param _iMipLevel The surface level to which to copy our texel data.
	 * \param _iLayer The layer to which to copy our texel data.
	 * \param _sTotalLayers The total number of layers in the image.
	 * \param _tbTexels The texel data to copy.
	 * \param _pfFormat Format of the data to copy.
	 * \param _ui32Width Width of the image data.
	 * \param _ui32Height Height of the image data.
	 * \return Returns true if there were no failures.
	 */
	LSBOOL LSE_CALL COpenGlTexture2d::GenerateMipMaps( GLint _iMipLevel, GLint _iLayer, GLsizei _sTotalLayers,
		const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height ) {
		COpenGl::glTexSubImage3D( GL_TEXTURE_2D_ARRAY, _iMipLevel, 0, 0,
			_iLayer,
			_ui32Width, _ui32Height,
			_sTotalLayers,
			COpenGlTextureConstants::GetOpenGlFormatStandard( _pfFormat ),
			COpenGlTextureConstants::GetOpenGlTypeStandard( _pfFormat ),
			&_tbTexels[0] );
		return glWarnErrorAlways( "Error uploading texture-array data" ) == false;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGL
