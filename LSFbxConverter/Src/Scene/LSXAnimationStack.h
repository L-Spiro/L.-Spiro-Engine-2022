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
 * Description: A stack of animation layers.
 */


#ifndef __LSX_ANIMATIONSTACK_H__
#define __LSX_ANIMATIONSTACK_H__

#include "../LSXFbxConverter.h"
#include "../Misc/LSXMisc.h"
#include "LSXAnimationLayer.h"
#include "Vector/LSTLVector.h"

namespace lsx {

	/**
	 * Class CAnimationStack
	 * \brief A stack of animation layers.
	 *
	 * Description: A stack of animation layers.
	 */
	class CAnimationStack {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CAnimationStack();


		// == Functions.
		/**
		 * Loads animation-stack information from an FBX animation stack.
		 *
		 * \param _pfasStack The stack from which to load information.
		 * \param _pfnNode The root node of the scene.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL							LoadStack( FbxAnimStack * _pfasStack, FbxNode * _pfnNode );

		/**
		 * Gets a constant reference to the layers in this stack.
		 *
		 * \return Returns a constant reference to the layers in this stack.
		 */
		LSE_INLINE const CVector<CAnimationLayer> & LSE_CALL
												GetStack() const;

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
		 * Array of layers for this animation stack.
		 */
		CVector<CAnimationLayer>				m_vLayers;

		/**
		 * Name of the stack.
		 */
		CString									m_sName;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a constant reference to the layers in this stack.
	 *
	 * \return Returns a constant reference to the layers in this stack.
	 */
	LSE_INLINE const CVector<CAnimationLayer> & LSE_CALL CAnimationStack::GetStack() const {
		return m_vLayers;
	}

}	// namespace lsx

#endif	// __LSX_ANIMATIONSTACK_H__
