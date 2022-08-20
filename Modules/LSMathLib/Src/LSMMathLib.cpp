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
 * Description: The base for our math functions.  This must be included when you want to use these math
 *	functions.  This library allows using a macro (LSM_PERFORMANCE) to change its performance in terms
 *	of speed/accuracy.  It also allows defining the base type used in all the structures/math (LSM_BASETYPE).
 *
 * Note that if LSM_PERFORMANCE <= 0 the size of the CVector3 class will change from 3 LSREALs to
 *	4 LSREALs.
 *
 * Aside from defining the base real type (LSREAL) it also defines a high-precision type (LSREALHP) for use
 *	when extra precision is required.  Use this only when speed is not a concern.  In the special cases where
 *	LSREALHP is used, it is also acceptable to use the LSM*HP math macros (LSMCOSHP, LSMSQRTHP, etc.)
 *	Always use LSREAL_TO_HP() and HP_TO_LSREAL() to cast between regular- and high- precision values!
 */

#include "LSMMathLib.h"


namespace lsm {

	// == Members.
	/**
	 * Are we initialized?
	 */
	LSBOOL		CMathLib::m_bInitialized = false;
#ifdef LSM_SINCOSTABLE
	LSREAL		CMathLib::m_fCosTable[LSM_TABLESIZE] = { LSM_ZERO };
	LSREAL		CMathLib::m_fSinTable[LSM_TABLESIZE] = { LSM_ZERO };
#endif	// #ifdef LSM_SINCOSTABLE

	// == Functions.
	/**
	 * Initializes the math library.  Can be called only once.  Multiple calls have no effect.  The math library
	 *	cannot be used until this is called.
	 */
	void LSE_FCALL CMathLib::Init() {
		if ( !m_bInitialized ) {

#ifdef LSM_SINCOSTABLE
			// Create a cos/sin look-up table.  Only used if LSM_PERFORMANCE <= 2.
			for ( int32_t I = LSM_TABLESIZE; --I >= 0; ) {
				LSREAL fCos = LSMCOS( (2.0 * LSM_PI_DOUBLE / LSM_TABLESIZE) * I );
				LSREAL fSin = LSMSIN( (2.0 * LSM_PI_DOUBLE / LSM_TABLESIZE) * I );
				m_fCosTable[I] = fCos;
				m_fSinTable[I] = fSin;
			}
#endif	// #ifdef LSM_SINCOSTABLE

			m_bInitialized = true;
		}
	}

}	// namespace lsm
