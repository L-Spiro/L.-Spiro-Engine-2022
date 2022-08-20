/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A part of a texture, consisting of a texture pointer and a part of that texture (rectangular).
 *	Origin is the upper-left corner of the texture.
 */

#include "LSRTextureRect.h"


namespace lsr {

	// == Various constructors.
	LSE_CALLCTOR CTextureRect::CTextureRect() :
		m_ui32Left( 0 ),
		m_ui32Top( 0 ),
		m_ui32Width( 0 ),
		m_ui32Height( 0 ),
		m_vDimensions( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ) {
	}

	LSE_CALLCTOR CTextureRect::CTextureRect( const CSharedTexture2dPtr &_st2Texture,
		uint32_t _ui32Left, uint32_t _ui32Top, uint32_t _ui32Width, uint32_t _ui32Height ) :
		m_st2SrcTexture( _st2Texture ),
		m_ui32Left( _ui32Left ),
		m_ui32Top( _ui32Top ),
		m_ui32Width( _ui32Width ),
		m_ui32Height( _ui32Height ) {
		UpdateInternal();
	}

}	// namespace lsr
