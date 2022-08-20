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
 * Description: A specialization of the 2-by-2 matrix template using the standard LSREAL type.
 */


#ifndef __LSM_MATRIX2X2_H__
#define __LSM_MATRIX2X2_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector2.h"
#include "LSMMatrix2x2Base.h"

namespace lsm {

	/**
	 * Class CMatrix2x2
	 * \brief A specialization of the 2-by-2 matrix template using the standard LSREAL type.
	 *
	 * Description: A specialization of the 2-by-2 matrix template using the standard LSREAL type.
	 */
	typedef CMatrix2x2Base<LSREAL, CVector2>	CMatrix2x2;

}	// namespace lsm

#endif	// __LSM_MATRIX2X2_H__

