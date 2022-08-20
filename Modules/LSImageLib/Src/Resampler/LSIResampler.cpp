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

#include "LSIResampler.h"


namespace lsi {

	/**
	 * Filter functions.
	 */
	CResampler::LSI_FILTER CResampler::m_fFilters[] = {
		{ "box",			CResampler::PointFilterFunc,				0.5f },
		{ "bilinear",		CResampler::BilinearFilterFunc,				1.0f },
		{ "quadratic",		CResampler::QuadraticInterpFilterFunc,		1.0f },
		{ "kaiser",			CResampler::KaiserFilterFunc,				3.0f },
		{ "lanczos2",		CResampler::Lanczos2FilterFunc,				2.0f },
		{ "lanczos3",		CResampler::Lanczos3FilterFunc,				3.0f },
		{ "lanczos4",		CResampler::Lanczos4FilterFunc,				4.0f },
		{ "lanczos6",		CResampler::Lanczos6FilterFunc,				6.0f },
		{ "lanczos8",		CResampler::Lanczos8FilterFunc,				8.0f },
		{ "lanczos12",		CResampler::Lanczos12FilterFunc,			12.0f },
		{ "lanczos64",		CResampler::Lanczos64FilterFunc,			64.0f },
		{ "mitchell",		CResampler::MitchellFilterFunc,				2.0f },
		{ "catmullrom",		CResampler::CatmullRomFilterFunc,			2.0f },
		{ "bspline",		CResampler::BSplineFilterFunc,				2.0f },
		{ "blackman",		CResampler::BlackmanFilterFunc,				3.0f },
		{ "gaussian",		CResampler::GaussianFilterFunc,				1.25f },
		{ "bell",			CResampler::BellFilterFunc,					1.5f },
	};

