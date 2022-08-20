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
 * Description: A standard 4D vector of type LSREAL.
 */


#ifndef __LSM_VECTOR4_H__
#define __LSM_VECTOR4_H__

#include "../LSMMathLib.h"
#include "LSMVector3.h"
#include "LSMVector4Base.h"

namespace lsm {

	/**
	 * Class CVector4
	 * \brief A standard 4D vector of type LSREAL.
	 *
	 * Description: A standard 4D vector of type LSREAL.
	 */
	typedef LSE_ALIGN( 16 ) CVector4Base<LSREAL, CVector3>			CVector4 LSE_POSTALIGN( 16 );

}	// namespace lsm

#endif	// __LSM_VECTOR4_H__

