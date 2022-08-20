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
 * Description: A standard resource manager.  It requires a key and a resource type, and manages resources of
 *	the given type, ensuring redundant copies do not exist.  It is basically a wrapper around a CLinearMap.
 * The resource type must be in the form of a shared pointer.
 */


#ifndef __LSTL_RESOURCEMANCRTP_H__
#define __LSTL_RESOURCEMANCRTP_H__

#include "../LSTLib.h"
#include "../Map/LSTLLinearMap.h"
#include "../SharedPtr/LSTLSharedPtr.h"

namespace lstl {

	/**
	 * Class CResourceManCrtp
	 * \brief A standard resource manager.
	 *
	 * Description: A standard resource manager.  It requires a key and a resource type, and manages resources of
	 *	the given type, ensuring redundant copies do not exist.  It is basically a wrapper around a CLinearMap.
	 * The resource type must be in the form of a shared pointer.
	 */
	template <typename _tDerivedType,
		typename _tKeyType, typename _tMappedType>
	class CResourceManCrtp {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CResourceManCrtp() {
		}
		LSE_CALLCTOR							~CResourceManCrtp() {
		}


		// == Functions.
		/**
		 * Gets a shared pointer to a resource by a key.  If there was a failure to load,
		 *	the shared pointer will be invalid.
		 *
		 * \param _tKey Key of the resource to load.
		 * \return Returns a shared pointer to a resource by a key.  If there was a failure to load,
		 *	the shared pointer will be invalid.
		 */
		_tMappedType LSE_CALL					GetRes( const _tKeyType &_tKey ) {
			uint32_t ui32Index;
			if ( m_mList.ItemExists( _tKey, ui32Index ) ) {
				return m_mList.GetByIndex( ui32Index );
			}
			return static_cast<_tDerivedType *>(this)->AddRes( _tKey );
		}
		/**
		 * Releases all shared pointers held by this manager.
		 */
		void LSE_CALL							Release() {
			m_mList.Reset();
		}


	protected :
		// == Members.
		/**
		 * Map of shared pointers.
		 */
		CLinearMap<_tKeyType, _tMappedType>		m_mList;


		// == Functions.
		/**
		 * Adds a shared resource to the list, returns a shared pointer to it.
		 *
		 * \param _tKey Key of the resource to load.
		 * \return Returns a shared pointer to the loaded resource.  If there was a failure to load,
		 *	the shared pointer will be invalid.
		 */
		_tMappedType LSE_CALL					AddRes( const _tKeyType &/*_tKey*/ ) {
			_tMappedType tRet;
			return tRet;
		}
	};

}	// namespace lstl

#endif	// __LSTL_RESOURCEMANCRTP_H__
