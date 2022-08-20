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
 * Description: A texture manager for the model library.  Using image CRC pairs and a key value, it avoid redundant
 *	unintentional texture loads.  The key can be used to force the same texture to be stored twice, but it will
 *	usually be for the purpose of sRGB vs. linear textures, although the case in which the same texture is used in
 *	both formats is very rare.
 */


#ifndef __LSD_MODELTEXTUREMANAGER_H__
#define __LSD_MODELTEXTUREMANAGER_H__

#include "../LSDModelLib.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "Map/LSTLLinearMap.h"
#include "Texture/LSGTexture2d.h"

namespace lsd {

	/**
	 * Class CModelTextureManager
	 * \brief A texture manager for the model library.
	 *
	 * Description: A texture manager for the model library.  Using image CRC pairs and a key value, it avoid redundant
	 *	unintentional texture loads.  The key can be used to force the same texture to be stored twice, but it will
	 *	usually be for the purpose of sRGB vs. linear textures, although the case in which the same texture is used in
	 *	both formats is very rare.
	 */
	class CModelTextureManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CModelTextureManager();
		LSE_CALLCTOR							~CModelTextureManager();


		// == Functions.
		/**
		 * Returns a shared pointer to a texture given a CRC pair, a flag, and a reference to a CImage.
		 *
		 * \param _ui32Crc The primary CRC.
		 * \param _ui32Crc2 The secondary CRC.
		 * \param _ui32Flag The custom flag for forcing duplicates of the same texture image.
		 * \param _iImage The image to be used for creating the texture if it does not already exist.
		 * \return Returns a shared pointer to an texture if it already exists, or uses _iImage to create
		 *	a new shared texture which is then returned.
		 */
		CSharedTexture2dPtr LSE_CALL			GetTexture( uint32_t _ui32Crc, uint32_t _ui32Crc2,
			uint32_t _ui32Flag, const CImage &_iImage );

		/**
		 * Clears all unreferenced textures.
		 */
		void LSE_CALL							ClearUnused();


	protected :
		// == Types.
		/** The map key for a 2D texture. */
		typedef struct LSD_TEXTURE2D {
			uint32_t							ui32Crc;						/**< The primary CRC. */
			uint32_t							ui32Crc2;						/**< The secondary CRC. */
			uint32_t							ui32Flag;						/**< The flag. */


			// == Operators.
			/**
			 * Less-than operator.
			 *
			 * \param _t2dOther Object against which to compare.
			 * \return Returns true if this object is less than _t2dOther based off the CRC and flag values.
			 */
			LSE_INLINE LSBOOL LSE_FCALL			operator < ( const LSD_TEXTURE2D &_t2dOther ) const {
				return ui32Crc < _t2dOther.ui32Crc ||
					ui32Crc2 < _t2dOther.ui32Crc2 ||
					ui32Flag < _t2dOther.ui32Flag;
			}

			/**
			 * Equality operator.
			 *
			 * \param _t2dOther Object against which to compare.
			 * \return Returns true if this object is equal to _t2dOther based off the CRC and flag values.
			 */
			LSE_INLINE LSBOOL LSE_FCALL			operator == ( const LSD_TEXTURE2D &_t2dOther ) const {
				return ui32Crc == _t2dOther.ui32Crc &&
					ui32Crc2 == _t2dOther.ui32Crc2 &&
					ui32Flag == _t2dOther.ui32Flag;
			}
		} * LPLSD_TEXTURE2D, * const LPCLSD_TEXTURE2D;


		// == Members.
		/** Linear map of 2D textures. */
		CLinearMap<LSD_TEXTURE2D, CSharedTexture2dPtr>
												m_lmTexture2dMap;

		/** Critical section. */
		CCriticalSection						m_csCrit;
		
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsd

#endif	// __LSD_MODELTEXTUREMANAGER_H__
