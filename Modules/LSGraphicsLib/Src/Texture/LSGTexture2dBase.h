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


#ifndef __LSG_TEXTUREBASE_H__
#define __LSG_TEXTUREBASE_H__

#include "../LSGGraphicsLib.h"
#include "../RenderTarget/LSGRenderTargetInterface.h"
#include "Image/LSIImage.h"

namespace lsg {

	/**
	 * Class CTexture2dBase
	 * \brief The base class for all 2D textures.
	 *
	 * Description: The base class for all 2D textures.
	 */
	class CTexture2dBase : public CRenderTargetInterface {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CTexture2dBase();
		LSE_CALLCTOR							~CTexture2dBase();


		// == Functions.
		/**
		 * Resets everything to scratch.
		 */
		void LSE_CALL							Reset();


	protected :
		// == Members.
		/** Number of mipmaps. */
		uint16_t								m_ui16MipMaps;

		/** Number of slices. */
		uint16_t								m_ui16ArraySize;

		/** Format. */
		uint32_t								m_ui32Format;

		/** Multisample count. */
		uint16_t								m_ui16MultiSampleCount;

		/** Multisample quality. */
		uint16_t								m_ui16MultiSampleQuality;

		/** Usage flags. */
		uint32_t								m_ui32Usage;

		/** sRGB? */
		uint32_t								m_bSRgb;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// __LSG_TEXTUREBASE_H__
