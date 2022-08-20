/**
 * Copyright L. Spiro 2017
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A Metal render-pass descriptor.
 */


#ifndef __LSG_METALRENDERPASSDESCRIPTOR_H__
#define __LSG_METALRENDERPASSDESCRIPTOR_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_METAL

#include "../Metal/LSGMetal.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Load actions. */
	enum LSG_RENDER_PASS_LOAD_ACTIONS {
		LSG_RPLA_DONT_CARE						= 0,											/**< Allows the GPU to avoid loading existing texture contents. */
		LSG_RPLA_LOAD							= 1,											/**< Preserves the existing texture contents. */
		LSG_RPLA_CLEAR							= 2,											/**< Clears the previous texture contents. */
		LSG_RPLA_FORCE_DWORD					= 0x7FFFFFFF
	};
	
	/** Store actions. */
	enum LSG_RENDER_PASS_STORE_ACTIONS {
		LSG_RPSA_DONT_CARE						= 0,											/**< If the data is not needed after the render operation. */
		LSG_RPSA_STORE							= 1,											/**< Directly stores the texture result. */
		LSG_RPSA_MULTISAMPLE_RESOLVE			= 2,											/**< Multi-sample resolve. */
		LSG_RPSA_FORCE_DWORD					= 0x7FFFFFFF
	};

	/**
	 * Class CMetalRenderPassDescriptor
	 * \brief A Metal render-pass descriptor.
	 *
	 * Description: A Metal render-pass descriptor.
	 */
	class CMetalRenderPassDescriptor {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMetalRenderPassDescriptor();
		LSE_CALLCTOR							~CMetalRenderPassDescriptor();
		
		
	protected :
		// == Members.
		/** The MTLRenderPassDescriptor object. */
		const LSVOID *							m_pvObj;
	};

}	// namespace lsg


#endif	// #ifdef LSG_METAL

#endif	// __LSG_METALRENDERPASSDESCRIPTOR_H__