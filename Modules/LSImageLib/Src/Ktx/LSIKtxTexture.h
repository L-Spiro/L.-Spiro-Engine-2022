/**
 * Copyright L. Spiro 2021
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Handles loading of .KTX files.
 */


#ifndef __LSI_KTXTEXTURE_H__
#define __LSI_KTXTEXTURE_H__

#define LSI_USE_KHRONOS

#ifdef LSI_USE_KHRONOS

#include "../KTX-Software/include/ktx.h"
//#include "../KTX-Software/include/ktxvulkan.h"

namespace lsi {
	/**
	 * Class CKtxTexture
	 * \brief Handles loading of version-1 .KTX files.
	 *
	 * Description: Handles loading of version-1 .KTX files.  Wraps ktxTexture1 and ktxTexture2 objects for RAII purposes.
	 */
	template <typename _tT>
	class CKtxTexture {
	public :
		CKtxTexture() :
			m_ptHandle( nullptr ) {
		}
		~CKtxTexture() {
			if ( m_ptHandle ) {
				ktxTexture_Destroy( ktxTexture( m_ptHandle ) );
				m_ptHandle = nullptr;
			}
		}


		// == Functions.
		/**
		 * Gets a pointer to the handle pointer for use with functions such as ::ktxTexture1_CreateFromMemory().
		 *
		 * \return Returns a pointer to the handle pointer for use with functions such as ::ktxTexture1_CreateFromMemory().
		 */
		LSE_INLINE _tT **				HandlePointer() { return &m_ptHandle; }

		/**
		 * Gets the texture handle pointer.
		 *
		 * \return Returns the texture handle pointer.
		 */
		LSE_INLINE const _tT *			Handle() const { return m_ptHandle; }

		/**
		 * Returns this object as a pointer to the handle for easier dereferencing.
		 *
		 * \return Returns this object as a pointer to the handle for easier dereferencing.
		 */
		LSE_INLINE						operator _tT * () const { return m_ptHandle; }


	private :
		// == Members.
		/** The texture object pointer. */
		_tT *							m_ptHandle;
	};

}	// namespace lsi

#endif	// LSI_USE_KHRONOS

#endif	// __LSI_KTXTEXTURE_H__