	// == Various constructors.
	LSE_CALLCTOR CResampler::CResampler() :
		m_pfFilter( NULL ),
		m_psScanBuffer( NULL ),
		m_pvDstBuffer( NULL ),
		m_ui32CurSrcY( 0 ),
		m_ui32CurDstY( 0 ),
		m_pclContribsX( NULL ),
		m_pclContribsY( NULL ),
		m_bBorrowedX( false ),
		m_bBorrowedY( false ) {
	}
	LSE_CALLCTOR CResampler::~CResampler() {
		m_vTempBuffer.Reset();
		m_vScanBufferChunk.Reset();
		m_vSrcCountY.Reset();
		m_vSrcFlagsY.Reset();
		if ( !m_bBorrowedX ) {
			LSEDELETE m_pclContribsX;
		}
		if ( !m_bBorrowedY ) {
			LSEDELETE m_pclContribsY;
		}
		m_pclContribsX = m_pclContribsY = NULL;


		/*if ( m_psScanBuffer ) {
			for ( uint32_t I = LSI_MAX_SCANLINE; I--; ) {
				LSEDELETE [] m_psScanBuffer->pfScanBufL[I];
			}
		}*/
		LSEDELETE m_psScanBuffer;
	}

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
	LSSTD_ERRORS LSE_CALL CResampler::InitResampler( uint32_t _ui32SrcWidth, uint32_t _ui32SrcHeight,
		uint32_t _ui32DstWidth, uint32_t _ui32DstHeight,
		LSI_ADDRESS_MODE _amAddressMode,
		float _fSampleLow, float _fSampleHi,
		const char * _pcFilter,
		LSI_CONTRIB_LIST * _pclContX,
		LSI_CONTRIB_LIST * _pclContY,
		float _fFilterScaleX,
		float _fFilterScaleY,
		CVectorPoD<float, uint32_t> * _pvDstBuffer ) {

		m_pclContribsX = _pclContX;
		m_pclContribsY = _pclContY;
		m_bBorrowedX = _pclContX != NULL;
		m_bBorrowedY = _pclContY != NULL;	// m_bBorrowedY will always be the same as m_bBorrowedX.
		m_fLow = _fSampleLow;
		m_fHi = _fSampleHi;
		m_ui32SrcWidth = _ui32SrcWidth;
		m_ui32SrcHeight = _ui32SrcHeight;
		m_ui32DstWidth = _ui32DstWidth;
		m_ui32DstHeight = _ui32DstHeight;
		m_pvDstBuffer = _pvDstBuffer;

#ifdef _DEBUG
#define LSI_FAIL( STRING )			CStd::DebugPrintA( "CResampler::InitResampler(): " STRING "\r\n" )
#else
#define LSI_FAIL( STRING )			
#endif	// #ifdef _DEBUG

		for ( uint32_t I = LSE_ELEMENTS( m_fFilters ); I--; ) {
			if ( CStd::StrCmp( m_fFilters[I].pcName, _pcFilter ) == 0 ) {
				m_pfFilter = &m_fFilters[I];
				break;
			}
		}

		if ( !m_pclContribsX ) {
			m_pclContribsX = CreateContribList( m_ui32SrcWidth, m_ui32DstWidth, _amAddressMode, m_pfFilter->pfFunc, m_pfFilter->pfSupport, _fFilterScaleX );
			if ( !m_pclContribsX ) {
				LSI_FAIL( "Unable to allocate X contributions." );
				return LSSTD_E_OUTOFMEMORY;
			}
		}

		if ( !m_pclContribsY ) {
			// Can we match it with the X?
			if ( m_ui32SrcHeight == m_ui32SrcWidth && m_ui32DstHeight == m_ui32DstWidth && _fFilterScaleY == _fFilterScaleX ) {
				// All parameters are the same as on the X list.  No need to duplicate it.
				m_pclContribsY = m_pclContribsX;
				m_bBorrowedY = true;
			}
			else {
				m_pclContribsY = CreateContribList( m_ui32SrcHeight, m_ui32DstHeight, _amAddressMode, m_pfFilter->pfFunc, m_pfFilter->pfSupport, _fFilterScaleY );
				if ( !m_pclContribsY ) {
					LSI_FAIL( "Unable to allocate Y contributions." );
					return LSSTD_E_OUTOFMEMORY;
				}
			}
		}

		if ( !m_vSrcCountY.Resize( m_ui32SrcHeight ) ) {
			LSI_FAIL( "Unable to allocate source counts." );
			return LSSTD_E_OUTOFMEMORY;
		}
		for ( uint32_t I = m_vSrcCountY.Length(); I--; ) {
			m_vSrcCountY[I] = 0;
		}

		if ( !m_vSrcFlagsY.Resize( m_ui32SrcHeight ) ) {
			LSI_FAIL( "Unable to allocate source flags." );
			return LSSTD_E_OUTOFMEMORY;
		}
		for ( uint32_t I = m_vSrcFlagsY.Length(); I--; ) {
			m_vSrcFlagsY[I] = false;
		}
		

		// Count how many times the source lines contribute to the destination lines.
		for ( uint32_t I = m_ui32DstHeight; I--; ) {
			for ( uint32_t J = m_pclContribsY->vList[I].ui32N; J--; ) {
				uint32_t ui32Index = m_pclContribsY->vList[I].pcValues[J].ui16Value;
				if ( ui32Index >= m_ui32SrcHeight ) {
					LSI_FAIL( "Index out of range." );
					return LSSTD_E_INTERNALERROR;
				}
				++m_vSrcCountY[ui32Index];
			}
		}


		m_psScanBuffer = LSENEW LSI_SCANLINE;
		if ( !m_psScanBuffer ) {
			LSI_FAIL( "Unable to allocate scan buffer." );
			return LSSTD_E_OUTOFMEMORY;
		}
		for ( uint32_t I = LSI_MAX_SCANLINE; I--; ) {
			m_psScanBuffer->pfScanBufL[I] = NULL;
			m_psScanBuffer->i32ScanBufY[I] = -1;
		}


		uint32_t ui32OpsX = CountOps( (*m_pclContribsX) );
		uint32_t ui32OpsY = CountOps( (*m_pclContribsY) );

		uint32_t ui32XyOps = ui32OpsX * m_ui32SrcHeight + (4 * ui32OpsY * m_ui32DstWidth) / 3;
		uint32_t ui32YxOps = (4 * ui32OpsY * m_ui32SrcWidth) / 3 + ui32OpsX * m_ui32DstHeight;

		// Which resample order is better?
		if ( (ui32XyOps > ui32YxOps) ||
			((ui32XyOps == ui32YxOps) && (m_ui32SrcWidth < m_ui32DstHeight)) ) {
			m_bDelayX = true;
			m_ui32IntermX = m_ui32SrcWidth;
		}
		else {
			m_bDelayX = false;
			m_ui32IntermX = m_ui32DstWidth;
		}


		if ( m_bDelayX ) {
			if ( !m_vTempBuffer.Resize( m_ui32IntermX ) ) {
				// Not worth failure here.  Just use the slower processing order.
				m_bDelayX = false;
				m_ui32IntermX = m_ui32DstWidth;
			}
		}


		if ( !m_vScanBufferChunk.Resize( m_ui32IntermX * _ui32SrcHeight ) ) {
			LSI_FAIL( "Unable to allocate scan line chunk." );
			return LSSTD_E_OUTOFMEMORY;
		}
		m_ui32ScanChunkPos = m_ui32LastSearchPos = 0;
		return LSSTD_E_SUCCESS;
#undef LSI_FAIL
	}

