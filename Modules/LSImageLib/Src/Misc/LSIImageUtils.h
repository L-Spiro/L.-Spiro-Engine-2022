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


#ifndef __LSI_IMAGEUTILS_H__
#define __LSI_IMAGEUTILS_H__

#include "../LSIImageLib.h"
#include "../Image/LSIImage.h"

namespace lsi {

	/**
	 * Class CImageUtils
	 * \brief Miscellaneous utility functions related to image manipulation..
	 *
	 * Description: Miscellaneous utility functions related to image manipulation..
	 */
	class CImageUtils {
	public :
		// == Functions.
		/**
		 * Gets the histogram for a given image.
		 *
		 * \param _iImage The image for which to generate a histogram.
		 * \param _pui8Buffer The buffer where the histogram is to be stored.  Must be a byte array of 256 values.
		 */
		static void LSE_CALL							GetHistogram( const CImage &_iImage, uint8_t * _pui8Buffer );

		/**
		 * Gets the 3 histograms for a given image.  One histogram for each ofthe R, G, and B channels.
		 *
		 * \param _iImage The image for which to generate a histogram.
		 * \param _pui8BufferR The buffer where the histogram for the R channel is to be stored.  Must be a byte array of 256 values.
		 * \param _pui8BufferG The buffer where the histogram for the G channel is to be stored.  Must be a byte array of 256 values.
		 * \param _pui8BufferB The buffer where the histogram for the B channel is to be stored.  Must be a byte array of 256 values.
		 */
		static void LSE_CALL							GetHistogram( const CImage &_iImage,
			uint8_t * _pui8BufferR, uint8_t * _pui8BufferG, uint8_t * _pui8BufferB );

		/**
		 * Gets the low and high tones from a histogram given some tolerance.
		 *
		 * \param _pui8Buffer The histogram from which to find the high and low tons.
		 * \param _fTolerance Amount of tolerance to apply while searching for the high and low tones.
		 * \param _ui32Low The returned low tone.
		 * \param _ui32High The returned high tone.
		 */
		static void LSE_CALL							FindLowAndHigh( const uint8_t * _pui8Buffer, float _fTolerance,
			uint32_t &_ui32Low, uint32_t &_ui32High );
	};

}	// namespace lsi

#endif	// __LSI_IMAGEUTILS_H__
