/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: The Metal implementation of a framebuffer.
 */


#ifndef __LSG_METALFRAMEBUFFERBASE_H__
#define __LSG_METALFRAMEBUFFERBASE_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_METAL

#include "../Framebuffer/LSGFramebufferBase.h"

namespace lsg {

	/**
	 * Class CMetalFramebufferBase
	 * \brief The Metal implementation of a framebuffer.
	 *
	 * Description: The Metal implementation of a framebuffer.
	 */
	class CMetalFramebufferBase : public CFramebufferBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMetalFramebufferBase();
		LSE_CALLCTOR							~CMetalFramebufferBase();


		// == Functions.
		/**
		 * Activates the framebuffer.  Applies all render targets.
		 *
		 * \return Returns true if the framebuffer was activated.
		 */
		LSBOOL LSE_CALL							Activate();


	protected :
		// == Members.
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// #ifdef LSG_METAL

#endif	// __LSG_METALFRAMEBUFFERBASE_H__
