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


#ifndef __LSG_DIRECTX9LOSABLERESOURCE_H__
#define __LSG_DIRECTX9LOSABLERESOURCE_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9

namespace lsg {

	/**
	 * Class CDirectX9LosableResource
	 * \brief Provides an interface for DirectX 9 objects that need to be reset when the device is lost.
	 *
	 * Description: All objects that have to take action when the DirectX device is lost should inherit
	 *	from this class.  This is for DirectX 9.0 only.
	 */
	class CDirectX9LosableResource {
		friend class					CDirectX9LosableResourceManager;
	public :
		// == Various constructors.
		LSE_CALLCTOR					CDirectX9LosableResource();
		LSE_CALLCTOR					~CDirectX9LosableResource();


		// == Functions.
		/**
		 * Must perform some action when the device is lost.
		 */
		virtual void LSE_CALL			OnDeviceLost() = 0;

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual LSBOOL LSE_CALL			OnDeviceReset() = 0;


	protected :
		// == Members.
		/**
		 * Do we need to reset the resource?
		 */
		LSBOOL							m_bResourceCanBeLost;

	private :
		/**
		 * An ID that lets us remove ourselves from the global resource list.
		 */
		uint32_t						m_ui32UniqueLosableResourceId;
	};

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9LOSABLERESOURCE_H__
