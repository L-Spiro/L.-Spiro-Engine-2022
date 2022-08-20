/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A kernel for images.
 */

#include "LSIKernel.h"


namespace lsi {

	// == Various constructors.
	LSE_CALLCTOR CKernel::CKernel() :
		m_pfKernel( NULL ),
		m_ui32Size( 0 ) {
	}
	LSE_CALLCTOR CKernel::CKernel( const CKernel &_kOther ) :
		m_pfKernel( NULL ),
		m_ui32Size( 0 ) {
		if ( SetSize( _kOther.Size() ) ) {
			CStd::MemCpy( m_pfKernel, _kOther.m_pfKernel, Size() * Size() * sizeof( float ) );
		}
	}
	LSE_CALLCTOR CKernel::~CKernel() {
		LSEDELETE [] m_pfKernel;
		m_pfKernel = NULL;
	}

	// == Functions.
	/**
	 * Normalizes the kernel.
	 */
	LSE_INLINE void LSE_CALL CKernel::Normalize() {
		float fSum = 0.0f;
		for ( uint32_t I = Size() * Size(); I--; ) {
			fSum += ::fabsf( m_pfKernel[I] );
		}
		float fDiv = 1.0f / fSum;
		for ( uint32_t I = Size() * Size(); I--; ) {
			m_pfKernel[I] *= fDiv;
		}
	}

	/**
	 * Creates a 3-by-3 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	LSBOOL LSE_CALL CKernel::CreateSobel3x3() {
		if ( !SetSize( 3 ) ) { return false; }
		static const float fTable[3][3] = {
			-1.0f, 0.0f, 1.0f,
			-2.0f, 0.0f, 2.0f,
			-1.0f, 0.0f, 1.0f,
		};
		CStd::MemCpy( m_pfKernel, fTable, sizeof( fTable ) );
		return true;
	}

	/**
	 * Creates a 5-by-5 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	LSBOOL LSE_CALL CKernel::CreateSobel5x5() {
		if ( !SetSize( 5 ) ) { return false; }
		static const float fTable[5][5] = {
			-1.0f, -2.0f, 0.0f, 2.0f, 1.0f,
            -2.0f, -3.0f, 0.0f, 3.0f, 2.0f,
            -3.0f, -4.0f, 0.0f, 4.0f, 3.0f,
            -2.0f, -3.0f, 0.0f, 3.0f, 2.0f,
            -1.0f, -2.0f, 0.0f, 2.0f, 1.0f,
		};
		CStd::MemCpy( m_pfKernel, fTable, sizeof( fTable ) );
		return true;
	}

	/**
	 * Creates a 7-by-7 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	LSBOOL LSE_CALL CKernel::CreateSobel7x7() {
		if ( !SetSize( 7 ) ) { return false; }
		static const float fTable[7][7] = {
			-1.0f, -2.0f, -3.0f, 0.0f, 3.0f, 2.0f, 1.0f,
            -2.0f, -3.0f, -4.0f, 0.0f, 4.0f, 3.0f, 2.0f,
            -3.0f, -4.0f, -5.0f, 0.0f, 5.0f, 4.0f, 3.0f,
            -4.0f, -5.0f, -6.0f, 0.0f, 6.0f, 5.0f, 4.0f,
            -3.0f, -4.0f, -5.0f, 0.0f, 5.0f, 4.0f, 3.0f,
            -2.0f, -3.0f, -4.0f, 0.0f, 4.0f, 3.0f, 2.0f,
            -1.0f, -2.0f, -3.0f, 0.0f, 3.0f, 2.0f, 1.0f,
		};
		CStd::MemCpy( m_pfKernel, fTable, sizeof( fTable ) );
		return true;
	}

	/**
	 * Creates a 9-by-9 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	LSBOOL LSE_CALL CKernel::CreateSobel9x9() {
		if ( !SetSize( 9 ) ) { return false; }
		static const float fTable[9][9] = {
			-1.0f, -2.0f, -3.0f, -4.0f, 0.0f, 4.0f, 3.0f, 2.0f, 1.0f,
            -2.0f, -3.0f, -4.0f, -5.0f, 0.0f, 5.0f, 4.0f, 3.0f, 2.0f,
            -3.0f, -4.0f, -5.0f, -6.0f, 0.0f, 6.0f, 5.0f, 4.0f, 3.0f,
            -4.0f, -5.0f, -6.0f, -7.0f, 0.0f, 7.0f, 6.0f, 5.0f, 4.0f,
            -5.0f, -6.0f, -7.0f, -8.0f, 0.0f, 8.0f, 7.0f, 6.0f, 5.0f,
            -4.0f, -5.0f, -6.0f, -7.0f, 0.0f, 7.0f, 6.0f, 5.0f, 4.0f,
            -3.0f, -4.0f, -5.0f, -6.0f, 0.0f, 6.0f, 5.0f, 4.0f, 3.0f,
            -2.0f, -3.0f, -4.0f, -5.0f, 0.0f, 5.0f, 4.0f, 3.0f, 2.0f,
            -1.0f, -2.0f, -3.0f, -4.0f, 0.0f, 4.0f, 3.0f, 2.0f, 1.0f,
		};
		CStd::MemCpy( m_pfKernel, fTable, sizeof( fTable ) );
		return true;
	}

}	// namespace lsi