	/**
	 * Gets the X contribution list.
	 *
	 * \return Returns the X contribution list for read and write.
	 */
	CResampler::LSI_CONTRIB_LIST * LSE_CALL CResampler::GetContribX() {
		return m_pclContribsX;
	}

	/**
	 * Gets the Y contribution list.
	 *
	 * \return Returns the Y contribution list for read and write.
	 */
	CResampler::LSI_CONTRIB_LIST * LSE_CALL CResampler::GetContribY() {
		return m_pclContribsY;
	}

	/**
	 * Adds a scanline.
	 *
	 * \param _pfScanline Pointer to the scan line to add.  Must be at least the length of the source image width.
	 * \return Returns false if there is not enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CResampler::AddScanLine( const float * _pfScanline ) {
		if ( m_ui32CurSrcY >= m_ui32SrcHeight ) { return false; }
		// If this scanline does not contribute to the line, do nothing.
		if ( !m_vSrcCountY[m_ui32CurSrcY] ) {
			++m_ui32CurSrcY;
			return true;
		}


		// Find an empty line in the buffer.
		uint32_t I = m_ui32LastSearchPos;
		for ( ; I < LSI_MAX_SCANLINE; ++I ) {
			if ( m_psScanBuffer->i32ScanBufY[I] == -1 ) { break; }
		}
		if ( I == LSI_MAX_SCANLINE ) {
			for ( I = 0; I < m_ui32LastSearchPos; ++I ) {
				if ( m_psScanBuffer->i32ScanBufY[I] == -1 ) { break; }
			}
		}
		if ( I == LSI_MAX_SCANLINE ) {
			// Failed to find one.
			return false;
		}
		m_ui32LastSearchPos = I;

		m_vSrcFlagsY[m_ui32CurSrcY] = true;
		m_psScanBuffer->i32ScanBufY[I] = static_cast<int32_t>(m_ui32CurSrcY);

		// Allocate memory if needed.
		if ( !m_psScanBuffer->pfScanBufL[I] ) {
			/*m_psScanBuffer->pfScanBufL[I] = LSENEW float[m_ui32IntermX];
			if ( !m_psScanBuffer->pfScanBufL[I] ) {
				return false;
			}*/
			m_psScanBuffer->pfScanBufL[I] = &m_vScanBufferChunk[m_ui32ScanChunkPos];
			m_ui32ScanChunkPos += m_ui32IntermX;
			assert( m_vScanBufferChunk.Length() >= m_ui32ScanChunkPos );
		}



		if ( m_bDelayX ) {
			assert( m_ui32IntermX == m_ui32SrcWidth );
			// X-Y sampling order.
			CStd::MemCpy( m_psScanBuffer->pfScanBufL[I], _pfScanline, m_ui32IntermX * sizeof( float ) );
		}
		else {
			ResampleX( m_psScanBuffer->pfScanBufL[I], _pfScanline );
		}


		++m_ui32CurSrcY;
		return true;
	}

	/**
	 * Gets a scanline.
	 *
	 * \return Returns a pointer to a floating-point array that is the next scanline.
	 */
	const CVectorPoD<float, uint32_t> * LSE_CALL CResampler::GetScanLine() {
		if ( m_ui32CurDstY == m_ui32DstHeight ) { return NULL; }

		// Check that all the needed contributors are there.
		for ( uint32_t I = 0; I < m_pclContribsY->vList[m_ui32CurDstY].ui32N; ++I ) {
			if ( !m_vSrcFlagsY[m_pclContribsY->vList[m_ui32CurDstY].pcValues[I].ui16Value] ) { return NULL; }
		}

		ResampleY( (*m_pvDstBuffer) );
		++m_ui32CurDstY;
		return m_pvDstBuffer;
	}

	/**
	 * Clamps a text coordinate between 0 and the given size.
	 *
	 * \param _i32TexelPos The texel index to clamp.
	 * \param _ui32Size Size of the image.  Provides the upper clamping range.
	 * \param _amAddressMode Texture addressing mode.
	 * \return Returns the clamped pixel index.
	 */
	int32_t LSE_CALL CResampler::ClampTexel( int32_t _i32TexelPos, uint32_t _ui32Size, LSI_ADDRESS_MODE _amAddressMode ) {
		switch ( _amAddressMode ) {
			case LSI_AM_REPEAT : {
				return PosMod( _i32TexelPos, _ui32Size );
			}
			case LSI_AM_MIRROR : {
				if ( _i32TexelPos < 0 ) {
					int32_t i32N = -_i32TexelPos;
					return CStd::Min<int32_t>( i32N, _ui32Size - 1 );
				}
				else if ( static_cast<uint32_t>(_i32TexelPos) >= _ui32Size ) {
					int32_t i32N = (static_cast<int32_t>(_ui32Size) - _i32TexelPos) + static_cast<int32_t>(_ui32Size - 1);
					return CStd::Max<int32_t>( i32N, 0 );
				}
				return _i32TexelPos;
			}
			default : {
				return CStd::Clamp<int32_t>( _i32TexelPos, 0, static_cast<int32_t>(_ui32Size - 1) );
			}
		}
	}

	/**
	 * Resample along the X axis.
	 *
	 * \param _pfDst The destination buffer into which to resample texels.
	 * \param _pfSrc The source buffer of texels.
	 */
	void LSE_CALL CResampler::ResampleX( float * _pfDst, const float * _pfSrc ) const {
		const LSI_CONTRIB_EX * pceContrib = &m_pclContribsX->vList[0];
		const LSI_CONTRIB * pcP;
		float fTotal;
		for ( uint32_t I = m_ui32DstWidth; I--; pceContrib++ ) {
			pcP = pceContrib->pcValues;
			fTotal = 0.0f;
			for ( uint32_t J = pceContrib->ui32N; J--; ++pcP ) {
				fTotal += _pfSrc[pcP->ui16Value] * pcP->fWeight;
			}
			(*_pfDst++) = fTotal;
		}
	}

	/**
	 * Resample along the Y axis.
	 *
	 * \param _vDst The destination buffer into which to resample texels.
	 */
	void LSE_CALL CResampler::ResampleY( CVectorPoD<float, uint32_t> &_vDst ) const {
		const LSI_CONTRIB_EX * pceContrib = &m_pclContribsY->vList[m_ui32CurDstY];
		CVectorPoD<float, uint32_t> * pfTemp = m_bDelayX ? &m_vTempBuffer : &_vDst;

		// Process each contributor.
		for ( uint32_t I = 0; I < pceContrib->ui32N; ++I ) {
			// Find the contributor's location within the scan buffer.
			uint32_t J;
			for ( J = 0; J < LSI_MAX_SCANLINE; ++J ) {
				if ( m_psScanBuffer->i32ScanBufY[J] == pceContrib->pcValues[I].ui16Value ) { break; }
			}
			assert( J < LSI_MAX_SCANLINE );	// Must always be found. 
			if ( J == LSI_MAX_SCANLINE ) { return; }


			const float * pfSrc = m_psScanBuffer->pfScanBufL[J];

			if ( I == 0 ) {
				ScaleY( (*pfTemp), pfSrc, pceContrib->pcValues[I].fWeight, m_ui32IntermX );
			}
			else {
				ScaleYAdd( (*pfTemp), pfSrc, pceContrib->pcValues[I].fWeight, m_ui32IntermX );
			}


			if ( --m_vSrcCountY[pceContrib->pcValues[I].ui16Value] == 0 ) {
				// Line is no longer needed.  Mark it as free.
				m_vSrcFlagsY[pceContrib->pcValues[I].ui16Value] = false;

				m_psScanBuffer->i32ScanBufY[J] = -1;
			}
		}


		// Generate the destination line.

		if ( m_bDelayX ) {
			ResampleX( &_vDst[0], &(*pfTemp)[0] );
		}
		
		ClampSamples( &_vDst[0], _vDst.Length(), m_fLow, m_fHi );
	}

	/**
	 * Copy from one buffer to another, scaling each component.
	 *
	 * \param _pfDst The destination buffer.
	 * \param _pfSrc The source buffer.
	 * \param _fScale The scaling to apply to each copy.
	 * \param _ui32Total Number of elements to copy.
	 */
	void LSE_CALL CResampler::ScaleY( CVectorPoD<float, uint32_t> &_pfDst, const float * _pfSrc, float _fScale, uint32_t _ui32Total ) {
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			_pfDst[I] = (*_pfSrc++) * _fScale;
		}
	}

	/**
	 * Adds values from one buffer to another, scaling each component.
	 *
	 * \param _pfDst The destination buffer.
	 * \param _pfSrc The source buffer.
	 * \param _fScale The scaling to apply to each copy.
	 * \param _ui32Total Number of elements to copy.
	 */
	void LSE_CALL CResampler::ScaleYAdd( CVectorPoD<float, uint32_t> &_pfDst, const float * _pfSrc, float _fScale, uint32_t _ui32Total ) {
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			_pfDst[I] += (*_pfSrc++) * _fScale;
		}
	}

	/**
	 * Clamp all the samples into a certain range.
	 *
	 * \param _pfSamples The samples to clamp.
	 * \param _ui32Total The total number of samples to which _pfSamples points.
	 * \param _fLo The low clamp value.
	 * \param _fHi The high clamp value.
	 */
	void LSE_CALL CResampler::ClampSamples( float * _pfSamples, uint32_t _ui32Total, float _fLo, float _fHi ) {
		if ( _fLo < _fHi ) {
			for ( uint32_t I = _ui32Total; I--; ) {
				(*_pfSamples) = CStd::Clamp( (*_pfSamples), _fLo, _fHi );
				_pfSamples++;
			}
		}
	}

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
	CResampler::LSI_CONTRIB_LIST * CResampler::CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
		LSI_ADDRESS_MODE _amAddressMode,
		PfFilterFunc _pfFilter, float _fFilterSupport, float _fFilterScale ) {

		LSI_CONTRIB_LIST * pclRet = LSENEW LSI_CONTRIB_LIST;
		if ( !pclRet ) { return NULL; }
#define LSI_FAIL		LSEDELETE pclRet;
#ifdef _DEBUG
#define LSI_FAIL_MSG( STRING )			CStd::DebugPrintA( "CResampler::CreateContribList(): " STRING "\r\n" )
#else
#define LSI_FAIL_MSG( STRING )			
#endif	// #ifdef _DEBUG
		if ( !pclRet->vList.Resize( _ui32DstSize ) ) {
			LSI_FAIL;
			LSI_FAIL_MSG( "!pclRet->vList.Resize( _ui32DstSize )" );
			return NULL;
		}


		// The center is in discrete coordinates (0.0f = center).
		CVectorPoD<LSI_CONTRIB_BOUNDS, uint32_t> vBounds;
		if ( !vBounds.Resize( _ui32DstSize ) ) {
			LSI_FAIL;
			LSI_FAIL_MSG( "!vBounds.Resize( _ui32DstSize )" );
			return NULL;
		}

		
		LSI_CONTRIB * pcNextPool = NULL;


		// For fast division.
		const float fFilterScale = 1.0f / _fFilterScale;
		const float fNudge = 0.5f;

		float fScale = static_cast<float>(_ui32DstSize) / static_cast<float>(_ui32SrcSize);
		float fOrigScale = fScale;

		fScale = CStd::Min( fScale, 1.0f );
		//int32_t i32Total;


		float fHalfWidth = (_fFilterSupport / fScale) * fFilterScale;


		int32_t i32N = 0;
		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			// Convert from discrete to continuous coordinates, then back.
			vBounds[I].fCenter = (static_cast<float>(I) + fNudge) / fOrigScale;
			vBounds[I].fCenter -= fNudge;
			vBounds[I].i32Left = static_cast<int32_t>(::floorf( vBounds[I].fCenter - fHalfWidth ));
			vBounds[I].i32Right = static_cast<int32_t>(::ceilf( vBounds[I].fCenter + fHalfWidth ));

			i32N += vBounds[I].i32Right - vBounds[I].i32Left + 1;
		}

		if ( !i32N || !pclRet->vPool.Resize( static_cast<uint32_t>(i32N) ) ) {
			LSI_FAIL;
			LSI_FAIL_MSG( "!i32N || !pclRet->vPool.Resize( i32N )" );
			return NULL;
		}
		CStd::MemSet( &pclRet->vPool[0], 0x00, sizeof( pclRet->vPool[0] ) * pclRet->vPool.Length() );

		//i32Total = i32N;

		pcNextPool = &pclRet->vPool[0];


		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			int32_t i32MaxK = -1;
			float fMaxW = -1.0e20f;
			float fCenter = vBounds[I].fCenter;
			int32_t i32Left = vBounds[I].i32Left;
			int32_t i32Right = vBounds[I].i32Right;


			pclRet->vList[I].ui32N = 0;
			pclRet->vList[I].pcValues = pcNextPool;

			pcNextPool += (i32Right - i32Left + 1);

			float fTotalWeight = 0.0f;
			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				fTotalWeight += (*_pfFilter)( (fCenter - static_cast<float>(J)) * fScale * _fFilterScale );
			}

			const float fNorm = 1.0f / fTotalWeight;
			fTotalWeight = 0.0f;

			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				float fThisWeight = (*_pfFilter)( (fCenter - static_cast<float>(J)) * fScale * _fFilterScale ) * fNorm;
				if ( fThisWeight == 0.0f ) { continue; }


				i32N = ClampTexel( J, _ui32SrcSize, _amAddressMode );


				// Increase the number of source samples that contribute to the current destination sample.
				uint32_t ui32K = pclRet->vList[I].ui32N++;
				pclRet->vList[I].pcValues[ui32K].ui16Value = static_cast<uint16_t>(i32N);
				pclRet->vList[I].pcValues[ui32K].fWeight = fThisWeight;

				fTotalWeight += fThisWeight;

				if ( fThisWeight > fMaxW ) {
					fMaxW = fThisWeight;
					i32MaxK = static_cast<int32_t>(ui32K);
				}

			}

			if ( i32MaxK == -1 || pclRet->vList[I].ui32N == 0 ) {
				LSI_FAIL;
				LSI_FAIL_MSG( "i32MaxK == -1 || pclRet->vList[I].ui32N == 0" );
				return NULL;
			}


			if ( fTotalWeight != 1.0f && i32MaxK >= 0 ) {
				pclRet->vList[I].pcValues[i32MaxK].fWeight += 1.0f - fTotalWeight;
			}
		}

		return pclRet;
