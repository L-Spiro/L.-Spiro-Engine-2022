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

#include "LSGDirectX9LosableResourceManager.h"

#ifdef LSG_DX9

namespace lsg {

	// == Members.
	/**
	 * List of resources.
	 */
	CVectorPoD<CDirectX9LosableResource *, uint32_t> CDirectX9LosableResourceManager::m_vResources;

	/**
	 * Unique resource ID.
	 */
	uint32_t CDirectX9LosableResourceManager::m_ui32ResId = 0;

	/**
	 * Thread safety.
	 */
	CCriticalSection CDirectX9LosableResourceManager::m_csCrit;

	// == Functions.
	/**
	 * Destroy the losable resource manager.  Should be called when shutting down.
	 */
	void LSE_CALL CDirectX9LosableResourceManager::Destroy() {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		assert( m_vResources.Length() == 0 );
		m_vResources.Reset();
	}

	/**
	 * Register a resource (also gives the resource a unique ID).
	 *
	 * \param _plrRes The resource to register.  Losable resources call this on
	 *	themselves directly, so this function should never be called by the user.
	 * \return Returns false if a memory error occurred.  If false is returned, the
	 *	engine must shut down.
	 */
	LSBOOL LSE_CALL CDirectX9LosableResourceManager::RegisterRes( CDirectX9LosableResource * _plrRes ) {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		if ( m_vResources.Push( _plrRes ) ) {
			_plrRes->m_ui32UniqueLosableResourceId = ++m_ui32ResId;
			return true;
		}
		return false;
	}

	/**
	 * Removes a resource by its ID.
	 *
	 * \param _ui32Id Unique ID of the resource to remove from the list.
	 */
	void LSE_CALL CDirectX9LosableResourceManager::RemoveRes( uint32_t _ui32Id ) {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		for ( uint32_t I = m_vResources.Length(); I--; ) {
			if ( m_vResources[I]->m_ui32UniqueLosableResourceId == _ui32Id ) {
				m_vResources.RemoveNoDealloc( I );
			}
		}
		if ( m_vResources.Length() == 0 ) { m_vResources.Reset(); }
	}

	/**
	 * Notify all objects that the device has been lost.
	 */
	void LSE_CALL CDirectX9LosableResourceManager::OnLostDevice() {
		CCriticalSection::CLocker lLockMe( m_csCrit );

		for ( uint32_t I = m_vResources.Length(); I--; ) {
			m_vResources[I]->OnDeviceLost();
		}
	}

	/**
	 * Notify all objects that the device has been reset.
	 */
	void LSE_CALL CDirectX9LosableResourceManager::OnResetDevice() {
		CCriticalSection::CLocker lLockMe( m_csCrit );

		for ( uint32_t I = m_vResources.Length(); I--; ) {
			m_vResources[I]->OnDeviceReset();
		}
	}

	/**
	 * Gets the number of losable resources currently registered.
	 *
	 * \return Returns the number of registered losable resources.
	 */
	uint32_t LSE_CALL CDirectX9LosableResourceManager::GetLosableResCount() {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		return m_vResources.Length();
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
