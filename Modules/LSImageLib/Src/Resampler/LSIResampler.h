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
 * Description: A resampler capable of resizing images up or down using various filters.
 *	This is a heavily modified port of Rich Geldreich's open-source resampler demonstration code.
 */


#ifndef __LSI_RESAMPLER_H__
#define __LSI_RESAMPLER_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CResampler
	 * \brief A resampler capable of resizing images up or down using various filters.
	 *
	 * Description: A resampler capable of resizing images up or down using various filters.
	 *	This is a heavily modified port of Rich Geldreich's open-source resampler demonstration code.
	 */
	class CResampler {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CResampler();
		LSE_CALLCTOR								~CResampler();


		// == Enumerations.
		/** Maximum scanline sample size. */
		enum {
			LSI_MAX_SCANLINE						= 16384
		};

		/** Addressing mode. */
		enum LSI_ADDRESS_MODE {
			LSI_AM_CLAMP,							/**< Clamps texture look-ups between 0 and N-1. */
			LSI_AM_REPEAT,							/**< Repeats texture look-ups between 0 and N-1. */
			LSI_AM_MIRROR,							/**< Uses mirror texture look-ups. */
		};


		// == Types.
		/** A contribution to a single texel. */
		typedef struct LSI_CONTRIB {
			/** The weight of the contribution. */
			float									fWeight;

			/** The contribution. */
			uint16_t								ui16Value;
		} * LPLSI_CONTRIB, * const LPCLSI_CONTRIB;

		/** References to contributions in a pool of contributions. */
		typedef struct LSI_CONTRIB_EX {
			/** Start of the contributions. */
			LSI_CONTRIB *							pcValues;

			/** Number of references in a pool of contributions. */
			uint32_t								ui32N;
		} * LPLSI_CONTRIB_EX, * const LPCLSI_CONTRIB_EX;

		/** An array of contribution references. */
		typedef struct LSI_CONTRIB_LIST {
			/** Pool of referenced values. */
			CVectorPoD<LSI_CONTRIB, uint32_t>		vPool;

			/** An array of contribution references. */
			CVectorPoD<LSI_CONTRIB_EX, uint32_t>	vList;
		} * LPLSI_CONTRIB_LIST, * const LPCLSI_CONTRIB_LIST;

		/** A buffered scanline. */
		typedef struct LSI_SCANLINE {
			/** Pointers to samples. */
			float *									pfScanBufL[LSI_MAX_SCANLINE];

			/** Scaline values. */
			int32_t									i32ScanBufY[LSI_MAX_SCANLINE];
		} * LPLSI_SCANLINE, * const LPCLSI_SCANLINE;

		/**
		 * A filter function.
		 *
		 * \param _fT The value to pass through the filter.
		 * \return Returns the filtered value.
		 */
		typedef float (LSE_CALL *					PfFilterFunc)( float _fT );

		/** Filter information. */
		typedef struct LSI_FILTER {
			/** Filter name. */
			const char *							pcName;

			/** The filter function to use. */
			PfFilterFunc							pfFunc;

			/** Filter helper value. */
			float									pfSupport;
		} * LPLSI_FILTER, * const LPCLSI_FILTER;


		// == Functions.
		/**
		 * Prepare for resampling.
		 *
		 * \param _ui32SrcWidth Source width.
		 * \param _ui32SrcHeight Source height.
		 * \param _ui32DstWidth Destination width.
		 * \param _ui32DstHeight Destination height.
		 * \param _amAddressMode Texture addressing mode.
		 * \param _fSampleLow Sample low frequency.
		 * \param _fSampleHi Sample high frequency.
		 * \param _pcFilter Name of the filter to use.  Valid values are: "box", "lanczos4".
		 * \param _pclContX Contribution list for the X axis or NULL if one is to be made.
		 * \param _pclContY Contribution list for the Y axis or NULL if one is to be made.
		 * \param _fFilterScaleX X-axis filter scale.
		 * \param _fFilterScaleY Y-axis filter scale.
		 * \param _pvDstBuffer A buffer the class can use for composing scanlines.
		 * \return Returns an error code indicating success or the reason for failure.
		 */
		LSSTD_ERRORS LSE_CALL						InitResampler( uint32_t _ui32SrcWidth, uint32_t _ui32SrcHeight,
			uint32_t _ui32DstWidth, uint32_t _ui32DstHeight,
			LSI_ADDRESS_MODE _amAddressMode = LSI_AM_REPEAT,
			float _fSampleLow = 0.0f, float _fSampleHi = 1.0f,
			const char * _pcFilter = "lanczos4",
			LSI_CONTRIB_LIST * _pclContX = NULL,
			LSI_CONTRIB_LIST * _pclContY = NULL,
			float _fFilterScaleX = 1.0f,
			float _fFilterScaleY = 1.0f,
			CVectorPoD<float, uint32_t> * _pvDstBuffer = NULL );

		/**
		 * Gets the X contribution list.
		 *
		 * \return Returns the X contribution list for read and write.
		 */
		LSI_CONTRIB_LIST * LSE_CALL					GetContribX();

		/**
		 * Gets the Y contribution list.
		 *
		 * \return Returns the Y contribution list for read and write.
		 */
		LSI_CONTRIB_LIST * LSE_CALL					GetContribY();

		/**
		 * Adds a scanline.
		 *
		 * \param _pfScanline Pointer to the scan line to add.  Must be at least the length of the source image width.
		 * \return Returns false if there is not enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								AddScanLine( const float * _pfScanline );

		/**
		 * Gets a scanline.
		 *
		 * \return Returns a pointer to a floating-point array that is the next scanline.
		 */
		const CVectorPoD<float, uint32_t> * LSE_CALL
													GetScanLine();

		/**
		 * Clamps a text coordinate between 0 and the given size.
		 *
		 * \param _i32TexelPos The texel index to clamp.
		 * \param _ui32Size Size of the image.  Provides the upper clamping range.
		 * \param _amAddressMode Texture addressing mode.
		 * \return Returns the clamped pixel index.
		 */
		static int32_t LSE_CALL						ClampTexel( int32_t _i32TexelPos, uint32_t _ui32Size, LSI_ADDRESS_MODE _amAddressMode );


	protected :
		// == Types.
		/** Contribution bounds. */
		typedef struct LSI_CONTRIB_BOUNDS {
			/** The center. */
			float									fCenter;

			/** Left. */
			int32_t									i32Left;

			/** Right. */
			int32_t									i32Right;
		} * LPLSI_CONTRIB_BOUNDS, * const LPCLSI_CONTRIB_BOUNDS;
		
		// == Members.
		/** X contributions. */
		LSI_CONTRIB_LIST *							m_pclContribsX;

		/** Y contributions. */
		LSI_CONTRIB_LIST *							m_pclContribsY;

		/** The filter to use. */
		LSI_FILTER *								m_pfFilter;

		/** If true, the X contibution list is borrowed. */
		LSBOOL										m_bBorrowedX;

		/** If true, the Y contibution list is borrowed. */
		LSBOOL										m_bBorrowedY;

		/** Sample low. */
		float										m_fLow;

		/** Sample high. */
		float										m_fHi;

		/** Source width. */
		uint32_t									m_ui32SrcWidth;

		/** Source height. */
		uint32_t									m_ui32SrcHeight;

		/** Destination width. */
		uint32_t									m_ui32DstWidth;

		/** Destination height. */
		uint32_t									m_ui32DstHeight;

		/** Current source Y. */
		uint32_t									m_ui32CurSrcY;

		/** Current destination Y. */
		uint32_t									m_ui32CurDstY;

		/** Intermediate X. */
		uint32_t									m_ui32IntermX;

		/** Delay the X? */
		LSBOOL										m_bDelayX;

		/** Source Y count. */
		mutable CVectorPoD<int32_t, uint32_t>		m_vSrcCountY;

		/** Source Y flags. */
		mutable CVectorPoD<uint8_t, uint32_t>		m_vSrcFlagsY;

		/** Destination sampler buffer. */
		CVectorPoD<float, uint32_t> *				m_pvDstBuffer;

		/** Temporary sampler buffer. */
		mutable CVectorPoD<float, uint32_t>			m_vTempBuffer;

		/** Scanline buffer. */
		LSI_SCANLINE *								m_psScanBuffer;

		/** A chunk allocated for use by the scanline buffer. */
		CVectorPoD<float, uint32_t>					m_vScanBufferChunk;

		/** Current position within the chunk. */
		uint32_t									m_ui32ScanChunkPos;

		/** The last chunk search position. */
		uint32_t									m_ui32LastSearchPos;

		/** Filter functions. */
		static LSI_FILTER							m_fFilters[];


		// == Functions.
		/**
		 * Resample along the X axis.
		 *
		 * \param _pfDst The destination buffer into which to resample texels.
		 * \param _pfSrc The source buffer of texels.
		 */
		void LSE_CALL								ResampleX( float * _pfDst, const float * _pfSrc ) const;

		/**
		 * Resample along the Y axis.
		 *
		 * \param _vDst The destination buffer into which to resample texels.
		 */
		void LSE_CALL								ResampleY( CVectorPoD<float, uint32_t> &_vDst ) const;

		/**
		 * Copy from one buffer to another, scaling each component.
		 *
		 * \param _pfDst The destination buffer.
		 * \param _pfSrc The source buffer.
		 * \param _fScale The scaling to apply to each copy.
		 * \param _ui32Total Number of elements to copy.
		 */
		static void LSE_CALL						ScaleY( CVectorPoD<float, uint32_t> &_pfDst, const float * _pfSrc, float _fScale, uint32_t _ui32Total );

		/**
		 * Adds values from one buffer to another, scaling each component.
		 *
		 * \param _pfDst The destination buffer.
		 * \param _pfSrc The source buffer.
		 * \param _fScale The scaling to apply to each copy.
		 * \param _ui32Total Number of elements to copy.
		 */
		static void LSE_CALL						ScaleYAdd( CVectorPoD<float, uint32_t> &_pfDst, const float * _pfSrc, float _fScale, uint32_t _ui32Total );

		/**
		 * Clamp all the samples into a certain range.
		 *
		 * \param _pfSamples The samples to clamp.
		 * \param _ui32Total The total number of samples to which _pfSamples points.
		 * \param _fLo The low clamp value.
		 * \param _fHi The high clamp value.
		 */
		static void LSE_CALL						ClampSamples( float * _pfSamples, uint32_t _ui32Total, float _fLo, float _fHi );

		/**
		 * Allocates and returns a new contribution list.  If NULL is not returned, the list must be deallocated via LSEDELETE.
		 *
		 * \param _ui32SrcSize Size of the source.
		 * \param _ui32DstSize Size of the destination.
		 * \param _amAddressMode Texture addressing mode.
		 * \param _pfFilter Filter function.
		 * \param _fFilterSupport Filter support value.
		 * \param _fFilterScale Filter scale.
		 * \return Returns a new contribution list which must be freed via LSEDELETE.
		 */
		static LSI_CONTRIB_LIST *					CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
			LSI_ADDRESS_MODE _amAddressMode,
			PfFilterFunc _pfFilter, float _fFilterSupport, float _fFilterScale );

		/**
		 * Integer modulus with special-case handling for negative numbers.
		 *
		 * \param _i32X The value to modulus.
		 * \param _ui32Mod The value by which to modulus _i32X.
		 * \return Returns the integer modulus with special handling for negative cases for use with images.
		 */
		static uint32_t LSE_INLINE LSE_FCALL		PosMod( int32_t _i32X, uint32_t _ui32Mod );

		/**
		 * Count the operations in a contribution list.
		 *
		 * \param _clList The list whose operation count is to be obtained.
		 * \return Returns the number of operations in the contribution list.
		 */
		static uint32_t LSE_CALL					CountOps( const LSI_CONTRIB_LIST &_clList );

		/**
		 * Reject values below a specific epsilon.
		 *
		 * \param _dVal The value to test.
		 * \return Returns the given value if it is above a certain epsilon or 0.
		 */
		static LSE_INLINE float LSE_CALL			Clean( double _dVal );

		/**
		 * A helper function for the Kaiser filter.
		 *
		 * \param _dAlpha A happy parameter.
		 * \param _dHalfWidth A happy parameter.
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static LSE_INLINE double LSE_CALL			KaiserHelper( double _dAlpha, double _dHalfWidth, double _dX );

		/**
		 * A helper function for the Mitchell filter.
		 *
		 * \param _fT The filter input.
		 * \param _fB A happy parameter.
		 * \param _fC A happy parameter.
		 * \return Returns happiness.
		 */
		static LSE_INLINE float LSE_CALL			MitchellFilterHelper( float _fT, float _fB, float _fC );

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static LSE_INLINE float LSE_CALL			BlackmanWindow( double _dX );

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static LSE_INLINE double LSE_CALL			Bessel0( double _dX );

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static LSE_INLINE double LSE_CALL			SinC( double _dX );

		/**
		 * The point filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						PointFilterFunc( float _fT );

		/**
		 * The tent/bilinear filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						BilinearFilterFunc( float _fT );

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \param _fB Helper value.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						QuadraticFilterFunc( float _fT, float _fB );

		/**
		 * Quadratic filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						QuadraticInterpFilterFunc( float _fT );

		/**
		 * The Kaiser filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						KaiserFilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 2 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos2FilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 3 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos3FilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 4 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos4FilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 6 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos6FilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 8 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos8FilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 12 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos12FilterFunc( float _fT );

		/**
		 * The Lanczos filter function with 64 samples.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						Lanczos64FilterFunc( float _fT );

		/**
		 * The Mitchell filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						MitchellFilterFunc( float _fT );

		/**
		 * The Catmull-Rom filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						CatmullRomFilterFunc( float _fT );

		/**
		 * The B-Spline filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						BSplineFilterFunc( float _fT );

		/**
		 * The Blackman filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						BlackmanFilterFunc( float _fT );

		/**
		 * The Gaussian filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						GaussianFilterFunc( float _fT );

		/**
		 * The Bell filter function.
		 *
		 * \param _fT The value to filter.
		 * \return Returns the filtered value.
		 */
		static float LSE_CALL						BellFilterFunc( float _fT );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Integer modulus with special-case handling for negative numbers.
	 *
	 * \param _i32X The value to modulus.
	 * \param _ui32Mod The value by which to modulus _i32X.
	 * \return Returns the integer modulus with special handling for negative cases for use with images.
	 */
	uint32_t LSE_INLINE LSE_FCALL CResampler::PosMod( int32_t _i32X, uint32_t _ui32Mod ) {
		if (_i32X >= 0) {
			return static_cast<uint32_t>(_i32X) % _ui32Mod;
		}
		else {
			uint32_t ui32M = static_cast<uint32_t>(-_i32X) % _ui32Mod;

			if ( ui32M != 0 ) {
				ui32M = _ui32Mod - ui32M;
			}

			return ui32M;
		}
	}

	/**
	 * Reject values below a specific epsilon.
	 *
	 * \param _dVal The value to test.
	 * \return Returns the given value if it is above a certain epsilon or 0.
	 */
	LSE_INLINE float LSE_CALL CResampler::Clean( double _dVal ) {
		static const double dEps = 0.0000125;
		return ::fabs( _dVal ) >= dEps ? static_cast<float>(_dVal) : 0.0f;
	}

	/**
	 * A helper function for the Kaiser filter.
	 *
	 * \param _dAlpha A happy parameter.
	 * \param _dHalfWidth A happy parameter.
	 * \param _dX A happy parameter.
	 * \return Returns happiness.
	 */
	LSE_INLINE double LSE_CALL CResampler::KaiserHelper( double _dAlpha, double _dHalfWidth, double _dX ) {
		const double dRatio = _dX / _dHalfWidth;
		return Bessel0( _dAlpha * ::sqrt( 1.0 - dRatio * dRatio ) ) / Bessel0( _dAlpha );
	}

	/**
	 * A helper function for the Mitchell filter.
	 *
	 * \param _fT The filter input.
	 * \param _fB A happy parameter.
	 * \param _fC A happy parameter.
	 * \return Returns happiness.
	 */
	LSE_INLINE float LSE_CALL CResampler::MitchellFilterHelper( float _fT, float _fB, float _fC ) {
		float fTt = _fT * _fT;
		_fT = ::fabsf( _fT );
		if ( _fT < 1.0f ) {
			_fT = (((12.0f - 9.0f * _fB - 6.0f * _fC) * (_fT * fTt))
				+ ((-18.0f + 12.0f * _fB + 6.0f * _fC) * fTt)
				+ (6.0f - 2.0f * _fB));
			return _fT / 6.0f;
		}
		else if ( _fT < 2.0f ) {
			_fT = (((-1.0f * _fB - 6.0f * _fC) * (_fT * fTt))
				+ ((6.0f * _fB + 30.0f * _fC) * fTt)
				+ ((-12.0f * _fB - 48.0f * _fC) * _fT)
				+ (8.0f * _fB + 24.0f * _fC));

			return (_fT / 6.0f);
		}

		return 0.0f;
	}

	/**
	 * A helper function.
	 *
	 * \param _dX A happy parameter.
	 * \return Returns happiness.
	 */
	LSE_INLINE float LSE_CALL CResampler::BlackmanWindow( double _dX ) {
		return static_cast<float>(0.42659071 + 0.49656062 * ::cos( LSM_PI_DOUBLE * _dX ) + 0.07684867 * ::cos( 2.0 * LSM_PI_DOUBLE * _dX ));
	}

	/**
	 * A helper function.
	 *
	 * \param _dX A happy parameter.
	 * \return Returns happiness.
	 */
	LSE_INLINE double LSE_CALL CResampler::Bessel0( double _dX ) {
		static const double dEspiRatio = 1.0e-16;
		double dSum = 1.0, dPow = 1.0, dDs = 1.0, dXh = _dX * 0.5;
		uint32_t ui32K = 0;

		while ( dDs > dSum * dEspiRatio ) {
			++ui32K;
			dPow *= (dXh / ui32K);
			dDs = dPow * dPow;
			dSum += dDs;
		}

		return dSum;
	}

	/**
	 * A helper function.
	 *
	 * \param _dX A happy parameter.
	 * \return Returns happiness.
	 */
	LSE_INLINE double LSE_CALL CResampler::SinC( double _dX ) {
		_dX *= LSM_PI_DOUBLE;
		if ( _dX < 0.01 && _dX > -0.01 ) {
			return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
		}

		return ::sin( _dX ) / _dX;
	}

}	// namespace lsi

#endif	// __LSI_RESAMPLER_H__
