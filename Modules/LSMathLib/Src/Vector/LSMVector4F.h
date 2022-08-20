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
 * Description: A standard 4D vector of type float.
 */


#ifndef __LSM_VECTOR4F_H__
#define __LSM_VECTOR4F_H__

#include "../LSMMathLib.h"
#include "LSMVector3F.h"
#include "LSMVector4Base.h"

namespace lsm {

	/**
	 * Class CVector4F
	 * \brief A standard 4D vector of type float.
	 *
	 * Description: A standard 4D vector of type float.
	 */
	typedef LSE_ALIGN( 16 ) CVector4Base<float, CVector3F>			CVector4F LSE_POSTALIGN( 16 );

}	// namespace lsm

#endif	// __LSM_VECTOR4F_H__

