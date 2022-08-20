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
 * Description: A 2D texture.
 */

#include "LSGTexture2d.h"
#include "../Gfx/LSGGfx.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CTexture2d::CTexture2d() {
	}
	LSE_CALLCTOR CTexture2d::~CTexture2d() {
	}

	// == Functions.
	/**
	 * Creates a texture and fills it with our texel data.  Mipmaps are generated if necessary.
	 *
	 * \param _piImages Array of images to use to create the texture.
	 * \param _ui32Total Number of images to which _piImages points.
	 * \param _ui32Usage Usage type.
	 * \param _ui32MultiSampleCount Multisample count.
	 * \param _ui32MultiSampleQuality Multisample quality.
	 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
	 */
	LSBOOL LSE_CALL CTexture2d::CreateTexture( const CImage * _piImages, uint32_t _ui32Total,
		uint32_t _ui32Usage, uint32_t _ui32MultiSampleCount, uint32_t _ui32MultiSampleQuality ) {
		if ( !_ui32Total || !_piImages ) { return false; }
		if ( _ui32Total > CGfx::GetMetrics().ui32MaxTextureArray ) { return false; }
		if ( !_piImages[0].GetWidth() || !_piImages[0].GetHeight() ) { return false; }

		m_ui16ArraySize = static_cast<uint16_t>(_ui32Total);
		m_ui16MipMaps = static_cast<uint16_t>(_piImages->TotalMipLevels());
		m_ui32Format = _piImages->GetFormat();
		m_ui32Width = _piImages->GetWidth();
		m_ui32Height = _piImages->GetHeight();
		m_ui32Usage = _ui32Usage;
		m_ui16MultiSampleCount = static_cast<uint16_t>(_ui32MultiSampleCount);
		m_ui16MultiSampleQuality = static_cast<uint16_t>(_ui32MultiSampleQuality);
		m_bSRgb = _piImages->IsSRgb();
		if ( !CreateApiTexture( _piImages, _ui32Total ) ) {
			Reset();
			return false;
		}
		return true;
	}

	/**
	 * Activates the texture in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
	 * \return Returns true if the texture was activated in the given slot.
	 */
	LSBOOL LSE_CALL CTexture2d::Activate( uint32_t _ui32Slot ) {
		if ( _ui32Slot >= LSG_MAX_TEXTURE_UNITS ) { return false; }	// Texture unit not available.
		if ( !ShaderResourceApi() ) { return false; }				// No texture created.
		CGfx::RenderState().ui32TextureId[_ui32Slot] = m_ui32Id;
		CGfx::RenderState().psrShaderResources[_ui32Slot] = ShaderResourceApi();
		CGfx::RenderState().ui32MaxTextureSet = CStd::Max( CGfx::RenderState().ui32MaxTextureSet, _ui32Slot + 1 );
		return Parent::ActivateApi( _ui32Slot );
	}

}	// namespace lsg
