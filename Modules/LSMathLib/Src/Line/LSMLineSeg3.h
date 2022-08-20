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
 * Description: A 3D line segment stored as 2 points.
 */



#ifndef __LSM_LINESEG3_H__
#define __LSM_LINESEG3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "LSMLineSeg3Base.h"

namespace lsm {

	/**
	 * Class CLineSeg3
	 * \brief A 3D line segment and the math that loves it.
	 *
	 * Description: A 3D line segment and the math that loves it.
	 */
	class CLineSeg3 : public CLineSeg3Base<LSREAL, CVector3> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR CLineSeg3();
		LSE_INLINE LSE_CALLCTOR CLineSeg3( const CLineSeg3 &_ls3Line );
		LSE_INLINE LSE_CALLCTOR CLineSeg3( const CVector3 &_vStart, const CVector3 &_vEnd );

	private :
		typedef CLineSeg3Base<LSREAL, CVector3>
								Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CLineSeg3::CLineSeg3() {
	}
	LSE_INLINE LSE_CALLCTOR CLineSeg3::CLineSeg3( const CLineSeg3 &_ls3Line ) :
		Parent( _ls3Line ) {
	}
	LSE_INLINE LSE_CALLCTOR CLineSeg3::CLineSeg3( const CVector3 &_vStart, const CVector3 &_vEnd ) :
		Parent( _vStart, _vEnd ) {
	}

}	// namespace lsm

#endif	// __LSM_LINESEG3_H__
