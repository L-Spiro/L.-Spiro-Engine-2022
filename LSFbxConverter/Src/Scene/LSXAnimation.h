/**
 * Copyright L. Spiro 2012
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: The animations in the loaded scene.
 */


#ifndef __LSX_ANIMATION_H__
#define __LSX_ANIMATION_H__

#include "../LSXFbxConverter.h"
#include "../Misc/LSXMisc.h"
#include "LSXAnimationStack.h"
#include "LSXSkeleton.h"
#include "Vector/LSTLVector.h"

namespace lsx {

	/**
	 * Class CAnimation
	 * \brief The animations in the loaded scene.
	 *
	 * Description: The animations in the loaded scene.
	 */
	class CAnimation {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CAnimation();


		// == Functions.
		/**
		 * Loads the animation data from the given FBX scene.
		 *
		 * \param _pfsScene The scene from which to load the animation data.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL							LoadAnimations( FbxScene * _pfsScene );

		/**
		 * Save the data to a buffer.
		 *
		 * \param _bBuffer The buffer to which to save our data.
		 * \return Returns true only if all data is written to the buffer.
		 */
		LSBOOL LSE_CALL							SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer );


	protected :
		// == Members.
		/**
		 * Array of stacks for this animation.
		 */
		CVector<CAnimationStack>				m_vStacks;

		/**
		 * The bind pose for this animation.
		 */
		CSkeleton								m_sBindPose;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsx

#endif	// __LSX_ANIMATION_H__
