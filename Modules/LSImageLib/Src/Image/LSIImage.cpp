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

#include "LSIImage.h"
#include "../Bmp/LSIBmp.h"
#include "../Bmp/LSIPBmp.h"
#include "../Gif/LSIGif.h"
#ifndef LSE_IPHONE
#include "../Ispc/ispc_texcomp.h"
#endif	// #ifndef LSE_IPHONE
#include "../Png/LSIPng.h"
#include "../Tga/LSITga.h"
#include "../Qoi/LSIQoi.h"
#include "Crc/LSSTDCrc.h"
#include "FreeImage.h"
#include "HighLevel/LSFFilesEx.h"
#include "Set/LSTLFixedSet.h"
#include "Set/LSTLSet.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "Vector/LSMVector4.h"

#include <squish.h>


namespace lsi {

	// == Operators.
	/**
	 * Less-than comparison.
	 *
	 * \param _spOther Object against which to test.
	 * \return Returns true if this object is lexicographically less than the given object.
	 */
	LSBOOL LSE_CALL CImage::LSI_SORTED_PIXEL::operator < ( const LSI_SORTED_PIXEL &_spOther ) const {
		if ( ui32Length <= sizeof( uint64_t ) ) { return u.ui64Rgb64 < _spOther.u.ui64Rgb64; }
		return CStd::MemCmp( u.ui8Rgb, _spOther.u.ui8Rgb, ui32Length ) < 0;
	}

	/**
	 * Equality comparison.
	 *
	 * \param _spOther Object against which to test.
	 * \return Returns true if this object is lexicographically equal to the given object.
	 */
	LSBOOL LSE_CALL CImage::LSI_SORTED_PIXEL::operator == ( const LSI_SORTED_PIXEL &_spOther ) const {
		if ( ui32Length <= sizeof( uint64_t ) ) { return u.ui64Rgb64 == _spOther.u.ui64Rgb64; }
		return CStd::MemCmp( u.ui8Rgb, _spOther.u.ui8Rgb, ui32Length ) == 0;
	}

	// == Various constructors.
	LSE_CALLCTOR CImage::CImage() :
		m_ui32Width( 0 ),
		m_ui32Height( 0 ),
		m_pfFormat( LSI_PF_ANY ),
		m_ui32RowWidth( 0 ),
		m_ui32TexelSize( 0 ),
		m_gColorSpace( LSI_G_LINEAR ),
		m_bAutoGamma( false ) {
	}
	LSE_CALLCTOR CImage::CImage( const CImage &_iOther ) {
		m_ui32Width = _iOther.m_ui32Width;
		m_ui32Height = _iOther.m_ui32Height;
		m_pfFormat = _iOther.m_pfFormat;
		m_ui32RowWidth = _iOther.m_ui32RowWidth;
		m_ui32TexelSize = _iOther.m_ui32TexelSize;
		m_gColorSpace = _iOther.m_gColorSpace;
		m_kvKtx1 = _iOther.m_kvKtx1;
		m_kvKtx2 = _iOther.m_kvKtx2;
		m_bAutoGamma = _iOther.m_bAutoGamma;

		if ( !m_tbBuffer.Resize( _iOther.m_tbBuffer.Length() ) ) { throw LSSTD_E_OUTOFMEMORY; }
		if ( _iOther.m_tbBuffer.Length() ) {
			CStd::MemCpy( &m_tbBuffer[0], &_iOther.m_tbBuffer[0], m_tbBuffer.Length() );
		}

		m_vMipMapBuffer = _iOther.m_vMipMapBuffer;
	}

	// == Operators.
	/**
	 * Assignment operator.
	 *
	 * \param _iOther The object to copy.
	 * \return Returns the copied object.
	 */
	CImage & LSE_CALL CImage::operator = ( const CImage &_iOther ) {
		Reset();
		m_ui32Width = _iOther.m_ui32Width;
		m_ui32Height = _iOther.m_ui32Height;
		m_pfFormat = _iOther.m_pfFormat;
		m_ui32RowWidth = _iOther.m_ui32RowWidth;
		m_ui32TexelSize = _iOther.m_ui32TexelSize;
		m_gColorSpace = _iOther.m_gColorSpace;
		m_kvKtx1 = _iOther.m_kvKtx1;
		m_kvKtx2 = _iOther.m_kvKtx2;

		if ( !m_tbBuffer.Resize( _iOther.m_tbBuffer.Length() ) ) { throw LSSTD_E_OUTOFMEMORY; }
		if ( _iOther.m_tbBuffer.Length() ) {
			CStd::MemCpy( &m_tbBuffer[0], &_iOther.m_tbBuffer[0], m_tbBuffer.Length() );
		}

		m_vMipMapBuffer = _iOther.m_vMipMapBuffer;
		return (*this);
	}

	/**
	 * Move-assignment operator.
	 *
	 * \param _iOther The object to copy.
	 * \return Returns the copied object.
	 */
	CImage & LSE_CALL CImage::operator = ( CImage &&_iOther ) {
		Reset();
		m_ui32Width = _iOther.m_ui32Width;
		m_ui32Height = _iOther.m_ui32Height;
		m_pfFormat = _iOther.m_pfFormat;
		m_ui32RowWidth = _iOther.m_ui32RowWidth;
		m_ui32TexelSize = _iOther.m_ui32TexelSize;
		m_gColorSpace = _iOther.m_gColorSpace;
		m_kvKtx1 = _iOther.m_kvKtx1;
		m_kvKtx2 = _iOther.m_kvKtx2;
		m_tbBuffer = std::move( _iOther.m_tbBuffer );

		if ( !m_tbBuffer.Resize( _iOther.m_tbBuffer.Length() ) ) { throw LSSTD_E_OUTOFMEMORY; }
		if ( _iOther.m_tbBuffer.Length() ) {
			CStd::MemCpy( &m_tbBuffer[0], &_iOther.m_tbBuffer[0], m_tbBuffer.Length() );
		}

		m_vMipMapBuffer = _iOther.m_vMipMapBuffer;
		return (*this);
	}

	// == Functions.
	/**
	 * Sets the allocator.  Causes the object to be fully reset.
	 *
	 * \param _paAllocator The new allocator to be used by this object or NULL to use the
	 *	default allocator.
	 */
	void LSE_CALL CImage::SetAllocator( CAllocator * _paAllocator ) {
		if ( _paAllocator != m_tbBuffer.GetAllocator() ) {
			Reset();
			m_tbBuffer.SetAllocator( _paAllocator );
			m_vMipMapBuffer.SetAllocator( _paAllocator );
		}
	}

	/**
	 * Reset the image entirely, leaving it ready to be used again for a new image.
	 */
	void LSE_CALL CImage::Reset() {
		m_ui32Width = m_ui32Height = 0;
		m_pfFormat = LSI_PF_ANY;
		m_tbBuffer.Reset();
		m_vMipMapBuffer.Reset();

		m_ui32RowWidth = m_ui32TexelSize = 0;
		m_gColorSpace = LSI_G_LINEAR;
		m_bAutoGamma = false;
	}