#undef LSI_FAIL_MSG
#undef LSI_FAIL
	}

	/**
	 * Count the operations in a contribution list.
	 *
	 * \param _clList The list whose operation count is to be obtained.
	 * \return Returns the number of operations in the contribution list.
	 */
	uint32_t LSE_CALL CResampler::CountOps( const LSI_CONTRIB_LIST &_clList ) {
		uint32_t ui32Total = 0;
		for ( uint32_t I = _clList.vList.Length(); I--; ) {
			ui32Total += _clList.vList[I].ui32N;
		}
		return ui32Total;
	}

	/**
	 * The point filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::PointFilterFunc( float _fT ) {
		return (_fT >= -LSM_HALF && _fT < LSM_HALF) ? 1.0f : 0.0f;
	}

	/**
	 * The tent/bilinear filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::BilinearFilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 1.0f ) { return 1.0f - _fT; }
		return 0.0f;
	}

	/**
	 * Quadratic filter function.
	 *
	 * \param _fT The value to filter.
	 * \param _fB Helper value.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::QuadraticFilterFunc( float _fT, float _fB ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 1.0f ) {
			float fTt = _fT * _fT;
			return _fT < 0.5f ? (-2.0f * _fB) * fTt + 0.5f * (_fB + 1.0f) :
				(_fB * fTt) + (-2.0f * _fB - 0.5f) * _fT + (3.0f / 4.0f) * (_fB + 1.0f);
		}
		return 0.0f;
	}

	/**
	 * Quadratic filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::QuadraticInterpFilterFunc( float _fT ) {
		return QuadraticFilterFunc( _fT, 1.0f );
	}

	/**
	 * The Kaiser filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::KaiserFilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 3.0f ) {
			static const float fAtt = 40.0f;
			static const double dAlpha = ::exp( ::log( 0.58417 * (fAtt - 20.96) ) * 0.4 ) + 0.07886 * (fAtt - 20.96);
			return static_cast<float>(Clean( SinC( _fT ) * KaiserHelper( dAlpha, 3.0, _fT ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 2 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos2FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 2.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 2.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 3 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos3FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 3.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 3.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 4 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos4FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 4.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 4.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 6 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos6FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 6.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 6.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 8 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos8FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 8.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 8.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 12 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos12FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 12.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 12.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Lanczos filter function with 64 samples.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::Lanczos64FilterFunc( float _fT ) {
		_fT = ::fabsf( _fT );
		if ( _fT < 64.0f ) {
			return static_cast<float>(Clean( SinC( _fT ) * SinC( _fT / 64.0 ) ));
		}
		return 0.0f;
	}

	/**
	 * The Mitchell filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::MitchellFilterFunc( float _fT ) {
		return MitchellFilterHelper( _fT, 1.0f / 3.0f, 1.0f / 3.0f );
	}

	/**
	 * The Catmull-Rom filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::CatmullRomFilterFunc( float _fT ) {
		return MitchellFilterHelper( _fT, 0.0f, 0.5f );
	}

	/**
	 * The B-Spline filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::BSplineFilterFunc( float _fT ) {
		return MitchellFilterHelper( _fT, 1.0f, 0.0f );
	}

	/**
	 * The Blackman filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::BlackmanFilterFunc( float _fT ) {
		if ( _fT < 0.0f ) { _fT = -_fT; }
		if ( _fT < 3.0f ) {
			return Clean( SinC( _fT ) * BlackmanWindow( _fT / 3.0 ) );
		}
		return 0.0f;
	}

	/**
	 * The Gaussian filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::GaussianFilterFunc( float _fT ) {
		if ( _fT < 0.0f ) { _fT = -_fT; }
		if ( _fT < 1.25f ) {
			return Clean( ::exp( -2.0 * _fT * _fT ) * ::sqrt( 2.0 / LSM_PI_DOUBLE ) * BlackmanWindow( _fT / 1.25f ) );
		}
		return 0.0f;
	}

	/**
	 * The Bell filter function.
	 *
	 * \param _fT The value to filter.
	 * \return Returns the filtered value.
	 */
	float LSE_CALL CResampler::BellFilterFunc( float _fT ) {
		if ( _fT < 0.0f ) { _fT = -_fT; }
		if ( _fT < 0.5f ) {
			return 0.75f - _fT * _fT;
		}
		if ( _fT < 1.5f ) {
			_fT = _fT - 1.5f;
			return 0.5f * _fT * _fT;
		}
		return 0.0f;
	}

}	// namespace lsi
