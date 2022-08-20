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
 * Description: Miscellaneous utility functions related to image manipulation.
 */

#include "LSIImageUtils.h"


namespace lsi {

	// == Functions.
	/**
	 * Gets the histogram for a given image.
	 *
	 * \param _iImage The image for which to generate a histogram.
	 * \param _pui8Buffer The buffer where the histogram is to be stored.  Must be a byte array of 256 values.
	 */
	void LSE_CALL CImageUtils::GetHistogram( const CImage &_iImage, uint8_t * _pui8Buffer ) {
		for ( uint32_t I = 256; I--; ) {
			_pui8Buffer[I] = 0;
		}

		for ( uint32_t Y = _iImage.GetHeight(); Y--; ) {
			for ( uint32_t X = _iImage.GetWidth(); X--; ) {
				uint64_t ui64Texel = _iImage.GetTexelAt( LSI_PF_R8G8B8A8, X, Y );
				float fVal = LSI_R( ui64Texel ) * 0.3f +
					LSI_G( ui64Texel ) * 0.59f +
					LSI_B( ui64Texel ) * 0.11f;
				uint32_t ui32Index = static_cast<uint32_t>(CStd::Clamp( fVal, 0.0f, 1.0f ) * 255.0f);
				++_pui8Buffer[ui32Index];
			}
		}
	}

	/**
	 * Gets the 3 histograms for a given image.  One histogram for each ofthe R, G, and B channels.
	 *
	 * \param _iImage The image for which to generate a histogram.
	 * \param _pui8BufferR The buffer where the histogram for the R channel is to be stored.  Must be a byte array of 256 values.
	 * \param _pui8BufferG The buffer where the histogram for the G channel is to be stored.  Must be a byte array of 256 values.
	 * \param _pui8BufferB The buffer where the histogram for the B channel is to be stored.  Must be a byte array of 256 values.
	 */
	void LSE_CALL CImageUtils::GetHistogram( const CImage &_iImage,
		uint8_t * _pui8BufferR, uint8_t * _pui8BufferG, uint8_t * _pui8BufferB ) {
		for ( uint32_t I = 256; I--; ) {
			_pui8BufferR[I] = _pui8BufferG[I] = _pui8BufferB[I] = 0;
		}

		for ( uint32_t Y = _iImage.GetHeight(); Y--; ) {
			for ( uint32_t X = _iImage.GetWidth(); X--; ) {
				uint64_t ui64Texel = _iImage.GetTexelAt( LSI_PF_R8G8B8A8, X, Y );
				++_pui8BufferR[LSI_R( ui64Texel )];
				++_pui8BufferG[LSI_G( ui64Texel )];
				++_pui8BufferB[LSI_B( ui64Texel )];
			}
		}
	}

	/**
	 * Gets the low and high tones from a histogram given some tolerance.
	 *
	 * \param _pui8Buffer The histogram from which to find the high and low tons.
	 * \param _fTolerance Amount of tolerance to apply while searching for the high and low tones.
	 * \param _ui32Low The returned low tone.
	 * \param _ui32High The returned high tone.
	 */
	void LSE_CALL CImageUtils::FindLowAndHigh( const uint8_t * _pui8Buffer, float _fTolerance,
		uint32_t &_ui32Low, uint32_t &_ui32High ) {
		uint8_t ui8Tol = static_cast<uint8_t>(CStd::Clamp( _fTolerance, 0.0f, 1.0f ) * 255.0f);
		// Find the high tolerance.
		for ( _ui32High = 256; _ui32High--; ) {
			if ( _pui8Buffer[_ui32High] > ui8Tol ) { break; }
		}

		// Find the low tolerance.
		for ( _ui32Low = 0; _ui32Low < 256; ++_ui32Low ) {
			if ( _pui8Buffer[_ui32Low] > ui8Tol ) { break; }
		}
	}

}	// namespace lsi
