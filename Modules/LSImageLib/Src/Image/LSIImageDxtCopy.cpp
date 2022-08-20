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
#include "../Gif/LSIGif.h"
#include "../Png/LSIPng.h"
#include "../Tga/LSITga.h"
#include "Crc/LSSTDCrc.h"
#include "FreeImage.h"
#include "HighLevel/LSFFilesEx.h"
#include "Set/LSTLFixedSet.h"
#include "Set/LSTLSet.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "Vector/LSMVector4.h"

#include <squish.h>


namespace lsi {

	// == Members.
	/** Found best matches for blocks of a single color in a DXT compression. */
	CLinearMap<LSUINT32, LSUINT32> CImage::m_mSingleColorMatches;

	/** Critical section for accessing m_mSingleColorMatches. */
	CCriticalSection CImage::m_csDxtCrit;

	/** Single-color R/B 4-index DXT table. */
	CVectorPoD<CImage::LSI_DXT_TABLE_4, LSUINT16> CImage::m_vDxtTableRb4;

	/** Single-color G 4-index DXT table. */
	CVectorPoD<CImage::LSI_DXT_TABLE_4, LSUINT16> CImage::m_vDxtTableG4;

	// == Operators.
	/**
	 * Less-than comparison.
	 *
	 * \param _spOther Object against which to test.
	 * \return Returns true if this object is lexicographically less than the given object.
	 */
	LSBOOL LSE_CALL CImage::LSI_SORTED_PIXEL::operator < ( const LSI_SORTED_PIXEL &_spOther ) const {
		if ( ui32Length <= sizeof( LSUINT64 ) ) { return u.ui64Rgb64 < _spOther.u.ui64Rgb64; }
		return CStd::MemCmp( u.ui8Rgb, _spOther.u.ui8Rgb, ui32Length ) < 0;
	}

	/**
	 * Equality comparison.
	 *
	 * \param _spOther Object against which to test.
	 * \return Returns true if this object is lexicographically equal to the given object.
	 */
	LSBOOL LSE_CALL CImage::LSI_SORTED_PIXEL::operator == ( const LSI_SORTED_PIXEL &_spOther ) const {
		if ( ui32Length <= sizeof( LSUINT64 ) ) { return u.ui64Rgb64 == _spOther.u.ui64Rgb64; }
		return CStd::MemCmp( u.ui8Rgb, _spOther.u.ui8Rgb, ui32Length ) == 0;
	}

