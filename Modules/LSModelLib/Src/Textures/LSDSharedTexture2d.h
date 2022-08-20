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
 * Description: DESCRIPTION.
 */


#ifndef __LSD_SHAREDTEXTURE2D_H__
#define __LSD_SHAREDTEXTURE2D_H__

#include "../LSDModelLib.h"
#include "Set/LSTLSet.h"
#include "Texture/LSGTexture2d.h"

namespace lsd {

	// == Types.
	/** A shared texture. */
	typedef struct LSD_SHARED_TEXTURE2D {
		uint32_t							ui32Id;							/**< Texture ID. */
		CSharedTexture2dPtr					st2pTexture;					/**< Shared texture pointer. */


		// == Operators.
		/**
		 * Less-than operator.
		 *
		 * \param _st2dOther Object against which to compare.
		 * \return Returns true if this object is less than _st2dOther based off the ID.
		 */
		LSE_INLINE LSBOOL LSE_FCALL			operator < ( const LSD_SHARED_TEXTURE2D &_st2dOther ) const;

		/**
		 * Equality operator.
		 *
		 * \param _st2dOther Object against which to compare.
		 * \return Returns true if this object is equal to _st2dOther based off the ID.
		 */
		LSE_INLINE LSBOOL LSE_FCALL			operator == ( const LSD_SHARED_TEXTURE2D &_st2dOther ) const;
	} * LPLSD_SHARED_TEXTURE2D, * const LPCLSD_SHARED_TEXTURE2D;


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** A shared 2D texture set. */
	typedef CSet<LSD_SHARED_TEXTURE2D>		LSD_SHARED_TEXTURE2D_SET;

	// == Operators.
	/**
	 * Less-than operator.
	 *
	 * \param _st2dOther Object against which to compare.
	 * \return Returns true if this object is less than _st2dOther based off the ID.
	 */
	LSE_INLINE LSBOOL LSE_FCALL LSD_SHARED_TEXTURE2D::operator < ( const LSD_SHARED_TEXTURE2D &_st2dOther ) const {
		return ui32Id < _st2dOther.ui32Id;
	}

	/**
	 * Equality operator.
	 *
	 * \param _st2dOther Object against which to compare.
	 * \return Returns true if this object is equal to _st2dOther based off the ID.
	 */
	LSE_INLINE LSBOOL LSE_FCALL LSD_SHARED_TEXTURE2D::operator == ( const LSD_SHARED_TEXTURE2D &_st2dOther ) const {
		return ui32Id == _st2dOther.ui32Id;
	}

}	// namespace lsd

#endif	// __LSD_SHAREDTEXTURE2D_H__
