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
 * Description: All objects that have to take action when the DirectX device is lost should inherit
 *	from this class.  This is for DirectX 9.0 only.
 */

#include "LSGDirectX9LosableResource.h"

#ifdef LSG_DX9

#include "LSGDirectX9LosableResourceManager.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9LosableResource::CDirectX9LosableResource() :
		m_bResourceCanBeLost( false ) {
		CDirectX9LosableResourceManager::RegisterRes( this );
	}
	LSE_CALLCTOR CDirectX9LosableResource::~CDirectX9LosableResource() {
		CDirectX9LosableResourceManager::RemoveRes( m_ui32UniqueLosableResourceId );
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
