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
 * Description: The base class for all 2D textures.
 */

#include "LSGTexture2dBase.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CTexture2dBase::CTexture2dBase() :
		m_ui16MipMaps( 0 ),
		m_ui16ArraySize( 0 ),
		m_ui32Format( 0 ),
		m_ui16MultiSampleCount( 1 ),
		m_ui16MultiSampleQuality( 0 ),
		m_ui32Usage( 0 ),
		m_bSRgb( false ) {
	}
	LSE_CALLCTOR CTexture2dBase::~CTexture2dBase() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets everything to scratch.
	 */
	void LSE_CALL CTexture2dBase::Reset() {
		// Remove from being a render target or texture.
		// ...

		// Default base members.
		m_ui32Id = CAtomic::InterlockedAdd( m_aIdCounter, 1 );
		m_ui32Width = 0;
		m_ui32Height = 0;

		// Default values.
		m_ui16MipMaps = 0;
		m_ui16ArraySize = 0;
		m_ui32Format = 0;
		m_ui16MultiSampleCount = 1;
		m_ui16MultiSampleQuality = 0;
		m_ui32Usage = 0;
		m_bSRgb = false;
	}

}	// namespace lsg