	/**
	 * Creates a mipmap chain up to the given maximum level or 0 to go all the way down to 1x1.
	 *
	 * \param _ui32Levels Maximum levels to generate or 0 to make a full mipmap chain.  The number includes the
	 *	base image, so a value of 1 will not create more mipmap levels beyond the main image.
	 * \param _fFilter The filter to use for resampling.
	 * \param _amAddressMode The texture address mode to use for resampling.
	 * \return Returns true if all mipmaps were created.
	 */
	LSBOOL LSE_CALL CImage::GenerateMipMaps( uint32_t _ui32Levels, LSI_FILTER _fFilter,
		CResampler::LSI_ADDRESS_MODE _amAddressMode ) {
		m_vMipMapBuffer.Reset();

		// Count the number of mipmaps we will generate.
		uint32_t ui32Mips = 0;
		uint32_t ui32Width = GetWidth() >> 1, ui32Height = GetHeight() >> 1;
		for ( uint32_t I = _ui32Levels; --I; ) {
			if ( ui32Width || ui32Height ) {
				ui32Width = CStd::Max<uint32_t>( ui32Width, 1 );
				ui32Height = CStd::Max<uint32_t>( ui32Height, 1 );
			}
			else { break; }
			++ui32Mips;

			ui32Width >>= 1;
			ui32Height >>= 1;
		}

		if ( !ui32Mips ) { return true; }

		m_vMipMapBuffer.Resize( ui32Mips );

		// There can never be more than 32 levels.
		LSH_THREAD tThreads[32] = {
			0
		};
		LSI_MIPMAP_DATA mdData[32] = {
			0
		};
		ui32Mips = 0;

		// ResampleThread
		ui32Width = GetWidth() >> 1, ui32Height = GetHeight() >> 1;
		while ( --_ui32Levels ) {
			if ( ui32Width || ui32Height ) {
				ui32Width = CStd::Max<uint32_t>( ui32Width, 1 );
				ui32Height = CStd::Max<uint32_t>( ui32Height, 1 );
			}
			else { break; }

			CImageLib::CTexelBuffer & tbThis = m_vMipMapBuffer[ui32Mips];
			if ( !tbThis.Resize( CImageLib::GetBufferSizeWithPadding( m_pfFormat, ui32Width, ui32Height ) ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}

			mdData[ui32Mips].piSrc = this;
			mdData[ui32Mips].ui32NewWidth = ui32Width;
			mdData[ui32Mips].ui32NewHeight = ui32Height;
			mdData[ui32Mips].fFilter = _fFilter;
			mdData[ui32Mips].ptbDst = &tbThis;
			mdData[ui32Mips].fGamma = IsSRgb() ? 2.2f : 1.0f;
			mdData[ui32Mips].amAddressMode = _amAddressMode;
			mdData[ui32Mips].fGammaScale = 1.0f;
			if ( !CThreadLib::CreateThread( ResampleThread, &mdData[ui32Mips], &tThreads[ui32Mips] ) ) {
				// Wait for all threads.
				for ( uint32_t J = ui32Mips; J--; ) {
					CThreadLib::WaitForThread( tThreads[J], NULL );
					CThreadLib::CloseThread( tThreads[J] );
				}
				m_vMipMapBuffer.Reset();
				return false;
			}

			/*if ( !m_vMipMapBuffer.Push( CImageLib::CTexelBuffer() ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}
			CImageLib::CTexelBuffer & tbThis = m_vMipMapBuffer[m_vMipMapBuffer.Length()-1];
			if ( !tbThis.Resize( CImageLib::GetBufferSizeWithPadding( m_pfFormat, ui32Width, ui32Height ) ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}

			if ( !Resample( ui32Width, ui32Height, _fFilter, tbThis, IsSRgb() ? 2.2f : 1.0f ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}*/
			++ui32Mips;

			ui32Width >>= 1;
			ui32Height >>= 1;
		}

		uint32_t ui32Ret;
		LSBOOL bRet = true;
		for ( uint32_t J = ui32Mips; J--; ) {
			CThreadLib::WaitForThread( tThreads[J], &ui32Ret );
			CThreadLib::CloseThread( tThreads[J] );
			if ( !ui32Ret ) { bRet = false; }
		}

		if ( !bRet ) { m_vMipMapBuffer.Reset(); }
		return bRet;
	}

	/**
	 * Create an image from a file already loaded to memory.
	 *
	 * \param _pui8FileData Pointer to the loaded file.
	 * \param _ui32DataLen Length of the loaded file data.
	 * \param _ppdPalettes An optional palette database.
	 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
	 */
	LSBOOL LSE_CALL CImage::LoadFileFromMemory( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CPaletteDatabase * _ppdPalettes ) {
		Reset();
		
		if ( CBmp::LoadBmp( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		//if ( CGif::LoadGif( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		if ( CTga::LoadTga( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		bool bSrgb = false;
		if ( CQoi::LoadQoi( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, bSrgb, m_tbBuffer ) ) {
			SetColorSpace( bSrgb ? LSI_G_sRGB : LSI_G_LINEAR );
			PostLoad();
			return true;
		}
		//if ( CPng::LoadPng( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		if ( CDds::LoadDds( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer, m_vMipMapBuffer ) ) { PostLoad(); return true; }
		if ( LoadFreeImage( _pui8FileData, _ui32DataLen ) ) { PostLoad(); return true; }

		if ( CKtx::LoadKtx1( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, 
			m_kvKtx1.ktGlType, m_kvKtx1.kfGlFormat, m_kvKtx1.kifGlInternalFormat, m_kvKtx1.kbifGlInternalBaseFormat,
			m_tbBuffer, m_vMipMapBuffer ) ) { PostLoad(); return true; }
		if ( CKtx::LoadKtx2( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, 
			m_kvKtx2.vkFormat,
			m_tbBuffer, m_vMipMapBuffer ) ) { PostLoad(); return true; }

		if ( _ppdPalettes ) {
			if ( CPBmp::LoadPBmp( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, (*_ppdPalettes), m_tbBuffer, m_vMipMapBuffer ) ) { PostLoad(); return true; }
		}
		return LoadLsi( _pui8FileData, _ui32DataLen );	// Calls PostLoad().
	}

	/**
	 * Create an image from a file.  Path must be in UTF-8 format.
	 *
	 * \param _pcFile UTF-8-formatted string representing the path to the file to load.
	 * \param _ppdPalettes An optional palette database.
	 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
	 */
	LSBOOL LSE_CALL CImage::LoadFile( const char * _pcFile, const CPaletteDatabase * _ppdPalettes ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		LSBOOL bRet = LoadFileFromMemory( ui8Buffer, static_cast<uint32_t>(uiptrSize), _ppdPalettes );
		CFilesEx::FreeFile( ui8Buffer );
		return bRet;
	}

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
	LSBOOL LSE_CALL CImage::CreateBlank( LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t _ui32MipMaps, LSI_GAMMA _gGamma ) {
		Reset();
		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		m_pfFormat = _pfFormat;
		m_gColorSpace = _gGamma;

		if ( !m_tbBuffer.Resize( CImageLib::GetBufferSizeWithPadding( m_pfFormat, m_ui32Width, m_ui32Height ) ) ) { return false; }
		CStd::MemSet( &m_tbBuffer[0], 0, m_tbBuffer.Length() );

		// Mipmaps.
		m_vMipMapBuffer.Reset();
		uint32_t ui32Width = GetWidth() >> 1, ui32Height = GetHeight() >> 1;
		while ( --_ui32MipMaps ) {
			if ( ui32Width || ui32Height ) {
				ui32Width = CStd::Max<uint32_t>( ui32Width, 1 );
				ui32Height = CStd::Max<uint32_t>( ui32Height, 1 );
			}
			else { break; }

			if ( !m_vMipMapBuffer.Push( CImageLib::CTexelBuffer() ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}
			CImageLib::CTexelBuffer & tbThis = m_vMipMapBuffer[m_vMipMapBuffer.Length()-1];
			if ( !tbThis.Resize( CImageLib::GetBufferSizeWithPadding( m_pfFormat, ui32Width, ui32Height ) ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}

			CStd::MemSet( &tbThis[0], 0, tbThis.Length() );


			ui32Width >>= 1;
			ui32Height >>= 1;
		}
		PostLoad();
		return true;
	}

	/**
	 * Create an image from an RGB and alpha source.  The created image is always in LSI_PF_R8G8B8A8 format.  If the alpha
	 *	source has no alpha channel, the intensities of its RGB channels are used to create the alpha.  The dimensions of
	 *	the created image are the the independant maximums of the widths and heights of the input images.
	 *
	 * \param _iRgb The image from which to extract the RGB components for the final image.
	 * \param _iAlpha The image from which to extract the alpha components for the final image.
	 * \return Returns true if there was enough memory to perform the combination.
	 */
	LSBOOL LSE_CALL CImage::CreateFromRgbAndAlpha( const CImage &_iRgb, const CImage &_iAlpha ) {
		CImage iFloatRgb, iFloatAlpha;
		
		try {
			iFloatRgb = _iRgb;
			iFloatAlpha = _iAlpha;

			// Resize to the larger dimensions.
			uint32_t ui32Width = CStd::Max( iFloatRgb.GetWidth(), iFloatAlpha.GetWidth() );
			uint32_t ui32Height = CStd::Max( iFloatRgb.GetHeight(), iFloatAlpha.GetHeight() );

			if ( !iFloatRgb.ResampleInPlace( ui32Width, ui32Height, LSI_F_BILINEAR_FILTER, _iRgb.IsSRgb() ? -2.2f : 1.0f ) ) { return false; }
			if ( !iFloatAlpha.ResampleInPlace( ui32Width, ui32Height, LSI_F_MITCHELL_FILTER, _iAlpha.IsSRgb() ? -2.2f : 1.0f ) ) { return false; }

			iFloatRgb.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
			iFloatAlpha.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );

			// Get ready to copy into ourself.
			if ( !CreateBlank( LSI_PF_R8G8B8A8, ui32Width, ui32Height, 1, _iRgb.IsSRgb() ? LSI_G_sRGB : LSI_G_LINEAR ) ) { return false; }

			uint32_t ui32SrcSize = CImageLib::GetFormatSize( LSI_PF_R32G32B32A32F );
			uint32_t ui32SrcRowSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, ui32Width );
			const uint8_t * pui8SrcRgb = reinterpret_cast<const uint8_t *>(iFloatRgb.GetBufferData());
			const uint8_t * pui8SrcAlpha = reinterpret_cast<const uint8_t *>(iFloatAlpha.GetBufferData());

			uint32_t ui32DstSize = CImageLib::GetFormatSize( LSI_PF_R8G8B8A8 );
			uint32_t ui32DstRowSize = CImageLib::GetRowSize( LSI_PF_R8G8B8A8, ui32Width );
			if ( CImageLib::GetTotalComponents( _iAlpha.GetFormat() ) == 3 ) {
				// Alpha is RGB.
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					uint32_t ui32OffsetSrcY = Y * ui32SrcRowSize;
					uint32_t ui32OffsetDstY = Y * ui32DstRowSize;
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint32_t ui32OffsetSrc = ui32OffsetSrcY + X * ui32SrcSize;
						uint32_t ui32OffsetDst = ui32OffsetDstY + X * ui32DstSize;
						const float * pfSrcRgb = reinterpret_cast<const float *>(&pui8SrcRgb[ui32OffsetSrc]);
						const float * pfSrcAlpha = reinterpret_cast<const float *>(&pui8SrcAlpha[ui32OffsetSrc]);
						float fAlpha = CStd::Clamp( pfSrcAlpha[LSI_PC_R] * LSI_R_WEIGHT +
							pfSrcAlpha[LSI_PC_G] * LSI_G_WEIGHT +
							pfSrcAlpha[LSI_PC_B] * LSI_B_WEIGHT,
							0.0f, 1.0f );

						uint32_t ui32R = static_cast<uint8_t>(CStd::Clamp( pfSrcRgb[LSI_PC_R], 0.0f, 1.0f ) * 255.0f);
						uint32_t ui32G = static_cast<uint8_t>(CStd::Clamp( pfSrcRgb[LSI_PC_G], 0.0f, 1.0f ) * 255.0f);
						uint32_t ui32B = static_cast<uint8_t>(CStd::Clamp( pfSrcRgb[LSI_PC_B], 0.0f, 1.0f ) * 255.0f);
						uint32_t ui32A = static_cast<uint8_t>(fAlpha * 255.0f);
						uint32_t ui32Rgba = (ui32R << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |
							(ui32G << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |
							(ui32B << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |
							(ui32A << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ));
						(*reinterpret_cast<uint32_t *>(&m_tbBuffer[ui32OffsetDst])) = ui32Rgba;
					}
				}
			}
			else {
				// Alpha is RGBA or A only.  Either way is the same here.
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					uint32_t ui32OffsetSrcY = Y * ui32SrcRowSize;
					uint32_t ui32OffsetDstY = Y * ui32DstRowSize;
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint32_t ui32OffsetSrc = ui32OffsetSrcY + X * ui32SrcSize;
						uint32_t ui32OffsetDst = ui32OffsetDstY + X * ui32DstSize;
						const float * pfSrcRgb = reinterpret_cast<const float *>(&pui8SrcRgb[ui32OffsetSrc]);
						const float * pfSrcAlpha = reinterpret_cast<const float *>(&pui8SrcAlpha[ui32OffsetSrc]);
						float fAlpha = CStd::Clamp( pfSrcAlpha[LSI_PC_A], 0.0f, 1.0f );

						uint32_t ui32R = static_cast<uint8_t>(CStd::Clamp( pfSrcRgb[LSI_PC_R], 0.0f, 1.0f ) * 255.0f);
						uint32_t ui32G = static_cast<uint8_t>(CStd::Clamp( pfSrcRgb[LSI_PC_G], 0.0f, 1.0f ) * 255.0f);
						uint32_t ui32B = static_cast<uint8_t>(CStd::Clamp( pfSrcRgb[LSI_PC_B], 0.0f, 1.0f ) * 255.0f);
						uint32_t ui32A = static_cast<uint8_t>(fAlpha * 255.0f);
						uint32_t ui32Rgba = (ui32R << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |
							(ui32G << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |
							(ui32B << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |
							(ui32A << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ));
						(*reinterpret_cast<uint32_t *>(&m_tbBuffer[ui32OffsetDst])) = ui32Rgba;
					}
				}
			}
		}
		catch ( ... ) { return false; }

		return true;
	}

	/**
	 * Creates a blank KTX 1 image.
	 *
	 * \param _kifFormat Format of the image to create.
	 * \param _ui32Width Width of the image to create.
	 * \param _ui32Height Height of the image to create.
	 * \param _ui32MipMaps Number of mipmaps to generate, including the top level.  Specify 0 to generate a full chain.
	 * \return Returns true if there is enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CImage::CreateBlankKtx1( LSI_KTX_INTERNAL_FORMAT _kifFormat, uint32_t _ui32Width, uint32_t _ui32Height,
		uint32_t _ui32MipMaps ) {
		const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdInternalFormat = CKtx::FindInternalFormatData( _kifFormat );
		if ( !pkifdInternalFormat ) { return false; }


		Reset();


		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		m_pfFormat = LSI_PF_KTX1;
		m_kvKtx1.kifGlInternalFormat = _kifFormat;
		m_kvKtx1.kbifGlInternalBaseFormat = pkifdInternalFormat->kbifBaseInternalFormat;
		m_kvKtx1.ktGlType = pkifdInternalFormat->bCompressed ? static_cast<LSI_KTX_TYPE>(0) : pkifdInternalFormat->ktType;
		m_kvKtx1.kfGlFormat = pkifdInternalFormat->bCompressed ? static_cast<LSI_KTX_FORMAT>(0) : static_cast<LSI_KTX_FORMAT>(pkifdInternalFormat->kbifBaseInternalFormat);
		m_gColorSpace = pkifdInternalFormat->bSrgb ? LSI_G_sRGB : LSI_G_LINEAR;

		uint32_t ui32Size = CKtx::GetTextureSize( m_ui32Width, m_ui32Height, 1, pkifdInternalFormat );
		if ( !ui32Size ) { return false; }
		if ( !m_tbBuffer.Resize( ui32Size + 8 ) ) { return false; }
		CStd::MemSet( &m_tbBuffer[0], 0, m_tbBuffer.Length() );

		// Mipmaps.
		m_vMipMapBuffer.Reset();
		uint32_t ui32Width = GetWidth() >> 1, ui32Height = GetHeight() >> 1;
		while ( --_ui32MipMaps ) {
			if ( ui32Width || ui32Height ) {
				ui32Width = CStd::Max<uint32_t>( ui32Width, 1 );
				ui32Height = CStd::Max<uint32_t>( ui32Height, 1 );
			}
			else { break; }

			if ( !m_vMipMapBuffer.Push( CImageLib::CTexelBuffer() ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}
			CImageLib::CTexelBuffer & tbThis = m_vMipMapBuffer[m_vMipMapBuffer.Length()-1];
			ui32Size = CKtx::GetTextureSize( ui32Width, ui32Height, 1, pkifdInternalFormat );
			if ( !tbThis.Resize( ui32Size + 8 ) ) {
				m_vMipMapBuffer.Reset();
				return false;
			}

			CStd::MemSet( &tbThis[0], 0, tbThis.Length() );


			ui32Width >>= 1;
			ui32Height >>= 1;
		}
		PostLoad();
		return true;
	}

	/**
	 * Compress this texture to a buffer.  All compressions are tested and the smallest is used.  Also stores the
	 *	width, height, and format.
	 *
	 * \param _bBuffer The output buffer where the compressed data will be stored.
	 * \param _bNoCompression If true, the image is not compressed when saved to the buffer.  This decreases the amount
	 *	of time it takes for the operation to complete.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CImage::CompressToBuffer( CMisc::LSC_BUFFER &_bBuffer, LSBOOL _bNoCompression ) {
		// Even though the data is small, compress the width, height, and format too.
		CMisc::LSC_BUFFER bTemp;
		if ( !bTemp.Append( reinterpret_cast<uint8_t *>(&m_ui32Width), sizeof( m_ui32Width ) ) ) { return false; }
		if ( !bTemp.Append( reinterpret_cast<uint8_t *>(&m_ui32Height), sizeof( m_ui32Height ) ) ) { return false; }
		if ( !bTemp.Append( reinterpret_cast<uint8_t *>(&m_pfFormat), 1 ) ) { return false; }
		uint8_t ui8Mips = static_cast<uint8_t>(TotalMipLevels());
		if ( !bTemp.Append( reinterpret_cast<uint8_t *>(&ui8Mips), 1 ) ) { return false; }
		uint8_t ui8Srgb = m_gColorSpace == LSI_G_sRGB;
		if ( !bTemp.Append( reinterpret_cast<uint8_t *>(&ui8Srgb), 1 ) ) { return false; }
		if ( !CMisc::CompressToBuffer( &bTemp[0], bTemp.Length(), _bBuffer ) ) { return false; }

		// Find the best compression type.
		LSI_COMPRESSIONS cComp;
		bTemp.Reset();
		CMisc::LSC_BUFFER bBest;
		switch ( GetFormat() ) {
			case LSI_PF_DXT1 : {}
			case LSI_PF_DXT2 : {}
			case LSI_PF_DXT3 : {}
			case LSI_PF_DXT4 : {}
			case LSI_PF_DXT5 : {
				cComp = LSI_C_DTXCOMPRESSION;
				if ( !CompressDxt( bBest ) ) { return false; }

				break; 
			}
			default : {
				if ( !_bNoCompression ) {
					// Set the best compression to the one most likely to give the best results.
					if ( !CompressIndex( bBest ) ) { return false; }
					cComp = LSI_C_INDEXCOMPRESSION;

					// Now try to beat it with the other compression types.
					if ( CompressRle( bTemp ) ) {
						if ( bTemp.Length() < bBest.Length() ) {
							// RLE beat it.
							bBest = bTemp;
							cComp = LSI_C_RLECOMPRESSION;
						}
					}
					bTemp.Reset();


					if ( CompressChannels( bTemp, bBest.Length() ) ) {
						if ( bTemp.Length() < bBest.Length() ) {
							// Channel stripping beat it.
							bBest = bTemp;
							cComp = LSI_C_CHANNELCOMPRESSION;
						}
					}
					bTemp.Reset();


					if ( !CompressRaw( bTemp ) ) { return false; }
					if ( bTemp.Length() < bBest.Length() ) {
						// Raw compression beat it.
						bBest = bTemp;
						cComp = LSI_C_RAWCOMPRESSION;
					}
					bTemp.Reset();
				}
				else {
					if ( !CompressRaw( bTemp ) ) { return false; }
					bBest = bTemp;
					cComp = LSI_C_RAWCOMPRESSION;
				}
			}
		}

		
		

		// Add it to the final result.
		if ( !_bBuffer.Append( reinterpret_cast<uint8_t *>(&cComp), 1 ) ) { return false; }
		if ( !_bBuffer.Append( &bBest[0], bBest.Length() ) ) { return false; }

		return true;
	}

	/**
	 * Flip the image vertically.
	 */
	void LSE_CALL CImage::Flip() {
		if ( m_tbBuffer.Length() <= 1 ) { return; }
		uint32_t ui32RowSize = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		void * pvBuffer = CMemLib::MAlloc( ui32RowSize );
		for ( uint32_t I = m_ui32Height >> 1; I--; ) {
			CStd::MemCpy( pvBuffer, &m_tbBuffer[I*ui32RowSize], ui32RowSize );
			CStd::MemCpy( &m_tbBuffer[I*ui32RowSize], &m_tbBuffer[(m_ui32Height-I-1)*ui32RowSize], ui32RowSize );
			CStd::MemCpy( &m_tbBuffer[(m_ui32Height-I-1)*ui32RowSize], pvBuffer, ui32RowSize );
		}
		CMemLib::Free( pvBuffer );
	}

	/**
	 * Is the given channel entirely set to a given value?
	 *
	 * \param _pcComponent The channel to check.
	 * \param _fValue The reference value.
	 * \return Returns true if the given channel is entirely set to the given value.
	 */
	LSBOOL LSE_CALL CImage::IsChannelSetTo( LSI_PIXEL_COMPONENTS _pcComponent, float _fValue ) const {
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return false; }
		uint32_t ui32Shift = CImageLib::GetComponentOffset( GetFormat(), _pcComponent );
		uint32_t ui32Mask = CImageLib::GetComponentBits( GetFormat(), _pcComponent );
		uint32_t ui32RowSize = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		uint32_t ui32SrcTexelSize = CImageLib::GetFormatSize( GetFormat() );
		float fMul = static_cast<float>(1 << ui32Mask) - 1.0f;
		uint32_t ui32Val = static_cast<uint32_t>(_fValue * fMul);

		ui32Mask = ((1 << ui32Mask) - 1);
#define LSI_TEXEL_AT( X, Y )			(*reinterpret_cast<const uint64_t *>(&m_tbBuffer[Y*ui32RowSize+X*ui32SrcTexelSize]))
		for ( uint32_t Y = GetHeight(); Y--; ) {
			for ( uint32_t X = GetWidth(); X--; ) {
				uint64_t ui64Texel = LSI_TEXEL_AT( X, Y );

				uint32_t ui32This = (ui64Texel >> ui32Shift) & ui32Mask;
				if ( ui32This != ui32Val ) { return false; }
			}
		}
		return true;
#undef LSI_TEXEL_AT
	}

	/**
	 * Swaps 2 channels.
	 *
	 * \param _pcCom0 Channel 0.
	 * \param _pcCom1 Channel 1.
	 */
	void LSE_CALL CImage::SwapChannels( LSI_PIXEL_COMPONENTS _pcCom0, LSI_PIXEL_COMPONENTS _pcCom1 ) {
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return; }
		uint32_t ui32Shift0 = CImageLib::GetComponentOffset( GetFormat(), _pcCom0 );
		uint32_t ui32Bits0 = CImageLib::GetComponentBits( GetFormat(), _pcCom0 );
		uint32_t ui32Mask0 = ((1 << ui32Bits0) - 1);
		float fMul0 = 1.0f / (static_cast<float>(1 << ui32Bits0) - 1.0f);
		
		uint32_t ui32Shift1 = CImageLib::GetComponentOffset( GetFormat(), _pcCom1 );
		uint32_t ui32Bits1 = CImageLib::GetComponentBits( GetFormat(), _pcCom1 );
		uint32_t ui32Mask1 = ((1 << ui32Bits1) - 1);
		float fMul1 = 1.0f / (static_cast<float>(1 << ui32Bits1) - 1.0f);

		uint64_t ui64ClearMask = (static_cast<uint64_t>(ui32Mask0) << static_cast<uint64_t>(ui32Shift0));
		ui64ClearMask |= (static_cast<uint64_t>(ui32Mask1) << static_cast<uint64_t>(ui32Shift1));
		ui64ClearMask = ~ui64ClearMask;

		uint32_t ui32RowSize = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		uint32_t ui32SrcTexelSize = CImageLib::GetFormatSize( GetFormat() );

#define LSI_TEXEL_AT( X, Y )			(*reinterpret_cast<uint64_t *>(&m_tbBuffer[Y*ui32RowSize+X*ui32SrcTexelSize]))
		for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < GetWidth(); ++X ) {
				uint64_t & ui64Texel = LSI_TEXEL_AT( X, Y );
				
				uint32_t ui32This0 = (ui64Texel >> ui32Shift0) & ui32Mask0;
				uint32_t ui32This1 = (ui64Texel >> ui32Shift1) & ui32Mask1;

				ui64Texel &= ui64ClearMask;

				float fThis0 = ui32This0 * fMul0;
				float fThis1 = ui32This1 * fMul1;

				// Convert to the opposite bit amount.
				fThis0 /= fMul1;
				fThis1 /= fMul0;

				ui32This0 = static_cast<uint32_t>(fThis0 + 0.5f);
				ui32This1 = static_cast<uint32_t>(fThis1 + 0.5f);

				ui64Texel |= ui32This0 << ui32Shift1;
				ui64Texel |= ui32This1 << ui32Shift0;
			}
		}
#undef LSI_TEXEL_AT
	}

	/**
	 * Converts to a given format and return a new image in that format.
	 *
	 * \param _pfFormat Format to which to convert this image.
	 * \param _iDest Destination image.
	 * \param _pvParm Optional parameters to pass to the conversion routine.
	 * \param _gTargetGamma If not LSI_G_NONE, an sRGB <-> Linear conversion will take place if necessary.
	 */
	void LSE_CALL CImage::ConvertToFormat( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDest, void * _pvParm, LSI_GAMMA _gTargetGamma ) const {
		LSI_GAMMA gTarget = _gTargetGamma == LSI_G_NONE ? m_gColorSpace : _gTargetGamma;
		if ( _pfFormat == m_pfFormat &&
			(m_gColorSpace == gTarget) ) {
			_iDest = (*this);		// Copy directly.
			return;
		}

		// Format changed, so make a copy.
		switch ( _pfFormat ) {
			case LSI_PF_KTX1 : {
			}
			default : {
				if ( !_iDest.CreateBlank( _pfFormat, m_ui32Width, m_ui32Height, TotalMipLevels(),
					gTarget ) ) { throw LSSTD_E_OUTOFMEMORY; }
				break;
			}
		}
		if ( !m_ui32Height || !m_ui32Width ) { return; }
		uint32_t ui32H = m_ui32Height;
		uint32_t ui32W = m_ui32Width;
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			CImageLib::CTexelBuffer & tbSrc = const_cast<CImageLib::CTexelBuffer &>(GetMipMapBuffers( I ));
			CImageLib::CTexelBuffer & tbDst = const_cast<CImageLib::CTexelBuffer &>(_iDest.GetMipMapBuffers( I ));
			CImageLib::CTexelBuffer * ptbSrc = &tbSrc;
			LSI_GAMMA gSrcGamma = m_gColorSpace;
			
			LSI_PIXEL_FORMAT pfSrcFormat = m_pfFormat;
			// Compressed formats go to an intermediary buffer.
			CImageLib::CTexelBuffer tbTmp;
			switch ( m_pfFormat ) {
				case LSI_PF_DXT1 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_DXT1( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_DXT2 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_DXT2( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_DXT3 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_DXT3( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_DXT4 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_DXT4( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_DXT5 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_DXT5( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC4 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC4U( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC4S : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC4S( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC5 : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC5U( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC5S : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC5S( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC6U : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC6U( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC6S : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC6S( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC7U : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC7U( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, IsSRgb() != 0 ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_BC7_SRGB : {
					ptbSrc = &tbTmp;
					if ( !CDds::Decompress_BC7U( pfSrcFormat, &tbSrc, ptbSrc, ui32W, ui32H, true ) ) { throw LSSTD_E_OUTOFMEMORY; }
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
				case LSI_PF_KTX1 : {
					const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdInternalFormat = CKtx::FindInternalFormatData( m_kvKtx1.kifGlInternalFormat );
					if ( !pkifdInternalFormat ) { throw LSSTD_E_INVALIDDATA; }
					if ( !pkifdInternalFormat->pfToRgba32F ) { throw LSSTD_E_FEATURENOTSUPPORTED; }
					ptbSrc = &tbTmp;
					pfSrcFormat = LSI_PF_R32G32B32A32F;
					if ( !ptbSrc->Resize( CImageLib::GetBufferSizeWithPadding( pfSrcFormat, ui32W, ui32H ) ) ) { throw LSSTD_E_OUTOFMEMORY; }
					if ( !pkifdInternalFormat->pfToRgba32F( &tbSrc[0], &(*ptbSrc)[0], ui32W, ui32H, 1, _pvParm ) ) { throw LSSTD_E_INTERNALERROR; }
					// These conversions to RGBA32F always give a linear result.
					gSrcGamma = LSI_G_LINEAR;
					_iDest.SetColorSpace( gSrcGamma );
					break;
				}
			}
			LSI_GAMMA gConvGamma = gSrcGamma == gTarget ? LSI_G_NONE : gTarget;
			switch ( _pfFormat ) {
				case LSI_PF_KTX1 : {
				}
				default : {
					CImageLib::BatchAnyTexelToAnytexel( pfSrcFormat, &(*ptbSrc)[0], _pfFormat, reinterpret_cast<void *>(&tbDst[0]), ui32W * ui32H, gConvGamma );
					break;
				}
			}
			ui32H = CStd::Max<uint32_t>( ui32H >> 1, 1 );
			ui32W = CStd::Max<uint32_t>( ui32W >> 1, 1 );
		}
	}

	/**
	 * Converts this image in-place to a given format.
	 *
	 * \param _pfFormat Format to which to convert this image.
	 * \param _pvParm Optional parameters to pass to the conversion routine.
	 * \return Returns the converted image.
	 */
	CImage & LSE_CALL CImage::ConvertToFormatInPlace( LSI_PIXEL_FORMAT _pfFormat, void * _pvParm ) {
		if ( _pfFormat == m_pfFormat ) { return (*this); }	// No conversion.
		CImage iTemp;
		ConvertToFormat( _pfFormat, iTemp, _pvParm );
		try {
			(*this) = iTemp;	// A little slow but not so bad since copy operations are super fast.
		}
		catch ( ... ) {
			CStd::DebugPrintA( "CImage::ConvertToFormatInPlace() failed." );
		}
		return (*this);
	}

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
	LSBOOL LSE_CALL CImage::Resample( uint32_t _ui32NewWidth, uint32_t _ui32NewHeight, LSI_FILTER _fFilter, CImage &_iDest,
		float _fSrcGamma, CResampler::LSI_ADDRESS_MODE _amAddressMode, float _fFilterScale ) const {
		if ( _ui32NewWidth == GetWidth() && _ui32NewHeight == GetHeight() ) {
			// Just copy it.
			try {
				_iDest = (*this);
			}
			catch ( ... ) { return false; }
			return true;
		}
		// If resizing is necessary then the texture must not be a DXT format.
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return false; }

#ifdef _DEBUG
#define LSI_FAIL( STRING )			CStd::DebugPrintA( "CImage::Resample(): " STRING "\r\n" ); return false
#else
#define LSI_FAIL( STRING )			return false
#endif	// #ifdef _DEBUG


		if ( !_iDest.CreateBlank( GetFormat(), _ui32NewWidth, _ui32NewHeight, 1U, GetColorSpace() ) ) {
			LSI_FAIL( "Out of memory while resampling." );
		}

		return Resample( _ui32NewWidth, _ui32NewHeight, _fFilter, _iDest.m_tbBuffer, _fSrcGamma, _amAddressMode, _fFilterScale );
	}

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
	LSBOOL LSE_CALL CImage::Resample( uint32_t _ui32NewWidth, uint32_t _ui32NewHeight, LSI_FILTER _fFilter, CImageLib::CTexelBuffer &_tbDest,
		float _fSrcGamma, CResampler::LSI_ADDRESS_MODE _amAddressMode, float _fFilterScale ) const {
		if ( _ui32NewWidth == GetWidth() && _ui32NewHeight == GetHeight() ) {
			// Just copy it.
			try {
				if ( _tbDest.Length() == m_tbBuffer.Length() && _tbDest.Length() ) {
					CStd::MemCpy( &_tbDest[0], &m_tbBuffer[0], _tbDest.Length() );
				}
			}
			catch ( ... ) { return false; }
			return true;
		}
		// If resizing is necessary then the texture must not be a DXT format.
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return false; }

#ifdef _DEBUG
#define LSI_FAIL( STRING )			CStd::DebugPrintA( "CImage::Resample(): " STRING "\r\n" ); return false
#else
#define LSI_FAIL( STRING )			return false
#endif	// #ifdef _DEBUG


		static const char * pcFiters[] = {
			"box",						// LSI_F_BOX_FILTER
			"bilinear",					// LSI_F_BILINEAR_FILTER
			"quadratic",				// LSI_F_QUADRATIC_FILTER
			"kaiser",					// LSI_F_KAISER_FILTER
			"lanczos2",					// LSI_F_LANCZOS2_FILTER
			"lanczos3",					// LSI_F_LANCZOS3_FILTER
			"lanczos4",					// LSI_F_LANCZOS4_FILTER
			"lanczos6",					// LSI_F_LANCZOS6_FILTER
			"lanczos8",					// LSI_F_LANCZOS8_FILTER
			"lanczos12",				// LSI_F_LANCZOS12_FILTER
			"lanczos64",				// LSI_F_LANCZOS64_FILTER
			"mitchell",					// LSI_F_MITCHELL_FILTER
			"catmullrom",				// LSI_F_CATMULLROM_FILTER
			"bspline",					// LSI_F_BSPLINE_FILTER
			"blackman",					// LSI_F_BLACKMAN_FILTER
			"gaussian",					// LSI_F_GAUSSIAN_FILTER
			"bell",						// LSI_F_BELL_FILTER
		};
		if ( _fFilter >= LSE_ELEMENTS( pcFiters ) ) {
			CStd::DebugPrintA( "CImage::Resample(): Invalid filter.\r\n" );
			return false;
		}


		float fSrgbToLinear[256];

		uint8_t ui8LinearToSrgb[LSI_LINEAR_TO_SRGB_TABLE_SIZE];
		static const float fInvLinearToSrgbTableSize = 1.0f / LSI_LINEAR_TO_SRGB_TABLE_SIZE;
		const float fInvSrcGamma = 1.0f / _fSrcGamma;

		if ( _fSrcGamma < 0.0f ) {
			for ( uint32_t I = LSE_ELEMENTS( fSrgbToLinear ); I--; ) {
				fSrgbToLinear[I] = CImageLib::SRgbToLinear( (1.0f / 255.0f) * I );
			}

			for ( uint32_t I = LSI_LINEAR_TO_SRGB_TABLE_SIZE; I--; ) {
				int32_t i32This = static_cast<int32_t>(255.0f * CImageLib::LinearToSRgb( I * fInvLinearToSrgbTableSize ) + 0.5f);
				i32This = CStd::Clamp<int32_t>( i32This, 0, 255 );
				ui8LinearToSrgb[I] = static_cast<uint8_t>(static_cast<uint32_t>(i32This));
			}
		}
		else {
			for ( uint32_t I = LSE_ELEMENTS( fSrgbToLinear ); I--; ) {
				fSrgbToLinear[I] = std::powf( (1.0f / 255.0f) * I, _fSrcGamma );
			}

			for ( uint32_t I = LSI_LINEAR_TO_SRGB_TABLE_SIZE; I--; ) {
				int32_t i32This = static_cast<int32_t>(255.0f * std::powf( I * fInvLinearToSrgbTableSize, fInvSrcGamma ) + 0.5f);
				i32This = CStd::Clamp<int32_t>( i32This, 0, 255 );
				ui8LinearToSrgb[I] = static_cast<uint8_t>(static_cast<uint32_t>(i32This));
			}
		}


		
		CVectorPoD<float, uint32_t> vSamples[4];
		CVectorPoD<float, uint32_t> vDstBuffer;

		CResampler rResamplers[4];

		
		if ( !vDstBuffer.Resize( _ui32NewWidth ) ) { 
			LSI_FAIL( "Failed to allocate scanline buffer (insufficient memory)." );
		}
		switch ( rResamplers[0].InitResampler( GetWidth(), GetHeight(), _ui32NewWidth, _ui32NewHeight,
			_amAddressMode,
			0.0f, 1.0f, pcFiters[_fFilter], NULL, NULL, _fFilterScale, _fFilterScale, &vDstBuffer ) ) {
			case LSSTD_E_OUTOFMEMORY : {
				LSI_FAIL( "Out of memory." );
			}
			case LSSTD_E_INTERNALERROR : {
				LSI_FAIL( "Internal error." );
			}
			default : {}
		}
		if ( !vSamples[0].Resize( GetWidth() ) ) {
			LSI_FAIL( "Out of memory while creating sample buffer." );
		}

		uint32_t ui32Components = CImageLib::GetTotalComponents( GetFormat() );
		for ( uint32_t I = 1; I < ui32Components; ++I ) {
			switch ( rResamplers[I].InitResampler( GetWidth(), GetHeight(), _ui32NewWidth, _ui32NewHeight,
				_amAddressMode,
				0.0f, 1.0f, pcFiters[_fFilter],
				rResamplers[0].GetContribX(), rResamplers[0].GetContribY(), _fFilterScale, _fFilterScale, &vDstBuffer ) ) {
				case LSSTD_E_OUTOFMEMORY : {
					LSI_FAIL( "Out of memory." );
				}
				case LSSTD_E_INTERNALERROR : {
					LSI_FAIL( "Internal error." );
				}
				default : {}
			}
			if ( !vSamples[I].Resize( GetWidth() ) ) {
				LSI_FAIL( "Out of memory while creating sample buffer." );
			}
		}




		// For each row in the source image.
		uint32_t ui32Pitch = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		uint32_t ui32Size = CImageLib::GetFormatSize( GetFormat() );
		uint32_t ui32DstY = 0;
		for ( uint32_t ui32SrcY = 0; ui32SrcY < GetHeight(); ++ui32SrcY ) {
			const uint8_t * pui8This = &m_tbBuffer[ui32Pitch*ui32SrcY];

			// Going across the row.
			for ( uint32_t X = 0; X < GetWidth(); ++X ) {
				// For each component.
				float fConv[4];
				switch ( GetFormat() ) {
					case LSI_PF_R16G16B16A16F : {
						CImageLib::Float16TexelToFloat32Texel( reinterpret_cast<const CFloat16 *>(pui8This), fConv, LSI_G_NONE );
						break;
					}
					case LSI_PF_R32G32B32A32F : {
						const float * pfThis = reinterpret_cast<const float *>(pui8This);
						fConv[0] = pfThis[0];
						fConv[1] = pfThis[1];
						fConv[2] = pfThis[2];
						fConv[3] = pfThis[3];
						break;
					}
					default : {
#ifdef LSE_IPHONE
						uint64_t ui64This = (static_cast<uint64_t>(pui8This[7]) << 56ULL) |
							(static_cast<uint64_t>(pui8This[6]) << 48ULL) |
							(static_cast<uint64_t>(pui8This[5]) << 40ULL) |
							(static_cast<uint64_t>(pui8This[4]) << 32ULL) |
							(static_cast<uint64_t>(pui8This[3]) << 24ULL) |
							(static_cast<uint64_t>(pui8This[2]) << 16ULL) |
							(static_cast<uint64_t>(pui8This[1]) << 8ULL) |
							(static_cast<uint64_t>(pui8This[0]) << 0ULL);
						CImageLib::IntegerTexelToFloat32Texel( GetFormat(), ui64This, fConv );
#else
						CImageLib::IntegerTexelToFloat32Texel( GetFormat(), (*reinterpret_cast<const uint64_t *>(pui8This)), fConv, LSI_G_NONE );
#endif	// #ifdef LSE_IPHONE
					}
				}

				// TODO: Do not use the table for floating-point formats.
				for ( uint32_t I = 0; I < ui32Components; ++I ) {
					// If this component is alpha, keep it in SRGB mode.
					LSI_PIXEL_COMPONENTS pcThis = CImageLib::ComponentIndexToComponentType( GetFormat(), I );
					if ( pcThis == LSI_PC_A ) {
						vSamples[I][X] = fConv[LSI_PC_A];
					}
					else {
						vSamples[I][X] = fSrgbToLinear[static_cast<uint8_t>(fConv[pcThis]*255.0f)];
					}
				}
				pui8This += ui32Size;
			}

			// Add the scanlines.
			for ( uint32_t I = 0; I < ui32Components; ++I ) {
				if ( !rResamplers[I].AddScanLine( &vSamples[I][0] ) ) {
					LSI_FAIL( "Out of memory while creating scanlines." );
				}
			}

			uint32_t ui32RowSize = CImageLib::GetRowSize( GetFormat(), _ui32NewWidth );
			for ( ; ; ) {
				const CVectorPoD<float, uint32_t> * pvOut = NULL;
				if ( ui32DstY >= _ui32NewHeight ) { break; }
				uint8_t * pui8Dest = &_tbDest[ui32RowSize*ui32DstY];
				for ( uint32_t I = 0; I < ui32Components; ++I ) {
					pvOut = rResamplers[I].GetScanLine();
					if ( !pvOut ) { break; }
					LSI_PIXEL_COMPONENTS pcThis = CImageLib::ComponentIndexToComponentType( GetFormat(), I );
					LSBOOL bIsAlpha = pcThis == LSI_PC_A;
					uint64_t ui64Shift = CImageLib::GetComponentOffset( GetFormat(), pcThis );

					// Mask out the existing value.
					uint64_t ui64HighestValue = (1ULL << CImageLib::GetComponentBits( GetFormat(), pcThis )) - 1ULL;
					uint64_t ui64Mask = ui64HighestValue << ui64Shift;
					ui64Mask = ~ui64Mask;
					// Scale from 8-bit components to the bits in this component.
					float fCompScale = static_cast<float>(ui64HighestValue) / 255.0f;

					// Go across the whole row.
					for ( uint32_t X = 0; X < _ui32NewWidth; ++X ) {
						uint8_t * pui8Target = &pui8Dest[X*ui32Size];
						
						switch ( GetFormat() ) {
							case LSI_PF_R16G16B16A16F : {
								float fVal;
								if ( bIsAlpha ) {
									fVal = (*pvOut)[X];
								}
								else {
									fVal = std::powf( (*pvOut)[X], fInvSrcGamma );
								}
								CFloat16 * pfThis = reinterpret_cast<CFloat16 *>(pui8Target);
								pfThis[I] = fVal;
								break;
							}
							case LSI_PF_R32G32B32A32F : {
								float fVal;
								if ( bIsAlpha ) {
									fVal = (*pvOut)[X];
								}
								else {
									fVal = std::powf( (*pvOut)[X], fInvSrcGamma );
								}
								float * pfThis = reinterpret_cast<float *>(pui8Target);
								pfThis[I] = fVal;
								break;
							}
							default : {
#ifdef LSE_IPHONE
								uint64_t ui64Val = (static_cast<uint64_t>(pui8Target[7]) << 56ULL) |
									(static_cast<uint64_t>(pui8Target[6]) << 48ULL) |
									(static_cast<uint64_t>(pui8Target[5]) << 40ULL) |
									(static_cast<uint64_t>(pui8Target[4]) << 32ULL) |
									(static_cast<uint64_t>(pui8Target[3]) << 24ULL) |
									(static_cast<uint64_t>(pui8Target[2]) << 16ULL) |
									(static_cast<uint64_t>(pui8Target[1]) << 8ULL) |
									(static_cast<uint64_t>(pui8Target[0]) << 0ULL);
#else
								uint64_t ui64Val = (*reinterpret_cast<uint64_t *>(pui8Target));
#endif	// #ifdef LSE_IPHONE
								ui64Val &= ui64Mask;
								if ( bIsAlpha ) {
									float fVal = (255.0f * (*pvOut)[X] + 0.5f) * fCompScale;
									int64_t i64Val = static_cast<int64_t>(fVal);
									i64Val = CStd::Clamp<int64_t>( i64Val, 0LL, static_cast<int64_t>(ui64HighestValue) );
									ui64Val |= static_cast<uint64_t>(i64Val << static_cast<int64_t>(ui64Shift));
								}
								else {
									int64_t i64Val = static_cast<int64_t>(LSI_LINEAR_TO_SRGB_TABLE_SIZE * (*pvOut)[X] + 0.5f);
									i64Val = CStd::Clamp<int64_t>( i64Val, 0LL, LSI_LINEAR_TO_SRGB_TABLE_SIZE - 1 );
									i64Val = static_cast<int64_t>(ui8LinearToSrgb[i64Val] * fCompScale);
									ui64Val |= static_cast<uint64_t>(i64Val << static_cast<int64_t>(ui64Shift));
								}
#ifdef LSE_IPHONE
								pui8Target[7] = static_cast<uint8_t>((ui64Val >> 56ULL) & 0xFF);
								pui8Target[6] = static_cast<uint8_t>((ui64Val >> 48ULL) & 0xFF);
								pui8Target[5] = static_cast<uint8_t>((ui64Val >> 40ULL) & 0xFF);
								pui8Target[4] = static_cast<uint8_t>((ui64Val >> 32ULL) & 0xFF);
								pui8Target[3] = static_cast<uint8_t>((ui64Val >> 24ULL) & 0xFF);
								pui8Target[2] = static_cast<uint8_t>((ui64Val >> 16ULL) & 0xFF);
								pui8Target[1] = static_cast<uint8_t>((ui64Val >> 8ULL) & 0xFF);
								pui8Target[0] = static_cast<uint8_t>((ui64Val >> 0ULL) & 0xFF);
#else
								(*reinterpret_cast<uint64_t *>(pui8Target)) = ui64Val;
#endif	// #ifdef LSE_IPHONE
							}
						}
					}
				}
				if ( !pvOut ) { break; }
				++ui32DstY;
			}
			
		}

		return true;
#undef LSI_FAIL
	}

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
	LSBOOL LSE_CALL CImage::ResampleInPlace( uint32_t _ui32NewWidth, uint32_t _ui32NewHeight, LSI_FILTER _fFilter,
		float _fSrcGamma, CResampler::LSI_ADDRESS_MODE _amAddressMode, float _fFilterScale ) {
		if ( _ui32NewWidth == GetWidth() && _ui32NewHeight == GetHeight() ) { return true; }
		CImage iTemp;
		if ( !Resample( _ui32NewWidth, _ui32NewHeight, _fFilter, iTemp, _fSrcGamma, _amAddressMode, _fFilterScale ) ) { return false; }
		(*this) = iTemp;
		return true;
	}

	/**
	 * Calculates and returns the standard 32-bit CRC for this image.
	 *
	 * \return Returns the standard 32-bit CRC for this image.
	 */
	uint32_t LSE_CALL CImage::GetCrc32() const {
		if ( !m_tbBuffer.Length() ) { return LSE_MAXU32; }
		return CCrc::GetCrc( &m_tbBuffer[0], m_tbBuffer.Length() );
	}

	/**
	 * Calculates and returns the alternative 32-bit CRC for this image.
	 *
	 * \return Returns the alternative 32-bit CRC for this image.
	 */
	uint32_t LSE_CALL CImage::GetCrc32Alt() const {
		if ( !m_tbBuffer.Length() ) { return LSE_MAXU32; }
		return CCrc::GetAltCrc( &m_tbBuffer[0], m_tbBuffer.Length() );
	}

	/**
	 * Converts the image to a normal map using average color intensities from each channel of the image.
	 *	Normally images are greyscale when converted to normal maps.
	 *
	 * \param _fStr The strength of the normals.  Must not be 0.
	 * \param _bWrapX If true, the normal map is calculated with wrapping at the horizontal borders.
	 * \param _bWrapY If true, the normal map is calculated with wrapping at the vertical borders.
	 */
	void LSE_CALL CImage::ConvertToNormalMap( float _fStr, LSBOOL _bWrapX, LSBOOL _bWrapY ) {
		assert( _fStr );
		CImage iCopy = (*this);


		CreateBlank( LSI_PF_R8G8B8, GetWidth(), GetHeight() );
		uint32_t ui32TexelWidth = CImageLib::GetFormatSize( GetFormat() );

		float fZ = 1.0f / _fStr;
		if ( GetFormat() == LSI_PF_A8 ) {
			// Convert alpha alone to a height value.
			for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
				uint32_t ui32Up = Y - 1;
				if ( ui32Up > Y ) {
					// ui32Up is less than 0.
					ui32Up = _bWrapY ? GetHeight() - 1 : 0;
				}
				uint32_t ui32Down = Y + 1;
				if ( ui32Down == GetHeight() ) {
					ui32Down = _bWrapY ? 0 : GetHeight() - 1;
				}

				for ( uint32_t X = 0; X < GetWidth(); ++X ) {
					uint32_t ui32Left = X - 1;
					if ( ui32Left > X ) {
						// ui32Up is less than 0.
						ui32Left = _bWrapX ? GetWidth() - 1 : 0;
					}
					uint32_t ui32Right = X + 1;
					if ( ui32Right == GetWidth() ) {
						ui32Right = _bWrapX ? 0 : GetWidth() - 1;
					}

					float fUl = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Up+ui32Left] * (1.0f / 255.0f);
					float fU = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Up+X] * (1.0f / 255.0f);
					float fUr = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Up+ui32Right] * (1.0f / 255.0f);

					float fL = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*Y+ui32Left] * (1.0f / 255.0f);
					float fR = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*Y+ui32Right] * (1.0f / 255.0f);

					float fBl = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Down+ui32Left] * (1.0f / 255.0f);
					float fB = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Down+X] * (1.0f / 255.0f);
					float fBr = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Down+ui32Right] * (1.0f / 255.0f);

					// X is computed with sobel:
					//	1 0 -1 
					//	2 0 -2
					//	1 0 -1
					float fX = fUl + (fL * 2.0f) + fBl - fUr + (fR * -2.0f) - fBr;
					//float fX = -fUl + (fL * -2.0f) - fBl + fUr + (fR * 2.0f) + fBr;

					// Y is computed with sobel:
					//	-1 -2 -1 
					//	 0  0  0
					//	 1  2  1
					//float fY = fUl + (fU * 2.0f) + fUr - fBl + (fB * -2.0f) - fBr;
					float fY = -fUl + (fU * -2.0f) - fUr + fBl + (fB * 2.0f) + fBr;

					// Normalize (manually).
					float fLen = 1.0f / ::sqrtf( fX * fX + fY * fY + fZ * fZ );
					fX *= fLen;
					fY *= fLen;
					float fThisZ = fZ * fLen;

					fX *= 0.5f;
					fX += 0.5f;

					fY *= 0.5f;
					fY += 0.5f;

					fThisZ *= 0.5f;
					fThisZ += 0.5f;

					uint64_t ui64This = 0ULL;
					ui64This |= static_cast<uint64_t>(static_cast<uint8_t>(255.0f - fX * 255.0f)) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_R );
					ui64This |= static_cast<uint64_t>(static_cast<uint8_t>(255.0f - fY * 255.0f)) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_G );
					ui64This |= static_cast<uint64_t>(static_cast<uint8_t>(fThisZ * 255.0f)) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_B );

					(*reinterpret_cast<uint64_t *>(&m_tbBuffer[m_ui32RowWidth*Y+X*ui32TexelWidth])) = ui64This;

				}
			}
		}
		else {
			iCopy.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
			const float * pfSrc = reinterpret_cast<const float *>(iCopy.GetBufferData());
			uint32_t ui32Components = CImageLib::GetTotalComponents( iCopy.GetFormat() );
			uint32_t ui32RowStride = iCopy.m_ui32RowWidth / ui32Components;
			for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
				uint32_t ui32Up = Y - 1;
				if ( ui32Up > Y ) {
					// ui32Up is less than 0.
					ui32Up = _bWrapY ? GetHeight() - 1 : 0;
				}
				ui32Up *= ui32RowStride;
				uint32_t ui32Down = Y + 1;
				if ( ui32Down == GetHeight() ) {
					ui32Down = _bWrapY ? 0 : GetHeight() - 1;
				}
				ui32Down *= ui32RowStride;

				for ( uint32_t X = 0; X < GetWidth(); ++X ) {
					uint32_t ui32Left = X - 1;
					if ( ui32Left > X ) {
						// ui32Left is less than 0.
						ui32Left = _bWrapX ? GetWidth() - 1 : 0;
					}
					uint32_t ui32Right = X + 1;
					if ( ui32Right == GetWidth() ) {
						ui32Right = _bWrapX ? 0 : GetWidth() - 1;
					}

					float fUl = pfSrc[ui32Up+ui32Left*ui32Components];
					float fU = pfSrc[ui32Up+X*ui32Components];
					float fUr = pfSrc[ui32Up+ui32Right*ui32Components];

					float fL = pfSrc[Y*ui32RowStride+ui32Left*ui32Components];
					float fR = pfSrc[Y*ui32RowStride+ui32Right*ui32Components];

					float fBl = pfSrc[ui32Down+ui32Left*ui32Components];
					float fB = pfSrc[ui32Down+X*ui32Components];
					float fBr = pfSrc[ui32Down+ui32Right*ui32Components];


					// X is computed with sobel:
					//	1 0 -1 
					//	2 0 -2
					//	1 0 -1
					float fX = fUl + (fL * 2.0f) + fBl - fUr + (fR * -2.0f) - fBr;
					//float fX = -fUl + (fL * -2.0f) - fBl + fUr + (fR * 2.0f) + fBr;

					// Y is computed with sobel:
					//	-1 -2 -1 
					//	 0  0  0
					//	 1  2  1
					//float fY = fUl + (fU * 2.0f) + fUr - fBl + (fB * -2.0f) - fBr;
					float fY = -fUl + (fU * -2.0f) - fUr + fBl + (fB * 2.0f) + fBr;

					
					// Normalize (manually).
					float fLen = 1.0f / ::sqrtf( fX * fX + fY * fY + fZ * fZ );
					fX *= fLen;
					fY *= fLen;
					float fThisZ = fZ * fLen;

					fX *= 0.5f;
					fX += 0.5f;

					fY *= 0.5f;
					fY += 0.5f;

					fThisZ *= 0.5f;
					fThisZ += 0.5f;

					uint64_t ui64This = 0ULL;
					ui64This |= static_cast<uint64_t>(CStd::Clamp( fX * 255.0f, 0.0f, 255.0f )) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_R );
					ui64This |= static_cast<uint64_t>(CStd::Clamp( fY * 255.0f, 0.0f, 255.0f )) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_G );
					ui64This |= static_cast<uint64_t>(CStd::Clamp( fThisZ * 255.0f, 0.0f, 255.0f )) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_B );

					(*reinterpret_cast<uint64_t *>(&m_tbBuffer[m_ui32RowWidth*Y+X*ui32TexelWidth])) = ui64This;
				}

			}
		}
	}

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
	void LSE_CALL CImage::ConvertToNormalMap( const CKernel &_kU, const CKernel &_kV, float _fStr,
		const CVector4F &_vWeights, CResampler::LSI_ADDRESS_MODE _amAddressMode, LSBOOL _bUseMaxes ) {
		assert( _fStr );
		_fStr = 1.0f / _fStr;
		uint32_t ui32W = GetWidth();
		uint32_t ui32H = GetHeight();
		float fVals[4];
		CVector4F * pvVec = reinterpret_cast<CVector4F *>(fVals);
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			CImage iCopy;
			if ( iCopy.CreateBlank( LSI_PF_R32G32B32A32F, ui32W, ui32H ) ) {
				float * pfStart = static_cast<float *>(const_cast<void *>(iCopy.GetBufferData()));
				for ( uint32_t Y = 0; Y < ui32H; ++Y ) {
					for ( uint32_t X = 0; X < ui32W; ++X ) {
						GetTexelAt( X, Y, fVals, I );
						if ( _bUseMaxes ) {
							pvVec->w = 0.0f;
							pfStart[LSI_PC_R] = (*pvVec).Max();
						}
						else {
							pfStart[LSI_PC_R] = (*pvVec).Dot( _vWeights );
						}
						pfStart += CImageLib::GetTotalComponents( iCopy.GetFormat() );
					}
				}


				for ( uint32_t Y = 0; Y < ui32H; ++Y ) {
					for ( uint32_t X = 0; X < ui32W; ++X ) {
						float fU = iCopy.ApplyKernel( X, Y, _kU, LSI_PC_R, _amAddressMode, 0, ui32W, ui32H );
						float fV = iCopy.ApplyKernel( X, Y, _kV, LSI_PC_R, _amAddressMode, 0, ui32W, ui32H );
						CVector3F vNormalized = CVector3F( fU, fV, _fStr );
						vNormalized.Normalize();
						fVals[LSI_PC_R] = vNormalized.x * 0.5f + 0.5f;
						fVals[LSI_PC_G] = vNormalized.y * 0.5f + 0.5f;
						fVals[LSI_PC_B] = vNormalized.z * 0.5f + 0.5f;
						fVals[LSI_PC_A] = 1.0f;
						SetTexelAt( X, Y, fVals, I );
					}
				}
			}

			ui32W = CStd::Max<uint32_t>( 1, ui32W >> 1 );
			ui32H = CStd::Max<uint32_t>( 1, ui32H >> 1 );
		}
	}

	/**
	 * Converts the image to DXT format.
	 *
	 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
	 * \param _iDst The destination image to which this image is converted.
	 * \param _pdoOptions Conversion options.
	 * \return Returns true if there are no resource problems.
	 */
	LSBOOL LSE_CALL CImage::ConvertToDxt( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDst, const LSI_DXT_OPTIONS * _pdoOptions ) const {
		if ( CImageLib::IsCompressed( GetFormat() ) ) {
			if ( GetFormat() == _pfFormat ) {
				try {
					_iDst = (*this);
					return true;
				}
				catch ( ... ) {
					return false;
				}
			}
			switch ( GetFormat() ) {
				case LSI_PF_DXT1 : {}
				case LSI_PF_DXT2 : {}
				case LSI_PF_DXT3 : {}
				case LSI_PF_DXT4 : {}
				case LSI_PF_DXT5 : {}
				case LSI_PF_BC4 : {}
				case LSI_PF_BC4S : {}
				case LSI_PF_BC5 : {}
				case LSI_PF_BC5S : {}
				case LSI_PF_BC6U : {}
				case LSI_PF_BC6S : {}
				case LSI_PF_BC7U : {}
				case LSI_PF_BC7_SRGB : { break; }
				default : {
					// TODO: Convert to RGBA, then continue.
					return false;
				}
			}
		}
		LSI_DXT_OPTIONS doOptions;
		if ( _pdoOptions ) {
			doOptions = (*_pdoOptions);
		}
		else {
			doOptions.fAlphaThresh = 0.0f;
			doOptions.fRedWeight = LSI_R_WEIGHT;
			doOptions.fGreenWeight = LSI_G_WEIGHT;
			doOptions.fBlueWeight = LSI_B_WEIGHT;
		}
		if ( _pfFormat != LSI_PF_DXT1 ) {
			doOptions.fAlphaThresh = 0.0f;
		}

		// First convert this image to floating-point format.
		CImage iTemp;
		try {
			ConvertToFormat( LSI_PF_R32G32B32A32F, iTemp );

			// We will gather every 4-by-4 block into a linear array.
			//LSI_BLOCK bColors[4][4];
			if ( !_iDst.CreateBlank( _pfFormat, GetWidth(), GetHeight(), TotalMipLevels() ) ) { return false; }
			uint32_t uiW = GetWidth();
			uint32_t uiH = GetHeight();
			for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
				uiW = CStd::Max<uint32_t>( uiW, 1UL );
				uiH = CStd::Max<uint32_t>( uiH, 1UL );

				switch ( _pfFormat ) {
					case LSI_PF_DXT1 : {
						IsSRgb() ?
							CDds::Dxt1FromRgba32F<0, true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Dxt1FromRgba32F<0, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_DXT2 : {
						IsSRgb() ?
							CDds::Dxt2FromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Dxt2FromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_DXT3 : {
						IsSRgb() ?
							CDds::Dxt3FromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Dxt3FromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_DXT4 : {
						IsSRgb() ?
							CDds::Dxt4FromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Dxt4FromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_DXT5 : {
						IsSRgb() ?
							CDds::Dxt5FromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Dxt5FromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC4 : {
						IsSRgb() ?
							CDds::Bc4uFromRgba32F<true, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc4uFromRgba32F<false, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC4S : {
						IsSRgb() ?
							CDds::Bc4sFromRgba32F<true, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc4sFromRgba32F<false, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC5 : {
						IsSRgb() ?
							CDds::Bc5uFromRgba32F<true, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc5uFromRgba32F<false, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC5S : {
						IsSRgb() ?
							CDds::Bc5sFromRgba32F<true, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc5sFromRgba32F<false, false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC6 : {
						IsSRgb() ?
							CDds::Bc6hFromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc6hFromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC6S : {
						IsSRgb() ?
							CDds::Bc6hFromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc6hFromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC7 : {
						IsSRgb() ?
							CDds::Bc7uFromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions ) :
							CDds::Bc7uFromRgba32F<false>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						break;
					}
					case LSI_PF_BC7_SRGB : {
						CDds::Bc7uFromRgba32F<true>( &iTemp.GetMipMapBuffers( I )[0], const_cast<uint8_t *>(&_iDst.GetMipMapBuffers( I )[0]), uiW, uiH, 1, &doOptions );
						iTemp.SetColorSpace( LSI_G_sRGB );
						break;
					}
				}

				uiW >>= 1;
				uiH >>= 1;
			}
		}
		catch ( ... ) {
			return false;
		}
		return true;
#if 0
		// First convert this image to floating-point format.
		CImage iTemp;
		try {
			ConvertToFormat( LSI_PF_R32G32B32A32F, iTemp );

			// Premultiply alpha if necessary.
			switch ( _pfFormat ) {
				case LSI_PF_DXT2 : {}
				case LSI_PF_DXT4 : {
					iTemp.PreMultiplyAlpha();
					break;
				}
				default : {}	// Remove warning.
			}

			// Create X number of threads as we go along the blocks.
			LSH_THREAD tThreads[6];
			for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
				CThreadLib::MakeInvalidHandle( tThreads[I] );
			}
			LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
			for ( uint32_t I = LSE_ELEMENTS( dtdData ); I--; ) {
				dtdData[I].bDone = true;
			}

			// We will gather every 4-by-4 block into a linear array.
			//LSI_BLOCK bColors[4][4];
			if ( !_iDst.CreateBlank( _pfFormat, GetWidth(), GetHeight() ) ) { return false; }

#ifndef LSE_IPHONE
			if ( _pfFormat == LSI_PF_BC6 ) {
				rgba_surface sSurface;
				sSurface.width = GetWidth();
				sSurface.height = GetHeight();
				sSurface.stride = CImageLib::GetRowSize( iTemp.GetFormat(), GetWidth() );
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))
				sSurface.ptr = LSI_CAST( &iTemp.m_tbBuffer[0] );

				bc6h_enc_settings besSettings;
				::GetProfile_bc6h_veryslow( &besSettings );
				// Have to convert to 16-bit floats.
				CVectorPoD<float, uint32_t> vHalfBuffer;
				if ( !vHalfBuffer.Resize( GetWidth() * GetHeight() * 4 ) ) { return false; }
				
				CFloat16 fZero = 0.0f;
				for ( uint32_t Y = GetHeight(); Y--; ) {
					uint32_t ui32Row = Y * sSurface.stride;
					uint32_t ui32DstRow = Y * GetWidth() * 4;
					for ( uint32_t X = GetWidth(); X--; ) {
						uint32_t ui32Index = ui32Row + X * sizeof( float ) * 4;
						CFloat16 fR = reinterpret_cast<float *>(&sSurface.ptr[ui32Index])[LSI_PC_R];
						CFloat16 fG = reinterpret_cast<float *>(&sSurface.ptr[ui32Index])[LSI_PC_G];
						CFloat16 fB = reinterpret_cast<float *>(&sSurface.ptr[ui32Index])[LSI_PC_B];
						vHalfBuffer[ui32DstRow+X*4+0] = fR.GetInt();
						vHalfBuffer[ui32DstRow+X*4+1] = fG.GetInt();
						vHalfBuffer[ui32DstRow+X*4+2] = fB.GetInt();
						vHalfBuffer[ui32DstRow+X*4+3] = fZero.GetInt();
					}
				}
				sSurface.stride = GetWidth() * 4 * sizeof( vHalfBuffer[0] );
				sSurface.ptr = LSI_CAST( &vHalfBuffer[0] );

				CompressBlocksBC6H( &sSurface, LSI_CAST( &_iDst.m_tbBuffer[0] ), &besSettings );
				return true;
#undef LSI_CAST
			}
			if ( _pfFormat == LSI_PF_BC7 ) {
				rgba_surface sSurface;
				sSurface.width = GetWidth();
				sSurface.height = GetHeight();
				sSurface.stride = CImageLib::GetRowSize( iTemp.GetFormat(), GetWidth() );
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))
				sSurface.ptr = LSI_CAST( &iTemp.m_tbBuffer[0] );

				bc7_enc_settings besSettings;
				::GetProfile_alpha_slow( &besSettings );

				CompressBlocksBC7( &sSurface, LSI_CAST( &_iDst.m_tbBuffer[0] ), &besSettings );
				return true;
#undef LSI_CAST
			}
#endif	// #ifndef LSE_IPHONE

#if 0
			if ( _pfFormat == LSI_PF_BC1 ) {
				rgba_surface sSurface;
				sSurface.width = GetWidth();
				sSurface.height = GetHeight();
				sSurface.stride = CImageLib::GetRowSize( iTemp.GetFormat(), GetWidth() );
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))
				sSurface.ptr = LSI_CAST( &iTemp.m_tbBuffer[0] );

				CompressBlocksBC1( &sSurface, LSI_CAST( &_iDst.m_tbBuffer[0] ) );
				return true;
#undef LSI_CAST
			}

			if ( _pfFormat == LSI_PF_BC3 ) {
				rgba_surface sSurface;
				sSurface.width = GetWidth();
				sSurface.height = GetHeight();
				sSurface.stride = CImageLib::GetRowSize( iTemp.GetFormat(), GetWidth() );
#define LSI_CAST( VAL )		const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(VAL))
				sSurface.ptr = LSI_CAST( &iTemp.m_tbBuffer[0] );

				CompressBlocksBC3( &sSurface, LSI_CAST( &_iDst.m_tbBuffer[0] ) );
				return true;
#undef LSI_CAST
			}
#endif	// #if 0


			// Number of blocks along the width.
			uint32_t ui32Width = (GetWidth() + 3) >> 2;
			// Number of blocks along the height.
			uint32_t ui32TotalScanLines = (GetHeight() + 3) >> 2;
			
#define LSI_DXT_ROWS	12
			uint8_t * pui8Dst = &_iDst.m_tbBuffer[0];
			/*uint32_t ui32SrcStride = CImageLib::GetRowSize( iTemp.GetFormat(), iTemp.GetWidth() );
			const uint8_t * pui8Src = &iTemp.m_tbBuffer[0];*/
			for ( uint32_t Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {					
				// Find a free thread.
				for ( uint32_t I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
					I %= LSE_ELEMENTS( tThreads );
					if ( dtdData[I].bDone ) {
						// Thread stopped or never started.
						CThreadLib::CloseThread( tThreads[I] );
						
						dtdData[I].bDone = false;
						dtdData[I].ui32SrcX = 0;
						dtdData[I].ui32SrcY = Y;
						dtdData[I].pui8Dest = pui8Dst;
						dtdData[I].pfFormat = _pfFormat;
						dtdData[I].piSrcImage = &iTemp;
						dtdData[I].pdoOptions = &doOptions;
						if ( !CThreadLib::CreateThread( DxtThread, &dtdData[I], &tThreads[I] ) ) {
							for ( I = LSE_ELEMENTS( tThreads ); I--; ) {
								CThreadLib::WaitForThread( tThreads[I], NULL );
								CThreadLib::CloseThread( tThreads[I] );
							}
							return false;
						}
						break;
					}
					CStd::Sleep( 10 );
				}

				pui8Dst += CDds::DxtBlockSize( _pfFormat ) * ui32Width * LSI_DXT_ROWS;
			}

			for ( uint32_t I = LSE_ELEMENTS( tThreads ); I--; ) {
				CThreadLib::WaitForThread( tThreads[I], NULL );
				CThreadLib::CloseThread( tThreads[I] );
			}
		}
		catch ( ... ) {
			return false;
		}
		return true;
#endif
	}

	/**
	 * Converts the image to DXT format in-place.
	 *
	 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
	 * \param _pdoOptions Conversion options.
	 * \return Returns true if there are no resource problems.
	 */
	LSBOOL LSE_CALL CImage::ConvertToDxtInPlace( LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS * _pdoOptions ) {
		try {
			CImage iCopy = (*this);
			return iCopy.ConvertToDxt( _pfFormat, (*this), _pdoOptions );
		}
		catch ( ... ) {
			return false;
		}
	}

	/**
	 * Converts the image to KTX 1 format.
	 *
	 * \param _kifFormat The OpenGL internal format to which to convert.
	 * \param _iDst The destination image to which this image is converted.
	 * \param _pvParm Optional parameters to pass to the conversion routine.
	 * \return Returns true if there are no resource problems and the conversion is supported.
	 */
	LSBOOL LSE_CALL CImage::ConvertToKtx1( LSI_KTX_INTERNAL_FORMAT _kifFormat, CImage &_iDst, void * _pvParm ) const {
		// If already in the target format then just copy.
		if ( GetFormat() == LSI_PF_KTX1 && m_kvKtx1.kifGlInternalFormat == _kifFormat ) {
			try {
				_iDst = (*this);
				return true;
			}
			catch ( ... ) {
				return false;
			}
		}

		const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdData = CKtx::FindInternalFormatData( _kifFormat );
		if ( !pkifdData ) { return false; }
		if ( !pkifdData->pfFromRgba32F ) { return false; }

		if ( !_iDst.CreateBlankKtx1( _kifFormat, GetWidth(), GetHeight(), TotalMipLevels() ) ) { return false; }
		// First convert this image to floating-point format.
		CImage iTemp;
		try {
			ConvertToFormat( LSI_PF_R32G32B32A32F, iTemp );
			iTemp.SetColorSpace( pkifdData->bSrgb ? LSI_G_sRGB : LSI_G_LINEAR );
			// Convert each mipmap from RGBA32F to the destiation format.
			uint32_t ui32H = m_ui32Height;
			uint32_t ui32W = m_ui32Width;
			for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
				CImageLib::CTexelBuffer & tbSrc = const_cast<CImageLib::CTexelBuffer &>(iTemp.GetMipMapBuffers( I ));
				CImageLib::CTexelBuffer & tbDst = const_cast<CImageLib::CTexelBuffer &>(_iDst.GetMipMapBuffers( I ));
				if ( !pkifdData->pfFromRgba32F( &tbSrc[0], &tbDst[0], ui32W, ui32H, 1, _pvParm ) ) { return false; }
				ui32H = CStd::Max<uint32_t>( ui32H >> 1, 1 );
				ui32W = CStd::Max<uint32_t>( ui32W >> 1, 1 );
			}
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

	/**
	 * Converts the image to KTX 1 format.
	 *
	 * \param _kifFormat The OpenGL internal format to which to convert.
	 * \param _pvParm Optional parameters to pass to the conversion routine.
	 * \return Returns true if there are no resource problems and the conversion is supported.
	 */
	LSBOOL LSE_CALL CImage::ConvertToKtx1InPlace( LSI_KTX_INTERNAL_FORMAT _kifFormat, void * _pvParm ) {
		try {
			CImage iCopy = (*this);
			return iCopy.ConvertToKtx1( _kifFormat, (*this), _pvParm );
		}
		catch ( ... ) {
			return false;
		}
	}

	/**
	 * Pre-multiplies the alpha in-place.  This should not be done more than once on a single image for correct results.
	 */
	void LSE_CALL CImage::PreMultiplyAlpha() {
		float fVal[4];
		uint32_t ui32Width = GetWidth();
		uint32_t ui32Height = GetHeight();
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			ui32Width = CStd::Max<uint32_t>( GetWidth() >> I, 1 );
			ui32Height = CStd::Max<uint32_t>( GetHeight() >> I, 1 );
			for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < ui32Width; ++X ) {
					GetTexelAt( X, Y, fVal, I );
					if ( IsSRgb() ) {
						fVal[LSI_PC_R] = CImageLib::LinearToSRgb( CImageLib::SRgbToLinear( fVal[LSI_PC_R] ) * fVal[LSI_PC_A] );
						fVal[LSI_PC_G] = CImageLib::LinearToSRgb( CImageLib::SRgbToLinear( fVal[LSI_PC_G] ) * fVal[LSI_PC_A] );
						fVal[LSI_PC_B] = CImageLib::LinearToSRgb( CImageLib::SRgbToLinear( fVal[LSI_PC_B] ) * fVal[LSI_PC_A] );
					}
					else {
						fVal[LSI_PC_R] *= fVal[LSI_PC_A];
						fVal[LSI_PC_G] *= fVal[LSI_PC_A];
						fVal[LSI_PC_B] *= fVal[LSI_PC_A];
					}
					SetTexelAt( X, Y, fVal, I );
				}
			}
		}
	}

	/**
	 * Increases the canvas area of the texture without resizing the image.  The additional space around the image is filled in with
	 *	a texture-mapping determined by _amAddressModeU and _amAddressModeV.
	 * 
	 * \param _iDest The destination texture.
	 * \param _ui32SizeFactorX The width multiplier of the new texture.
	 * \param _ui32SizeFactorY The height multiplier of the new texture.
	 * \param _amAddressModeU Addressing mode for the U texture coordinates.
	 * \param _amAddressModeV Addressing mode for the V texture coordinates.
	 * \return DESC
	 **/
	LSBOOL LSE_CALL CImage::BakeTextureMapping( CImage &_iDest,
		uint32_t _ui32SizeFactorX, uint32_t _ui32SizeFactorY,
		CResampler::LSI_ADDRESS_MODE _amAddressModeU,
		CResampler::LSI_ADDRESS_MODE _amAddressModeV ) const {

		// If resizing is necessary then the texture must not be a DXT format.
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return false; }

#ifdef _DEBUG
#define LSI_FAIL( STRING )			CStd::DebugPrintA( "CImage::BakeTextureMapping(): " STRING "\r\n" ); return false
#else
#define LSI_FAIL( STRING )			return false
#endif	// #ifdef _DEBUG


		if ( !_iDest.CreateBlank( GetFormat(), GetWidth() * _ui32SizeFactorX, GetHeight() * _ui32SizeFactorY,
			TotalMipLevels(), GetColorSpace() ) ) {
			LSI_FAIL( "Out of memory while baking texture addressing." );
		}


		float fVal[4];
		uint32_t ui32Width = 0;
		uint32_t ui32Height = 0;
		uint32_t ui32SrcWidth = 0;
		uint32_t ui32SrcHeight = 0;
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			ui32SrcWidth = CStd::Max<uint32_t>( GetWidth() >> I, 1 );
			ui32SrcHeight = CStd::Max<uint32_t>( GetHeight() >> I, 1 );
			ui32Width = CStd::Max<uint32_t>( (GetWidth() * _ui32SizeFactorX) >> I, 1 );
			ui32Height = CStd::Max<uint32_t>( (GetHeight() * _ui32SizeFactorY) >> I, 1 );

			
			for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
				int32_t i32SrcV = 0;
				switch ( _amAddressModeV ) {
					case CResampler::LSI_AM_CLAMP : {
						i32SrcV = CStd::Clamp( int32_t( Y ) - int32_t( ui32Height - ui32SrcHeight ) / 2, 0, int32_t( ui32SrcHeight ) - 1 );
						break;
					}
					case CResampler::LSI_AM_REPEAT : {
						i32SrcV = int32_t( Y % ui32SrcHeight );
						break;
					}
					case CResampler::LSI_AM_MIRROR : {
						i32SrcV = int32_t( ((Y / ui32SrcHeight) & 1) ? (ui32SrcHeight - 1 - (Y % ui32SrcHeight)) : (Y % ui32SrcHeight) );
						break;
					}
				}
				for ( uint32_t X = 0; X < ui32Width; ++X ) {
					int32_t i32SrcU = 0;
					switch ( _amAddressModeU ) {
						case CResampler::LSI_AM_CLAMP : {
							i32SrcU = CStd::Clamp( int32_t( X ) - int32_t( ui32Width - ui32SrcWidth ) / 2, 0, int32_t( ui32SrcWidth ) - 1 );
							break;
						}
						case CResampler::LSI_AM_REPEAT : {
							i32SrcU = int32_t( X % ui32SrcWidth );
							break;
						}
						case CResampler::LSI_AM_MIRROR : {
							i32SrcU = int32_t( ((X / ui32SrcWidth) & 1) ? (ui32SrcWidth - 1 - (X % ui32SrcWidth)) : (X % ui32SrcWidth) );
							break;
						}
					}

					GetTexelAt( i32SrcU, i32SrcV, fVal, I );
					_iDest.SetTexelAt( X, Y, fVal, I );
				}
			}
		}
		return true;
	}

	/**
	 * Increases the canvas area of the texture without resizing the image (in-place).  The additional space around the image is filled in with
	 *	a texture-mapping determined by _amAddressModeU and _amAddressModeV.
	 * 
	 * \param _ui32SizeFactorX The width multiplier of the new texture.
	 * \param _ui32SizeFactorY The height multiplier of the new texture.
	 * \param _amAddressModeU Addressing mode for the U texture coordinates.
	 * \param _amAddressModeV Addressing mode for the V texture coordinates.
	 * \return DESC
	 **/
	LSBOOL LSE_CALL CImage::BakeTextureMappingInPlace( uint32_t _ui32SizeFactorX, uint32_t _ui32SizeFactorY,
		CResampler::LSI_ADDRESS_MODE _amAddressModeU,
		CResampler::LSI_ADDRESS_MODE _amAddressModeV ) {

		CImage iTemp;
		if ( !BakeTextureMapping( iTemp, _ui32SizeFactorX, _ui32SizeFactorY, _amAddressModeU, _amAddressModeV ) ) { return false; }
		(*this) = iTemp;
		return true;
	}

	/**
	 * Adds a normal-map channel to the Z component.
	 */
	void LSE_CALL CImage::AddNormalMapZ() {
		float fVal[4];
		uint32_t ui32Width = GetWidth();
		uint32_t ui32Height = GetHeight();
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			ui32Width = CStd::Max<uint32_t>( GetWidth() >> I, 1 );
			ui32Height = CStd::Max<uint32_t>( GetHeight() >> I, 1 );
			for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < ui32Width; ++X ) {
					GetTexelAt( X, Y, fVal, I );
					float fX = fVal[LSI_PC_R] * 2.0f - 1.0f;
					float fY = fVal[LSI_PC_G] * 2.0f - 1.0f;
					float fTemp = 1.0f - ((fX * fX) + (fY * fY));
					if ( fTemp ) {
						fTemp = ::sqrtf( fTemp );
					}
					fVal[LSI_PC_B] = fTemp;
					SetTexelAt( X, Y, fVal, I );
				}
			}
		}
	}

	/**
	 * Normalizes the image in-place.  This should be done for normal maps.
	 */
	void LSE_CALL CImage::Normalize() {
		float fVal[4];
		uint32_t ui32Width = GetWidth();
		uint32_t ui32Height = GetHeight();
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			ui32Width = CStd::Max<uint32_t>( GetWidth() >> I, 1 );
			ui32Height = CStd::Max<uint32_t>( GetHeight() >> I, 1 );
			for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < ui32Width; ++X ) {
					GetTexelAt( X, Y, fVal, I );
					float fX = fVal[LSI_PC_R] * 2.0f - 1.0f;
					float fY = fVal[LSI_PC_G] * 2.0f - 1.0f;
					float fZ = fVal[LSI_PC_B] * 2.0f - 1.0f;
					float fThisVal = fX * fX +
						fY * fY +
						fZ * fZ;
					if ( fThisVal ) {
						fThisVal = 1.0f / ::sqrtf( fThisVal );
						fVal[LSI_PC_R] = ((fX * fThisVal) + 1.0f) * 0.5f;
						fVal[LSI_PC_G] = ((fY * fThisVal) + 1.0f) * 0.5f;
						fVal[LSI_PC_B] = ((fZ * fThisVal) + 1.0f) * 0.5f;
						SetTexelAt( X, Y, fVal, I );
					}
				}
			}
		}
	}

	/**
	 * Is this image black-and-white?
	 *
	 * \return Returns true if the image is in format LSI_PF_A8, or if it is in formats LSI_PF_R4G4B4A4, LSI_PF_R5G5B5A1,
	 *	LSI_PF_R8G8B8, LSI_PF_R8G8B8A8, LSI_PF_R16G16B16A16, LSI_PF_R16G16B16A16F, or LSI_PF_R32G32B32A32F, and the R, G,
	 *	and B channels are all the same.
	 */
	LSBOOL LSE_CALL CImage::IsBlackAndWhite() const {
		switch ( GetFormat() ) {
			case LSI_PF_A8 : { return true; }
			case LSI_PF_DXT1 : {}
			case LSI_PF_DXT2 : {}
			case LSI_PF_DXT3 : {}
			case LSI_PF_DXT4 : {}
			case LSI_PF_DXT5 : {}
			case LSI_PF_R3G3B2 : {}
			case LSI_PF_R5G6B5 : { return false; }
			default : {}
		}
		if ( GetFormat() < LSI_PF_TOTAL_INT ) {
			uint64_t ui64RedMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_R )) - 1ULL;
			uint64_t ui64GreenMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_G )) - 1ULL;
			uint64_t ui64BlueMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_B )) - 1ULL;
			// Integer format.
			for ( uint32_t Y = GetHeight(); Y--; ) {
				for ( uint32_t X = GetWidth(); X--; ) {
					uint64_t ui64This = GetTexelAt( LSI_PF_R8G8B8, X, Y );
					uint64_t ui64Red = ui64This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_R );
					ui64Red &= ui64RedMask;
					uint64_t ui64Green = ui64This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_G );
					ui64Green &= ui64GreenMask;
					uint64_t ui64Blue = ui64This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_B );
					ui64Blue &= ui64BlueMask;

					uint64_t ui64Avg = (ui64Red + ui64Green + ui64Blue) / 3ULL;

					int64_t i64Dif = static_cast<int64_t>(ui64Red) - static_cast<int64_t>(ui64Avg);
					if ( i64Dif >= 8LL || i64Dif <= -8LL ) {
						return false;
					}
					i64Dif = static_cast<int64_t>(ui64Green) - static_cast<int64_t>(ui64Avg);
					if ( i64Dif >= 8LL || i64Dif <= -8LL ) {
						return false;
					}
					i64Dif = static_cast<int64_t>(ui64Blue) - static_cast<int64_t>(ui64Avg);
					if ( i64Dif >= 8LL || i64Dif <= -8LL ) {
						return false;
					}
				}
			}
		}
		else {
			CImage iCopy;
			const CImage * piUseMe = this;
			if ( GetFormat() == LSI_PF_R16G16B16A16F ) {
				iCopy = (*this);
				iCopy.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
				piUseMe = &iCopy;
			}

			
			for ( uint32_t Y = GetHeight(); Y--; ) {
				uint32_t ui32Y = Y * piUseMe->m_ui32RowWidth;
				for ( uint32_t X = GetWidth(); X--; ) {
					const float * pfThis = reinterpret_cast<const float *>(&reinterpret_cast<const uint8_t *>(piUseMe->GetBufferData())[ui32Y+X*piUseMe->m_ui32TexelSize]);
					if ( pfThis[LSI_PC_R] != pfThis[LSI_PC_G] || pfThis[LSI_PC_R] != pfThis[LSI_PC_B] ) { return false; }
				}
			}
		}

		return true;
	}

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
	float LSE_CALL CImage::ApplyKernel( uint32_t _ui32X, uint32_t _ui32Y, const CKernel &_kKernel,
		uint32_t _ui32Channel, CResampler::LSI_ADDRESS_MODE _amAddressMode, uint32_t _ui32Mip ) const {
		uint32_t ui32W = GetWidth();
		uint32_t ui32H = GetHeight();
		for ( uint32_t I = 0; I < _ui32Mip; ++I ) {
			ui32W >>= 1;
			ui32H >>= 1;
		}
		ui32W = CStd::Max<uint32_t>( ui32W, 1 );
		ui32H = CStd::Max<uint32_t>( ui32H, 1 );

		return ApplyKernel( _ui32X, _ui32Y, _kKernel,
			_ui32Channel, _amAddressMode, _ui32Mip, ui32W, ui32H );
	}

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
	float LSE_CALL CImage::ApplyKernel( uint32_t _ui32X, uint32_t _ui32Y, const CKernel &_kKernel,
		uint32_t _ui32Channel, CResampler::LSI_ADDRESS_MODE _amAddressMode, uint32_t _ui32Mip,
		uint32_t _ui32Width, uint32_t _ui32Height ) const {
		float fSum = 0.0f;
		float fColors[4];

		const uint32_t ui32Size = _kKernel.Size();
		const int32_t i32KernelOffset = static_cast<int32_t>(ui32Size >> 1) - 1;
		for ( uint32_t Y = 0; Y < ui32Size; ++Y ) {
			int32_t i32SrcY = static_cast<int32_t>(_ui32Y + Y) - i32KernelOffset;
			i32SrcY = CResampler::ClampTexel( i32SrcY, _ui32Height, _amAddressMode );
			for ( uint32_t X = 0; X < ui32Size; ++X ) {
				int32_t i32SrcX = static_cast<int32_t>(_ui32X + X) - i32KernelOffset;
				i32SrcX = CResampler::ClampTexel( i32SrcX, _ui32Width, _amAddressMode );
				GetTexelAt( i32SrcX, i32SrcY, fColors, _ui32Mip );
				float fVal = fColors[_ui32Channel];
				fSum += _kKernel[Y][X] * fVal;
			}
		}
		return fSum;
	}

	/**
	 * Called after a successful load.  Sets various values based on the loaded data.
	 */
	void LSE_CALL CImage::PostLoad() {
		if ( m_pfFormat == LSI_PF_KTX1 ) {
			const CKtx::LSI_KTX_INTERNAL_FORMAT_DATA * pkifdData = CKtx::FindInternalFormatData( m_kvKtx1.kifGlInternalFormat );
			m_ui32RowWidth = m_tbBuffer.Length() / m_ui32Height;
			m_ui32TexelSize = pkifdData ? ((pkifdData->ui32BlockSizeInBits + 7) / 8) : 0;
			if ( pkifdData && pkifdData->bSrgb ) {
				m_gColorSpace = LSI_G_sRGB;
			}
			else { m_gColorSpace = LSI_G_LINEAR; }
			m_bAutoGamma = true;
		}
		else if ( m_pfFormat == LSI_PF_KTX2 ) {
		}
		else if ( CImageLib::IsCompressed( m_pfFormat ) ) {
			m_ui32RowWidth = CDds::DxtScanlineWidth( m_ui32Width, m_pfFormat );
			m_ui32TexelSize = CDds::DxtScanlineWidth( 1, m_pfFormat );
		}
		else {
			m_ui32RowWidth = CImageLib::GetRowSize( m_pfFormat, m_ui32Width );
			m_ui32TexelSize = CImageLib::GetFormatSize( m_pfFormat );
		}
	}

	/**
	 * Compress this texture by grouping the reds, greens, blues, and alphas together
	 *	and compressing them separately.
	 *
	 * \param _bBuffer The output buffer where the compressed data will be stored.
	 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CImage::CompressChannels( CMisc::LSC_BUFFER &_bBuffer, uint32_t _ui32MaxSize ) {
		lsc::CBitStream bsTemp;

		const static uint32_t ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			bsTemp.Reset();
			uint32_t ui32Comp = CImageLib::GetComponentBits( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			uint32_t ui32Shift = CImageLib::GetComponentOffset( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
					for ( uint32_t X = 0; X < GetWidth(); ++X ) {
						uint64_t ui64Texel = GetTexelAt( GetFormat(), X, Y ) >> ui32Shift;
						if ( !bsTemp.WriteBits( reinterpret_cast<uint8_t *>(&ui64Texel), ui32Comp ) ) { return false; }
					}
				}

				// Compress the buffer.
				if ( !CMisc::CompressToBuffer( bsTemp.GetRawBytes(), bsTemp.GetLengthInBytes(), _bBuffer ) ) { return false; }
				if ( _bBuffer.Length() > _ui32MaxSize ) { return false; }
			}
		}

		return true;
	}

	/**
	 * Compress this texture by just performing a raw compression of the data.
	 *
	 * \param _bBuffer The output buffer where the compressed data will be stored.
	 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CImage::CompressRaw( CMisc::LSC_BUFFER &_bBuffer, uint32_t /*_ui32MaxSize*/ ) {
		if ( !m_tbBuffer.Length() ) { return true; }
		// Compress each mipmap level.
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
#if 0
			if ( !CMisc::CompressToBuffer( reinterpret_cast<const uint8_t *>(&GetMipMapBuffers( I )[0]), GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
#else
			if ( !CMisc::WriteUInt( GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
			if ( !CMisc::WriteBytes( reinterpret_cast<const uint8_t *>(&GetMipMapBuffers( I )[0]), GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
#endif	// #if 0
		}
		return true;
	}

	/**
	 * Compress this texture by using RLE encoding.  The RLE data is also compressed.
	 *
	 * \param _bBuffer The output buffer where the compressed data will be stored.
	 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CImage::CompressRle( CMisc::LSC_BUFFER &_bBuffer, uint32_t /*_ui32MaxSize*/ ) {
		lsc::CBitStream bsTemp;
		const static uint32_t ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		bsTemp.Reset();
		LSI_PIXEL_FORMAT pfFormat = GetFormat();
		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			uint32_t ui32Comp = CImageLib::GetComponentBits( pfFormat, static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				uint32_t ui32Shift = CImageLib::GetComponentOffset( pfFormat, static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
				for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
					for ( uint32_t X = 0; X < GetWidth(); ++X ) {
						uint64_t ui64Texel = GetTexelAt( pfFormat, X, Y ) >> ui32Shift;
						ui64Texel &= (1ULL << ui32Comp) - 1ULL;
						// Now we have just the component.
						//	Count how many follow that are the same.
						uint32_t ui32Count = 0;
						
						for ( uint32_t J = X + 1; J < GetWidth(); ++J ) {
							uint64_t ui64NewTexel = GetTexelAt( pfFormat, J, Y ) >> ui32Shift;
							ui64NewTexel &= (1ULL << ui32Comp) - 1ULL;
							if ( ui64NewTexel != ui64Texel ) { break; }
							++ui32Count;
							if ( ui32Count == 255UL ) { break; }
						}
						if ( ui32Comp >= 32 ) {
							if ( !bsTemp.WriteBytes( reinterpret_cast<uint8_t *>(&ui64Texel), 4 ) ) { return false; }
						}
						if ( ui32Comp >= 24UL ) {
							if ( !bsTemp.WriteBytes( reinterpret_cast<uint8_t *>(&ui64Texel), 3 ) ) { return false; }
						}
						else if ( ui32Comp >= 16 ) {
							if ( !bsTemp.WriteBytes( reinterpret_cast<uint8_t *>(&ui64Texel), 2 ) ) { return false; }
						}
						else {
							if ( !bsTemp.WriteBytes( reinterpret_cast<uint8_t *>(&ui64Texel), 1 ) ) { return false; }
						}
						if ( !bsTemp.WriteBytes( reinterpret_cast<uint8_t *>(&ui32Count), 1 ) ) { return false; }
						X += ui32Count;
					}
				}
			}
		}

		return CMisc::CompressToBuffer( bsTemp.GetRawBytes(), bsTemp.GetLengthInBytes(), _bBuffer );
	}

	/**
	 * Compress this texture by creating indexed data.  The index table and indexed data are
	 *	then further compressed.
	 *
	 * \param _bBuffer The output buffer where the compressed data will be stored.
	 * \param _ui32MaxSize Buffer Size after which compression automatically fails.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CImage::CompressIndex( CMisc::LSC_BUFFER &_bBuffer, uint32_t _ui32MaxSize ) {
		lsc::CBitStream bsTable, bsIndices;
		// First build the table.  This has to be done in a separate pass to allow us to determine how many bits are
		//	needed to store the indices.
		uint32_t ui32IndexSize = 0, ui32TableSize = 0, ui32FormatSize = CImageLib::GetFormatSize( GetFormat() );

		CSet<LSI_SORTED_PIXEL> spSorted;
		CSet<LSI_SORTED_PIXEL> spSortedHash[1024];
		LSI_SORTED_PIXEL spThis;
		spThis.ui32Length = ui32FormatSize;
		
		for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < GetWidth(); ++X ) {
				spThis.u.ui64Rgb64 = GetTexelAt( GetFormat(), X, Y );
				uint32_t ui32R = static_cast<uint32_t>(LSI_R( CImageLib::IntegerTexelToIntegerTexel( GetFormat(), LSI_PF_R8G8B8A8, spThis.u.ui64Rgb64, LSI_G_NONE ) ));
				uint32_t ui32G = static_cast<uint32_t>(LSI_G( CImageLib::IntegerTexelToIntegerTexel( GetFormat(), LSI_PF_R8G8B8A8, spThis.u.ui64Rgb64, LSI_G_NONE ) ));
				ui32R = ui32R & ((ui32G & 3) << 8);
				// Already in the table?
				if ( !spSortedHash[ui32R].Insert( spThis ) ) { return false; }
			}
		}

		// Combine the hashes.
		for ( uint32_t I = LSE_ELEMENTS( spSortedHash ); I--; ) {
			ui32TableSize += spSortedHash[I].Length();
		}
		spSorted.AllocateAtLeast( ui32TableSize );
		for ( uint32_t I = LSE_ELEMENTS( spSortedHash ); I--; ) {
			for ( uint32_t J = spSortedHash[I].Length(); J--; ) {
				if ( !spSorted.Insert( spSortedHash[I].GetByIndex( J ) ) ) { return false; }
			}
			spSortedHash[I].Reset();
		}

		ui32TableSize = spSorted.Length();
		for ( uint32_t I = 0; I < ui32TableSize; ++I ) {
			if ( !bsTable.WriteBytes( spSorted.GetByIndex( I ).u.ui8Rgb, ui32FormatSize ) ) { return false; }
		}

		// How many bits per index are needed to index fully into the table?
		// The highest index value will be table size - 1, so check using that number.
		ui32IndexSize = CStd::HighestBit( static_cast<uint32_t>(ui32TableSize - 1) ) + 1;

		for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < GetWidth(); ++X ) {
				spThis.u.ui64Rgb64 = GetTexelAt( GetFormat(), X, Y );
				// Already in the table?
				uint32_t I = 0;
				if ( !spSorted.GetItemIndex( spThis, I ) ) { return false; }
				if ( !bsIndices.WriteBits( reinterpret_cast<uint8_t *>(&I), ui32IndexSize ) ) { return false; }
			}
		}

		// Compress the table.
		CMisc::LSC_BUFFER bTable;
		if ( !bTable.Append( reinterpret_cast<uint8_t *>(&ui32TableSize), sizeof( ui32TableSize ) ) ) { return false; }
		if ( !bTable.Append( bsTable.GetRawBytes(), bsTable.GetLengthInBytes() ) ) { return false; }
		if ( !CMisc::CompressToBuffer( &bTable[0], bTable.Length(), _bBuffer ) ) { return false; }
		if ( _bBuffer.Length() >= _ui32MaxSize ) { return false; }	// Early out.
		// And the indices.
		if ( !CMisc::CompressToBuffer( bsIndices.GetRawBytes(), bsIndices.GetLengthInBytes(), _bBuffer ) ) { return false; }

		return true;
	}

	/**
	 * Compress an image that is already in DXT format.  Simply uses LZW on the compressed data.
	 *
	 * \param _bBuffer The output buffer where the compressed data will be stored.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CImage::CompressDxt( CMisc::LSC_BUFFER &_bBuffer ) {
		if ( !m_tbBuffer.Length() ) { return true; }
		// Compress each mipmap level.
		for ( uint32_t I = 0; I < TotalMipLevels(); ++I ) {
			if ( !CMisc::CompressToBuffer( reinterpret_cast<const uint8_t *>(&GetMipMapBuffers( I )[0]), GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
		}
		return true;
	}

	/**
	 * Loads an LSI image that was compressed via channel compression.
	 *
	 * \param _bBuffer Buffer from which to load the image data.
	 * \param _tbOut Buffer where to store the decompressed image data.
	 * \return Returns true if there are no memory failures and the given data is valid. 
	 */
	LSBOOL LSE_CALL CImage::LoadCompressedChannels( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut ) {
		if ( !_tbOut.Length() ) { return false; }
		CStd::MemSet( &_tbOut[0], 0, _tbOut.Length() );
		if ( _ui32DataLen == 0 ) { return false; }
		lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;

		const static uint32_t ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			//bsTemp.Reset();
			uint32_t ui32Comp = CImageLib::GetComponentBits( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				bTemp.ResetNoDealloc();
				if ( !CMisc::DecompressToBuffer( bsTemp, bTemp ) ) { return false; }
				if ( bTemp.Length() == 0 && GetHeight() * GetWidth() != 0 ) { return false; }
				if ( bTemp.Length() == 0 ) { continue; }
				lsc::CBitStream bsThisLine;
				bsThisLine.Borrow( &bTemp[0], bTemp.Length() );
				for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
					for ( uint32_t X = 0; X < GetWidth(); ++X ) {
						uint64_t ui64ThisTexel = 0;
						if ( !bsThisLine.ReadBits( reinterpret_cast<uint8_t *>(&ui64ThisTexel), ui32Comp ) ) { return false; }

						// The image buffer has been filled with 0's so we can just | our new component into place.
						ui64ThisTexel <<= CImageLib::GetComponentOffset( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );

						(*reinterpret_cast<uint64_t *>(&_tbOut[Y*m_ui32RowWidth+X*m_ui32TexelSize])) |= ui64ThisTexel;
					}
				}
			}
		}

		return true;
	}

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
	LSBOOL LSE_CALL CImage::LoadCompressedRaw( uint8_t _ui8MipLevels, const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {
		if ( _ui8MipLevels == 0 ) { return false; }
		//if ( !_tbOut.Length() ) { return false; }
		if ( _ui32DataLen == 0 ) {
			/*if ( _ui8MipLevels == 1 && (!m_ui32Width || !m_ui32Height) ) {
				// If only 1 level and no width or no height, 0 is valid.
				return true;
			}*/
			return false;
		}
		// First level is manual.
		CExternalByteStream ebsStream( static_cast<const uint8_t *>(_pui8FileData), _ui32DataLen );
#if 0
		CMisc::LSC_BUFFER bTemp;
		if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }
		if ( !_tbOut.Resize( bTemp.Length() ) ) { return false; }
		CStd::MemCpy( &_tbOut[0], &bTemp[0], bTemp.Length() );
#else
		uint32_t ui32Size = ebsStream.ReadUInt32();
		if ( !ui32Size || ui32Size > ebsStream.Length() - ebsStream.GetPos() ) { return false; }
		if ( !_tbOut.Resize( ui32Size ) ) { return false; }
		CStd::MemCpy( &_tbOut[0], ebsStream.GetCurBufferPointer(), ui32Size );
		ebsStream.SkipBytes( ui32Size );
#endif	// #if 0

		// The mipmaps.
		--_ui8MipLevels;
		if ( !_vMipMaps.Resize( _ui8MipLevels ) ) { return false; }
		for ( uint32_t I = 0; I < _ui8MipLevels; ++I ) {
#if 0
			bTemp.ResetNoDealloc();
			if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }
			if ( !_vMipMaps[I].Resize( bTemp.Length() ) ) { return false; }
			CStd::MemCpy( &_vMipMaps[I][0], &bTemp[0], bTemp.Length() );
#else
			ui32Size = ebsStream.ReadUInt32();
			if ( !ui32Size || ui32Size > ebsStream.Length() - ebsStream.GetPos() ) { return false; }
			if ( !_vMipMaps[I].Resize( ui32Size ) ) { return false; }
			CStd::MemCpy( &_vMipMaps[I][0], ebsStream.GetCurBufferPointer(), ui32Size );
			ebsStream.SkipBytes( ui32Size );
#endif	// #if 0
		}

		/*lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;
		bTemp.Allocate( GetHeight() * m_ui32RowWidth );
		if ( !CMisc::DecompressToBuffer( bsTemp, bTemp ) ) { return false; }
		if ( bTemp.Length() < GetHeight() * m_ui32RowWidth ) { return false; }
		CStd::MemCpy( &_tbOut[0], &bTemp[0], GetHeight() * m_ui32RowWidth );*/
		return true;
	}

	/**
	 * Loads an LSI image that was compressed via bitwise RLE compression.
	 *
	 * \param _bBuffer Buffer from which to load the image data.
	 * \param _tbOut Buffer where to store the decompressed image data.
	 * \return Returns true if there are no memory failures and the given data is valid. 
	 */
	LSBOOL LSE_CALL CImage::LoadCompressedBitwiseRle( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut ) {
		if ( !_tbOut.Length() ) { return false; }
		CStd::MemSet( &_tbOut[0], 0, _tbOut.Length() );
		if ( _ui32DataLen == 0 ) { return false; }
		lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;

		const static uint32_t ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		if ( !CMisc::DecompressToBuffer( bsTemp, bTemp ) ) { return false; }
		if ( bTemp.Length() == 0 && GetHeight() * GetWidth() != 0 ) { return false; }
		if ( bTemp.Length() == 0 ) { return true; }
		CExternalByteStream ebsThis( &bTemp[0], bTemp.Length() );

		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			uint32_t Y = 0, X = 0, ui32Total = 0, ui32Max = GetWidth() * GetHeight();
			bsTemp.Reset();
			uint32_t ui32Comp = CImageLib::GetComponentBits( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				while ( ui32Total < ui32Max ) {
					uint64_t ui64ThisTexel = 0;
					uint32_t ui32Count = 0;
					if ( ui32Comp >= 32 ) {
						if ( !ebsThis.ReadBytes( reinterpret_cast<uint8_t *>(&ui64ThisTexel), 4 ) ) { return false; }
					}
					if ( ui32Comp >= 24UL ) {
						if ( !ebsThis.ReadBytes( reinterpret_cast<uint8_t *>(&ui64ThisTexel), 3 ) ) { return false; }
					}
					else if ( ui32Comp >= 16 ) {
						if ( !ebsThis.ReadBytes( reinterpret_cast<uint8_t *>(&ui64ThisTexel), 2 ) ) { return false; }
					}
					else {
						if ( !ebsThis.ReadBytes( reinterpret_cast<uint8_t *>(&ui64ThisTexel), 1 ) ) { return false; }
					}
					if ( !ebsThis.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Count), 1 ) ) { return false; }

					// The image buffer has been filled with 0's so we can just | our new component into place.
					ui64ThisTexel <<= CImageLib::GetComponentOffset( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );

					for ( uint32_t J = 0; J <= ui32Count; ++J ) {
						(*reinterpret_cast<uint64_t *>(&_tbOut[Y*m_ui32RowWidth+X++*m_ui32TexelSize])) |= ui64ThisTexel;
						if ( X == GetWidth() ) {
							X = 0;
							++Y;
							if ( Y == GetHeight() && J != ui32Count ) { return false; }	// Trying to overrun the buffer?
						}
					}
					ui32Total = ui32Total + 1 + ui32Count;
				}
			}
		}
		return true;
	}

	/**
	 * Loads an LSI image that was compressed via index compression.
	 *
	 * \param _bBuffer Buffer from which to load the image data.
	 * \param _tbOut Buffer where to store the decompressed image data.
	 * \return Returns true if there are no memory failures and the given data is valid. 
	 */
	LSBOOL LSE_CALL CImage::LoadCompressedIndices( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut ) {
		if ( !_tbOut.Length() ) { return false; }
		CStd::MemSet( &_tbOut[0], 0, _tbOut.Length() );
		if ( _ui32DataLen == 0 ) { return false; }

		lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTableEx, bTable, bIndices;

		uint32_t ui32IndexSize = 0, ui32TableSize = 0, ui32FormatSize = CImageLib::GetFormatSize( GetFormat() );
		if ( !CMisc::DecompressToBuffer( bsTemp, bTableEx ) ) { return false; }
		CExternalByteStream ebsTable( &bTableEx[0], bTableEx.Length() );
		// Read the table size.
		if ( !ebsTable.ReadBytes( reinterpret_cast<uint8_t *>(&ui32TableSize), sizeof( ui32TableSize ) ) ) { return false; }
		ui32IndexSize = CStd::HighestBit( static_cast<uint32_t>(ui32TableSize - 1) ) + 1;
		if ( !bTable.Resize( ui32TableSize * ui32FormatSize + sizeof( uint64_t ) ) ) { return false; }
		if ( !ebsTable.ReadBytes( reinterpret_cast<uint8_t *>(&bTable[0]), ui32TableSize * ui32FormatSize ) ) { return false; }

		if ( !bTable.Length() ) { return false; }

		// Decompress the indices.
		if ( !CMisc::DecompressToBuffer( bsTemp, bIndices ) ) { return false; }
		if ( !bIndices.Length() ) { return false; }
		bsTemp.Borrow( &bIndices[0], bIndices.Length() );

		uint64_t ui64Mask = ~(~(1ULL << (ui32FormatSize * 8)) + 1ULL);
		for ( uint32_t Y = 0; Y < GetHeight(); ++Y ) {
			for ( uint32_t X = 0; X < GetWidth(); ++X ) {
				uint32_t ui32Index = 0;
				if ( !bsTemp.ReadBits( reinterpret_cast<uint8_t *>(&ui32Index), ui32IndexSize ) ) { return false; }
				if ( ui32Index >= ui32TableSize ) { return false; }

				uint64_t ui64Texel = (*reinterpret_cast<uint64_t *>(&bTable[ui32FormatSize*ui32Index]));
				ui64Texel &= ui64Mask;
				(*reinterpret_cast<uint64_t *>(&_tbOut[Y*m_ui32RowWidth+X*m_ui32TexelSize])) |= ui64Texel;
			}
		}

		return true;
	}

	/**
	 * Loads an LSI image that was compressed via DXT compression.
	 *
	 * \param _ui8MipLevels The number of mipmap levels in the file.  Always at least 1.
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _tbOut Buffer where to store the decompressed image data.
	 * \param _vMipMaps Buffers where to store the mipmap decompressed image data.
	 * \return Returns true if there are no memory failures and the given data is valid. 
	 */
	LSBOOL LSE_CALL CImage::LoadDxt( uint8_t _ui8MipLevels, const uint8_t * _pui8FileData, uint32_t _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {
		if ( _ui8MipLevels == 0 ) { return false; }
		// First level is manual.
		CExternalByteStream ebsStream( static_cast<const uint8_t *>(_pui8FileData), _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;
		if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }
		if ( !_tbOut.Resize( bTemp.Length() ) ) { return false; }
		CStd::MemCpy( &_tbOut[0], &bTemp[0], bTemp.Length() );

		// The mipmaps.
		--_ui8MipLevels;
		if ( !_vMipMaps.Resize( _ui8MipLevels ) ) { return false; }
		for ( uint32_t I = 0; I < _ui8MipLevels; ++I ) {
			bTemp.ResetNoDealloc();
			if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }
			if ( !_vMipMaps[I].Resize( bTemp.Length() ) ) { return false; }
			CStd::MemCpy( &_vMipMaps[I][0], &bTemp[0], bTemp.Length() );
		}
		return true;
	}

	/**
	 * Loads an LSI file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CImage::LoadLsi( const uint8_t * _pui8FileData, uint32_t _ui32DataLen ) {

		CExternalByteStream ebsStream( static_cast<const uint8_t *>(_pui8FileData), _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;
		if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }

		CExternalByteStream ebsParms( &bTemp[0], bTemp.Length() );
		m_pfFormat = static_cast<LSI_PIXEL_FORMAT>(0);
		if ( !ebsParms.ReadBytes( reinterpret_cast<uint8_t *>(&m_ui32Width), sizeof( m_ui32Width ) ) ) { Reset(); return false; }
		if ( !ebsParms.ReadBytes( reinterpret_cast<uint8_t *>(&m_ui32Height), sizeof( m_ui32Height ) ) ) { Reset(); return false; }
		if ( !ebsParms.ReadBytes( reinterpret_cast<uint8_t *>(&m_pfFormat), 1 ) ) { Reset(); return false; }
		uint8_t ui8MipMaps = 0;
		if ( !ebsParms.ReadBytes( reinterpret_cast<uint8_t *>(&ui8MipMaps), 1 ) ) { Reset(); return false; }
		uint8_t ui8Srgb = 1;
		if ( !ebsParms.ReadBytes( reinterpret_cast<uint8_t *>(&ui8Srgb), 1 ) ) { Reset(); return false; }
		m_gColorSpace = ui8Srgb ? LSI_G_sRGB : LSI_G_LINEAR;
		PostLoad();	// Sets m_ui32RowWidth and m_ui32TexelSize.

		LSI_COMPRESSIONS cComp = static_cast<LSI_COMPRESSIONS>(0);
		CExternalByteStream ebsNext( &_pui8FileData[static_cast<uint32_t>(ebsStream.GetPos())], static_cast<uint32_t>(bTemp.Length() - ebsStream.GetPos()) );
		if ( !ebsNext.ReadBytes( reinterpret_cast<uint8_t *>(&cComp), 1 ) ) { Reset(); return false; }

		uint32_t ui32Offset = static_cast<uint32_t>(ebsStream.GetPos() + ebsNext.GetPos());
		uint32_t ui32Size = static_cast<uint32_t>(_ui32DataLen - ebsStream.GetPos() - ebsNext.GetPos());
		if ( !m_tbBuffer.Resize( CImageLib::GetBufferSizeWithPadding( m_pfFormat, m_ui32Width, m_ui32Height ) ) ) { return false; }
		CStd::MemSet( &m_tbBuffer[0], 0, m_tbBuffer.Length() );
		switch ( cComp ) {
			case LSI_C_RAWCOMPRESSION : {
				return LoadCompressedRaw( ui8MipMaps, &_pui8FileData[ui32Offset], ui32Size, m_tbBuffer, m_vMipMapBuffer );
			}
			case LSI_C_CHANNELCOMPRESSION : {
				return LoadCompressedChannels( &_pui8FileData[ui32Offset], ui32Size, m_tbBuffer );
			}
			case LSI_C_RLECOMPRESSION : {
				return LoadCompressedBitwiseRle( &_pui8FileData[ui32Offset], ui32Size, m_tbBuffer );
			}
			case LSI_C_INDEXCOMPRESSION : {
				return LoadCompressedIndices( &_pui8FileData[ui32Offset], ui32Size, m_tbBuffer );
			}
			case LSI_C_DTXCOMPRESSION : {
				return LoadDxt( ui8MipMaps, &_pui8FileData[ui32Offset], ui32Size, m_tbBuffer, m_vMipMapBuffer );
			}
			default : { Reset(); return false; }
		}
	}

	/**
	 * Loads an image via the FreeImage library.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CImage::LoadFreeImage( const uint8_t * _pui8FileData, uint32_t _ui32DataLen ) {
		FIMEMORY * pmMem = ::FreeImage_OpenMemory( const_cast<uint8_t *>(_pui8FileData), _ui32DataLen );
		if ( !pmMem ) { return false; }

		FREE_IMAGE_FORMAT fifFormat = ::FreeImage_GetFileTypeFromMemory( pmMem, 0 );
		if ( FIF_UNKNOWN == fifFormat ) {
			::FreeImage_CloseMemory( pmMem );
			return false;
		}

		FIBITMAP * pbData = ::FreeImage_LoadFromMemory( fifFormat, pmMem, 0 );
		int iTransIndex = ::FreeImage_GetTransparentIndex( pbData );

		uint32_t ui32Width = ::FreeImage_GetWidth( pbData );
		uint32_t ui32Height = ::FreeImage_GetHeight( pbData );

#define FreeImage_GetScanLine( DATA, Y )	FreeImage_GetScanLine( DATA, ui32Height - Y - 1 )
		LSBOOL bRes = false;
#define LSI_CREATE( FORMAT )			LSBOOL bCreated = CreateBlank( (FORMAT), ui32Width, ui32Height );						\
	uint32_t ui32AShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_A );													\
	uint32_t ui32RShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_R );													\
	uint32_t ui32GShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_G );													\
	uint32_t ui32BShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_B )
#define LSI_MAKE_COLOR( R, G, B, A )	uint64_t ui64Color = static_cast<uint64_t>((static_cast<uint64_t>(R) << ui32RShift) |	\
	(static_cast<uint64_t>(G) << ui32GShift) |																					\
	(static_cast<uint64_t>(B) << ui32BShift) |																					\
	(static_cast<uint64_t>(A) << ui32AShift));																					\
	(*reinterpret_cast<uint64_t *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])) = ui64Color


		FREE_IMAGE_TYPE fitType = ::FreeImage_GetImageType( pbData );
		switch ( fitType ) {
			case FIT_BITMAP : {
				switch ( ::FreeImage_GetBPP( pbData ) ) {
					case 1 : {
						LSI_CREATE( LSI_PF_R8G8B8A8 );
						if ( bCreated ) {
							const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( pbData );
							if ( prgbqPal ) {
								for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
									const uint8_t * pui8Data = ::FreeImage_GetScanLine( pbData, Y );
									for ( uint32_t X = 0; X < ui32Width; ++X ) {
										uint8_t ui8Bit = (pui8Data[X>>3] >> (X % 8)) & 1;
										BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
										if ( iTransIndex != -1 ) {
											bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
										}
										LSI_MAKE_COLOR( prgbqPal[ui8Bit].rgbRed, prgbqPal[ui8Bit].rgbGreen,
											prgbqPal[ui8Bit].rgbBlue, bAlpha );
									}
								}
								bRes = true;
							}
						}
						break;
					}
					case 4 : {
						LSI_CREATE( LSI_PF_R8G8B8A8 );
						if ( bCreated ) {
							const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( pbData );
							if ( prgbqPal ) {
								for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
									const uint8_t * pui8Data = ::FreeImage_GetScanLine( pbData, Y );
									for ( uint32_t X = 0; X < ui32Width; ++X ) {
										uint8_t ui8Bit = (pui8Data[X>>1] >> (X % 2 << 2)) & 0xF;
										BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
										if ( iTransIndex != -1 ) {
											bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
										}
										LSI_MAKE_COLOR( prgbqPal[ui8Bit].rgbRed, prgbqPal[ui8Bit].rgbGreen,
											prgbqPal[ui8Bit].rgbBlue, bAlpha );
									}
								}
								bRes = true;
							}
						}
						break;
					}
					case 8 : {
						LSI_CREATE( LSI_PF_R8G8B8A8 );
						if ( bCreated ) {
							const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( pbData );
							if ( prgbqPal ) {
								for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
									const uint8_t * pui8Data = ::FreeImage_GetScanLine( pbData, Y );
									for ( uint32_t X = 0; X < ui32Width; ++X ) {
										uint8_t ui8Bit = pui8Data[X];
										BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
										if ( iTransIndex != -1 ) {
											bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
										}
										LSI_MAKE_COLOR( prgbqPal[ui8Bit].rgbRed, prgbqPal[ui8Bit].rgbGreen,
											prgbqPal[ui8Bit].rgbBlue, bAlpha );
									}
								}
								bRes = true;
							}
						}
						break;
					}
					case 16 : {
						LSI_CREATE( LSI_PF_R16G16B16A16 );
						if ( bCreated ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint16_t * pui32Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( pbData, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint16_t ui16Bit = pui32Data[X];
									LSI_MAKE_COLOR( ui16Bit, ui16Bit,
										ui16Bit, 0xFFFF );
								}
							}
							bRes = true;
						}
						break;
					}
					case 24 : {
						LSI_CREATE( LSI_PF_R8G8B8 );
						if ( bCreated ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const RGBTRIPLE * prgbtData = reinterpret_cast<RGBTRIPLE *>(::FreeImage_GetScanLine( pbData, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									LSI_MAKE_COLOR( prgbtData[X].rgbtRed, prgbtData[X].rgbtGreen,
										prgbtData[X].rgbtBlue, 0 );
								}
							}
							bRes = true;
						}
						break;
					}
					case 32 : {
						LSI_CREATE( LSI_PF_R8G8B8A8 );
						if ( bCreated ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const RGBQUAD * prgbqData = reinterpret_cast<RGBQUAD *>(::FreeImage_GetScanLine( pbData, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									LSI_MAKE_COLOR( prgbqData[X].rgbRed, prgbqData[X].rgbGreen,
										prgbqData[X].rgbBlue, prgbqData[X].rgbReserved );
								}
							}
							bRes = true;
						}
						break;
					}
				}
				break;	// FIT_BITMAP
			}
			case FIT_UINT16 : {
				LSI_CREATE( LSI_PF_R16G16B16A16 );
				if ( bCreated ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const uint16_t * pui32Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							uint16_t ui16Bit = pui32Data[X];
							LSI_MAKE_COLOR( ui16Bit, ui16Bit,
								ui16Bit, 0xFFFF );
						}
					}
					bRes = true;
				}
				break;	// FIT_UINT16
			}
			case FIT_INT16 : {
				break;	// FIT_INT16
			}
			case FIT_UINT32 : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const uint32_t * pui32Data = reinterpret_cast<uint32_t *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							float fBit = static_cast<float>(pui32Data[X] / static_cast<double>(LSE_MAXU32));
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_UINT32
			}
			case FIT_INT32 : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const int32_t * pui32Data = reinterpret_cast<int32_t *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							float fBit = static_cast<float>(pui32Data[X] / static_cast<double>(LSSTD_MAX_INT32));
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_INT32
			}
			case FIT_FLOAT : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const float * pfData = reinterpret_cast<float *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							float fBit = pfData[X];
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_FLOAT
			}
			case FIT_DOUBLE : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const double * pdData = reinterpret_cast<double *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							float fBit = static_cast<float>(pdData[X]);
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_DOUBLE
			}
			case FIT_COMPLEX : {
				break;	// FIT_COMPLEX
			}
			case FIT_RGB16 : {
				LSI_CREATE( LSI_PF_R16G16B16A16 );
				if ( bCreated ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const FIRGB16 * pfrgb16Data = reinterpret_cast<FIRGB16 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							LSI_MAKE_COLOR( pfrgb16Data[X].red, pfrgb16Data[X].green,
								pfrgb16Data[X].blue, 0xFFFF );
						}
					}
					bRes = true;
				}
				break;	// FIT_RGB16
			}
			case FIT_RGBA16 : {
				LSI_CREATE( LSI_PF_R16G16B16A16 );
				if ( bCreated ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const FIRGBA16 * pfrgba16Data = reinterpret_cast<FIRGBA16 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							LSI_MAKE_COLOR( pfrgba16Data[X].red, pfrgba16Data[X].green,
								pfrgba16Data[X].blue, pfrgba16Data[X].alpha );
						}
					}
					bRes = true;
				}
				break;	// FIT_RGBA16
			}
			case FIT_RGBF : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const FIRGBF * pfData = reinterpret_cast<FIRGBF *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = pfData[X].red;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = pfData[X].green;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = pfData[X].blue;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_RGBF
			}
			case FIT_RGBAF : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						const FIRGBAF * pfData = reinterpret_cast<FIRGBAF *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( uint32_t X = 0; X < ui32Width; ++X ) {
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = pfData[X].red;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = pfData[X].green;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = pfData[X].blue;
							reinterpret_cast<float *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = pfData[X].alpha;
						}
					}
					bRes = true;
				}
				break;	// FIT_RGBAF
			}
			default : {}
		}
#undef FreeImage_GetScanLine
#undef LSI_CREATE
#undef LSI_MAKE_COLOR
		::FreeImage_Unload( pbData );
		::FreeImage_CloseMemory( pmMem );
		return bRes;
	}

	/**
	 * A thread function for resampling from a source image to a destination texel buffer.
	 *
	 * \param _lpParameter Pointer to an LSI_MIPMAP_DATA structure.
	 * \return Returns the result of the call to Resample() on the created thread.
	 */
	uint32_t LSH_CALL CImage::ResampleThread( void * _lpParameter ) {
		LSI_MIPMAP_DATA * pmdData = static_cast<LSI_MIPMAP_DATA *>(_lpParameter);
		return pmdData->piSrc->Resample( pmdData->ui32NewWidth, pmdData->ui32NewHeight,
			pmdData->fFilter, (*pmdData->ptbDst), pmdData->fGamma, pmdData->amAddressMode, pmdData->fGammaScale );
	}

	/**
	 * Thread function for converting a block of texels to DXTn format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_DXT_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	uint32_t LSH_CALL CImage::DxtThread( void * _lpParameter ) {
		LSI_DXT_THREAD_DATA * pdtdData = static_cast<LSI_DXT_THREAD_DATA *>(_lpParameter);


		LSI_BLOCK bColors[4][4];
		uint32_t ui32Width = (pdtdData->piSrcImage->GetWidth() + 3) >> 2;
		// Number of blocks along the height.
		uint32_t ui32TotalScanLines = (pdtdData->piSrcImage->GetHeight() + 3) >> 2;

		uint32_t ui32SrcStride = CImageLib::GetRowSize( pdtdData->piSrcImage->GetFormat(), pdtdData->piSrcImage->GetWidth() );
		const uint8_t * pui8Src = &pdtdData->piSrcImage->m_tbBuffer[0];
		pui8Src += (ui32SrcStride << 2) * pdtdData->ui32SrcY;
		// Do LSI_DXT_ROWS rows at a time.
		uint32_t ui32Mask = 0;
		for ( uint32_t K = 0; K < LSI_DXT_ROWS; ++K ) {
			if ( pdtdData->ui32SrcY >= ui32TotalScanLines ) { break; }
			LSE_PREFETCH_LINE( pui8Src + (K + 1) * ui32SrcStride );

			for ( uint32_t X = pdtdData->ui32SrcX; X < ui32Width; ++X ) {
				uint32_t ui32SrcX = X << 4;							// 4 texels per block wide, 4 channels per texel.
				// Get the 4-by-4 block.
				//float fAlphaHigh = 0.0f, fAlphaLow = 1.0f;
				for ( uint32_t J = 0; J < 4; ++J ) {				// Down the block height.
					const float * pfThisRowStart = reinterpret_cast<const float *>(pui8Src + J * ui32SrcStride);
					if ( J + (pdtdData->ui32SrcY << 2) >= pdtdData->piSrcImage->GetHeight() ) {
						for ( uint32_t I = 0; I < 4; ++I ) {		// Along the block width.
							for ( uint32_t C = 0; C < 4; ++C ) {	// For each channel in this texel.
								bColors[J][I].fValues[C] = 0.0f;
							}
						}
					}
					else {
						// The row is valid.
						for ( uint32_t I = 0; I < 4; ++I ) {		// Along the block width.
							// We are on a single texel now.  But it might be beyond the width of the image.
							if ( I + (X << 2) >= pdtdData->piSrcImage->GetWidth() ) {
								for ( uint32_t C = 0; C < 4; ++C ) {// For each channel in this texel.
									bColors[J][I].fValues[C] = 0.0f;
								}
							}
							else {
								const float * pfSrcTexel = pfThisRowStart + ui32SrcX + (I << 2);
								// It is inside the image, so add it to an entry in the block.
								bColors[J][I].s.fR = pfSrcTexel[LSI_PC_R];
								bColors[J][I].s.fG = pfSrcTexel[LSI_PC_G];
								bColors[J][I].s.fB = pfSrcTexel[LSI_PC_B];
								bColors[J][I].s.fA = pfSrcTexel[LSI_PC_A];
								/*fAlphaHigh = CStd::Max<float>( fAlphaHigh, bColors[J][I].s.fA );
								fAlphaLow = CStd::Min<float>( fAlphaLow, bColors[J][I].s.fA );*/
								ui32Mask |= 1 << ((J << 2) + I);
							}
						}
					}
				}

				// The local block is filled.  Send it to the filter to determine the best high and low colors to use.
				int iFlags = squish::kColorMetricPerceptual | squish::kColorIterativeClusterFit | squish::kWeightColorByAlpha;
				
				squish::SquishConfig scConfig = {
					(*pdtdData->pdoOptions).fRedWeight,
					(*pdtdData->pdoOptions).fGreenWeight,
					(*pdtdData->pdoOptions).fBlueWeight,
					(*pdtdData->pdoOptions).fAlphaThresh,
				};
