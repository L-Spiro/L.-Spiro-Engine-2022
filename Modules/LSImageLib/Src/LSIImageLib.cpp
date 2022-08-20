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

#include "LSIImageLib.h"
#include "FreeImage.h"
#include "Dds/LSIDds.h"


namespace lsi {

	// == Functions.
	/**
	 * Initializes the image library.
	 */
	void LSE_CALL CImageLib::InitImageLibrary() {
		::FreeImage_Initialise( true );
	}

	/**
	 * Shuts down the image library.
	 */
	void LSE_CALL CImageLib::ShutDownImageLibrary() {
		/*HMODULE hMod = ::GetModuleHandleA( "PVRTexLib.dll" );
		while ( ::FreeLibrary( hMod ) ) {}*/
		::FreeImage_DeInitialise();
	}

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
	uint32_t LSE_FCALL CImageLib::GetBufferSizeWithPadding( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height ) {
		if ( IsCompressed( _pfFormat ) ) {
			return CDds::GetCompressedSize( _ui32Width, _ui32Height, CDds::DxtBlockSize( _pfFormat ) );
		}
		return GetRowSize( _pfFormat, _ui32Width ) * _ui32Height + 8;
	}

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
	uint64_t LSE_FCALL CImageLib::IntegerTexelToIntegerTexel( LSI_PIXEL_FORMAT _pfSrcFormat, LSI_PIXEL_FORMAT _pfDstFormat, uint64_t _ui64Color, LSI_GAMMA _gGamma ) {
		uint64_t ui64R = ConvComponent( _pfSrcFormat, _pfDstFormat, LSI_PC_R,
			static_cast<uint32_t>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_R )) & ((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_R )) - 1ULL)), _gGamma );
		uint64_t ui64G = ConvComponent( _pfSrcFormat, _pfDstFormat, LSI_PC_G,
			static_cast<uint32_t>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_G )) & ((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_G )) - 1ULL)), _gGamma );
		uint64_t ui64B = ConvComponent( _pfSrcFormat, _pfDstFormat, LSI_PC_B,
			static_cast<uint32_t>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_B )) & ((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_B )) - 1ULL)), _gGamma );
		uint64_t ui64A = ConvComponent( _pfSrcFormat, _pfDstFormat, LSI_PC_A,
			static_cast<uint32_t>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_A )) & ((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_A )) - 1ULL)), LSI_G_NONE );
		return (ui64R << GetComponentOffset( _pfDstFormat, LSI_PC_R )) |
			(ui64G << GetComponentOffset( _pfDstFormat, LSI_PC_G )) |
			(ui64B << GetComponentOffset( _pfDstFormat, LSI_PC_B )) |
			(ui64A << GetComponentOffset( _pfDstFormat, LSI_PC_A ));
	}

	/**
	 * Converts any integer format to a 128-bit floating-point format with 32 bits in each component.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _ui64Color The texel to convert.
	 * \param _fReturn Holds the converted color value.  Must be a 4-element array.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	void LSE_FCALL CImageLib::IntegerTexelToFloat32Texel( LSI_PIXEL_FORMAT _pfSrcFormat, uint64_t _ui64Color, float _fReturn[4], LSI_GAMMA _gGamma ) {
		uint32_t ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_R )) - 1ULL);
		if ( ui32Max ) {
			_fReturn[LSI_PC_R] = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_R )) & ui32Max) / ui32Max;
		}
		else { _fReturn[LSI_PC_R] = 0.0f; }

		ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_G )) - 1ULL);
		if ( ui32Max ) {
			_fReturn[LSI_PC_G] = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_G )) & ui32Max) / ui32Max;
		}
		else { _fReturn[LSI_PC_G] = 0.0f; }

		ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_B )) - 1ULL);
		if ( ui32Max ) {
			_fReturn[LSI_PC_B] = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_B )) & ui32Max) / ui32Max;
		}
		else { _fReturn[LSI_PC_B] = 0.0f; }

		ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_A )) - 1ULL);
		if ( ui32Max ) {
			_fReturn[LSI_PC_A] = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_A )) & ui32Max) / ui32Max;
		}
		else { _fReturn[LSI_PC_A] = 1.0f; }
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				_fReturn[LSI_PC_R] = CImageLib::LinearToSRgb( _fReturn[LSI_PC_R] );
				_fReturn[LSI_PC_G] = CImageLib::LinearToSRgb( _fReturn[LSI_PC_G] );
				_fReturn[LSI_PC_B] = CImageLib::LinearToSRgb( _fReturn[LSI_PC_B] );
				break;
			}
			case LSI_G_LINEAR : {
				_fReturn[LSI_PC_R] = CImageLib::SRgbToLinear( _fReturn[LSI_PC_R] );
				_fReturn[LSI_PC_G] = CImageLib::SRgbToLinear( _fReturn[LSI_PC_G] );
				_fReturn[LSI_PC_B] = CImageLib::SRgbToLinear( _fReturn[LSI_PC_B] );
				break;
			}
		}
	}

	/**
	 * Converts from a 32-bit-per-component floating-point format to any integer format.
	 *
	 * \param _pfDstFormat The destination texel format.
	 * \param _fValue The 4-element array of texels in 32-bit floating format to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 * \return Returns the converted texel.  The number of bytes within the return that are valid is
	 *	equal to the number of bytes in the destination format.
	 */
	uint64_t LSE_FCALL CImageLib::Float32TexelToIntegerTexel( LSI_PIXEL_FORMAT _pfDstFormat, const float _fValue[4], LSI_GAMMA _gGamma ) {
		float fR, fG, fB;
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				fR = CImageLib::LinearToSRgb( CStd::Clamp( _fValue[LSI_PC_R], 0.0f, 1.0f ) );
				fG = CImageLib::LinearToSRgb( CStd::Clamp( _fValue[LSI_PC_G], 0.0f, 1.0f ) );
				fB = CImageLib::LinearToSRgb( CStd::Clamp( _fValue[LSI_PC_B], 0.0f, 1.0f ) );
				break;
			}
			case LSI_G_LINEAR : {
				fR = CImageLib::SRgbToLinear( CStd::Clamp( _fValue[LSI_PC_R], 0.0f, 1.0f ) );
				fG = CImageLib::SRgbToLinear( CStd::Clamp( _fValue[LSI_PC_G], 0.0f, 1.0f ) );
				fB = CImageLib::SRgbToLinear( CStd::Clamp( _fValue[LSI_PC_B], 0.0f, 1.0f ) );
				break;
			}
			default : {
				fR = CStd::Clamp( _fValue[LSI_PC_R], 0.0f, 1.0f );
				fG = CStd::Clamp( _fValue[LSI_PC_G], 0.0f, 1.0f );
				fB = CStd::Clamp( _fValue[LSI_PC_B], 0.0f, 1.0f );
			}
		}
		uint64_t ui64R = static_cast<uint64_t>(CStd::RoundInt( fR * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_R )) - 1ULL) ));
		uint64_t ui64G = static_cast<uint64_t>(CStd::RoundInt( fG * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_G )) - 1ULL) ));
		uint64_t ui64B = static_cast<uint64_t>(CStd::RoundInt( fB * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_B )) - 1ULL) ));
		uint64_t ui64A = static_cast<uint64_t>(CStd::RoundInt( CStd::Clamp( _fValue[LSI_PC_A], 0.0f, 1.0f ) * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_A )) - 1ULL) ));
		return (ui64R << GetComponentOffset( _pfDstFormat, LSI_PC_R )) |
			(ui64G << GetComponentOffset( _pfDstFormat, LSI_PC_G )) |
			(ui64B << GetComponentOffset( _pfDstFormat, LSI_PC_B )) |
			(ui64A << GetComponentOffset( _pfDstFormat, LSI_PC_A ));
	}

	/**
	 * Converts any integer format to a 64-bit floating-point format with 16 bits in each component.
	 *
	 * \param _pfSrcFormat The source texel format.
	 * \param _ui64Color The texel to convert.
	 * \param _f16Return Holds the converted color value.  Must be a 4-element array.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	void LSE_FCALL CImageLib::IntegerTexelToFloat16Texel( LSI_PIXEL_FORMAT _pfSrcFormat, uint64_t _ui64Color, CFloat16 _f16Return[4], LSI_GAMMA _gGamma ) {
		float fR, fG, fB;

		uint32_t ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_R )) - 1ULL);
		if ( ui32Max ) {
			fR = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_R )) & ui32Max) / ui32Max;
		}
		else { fR = 0.0f; }
		ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_G )) - 1ULL);
		if ( ui32Max ) {
			fG = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_G )) & ui32Max) / ui32Max;
		}
		else { fG = 0.0f; }

		ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_B )) - 1ULL);
		if ( ui32Max ) {
			fB = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_B )) & ui32Max) / ui32Max;
		}
		else { fB = 0.0f; }

		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				fR = CImageLib::LinearToSRgb( fR );
				fG = CImageLib::LinearToSRgb( fG );
				fB = CImageLib::LinearToSRgb( fB );
				break;
			}
			case LSI_G_LINEAR : {
				fR = CImageLib::SRgbToLinear( fR );
				fG = CImageLib::SRgbToLinear( fG );
				fB = CImageLib::SRgbToLinear( fB );
				break;
			}
		}

		_f16Return[LSI_PC_R] = fR;
		_f16Return[LSI_PC_G] = fG;
		_f16Return[LSI_PC_B] = fB;

		ui32Max = static_cast<uint32_t>((1ULL << GetComponentBits( _pfSrcFormat, LSI_PC_A )) - 1ULL);
		if ( ui32Max ) {
			_f16Return[LSI_PC_A] = static_cast<float>((_ui64Color >> GetComponentOffset( _pfSrcFormat, LSI_PC_A )) & ui32Max) / ui32Max;
		}
		else { _f16Return[LSI_PC_A] = 1.0f; }
	}

	/**
	 * Converts from a 16-bit-per-component floating-point format to any integer format.
	 *
	 * \param _pfDstFormat The destination texel format.
	 * \param _f16Value The 4-element array of texels in 16-bit floating format to convert.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 * \return Returns the converted texel.  The number of bytes within the return that are valid is
	 *	equal to the number of bytes in the destination format.
	 */
	uint64_t LSE_FCALL CImageLib::Float16TexelToIntegerTexel( LSI_PIXEL_FORMAT _pfDstFormat, const CFloat16 _f16Value[4], LSI_GAMMA _gGamma ) {
		float fR, fG, fB;
		fR = static_cast<float>(_f16Value[LSI_PC_R]);
		fG = static_cast<float>(_f16Value[LSI_PC_G]);
		fB = static_cast<float>(_f16Value[LSI_PC_B]);
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				fR = CImageLib::LinearToSRgb( fR );
				fG = CImageLib::LinearToSRgb( fG );
				fB = CImageLib::LinearToSRgb( fB );
				break;
			}
			case LSI_G_LINEAR : {
				fR = CImageLib::SRgbToLinear( fR );
				fG = CImageLib::SRgbToLinear( fG );
				fB = CImageLib::SRgbToLinear( fB );
				break;
			}
		}

		uint64_t ui64R = static_cast<uint64_t>(CStd::RoundInt( fR * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_R )) - 1ULL) ));
		uint64_t ui64G = static_cast<uint64_t>(CStd::RoundInt( fG * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_G )) - 1ULL) ));
		uint64_t ui64B = static_cast<uint64_t>(CStd::RoundInt( fB * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_B )) - 1ULL) ));
		uint64_t ui64A = static_cast<uint64_t>(CStd::RoundInt( CStd::Clamp( static_cast<float>(_f16Value[LSI_PC_A]), 0.0f, 1.0f ) * ((1ULL << GetComponentBits( _pfDstFormat, LSI_PC_A )) - 1ULL) ));
		return (ui64R << GetComponentOffset( _pfDstFormat, LSI_PC_R )) |
			(ui64G << GetComponentOffset( _pfDstFormat, LSI_PC_G )) |
			(ui64B << GetComponentOffset( _pfDstFormat, LSI_PC_B )) |
			(ui64A << GetComponentOffset( _pfDstFormat, LSI_PC_A ));
	}
	
	/**
	 * Converts from a 16-bit-per-component floating-point format to 32-bit-per-component floating-point format.
	 *
	 * \param _f16Value The 4-element array of texels in 16-bit floating format to convert.
	 * \param _fReturn Holds the converted color value.  Must be a 4-element array.
	 * \param _gGamma If _gGamma is not LSI_G_NONE, then an sRGB <-> Linear conversion is performed.
	 */
	void LSE_FCALL CImageLib::Float16TexelToFloat32Texel( const CFloat16 _f16Value[4], float _fReturn[4], LSI_GAMMA _gGamma ) {
		float fR, fG, fB;
		fR = static_cast<float>(_f16Value[LSI_PC_R]);
		fG = static_cast<float>(_f16Value[LSI_PC_G]);
		fB = static_cast<float>(_f16Value[LSI_PC_B]);
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				fR = CImageLib::LinearToSRgb( fR );
				fG = CImageLib::LinearToSRgb( fG );
				fB = CImageLib::LinearToSRgb( fB );
				break;
			}
			case LSI_G_LINEAR : {
				fR = CImageLib::SRgbToLinear( fR );
				fG = CImageLib::SRgbToLinear( fG );
				fB = CImageLib::SRgbToLinear( fB );
				break;
			}
		}

		_fReturn[LSI_PC_R] = fR;
		_fReturn[LSI_PC_G] = fG;
		_fReturn[LSI_PC_B] = fB;
		_fReturn[LSI_PC_A] = _f16Value[LSI_PC_A];
	}

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
	uint64_t LSE_FCALL CImageLib::Float32TexelToIntegerTexel( LSI_PIXEL_FORMAT _pfDstFormat,
		float _fR, float _fG, float _fB, float _fA, LSI_GAMMA _gGamma ) {
		float fArray[4];
		fArray[LSI_PC_R] = _fR;
		fArray[LSI_PC_G] = _fG;
		fArray[LSI_PC_B] = _fB;
		fArray[LSI_PC_A] = _fA;
		switch ( _gGamma ) {
			case LSI_G_sRGB : {
				fArray[LSI_PC_R] = CImageLib::LinearToSRgb( _fR );
				fArray[LSI_PC_G] = CImageLib::LinearToSRgb( _fG );
				fArray[LSI_PC_B] = CImageLib::LinearToSRgb( _fB );
				break;
			}
			case LSI_G_LINEAR : {
				fArray[LSI_PC_R] = CImageLib::SRgbToLinear( _fR );
				fArray[LSI_PC_G] = CImageLib::SRgbToLinear( _fG );
				fArray[LSI_PC_B] = CImageLib::SRgbToLinear( _fB );
				break;
			}
			default : {
				fArray[LSI_PC_R] = _fR;
				fArray[LSI_PC_G] = _fG;
				fArray[LSI_PC_B] = _fB;
			}
		}
		return Float32TexelToIntegerTexel( _pfDstFormat, fArray, _gGamma );
	}

}	// namespace lsi
