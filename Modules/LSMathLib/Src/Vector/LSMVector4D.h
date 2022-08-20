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
 * Description: A standard 4D vector of type double.
 */


#ifndef __LSM_VECTOR4D_H__
#define __LSM_VECTOR4D_H__

#include "../LSMMathLib.h"
#include "LSMVector3D.h"
#include "LSMVector4Base.h"

namespace lsm {

	/**
	 * Class CVector4D
	 * \brief A standard 4D vector of type double.
	 *
	 * Description: A standard 4D vector of type double.
	 */
	typedef CVector4Base<double, CVector3D>			CVector4D;

}	// namespace lsm

#endif	// __LSM_VECTOR4D_H__

