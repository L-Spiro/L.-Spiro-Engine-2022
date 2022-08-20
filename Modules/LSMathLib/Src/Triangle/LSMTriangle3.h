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
 * Description: A 3D triangle with no additional data.  It is just 3 points assumed to be in counter-clockwise
 *	order.
 */


#ifndef __LSM_TRIANGLE3_H__
#define __LSM_TRIANGLE3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "LSMTriangle3Base.h"

namespace lsm {

	/** A 3D triangle. */
	typedef CTriangle3Base<LSREAL, CVector3>			CTriangle3;

}	// namespace lsm

#endif	// __LSM_TRIANGLE3_H__
