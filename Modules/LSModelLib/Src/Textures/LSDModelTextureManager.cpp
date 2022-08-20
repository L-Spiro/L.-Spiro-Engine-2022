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

#include "LSDModelTextureManager.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CModelTextureManager::CModelTextureManager() {
	}
	LSE_CALLCTOR CModelTextureManager::~CModelTextureManager() {
	}

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
	CSharedTexture2dPtr LSE_CALL CModelTextureManager::GetTexture( uint32_t _ui32Crc, uint32_t _ui32Crc2,
		uint32_t _ui32Flag, const CImage &_iImage ) {

		LSD_TEXTURE2D t2dTemp = {
			_ui32Crc,
			_ui32Crc2,
			_ui32Flag
		};
		uint32_t ui32Index;
		CCriticalSection::CLocker lLock( m_csCrit );
		if ( m_lmTexture2dMap.ItemExists( t2dTemp, ui32Index ) ) {
			return m_lmTexture2dMap.GetByIndex( ui32Index );
		}
		CSharedTexture2dPtr st2pPtr;
		if ( !st2pPtr.New() ) { return st2pPtr; }	// Blank texture pointer.

		// It created a new instance.  Try to create the texture data.
		if ( !st2pPtr->CreateTexture( &_iImage ) ) {
			// Return a blank texture.
			st2pPtr.Reset();
			return st2pPtr;
		}
		// It was created successfully.  Add and return it.
		if ( !m_lmTexture2dMap.Insert( t2dTemp, st2pPtr ) ) {
			st2pPtr.Reset();
			// Fall-through intentional.
		}
		return st2pPtr;
	}

	/**
	 * Clears all unreferenced textures.
	 */
	void LSE_CALL CModelTextureManager::ClearUnused() {
		CCriticalSection::CLocker lLock( m_csCrit );
		for ( uint32_t I = m_lmTexture2dMap.Length(); I--; ) {
			if ( m_lmTexture2dMap.GetByIndex( I ).RefCount() == 1 ) {
				// Only referenced by this manager.
				m_lmTexture2dMap.RemoveByIndex( I );
			}
		}
		if ( !m_lmTexture2dMap.Length() ) {
			m_lmTexture2dMap.Reset();
		}
	}

}	// namespace lsd
