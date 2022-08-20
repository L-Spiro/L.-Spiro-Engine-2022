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
 * Description: Constants used throughout the library.
 */



#ifndef __LSM_CONSTANTS_H__
#define __LSM_CONSTANTS_H__


// LSM_PI.
#define LSM_PI_DOUBLE		(3.1415926535897932384626433832795)
#define LSM_PI				(static_cast<LSREAL>(LSM_PI_DOUBLE))
#define LSM_RCPPI_DOUBLE	(0.31830988618379067153776752674503)
#define LSM_RCPPI			(static_cast<LSREAL>(LSM_RCPPI_DOUBLE))
#define LSM_HALFPI			(static_cast<LSREAL>(LSM_PI_DOUBLE * 0.5))
#define LSM_DOUBLEPI		(static_cast<LSREAL>(LSM_PI_DOUBLE * 2.0))
#define LSM_ONEFOURTHPI		(static_cast<LSREAL>(LSM_PI_DOUBLE * 0.25))
#define LSM_E				(static_cast<LSREAL>(2.7182818284590452353602874713527))
#define LSM_SQRT_TWO		(static_cast<LSREAL>(1.4142135623730950488016887242097))
#define LSM_SQRT_THREE		(static_cast<LSREAL>(1.7320508075688772935274463415059))
#define LSM_SQRT_1OVER2		(static_cast<LSREAL>(0.70710678118654752440084436210485))
#define LSM_SQRT_1OVER3		(static_cast<LSREAL>(0.57735026918962576450914878050196))
#define LSM_DEG2RAD			(static_cast<LSREAL>(LSM_PI_DOUBLE / 180.0))
#define LSM_RAD2DEG			(static_cast<LSREAL>(180.0 / LSM_PI_DOUBLE))
#define LSM_SEC2MS			(static_cast<LSREAL>(1000.0))
#define LSM_MS2SEC			(static_cast<LSREAL>(0.001))
#if LSM_BASETYPE == LSM_FIXED
#define LSM_INFINITY		(LSREAL::DigitMask())
#define LSM_REAL_EPSILON	LSREAL( LSREAL::CFixedRawType( 1 ) )
#else
#define LSM_INFINITY		(static_cast<LSREAL>(1e30))
#define LSM_REAL_EPSILON	(static_cast<LSREAL>(1.192092896e-07))
#endif	// LSM_BASETYPE == LSM_FIXED
#define LSM_FLT_EPSILON		1.192092896e-07f
#define LSM_DBL_EPSILON		2.2204460492503131e-016

#define LSM_MAT_EPSILON		LSM_REAL_EPSILON
#define LSM_TWO				(static_cast<LSREAL>(2.0))
#define LSM_ONE				(static_cast<LSREAL>(1.0))
#define LSM_HALF			(static_cast<LSREAL>(0.5))
#define LSM_FOURTH			(static_cast<LSREAL>(0.25))
#define LSM_ZERO			(static_cast<LSREAL>(0.0))
#define LSM_PLANE_THICKNESS	(static_cast<LSREAL>(0.00005))


#define LSM_FLT_LOOKUP_BITS	8
#define LSM_FLT_EXP_POS		23
#define LSM_FLT_EXP_BIAS	127

#define LSM_DBL_LOOKUP_BITS	11
#define LSM_DBL_EXP_POS		52
#define LSM_DBL_EXP_BIAS	1023

#define LSM_FLT_MAX			3.402823466e+38F
#define LSM_DBL_MAX			1.7976931348623158e+308

#if LSM_BASETYPE == LSM_FLOAT
#define LSM_LOOKUP_BITS		LSM_FLT_LOOKUP_BITS
#define LSM_EXP_POS			LSM_FLT_EXP_POS
#define LSM_EXP_BIAS		LSM_FLT_EXP_BIAS
#elif LSM_BASETYPE == LSM_DOUBLE
#define LSM_LOOKUP_BITS		LSM_DBL_LOOKUP_BITS
#define LSM_EXP_POS			LSM_DBL_EXP_POS
#define LSM_EXP_BIAS		LSM_DBL_EXP_BIAS
#endif	// LSM_BASETYPE

#define LSM_LOOKUP_POS		(LSM_EXP_POS - LSM_LOOKUP_BITS)
#define LSM_SEED_POS		(LSM_EXP_POS - 8)
#define LSM_SQRT_TABLE_SIZE	(2 << LSM_LOOKUP_BITS)
#define LSM_LOOKUP_MASK		(LSM_SQRT_TABLE_SIZE - 1)

#endif	// __LSM_CONSTANTS_H__