	// == Various constructors.
	LSE_CALLCTOR CImage::CImage() :
		m_ui32Width( 0 ),
		m_ui32Height( 0 ),
		m_pfFormat( LSI_PF_ANY ),
		m_ui32RowWidth( 0 ),
		m_ui32TexelSize( 0 ),
		m_gColorSpace( LSI_G_LINEAR ) {
	}
	LSE_CALLCTOR CImage::CImage( const CImage &_iOther ) {
		m_ui32Width = _iOther.m_ui32Width;
		m_ui32Height = _iOther.m_ui32Height;
		m_pfFormat = _iOther.m_pfFormat;
		m_ui32RowWidth = _iOther.m_ui32RowWidth;
		m_ui32TexelSize = _iOther.m_ui32TexelSize;
		m_gColorSpace = _iOther.m_gColorSpace;

		if ( !m_tbBuffer.Resize( _iOther.m_tbBuffer.Length() ) ) { throw LSSTD_E_OUTOFMEMORY; }
		CStd::MemCpy( &m_tbBuffer[0], &_iOther.m_tbBuffer[0], m_tbBuffer.Length() );

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
	LSVOID LSE_CALL CImage::SetAllocator( CAllocator * _paAllocator ) {
		if ( _paAllocator != m_tbBuffer.GetAllocator() ) {
			Reset();
			m_tbBuffer.SetAllocator( _paAllocator );
			m_vMipMapBuffer.SetAllocator( _paAllocator );
		}
	}

	/**
	 * Reset the image entirely, leaving it ready to be used again for a new image.
	 */
	LSVOID LSE_CALL CImage::Reset() {
		m_ui32Width = m_ui32Height = 0;
		m_pfFormat = LSI_PF_ANY;
		m_tbBuffer.Reset();
		m_vMipMapBuffer.Reset();

		m_ui32RowWidth = m_ui32TexelSize = 0;
		m_gColorSpace = LSI_G_LINEAR;
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
	LSBOOL LSE_CALL CImage::GenerateMipMaps( LSUINT32 _ui32Levels, LSI_FILTER _fFilter,
		CResampler::LSI_ADDRESS_MODE _amAddressMode ) {
		m_vMipMapBuffer.Reset();

		// Count the number of mipmaps we will generate.
		LSUINT32 ui32Mips = 0;
		LSUINT32 ui32Width = GetWidth() >> 1, ui32Height = GetHeight() >> 1;
		for ( LSUINT32 I = _ui32Levels; --I; ) {
			if ( ui32Width || ui32Height ) {
				ui32Width = CStd::Max<LSUINT32>( ui32Width, 1 );
				ui32Height = CStd::Max<LSUINT32>( ui32Height, 1 );
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
				ui32Width = CStd::Max<LSUINT32>( ui32Width, 1 );
				ui32Height = CStd::Max<LSUINT32>( ui32Height, 1 );
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
				for ( LSUINT32 J = ui32Mips; J--; ) {
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

		LSUINT32 ui32Ret;
		LSBOOL bRet = true;
		for ( LSUINT32 J = ui32Mips; J--; ) {
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
	 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
	 */
	LSBOOL LSE_CALL CImage::LoadFileFromMemory( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen ) {
		Reset();
		
		if ( CBmp::LoadBmp( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		if ( CGif::LoadGif( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		if ( CTga::LoadTga( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		//if ( CPng::LoadPng( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer ) ) { PostLoad(); return true; }
		if ( CDds::LoadDds( _pui8FileData, _ui32DataLen, m_ui32Width, m_ui32Height, m_pfFormat, m_tbBuffer, m_vMipMapBuffer ) ) { PostLoad(); return true; }
		if ( LoadFreeImage( _pui8FileData, _ui32DataLen ) ) { PostLoad(); return true; }
		return LoadLsi( _pui8FileData, _ui32DataLen );	// Calls PostLoad().
	}

	/**
	 * Create an image from a file.  Path must be in UTF-8 format.
	 *
	 * \param _pcFile UTF-8-formatted string representing the path to the file to load.
	 * \return Returns true if the file can be loaded and if there are enough resources available to load the file.
	 */
	LSBOOL LSE_CALL CImage::LoadFile( const LSCHAR * _pcFile ) {
		LSUINT8 * ui8Buffer;
		LSUINTPTR uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		LSBOOL bRet = LoadFileFromMemory( ui8Buffer, static_cast<LSUINT32>(uiptrSize) );
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
	LSBOOL LSE_CALL CImage::CreateBlank( LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
		LSUINT32 _ui32MipMaps, LSI_GAMMA _gGamma ) {
		Reset();
		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		m_pfFormat = _pfFormat;
		m_gColorSpace = _gGamma;

		if ( !m_tbBuffer.Resize( CImageLib::GetBufferSizeWithPadding( m_pfFormat, m_ui32Width, m_ui32Height ) ) ) { return false; }
		CStd::MemSet( &m_tbBuffer[0], 0, m_tbBuffer.Length() );

		// Mipmaps.
		m_vMipMapBuffer.Reset();
		LSUINT32 ui32Width = GetWidth() >> 1, ui32Height = GetHeight() >> 1;
		while ( --_ui32MipMaps ) {
			if ( ui32Width || ui32Height ) {
				ui32Width = CStd::Max<LSUINT32>( ui32Width, 1 );
				ui32Height = CStd::Max<LSUINT32>( ui32Height, 1 );
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
			LSUINT32 ui32Width = CStd::Max( iFloatRgb.GetWidth(), iFloatAlpha.GetWidth() );
			LSUINT32 ui32Height = CStd::Max( iFloatRgb.GetHeight(), iFloatAlpha.GetHeight() );

			if ( !iFloatRgb.ResampleInPlace( ui32Width, ui32Height, LSI_F_BILINEAR_FILTER, -2.2f ) ) { return false; }
			if ( !iFloatAlpha.ResampleInPlace( ui32Width, ui32Height, LSI_F_MITCHELL_FILTER, -2.2f ) ) { return false; }

			iFloatRgb.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
			iFloatAlpha.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );

			// Get ready to copy into ourself.
			if ( !CreateBlank( LSI_PF_R8G8B8A8, ui32Width, ui32Height ) ) { return false; }

			LSUINT32 ui32SrcSize = CImageLib::GetFormatSize( LSI_PF_R32G32B32A32F );
			LSUINT32 ui32SrcRowSize = CImageLib::GetRowSize( LSI_PF_R32G32B32A32F, ui32Width );
			const LSUINT8 * pui8SrcRgb = reinterpret_cast<const LSUINT8 *>(iFloatRgb.GetBufferData());
			const LSUINT8 * pui8SrcAlpha = reinterpret_cast<const LSUINT8 *>(iFloatAlpha.GetBufferData());

			LSUINT32 ui32DstSize = CImageLib::GetFormatSize( LSI_PF_R8G8B8A8 );
			LSUINT32 ui32DstRowSize = CImageLib::GetRowSize( LSI_PF_R8G8B8A8, ui32Width );
			if ( CImageLib::GetTotalComponents( _iAlpha.GetFormat() ) == 3 ) {
				// Alpha is RGB.
				for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
					LSUINT32 ui32OffsetSrcY = Y * ui32SrcRowSize;
					LSUINT32 ui32OffsetDstY = Y * ui32DstRowSize;
					for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
						LSUINT32 ui32OffsetSrc = ui32OffsetSrcY + X * ui32SrcSize;
						LSUINT32 ui32OffsetDst = ui32OffsetDstY + X * ui32DstSize;
						const LSFLOAT * pfSrcRgb = reinterpret_cast<const LSFLOAT *>(&pui8SrcRgb[ui32OffsetSrc]);
						const LSFLOAT * pfSrcAlpha = reinterpret_cast<const LSFLOAT *>(&pui8SrcAlpha[ui32OffsetSrc]);
						LSFLOAT fAlpha = CStd::Clamp( pfSrcAlpha[LSI_PC_R] * LSI_R_WEIGHT +
							pfSrcAlpha[LSI_PC_G] * LSI_G_WEIGHT +
							pfSrcAlpha[LSI_PC_B] * LSI_B_WEIGHT,
							0.0f, 1.0f );

						LSUINT32 ui32R = static_cast<LSUINT8>(CStd::Clamp( pfSrcRgb[LSI_PC_R], 0.0f, 1.0f ) * 255.0f);
						LSUINT32 ui32G = static_cast<LSUINT8>(CStd::Clamp( pfSrcRgb[LSI_PC_G], 0.0f, 1.0f ) * 255.0f);
						LSUINT32 ui32B = static_cast<LSUINT8>(CStd::Clamp( pfSrcRgb[LSI_PC_B], 0.0f, 1.0f ) * 255.0f);
						LSUINT32 ui32A = static_cast<LSUINT8>(fAlpha * 255.0f);
						LSUINT32 ui32Rgba = (ui32R << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |
							(ui32G << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |
							(ui32B << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |
							(ui32A << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ));
						(*reinterpret_cast<LSUINT32 *>(&m_tbBuffer[ui32OffsetDst])) = ui32Rgba;
					}
				}
			}
			else {
				// Alpha is RGBA or A only.  Either way is the same here.
				for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
					LSUINT32 ui32OffsetSrcY = Y * ui32SrcRowSize;
					LSUINT32 ui32OffsetDstY = Y * ui32DstRowSize;
					for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
						LSUINT32 ui32OffsetSrc = ui32OffsetSrcY + X * ui32SrcSize;
						LSUINT32 ui32OffsetDst = ui32OffsetDstY + X * ui32DstSize;
						const LSFLOAT * pfSrcRgb = reinterpret_cast<const LSFLOAT *>(&pui8SrcRgb[ui32OffsetSrc]);
						const LSFLOAT * pfSrcAlpha = reinterpret_cast<const LSFLOAT *>(&pui8SrcAlpha[ui32OffsetSrc]);
						LSFLOAT fAlpha = CStd::Clamp( pfSrcAlpha[LSI_PC_A], 0.0f, 1.0f );

						LSUINT32 ui32R = static_cast<LSUINT8>(CStd::Clamp( pfSrcRgb[LSI_PC_R], 0.0f, 1.0f ) * 255.0f);
						LSUINT32 ui32G = static_cast<LSUINT8>(CStd::Clamp( pfSrcRgb[LSI_PC_G], 0.0f, 1.0f ) * 255.0f);
						LSUINT32 ui32B = static_cast<LSUINT8>(CStd::Clamp( pfSrcRgb[LSI_PC_B], 0.0f, 1.0f ) * 255.0f);
						LSUINT32 ui32A = static_cast<LSUINT8>(fAlpha * 255.0f);
						LSUINT32 ui32Rgba = (ui32R << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |
							(ui32G << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |
							(ui32B << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |
							(ui32A << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ));
						(*reinterpret_cast<LSUINT32 *>(&m_tbBuffer[ui32OffsetDst])) = ui32Rgba;
					}
				}
			}
		}
		catch ( ... ) { return false; }


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
		if ( !bTemp.Append( reinterpret_cast<LSUINT8 *>(&m_ui32Width), sizeof( m_ui32Width ) ) ) { return false; }
		if ( !bTemp.Append( reinterpret_cast<LSUINT8 *>(&m_ui32Height), sizeof( m_ui32Height ) ) ) { return false; }
		if ( !bTemp.Append( reinterpret_cast<LSUINT8 *>(&m_pfFormat), 1 ) ) { return false; }
		LSUINT8 ui8Mips = static_cast<LSUINT8>(TotalMipLevels());
		if ( !bTemp.Append( reinterpret_cast<LSUINT8 *>(&ui8Mips), 1 ) ) { return false; }
		LSUINT8 ui8Srgb = m_gColorSpace == LSI_G_sRGB;
		if ( !bTemp.Append( reinterpret_cast<LSUINT8 *>(&ui8Srgb), 1 ) ) { return false; }
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
		if ( !_bBuffer.Append( reinterpret_cast<LSUINT8 *>(&cComp), 1 ) ) { return false; }
		if ( !_bBuffer.Append( &bBest[0], bBest.Length() ) ) { return false; }

		return true;
	}

	/**
	 * Flip the image vertically.
	 */
	LSVOID LSE_CALL CImage::Flip() {
		if ( m_tbBuffer.Length() <= 1 ) { return; }
		LSVOID * pvBuffer = CMemLib::MAlloc( m_ui32RowWidth );
		for ( LSUINT32 I = m_ui32Height >> 1; I--; ) {
			CStd::MemCpy( pvBuffer, &m_tbBuffer[I*m_ui32RowWidth], m_ui32RowWidth );
			CStd::MemCpy( &m_tbBuffer[I*m_ui32RowWidth], &m_tbBuffer[(m_ui32Height-I-1)*m_ui32RowWidth], m_ui32RowWidth );
			CStd::MemCpy( &m_tbBuffer[(m_ui32Height-I-1)*m_ui32RowWidth], pvBuffer, m_ui32RowWidth );
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
	LSBOOL LSE_CALL CImage::IsChannelSetTo( LSI_PIXEL_COMPONENTS _pcComponent, LSFLOAT _fValue ) const {
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return false; }
		LSUINT32 ui32Shift = CImageLib::GetComponentOffset( GetFormat(), _pcComponent );
		LSUINT32 ui32Mask = CImageLib::GetComponentBits( GetFormat(), _pcComponent );
		LSUINT32 ui32RowSize = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		LSUINT32 ui32SrcTexelSize = CImageLib::GetFormatSize( GetFormat() );
		LSFLOAT fMul = static_cast<LSFLOAT>(1 << ui32Mask) - 1.0f;
		LSUINT32 ui32Val = static_cast<LSUINT32>(_fValue * fMul);

		ui32Mask = ((1 << ui32Mask) - 1);
#define LSI_TEXEL_AT( X, Y )			(*reinterpret_cast<const LSUINT64 *>(&m_tbBuffer[Y*ui32RowSize+X*ui32SrcTexelSize]))
		for ( LSUINT32 Y = GetHeight(); Y--; ) {
			for ( LSUINT32 X = GetWidth(); X--; ) {
				LSUINT64 ui64Texel = LSI_TEXEL_AT( X, Y );

				LSUINT32 ui32This = (ui64Texel >> ui32Shift) & ui32Mask;
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
	LSVOID LSE_CALL CImage::SwapChannels( LSI_PIXEL_COMPONENTS _pcCom0, LSI_PIXEL_COMPONENTS _pcCom1 ) {
		if ( CImageLib::IsCompressed( GetFormat() ) ) { return; }
		LSUINT32 ui32Shift0 = CImageLib::GetComponentOffset( GetFormat(), _pcCom0 );
		LSUINT32 ui32Bits0 = CImageLib::GetComponentBits( GetFormat(), _pcCom0 );
		LSUINT32 ui32Mask0 = ((1 << ui32Bits0) - 1);
		LSFLOAT fMul0 = 1.0f / (static_cast<LSFLOAT>(1 << ui32Bits0) - 1.0f);
		
		LSUINT32 ui32Shift1 = CImageLib::GetComponentOffset( GetFormat(), _pcCom1 );
		LSUINT32 ui32Bits1 = CImageLib::GetComponentBits( GetFormat(), _pcCom1 );
		LSUINT32 ui32Mask1 = ((1 << ui32Bits1) - 1);
		LSFLOAT fMul1 = 1.0f / (static_cast<LSFLOAT>(1 << ui32Bits1) - 1.0f);

		LSUINT64 ui64ClearMask = (static_cast<LSUINT64>(ui32Mask0) << static_cast<LSUINT64>(ui32Shift0));
		ui64ClearMask |= (static_cast<LSUINT64>(ui32Mask1) << static_cast<LSUINT64>(ui32Shift1));
		ui64ClearMask = ~ui64ClearMask;

		LSUINT32 ui32RowSize = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		LSUINT32 ui32SrcTexelSize = CImageLib::GetFormatSize( GetFormat() );

#define LSI_TEXEL_AT( X, Y )			(*reinterpret_cast<LSUINT64 *>(&m_tbBuffer[Y*ui32RowSize+X*ui32SrcTexelSize]))
		for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
			for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
				LSUINT64 & ui64Texel = LSI_TEXEL_AT( X, Y );
				
				LSUINT32 ui32This0 = (ui64Texel >> ui32Shift0) & ui32Mask0;
				LSUINT32 ui32This1 = (ui64Texel >> ui32Shift1) & ui32Mask1;

				ui64Texel &= ui64ClearMask;

				LSFLOAT fThis0 = ui32This0 * fMul0;
				LSFLOAT fThis1 = ui32This1 * fMul1;

				// Convert to the opposite bit amount.
				fThis0 /= fMul1;
				fThis1 /= fMul0;

				ui32This0 = static_cast<LSUINT32>(fThis0 + 0.5f);
				ui32This1 = static_cast<LSUINT32>(fThis1 + 0.5f);

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
	 */
	LSVOID LSE_CALL CImage::ConvertToFormat( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDest ) const {
		if ( _pfFormat == m_pfFormat ) {
			_iDest = (*this);		// Copy directly.
			return;
		}

		// Format changed, so make a copy.		
		if ( !_iDest.CreateBlank( _pfFormat, m_ui32Width, m_ui32Height, TotalMipLevels(), m_gColorSpace ) ) { throw LSSTD_E_OUTOFMEMORY; }
		LSUINT32 ui32H = m_ui32Height;
		LSUINT32 ui32W = m_ui32Width;
		for ( LSUINT32 I = 0; I < TotalMipLevels(); ++I ) {
			const CImageLib::CTexelBuffer & tbSrc = GetMipMapBuffers( I );
			CImageLib::CTexelBuffer & tbDst = const_cast<CImageLib::CTexelBuffer &>(_iDest.GetMipMapBuffers( I ));
			CImageLib::BatchAnyTexelToAnytexel( m_pfFormat, &tbSrc[0], _pfFormat, &tbDst[0], ui32W * ui32H );
			/*LSUINT32 ui32SrcRow = CImageLib::GetRowSize( m_pfFormat, ui32W );
			LSUINT32 ui32DstRow = CImageLib::GetRowSize( _pfFormat, ui32W );
			for ( LSUINT32 Y = 0; Y < ui32H; ++Y ) {
				CImageLib::BatchAnyTexelToAnytexel( m_pfFormat, &tbSrc[Y*ui32SrcRow], _pfFormat, &tbDst[Y*ui32DstRow], ui32W );
			}*/
			ui32H = CStd::Max<LSUINT32>( ui32H >> 1, 1 );
			ui32W = CStd::Max<LSUINT32>( ui32W >> 1, 1 );
		}
	}

	/**
	 * Converts this image in-place to a given format.
	 *
	 * \param _pfFormat Format to which to convert this image.
	 * \return Returns the converted image.
	 */
	CImage & LSE_CALL CImage::ConvertToFormatInPlace( LSI_PIXEL_FORMAT _pfFormat ) {
		if ( _pfFormat == m_pfFormat ) { return (*this); }	// No conversion.
		CImage iTemp;
		ConvertToFormat( _pfFormat, iTemp );
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
	LSBOOL LSE_CALL CImage::Resample( LSUINT32 _ui32NewWidth, LSUINT32 _ui32NewHeight, LSI_FILTER _fFilter, CImage &_iDest,
		LSFLOAT _fSrcGamma, CResampler::LSI_ADDRESS_MODE _amAddressMode, LSFLOAT _fFilterScale ) const {
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


		if ( !_iDest.CreateBlank( GetFormat(), _ui32NewWidth, _ui32NewHeight ) ) {
			LSI_FAIL( "Out of memory while resampling." );
		}
		_iDest.SetColorSpace( IsSRgb() ? LSI_G_sRGB : LSI_G_LINEAR );

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
	LSBOOL LSE_CALL CImage::Resample( LSUINT32 _ui32NewWidth, LSUINT32 _ui32NewHeight, LSI_FILTER _fFilter, CImageLib::CTexelBuffer &_tbDest,
		LSFLOAT _fSrcGamma, CResampler::LSI_ADDRESS_MODE _amAddressMode, LSFLOAT _fFilterScale ) const {
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


		static const LSCHAR * pcFiters[] = {
			"box",						// LSI_F_BOX_FILTER
			"bilinear",					// LSI_F_BILINEAR_FILTER
			"quadratic",				// LSI_F_QUADRATIC_FILTER
			"kaiser",					// LSI_F_KAISER_FILTER
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


		LSFLOAT fSrgbToLinear[256];

		LSUINT8 ui8LinearToSrgb[LSI_LINEAR_TO_SRGB_TABLE_SIZE];
		static const LSFLOAT fInvLinearToSrgbTableSize = 1.0f / LSI_LINEAR_TO_SRGB_TABLE_SIZE;
		const LSFLOAT fInvSrcGamma = 1.0f / _fSrcGamma;

		if ( _fSrcGamma < 0.0f ) {
			for ( LSUINT32 I = LSE_ELEMENTS( fSrgbToLinear ); I--; ) {
				fSrgbToLinear[I] = CImageLib::SRgbToLinear( (1.0f / 255.0f) * I );
			}

			for ( LSUINT32 I = LSI_LINEAR_TO_SRGB_TABLE_SIZE; I--; ) {
				LSINT32 i32This = static_cast<LSINT32>(255.0f * CImageLib::LinearToSRgb( I * fInvLinearToSrgbTableSize ) + 0.5f);
				i32This = CStd::Clamp<LSINT32>( i32This, 0, 255 );
				ui8LinearToSrgb[I] = static_cast<LSUINT8>(static_cast<LSUINT32>(i32This));
			}
		}
		else {
			for ( LSUINT32 I = LSE_ELEMENTS( fSrgbToLinear ); I--; ) {
				fSrgbToLinear[I] = ::powf( (1.0f / 255.0f) * I, _fSrcGamma );
			}

			for ( LSUINT32 I = LSI_LINEAR_TO_SRGB_TABLE_SIZE; I--; ) {
				LSINT32 i32This = static_cast<LSINT32>(255.0f * ::powf( I * fInvLinearToSrgbTableSize, fInvSrcGamma ) + 0.5f);
				i32This = CStd::Clamp<LSINT32>( i32This, 0, 255 );
				ui8LinearToSrgb[I] = static_cast<LSUINT8>(static_cast<LSUINT32>(i32This));
			}
		}


		
		CVectorPoD<LSFLOAT, LSUINT32> vSamples[4];
		CVectorPoD<LSFLOAT, LSUINT32> vDstBuffer;

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

		LSUINT32 ui32Components = CImageLib::GetTotalComponents( GetFormat() );
		for ( LSUINT32 I = 1; I < ui32Components; ++I ) {
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
		LSUINT32 ui32Pitch = CImageLib::GetRowSize( GetFormat(), GetWidth() );
		LSUINT32 ui32Size = CImageLib::GetFormatSize( GetFormat() );
		LSUINT32 ui32DstY = 0;
		for ( LSUINT32 ui32SrcY = 0; ui32SrcY < GetHeight(); ++ui32SrcY ) {
			const LSUINT8 * pui8This = &m_tbBuffer[ui32Pitch*ui32SrcY];

			// Going across the row.
			for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
				// For each component.
				LSFLOAT fConv[4];
				switch ( GetFormat() ) {
					case LSI_PF_R16G16B16A16F : {
						CImageLib::Float16TexelToFloat32Texel( reinterpret_cast<const CFloat16 *>(pui8This), fConv );
						break;
					}
					case LSI_PF_R32G32B32A32F : {
						const LSFLOAT * pfThis = reinterpret_cast<const LSFLOAT *>(pui8This);
						fConv[0] = pfThis[0];
						fConv[1] = pfThis[1];
						fConv[2] = pfThis[2];
						fConv[3] = pfThis[3];
						break;
					}
					default : {
#ifdef LSE_IPHONE
						LSUINT64 ui64This = (static_cast<LSUINT64>(pui8This[7]) << 56ULL) |
							(static_cast<LSUINT64>(pui8This[6]) << 48ULL) |
							(static_cast<LSUINT64>(pui8This[5]) << 40ULL) |
							(static_cast<LSUINT64>(pui8This[4]) << 32ULL) |
							(static_cast<LSUINT64>(pui8This[3]) << 24ULL) |
							(static_cast<LSUINT64>(pui8This[2]) << 16ULL) |
							(static_cast<LSUINT64>(pui8This[1]) << 8ULL) |
							(static_cast<LSUINT64>(pui8This[0]) << 0ULL);
						CImageLib::IntegerTexelToFloat32Texel( GetFormat(), ui64This, fConv );
#else
						CImageLib::IntegerTexelToFloat32Texel( GetFormat(), (*reinterpret_cast<const LSUINT64 *>(pui8This)), fConv );
#endif	// #ifdef LSE_IPHONE
					}
				}

				// TODO: Do not use the table for floating-point formats.
				for ( LSUINT32 I = 0; I < ui32Components; ++I ) {
					// If this component is alpha, keep it in SRGB mode.
					LSI_PIXEL_COMPONENTS pcThis = CImageLib::ComponentIndexToComponentType( GetFormat(), I );
					if ( pcThis == LSI_PC_A ) {
						vSamples[I][X] = fConv[LSI_PC_A];
					}
					else {
						vSamples[I][X] = fSrgbToLinear[static_cast<LSUINT8>(fConv[pcThis]*255.0f)];
					}
				}
				pui8This += ui32Size;
			}

			// Add the scanlines.
			for ( LSUINT32 I = 0; I < ui32Components; ++I ) {
				if ( !rResamplers[I].AddScanLine( &vSamples[I][0] ) ) {
					LSI_FAIL( "Out of memory while creating scanlines." );
				}
			}

			LSUINT32 ui32RowSize = CImageLib::GetRowSize( GetFormat(), _ui32NewWidth );
			for ( ; ; ) {
				const CVectorPoD<LSFLOAT, LSUINT32> * pvOut = NULL;
				if ( ui32DstY >= _ui32NewHeight ) { break; }
				LSUINT8 * pui8Dest = &_tbDest[ui32RowSize*ui32DstY];
				for ( LSUINT32 I = 0; I < ui32Components; ++I ) {
					pvOut = rResamplers[I].GetScanLine();
					if ( !pvOut ) { break; }
					LSI_PIXEL_COMPONENTS pcThis = CImageLib::ComponentIndexToComponentType( GetFormat(), I );
					LSBOOL bIsAlpha = pcThis == LSI_PC_A;
					LSUINT64 ui64Shift = CImageLib::GetComponentOffset( GetFormat(), pcThis );

					// Mask out the existing value.
					LSUINT64 ui64HighestValue = (1ULL << CImageLib::GetComponentBits( GetFormat(), pcThis )) - 1ULL;
					LSUINT64 ui64Mask = ui64HighestValue << ui64Shift;
					ui64Mask = ~ui64Mask;
					// Scale from 8-bit components to the bits in this component.
					LSFLOAT fCompScale = static_cast<LSFLOAT>(ui64HighestValue) / 255.0f;

					// Go across the whole row.
					for ( LSUINT32 X = 0; X < _ui32NewWidth; ++X ) {
						LSUINT8 * pui8Target = &pui8Dest[X*ui32Size];
						
						switch ( GetFormat() ) {
							case LSI_PF_R16G16B16A16F : {
								LSFLOAT fVal;
								if ( bIsAlpha ) {
									fVal = (*pvOut)[X];
								}
								else {
									fVal = ::powf( (*pvOut)[X], fInvSrcGamma );
								}
								CFloat16 * pfThis = reinterpret_cast<CFloat16 *>(pui8Target);
								pfThis[I] = fVal;
								break;
							}
							case LSI_PF_R32G32B32A32F : {
								LSFLOAT fVal;
								if ( bIsAlpha ) {
									fVal = (*pvOut)[X];
								}
								else {
									fVal = ::powf( (*pvOut)[X], fInvSrcGamma );
								}
								LSFLOAT * pfThis = reinterpret_cast<LSFLOAT *>(pui8Target);
								pfThis[I] = fVal;
								break;
							}
							default : {
#ifdef LSE_IPHONE
								LSUINT64 ui64Val = (static_cast<LSUINT64>(pui8Target[7]) << 56ULL) |
									(static_cast<LSUINT64>(pui8Target[6]) << 48ULL) |
									(static_cast<LSUINT64>(pui8Target[5]) << 40ULL) |
									(static_cast<LSUINT64>(pui8Target[4]) << 32ULL) |
									(static_cast<LSUINT64>(pui8Target[3]) << 24ULL) |
									(static_cast<LSUINT64>(pui8Target[2]) << 16ULL) |
									(static_cast<LSUINT64>(pui8Target[1]) << 8ULL) |
									(static_cast<LSUINT64>(pui8Target[0]) << 0ULL);
#else
								LSUINT64 ui64Val = (*reinterpret_cast<LSUINT64 *>(pui8Target));
#endif	// #ifdef LSE_IPHONE
								ui64Val &= ui64Mask;
								if ( bIsAlpha ) {
									LSFLOAT fVal = (255.0f * (*pvOut)[X] + 0.5f) * fCompScale;
									LSINT64 i64Val = static_cast<LSINT64>(fVal);
									i64Val = CStd::Clamp<LSINT64>( i64Val, 0LL, static_cast<LSINT64>(ui64HighestValue) );
									ui64Val |= static_cast<LSUINT64>(i64Val << static_cast<LSINT64>(ui64Shift));
								}
								else {
									LSINT64 i64Val = static_cast<LSINT64>(LSI_LINEAR_TO_SRGB_TABLE_SIZE * (*pvOut)[X] + 0.5f);
									i64Val = CStd::Clamp<LSINT64>( i64Val, 0LL, LSI_LINEAR_TO_SRGB_TABLE_SIZE - 1 );
									i64Val = static_cast<LSINT64>(ui8LinearToSrgb[i64Val] * fCompScale);
									ui64Val |= static_cast<LSUINT64>(i64Val << static_cast<LSINT64>(ui64Shift));
								}
#ifdef LSE_IPHONE
								pui8Target[7] = static_cast<LSUINT8>((ui64Val >> 56ULL) & 0xFF);
								pui8Target[6] = static_cast<LSUINT8>((ui64Val >> 48ULL) & 0xFF);
								pui8Target[5] = static_cast<LSUINT8>((ui64Val >> 40ULL) & 0xFF);
								pui8Target[4] = static_cast<LSUINT8>((ui64Val >> 32ULL) & 0xFF);
								pui8Target[3] = static_cast<LSUINT8>((ui64Val >> 24ULL) & 0xFF);
								pui8Target[2] = static_cast<LSUINT8>((ui64Val >> 16ULL) & 0xFF);
								pui8Target[1] = static_cast<LSUINT8>((ui64Val >> 8ULL) & 0xFF);
								pui8Target[0] = static_cast<LSUINT8>((ui64Val >> 0ULL) & 0xFF);
#else
								(*reinterpret_cast<LSUINT64 *>(pui8Target)) = ui64Val;
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
	LSBOOL LSE_CALL CImage::ResampleInPlace( LSUINT32 _ui32NewWidth, LSUINT32 _ui32NewHeight, LSI_FILTER _fFilter,
		LSFLOAT _fSrcGamma, CResampler::LSI_ADDRESS_MODE _amAddressMode, LSFLOAT _fFilterScale ) {
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
	LSUINT32 LSE_CALL CImage::GetCrc32() const {
		if ( !m_tbBuffer.Length() ) { return LSE_MAXU32; }
		return CCrc::GetCrc( &m_tbBuffer[0], m_tbBuffer.Length() );
	}

	/**
	 * Calculates and returns the alternative 32-bit CRC for this image.
	 *
	 * \return Returns the alternative 32-bit CRC for this image.
	 */
	LSUINT32 LSE_CALL CImage::GetCrc32Alt() const {
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
	LSVOID LSE_CALL CImage::ConvertToNormalMap( LSFLOAT _fStr, LSBOOL _bWrapX, LSBOOL _bWrapY ) {
		assert( _fStr );
		CImage iCopy = (*this);


		CreateBlank( LSI_PF_R8G8B8, GetWidth(), GetHeight() );
		LSUINT32 ui32TexelWidth = CImageLib::GetFormatSize( GetFormat() );

		LSFLOAT fZ = 1.0f / _fStr;
		if ( GetFormat() == LSI_PF_A8 ) {
			// Convert alpha alone to a height value.
			for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
				LSUINT32 ui32Up = Y - 1;
				if ( ui32Up > Y ) {
					// ui32Up is less than 0.
					ui32Up = _bWrapY ? GetHeight() - 1 : 0;
				}
				LSUINT32 ui32Down = Y + 1;
				if ( ui32Down == GetHeight() ) {
					ui32Down = _bWrapY ? 0 : GetHeight() - 1;
				}

				for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
					LSUINT32 ui32Left = X - 1;
					if ( ui32Left > X ) {
						// ui32Up is less than 0.
						ui32Left = _bWrapX ? GetWidth() - 1 : 0;
					}
					LSUINT32 ui32Right = X + 1;
					if ( ui32Right == GetWidth() ) {
						ui32Right = _bWrapX ? 0 : GetWidth() - 1;
					}

					LSFLOAT fUl = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Up+ui32Left] * (1.0f / 255.0f);
					LSFLOAT fU = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Up+X] * (1.0f / 255.0f);
					LSFLOAT fUr = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Up+ui32Right] * (1.0f / 255.0f);

					LSFLOAT fL = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*Y+ui32Left] * (1.0f / 255.0f);
					LSFLOAT fR = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*Y+ui32Right] * (1.0f / 255.0f);

					LSFLOAT fBl = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Down+ui32Left] * (1.0f / 255.0f);
					LSFLOAT fB = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Down+X] * (1.0f / 255.0f);
					LSFLOAT fBr = iCopy.m_tbBuffer[iCopy.m_ui32RowWidth*ui32Down+ui32Right] * (1.0f / 255.0f);

					// X is computed with sobel:
					//	1 0 -1 
					//	2 0 -2
					//	1 0 -1
					LSFLOAT fX = fUl + (fL * 2.0f) + fBl - fUr + (fR * -2.0f) - fBr;
					//LSFLOAT fX = -fUl + (fL * -2.0f) - fBl + fUr + (fR * 2.0f) + fBr;

					// Y is computed with sobel:
					//	-1 -2 -1 
					//	 0  0  0
					//	 1  2  1
					//LSFLOAT fY = fUl + (fU * 2.0f) + fUr - fBl + (fB * -2.0f) - fBr;
					LSFLOAT fY = -fUl + (fU * -2.0f) - fUr + fBl + (fB * 2.0f) + fBr;

					// Normalize (manually).
					LSFLOAT fLen = 1.0f / ::sqrtf( fX * fX + fY * fY + fZ * fZ );
					fX *= fLen;
					fY *= fLen;
					LSFLOAT fThisZ = fZ * fLen;

					fX *= 0.5f;
					fX += 0.5;

					fY *= 0.5f;
					fY += 0.5;

					LSUINT64 ui64This = 0ULL;
					ui64This |= static_cast<LSUINT64>(static_cast<LSUINT8>(255.0f - fX * 255.0f)) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_R );
					ui64This |= static_cast<LSUINT64>(static_cast<LSUINT8>(255.0f - fY * 255.0f)) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_G );
					ui64This |= static_cast<LSUINT64>(static_cast<LSUINT8>(fThisZ * 255.0f)) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_B );

					(*reinterpret_cast<LSUINT64 *>(&m_tbBuffer[m_ui32RowWidth*Y+X*ui32TexelWidth])) = ui64This;

				}
			}
		}
		else {
			iCopy.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
			const LSFLOAT * pfSrc = reinterpret_cast<const LSFLOAT *>(iCopy.GetBufferData());
			LSUINT32 ui32Components = CImageLib::GetTotalComponents( iCopy.GetFormat() );
			LSUINT32 ui32RowStride = iCopy.m_ui32RowWidth / ui32Components;
			for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
				LSUINT32 ui32Up = Y - 1;
				if ( ui32Up > Y ) {
					// ui32Up is less than 0.
					ui32Up = _bWrapY ? GetHeight() - 1 : 0;
				}
				ui32Up *= ui32RowStride;
				LSUINT32 ui32Down = Y + 1;
				if ( ui32Down == GetHeight() ) {
					ui32Down = _bWrapY ? 0 : GetHeight() - 1;
				}
				ui32Down *= ui32RowStride;

				for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
					LSUINT32 ui32Left = X - 1;
					if ( ui32Left > X ) {
						// ui32Left is less than 0.
						ui32Left = _bWrapX ? GetWidth() - 1 : 0;
					}
					LSUINT32 ui32Right = X + 1;
					if ( ui32Right == GetWidth() ) {
						ui32Right = _bWrapX ? 0 : GetWidth() - 1;
					}

					LSFLOAT fUl = pfSrc[ui32Up+ui32Left*ui32Components];
					LSFLOAT fU = pfSrc[ui32Up+X*ui32Components];
					LSFLOAT fUr = pfSrc[ui32Up+ui32Right*ui32Components];

					LSFLOAT fL = pfSrc[Y*ui32RowStride+ui32Left*ui32Components];
					LSFLOAT fR = pfSrc[Y*ui32RowStride+ui32Right*ui32Components];

					LSFLOAT fBl = pfSrc[ui32Down+ui32Left*ui32Components];
					LSFLOAT fB = pfSrc[ui32Down+X*ui32Components];
					LSFLOAT fBr = pfSrc[ui32Down+ui32Right*ui32Components];


					// X is computed with sobel:
					//	1 0 -1 
					//	2 0 -2
					//	1 0 -1
					LSFLOAT fX = fUl + (fL * 2.0f) + fBl - fUr + (fR * -2.0f) - fBr;
					//LSFLOAT fX = -fUl + (fL * -2.0f) - fBl + fUr + (fR * 2.0f) + fBr;

					// Y is computed with sobel:
					//	-1 -2 -1 
					//	 0  0  0
					//	 1  2  1
					//LSFLOAT fY = fUl + (fU * 2.0f) + fUr - fBl + (fB * -2.0f) - fBr;
					LSFLOAT fY = -fUl + (fU * -2.0f) - fUr + fBl + (fB * 2.0f) + fBr;

					
					// Normalize (manually).
					LSFLOAT fLen = 1.0f / ::sqrtf( fX * fX + fY * fY + fZ * fZ );
					fX *= fLen;
					fY *= fLen;
					LSFLOAT fThisZ = fZ * fLen;

					fX *= 0.5f;
					fX += 0.5;

					fY *= 0.5f;
					fY += 0.5;

					LSUINT64 ui64This = 0ULL;
					ui64This |= static_cast<LSUINT64>(CStd::Clamp( fX * 255.0f, 0.0f, 255.0f )) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_R );
					ui64This |= static_cast<LSUINT64>(CStd::Clamp( fY * 255.0f, 0.0f, 255.0f )) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_G );
					ui64This |= static_cast<LSUINT64>(CStd::Clamp( fThisZ * 255.0f, 0.0f, 255.0f )) << CImageLib::GetComponentOffset( GetFormat(), LSI_PC_B );

					(*reinterpret_cast<LSUINT64 *>(&m_tbBuffer[m_ui32RowWidth*Y+X*ui32TexelWidth])) = ui64This;
				}

			}
		}
	}

	/**
	 * Converts the image to DDS format.
	 *
	 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
	 * \param _iDst The destination image to which this image is converted.
	 * \param _pdoOptions Conversion options.
	 * \return Returns true if there are no resource problems.
	 */
	LSBOOL LSE_CALL CImage::ConvertToDds( LSI_PIXEL_FORMAT _pfFormat, CImage &_iDst, const LSI_DXT_OPTIONS * _pdoOptions ) {
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
			// TODO: Convert to RGBA, then continue.
			return false;
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
			doOptions.bIgnoreDxt1Alpha = false;
			doOptions.ui32SecondStageIterations = 150;
			doOptions.ui32ThirdStageIterations[0] = 2;
			doOptions.ui32ThirdStageIterations[1] = 3;
			doOptions.ui32ThirdStageIterations[2] = 2;
		}
		if ( _pfFormat != LSI_PF_DXT1 ) {
			doOptions.fAlphaThresh = 0.0f;
		}
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
			for ( LSUINT32 I = LSE_ELEMENTS( tThreads ); I--; ) {
				CThreadLib::MakeInvalidHandle( tThreads[I] );
			}
			LSI_DXT_THREAD_DATA dtdData[LSE_ELEMENTS( tThreads )];
			for ( LSUINT32 I = LSE_ELEMENTS( dtdData ); I--; ) {
				dtdData[I].bDone = true;
			}

			// We will gather every 4-by-4 block into a linear array.
			//LSI_BLOCK bColors[4][4];
			if ( !_iDst.CreateBlank( _pfFormat, GetWidth(), GetHeight() ) ) { return false; }
			// Number of blocks along the width.
			LSUINT32 ui32Width = (GetWidth() + 3) >> 2;
			// Number of blocks along the height.
			LSUINT32 ui32TotalScanLines = (GetHeight() + 3) >> 2;
			
#define LSI_DXT_ROWS	12
			LSUINT8 * pui8Dst = &_iDst.m_tbBuffer[0];
			/*LSUINT32 ui32SrcStride = CImageLib::GetRowSize( iTemp.GetFormat(), iTemp.GetWidth() );
			const LSUINT8 * pui8Src = &iTemp.m_tbBuffer[0];*/
			for ( LSUINT32 Y = 0; Y < ui32TotalScanLines; Y += LSI_DXT_ROWS ) {
				//for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
					//LSUINT32 ui32SrcX = X << 4;							// 4 texels per block wide, 4 channels per texel.
					// Get the 4-by-4 block.
					//for ( LSUINT32 J = 0; J < 4; ++J ) {				// Down the block height.
					//	const LSFLOAT * pfThisRowStart = reinterpret_cast<const LSFLOAT *>(pui8Src + J * ui32SrcStride);
					//	if ( J + (Y << 2) >= GetHeight() ) {
					//		for ( LSUINT32 I = 0; I < 4; ++I ) {		// Along the block width.
					//			for ( LSUINT32 K = 0; K < 4; ++K ) {	// For each channel in this texel.
					//				bColors[J][I].fValues[K] = 0.0f;
					//			}
					//		}
					//	}
					//	else {
					//		// The row is valid.
					//		for ( LSUINT32 I = 0; I < 4; ++I ) {		// Along the block width.
					//			// We are on a single texel now.  But it might be beyond the width of the image.
					//			if ( I + (X << 2) >= GetWidth() ) {
					//				for ( LSUINT32 K = 0; K < 4; ++K ) {// For each channel in this texel.
					//					bColors[J][I].fValues[K] = 0.0f;
					//				}
					//			}
					//			else {
					//				const LSFLOAT * pfSrcTexel = pfThisRowStart + ui32SrcX + (I << 2);
					//				// It is inside the image, so add it to an entry in the block.
					//				bColors[J][I].s.fR = pfSrcTexel[LSI_PC_R];
					//				bColors[J][I].s.fG = pfSrcTexel[LSI_PC_G];
					//				bColors[J][I].s.fB = pfSrcTexel[LSI_PC_B];
					//				bColors[J][I].s.fA = pfSrcTexel[LSI_PC_A];
					//			}
					//		}
					//	}
					//}

					//// The local block is filled.  Send it to the filter to determine the best high and low colors to use.
					//LSI_BLOCK bLow;
					//LSI_BLOCK bHi;
					//GetKeyColors( bColors, bHi, bLow,
					//	CStd::Min<LSUINT32>( 4, GetWidth() - (X << 2) ),
					//	CStd::Min<LSUINT32>( 4, GetHeight() - (Y << 2) ) );

					//MakeDxt1Block( bColors, bHi, bLow, pui8Dst );

					
					// Find a free thread.
					for ( LSUINT32 I = 0; I <= LSE_ELEMENTS( tThreads ); ++I ) {
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
								{
									CCriticalSection::CLocker lLock( m_csDxtCrit );
									m_mSingleColorMatches.Reset();
									m_vDxtTableRb4.Reset();
									m_vDxtTableG4.Reset();
								}
								return false;
							}
							break;
						}
						CStd::Sleep( 10 );
					}

					pui8Dst += CDds::DdsBlockSize( _pfFormat ) * ui32Width * LSI_DXT_ROWS;
				//}
				//pui8Src += (ui32SrcStride << 2);
			}

			for ( LSUINT32 I = LSE_ELEMENTS( tThreads ); I--; ) {
				CThreadLib::WaitForThread( tThreads[I], NULL );
				CThreadLib::CloseThread( tThreads[I] );
			}
		}
		catch ( ... ) {
			{
				CCriticalSection::CLocker lLock( m_csDxtCrit );
				m_mSingleColorMatches.Reset();
				m_vDxtTableRb4.Reset();
				m_vDxtTableG4.Reset();
			}
			return false;
		}
		{
			CCriticalSection::CLocker lLock( m_csDxtCrit );
			m_mSingleColorMatches.Reset();
			m_vDxtTableRb4.Reset();
			m_vDxtTableG4.Reset();
		}
		return true;
	}

	/**
	 * Converts the image to DDS format in-place.
	 *
	 * \param _pfFormat The format to which to convert the image.  Must be one of the DXT formats.
	 * \param _pdoOptions Conversion options.
	 * \return Returns true if there are no resource problems.
	 */
	LSBOOL LSE_CALL CImage::ConvertToDdsInPlace( LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS * _pdoOptions ) {
		try {
			CImage iCopy = (*this);
			return iCopy.ConvertToDds( _pfFormat, (*this), _pdoOptions );
		}
		catch ( ... ) {
			return false;
		}
	}

	/**
	 * Pre-multiplies the alpha in-place.  This should not be done more than once on a single image for correct results.
	 */
	LSVOID LSE_CALL CImage::PreMultiplyAlpha() {
		LSFLOAT fVal[4];
		LSUINT32 ui32Width = GetWidth();
		LSUINT32 ui32Height = GetHeight();
		for ( LSUINT32 I = 0; I < TotalMipLevels(); ++I ) {
			ui32Width = CStd::Max<LSUINT32>( GetWidth() >> I, 1 );
			ui32Height = CStd::Max<LSUINT32>( GetHeight() >> I, 1 );
			for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
				for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
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
	 * Adds a normal-map channel to the Z component.
	 */
	LSVOID LSE_CALL CImage::AddNormalMapZ() {
		LSFLOAT fVal[4];
		LSUINT32 ui32Width = GetWidth();
		LSUINT32 ui32Height = GetHeight();
		for ( LSUINT32 I = 0; I < TotalMipLevels(); ++I ) {
			ui32Width = CStd::Max<LSUINT32>( GetWidth() >> I, 1 );
			ui32Height = CStd::Max<LSUINT32>( GetHeight() >> I, 1 );
			for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
				for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
					GetTexelAt( X, Y, fVal, I );
					LSFLOAT fX = fVal[LSI_PC_R] * 2.0f - 1.0f;
					LSFLOAT fY = fVal[LSI_PC_G] * 2.0f - 1.0f;
					LSFLOAT fTemp = 1.0f - ((fX * fX) + (fY * fY));
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
	LSVOID LSE_CALL CImage::Normalize() {
		LSFLOAT fVal[4];
		LSUINT32 ui32Width = GetWidth();
		LSUINT32 ui32Height = GetHeight();
		for ( LSUINT32 I = 0; I < TotalMipLevels(); ++I ) {
			ui32Width = CStd::Max<LSUINT32>( GetWidth() >> I, 1 );
			ui32Height = CStd::Max<LSUINT32>( GetHeight() >> I, 1 );
			for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
				for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
					GetTexelAt( X, Y, fVal, I );
					LSFLOAT fX = fVal[LSI_PC_R] * 2.0f - 1.0f;
					LSFLOAT fY = fVal[LSI_PC_G] * 2.0f - 1.0f;
					LSFLOAT fZ = fVal[LSI_PC_B] * 2.0f - 1.0f;
					LSFLOAT fThisVal = fX * fX +
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
			LSUINT64 ui64RedMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_R )) - 1ULL;
			LSUINT64 ui64GreenMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_G )) - 1ULL;
			LSUINT64 ui64BlueMask = (1ULL << CImageLib::GetComponentBits( LSI_PF_R8G8B8, LSI_PC_B )) - 1ULL;
			// Integer format.
			for ( LSUINT32 Y = GetHeight(); Y--; ) {
				for ( LSUINT32 X = GetWidth(); X--; ) {
					LSUINT64 ui64This = GetTexelAt( LSI_PF_R8G8B8, X, Y );
					LSUINT64 ui64Red = ui64This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_R );
					ui64Red &= ui64RedMask;
					LSUINT64 ui64Green = ui64This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_G );
					ui64Green &= ui64GreenMask;
					LSUINT64 ui64Blue = ui64This >> CImageLib::GetComponentOffset( LSI_PF_R8G8B8, LSI_PC_B );
					ui64Blue &= ui64BlueMask;

					LSUINT64 ui64Avg = (ui64Red + ui64Green + ui64Blue) / 3ULL;

					LSINT64 i64Dif = static_cast<LSINT64>(ui64Red) - static_cast<LSINT64>(ui64Avg);
					if ( i64Dif >= 8LL || i64Dif <= -8LL ) {
						return false;
					}
					i64Dif = static_cast<LSINT64>(ui64Green) - static_cast<LSINT64>(ui64Avg);
					if ( i64Dif >= 8LL || i64Dif <= -8LL ) {
						return false;
					}
					i64Dif = static_cast<LSINT64>(ui64Blue) - static_cast<LSINT64>(ui64Avg);
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

			
			for ( LSUINT32 Y = GetHeight(); Y--; ) {
				LSUINT32 ui32Y = Y * piUseMe->m_ui32RowWidth;
				for ( LSUINT32 X = GetWidth(); X--; ) {
					const LSFLOAT * pfThis = reinterpret_cast<const LSFLOAT *>(&reinterpret_cast<const LSUINT8 *>(piUseMe->GetBufferData())[ui32Y+X*piUseMe->m_ui32TexelSize]);
					if ( pfThis[LSI_PC_R] != pfThis[LSI_PC_G] || pfThis[LSI_PC_R] != pfThis[LSI_PC_B] ) { return false; }
				}
			}
		}

		return true;
	}

	/**
	 * Called after a successful load.  Sets various values based on the loaded data.
	 */
	LSVOID LSE_CALL CImage::PostLoad() {
		if ( CImageLib::IsCompressed( m_pfFormat ) ) {
			m_ui32RowWidth = CDds::DdsScanlineWidth( m_ui32Width, m_pfFormat );
			m_ui32TexelSize = CDds::DdsScanlineWidth( 1, m_pfFormat );
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
	LSBOOL LSE_CALL CImage::CompressChannels( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32MaxSize ) {
		lsc::CBitStream bsTemp;

		const static LSUINT32 ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			bsTemp.Reset();
			LSUINT32 ui32Comp = CImageLib::GetComponentBits( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			LSUINT32 ui32Shift = CImageLib::GetComponentOffset( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
					for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
						LSUINT64 ui64Texel = GetTexelAt( GetFormat(), X, Y ) >> ui32Shift;
						if ( !bsTemp.WriteBits( reinterpret_cast<LSUINT8 *>(&ui64Texel), ui32Comp ) ) { return false; }
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
	LSBOOL LSE_CALL CImage::CompressRaw( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 /*_ui32MaxSize*/ ) {
		if ( !m_tbBuffer.Length() ) { return true; }
		// Compress each mipmap level.
		for ( LSUINT32 I = 0; I < TotalMipLevels(); ++I ) {
#if 0
			if ( !CMisc::CompressToBuffer( reinterpret_cast<const LSUINT8 *>(&GetMipMapBuffers( I )[0]), GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
#else
			if ( !CMisc::WriteUInt( GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
			if ( !CMisc::WriteBytes( reinterpret_cast<const LSUINT8 *>(&GetMipMapBuffers( I )[0]), GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
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
	LSBOOL LSE_CALL CImage::CompressRle( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 /*_ui32MaxSize*/ ) {
		lsc::CBitStream bsTemp;
		const static LSUINT32 ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		bsTemp.Reset();
		LSI_PIXEL_FORMAT pfFormat = GetFormat();
		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			LSUINT32 ui32Comp = CImageLib::GetComponentBits( pfFormat, static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				LSUINT32 ui32Shift = CImageLib::GetComponentOffset( pfFormat, static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
				for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
					for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
						LSUINT64 ui64Texel = GetTexelAt( pfFormat, X, Y ) >> ui32Shift;
						ui64Texel &= (1ULL << ui32Comp) - 1ULL;
						// Now we have just the component.
						//	Count how many follow that are the same.
						LSUINT32 ui32Count = 0;
						
						for ( LSUINT32 J = X + 1; J < GetWidth(); ++J ) {
							LSUINT64 ui64NewTexel = GetTexelAt( pfFormat, J, Y ) >> ui32Shift;
							ui64NewTexel &= (1ULL << ui32Comp) - 1ULL;
							if ( ui64NewTexel != ui64Texel ) { break; }
							++ui32Count;
							if ( ui32Count == 255UL ) { break; }
						}
						if ( ui32Comp >= 32 ) {
							if ( !bsTemp.WriteBytes( reinterpret_cast<LSUINT8 *>(&ui64Texel), 4 ) ) { return false; }
						}
						if ( ui32Comp >= 24UL ) {
							if ( !bsTemp.WriteBytes( reinterpret_cast<LSUINT8 *>(&ui64Texel), 3 ) ) { return false; }
						}
						else if ( ui32Comp >= 16 ) {
							if ( !bsTemp.WriteBytes( reinterpret_cast<LSUINT8 *>(&ui64Texel), 2 ) ) { return false; }
						}
						else {
							if ( !bsTemp.WriteBytes( reinterpret_cast<LSUINT8 *>(&ui64Texel), 1 ) ) { return false; }
						}
						if ( !bsTemp.WriteBytes( reinterpret_cast<LSUINT8 *>(&ui32Count), 1 ) ) { return false; }
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
	LSBOOL LSE_CALL CImage::CompressIndex( CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32MaxSize ) {
		lsc::CBitStream bsTable, bsIndices;
		// First build the table.  This has to be done in a separate pass to allow us to determine how many bits are
		//	needed to store the indices.
		LSUINT32 ui32IndexSize = 0, ui32TableSize = 0, ui32FormatSize = CImageLib::GetFormatSize( GetFormat() );

		CSet<LSI_SORTED_PIXEL> spSorted;
		CSet<LSI_SORTED_PIXEL> spSortedHash[1024];
		LSI_SORTED_PIXEL spThis;
		spThis.ui32Length = ui32FormatSize;
		
		for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
			for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
				spThis.u.ui64Rgb64 = GetTexelAt( GetFormat(), X, Y );
				LSUINT32 ui32R = static_cast<LSUINT32>(LSI_R( CImageLib::IntegerTexelToIntegerTexel( GetFormat(), LSI_PF_R8G8B8A8, spThis.u.ui64Rgb64 ) ));
				LSUINT32 ui32G = static_cast<LSUINT32>(LSI_G( CImageLib::IntegerTexelToIntegerTexel( GetFormat(), LSI_PF_R8G8B8A8, spThis.u.ui64Rgb64 ) ));
				ui32R = ui32R & ((ui32G & 3) << 8);
				// Already in the table?
				if ( !spSortedHash[ui32R].Insert( spThis ) ) { return false; }
			}
		}

		// Combine the hashes.
		for ( LSUINT32 I = LSE_ELEMENTS( spSortedHash ); I--; ) {
			ui32TableSize += spSortedHash[I].Length();
		}
		spSorted.AllocateAtLeast( ui32TableSize );
		for ( LSUINT32 I = LSE_ELEMENTS( spSortedHash ); I--; ) {
			for ( LSUINT32 J = spSortedHash[I].Length(); J--; ) {
				if ( !spSorted.Insert( spSortedHash[I].GetByIndex( J ) ) ) { return false; }
			}
			spSortedHash[I].Reset();
		}

		ui32TableSize = spSorted.Length();
		for ( LSUINT32 I = 0; I < ui32TableSize; ++I ) {
			if ( !bsTable.WriteBytes( spSorted.GetByIndex( I ).u.ui8Rgb, ui32FormatSize ) ) { return false; }
		}

		// How many bits per index are needed to index fully into the table?
		// The highest index value will be table size - 1, so check using that number.
		ui32IndexSize = CStd::HighestBit( static_cast<LSUINT32>(ui32TableSize - 1) ) + 1;

		for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
			for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
				spThis.u.ui64Rgb64 = GetTexelAt( GetFormat(), X, Y );
				// Already in the table?
				LSUINT32 I = 0;
				if ( !spSorted.GetItemIndex( spThis, I ) ) { return false; }
				if ( !bsIndices.WriteBits( reinterpret_cast<LSUINT8 *>(&I), ui32IndexSize ) ) { return false; }
			}
		}

		// Compress the table.
		CMisc::LSC_BUFFER bTable;
		if ( !bTable.Append( reinterpret_cast<LSUINT8 *>(&ui32TableSize), sizeof( ui32TableSize ) ) ) { return false; }
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
		for ( LSUINT32 I = 0; I < TotalMipLevels(); ++I ) {
			if ( !CMisc::CompressToBuffer( reinterpret_cast<const LSUINT8 *>(&GetMipMapBuffers( I )[0]), GetMipMapBuffers( I ).Length(), _bBuffer ) ) { return false; }
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
	LSBOOL LSE_CALL CImage::LoadCompressedChannels( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut ) {
		if ( !_tbOut.Length() ) { return false; }
		CStd::MemSet( &_tbOut[0], 0, _tbOut.Length() );
		if ( _ui32DataLen == 0 ) { return false; }
		lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;

		const static LSUINT32 ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			//bsTemp.Reset();
			LSUINT32 ui32Comp = CImageLib::GetComponentBits( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				bTemp.ResetNoDealloc();
				if ( !CMisc::DecompressToBuffer( bsTemp, bTemp ) ) { return false; }
				if ( bTemp.Length() == 0 && GetHeight() * GetWidth() != 0 ) { return false; }
				if ( bTemp.Length() == 0 ) { continue; }
				lsc::CBitStream bsThisLine;
				bsThisLine.Borrow( &bTemp[0], bTemp.Length() );
				for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
					for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
						LSUINT64 ui64ThisTexel = 0;
						if ( !bsThisLine.ReadBits( reinterpret_cast<LSUINT8 *>(&ui64ThisTexel), ui32Comp ) ) { return false; }

						// The image buffer has been filled with 0's so we can just | our new component into place.
						ui64ThisTexel <<= CImageLib::GetComponentOffset( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );

						(*reinterpret_cast<LSUINT64 *>(&_tbOut[Y*m_ui32RowWidth+X*m_ui32TexelSize])) |= ui64ThisTexel;
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
	LSBOOL LSE_CALL CImage::LoadCompressedRaw( LSUINT8 _ui8MipLevels, const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
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
		CExternalByteStream ebsStream( static_cast<const LSUINT8 *>(_pui8FileData), _ui32DataLen );
#if 0
		CMisc::LSC_BUFFER bTemp;
		if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }
		if ( !_tbOut.Resize( bTemp.Length() ) ) { return false; }
		CStd::MemCpy( &_tbOut[0], &bTemp[0], bTemp.Length() );
#else
		LSUINT32 ui32Size = ebsStream.ReadUInt32();
		if ( !ui32Size || ui32Size > ebsStream.Length() - ebsStream.GetPos() ) { return false; }
		if ( !_tbOut.Resize( ui32Size ) ) { return false; }
		CStd::MemCpy( &_tbOut[0], ebsStream.GetCurBufferPointer(), ui32Size );
		ebsStream.SkipBytes( ui32Size );
#endif	// #if 0

		// The mipmaps.
		--_ui8MipLevels;
		if ( !_vMipMaps.Resize( _ui8MipLevels ) ) { return false; }
		for ( LSUINT32 I = 0; I < _ui8MipLevels; ++I ) {
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
	LSBOOL LSE_CALL CImage::LoadCompressedBitwiseRle( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut ) {
		if ( !_tbOut.Length() ) { return false; }
		CStd::MemSet( &_tbOut[0], 0, _tbOut.Length() );
		if ( _ui32DataLen == 0 ) { return false; }
		lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;

		const static LSUINT32 ui32Components[] = {
			LSI_PC_R,
			LSI_PC_G,
			LSI_PC_B,
			LSI_PC_A
		};
		if ( !CMisc::DecompressToBuffer( bsTemp, bTemp ) ) { return false; }
		if ( bTemp.Length() == 0 && GetHeight() * GetWidth() != 0 ) { return false; }
		if ( bTemp.Length() == 0 ) { return true; }
		CExternalByteStream ebsThis( &bTemp[0], bTemp.Length() );

		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( ui32Components ); ++I ) {
			LSUINT32 Y = 0, X = 0, ui32Total = 0, ui32Max = GetWidth() * GetHeight();
			bsTemp.Reset();
			LSUINT32 ui32Comp = CImageLib::GetComponentBits( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );
			if ( ui32Comp ) {
				while ( ui32Total < ui32Max ) {
					LSUINT64 ui64ThisTexel = 0;
					LSUINT32 ui32Count = 0;
					if ( ui32Comp >= 32 ) {
						if ( !ebsThis.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui64ThisTexel), 4 ) ) { return false; }
					}
					if ( ui32Comp >= 24UL ) {
						if ( !ebsThis.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui64ThisTexel), 3 ) ) { return false; }
					}
					else if ( ui32Comp >= 16 ) {
						if ( !ebsThis.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui64ThisTexel), 2 ) ) { return false; }
					}
					else {
						if ( !ebsThis.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui64ThisTexel), 1 ) ) { return false; }
					}
					if ( !ebsThis.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui32Count), 1 ) ) { return false; }

					// The image buffer has been filled with 0's so we can just | our new component into place.
					ui64ThisTexel <<= CImageLib::GetComponentOffset( GetFormat(), static_cast<LSI_PIXEL_COMPONENTS>(ui32Components[I]) );

					for ( LSUINT32 J = 0; J <= ui32Count; ++J ) {
						(*reinterpret_cast<LSUINT64 *>(&_tbOut[Y*m_ui32RowWidth+X++*m_ui32TexelSize])) |= ui64ThisTexel;
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
	LSBOOL LSE_CALL CImage::LoadCompressedIndices( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut ) {
		if ( !_tbOut.Length() ) { return false; }
		CStd::MemSet( &_tbOut[0], 0, _tbOut.Length() );
		if ( _ui32DataLen == 0 ) { return false; }

		lsc::CBitStream bsTemp;
		bsTemp.Borrow( _pui8FileData, _ui32DataLen );
		CMisc::LSC_BUFFER bTableEx, bTable, bIndices;

		LSUINT32 ui32IndexSize = 0, ui32TableSize = 0, ui32FormatSize = CImageLib::GetFormatSize( GetFormat() );
		if ( !CMisc::DecompressToBuffer( bsTemp, bTableEx ) ) { return false; }
		CExternalByteStream ebsTable( &bTableEx[0], bTableEx.Length() );
		// Read the table size.
		if ( !ebsTable.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui32TableSize), sizeof( ui32TableSize ) ) ) { return false; }
		ui32IndexSize = CStd::HighestBit( static_cast<LSUINT32>(ui32TableSize - 1) ) + 1;
		if ( !bTable.Resize( ui32TableSize * ui32FormatSize + sizeof( LSUINT64 ) ) ) { return false; }
		if ( !ebsTable.ReadBytes( reinterpret_cast<LSUINT8 *>(&bTable[0]), ui32TableSize * ui32FormatSize ) ) { return false; }

		if ( !bTable.Length() ) { return false; }

		// Decompress the indices.
		if ( !CMisc::DecompressToBuffer( bsTemp, bIndices ) ) { return false; }
		if ( !bIndices.Length() ) { return false; }
		bsTemp.Borrow( &bIndices[0], bIndices.Length() );

		LSUINT64 ui64Mask = ~(~(1ULL << (ui32FormatSize * 8)) + 1ULL);
		for ( LSUINT32 Y = 0; Y < GetHeight(); ++Y ) {
			for ( LSUINT32 X = 0; X < GetWidth(); ++X ) {
				LSUINT32 ui32Index = 0;
				if ( !bsTemp.ReadBits( reinterpret_cast<LSUINT8 *>(&ui32Index), ui32IndexSize ) ) { return false; }
				if ( ui32Index >= ui32TableSize ) { return false; }

				LSUINT64 ui64Texel = (*reinterpret_cast<LSUINT64 *>(&bTable[ui32FormatSize*ui32Index]));
				ui64Texel &= ui64Mask;
				(*reinterpret_cast<LSUINT64 *>(&_tbOut[Y*m_ui32RowWidth+X*m_ui32TexelSize])) |= ui64Texel;
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
	LSBOOL LSE_CALL CImage::LoadDxt( LSUINT8 _ui8MipLevels, const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen, CImageLib::CTexelBuffer &_tbOut,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {
		if ( _ui8MipLevels == 0 ) { return false; }
		// First level is manual.
		CExternalByteStream ebsStream( static_cast<const LSUINT8 *>(_pui8FileData), _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;
		if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }
		if ( !_tbOut.Resize( bTemp.Length() ) ) { return false; }
		CStd::MemCpy( &_tbOut[0], &bTemp[0], bTemp.Length() );

		// The mipmaps.
		--_ui8MipLevels;
		if ( !_vMipMaps.Resize( _ui8MipLevels ) ) { return false; }
		for ( LSUINT32 I = 0; I < _ui8MipLevels; ++I ) {
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
	LSBOOL LSE_CALL CImage::LoadLsi( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen ) {

		CExternalByteStream ebsStream( static_cast<const LSUINT8 *>(_pui8FileData), _ui32DataLen );
		CMisc::LSC_BUFFER bTemp;
		if ( !CMisc::DecompressToBuffer( ebsStream, bTemp ) ) { return false; }

		CExternalByteStream ebsParms( &bTemp[0], bTemp.Length() );
		m_pfFormat = static_cast<LSI_PIXEL_FORMAT>(0);
		if ( !ebsParms.ReadBytes( reinterpret_cast<LSUINT8 *>(&m_ui32Width), sizeof( m_ui32Width ) ) ) { Reset(); return false; }
		if ( !ebsParms.ReadBytes( reinterpret_cast<LSUINT8 *>(&m_ui32Height), sizeof( m_ui32Height ) ) ) { Reset(); return false; }
		if ( !ebsParms.ReadBytes( reinterpret_cast<LSUINT8 *>(&m_pfFormat), 1 ) ) { Reset(); return false; }
		LSUINT8 ui8MipMaps = 0;
		if ( !ebsParms.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui8MipMaps), 1 ) ) { Reset(); return false; }
		LSUINT8 ui8Srgb = 1;
		if ( !ebsParms.ReadBytes( reinterpret_cast<LSUINT8 *>(&ui8Srgb), 1 ) ) { Reset(); return false; }
		m_gColorSpace = ui8Srgb ? LSI_G_sRGB : LSI_G_LINEAR;
		PostLoad();	// Sets m_ui32RowWidth and m_ui32TexelSize.

		LSI_COMPRESSIONS cComp = static_cast<LSI_COMPRESSIONS>(0);
		CExternalByteStream ebsNext( &_pui8FileData[static_cast<LSUINT32>(ebsStream.GetPos())], static_cast<LSUINT32>(bTemp.Length() - ebsStream.GetPos()) );
		if ( !ebsNext.ReadBytes( reinterpret_cast<LSUINT8 *>(&cComp), 1 ) ) { Reset(); return false; }

		LSUINT32 ui32Offset = static_cast<LSUINT32>(ebsStream.GetPos() + ebsNext.GetPos());
		LSUINT32 ui32Size = static_cast<LSUINT32>(_ui32DataLen - ebsStream.GetPos() - ebsNext.GetPos());
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
	LSBOOL LSE_CALL CImage::LoadFreeImage( const LSUINT8 * _pui8FileData, LSUINT32 _ui32DataLen ) {
		FIMEMORY * pmMem = ::FreeImage_OpenMemory( const_cast<LSUINT8 *>(_pui8FileData), _ui32DataLen );
		if ( !pmMem ) { return false; }

		FREE_IMAGE_FORMAT fifFormat = ::FreeImage_GetFileTypeFromMemory( pmMem, 0 );
		if ( FIF_UNKNOWN == fifFormat ) {
			::FreeImage_CloseMemory( pmMem );
			return false;
		}

		FIBITMAP * pbData = ::FreeImage_LoadFromMemory( fifFormat, pmMem, 0 );

		LSUINT32 ui32Width = ::FreeImage_GetWidth( pbData );
		LSUINT32 ui32Height = ::FreeImage_GetHeight( pbData );

#define FreeImage_GetScanLine( DATA, Y )	FreeImage_GetScanLine( DATA, ui32Height - Y - 1 )
		LSBOOL bRes = false;
#define LSI_CREATE( FORMAT )			LSBOOL bCreated = CreateBlank( (FORMAT), ui32Width, ui32Height );						\
	LSUINT32 ui32AShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_A );													\
	LSUINT32 ui32RShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_R );													\
	LSUINT32 ui32GShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_G );													\
	LSUINT32 ui32BShift = CImageLib::GetComponentOffset( (FORMAT), LSI_PC_B )
#define LSI_MAKE_COLOR( R, G, B, A )	LSUINT64 ui64Color = static_cast<LSUINT64>((static_cast<LSUINT64>(R) << ui32RShift) |	\
	(static_cast<LSUINT64>(G) << ui32GShift) |																					\
	(static_cast<LSUINT64>(B) << ui32BShift) |																					\
	(static_cast<LSUINT64>(A) << ui32AShift));																					\
	(*reinterpret_cast<LSUINT64 *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])) = ui64Color


		FREE_IMAGE_TYPE fitType = ::FreeImage_GetImageType( pbData );
		switch ( fitType ) {
			case FIT_BITMAP : {
				switch ( ::FreeImage_GetBPP( pbData ) ) {
					case 1 : {
						LSI_CREATE( LSI_PF_R8G8B8A8 );
						if ( bCreated ) {
							const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( pbData );
							if ( prgbqPal ) {
								for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
									const LSUINT8 * pui8Data = ::FreeImage_GetScanLine( pbData, Y );
									for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
										LSUINT8 ui8Bit = (pui8Data[X>>3] >> (X % 8)) & 1;
										LSI_MAKE_COLOR( prgbqPal[ui8Bit].rgbRed, prgbqPal[ui8Bit].rgbGreen,
											prgbqPal[ui8Bit].rgbBlue, prgbqPal[ui8Bit].rgbReserved );
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
								for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
									const LSUINT8 * pui8Data = ::FreeImage_GetScanLine( pbData, Y );
									for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
										LSUINT8 ui8Bit = (pui8Data[X>>1] >> (X % 2 << 2)) & 0xF;
										LSI_MAKE_COLOR( prgbqPal[ui8Bit].rgbRed, prgbqPal[ui8Bit].rgbGreen,
											prgbqPal[ui8Bit].rgbBlue, prgbqPal[ui8Bit].rgbReserved );
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
								for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
									const LSUINT8 * pui8Data = ::FreeImage_GetScanLine( pbData, Y );
									for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
										LSUINT8 ui8Bit = pui8Data[X];
										LSI_MAKE_COLOR( prgbqPal[ui8Bit].rgbRed, prgbqPal[ui8Bit].rgbGreen,
											prgbqPal[ui8Bit].rgbBlue, prgbqPal[ui8Bit].rgbReserved );
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
							for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
								const LSUINT16 * pui32Data = reinterpret_cast<LSUINT16 *>(::FreeImage_GetScanLine( pbData, Y ));
								for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
									LSUINT16 ui16Bit = pui32Data[X];
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
							for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
								const RGBTRIPLE * prgbtData = reinterpret_cast<RGBTRIPLE *>(::FreeImage_GetScanLine( pbData, Y ));
								for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
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
							for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
								const RGBQUAD * prgbqData = reinterpret_cast<RGBQUAD *>(::FreeImage_GetScanLine( pbData, Y ));
								for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
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
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const LSUINT16 * pui32Data = reinterpret_cast<LSUINT16 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							LSUINT16 ui16Bit = pui32Data[X];
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
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const LSUINT32 * pui32Data = reinterpret_cast<LSUINT32 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							LSFLOAT fBit = static_cast<LSFLOAT>(pui32Data[X] / static_cast<LSDOUBLE>(LSE_MAXU32));
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_UINT32
			}
			case FIT_INT32 : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const LSINT32 * pui32Data = reinterpret_cast<LSINT32 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							LSFLOAT fBit = static_cast<LSFLOAT>(pui32Data[X] / static_cast<LSDOUBLE>(LSSTD_MAX_INT32));
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_INT32
			}
			case FIT_FLOAT : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const LSFLOAT * pfData = reinterpret_cast<LSFLOAT *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							LSFLOAT fBit = pfData[X];
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_FLOAT
			}
			case FIT_DOUBLE : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const LSDOUBLE * pdData = reinterpret_cast<LSDOUBLE *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							LSFLOAT fBit = static_cast<LSFLOAT>(pdData[X]);
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = fBit;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
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
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const FIRGB16 * pfrgb16Data = reinterpret_cast<FIRGB16 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
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
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const FIRGBA16 * pfrgba16Data = reinterpret_cast<FIRGBA16 *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
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
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const FIRGBF * pfData = reinterpret_cast<FIRGBF *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = pfData[X].red;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = pfData[X].green;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = pfData[X].blue;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = 1.0f;
						}
					}
					bRes = true;
				}
				break;	// FIT_RGBF
			}
			case FIT_RGBAF : {
				if ( CreateBlank( LSI_PF_R32G32B32A32F, ui32Width, ui32Height ) ) {
					for ( LSUINT32 Y = 0; Y < ui32Height; ++Y ) {
						const FIRGBAF * pfData = reinterpret_cast<FIRGBAF *>(::FreeImage_GetScanLine( pbData, Y ));
						for ( LSUINT32 X = 0; X < ui32Width; ++X ) {
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[3] = pfData[X].red;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[2] = pfData[X].green;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[1] = pfData[X].blue;
							reinterpret_cast<LSFLOAT *>(&m_tbBuffer[Y*m_ui32RowWidth+X*m_ui32TexelSize])[0] = pfData[X].alpha;
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
	LSUINT32 LSH_CALL CImage::ResampleThread( LSVOID * _lpParameter ) {
		LSI_MIPMAP_DATA * pmdData = static_cast<LSI_MIPMAP_DATA *>(_lpParameter);
		return pmdData->piSrc->Resample( pmdData->ui32NewWidth, pmdData->ui32NewHeight,
			pmdData->fFilter, (*pmdData->ptbDst), pmdData->fGamma, pmdData->amAddressMode, pmdData->fGammaScale );
	}

	/**
	 * Takes an array of 16 texels in floating-point format and determines the two colors best suited to represent
	 *	the given colors via simple interpolation.  In other words, it determines the two key colors for a DXT
	 *	block.
	 *
	 * \param _bBlock The input block of 4-by-4 texels.
	 * \param _bMax Upon return, this holds the computed maximum color value.
	 * \param _bMin Upon return, this holds the computed minimum color value.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _fAlphaHigh The highest alpha value in the block.
	 * \param _fAlphaLow The lowest alpha value in the block.
	 * \param _pfFormat The DXTn format to which the block is to be converted.
	 * \param _doOptions Options for the conversion.
	 */
	LSVOID LSE_CALL CImage::GetKeyColors( const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
		LSFLOAT _fAlphaHigh, LSFLOAT _fAlphaLow, LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS &_doOptions ) {
		if ( !_ui32Width && !_ui32Height ) { return; }	// Not possible.
		/*static const LSFLOAT fErrorRes[4] = {
			31.0f,
			63.0f,
			31.0f,
			1.0f,
		};*/

		// For each channel.
		CFixedSet<LSFLOAT, 16> sColors;
		CStaticMap<LSFLOAT, LSUINT32, 16> mUniqueValues[4];
		CStaticMap<LSI_BLOCK, LSUINT32, 16> mUniqueColors;
		for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				mUniqueColors[_bBlock[Y][X]]++;
			}
		}
		//CVectorPoD<LSFLOAT> vColors;
		for ( LSUINT32 I = 3; I--; ) {
			// We want only unique colors, and we want them sorted.
			sColors.Reset();
			//vColors.ResetNoDealloc();
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				for ( LSUINT32 X = _ui32Width; X--; ) {
					//LSFLOAT fValue = ::floorf( _bBlock[Y][X].fValues[I] * fErrorRes[I] + 0.5f ) / fErrorRes[I];
					//LSFLOAT fValue = ::floorf( _bBlock[Y][X].fValues[I] * fErrorRes[I] ) / fErrorRes[I];
					LSFLOAT fValue = _bBlock[Y][X].fValues[I];
					sColors.Insert( fValue );
					//vColors.Push( _bBlock[Y][X].fValues[I] );
					mUniqueValues[I][_bBlock[Y][X].fValues[I]]++;
				}
			}

			// If there is only one color, try to match it exactly by putting it in the middle.
			//	The two interpolated middle colors are more precise than the edge colors.
			if ( sColors.Length() == 1 ) {
				_bMax.fValues[I] = _bMin.fValues[I] = sColors.GetByIndex( 0 );
				//continue;
				/*if ( I == 3 ) {
					// Do not perform for alpha channels.
					continue;
				}*/
				LSUINT32 ui32Colors = (_pfFormat == LSI_PF_DXT1 && _fAlphaLow < _doOptions.fAlphaThresh) ? 3 : 4;
				LSBOOL bOmitLast = ui32Colors == 3;
				if ( GetBestSingleColor( mUniqueColors, _bBlock, _bMax, _bMin, _ui32Width, _ui32Height, bOmitLast, _doOptions ) ) {
					// We are done!
					return;
				}
				continue;
			}
			
			// If there are 2 colors, they are the high and low.
			if ( sColors.Length() == 2 ) {
				_bMax.fValues[I] = sColors.GetByIndex( 1 );
				_bMin.fValues[I] = sColors.GetByIndex( 0 );
				continue;
			}

			// Find the average slope of the colors using linear regression.
			LSFLOAT fM = static_cast<LSFLOAT>(sColors.Length());
			//CAlgorithm::QSort( &vColors[0], vColors.Length() );
			LSFLOAT fSumX = 0.0f;
			LSFLOAT fSumXSq = 0.0f;
			LSFLOAT fSumXByY = 0.0f;
			LSFLOAT fSumY = 0.0f;
			LSFLOAT fMax = -1.0f, fMin = 2.0f;
			for ( LSUINT32 J = 0; J < fM; ++J ) {
				LSFLOAT fThis = sColors.GetByIndex( J );
				//LSFLOAT fThis = vColors[J];

				fMax = CStd::Max( fMax, fThis );
				fMin = CStd::Min( fMin, fThis );
				fSumXByY += static_cast<LSFLOAT>(J) * fThis;
				fSumX += static_cast<LSFLOAT>(J);
				fSumY += fThis;
				fSumXSq += static_cast<LSFLOAT>(J) * static_cast<LSFLOAT>(J);
			}

			LSFLOAT fW1 = (fM * fSumXByY - fSumX * fSumY) / (fM * fSumXSq - fSumX * fSumX);
			LSFLOAT fW0 = (1.0f / fM) * fSumY - (fW1 / fM) * fSumX;
			// Plug it into the linear equation to get the 2 end points.
			LSFLOAT fLow = fW0;	// Simplified 0 term for X.
			LSFLOAT fHi = (fM - 1.0f) * fW1 + fW0;
			_bMin.fValues[I] = CStd::Clamp( fLow, fMin, fMax );
			_bMax.fValues[I] = CStd::Clamp( fHi, fMin, fMax );
		}

		// If the min and max are the same, there is no need for refinement.
		RefineKeyColors( mUniqueValues, mUniqueColors, _bBlock, _bMax, _bMin, _ui32Width, _ui32Height, _doOptions.ui32SecondStageIterations, _fAlphaHigh, _fAlphaLow, _pfFormat,
			_doOptions );

		for ( LSUINT32 I = 4; I--; ) {
			_bMax.fValues[I] = CStd::Clamp( _bMax.fValues[I], 0.0f, 1.0f );
			_bMin.fValues[I] = CStd::Clamp( _bMin.fValues[I], 0.0f, 1.0f );
		}
	}

	/**
	 * Refines the given key colors iteratively, with each iteration bringing it closer to the most optimal solution.
	 *
	 * \param _bUniqueValues The unique values in each channel in the block, along with the number of times the value appears.
	 * \param _bUniqueColors The unique colors in the block, along with the number of times the value appears.
	 * \param _bBlock The input block of 4-by-4 texels.
	 * \param _bMax Upon return, this holds the computed maximum color value.
	 * \param _bMin Upon return, this holds the computed minimum color value.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _ui32Iters Iteration count for the 2nd phase.
	 * \param _fAlphaHigh The highest alpha value in the block.
	 * \param _fAlphaLow The lowest alpha value in the block.
	 * \param _pfFormat The DXTn format to which the block is to be converted.
	 * \param _doOptions Options for the conversion.
	 */
	LSVOID LSE_CALL CImage::RefineKeyColors( const CStaticMap<LSFLOAT, LSUINT32, 16> _bUniqueValues[4], const CStaticMap<LSI_BLOCK, LSUINT32, 16> &_bUniqueColors,
		const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
		LSUINT32 _ui32Iters, LSFLOAT /*_fAlphaHigh*/, LSFLOAT _fAlphaLow, LSI_PIXEL_FORMAT _pfFormat, const LSI_DXT_OPTIONS &_doOptions ) {
		// For the 3rd phase, we want to determine the amount of error the given key colors will cause over the given block.
		//	Firstly, the key colors can be matched at any of their 4 interpolation points.  We determine amount of error by finding the point
		//	with the lowest amount of error.

		// Get the unique colors.
		//CSet<LSI_BLOCK> sUnique;
		/*for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				sUnique.Insert( _bBlock[Y][X] );
			}
		}*/

		/*LSI_BLOCK bBestLow = _bMin;
		LSI_BLOCK bBestHi = _bMax;
		LSFLOAT fLowError[4] = {
			2000.0f,
			2000.0f,
			2000.0f,
			2000.0f,
		};*/
		/*LSFLOAT fMultiplier[4] = {
			5.0f,
			5.0f,
			5.0f,
			5.0f,
		};
		LSFLOAT fLastError[4] = {
			10.0f,
			10.0f,
			10.0f,
			10.0f,
		};*/
		LSBOOL bSkip[4] = {
			_bUniqueValues[0].Length() < 2,
			_bUniqueValues[1].Length() < 2,
			_bUniqueValues[2].Length() < 2,
			_bUniqueValues[3].Length() < 2,
		};
		/*static const LSFLOAT fErrorRes[4] = {
			31.0f,
			63.0f,
			31.0f,
			1.0f,
		};*/

		// Determine if this block will have an alpha component that would cause the color keys to be 3 colors instead of 4.
		LSUINT32 ui32Colors = (_pfFormat == LSI_PF_DXT1 && _fAlphaLow < _doOptions.fAlphaThresh) ? 3 : 4;
		LSBOOL bOmitLast = ui32Colors == 3;

		/*LSI_BLOCK bBestMin = _bMin, bBestMax = _bMax;
		LSFLOAT fMinError[3] = {
			100.0f,
			100.0f,
			100.0f,
		};*/
		for ( ; _ui32Iters--; ) {
			// First create the 4 points that can be matched.
			LSI_BLOCK bMatchPoints[4];
			Get4Colors( _bMax, _bMin, bMatchPoints, bOmitLast );

			// For each channel.
			for ( LSUINT32 I = 3; I--; ) {
				if ( bSkip[I] ) { continue; }
				// Make the error "buckets".
				LSFLOAT fError[4] = { 0.0f };
				// The number of entries in each bucket.  All buckets must be filled so if any are 0
				//	we have to make a second pass to add values into them.
				LSUINT32 ui32Total[4] = { 0 };
				LSUINT32 ui32TotalBucketsUsed = 0;

				// For each texel.
				for ( LSUINT32 J = _bUniqueValues[I].Length(); J--; ) {
					LSUINT32 ui32Match = 0;
					LSFLOAT fMinError = GetError( I, bMatchPoints, _bUniqueValues[I].GetKeyByIndex( J ), ui32Match, bOmitLast );

					// Add its error to the appropriate bucket.
					if ( fMinError < 0.0f ) {
						fMinError = -fMinError * fMinError;
					}
					else {
						fMinError *= fMinError;
					}
					fError[ui32Match] -= fMinError * _bUniqueValues[I].GetByIndex( J );
					if ( !ui32Total[ui32Match] ) {
						++ui32TotalBucketsUsed;
					}
					ui32Total[ui32Match] += _bUniqueValues[I].GetByIndex( J );
				}
				// If only one bucket was used, there is no need to make any adjustments.
				if ( ui32TotalBucketsUsed == 1 ) {
					// If that bucket has a 0 error, skip this channel from now on.
					/*for ( LSUINT32 A = ui32Colors; A--; ) {
						if ( ui32Total[A] ) {
							if ( fError[A] == 0.0f ) {
								bSkip[I] = true;
								break;
							}
						}
					}
					if ( bSkip[I] ) { continue; }*/
					//bSkip[I] = true;
					//continue;
				}
				// Check that all buckets have been filled.
				/*for ( LSUINT32 A = ui32Colors; A--; ) {
					if ( !ui32Total[A] ) {
						// Bucket is empty.  Get the nearest point on both sides of the bucket.
						//	Stated another way, get the nearest positive and negative displacements.
						LSFLOAT fPos = 10.0f, fNeg = -10.0f;
						for ( LSUINT32 Y = _ui32Height; Y--; ) {
							for ( LSUINT32 X = _ui32Width; X--; ) {
								LSFLOAT fThisError = bMatchPoints[A].fValues[I] - _bBlock[Y][X].fValues[I];
								if ( fThisError < 0.0f ) {
									fNeg = CStd::Max( fNeg, fThisError );
								}
								else {
									fPos = CStd::Min( fPos, fThisError );
								}
							}
						}
						LSBOOL bSquareMe = false;
						if ( fPos < 2.0f && fNeg > -2.0f ) {
							// Both a positive and negative match.
							fError[A] = fPos + fNeg;
							bSquareMe = true;
						}
						else if ( fPos < 2.0f ) {
							// A positive match only.
							fError[A] = fPos;
						}
						else {
							// A negative match only.
							fError[A] = fNeg;
						}

						if ( bSquareMe ) {
							if ( fError[A] < 0.0f ) {
								fError[A] = fError[A] * fError[A];
							}
							else {
								fError[A] = -fError[A] * fError[A];
							}
						}
						else {
							fError[A] = -fError[A];
						}
					}
				}*/

				// Normalize the error buckets.
				//LSFLOAT fTotalError = 0.0f;
				for ( LSUINT32 A = ui32Colors; A--; ) {
					fError[A] /= _ui32Width * _ui32Height;
					//fTotalError += CMathLib::Abs( fError[A] );

					//fError[A] *= fMultiplier[I];
				}
				/*if ( fTotalError < fLowError[I] ) {
					fLowError[I] = fTotalError;
					bBestLow = _bMin;
					bBestHi = _bMax;
				}*/
				/*if ( fLastError[I] < fTotalError ) {
					// Last loop produced better results.
					_bMin = bBestLow;
					_bMax = bBestHi;
					continue;
				}
				fLastError[I] = fTotalError;*/
				/*fMultiplier[I] *= 0.8f;//0.95f;
				fMultiplier[I] = CStd::Max( fMultiplier[I], 1.0f );*/


				// Apply linear regression to the error table.
				LSFLOAT fM = static_cast<LSFLOAT>(ui32TotalBucketsUsed);
				LSFLOAT fSumX = 0.0f;
				LSFLOAT fSumXSq = 0.0f;
				LSFLOAT fSumXByY = 0.0f;
				LSFLOAT fSumY = 0.0f;
				for ( LSUINT32 J = 0; J < ui32Colors; ++J ) {
					if ( !ui32Total[J] ) { continue; }
					LSFLOAT fThis = fError[J];
					fSumXByY += static_cast<LSFLOAT>(J) * fThis;
					fSumX += static_cast<LSFLOAT>(J);
					fSumY += fThis;
					fSumXSq += static_cast<LSFLOAT>(J) * static_cast<LSFLOAT>(J);
				}

				LSFLOAT fW1 = (fM * fSumXByY - fSumX * fSumY) / (fM * fSumXSq - fSumX * fSumX);
				LSFLOAT fW0 = (1.0f / fM) * fSumY - (fW1 / fM) * fSumX;
				// Plug it into the linear equation to get the 2 end points.
				LSFLOAT fLow = fW0;	// Simplified 0 term for X.
				LSFLOAT fHi = (fM - 1.0f) * fW1 + fW0;
				fLow *= 0.125f;
				fHi *= 0.125f;
				if ( _bMin.fValues[I] + fLow == _bMin.fValues[I] && _bMax.fValues[I] + fHi == _bMax.fValues[I] ) {
					// The values have converged.
					bSkip[I] = true;
					continue;
				}
				_bMin.fValues[I] += fLow;
				_bMax.fValues[I] += fHi;


				// This could cause the low and high to change order.
				fLow = _bMin.fValues[I];
				_bMin.fValues[I] = CStd::Min( _bMin.fValues[I], _bMax.fValues[I] );
				_bMax.fValues[I] = CStd::Max( _bMax.fValues[I], fLow );
				
				/*LSFLOAT fThisError = GetError( I, _bBlock, _bMax, _bMin, _ui32Width, _ui32Height, bOmitLast );
				if ( fThisError < fMinError[I] ) {
					fMinError[I] = fThisError;
					bBestMin.fValues[I] = _bMin.fValues[I];
					bBestMax.fValues[I] = _bMax.fValues[I];
				}*/
			}
		}
		/*_bMin = bBestMin;
		_bMax = bBestMax;*/

#if 0	// This seems only to decrease the quality of the image.
		// Next, check endpoints on each channel separately by manually adjusting them and checking for a better result.
		LSI_BLOCK bMatchPoints[4];
		Get4Colors( _bMax, _bMin, bMatchPoints, bOmitLast );
		for ( LSUINT32 I = 4; I--; ) {
			LSUINT32 ui32Low = static_cast<LSUINT32>(::floorf( CStd::Clamp( _bMin.fValues[I], 0.0f, 1.0f ) * fErrorRes[I] + 0.5f ));
			LSUINT32 ui32High = static_cast<LSUINT32>(::floorf( CStd::Clamp( _bMax.fValues[I], 0.0f, 1.0f ) * fErrorRes[I] + 0.5f ));
			LSUINT32 ui32BestLow = ui32Low;
			LSUINT32 ui32BestHigh = ui32High;
			LSFLOAT fError = 0.0f;
			LSFLOAT fNormalizer = 1.0f / (_ui32Height * _ui32Width);
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				for ( LSUINT32 X = _ui32Width; X--; ) {
					LSUINT32 ui32Match;
					fError += ::fabsf( GetError( I, bMatchPoints, _bBlock[Y][X], ui32Match, bOmitLast ) );
				}
			}
			fError *= fNormalizer;

			// Within a certain range, guess-and-check.
#define LSI_SEARCH_RANGE	30
			LSINT32 i32HighL = static_cast<LSINT32>(ui32Low + LSI_SEARCH_RANGE);
			LSINT32 i32HighH = static_cast<LSINT32>(ui32High + LSI_SEARCH_RANGE);
			for ( LSINT32 i32L = static_cast<LSINT32>(ui32Low - LSI_SEARCH_RANGE); i32L <= i32HighL; ++i32L ) {
#undef LSI_SEARCH_RANGE
				if ( i32L < 0 ) { continue; }
				if ( i32L > static_cast<LSINT32>(fErrorRes[I]) ) { break; }
				for ( LSINT32 i32H = i32L; i32H <= i32HighH; ++i32H ) {
					if ( i32H > static_cast<LSINT32>(fErrorRes[I]) || i32L == i32H ) { continue; }
					LSI_BLOCK bNewLow, bNewHi;
					
					bNewLow.fValues[I] = i32L / fErrorRes[I];
					bNewHi.fValues[I] = i32H / fErrorRes[I];
					LSI_BLOCK bTempPoints[4];
					LSFLOAT fColors[4];
					// Create the points that would be made by these new end points.
					Get4Colors( I, bNewHi, bNewLow, fColors, bOmitLast );
					bTempPoints[0].fValues[I] = fColors[0];
					bTempPoints[1].fValues[I] = fColors[1];
					bTempPoints[2].fValues[I] = fColors[2];
					bTempPoints[3].fValues[I] = fColors[3];
					
					// Get the new amount of error.
					LSFLOAT fThisError = 0.0f;
					for ( LSUINT32 Y = _ui32Height; Y--; ) {
						for ( LSUINT32 X = _ui32Width; X--; ) {
							LSUINT32 ui32Match;
							fThisError += ::fabsf( GetError( I, bTempPoints, _bBlock[Y][X], ui32Match, bOmitLast ) );
						}
					}
					fThisError *= fNormalizer;
					if ( fThisError < fError ) {
						// This combination is better.
						ui32BestLow = i32L;
						ui32BestHigh = i32H;
						fError = fThisError;
					}
				}
			}

			_bMin.fValues[I] = CStd::Min( ui32BestLow / fErrorRes[I], ui32BestHigh / fErrorRes[I] );
			_bMax.fValues[I] = CStd::Max( ui32BestLow / fErrorRes[I], ui32BestHigh / fErrorRes[I] );
		}
#endif	// #if 0


		// Last step to refining is to adjust each channel by 1 and test if the result is a closer match.
		const LSINT32 i32Gran[4] = {
			static_cast<LSINT32>(_doOptions.ui32ThirdStageIterations[0]),			// Red.
			static_cast<LSINT32>(_doOptions.ui32ThirdStageIterations[1]),			// Green.
			static_cast<LSINT32>(_doOptions.ui32ThirdStageIterations[2]),			// Blue.
			0,			// Alpha.
		};
		LSUINT16 ui16IntForm[2] = {
			ConvertBlockTexelTo16Bit( _bMin ),
			ConvertBlockTexelTo16Bit( _bMax )
		};
		
		
		LSFLOAT fError = GetError( _bUniqueColors,
			Convert16BitToBlockTexel( ui16IntForm[1] ),
			Convert16BitToBlockTexel( ui16IntForm[0] ),
			_ui32Width, _ui32Height, bOmitLast, 100.0f,
			_doOptions );
		/*LSFLOAT fError = GetError( &sUnique.GetByIndex( 0 ),
			sUnique.Length(),
			Convert16BitToBlockTexel( ui16IntForm[1] ),
			Convert16BitToBlockTexel( ui16IntForm[0] ),
			bOmitLast );*/
		if ( fError != 0.0f ) {
#if 1
			LSUINT32 ui32EarlyOut = LSE_MAXU32;
			/*static const LSFLOAT fSteps[] = {
				1.0f / 31.0f,
				1.0f / 63.0f,
				1.0f / 31.0f,
			};*/
			RoundOffBlock( _bMin );
			RoundOffBlock( _bMax );
			/*LSI_BLOCK bOrigMin = _bMin;
			LSI_BLOCK bOrigMax = _bMax;*/
			LSUINT16 ui16OrigMin[3], ui16OrigMax[3];
			LSUINT16 ui16Temp = ConvertBlockTexelTo16Bit( _bMin );
			ui16OrigMin[0] = ui16Temp >> 11;
			ui16OrigMin[1] = (ui16Temp >> 5) & 0x3F;
			ui16OrigMin[2] = ui16Temp & 0x1F;
			ui16Temp = ConvertBlockTexelTo16Bit( _bMax );
			ui16OrigMax[0] = ui16Temp >> 11;
			ui16OrigMax[1] = (ui16Temp >> 5) & 0x3F;
			ui16OrigMax[2] = ui16Temp & 0x1F;
			LSUINT16 ui16Min[3], ui16Max[3];

			LSI_BLOCK bThisMin, bThisMax;
			bThisMin.s.fA = bThisMax.s.fA = 1.0f;

			// For all reds.
			// =============
			for ( LSINT32 Ry = -i32Gran[0]; Ry <= i32Gran[0] && fError; ++Ry ) {
				ui16Min[0] = static_cast<LSUINT16>(ui16OrigMin[0] + Ry);
				if ( ui16Min[0] > 31 ) { continue; }
				for ( LSINT32 Rx = Ry; Rx <= i32Gran[0] && fError; ++Rx ) {
					ui16Max[0] = static_cast<LSUINT16>(ui16OrigMax[0] + Rx);
					if ( ui16Max[0] > 31 ) { continue; }
					// =============

					// For all greens.
					// =============
					for ( LSINT32 Gy = -i32Gran[1]; Gy <= i32Gran[1] && fError; ++Gy ) {
						ui16Min[1] = static_cast<LSUINT16>(ui16OrigMin[1] + Gy);
						if ( ui16Min[1] > 63 ) { continue; }
						for ( LSINT32 Gx = Gy; Gx <= i32Gran[1] && fError; ++Gx ) {
							ui16Max[1] = static_cast<LSUINT16>(ui16OrigMax[1] + Gx);
							if ( ui16Max[1] > 63 ) { continue; }
							// =============

							// For all blues.
							// =============
							for ( LSINT32 By = -i32Gran[2]; By <= i32Gran[2] && fError; ++By ) {
								ui16Min[2] = static_cast<LSUINT16>(ui16OrigMin[2] + By);
								if ( ui16Min[2] > 31 ) { continue; }
								for ( LSINT32 Bx = By; Bx <= i32Gran[2] && fError; ++Bx ) {
									ui16Max[2] = static_cast<LSUINT16>(ui16OrigMax[2] + Bx);
									if ( ui16Max[2] > 31 ) { continue; }
									// =============

									if ( Ry == 0 && Rx == 0 &&
										Gy == 0 && Gx == 0 &&
										By == 0 && Bx == 0 ) { continue; }

									bThisMax.fValues[0] = ui16Max[0] * 1.0f / 31.0f;
									bThisMax.fValues[1] = ui16Max[1] * 1.0f / 63.0f;
									bThisMax.fValues[2] = ui16Max[2] * 1.0f / 31.0f;

									bThisMin.fValues[0] = ui16Min[0] * 1.0f / 31.0f;
									bThisMin.fValues[1] = ui16Min[1] * 1.0f / 63.0f;
									bThisMin.fValues[2] = ui16Min[2] * 1.0f / 31.0f;

									// Get the amount of error in this new configuration.
									LSFLOAT fThisError = GetError( _bUniqueColors,
										bThisMax,
										bThisMin,
										_ui32Width, _ui32Height, bOmitLast, fError,
										_doOptions );

									// If the error has decreased, keep the changes.
									if ( fThisError < fError ) {
										fError = fThisError;
										_bMin = bThisMin;
										_bMax = bThisMax;
										ui32EarlyOut = 3200;
										continue;
									}

									if ( --ui32EarlyOut == 0 ) {
										fError = 0.0f;
									}

								}
							}
						}
					}
				}
			}

#else
			LSUINT16 ui16WinnerMin = ui16IntForm[0];
			LSUINT16 ui16WinnerMax = ui16IntForm[1];

			// Y is applied to the min, X to the max.
			// For all reds.
			for ( LSINT32 Ry = -i32Gran[0]; Ry <= i32Gran[0] && fError; ++Ry ) {
				// Create the new 16-bit value based off these adjustments.
				LSUINT16 ui16TempMin0 = ui16IntForm[0];
				if ( !Offset16BitColorChannel( 0, Ry, ui16TempMin0 ) ) { continue; }
				for ( LSINT32 Rx = Ry; Rx <= i32Gran[0] && fError; ++Rx ) {
					// Create the new 16-bit value based off these adjustments.
					LSUINT16 ui16TempMax0 = ui16IntForm[1];
					if ( !Offset16BitColorChannel( 0, Rx, ui16TempMax0 ) ) { continue; }

					// For all greens.
					for ( LSINT32 By = -i32Gran[1]; By <= i32Gran[1] && fError; ++By ) {
						// Create the new 16-bit value based off these adjustments.
						LSUINT16 ui16TempMin1 = ui16TempMin0;
						if ( !Offset16BitColorChannel( 1, By, ui16TempMin1 ) ) { continue; }
						for ( LSINT32 Bx = By; Bx <= i32Gran[1] && fError; ++Bx ) {
							// Create the new 16-bit value based off these adjustments.
							LSUINT16 ui16TempMax1 = ui16TempMax0;
							if ( !Offset16BitColorChannel( 1, Bx, ui16TempMax1 ) ) { continue; }

							// For all blues.
							for ( LSINT32 Gy = -i32Gran[2]; Gy <= i32Gran[2] && fError; ++Gy ) {
								// Create the new 16-bit value based off these adjustments.
								LSUINT16 ui16TempMin2 = ui16TempMin1;
								if ( !Offset16BitColorChannel( 2, Gy, ui16TempMin2 ) ) { continue; }
								for ( LSINT32 Gx = Gy; Gx <= i32Gran[2] && fError; ++Gx ) {
									if ( Ry == 0 && Rx == 0 &&
										Gy == 0 && Gx == 0 &&
										By == 0 && Bx == 0 ) { continue; }
									// Create the new 16-bit value based off these adjustments.
									LSUINT16 ui16TempMax2 = ui16TempMax1;
									if ( !Offset16BitColorChannel( 2, Gx, ui16TempMax2 ) ) { continue; }

									/*if ( ui16TempMin2 == 0x4A06 ) {
										if ( ui16TempMax2 == 0xFEF5 ) {
											int gjhg = 0;
										}
									}*/

									// Get the amount of error in this new configuration.
									LSFLOAT fThisError = GetError( _bUniqueColors,
										Convert16BitToBlockTexel( ui16TempMax2 ),
										Convert16BitToBlockTexel( ui16TempMin2 ),
										_ui32Width, _ui32Height, bOmitLast, fError,
										_doOptions );

									/*LSFLOAT fThisError = GetError( &sUnique.GetByIndex( 0 ),
										sUnique.Length(),
										Convert16BitToBlockTexel( ui16TempMax2 ),
										Convert16BitToBlockTexel( ui16TempMin2 ),
										bOmitLast );*/

									// If the error has decreased, keep the changes.
									if ( fThisError < fError ) {
										fError = fThisError;
										ui16WinnerMin = ui16TempMin2;
										ui16WinnerMax = ui16TempMax2;
									}
								}
							}
						}
					}
				}
			}
			ui16IntForm[0] = ui16WinnerMin;
			ui16IntForm[1] = ui16WinnerMax;
#endif	// #if 0
		}



		/*_bMax = Convert16BitToBlockTexel( ui16IntForm[1] );
		_bMin = Convert16BitToBlockTexel( ui16IntForm[0] );*/
		Finalize16BitColors( _bBlock, _bMax, _bMin, _ui32Width, _ui32Height,
			bOmitLast,
			_doOptions );
	}

	/**
	 * Gets the best color pair for a single-color block.
	 *
	 * \param _bUniqueColors The unique colors in the block, along with the number of times the value appears.
	 * \param _bBlock The input block of 4-by-4 texels.
	 * \param _bMax Upon return, this holds the computed maximum color value.
	 * \param _bMin Upon return, this holds the computed minimum color value.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _bOmitLast If true, 3 colors are returned instead of 4.
	 * \param _doOptions Options for the conversion.
	 * \return Returns true if all channels have only one unique color.
	 */
	LSBOOL LSE_CALL CImage::GetBestSingleColor( const CStaticMap<LSI_BLOCK, LSUINT32, 16> &_bUniqueColors,
		const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
		LSBOOL _bOmitLast, const LSI_DXT_OPTIONS &_doOptions ) {
		/*static const LSFLOAT fErrorRes[4] = {
			31.0f,
			63.0f,
			31.0f,
			1.0f,
		};*/
		if ( _bUniqueColors.Length() != 1 ) { return false; }

		// Have we found this color before?
		LSUINT32 ui32MinMax = ConvertBlockTexelTo16Bit( _bUniqueColors.GetKeyByIndex( 0 ) );
		LSUINT32 ui32Index;
		{
			CCriticalSection::CLocker lLock( m_csDxtCrit );
			if ( m_mSingleColorMatches.GetItemIndex( ui32MinMax, ui32Index ) ) {
				LSUINT32 ui32Match = m_mSingleColorMatches.GetByIndex( ui32Index );
				LSUINT16 ui16Min = ui32Match & 0xFFFF;
				LSUINT16 ui16Max = ui32Match >> 16;
				_bMin = Convert16BitToBlockTexel( ui16Min );
				_bMax = Convert16BitToBlockTexel( ui16Max );
				return true;
			}

			if ( m_vDxtTableRb4.Length() == 0 ) {
				// Build the tables.
				m_vDxtTableRb4.Allocate( 32 * 32 - 32 );


				for ( LSUINT32 L = 0; L <= 31; ++L ) {
					LSDOUBLE dLow = L / 31.0;
                    for ( LSUINT32 H = 0; H <= 31; ++H ) {
                        if ( H == L ) { continue; }
                        LSDOUBLE dHigh = H / 31.0;
						LSI_DXT_TABLE_4 dtTable = {
							static_cast<LSFLOAT>(dLow),
                            static_cast<LSFLOAT>((dLow + dHigh * 2.0) / 3.0),
                            static_cast<LSFLOAT>((dLow * 2.0 + dHigh) / 3.0),
                            static_cast<LSFLOAT>(dHigh)
						};
						m_vDxtTableRb4.Push( dtTable );
					}
                }
			}
			if ( m_vDxtTableG4.Length() == 0 ) {
				// Build the tables.
				m_vDxtTableG4.Allocate( 64 * 64 - 64 );


				for ( LSUINT32 L = 0; L <= 63; ++L ) {
					LSDOUBLE dLow = L / 63.0;
                    for ( LSUINT32 H = 0; H <= 63; ++H ) {
                        if ( H == L ) { continue; }
                        LSDOUBLE dHigh = H / 63.0;
						LSI_DXT_TABLE_4 dtTable = {
							static_cast<LSFLOAT>(dLow),
                            static_cast<LSFLOAT>((dLow + dHigh * 2.0) / 3.0),
                            static_cast<LSFLOAT>((dLow * 2.0 + dHigh) / 3.0),
                            static_cast<LSFLOAT>(dHigh)
						};
						m_vDxtTableG4.Push( dtTable );
					}
                }
			}
		}

		// This only works correctly if all channels have only 1 unique color.
		/*for ( LSUINT32 H = 3; H--; ) {
			//if ( H == _ui32Channel ) { continue; }
			// Get the high and low for this channel.
			LSFLOAT fChannelLow = 2.0f;
			LSFLOAT fChannelHi = -2.0f;
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				for ( LSUINT32 X = _ui32Width; X--; ) {
					LSFLOAT fValue = ::floorf( _bBlock[Y][X].fValues[H] * fErrorRes[H] + 0.5f ) / fErrorRes[H];
					fChannelLow = CStd::Min( fChannelLow, fValue );
					fChannelHi = CStd::Max( fChannelHi, fValue );
				}
			}
			if ( fChannelLow != fChannelHi ) {
				// More than one color.
				return false;
			}
		}*/

#if 1
		// Use the tables.
		LSFLOAT fError = 1000.0f;

		LSI_BLOCK bColor = _bUniqueColors.GetKeyByIndex( 0 );
		/*bColor.s.fR = CMathLib::Pow( bColor.s.fR, 2.2f );
		bColor.s.fG = CMathLib::Pow( bColor.s.fG, 2.2f );
		bColor.s.fB = CMathLib::Pow( bColor.s.fB, 2.2f );*/
		LSUINT32 ui32RIndex = 0, ui32GIndex = 0, ui32BIndex = 0;

		// For each index.
		for ( LSUINT32 I = 0; I < 4 && fError; ++I ) {
			// Find the best matches for each color in this index.

			LSFLOAT fRed = 100.0f, fGreen = 100.0f, fBlue = 100.0f;
//#define LSI_GETCOLOR( TABLE ) LSFLOAT fThisColor = ::floorf( TABLE[C].fColors[I] * 255.0f + 0.5f ) / 255.0f
//#define LSI_GETCOLOR( TABLE ) LSFLOAT fThisColor = ::floorf( TABLE[C].fColors[I] * 255.0f ) / 255.0f
#define LSI_GETCOLOR( TABLE )   LSFLOAT fThisColor = TABLE[C].fColors[I]
			for ( LSUINT32 C = 0; C < m_vDxtTableRb4.Length(); ++C ) {
				LSI_GETCOLOR( m_vDxtTableRb4 );
				
				// Red.
				LSFLOAT fThisRed = fThisColor - bColor.s.fR;
				fThisRed *= fThisRed;
				if ( fThisRed < fRed ) {
					fRed = fThisRed;
					ui32RIndex = C;
				}

				// Blue.
				LSFLOAT fThisBlue = fThisColor - bColor.s.fB;
				fThisBlue *= fThisBlue;
				if ( fThisBlue < fBlue ) {
					fBlue = fThisBlue;
					ui32BIndex = C;
				}
			}

			for ( LSUINT32 C = 0; C < m_vDxtTableG4.Length(); ++C ) {
				LSI_GETCOLOR( m_vDxtTableG4 );
				// Green.
				LSFLOAT fThisGreen = fThisColor - bColor.s.fG;
				fThisGreen *= fThisGreen;
				if ( fThisGreen < fGreen ) {
					fGreen = fThisGreen;
					ui32GIndex = C;
				}
			}
#undef LSI_GETCOLOR

			LSFLOAT fThisError = fRed * _doOptions.fRedWeight +
				fBlue * _doOptions.fBlueWeight +
				fGreen * _doOptions.fGreenWeight;
			if ( fThisError < fError ) {
				fError = fThisError;
				_bMin.s.fR = m_vDxtTableRb4[ui32RIndex].fColors[0];
				_bMax.s.fR = m_vDxtTableRb4[ui32RIndex].fColors[3];

				_bMin.s.fG = m_vDxtTableG4[ui32GIndex].fColors[0];
				_bMax.s.fG = m_vDxtTableG4[ui32GIndex].fColors[3];

				_bMin.s.fB = m_vDxtTableRb4[ui32BIndex].fColors[0];
				_bMax.s.fB = m_vDxtTableRb4[ui32BIndex].fColors[3];
			}
		}
#else
		LSFLOAT fError = 100.0f;
		// We only check the first and second bucket indices.
		//	The first bucket entry is done by just taking the actual colors for each channel and
		//	applying them to both the high and low.
		LSUINT32 ui32Colors = _bOmitLast ? 3 : 4;
		LSI_BLOCK bBestLow = { 0.0f };
		LSI_BLOCK bBestHigh = { 0.0f };
		for ( LSFLOAT fOffset = -1.0f; fOffset <= 1.0; ++fOffset ) {
			LSI_BLOCK bLow;
			for ( LSUINT32 I = 3; I--; ) {
				LSFLOAT fValue = ::floorf( _bBlock[0][0].fValues[I] * fErrorRes[I] + 0.5f ) / fErrorRes[I];
				bLow.fValues[I] = fValue;
			}
			LSFLOAT fThisError = GetError( _bUniqueColors,
				bLow,
				bLow,
				_ui32Width, _ui32Height, _bOmitLast, fError,
				_doOptions );
			if ( fThisError < fError ) {
				bBestLow = bBestHigh = bLow;
				fError = fThisError;
			}
		}

		_bMax = bBestHigh;
		_bMin = bBestLow;

		if ( fError != 0.0f ) {
			static LSUINT32 ui32Ranges[3] = {
				32,
				64,
				32
			};
			// Now try the second indices.
			// For each red.
			for ( LSUINT32 iR = 0; iR < ui32Ranges[0]; ++iR ) {
				LSFLOAT fRedLow, fRedHigh;
				LSFLOAT fRedRes = 1.0f / fErrorRes[0];
				FindEndPoints( iR * fRedRes, _bBlock[0][0].fValues[0], 0, 1,
					ui32Colors, fRedRes, fRedLow, fRedHigh );
				// Try offsets up and down by 1 on the far end.
				for ( LSFLOAT fRedOffset = -1.0f; fRedOffset <= 1.0; ++fRedOffset ) {
					LSFLOAT fThisRedHigh = fRedHigh + (fRedOffset * fRedRes);
					if ( fThisRedHigh >= 0.0f && fThisRedHigh <= 1.0f && fThisRedHigh >= fRedLow ) {
						// For each green.
						for ( LSUINT32 iG = 0; iG < ui32Ranges[1]; ++iG ) {
							LSFLOAT fGreenLow, fGreenHigh;
							LSFLOAT fGreenRes = 1.0f / fErrorRes[1];
							FindEndPoints( iG * fGreenRes, _bBlock[0][0].fValues[1], 0, 1,
								ui32Colors, fGreenRes, fGreenLow, fGreenHigh );
							// Try offsets up and down by 1 on the far end.
							for ( LSFLOAT fGreenOffset = -1.0f; fGreenOffset <= 1.0; ++fGreenOffset ) {
								LSFLOAT fThisGreenHigh = fGreenHigh + (fGreenOffset * fGreenRes);
								if ( fThisGreenHigh >= 0.0f && fThisGreenHigh <= 1.0f && fThisGreenHigh >= fGreenLow ) {
									// For each blue.
									for ( LSUINT32 iB = 0; iB < ui32Ranges[2]; ++iB ) {
										LSFLOAT fBlueLow, fBlueHigh;
										LSFLOAT fBlueRes = 1.0f / fErrorRes[2];
										FindEndPoints( iB * fBlueRes, _bBlock[0][0].fValues[2], 0, 1,
											ui32Colors, fBlueRes, fBlueLow, fBlueHigh );
										// Try offsets up and down by 1 on the far end.
										for ( LSFLOAT fBlueOffset = -1.0f; fBlueOffset <= 1.0; ++fBlueOffset ) {
											LSFLOAT fThisBlueHigh = fBlueHigh + (fBlueOffset * fBlueRes);
											if ( fThisBlueHigh >= 0.0f && fThisBlueHigh <= 1.0f && fThisBlueHigh >= fBlueLow ) {
												// Get the score.
												LSI_BLOCK bThisColorLow;
												bThisColorLow.s.fR = fRedLow;
												bThisColorLow.s.fG = fGreenLow;
												bThisColorLow.s.fB = fBlueLow;

												LSI_BLOCK bThisColorHigh;
												bThisColorHigh.s.fR = fThisRedHigh;
												bThisColorHigh.s.fG = fThisGreenHigh;
												bThisColorHigh.s.fB = fThisBlueHigh;

												LSFLOAT fThisError = GetError( _bUniqueColors,
													bThisColorHigh,
													bThisColorLow,
													_ui32Width, _ui32Height, _bOmitLast, fError, _doOptions );
												if ( fThisError < fError ) {
													_bMax = bThisColorHigh;
													_bMin = bThisColorLow;
													fError = fThisError;
												}
											}
										}
									}
								}
							}
						}




					}
				}
			}
		}
#endif	// #if 0
		

		Finalize16BitColors( _bBlock, _bMax, _bMin, _ui32Width, _ui32Height,
			_bOmitLast,
			_doOptions );

		{
			LSUINT16 ui16NewMin = ConvertBlockTexelTo16Bit( _bMin );
			LSUINT16 ui16NewMax = ConvertBlockTexelTo16Bit( _bMax );
			LSUINT32 ui32NewMinMax = ui16NewMin | (ui16NewMax << 16);
			CCriticalSection::CLocker lLock( m_csDxtCrit );
			m_mSingleColorMatches[ui32MinMax] = ui32NewMinMax;
		}

		return true;
	}

	/**
	 * Fixes a 16-bit color pair by swapping some components to create a better match for the given input block.
	 *
	 * \param _bBlock The input block of 4-by-4 texels.
	 * \param _bMax Upon return, this holds the computed maximum color value.
	 * \param _bMin Upon return, this holds the computed minimum color value.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _bOmitLast If true, 3 colors are used instead of 4.
	 * \param _doOptions Options for the conversion.
	 */
	LSVOID LSE_CALL CImage::Finalize16BitColors( const LSI_BLOCK _bBlock[4][4], LSI_BLOCK &_bMax, LSI_BLOCK &_bMin, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
		LSBOOL _bOmitLast, const LSI_DXT_OPTIONS &_doOptions ) {
		// Until now we have worked on each channel separately such that the mins and maxes are both on one side or the other for each
		//	channel.
		// In the cases in an image where one channel goes down as another channel goes up, this will leave us with nothing but poor matches
		//	when we try to figure out how much of each of the key colors to use for the respective block texel.
		// Here we try every combination of flipping each channel to find the one that is best.
		static const LSUINT32 ui32Shifts[4] = {
			11,			// Red.
			5,			// Green.
			0,			// Blue.
			0,			// Alpha.
		};
		static const LSUINT32 ui32Bits[4] = {
			5,			// Red.
			6,			// Green.
			5,			// Blue.
			0,			// Alpha.
		};
		LSUINT16 ui16IntForm[2] = {
			ConvertBlockTexelTo16Bit( _bMin ),
			ConvertBlockTexelTo16Bit( _bMax )
		};
		// For each channel.
		for ( LSUINT32 I = 3; I--; ) {
			LSUINT32 ui32Mask = (1 << ui32Bits[I]) - 1;
			LSFLOAT fError = GetErrorStrict( _bBlock,
				Convert16BitToBlockTexel( ui16IntForm[1] ),
				Convert16BitToBlockTexel( ui16IntForm[0] ),
				_ui32Width, _ui32Height, _bOmitLast,
				_doOptions );

			LSUINT32 ui32Min = (ui16IntForm[0] >> ui32Shifts[I]) & ui32Mask;
			LSUINT32 ui32Max = (ui16IntForm[1] >> ui32Shifts[I]) & ui32Mask;

			// See if the error decreases with the channel swapped.
			// Create the new 16-bit value based off these adjustments.
			LSUINT16 ui16TempMin = static_cast<LSUINT16>(ui16IntForm[0] & ~(ui32Mask << ui32Shifts[I]));
			LSUINT16 ui16TempMax = static_cast<LSUINT16>(ui16IntForm[1] & ~(ui32Mask << ui32Shifts[I]));

			ui16TempMin |= ui32Max << ui32Shifts[I];
			ui16TempMax |= ui32Min << ui32Shifts[I];

			if ( GetErrorStrict( _bBlock,
				Convert16BitToBlockTexel( ui16TempMin ),
				Convert16BitToBlockTexel( ui16TempMax ),
				_ui32Width, _ui32Height, _bOmitLast,
				_doOptions ) < fError ) {
				// Swapped is better.
				ui16IntForm[0] = ui16TempMin;
				ui16IntForm[1] = ui16TempMax;
			}
		}


		_bMin = Convert16BitToBlockTexel( CStd::Min( ui16IntForm[0], ui16IntForm[1] ) );
		_bMax = Convert16BitToBlockTexel( CStd::Max( ui16IntForm[0], ui16IntForm[1] ) );
	}

	/**
	 * Makes the 4 colors that can be created with the given block texels.
	 *
	 * \param _bMax The max color.
	 * \param _bMin The min color.
	 * \param _bRet The returned array of 4 colors.
	 * \param _bOmitLast If true, 3 colors are returned instead of 4.
	 */
	LSVOID LSE_CALL CImage::Get4Colors( const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin, LSI_BLOCK _bRet[4], LSBOOL _bOmitLast ) {
		if ( _bOmitLast ) {
			_bRet[0] = _bMin;
			_bRet[2] = _bMax;
			// Account for rounding errors introduced by converting to 16 bits.  Does not apply to the middle colors, only the ends.
			/*RoundOffBlock( _bRet[0] );
			RoundOffBlock( _bRet[2] );*/

			// Derive the middle colors.  When rendered, they will be interpolated with floating-accuracy, so the above rounding does
			//	not apply.
			for ( LSUINT32 I = 3; I--; ) {
				//_bRet[1].fValues[I] = _bRet[0].fValues[I] * 0.5f + _bRet[2].fValues[I] * 0.5f;
				_bRet[1].fValues[I] = _bMin.fValues[I] * 0.5f + _bMax.fValues[I] * 0.5f;
			}
		}
		else {
			// Account for rounding errors introduced by converting to 16 bits.  Does not apply to the middle colors, only the ends.
			_bRet[0] = _bMin;
			_bRet[3] = _bMax;
			/*RoundOffBlock( _bRet[0] );
			RoundOffBlock( _bRet[3] );*/

			// Derive the middle colors.  When rendered, they will be interpolated with floating-accuracy, so the above rounding does
			//	not apply.
			for ( LSUINT32 I = 3; I--; ) {
				//_bRet[2].fValues[I] = _bRet[0].fValues[I] * (1.0f / 3.0f) + _bRet[3].fValues[I] * (1.0f - 1.0f / 3.0f);
				_bRet[2].fValues[I] = _bMin.fValues[I] * (1.0f / 3.0f) + _bMax.fValues[I] * (1.0f - 1.0f / 3.0f);
			}
			for ( LSUINT32 I = 3; I--; ) {
				//_bRet[1].fValues[I] = _bRet[0].fValues[I] * (2.0f / 3.0f) + _bRet[3].fValues[I] * (1.0f - 2.0f / 3.0f);
				_bRet[1].fValues[I] = _bMin.fValues[I] * (2.0f / 3.0f) + _bMax.fValues[I] * (1.0f - 2.0f / 3.0f);
			}
		}
	}

	/**
	 * Makes the 4 colors that can be created with the given block texels on one channel only.
	 *
	 * \param _ui32Channel The channel for which to derive the 4 possible values.
	 * \param _bMax The max color.
	 * \param _bMin The min color.
	 * \param _fRet The returned array of 4 colors along the given channel.
	 * \param _bOmitLast If true, 3 colors are returned instead of 4.
	 */
	LSVOID LSE_CALL CImage::Get4Colors( LSUINT32 _ui32Channel, const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin, LSFLOAT _fRet[4], LSBOOL _bOmitLast ) {
		if ( _bOmitLast ) {
			_fRet[0] = _bMin.fValues[_ui32Channel];
			_fRet[2] = _bMax.fValues[_ui32Channel];
			_fRet[1] = _bMin.fValues[_ui32Channel] * 0.5f + _bMax.fValues[_ui32Channel] * 0.5f;
		}
		else {
			_fRet[0] = _bMin.fValues[_ui32Channel];
			_fRet[3] = _bMax.fValues[_ui32Channel];
			_fRet[2] = _bMin.fValues[_ui32Channel] * (1.0f / 3.0f) + _bMax.fValues[_ui32Channel] * (1.0f - 1.0f / 3.0f);
			_fRet[1] = _bMin.fValues[_ui32Channel] * (2.0f / 3.0f) + _bMax.fValues[_ui32Channel] * (1.0f - 2.0f / 3.0f);
		}
	}

	/**
	 * Makes a DXT1 block.
	 *
	 * \param _bBlock Contains the 4-by-4 block of texels to be encoded.
	 * \param _fMax Holds the maximum values for each channel of the color to use.
	 * \param _fMin Holds the minimum values for each channel of the color to use.
	 * \param _fAlphaHigh The highest alpha value in the block.
	 * \param _fAlphaLow The lowest alpha value in the block.
	 * \param _doOptions Options for the conversion.
	 * \param _pui8Return Holds the returned compressed block.
	 */
	LSVOID LSE_CALL CImage::MakeDxt1Block( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
		LSFLOAT _fAlphaHigh, LSFLOAT _fAlphaLow,
		LSUINT8 * _pui8Return, const LSI_DXT_OPTIONS &_doOptions ) {
		// We always use color0 > color1.

		LSUINT16 * pui16ColorTarget = reinterpret_cast<LSUINT16 *>(_pui8Return);
		if ( _fAlphaLow < _doOptions.fAlphaThresh ) {
			LSUINT16 ui16Min = ConvertBlockTexelTo16Bit( _bMin );
			LSUINT16 ui16Max = ConvertBlockTexelTo16Bit( _bMax );
			pui16ColorTarget[0] = CStd::Min( ui16Min, ui16Max );
			pui16ColorTarget[1] = CStd::Max( ui16Min, ui16Max );
		}
		else {
			pui16ColorTarget[0] = ConvertBlockTexelTo16Bit( _bMax );
			pui16ColorTarget[1] = ConvertBlockTexelTo16Bit( _bMin );

			
			if ( pui16ColorTarget[0] <= pui16ColorTarget[1] ) {
				if ( pui16ColorTarget[0] != 0xFFFF ) {
					++pui16ColorTarget[0];
				}
				else {
					--pui16ColorTarget[1];
				}
			}
		}
		
		// Remap the key colors back to floating-point land.
		LSI_BLOCK bColor0 = Convert16BitToBlockTexel( pui16ColorTarget[0] ), bColor1 = Convert16BitToBlockTexel( pui16ColorTarget[1] );

		LSUINT32 * pui32Table = reinterpret_cast<LSUINT32 *>(&_pui8Return[4]);
		(*pui32Table) = 0;
		// Now build the table.
		for ( LSUINT32 Y = 4; Y--; ) {
			for ( LSUINT32 X = 4; X--; ) {
				(*pui32Table) <<= 2;
				(*pui32Table) |= FindDxt1Match( bColor0, bColor1, _bBlock[Y][X], _fAlphaHigh, _fAlphaLow, _doOptions );
			}
		}
	}

	/**
	 * Makes a DXT3 block.
	 *
	 * \param _bBlock Contains the 4-by-4 block of texels to be encoded.
	 * \param _fMax Holds the maximum values for each channel of the color to use.
	 * \param _fMin Holds the minimum values for each channel of the color to use.
	 * \param _doOptions Options for the conversion.
	 * \param _pui8Return Holds the returned compressed block.
	 */
	LSVOID LSE_CALL CImage::MakeDxt3Block( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
		LSUINT8 * _pui8Return, const LSI_DXT_OPTIONS &_doOptions ) {
		LSUINT16 ui16Max = ConvertBlockTexelTo16Bit( _bMax );
		LSUINT16 ui16Min = ConvertBlockTexelTo16Bit( _bMin );

		// Remap the key colors back to floating-point land.
		LSI_BLOCK bColor0 = Convert16BitToBlockTexel( ui16Max ), bColor1 = Convert16BitToBlockTexel( ui16Min );

		// Add the alphas.
		LSUINT16 * pui16Table = reinterpret_cast<LSUINT16 *>(_pui8Return);
		for ( LSUINT32 Y = 0; Y < 4; ++Y ) {
			(*pui16Table) = 0;
			for ( LSUINT32 X = 4; X--; ) {
				(*pui16Table) <<= 4;
				(*pui16Table) |= static_cast<LSUINT16>(CStd::Clamp( _bBlock[Y][X].s.fA * 15.0f + 0.5f, 0.0f, 15.0f ));
			}
			++pui16Table;
		}

		(*pui16Table++) = ui16Max;
		(*pui16Table++) = ui16Min;

		LSUINT32 * pui32Table = reinterpret_cast<LSUINT32 *>(pui16Table);
		(*pui32Table) = 0;

		// Now build the table.
		for ( LSUINT32 Y = 4; Y--; ) {
			for ( LSUINT32 X = 4; X--; ) {
				(*pui32Table) <<= 2;
				(*pui32Table) |= FindDxt1Match( bColor0, bColor1, _bBlock[Y][X], 1.0f, 1.0f, _doOptions );
			}
		}
	}

	/**
	 * Makes a DXT5 block.
	 *
	 * \param _bBlock Contains the 4-by-4 block of texels to be encoded.
	 * \param _fMax Holds the maximum values for each channel of the color to use.
	 * \param _fMin Holds the minimum values for each channel of the color to use.
	 * \param _doOptions Options for the conversion.
	 * \param _pui8Return Holds the returned compressed block.
	 */
	LSVOID LSE_CALL CImage::MakeDxt5Block( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
		LSUINT8 * _pui8Return, const LSI_DXT_OPTIONS &_doOptions ) {
		LSUINT16 ui16Max = ConvertBlockTexelTo16Bit( _bMax );
		LSUINT16 ui16Min = ConvertBlockTexelTo16Bit( _bMin );

		// Remap the key colors back to floating-point land.
		LSI_BLOCK bColor0 = Convert16BitToBlockTexel( ui16Max ), bColor1 = Convert16BitToBlockTexel( ui16Min );


		// Add the alphas.
		LSUINT16 ui16Alpha0, ui16Alpha1;
		FindDxt5AlphaKeyColors( ui16Alpha0, ui16Alpha1, _bBlock, 4, 4 );
		LSUINT64 * pui64Table = reinterpret_cast<LSUINT64 *>(_pui8Return);
		(*pui64Table) = 0ULL;
		if ( ui16Alpha0 > ui16Alpha1 ) {
			for ( LSUINT32 Y = 4; Y--; ) {
				for ( LSUINT32 X = 4; X--; ) {
					(*pui64Table) <<= 3ULL;
					(*pui64Table) |= FindDxt5AlphaMatch0( ui16Alpha0, ui16Alpha1, ::floorf( _bBlock[Y][X].s.fA * 255.0f + 0.5f ) );
				}
			}
		}
		else {
			for ( LSUINT32 Y = 4; Y--; ) {
				for ( LSUINT32 X = 4; X--; ) {
					(*pui64Table) <<= 3ULL;
					(*pui64Table) |= FindDxt5AlphaMatch1( ui16Alpha0, ui16Alpha1, ::floorf( _bBlock[Y][X].s.fA * 255.0f + 0.5f ) );
				}
			}
		}
		(*pui64Table) <<= 16ULL;
		// The alpha key colors.
		_pui8Return[0] = static_cast<LSUINT8>(ui16Alpha0);
		_pui8Return[1] = static_cast<LSUINT8>(ui16Alpha1);

		LSUINT16 * pui16Table = reinterpret_cast<LSUINT16 *>(_pui8Return + 8);
		(*pui16Table++) = ui16Max;
		(*pui16Table++) = ui16Min;

		LSUINT32 * pui32Table = reinterpret_cast<LSUINT32 *>(pui16Table);
		(*pui32Table) = 0;

		// Now build the table.
		for ( LSUINT32 Y = 4; Y--; ) {
			for ( LSUINT32 X = 4; X--; ) {
				(*pui32Table) <<= 2;
				(*pui32Table) |= FindDxt1Match( bColor0, bColor1, _bBlock[Y][X], 1.0f, 1.0f, _doOptions );
			}
		}
	}

	/**
	 * Finds the 2-bit DXT code that represents the given color's relationship between the two given key colors.
	 *	0	color0
	 *	1	color1
	 *	2	(2*color0 + color1) / 3
	 *	3	(color0 + 2*color1) / 3
	 *
	 * \param _bColor0 The first key color.
	 * \param _bColor1 The second key color.
	 * \param _bColor The color to test for being closer to one or the other key color.
	 * \param _fAlphaHigh The highest alpha value in the block.
	 * \param _fAlphaLow The lowest alpha value in the block.
	 * \param _doOptions Weights to use for the error calculation.
	 * \return Returns a 2-bit DXT code indicating the color's relationship to the given key colors.
	 */
	LSUINT32 LSE_CALL CImage::FindDxt1Match( const LSI_BLOCK &_bColor0, const LSI_BLOCK &_bColor1,
		const LSI_BLOCK &_bColor, LSFLOAT /*_fAlphaHigh*/, LSFLOAT _fAlphaLow, const LSI_DXT_OPTIONS &_doOptions ) {
		if ( _bColor.s.fA < _doOptions.fAlphaThresh ) {
			return 3;
		}
		static const LSFLOAT fAlphaTable4[] = {
			1.0f,
			0.0f,
			2.0f / 3.0f,
			1.0f / 3.0f
		};
		static const LSFLOAT fAlphaTable3[] = {
			1.0f,
			0.0f,
			1.0f / 2.0f,
		};
		LSUINT32 ui32Colors;
		const LSFLOAT * pfTable;
		if ( _fAlphaLow < _doOptions.fAlphaThresh ) {
			ui32Colors = 3;
			pfTable = fAlphaTable3;
		}
		else {
			ui32Colors = 4;
			pfTable = fAlphaTable4;
		}
		

		LSUINT32 ui32Winner = 0;
		LSFLOAT fMinDeviation = 2.0f;
		for ( LSUINT32 I = 0; I < ui32Colors; ++I ) {
			LSFLOAT fAlpha = pfTable[I];
			// Create the sample color for this level of interpolation.
			LSI_BLOCK bSample;
			for ( LSUINT32 J = 4; J--; ) {
				bSample.fValues[J] = _bColor0.fValues[J] * fAlpha + _bColor1.fValues[J] * (1.0f - fAlpha);
			}

			// Get the amount by which the color is off.
			LSFLOAT fError = GetError( bSample, _bColor, _doOptions );

			if ( fError == 0.0f ) { return I; }
			if ( fError < fMinDeviation ) {
				fMinDeviation = fError;
				ui32Winner = I;
			}
		}
		return ui32Winner;
	}

	/**
	 * Finds the 3-bit DXT code that represents the given alpha value for an alpha0 > alpha1 block in DXT5.
	 *
	 * \param _fAlpha0 The first key color.
	 * \param _fAlpha1 The second key color.
	 * \param _fAlpha The alpha value from 0 to 255.
	 * \return Returns the 3-bit code for the given alpha in relationship to the key alphas using alpha0 > alpha1
	 *	rules.
	 */
	LSUINT32 LSE_CALL CImage::FindDxt5AlphaMatch0( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha ) {
		LSFLOAT fError = ::fabsf( _fAlpha0 - _fAlpha );
		if ( !fError ) { return 0; }
		LSFLOAT fThisError = ::fabsf( _fAlpha1 - _fAlpha );
		if ( !fThisError ) { return 1; }
		LSUINT32 ui32Ret = 0;
		if ( fThisError < fError ) {
			ui32Ret = 1;
			fError = fThisError;
		}

		
		for ( LSUINT32 I = 1; I <= 6 && fError; ++I ) {
			// (6 * alpha_0 + 1 * alpha_1 + 3) / 7
			LSFLOAT fI = static_cast<LSFLOAT>(I);
			LSFLOAT fVal = ((7.0f - fI) * _fAlpha0 + fI * _fAlpha1 + 3.0f) * (1.0f / 7.0f);
			fThisError = ::fabsf( fVal - _fAlpha );
			if ( fThisError < fError ) {
				ui32Ret = I + 1;
				fError = fThisError;
			}
		}
		return ui32Ret;
	}

	/**
	 * Finds the 3-bit DXT code that represents the given alpha value for an alpha0 <= alpha1 block in DXT5.
	 *
	 * \param _fAlpha0 The first key color.
	 * \param _fAlpha1 The second key color.
	 * \param _fAlpha The alpha value from 0 to 255.
	 * \return Returns the 3-bit code for the given alpha in relationship to the key alphas using alpha0 <= alpha1
	 *	rules.
	 */
	LSUINT32 LSE_CALL CImage::FindDxt5AlphaMatch1( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha ) {
		// Left alpha.
		LSFLOAT fError = ::fabsf( _fAlpha0 - _fAlpha );
		if ( !fError ) { return 0; }

		// Right alpha.
		LSFLOAT fThisError = ::fabsf( _fAlpha1 - _fAlpha );
		if ( !fThisError ) { return 1; }
		LSUINT32 ui32Ret = 0;
		if ( fThisError < fError ) {
			ui32Ret = 1;
			fError = fThisError;
		}

		// 0x00.
		fThisError = _fAlpha;
		if ( !fThisError ) { return 6; }
		if ( fThisError < fError ) {
			ui32Ret = 6;
			fError = fThisError;
		}

		// 0xFF.
		fThisError = ::fabsf( 255.0f - _fAlpha );
		if ( !fThisError ) { return 7UL; }
		if ( fThisError < fError ) {
			ui32Ret = 7UL;
			fError = fThisError;
		}

		for ( LSUINT32 I = 1; I <= 4 && fError; ++I ) {
			// (4 * alpha_0 + 1 * alpha_1 + 2) / 5
			LSFLOAT fI = static_cast<LSFLOAT>(I);
			LSFLOAT fVal = ((5.0f - fI) * _fAlpha0 + fI * _fAlpha1 + 2.0f) * (1.0f / 5.0f);
			fThisError = ::fabsf( fVal - _fAlpha );
			if ( fThisError < fError ) {
				ui32Ret = I + 1;
				fError = fThisError;
			}
		}
		return ui32Ret;
	}

	/**
	 * Finds the key colors for DXT5 alpha.
	 *
	 * \param _ui16Color0 The returned left key color.
	 * \param _ui16Color1 The returned right key color.
	 * \param _bBlock Contains the 4-by-4 block of texels to be used to determine the key colors.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 */
	LSVOID LSE_CALL CImage::FindDxt5AlphaKeyColors( LSUINT16 &_ui16Color0, LSUINT16 &_ui16Color1,
		const LSI_BLOCK _bBlock[4][4], LSUINT32 _ui32Width, LSUINT32 _ui32Height ) {
		// If the alpha is entirely 0x00 or 0xFF, the answer is simple.
		LSFLOAT fMinAlpha = 1.0f, fMaxAlpha = 0.0f;
		for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				fMinAlpha = CStd::Min( fMinAlpha, _bBlock[Y][X].s.fA );
				fMaxAlpha = CStd::Max( fMaxAlpha, _bBlock[Y][X].s.fA );
			}
		}
		if ( fMinAlpha >= 255.5f / 256.0f || fMaxAlpha < 0.5f / 256.0f ) {
			// Just use the alpha0 <= alpha1 scheme.
			_ui16Color0 = 0;
			_ui16Color1 = 0;
		}
		else if ( fMinAlpha == fMaxAlpha ) {
			// Only one alpha value.  Easy to find the best answer.
			LSFLOAT fAlpha = ::floorf( fMinAlpha * 255.0f + 0.5f );
			LSUINT16 ui16Best0 = static_cast<LSUINT8>(fAlpha), ui16Best1 = static_cast<LSUINT8>(fAlpha);
			LSFLOAT fError = GetErrorDxt5Alpha1( ui16Best0, ui16Best1, fAlpha );
			// Try the alpha0 <= alpha1 case.
			for ( LSUINT16 ui16Left = 0; ui16Left < 256 && fError; ++ui16Left ) {
				for ( LSUINT16 ui16Right = ui16Left; ui16Right < 256; ++ui16Right ) {
					LSFLOAT fThisError = GetErrorDxt5Alpha1( ui16Left, ui16Right, fAlpha );
					if ( fThisError < fError ) {
						fError = fThisError;
						ui16Best0 = ui16Left;
						ui16Best1 = ui16Right;
						if ( !fError ) { break; }
					}
				}
			}
			// Try the alpha0 > alpha1 case.
			for ( LSUINT16 ui16Left = 1; ui16Left < 256 && fError; ++ui16Left ) {
				for ( LSUINT16 ui16Right = 0; ui16Right < ui16Left; ++ui16Right ) {
					LSFLOAT fThisError = GetErrorDxt5Alpha0( ui16Left, ui16Right, fAlpha );
					if ( fThisError < fError ) {
						fError = fThisError;
						ui16Best0 = ui16Left;
						ui16Best1 = ui16Right;
						if ( !fError ) { break; }
					}
				}
			}
			_ui16Color0 = ui16Best0;
			_ui16Color1 = ui16Best1;
		}
		else {
			// Have to interpolate between key colors.
			//	Denormalize the alpha values.
			LSI_BLOCK bDenormalBlock[4][4];
			for ( LSUINT32 Y = _ui32Height; Y--; ) {
				for ( LSUINT32 X = _ui32Width; X--; ) {
					bDenormalBlock[Y][X].s.fA = ::floorf( _bBlock[Y][X].s.fA * 255.0f + 0.5f );
				}
			}

			LSUINT16 ui16Best0 = 1, ui16Best1 = 0;
			LSFLOAT fError = GetErrorDxt5Alpha0( ui16Best0, ui16Best1, bDenormalBlock, _ui32Width, _ui32Height );
			// Try the alpha0 > alpha1 case.
			for ( LSUINT16 ui16Left = ui16Best0; ui16Left < 256 && fError; ++ui16Left ) {
				for ( LSUINT16 ui16Right = 0; ui16Right < ui16Left; ++ui16Right ) {
					LSFLOAT fThisError = GetErrorDxt5Alpha0( ui16Left, ui16Right, bDenormalBlock, _ui32Width, _ui32Height );
					if ( fThisError < fError ) {
						fError = fThisError;
						ui16Best0 = ui16Left;
						ui16Best1 = ui16Right;
						if ( !fError ) { break; }
					}
				}
			}
			// Try the alpha0 <= alpha1 case.
			for ( LSUINT16 ui16Left = 0; ui16Left < 256 && fError; ++ui16Left ) {
				for ( LSUINT16 ui16Right = ui16Left; ui16Right < 256; ++ui16Right ) {
					LSFLOAT fThisError = GetErrorDxt5Alpha1( ui16Left, ui16Right, bDenormalBlock, _ui32Width, _ui32Height );
					if ( fThisError < fError ) {
						fError = fThisError;
						ui16Best0 = ui16Left;
						ui16Best1 = ui16Right;
						GetErrorDxt5Alpha1( ui16Left, ui16Right, bDenormalBlock, _ui32Width, _ui32Height );
						if ( !fError ) { break; }
					}
				}
			}
			_ui16Color0 = ui16Best0;
			_ui16Color1 = ui16Best1;
		}
	}

	/**
	 * Gets the amount of error for a DXT5 alpha block using the alpha0 > alpha1 rules.
	 *
	 * \param _fAlpha0 alpha0.
	 * \param _fAlpha1 alpha1.
	 * \param _fAlpha The value whose error is to be calculated.
	 * \return Returns the minimum amount of error between the alpha points for the given alpha value.
	 */
	LSFLOAT LSE_CALL CImage::GetErrorDxt5Alpha0( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha ) {
		LSFLOAT fError = CStd::Min( ::fabsf( _fAlpha0 - _fAlpha ), ::fabsf( _fAlpha1 - _fAlpha ) );	// Minimum error between the key colors.
		if ( fError == 0.0f ) {
			// One of the key colors is an exact match.
			return fError;
		}
		// An interpolated value might be a better match.
		if ( _fAlpha < _fAlpha1 && _fAlpha > _fAlpha0 ) {
			for ( LSUINT32 I = 1; I <= 6; ++I ) {
				// (6 * alpha_0 + 1 * alpha_1 + 3) / 7
				LSFLOAT fI = static_cast<LSFLOAT>(I);
				LSFLOAT fVal = ((7.0f - fI) * _fAlpha0 + fI * _fAlpha1 + 3.0f) * (1.0f / 7.0f);
				fError = CStd::Min( fError, ::fabsf( fVal - _fAlpha ) );
			}
		}
		fError *= (1.0f / 255.0f);
		return fError * fError;
	}

	/**
	 * Gets the amount of non-normalized error for a DXT5 alpha block using the alpha0 > alpha1 rules.
	 *	Of note is that this is a special-case function which assumes the alpha values in the block have
	 *	already been set to the 0-255 range.
	 *
	 * \param _fAlpha0 alpha0.
	 * \param _fAlpha1 alpha1.
	 * \param _bBlock Contains the 4-by-4 block of texels to be used to determine the key colors.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \return Returns the minimum amount of error between the alpha points for the given block.
	 */
	LSFLOAT LSE_CALL CImage::GetErrorDxt5Alpha0( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1,
		const LSI_BLOCK _bBlock[4][4], LSUINT32 _ui32Width, LSUINT32 _ui32Height ) {
		LSFLOAT fError = 0.0f;
		for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				fError += GetErrorDxt5Alpha0( _fAlpha0, _fAlpha1, _bBlock[Y][X].s.fA );
			}
		}
		return fError;
	}

	/**
	 * Gets the amount of error for a DXT5 alpha block using the alpha0 <= alpha1 rules.
	 *
	 * \param _fAlpha0 alpha0.
	 * \param _fAlpha1 alpha1.
	 * \param _fAlpha The value whose error is to be calculated.
	 * \return Returns the minimum amount of error between the alpha points for the given alpha value.
	 */
	LSFLOAT LSE_CALL CImage::GetErrorDxt5Alpha1( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1, LSFLOAT _fAlpha ) {
		LSFLOAT fError = CStd::Min( _fAlpha, ::fabsf( 255.0f - _fAlpha ) );	// Minimum error between the implicit 0 and 255 key colors.
		if ( fError == 0.0f ) {
			// Exact match.
			return fError;
		}
		fError = CStd::Min( fError, CStd::Min( ::fabsf( _fAlpha0 - _fAlpha ), ::fabsf( _fAlpha1 - _fAlpha ) ) );	// Minimum error between the key colors.
		if ( fError == 0.0f ) {
			// One of the key colors is an exact match.
			return fError;
		}
		// An interpolated value might be a better match.
		if ( _fAlpha < _fAlpha1 && _fAlpha > _fAlpha0 ) {
			for ( LSUINT32 I = 1; I <= 4; ++I ) {
				// (4 * alpha_0 + 1 * alpha_1 + 2) / 5
				LSFLOAT fI = static_cast<LSFLOAT>(I);
				LSFLOAT fVal = ((5.0f - fI) * _fAlpha0 + fI * _fAlpha1 + 2.0f) * (1.0f / 5.0f);
				fError = CStd::Min( fError, ::fabsf( fVal - _fAlpha ) );
			}
		}
		fError *= (1.0f / 255.0f);
		return fError * fError;
	}

	/**
	 * Gets the amount of non-normalized error for a DXT5 alpha block using the alpha0 <= alpha1 rules.
	 *	Of note is that this is a special-case function which assumes the alpha values in the block have
	 *	already been set to the 0-255 range.
	 *
	 * \param _fAlpha0 alpha0.
	 * \param _fAlpha1 alpha1.
	 * \param _bBlock Contains the 4-by-4 block of texels to be used to determine the key colors.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \return Returns the minimum amount of error between the alpha points for the given block.
	 */
	LSFLOAT LSE_CALL CImage::GetErrorDxt5Alpha1( LSFLOAT _fAlpha0, LSFLOAT _fAlpha1,
		const LSI_BLOCK _bBlock[4][4], LSUINT32 _ui32Width, LSUINT32 _ui32Height ) {
		LSFLOAT fError = 0.0f;
		for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				fError += GetErrorDxt5Alpha1( _fAlpha0, _fAlpha1, _bBlock[Y][X].s.fA );
			}
		}
		return fError;
	}

	/**
	 * Gets the amount of error of an entire block between 2 colors.  The orders of the highs and lows in the channels does
	 *	not impact the result.
	 *
	 * \param _bUniqueColors The input block of 4-by-4 texels.
	 * \param _bMax The max color.
	 * \param _bMin The min color.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _bOmitLast If true, 3 colors are returned instead of 4.
	 * \param _fCurError The current amount of error.  Knowing this helps the routine early-out.
	 * \param _doOptions Weights to use for the error calculation.
	 * \return Returns the amount of error among the block given the 2 end colors.
	 */
	LSFLOAT LSE_CALL CImage::GetError( const CStaticMap<LSI_BLOCK, LSUINT32, 16> &_bUniqueColors, const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
		LSUINT32 _ui32Width, LSUINT32 _ui32Height, LSBOOL _bOmitLast, LSFLOAT _fCurError, const LSI_DXT_OPTIONS &_doOptions ) {
		// First create the 4 points that can be matched.
		LSI_BLOCK bMatchPoints[4][4];
		Get4Colors( _bMax, _bMin, bMatchPoints[0], _bOmitLast );
		CStd::MemCpy( bMatchPoints[1], bMatchPoints[0], sizeof( bMatchPoints[0] ) );
		CStd::MemCpy( bMatchPoints[2], bMatchPoints[0], sizeof( bMatchPoints[0] ) );
		CStd::MemCpy( bMatchPoints[3], bMatchPoints[0], sizeof( bMatchPoints[0] ) );

		LSUINT32 ui32Colors = _bOmitLast ? 3 : 4;
		for ( LSUINT32 I = ui32Colors; I--; ) {
			const LSI_BLOCK &bSrc = bMatchPoints[0][ui32Colors-1-I];
			bMatchPoints[1][I].fValues[0] = bSrc.fValues[0];	// Swap red.
			bMatchPoints[2][I].fValues[1] = bSrc.fValues[1];	// Swap blue.
			bMatchPoints[3][I].fValues[2] = bSrc.fValues[2];	// Swap green.
		}

		CVector4 vChannelErrors[4] = {
			CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ),
			CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ),
			CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ),
			CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ),
		};
		LSFLOAT fTotal[4] = { 0.0f };
		LSFLOAT fMaxError[4] = { 0.0f };
		LSFLOAT fBucketMinError = _fCurError;
		LSFLOAT fNormalizer = 1.0f / (_ui32Width * _ui32Height);

		// For each error bucket.
		for ( LSUINT32 B = 4; B--; ) {
			// For each texel.
			LSFLOAT fTempTotal = 0.0f;
			for ( LSUINT32 J = _bUniqueColors.Length(); J--; ) {
				const LSI_BLOCK &bColor = _bUniqueColors.GetKeyByIndex( J );
				// Find out to which point it is closest.
				LSFLOAT fMinError = 10.0f;
				LSUINT32 ui32Slot = 0;
				CVector4 vThisError;
				vThisError.w = 0.0f;
				for ( LSUINT32 A = ui32Colors; A--; ) {
					LSFLOAT fThisError = GetError( bMatchPoints[B][A], bColor, _doOptions );
					if ( fThisError < fMinError ) {
						fMinError = fThisError;
						ui32Slot = A;
						vThisError.x = bMatchPoints[B][A].s.fR - bColor.s.fR;
						vThisError.y = bMatchPoints[B][A].s.fG - bColor.s.fG;
						vThisError.z = bMatchPoints[B][A].s.fB - bColor.s.fB;
						//vThisError.w = bMatchPoints[B][A].s.fA - bColor.s.fA;
						vThisError.x *= vThisError.x;
						vThisError.y *= vThisError.y;
						vThisError.z *= vThisError.z;
						//vThisError.w *= vThisError.w;
						if ( fThisError == 0.0f ) { break; }
					}
				}

				// Add its error to the appropriate bucket.
				fMaxError[B] = CStd::Max( fMaxError[B], fMinError );
				vChannelErrors[B] += vThisError * static_cast<LSREAL>(_bUniqueColors.GetByIndex( J ));

				LSFLOAT fA = vChannelErrors[B].x * fNormalizer;
				LSFLOAT fB = vChannelErrors[B].y * fNormalizer;
				LSFLOAT fC = vChannelErrors[B].z * fNormalizer;
#define LSI_ERROR( A, B, C )	(A) * _doOptions.fRedWeight + (B) * _doOptions.fGreenWeight + (C) * _doOptions.fBlueWeight// + (A) + (B) + (C)
				fTempTotal = LSI_ERROR( fA, fB, fC );
				fTempTotal += fMaxError[B] * fMaxError[B] * fNormalizer;
				if ( fTempTotal >= fBucketMinError ) {
					// It can't win.  Another bucket was already closer.
					// Make sure this bucket can't win later.
					fTempTotal = 100.0f;
					vChannelErrors[B].x = 100.0f;
					break;
				}
			}
			fBucketMinError = CStd::Min( fBucketMinError, fTempTotal );
			if ( fBucketMinError == 0.0f ) { return 0.0f; }
		}

		// Get the MSE of each bucket.
		for ( LSUINT32 B = 0; B < 4; ++B ) {
			vChannelErrors[B] *= fNormalizer;
			fTotal[B] = LSI_ERROR( vChannelErrors[B].x, vChannelErrors[B].y, vChannelErrors[B].z );
			fTotal[B] += fMaxError[B] * fMaxError[B] * fNormalizer;	// Add a penalty based on the maximum error.
		}

		// Find the minimum error.
		LSFLOAT fError = fTotal[0];
		for ( LSUINT32 B = 1; B < 4; ++B ) {
			fError = CStd::Min( fError, fTotal[B] );
		}
#undef LSI_ERROR
		return fError;
	}

	/**
	 * Gets the amount of error of an entire block between 2 colors.
	 *
	 * \param _bBlock The input block of 4-by-4 texels.
	 * \param _bMax The max color.
	 * \param _bMin The min color.
	 * \param _ui32Width Width of the block.
	 * \param _ui32Height Height of the block.
	 * \param _bOmitLast If true, 3 colors are returned instead of 4.
	 * \param _doOptions Weights to use for the error calculation.
	 * \return Returns the amount of error among the block given the 2 end colors.
	 */
	LSFLOAT LSE_CALL CImage::GetErrorStrict( const LSI_BLOCK _bBlock[4][4], const LSI_BLOCK &_bMax, const LSI_BLOCK &_bMin,
		LSUINT32 _ui32Width, LSUINT32 _ui32Height, LSBOOL _bOmitLast, const LSI_DXT_OPTIONS &_doOptions ) {
		LSI_BLOCK bMatchPoints[4];
		Get4Colors( _bMax, _bMin, bMatchPoints, _bOmitLast );

		LSUINT32 ui32Colors = _bOmitLast ? 3 : 4;

		// For each texel.
		CVector3 vErrors = CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO );
		for ( LSUINT32 Y = _ui32Height; Y--; ) {
			for ( LSUINT32 X = _ui32Width; X--; ) {
				// Find out to which point it is closest.
				LSFLOAT fMinError = 10.0f;
				LSUINT32 ui32Bucket = 0;
				for ( LSUINT32 A = ui32Colors; A--; ) {
					LSFLOAT fThisError = GetError( bMatchPoints[A], _bBlock[Y][X], _doOptions );
					if ( fThisError < fMinError ) {
						fMinError = fThisError;
						ui32Bucket = A;
						if ( fThisError == 0.0f ) { break; }
					}
				}
				// Add its error to the total.
				CVector3 vError = CVector3( static_cast<LSREAL>(bMatchPoints[ui32Bucket].s.fR - _bBlock[Y][X].s.fR),
					static_cast<LSREAL>(bMatchPoints[ui32Bucket].s.fG - _bBlock[Y][X].s.fG),
					static_cast<LSREAL>(bMatchPoints[ui32Bucket].s.fB - _bBlock[Y][X].s.fB) );
				vErrors.x += vError.x * vError.x;
				vErrors.y += vError.y * vError.y;
				vErrors.z += vError.z * vError.z;
				
			}
		}
		LSREAL fNorm = LSM_ONE / (_ui32Height * _ui32Width);
		vErrors *= fNorm;
		return vErrors.x * _doOptions.fRedWeight + vErrors.y * _doOptions.fGreenWeight + vErrors.z * _doOptions.fBlueWeight;
	}

	/**
	 * Gets the 2 end points that would allow the line segment between them pass through the given value.
	 *
	 * \param _fEndPoint The left end point.
	 * \param _fMidPoint The point through which to pass.
	 * \param _ui32EndIndex The index of the left point.
	 * \param _ui32MidIndex The index of the middle point.
	 * \param _ui32Points The total number of possible points.
	 * \param _fRes The resolution of the rounding.
	 * \param _fLow Holds the low end point on return.
	 * \param _fHigh Holds the high end point on return.
	 */
	LSVOID LSE_CALL CImage::FindEndPoints( LSFLOAT _fEndPoint, LSFLOAT _fMidPoint, LSUINT32 _ui32EndIndex, LSUINT32 _ui32MidIndex,
		LSUINT32 _ui32Points, LSFLOAT _fRes, LSFLOAT &_fLow, LSFLOAT &_fHigh ) {
		LSFLOAT fSlope = (_fMidPoint - _fEndPoint) / (_ui32MidIndex - _ui32EndIndex);
		_fLow = Round( -static_cast<LSINT32>(_ui32MidIndex) * fSlope + _fMidPoint, _fRes );
		_fHigh = Round( static_cast<LSINT32>(_ui32Points - _ui32MidIndex - 1) * fSlope + _fMidPoint, _fRes );
	}

	/**
	 * Thread function for converting a block of texels to DXTn format.
	 *
	 * \param _lpParameter The thread information, stored as a pointer to a LSI_DXT_THREAD_DATA structure.
	 * \return Returns 0.
	 */
	LSUINT32 LSH_CALL CImage::DxtThread( LSVOID * _lpParameter ) {
		LSI_DXT_THREAD_DATA * pdtdData = static_cast<LSI_DXT_THREAD_DATA *>(_lpParameter);


		LSI_BLOCK bColors[4][4];
		LSUINT32 ui32Width = (pdtdData->piSrcImage->GetWidth() + 3) >> 2;
		// Number of blocks along the height.
		LSUINT32 ui32TotalScanLines = (pdtdData->piSrcImage->GetHeight() + 3) >> 2;

		LSUINT32 ui32SrcStride = CImageLib::GetRowSize( pdtdData->piSrcImage->GetFormat(), pdtdData->piSrcImage->GetWidth() );
		const LSUINT8 * pui8Src = &pdtdData->piSrcImage->m_tbBuffer[0];
		pui8Src += (ui32SrcStride << 2) * pdtdData->ui32SrcY;
		// Do 4 rows at a time.
		for ( LSUINT32 K = 0; K < LSI_DXT_ROWS; ++K ) {
			if ( pdtdData->ui32SrcY >= ui32TotalScanLines ) { break; }
			LSE_PREFETCH_LINE( pui8Src + (K + 1) * ui32SrcStride );

			for ( LSUINT32 X = pdtdData->ui32SrcX; X < ui32Width; ++X ) {
				LSUINT32 ui32SrcX = X << 4;							// 4 texels per block wide, 4 channels per texel.
				// Get the 4-by-4 block.
				LSFLOAT fAlphaHigh = 0.0f, fAlphaLow = 1.0f;
				for ( LSUINT32 J = 0; J < 4; ++J ) {				// Down the block height.
					const LSFLOAT * pfThisRowStart = reinterpret_cast<const LSFLOAT *>(pui8Src + J * ui32SrcStride);
					if ( J + (pdtdData->ui32SrcY << 2) >= pdtdData->piSrcImage->GetHeight() ) {
						for ( LSUINT32 I = 0; I < 4; ++I ) {		// Along the block width.
							for ( LSUINT32 C = 0; C < 4; ++C ) {	// For each channel in this texel.
								bColors[J][I].fValues[C] = 0.0f;
							}
						}
					}
					else {
						// The row is valid.
						for ( LSUINT32 I = 0; I < 4; ++I ) {		// Along the block width.
							// We are on a single texel now.  But it might be beyond the width of the image.
							if ( I + (X << 2) >= pdtdData->piSrcImage->GetWidth() ) {
								for ( LSUINT32 C = 0; C < 4; ++C ) {// For each channel in this texel.
									bColors[J][I].fValues[C] = 0.0f;
								}
							}
							else {
								const LSFLOAT * pfSrcTexel = pfThisRowStart + ui32SrcX + (I << 2);
								// It is inside the image, so add it to an entry in the block.
								bColors[J][I].s.fR = pfSrcTexel[LSI_PC_R];
								bColors[J][I].s.fG = pfSrcTexel[LSI_PC_G];
								bColors[J][I].s.fB = pfSrcTexel[LSI_PC_B];
								bColors[J][I].s.fA = pfSrcTexel[LSI_PC_A];
								fAlphaHigh = CStd::Max<LSFLOAT>( fAlphaHigh, bColors[J][I].s.fA );
								fAlphaLow = CStd::Min<LSFLOAT>( fAlphaLow, bColors[J][I].s.fA );
							}
						}
					}
				}

				// The local block is filled.  Send it to the filter to determine the best high and low colors to use.
#if 0
				LSI_BLOCK bLow;
				LSI_BLOCK bHi;
				GetKeyColors( bColors, bHi, bLow,
					CStd::Min<LSUINT32>( 4, pdtdData->piSrcImage->GetWidth() - (X << 2) ),
					CStd::Min<LSUINT32>( 4, pdtdData->piSrcImage->GetHeight() - (pdtdData->ui32SrcY << 2) ),
					fAlphaHigh, fAlphaLow,
					pdtdData->pfFormat,
					(*pdtdData->pdoOptions) );

				switch ( pdtdData->pfFormat ) {
					case LSI_PF_DXT1 : {
						MakeDxt1Block( bColors, bHi, bLow, fAlphaHigh, fAlphaLow, pdtdData->pui8Dest, (*pdtdData->pdoOptions) );
						break;
					}
					case LSI_PF_DXT2 : {}
					case LSI_PF_DXT3 : {
						MakeDxt3Block( bColors, bHi, bLow, pdtdData->pui8Dest, (*pdtdData->pdoOptions) );
						break;
					}
					case LSI_PF_DXT4 : {}
					case LSI_PF_DXT5 : {
						MakeDxt5Block( bColors, bHi, bLow, pdtdData->pui8Dest, (*pdtdData->pdoOptions) );
						break;
					}
					default : {}	// Remove warning.
				}
#else
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
				for ( LSUINT32 Y = 0; Y < 4; ++Y ) {			// Block height.
					for ( LSUINT32 X = 0; X < 4; ++X ) {		// Block width.
						for ( LSUINT32 I = 0; I < 4; ++I ) {	// Colors.
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
				squish::Compress(
#ifdef SQUISH_USE_FLOATS
					pfBlock,
#else
					reinterpret_cast<squish::u8 *>(u8Block),
#endif	// #ifdef SQUISH_USE_FLOATS
					pdtdData->pui8Dest, iFlags, scConfig );
#endif

				pdtdData->pui8Dest += CDds::DdsBlockSize( pdtdData->pfFormat );
			}
			pui8Src += (ui32SrcStride << 2);
			++pdtdData->ui32SrcY;
		}

		pdtdData->bDone = true;
		return 0;
	}

}	// namespace lsi
