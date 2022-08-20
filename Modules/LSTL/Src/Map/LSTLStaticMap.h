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
 *
 * This class uses no allocations and is better for performance, however the maximum number of elements must be
 *	known in advance.
 */


#ifndef __LSTL_STATICMAP_H__
#define __LSTL_STATICMAP_H__

#include "../LSTLib.h"
#include "../Vector/LSTLStaticVector.h"
#include "LSTLMapTemplateBase.h"

namespace lstl {

	/**
	 * Class CStaticMap
	 * \brief A container associating uniqe keys with mapped values.
	 *
	 * Description: Description: An associative container pairing a unique key with a mapped value.  The key and value do not
	 *	need to be of the same type.
	 *
	 * Values are accessed via their keys, so each key is unique.  Mapped values do not need to be unique.  Mapped
	 *	values are sorted, and the key type must implement the == and < operators.
	 *
	 * This class uses no allocations and is better for performance, however the maximum number of elements must be
	 *	known in advance.
	 */
	template <typename _tKeyType, typename _tMappedType, unsigned _uMaxElements>
	class CStaticMap : public CMapTemplateBase<_tKeyType, _tMappedType, CStaticVector<LSTL_PAIR<_tKeyType, _tMappedType>, _uMaxElements> > {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CStaticMap() {
		}
		explicit LSE_CALLCTOR					CStaticMap( const CStaticMap<_tKeyType, _tMappedType, _uMaxElements> &_mSrc ) :
			Parent( _mSrc.m_vPairs ) {
		}
		LSE_CALLCTOR							~CStaticMap() {
			Parent::Reset();
		}



	protected :
		// == Members.

	private :
		typedef CMapTemplateBase<_tKeyType, _tMappedType, CStaticVector<LSTL_PAIR<_tKeyType, _tMappedType>, _uMaxElements> >
												Parent;
	};

}	// namespace lstl

#endif	// __LSTL_STATICMAP_H__
