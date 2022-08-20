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
* Description: The Direct3D 11 implementation of a framebuffer.
*/


#ifndef __LSG_DIRECTX11FRAMEBUFFERBASE_H__
#define __LSG_DIRECTX11FRAMEBUFFERBASE_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../Framebuffer/LSGFramebufferBase.h"

namespace lsg {

	/**
	* Class CDirectX11FramebufferBase
	* \brief The Direct3D 11 implementation of a framebuffer.
	*
	* Description: The Direct3D 11 implementation of a framebuffer.
	*/
	class CDirectX11FramebufferBase : public CFramebufferBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX11FramebufferBase();
		LSE_CALLCTOR							~CDirectX11FramebufferBase();


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

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11FRAMEBUFFERBASE_H__
