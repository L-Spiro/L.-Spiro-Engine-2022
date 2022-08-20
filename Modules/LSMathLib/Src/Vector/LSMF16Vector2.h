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
 * Description: A standard 2D vector of type CFloat16.
 */


#ifndef __LSM_F16VECTOR2_H__
#define __LSM_F16VECTOR2_H__

#include "../LSMMathLib.h"
#include "../Float16/LSMFloat16.h"
#include "LSMVector2Base.h"

namespace lsm {

	/**
	 * Class CF16Vector2
	 * \brief A standard 2D vector of type CFloat16.
	 *
	 * Description: A standard 2D vector of type CFloat16.
	 */
	class CF16Vector2 : public CVector2Base<CFloat16, CF16Vector2> {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CF16Vector2();
		LSE_INLINE LSE_CALLCTOR							CF16Vector2( LSREAL _fX, LSREAL _fY );
		LSE_INLINE LSE_CALLCTOR							CF16Vector2( const CF16Vector2 &_vOther );
		LSE_INLINE LSE_CALLCTOR							CF16Vector2( LSREAL * _pfValues );
	

	private :
		typedef CVector2Base<CFloat16, CF16Vector2>		Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CF16Vector2::CF16Vector2() {
	}
	LSE_INLINE LSE_CALLCTOR CF16Vector2::CF16Vector2( LSREAL _fX, LSREAL _fY ) :
		Parent( static_cast<CFloat16>(_fX), static_cast<CFloat16>(_fY) ) {
	}
	LSE_INLINE LSE_CALLCTOR CF16Vector2::CF16Vector2( const CF16Vector2 &_vOther ) :
		Parent( _vOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CF16Vector2::CF16Vector2( LSREAL * _pfValues ) :
		Parent( _pfValues ) {
	}

}	// namespace lsm

#endif	// __LSM_F16VECTOR2_H__
