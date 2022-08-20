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
 * Description: Manages objects whose DirectX 9.0 resources can be lost (when the device is lost, for example).
 *	Every losable resource registers itself with this class automatically (using a small circular dependency).
 *	This class can then inform those resources when the device is lost and regained, allowing them to re-register
 *	their resources.
 */


#ifndef __LSG_DIRECTX9LOSABLERESOURCEMANAGER_H__
#define __LSG_DIRECTX9LOSABLERESOURCEMANAGER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9

#include "CriticalSection/LSHCriticalSection.h"
#include "LSGDirectX9LosableResource.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsg {

	/**
	 * Class CDirectX9LosableResourceManager
	 * \brief Manages losable resources, notifying them when the device is lost and regained.
	 *
	 * Description: Manages objects whose DirectX 9.0 resources can be lost (when the device is lost, for example).
	 *	Every losable resource registers itself with this class automatically (using a small circular dependency).
	 *	This class can then inform those resources when the device is lost and regained, allowing them to re-register
	 *	their resources.
	 */
	class CDirectX9LosableResourceManager {
	public :
		// == Functions.
		/**
		 * Destroy the losable resource manager.  Should be called when shutting down.
		 */
		static void LSE_CALL									Destroy();

		/**
		 * Register a resource (also gives the resource a unique ID).
		 *
		 * \param _plrRes The resource to register.  Losable resources call this on
		 *	themselves directly, so this function should never be called by the user.
		 * \return Returns false if a memory error occurred.  If false is returned, the
		 *	engine must shut down.
		 */
		static LSBOOL LSE_CALL									RegisterRes( CDirectX9LosableResource * _plrRes );

		/**
		 * Removes a resource by its ID.
		 *
		 * \param _ui32Id Unique ID of the resource to remove from the list.
		 */
		static void LSE_CALL									RemoveRes( uint32_t _ui32Id );

		/**
		 * Notify all objects that the device has been lost.
		 */
		static void LSE_CALL									OnLostDevice();

		/**
		 * Notify all objects that the device has been reset.
		 */
		static void LSE_CALL									OnResetDevice();

		/**
		 * Gets the number of losable resources currently registered.
		 *
		 * \return Returns the number of registered losable resources.
		 */
		static uint32_t LSE_CALL								GetLosableResCount();


	protected :
		// == Members.
		/**
		 * List of resources.
		 */
		static CVectorPoD<CDirectX9LosableResource *, uint32_t>	m_vResources;

		/**
		 * Unique resource ID.
		 */
		static uint32_t											m_ui32ResId;

		/**
		 * Thread safety.
		 */
		static CCriticalSection									m_csCrit;
	};

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9LOSABLERESOURCEMANAGER_H__