#ifdef SQUISH_USE_FLOATS
				float * pfBlock = bColors[0][0].fValues;
#else
				squish::u8 u8Block[4][4][4];
				for ( uint32_t Y = 0; Y < 4; ++Y ) {			// Block height.
					for ( uint32_t X = 0; X < 4; ++X ) {		// Block width.
						for ( uint32_t I = 0; I < 4; ++I ) {	// Colors.
							u8Block[Y][X][I] = static_cast<squish::u8>(bColors[Y][X].fValues[I] * 255.0f);
						}
					}
				}
#endif	// #ifdef SQUISH_USE_FLOATS
				switch ( pdtdData->pfFormat ) {
					case LSI_PF_DXT1 : {
						iFlags |= squish::kDxt1;
						break;
					}
					case LSI_PF_DXT2 : {}
					case LSI_PF_DXT3 : {
						iFlags |= squish::kDxt3;
						break;
					}
					case LSI_PF_DXT4 : {}
					case LSI_PF_DXT5 : {
						iFlags |= squish::kDxt5;
						break;
					}
					case LSI_PF_BC4 : {
						iFlags |= squish::kBc4;
						break;
					}
					case LSI_PF_BC5 : {
						iFlags |= squish::kBc5;
						break;
					}
					default : {}
				}
				squish::CompressMasked(
#ifdef SQUISH_USE_FLOATS
					pfBlock,
#else
					reinterpret_cast<squish::u8 *>(u8Block),
#endif	// #ifdef SQUISH_USE_FLOATS
					ui32Mask,
					pdtdData->pui8Dest, iFlags, scConfig );

				pdtdData->pui8Dest += CDds::DxtBlockSize( pdtdData->pfFormat );
			}
			pui8Src += (ui32SrcStride << 2);
			++pdtdData->ui32SrcY;
		}

		pdtdData->bDone = true;
		return 0;
	}

}	// namespace lsi
