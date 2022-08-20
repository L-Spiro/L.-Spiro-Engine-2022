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
 * Description: An associative container pairing a unique key with a mapped value.  The key and value do not
 *	need to be of the same type.
 *
 * Values are accessed via their keys, so each key is unique.  Mapped values do not need to be unique.  Mapped
 *	values are sorted, and the key type must implement the == and < operators.
 */


#ifndef __LSTL_LINEARMAP_H__
#define __LSTL_LINEARMAP_H__

#include "../LSTLib.h"
#include "LSTLMapTemplateBase.h"

namespace lstl {

	/**
	 * Class CLinearMap
	 * \brief A container associating uniqe keys with mapped values.
	 *
	 * Description: Description: An associative container pairing a unique key with a mapped value.  The key and value do not
	 *	need to be of the same type.
	 *
	 * Values are accessed via their keys, so each key is unique.  Mapped values do not need to be unique.  Mapped
	 *	values are sorted, and the key type must implement the == and < operators.
	 */
	template <typename _tKeyType, typename _tMappedType, unsigned _uSize = 4096>
	class CLinearMap : public CMapTemplateBase<_tKeyType, _tMappedType, CVector<LSTL_PAIR<_tKeyType, _tMappedType>, uint32_t, _uSize> > {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CLinearMap() {
		}
		explicit LSE_CALLCTOR					CLinearMap( const CLinearMap<_tKeyType, _tMappedType, _uSize> &_mSrc ) :
			Parent( _mSrc.m_vPairs ) {
		}
		explicit LSE_CALLCTOR					CLinearMap( CAllocator * _paAllocator ) :
			Parent::m_vPairs( _paAllocator ) {
		}
		LSE_CALLCTOR							~CLinearMap() {
			Parent::Reset();
		}


		// == Functions.
		/**
		 * Sets the allocator.  Causes the object to be fully reset.  Should not be used on
		 *	lists that have contents already.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator for map objects.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			Parent::m_vPairs.SetAllocator( _paAllocator );
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns a pointer to the allocator used by this object.
		 */
		CAllocator * LSE_CALL					GetAllocator() {
			return Parent::m_vPairs.GetAllocator();
		}

	protected :
		// == Members.

	private :
		typedef CMapTemplateBase<_tKeyType, _tMappedType, CVector<LSTL_PAIR<_tKeyType, _tMappedType>, uint32_t, _uSize> >
												Parent;
	};

}	// namespace lstl

#endif	// __LSTL_LINEARMAP_H__
