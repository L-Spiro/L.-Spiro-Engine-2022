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
 * Description: A texture from the FBX file.
 */

#include "LSXTexture.h"
#include "../Misc/LSXMisc.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsx {

	/** Texture ID counter. */
	CAtomic::ATOM CTexture::m_aTexId = 0;

	// == Various constructors.
	LSE_CALLCTOR CTexture::CTexture() :
		m_ui32Id( CAtomic::InterlockedIncrement( m_aTexId ) ),
		m_pcPath( NULL ),
		m_ui16Flags( 0 ) {
	}
	LSE_CALLCTOR CTexture::~CTexture() {
	}

	// == Functions.
	/**
	 * Creates the texture given a path and some flags.
	 *
	 * \param _pcPath Path to the file to load.
	 * \param _ui16Flags Flags.
	 * \return Returns an error code indicating whether there was a memory failure, path-not-found, or
	 *	success.
	 */
	LSSTD_ERRORS LSE_CALL CTexture::CreateFromFile( const LSCHAR * _pcPath, LSUINT16 _ui16Flags ) {
		if ( !CFileLib::Exists( _pcPath ) ) {
			return LSSTD_E_FILENOTFOUND;
		}
		if ( !m_iImage.LoadFile( _pcPath ) ) { return LSSTD_E_OUTOFMEMORY; }
		if ( _ui16Flags & LSX_TEX_sRGB ) { m_iImage.SetColorSpace( LSI_G_sRGB ); }
		else { m_iImage.SetColorSpace( LSI_G_LINEAR ); }
		m_ui16Flags = _ui16Flags;
		m_pcPath = _pcPath;
		if ( !m_iImage.GetWidth() || !m_iImage.GetHeight() ) {
			::printf( "Texture \"%s\" has 0 width or height.\r\n", m_pcPath );
		}
		return LSSTD_E_SUCCESS;
	}

	/**
	 * Creates the texture from another texture with the specified flags.
	 *
	 * \param _tTexture The source texture to copy.
	 * \param _ui16Flags The flags to override in the new copy.
	 * \return Returns an error code indicating whether there was a memory failure or success.
	 */
	LSSTD_ERRORS LSE_CALL CTexture::CreateFromTexture( const CTexture &_tTexture, LSUINT16 _ui16Flags ) {
		try {
			m_iImage = _tTexture.m_iImage;
		}
		catch ( ... ) {
			return LSSTD_E_OUTOFMEMORY;
		}
		if ( _ui16Flags & LSX_TEX_sRGB ) { m_iImage.SetColorSpace( LSI_G_sRGB ); }
		else { m_iImage.SetColorSpace( LSI_G_LINEAR ); }
		m_ui16Flags = _ui16Flags;
		m_pcPath = _tTexture.Path();

		if ( !m_iImage.GetWidth() || !m_iImage.GetHeight() ) {
			::printf( "Texture \"%s\" has 0 width or height.\r\n", m_pcPath );
		}
		return LSSTD_E_SUCCESS;
	}

	/**
	 * Save the texture to a binary buffer.
	 *
	 * \param _bBuffer The buffer where the data should be written.
	 * \return Returns true if there is enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CTexture::SaveTexture( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		lsc::CMisc::LSC_BUFFER bTemp;
		if ( !m_iImage.CompressToBuffer( bTemp, true ) ) { return false; }

		if ( !CMisc::WriteUInt( bTemp.Length(), _bBuffer ) ) { return false; }
		if ( !CMisc::WriteUInt( Id(), _bBuffer ) ) { return false; }
		if ( !CMisc::WriteUInt( m_iImage.GetCrc32(), _bBuffer ) ) { return false; }
		if ( !CMisc::WriteUInt( m_iImage.GetCrc32Alt(), _bBuffer ) ) { return false; }
		if ( bTemp.Length() ) {
			if ( !CMisc::WriteBytes( &bTemp[0], bTemp.Length(), _bBuffer ) ) { return false; }
		}
		// Once this is done, we no longer need the image.  Free its memory.
		m_iImage.Reset();
		return true;
	}

}	// namespace lsx

#pragma warning( pop )
