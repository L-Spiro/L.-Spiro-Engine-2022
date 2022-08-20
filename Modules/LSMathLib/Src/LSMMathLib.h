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


#ifndef __LSM_MATHLIB_H__
#define __LSM_MATHLIB_H__

#include "LSSTDStandardLib.h"
using namespace ::lsstd;

#include <cassert>
#include <cmath>
#include "Fixed/LSMFixed.h"			// Include the fixed-point class.

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

/**
 * \def LSM_PERFORMANCE
 * \brief Adjusts the trade-off between speed and accuracy throughout the math library.
 *
 * Values range from 0 to 4.  0 = Faster but less accuracy.  4 = slower but more accurate.
 *	Adjust to suit your needs.  Note that if LSM_PERFORMANCE <= 0 the size of the CVector3
 *	class will change from 3 LSREALs to 4 LSREALs.
 */
#define LSM_PERFORMANCE				3





// ========================================
// Base type.
// ========================================
// The types of basics available.  This is just an enumeration.
// This type is used in all vectors/matrices/etc.
#define LSM_FLOAT					0
#define LSM_DOUBLE					1
#define LSM_FIXED					2
// Now select one from the above list to actually be used.
#ifndef LSM_BASETYPE
#define LSM_BASETYPE				LSM_FLOAT
#endif	// #ifndef LSM_BASETYPE
// ========================================


// ========================================
// Performance.
// ========================================
// The performance desired.  Certain calculations
//	are modified depending on this, losing precision
//	in favor of speed or vice-versa.
// Values from 0 to 4 are allowed.
//	0 = Fastest, but less accuracy.
//	4 =	Slowest, but more accurate.
#ifndef LSM_PERFORMANCE
#define LSM_PERFORMANCE				2
#endif	// #ifndef LSM_PERFORMANCE
// ========================================



// Define all the constants.
#include "LSMConstants.h"


namespace lsm {

	// Create the basic type based on the LSM_BASETYPE macro.
	// LSREAL is the type we use throughout the engine for all math.
	// This also changes which cos(), sin(), etc. functions are used.
#if LSM_BASETYPE == LSM_FLOAT
	typedef float					LSREAL;
	typedef double					LSREALHP;			// High-precision type.
	// Cast from regular precision and high precision.
#define LSREAL_TO_HP( VAL )			(static_cast<LSREALHP>(VAL))
	// Cast from high-precision back to regular precision.
#define HP_TO_LSREAL( VAL )			(static_cast<LSREAL>(VAL))
#define LSMCOS( F )					::cosf( static_cast<LSREAL>(F) )
#define LSMACOS( F )				::acosf( static_cast<LSREAL>(F) )
#define LSMSIN( F )					::sinf( static_cast<LSREAL>(F) )
#define LSMASIN( F )				::asinf( static_cast<LSREAL>(F) )
#define LSMSQRT( F )				::sqrtf( static_cast<LSREAL>(F) )
#define LSMPOW( F, Q )				std::powf( static_cast<LSREAL>(F), static_cast<LSREAL>(Q) )
#define LSMLOG( F )					::logf( static_cast<LSREAL>(F) )
#define LSMEXP( F )					::expf( static_cast<LSREAL>(F) )
#define LSMABS( F )					::fabsf( static_cast<LSREAL>(F) )
#define LSMTAN( F )					::tanf( static_cast<LSREAL>(F) )
#define LSMATAN( F )				::atanf( static_cast<LSREAL>(F) )
#define LSMATAN2F( Y, X )			::atan2f( static_cast<LSREAL>(Y), static_cast<LSREAL>(X) )
#define LSMFMOD( Y, X )				::fmodf( static_cast<LSREAL>(Y), static_cast<LSREAL>(X) )
#define LSMFLOOR( F )				::floorf( static_cast<LSREAL>(F) )
#define LSMCEIL( F )				::ceilf( static_cast<LSREAL>(F) )

#define LSMCOSHP( F )				::cos( static_cast<LSREALHP>(F) )
#define LSMACOSHP( F )				::acos( static_cast<LSREALHP>(F) )
#define LSMSINHP( F )				::sin( static_cast<LSREALHP>(F) )
#define LSMASINHP( F )				::asin( static_cast<LSREALHP>(F) )
#define LSMSQRTHP( F )				::sqrt( static_cast<LSREALHP>(F) )
#define LSMPOWHP( F, Q )			std::pow( static_cast<LSREALHP>(F), static_cast<LSREALHP>(Q) )
#define LSMEXPHP( F )				::exp( static_cast<LSREALHP>(F) )
#define LSMABSHP( F )				::fabs( static_cast<LSREALHP>(F) )
#define LSMTANHP( F )				::tan( static_cast<LSREALHP>(F) )
#define LSMATANHP( F )				::atan( static_cast<LSREALHP>(F) )
#define LSMATAN2FHP( Y, X )			::atan2( static_cast<LSREALHP>(Y), static_cast<LSREALHP>(X) )
#define LSMFMODHP( Y, X )			::fmod( static_cast<LSREALHP>(Y), static_cast<LSREALHP>(X) )
#define LSMFLOORHP( F )				::floor( static_cast<LSREALHP>(F) )
#define LSMCEILHP( F )				::ceil( static_cast<LSREALHP>(F) )
#elif LSM_BASETYPE == LSM_DOUBLE
	typedef double					LSREAL;
	typedef long double				LSREALHP;			// High-precision type.
	// Cast from regular precision and high precision.
#define LSREAL_TO_HP( VAL )			(static_cast<LSREALHP>(VAL))
	// Cast from high-precision back to regular precision.
#define HP_TO_LSREAL( VAL )			(static_cast<LSREAL>(VAL))
#define LSMCOS( F )					::cos( static_cast<LSREAL>(F) )
#define LSMACOS( F )				::acos( static_cast<LSREAL>(F) )
#define LSMSIN( F )					::sin( static_cast<LSREAL>(F) )
#define LSMASIN( F )				::asin( static_cast<LSREAL>(F) )
#define LSMSQRT( F )				::sqrt( static_cast<LSREAL>(F) )
#define LSMPOW( F, Q )				std::pow( static_cast<LSREAL>(F), static_cast<LSREAL>(Q) )
#define LSMLOG( F )					::log( static_cast<LSREAL>(F) )
#define LSMEXP( F )					::exp( static_cast<LSREAL>(F) )
#define LSMABS( F )					::fabs( static_cast<LSREAL>(F) )
#define LSMTAN( F )					::tan( static_cast<LSREAL>(F) )
#define LSMATAN( F )				::atan( static_cast<LSREAL>(F) )
#define LSMATAN2F( Y, X )			::atan2( static_cast<LSREAL>(Y), static_cast<LSREAL>(X) )
#define LSMFMOD( Y, X )				::fmod( static_cast<LSREAL>(Y), static_cast<LSREAL>(X) )
#define LSMFLOOR( F )				::floor( static_cast<LSREAL>(F) )
#define LSMCEIL( F )				::ceil( static_cast<LSREAL>(F) )

#define LSMCOSHP( F )				::cos( static_cast<LSREALHP>(F) )
#define LSMACOSHP( F )				::acos( static_cast<LSREALHP>(F) )
#define LSMSINHP( F )				::sin( static_cast<LSREALHP>(F) )
#define LSMASINHP( F )				::asin( static_cast<LSREALHP>(F) )
#define LSMSQRTHP( F )				::sqrt( static_cast<LSREALHP>(F) )
#define LSMPOWHP( F, Q )			std::pow( static_cast<LSREALHP>(F), static_cast<LSREALHP>(Q) )
#define LSMEXPHP( F )				::exp( static_cast<LSREALHP>(F) )
#define LSMABSHP( F )				::fabs( static_cast<LSREALHP>(F) )
#define LSMTANHP( F )				::tan( static_cast<LSREALHP>(F) )
#define LSMATANHP( F )				::atan( static_cast<LSREALHP>(F) )
#define LSMATAN2FHP( Y, X )			::atan2( static_cast<LSREALHP>(Y), static_cast<LSREALHP>(X) )
#define LSMFMODHP( Y, X )			::fmod( static_cast<LSREALHP>(Y), static_cast<LSREALHP>(X) )
#define LSMFLOORHP( F )				::floor( static_cast<LSREALHP>(F) )
#define LSMCEILHP( F )				::ceil( static_cast<LSREALHP>(F) )
#elif LSM_BASETYPE == LSM_FIXED
	typedef CFixed<int32_t, 10>		LSREAL;
	typedef CFixed<int64_t, 24>		LSREALHP;			// High-precision type.
// Cast from regular precision to high precision.
#define LSREAL_TO_HP( VAL )			(LSREALHP( LSREALHP::CFixedRawType( (VAL).GetRawValue() << (LSREALHP::Bits() - LSREAL::Bits()) ) ))
// Cast from high-precision back to regular precision.
#define HP_TO_LSREAL( VAL )			(LSREAL( LSREAL::CFixedRawType( LSREAL::Cast( (VAL).GetRawValue() >> (LSREALHP::Bits() - LSREAL::Bits()) ) ) ))
#define LSMCOS( F )					static_cast<LSREAL>(::cosf( static_cast<float>(F) ))
#define LSMACOS( F )				static_cast<LSREAL>(::acosf( static_cast<float>(F) ))
#define LSMSIN( F )					static_cast<LSREAL>(::sinf( static_cast<float>(F) ))
#define LSMASIN( F )				static_cast<LSREAL>(::asinf( static_cast<float>(F) ))
#define LSMSQRT( F )				static_cast<LSREAL>(::sqrtf( static_cast<float>(F) ))
#define LSMPOW( F, Q )				static_cast<LSREAL>(std::powf( static_cast<float>(F), static_cast<float>(Q) ))
#define LSMLOG( F )					static_cast<LSREAL>(::log( static_cast<float>(F) ))
#define LSMEXP( F )					static_cast<LSREAL>(::expf( static_cast<float>(F) ))
#define LSMABS( F )					static_cast<LSREAL>(::fabs( static_cast<float>(F) ))
#define LSMTAN( F )					static_cast<LSREAL>(::tanf( static_cast<float>(F) ))
#define LSMATAN( F )				static_cast<LSREAL>(::atanf( static_cast<float>(F) ))
#define LSMATAN2F( Y, X )			static_cast<LSREAL>(::atan2f( static_cast<float>(Y), static_cast<float>(X) ))
#define LSMFMOD( Y, X )				static_cast<LSREAL>(::fmodf( static_cast<float>(Y), static_cast<float>(X) ))
#define LSMFLOOR( F )				static_cast<LSREAL>(::floorf( static_cast<float>(F) ))
#define LSMCEIL( F )				static_cast<LSREAL>(::ceilf( static_cast<float>(F) ))

#define LSMCOSHP( F )				static_cast<LSREALHP>(::cos( static_cast<double>(F) ))
#define LSMACOSHP( F )				static_cast<LSREALHP>(::acos( static_cast<double>(F) ))
#define LSMSINHP( F )				static_cast<LSREALHP>(::sin( static_cast<double>(F) ))
#define LSMASINHP( F )				static_cast<LSREALHP>(::asin( static_cast<double>(F) ))
#define LSMSQRTHP( F )				static_cast<LSREALHP>(::sqrt( static_cast<double>(F) ))
#define LSMPOWHP( F, Q )			static_cast<LSREALHP>(std::pow( static_cast<double>(F), static_cast<double>(Q) ))
#define LSMEXPHP( F )				static_cast<LSREALHP>(::exp( static_cast<double>(F) ))
#define LSMABSHP( F )				static_cast<LSREALHP>(::fabs( static_cast<double>(F) ))
#define LSMTANHP( F )				static_cast<LSREALHP>(::tan( static_cast<double>(F) ))
#define LSMATANHP( F )				static_cast<LSREALHP>(::atan( static_cast<double>(F) ))
#define LSMATAN2FHP( Y, X )			static_cast<LSREALHP>(::atan2( static_cast<double>(Y), static_cast<double>(X) ))
#define LSMFMODHP( Y, X )			static_cast<LSREALHP>(::fmod( static_cast<double>(Y), static_cast<double>(X) ))
#define LSMFLOORHP( F )				static_cast<LSREALHP>(::floor( static_cast<double>(F) ))
#define LSMCEILHP( F )				static_cast<LSREALHP>(::ceil( static_cast<double>(F) ))
#else
#error "LSM_BASETYPE must be set to LSM_FLOAT, LSM_DOUBLE, or LSM_FIXED."
#endif	// LSM_BASETYPE


	/**
	 * Class CMathLib
	 * \brief The base for our math functions.
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
	class CMathLib {
	public :
		/**
		 * Initializes the math library.  Can be called only once.  Multiple calls have no effect.  The math library
		 *	cannot be used until this is called.
		 */
		static void LSE_FCALL		Init();


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// Math functions, built either for speed or for power, depending on the
		//	LSM_PERFORMANCE macro.
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Inverse square root.
		 */
		static LSREAL LSE_FCALL		InvSqrt( LSREAL _fX );

		/**
		 * Square root.
		 */
		static LSREAL LSE_FCALL		Sqrt( LSREAL _fX );

		/**
		 * Sine.
		 */
		static LSREAL LSE_FCALL		Sin( LSREAL _fX );

		/**
		 * Cosine.
		 */
		static LSREAL LSE_FCALL		Cos( LSREAL _fX );

		/**
		 * Sine and cosine at once.  Fast on the Win32 platform.
		 */
		static void LSE_FCALL		SinCos( LSREAL _fA, LSREAL &_fSine, LSREAL &_fCosine );

		/**
		 * Float sine.
		 */
		static float LSE_FCALL	SinF( float _fX );

		/**
		 * Float cosine.
		 */
		static float LSE_FCALL	CosF( float _fX );

		/**
		 * Float sine and cosine at once.
		 */
		static void LSE_FCALL		SinCosF( float _fA, float &_fSine, float &_fCosine );

		/**
		 * Double sine.
		 */
		static double LSE_FCALL	SinD( double _fX );

		/**
		 * Double cosine.
		 */
		static double LSE_FCALL	CosD( double _fX );

		/**
		 * Double sine and cosine at once.
		 */
		static void LSE_FCALL		SinCosD( double _fA, double &_fSine, double &_fCosine );

		/**
		 * 11th-degree float sine.
		 */
		static float LSE_FCALL	SinF_Medium( float _fX );

		/**
		 * 10th-degree float cosine.
		 */
		static float LSE_FCALL	CosF_Medium( float _fX );

		/**
		 * Float sine and cosine at once.
		 */
		static void LSE_FCALL		SinCosF_Medium( float _fA, float &_fSine, float &_fCosine );

		/**
		 * Tangent.
		 */
		static LSREAL LSE_FCALL		Tan( LSREAL _fX );

		/**
		 * Arc sine.
		 */
		static LSREAL LSE_FCALL		ASin( LSREAL _fX );

		/**
		 * Arc cosine.
		 */
		static LSREAL LSE_FCALL		ACos( LSREAL _fX );

		/**
		 * Arc tangent.
		 */
		static LSREAL LSE_FCALL		ATan( LSREAL _fX );

		/**
		 * Arc tangent.
		 */
		static LSREAL LSE_FCALL		ATan( LSREAL _fY, LSREAL _fX );

		/**
		 * _fX raised to the power of fY.
		 */
		static LSREAL LSE_FCALL		Pow( LSREAL _fX, LSREAL _fY );

		/**
		 * E raised to the power of _fX.
		 */
		static LSREAL LSE_FCALL		Exp( LSREAL _fX );

		/**
		 *  LOGy( X ).  Determines by what exponent _fX was raised to get _fY result.
		 *
		 * \param _fX The base.
		 * \param _fY The result.
		 * \return Returns LOGy( X ).
		 */
		static LSREAL LSE_FCALL		Log( LSREAL _fX, LSREAL _fY );

		/**
		 * Absolute value of _fX.
		 */
		static LSREAL LSE_FCALL		Abs( LSREAL _fX );

		/**
		 * Absolute value of _fX.
		 */
		template <typename _tType>
		static _tType LSE_FCALL		AbsT( _tType _fX );

		/**
		 * Test two numbers for having the same sign.
		 */
		static LSBOOL LSE_FCALL		SameSign( LSREAL _fLeft, LSREAL _fRight );

		/**
		 * Floating-point modulus.
		 */
		static LSREAL LSE_FCALL		FMod( LSREAL _fX, LSREAL _fMod );

		/**
		 * Nearest integer.
		 */
		static LSREAL LSE_FCALL		Floor( LSREAL _fX );

		/**
		 * Nearest integer upwards.
		 *
		 * \param _fX The value to round upwards.
		 * \return Returns ceil( X ).
		 */
		static LSREAL LSE_FCALL		Ceil( LSREAL _fX );

		/**
		 * Max between two numbers.
		 */
		template <typename _tType>
		static _tType LSE_FCALL		Max( _tType _fX, _tType _fY );

		/**
		 * Min between two numbers.
		 */
		template <typename _tType>
		static _tType LSE_FCALL		Min( _tType _fX, _tType _fY );

		/**
		 * Swap two numbers.
		 *
		 * \param _fLeft The left operand.
		 * \param _fRight The right operand.
		 */
		template <typename _tType>
		static void LSE_FCALL		Swap( LSE_UNALIGNED _tType &_fLeft, LSE_UNALIGNED _tType &_fRight );

		/**
		 * Clamp a value between two others.
		 *
		 * \param _fValue The value to be clamped.
		 * \param _fLow Lower limit for the value to clamp, inclusive.
		 * \param _fHigh Upper limit for the value to clamp, inclusive.
		 * \return Returns the clamped value.
		 */
		template <typename _tType>
		static _tType LSE_FCALL		Clamp( _tType _fValue, _tType _fLow, _tType _fHigh );

		/**
		 * Compare values for equality using an epsilon factor.
		 *
		 * \param _fX The left value to compare using epsilon.
		 * \param _fY The right value to compare using epsilon.
		 * \param _fEpsilon The epsilon to use for the comparison.
		 * \return Returns true if the numbers are equal given an epsilon value.
		 */
		static LSBOOL LSE_FCALL		Equals( LSREAL _fX, LSREAL _fY, LSREAL _fEpsilon );

		/**
		 * Compare values for equality using an epsilon factor.
		 *
		 * \param _fX The left value to compare using epsilon.
		 * \param _fY The right value to compare using epsilon.
		 * \param _fEpsilon The epsilon to use for the comparison.
		 * \return Returns true if the numbers are equal given an epsilon value.
		 */
		template <typename _tType>
		static LSBOOL LSE_FCALL		EqualsT( _tType _fX, _tType _fY, _tType _fEpsilon );

	protected :
#if LSM_BASETYPE == LSM_FLOAT || LSM_BASETYPE == LSM_DOUBLE
		union LSM_FLINT {
			uint32_t				ui32Int;
			LSREAL					fReal;
		};
#endif	// #if LSM_BASETYPE == LSM_FLOAT || LSM_BASETYPE == LSM_DOUBLE

		/**
		 * Are we initialized?
		 */
		static LSBOOL				m_bInitialized;

		

#if LSM_PERFORMANCE <= -1
#define LSM_SINCOSTABLE
#endif	// #if LSM_PERFORMANCE <= -1
#ifdef LSM_SINCOSTABLE
		// We have tables for fast cosine and sine look-up if performance
		//	is desired.
		// If you change the table size you must also change the LSMCOS_F() and LSMSIN_F() macros.
		//	Currently they rely on this size to save time by casting the value to uint16_t
		//	(preventing array overflow).  You can change LSM_TABLESIZE, but you may have
		//	to add a % operator for array sanity, which will be slower.
#define LSM_TABLESIZE				(0xFFFF+1)
		static LSREAL				m_fCosTable[LSM_TABLESIZE];
		static LSREAL				m_fSinTable[LSM_TABLESIZE];

		// Easily change between the look-up table and the real value.
#define LSMCOS_F( F )				m_fCosTable[static_cast<uint16_t>(static_cast<float>(1.0/(LSM_PI_DOUBLE*2.0)*LSM_TABLESIZE)*(F))]
#define LSMSIN_F( F )				m_fSinTable[static_cast<uint16_t>(static_cast<float>(1.0/(LSM_PI_DOUBLE*2.0)*LSM_TABLESIZE)*(F))]
#else
#define LSMCOS_F( F )				LSMCOS( F )
#define LSMSIN_F( F )				LSMSIN( F )
#endif	// #ifdef LSM_SINCOSTABLE
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Inverse square root.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::InvSqrt( LSREAL _fX ) {
#if LSM_BASETYPE == LSM_FLOAT
#if defined( LSE_GCC )
		// Strange problem with these machines.  Maybe they should upgrade their FPU's.
		if ( _fX == LSM_ZERO ) {
			return (LSM_INFINITY * static_cast<LSREAL>(0.25));
		}
#endif	// #if defined( LSE_GCC )
		float fY, fR;
		union {
			float fVal;
			int32_t i32Val;
		} uVal;

		uVal.fVal = _fX;

		fY = _fX * LSM_HALF;
		uVal.i32Val = /*0x5F375A86*/0x5F3759DF - (uVal.i32Val >> 1);
		fR = uVal.fVal * (1.5f - uVal.fVal * uVal.fVal * fY);
		fR = fR * (1.5f - fR * fR * fY);

		// More accuracy?
#if LSM_PERFORMANCE >= 3
		fR = fR * (1.5f - fR * fR * fY);
#endif	// #if LSM_PERFORMANCE >= 3

		// Even MORE accuracy?
#if LSM_PERFORMANCE >= 4
		fR = fR * (1.5f - fR * fR * fY);
#endif	// #if LSM_PERFORMANCE >= 4
		return static_cast<LSREAL>(fR);
#else	// #if LSM_BASETYPE == LSM_FLOAT
		return static_cast<LSREAL>(1.0f / ::sqrtf( static_cast<float>(_fX) ));
#endif	// #if LSM_BASETYPE == LSM_FLOAT
	}

	/**
	 * Square root.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Sqrt( LSREAL _fX ) {
#if LSM_PERFORMANCE >= 0
		// Windows test: ::sqrtf() faster than _fX * InvSqrt( _fX ).
		return LSMSQRT( _fX );
#else
#if LSM_BASETYPE == LSM_FLOAT
		return _fX * InvSqrt( _fX );
#else	// #if LSM_BASETYPE == LSM_FLOAT
		return LSMSQRT( _fX );
#endif	// #if LSM_BASETYPE == LSM_FLOAT
#endif	// #if LSM_PERFORMANCE >= 3
	}

	/**
	 * Sine.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Sin( LSREAL _fX ) {
#if LSM_BASETYPE == LSM_FLOAT
		return SinF( _fX );
#elif LSM_BASETYPE == LSM_DOUBLE
		return SinD( _fX );
#else
		return LSMSIN( _fX );
#endif	// #if LSM_BASETYPE == LSM_FLOAT
	}

	/**
	 * Cosine.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Cos( LSREAL _fX ) {
#if LSM_BASETYPE == LSM_FLOAT
		return CosF( _fX );
#elif LSM_BASETYPE == LSM_DOUBLE
		return CosD( _fX );
#else
		return LSMCOS( _fX );
#endif	// #if LSM_BASETYPE == LSM_FLOAT
	}

	/**
	 * Sine and cosine at once.  Fast on the Win32 platform.
	 */
	LSE_INLINE void LSE_FCALL CMathLib::SinCos( LSREAL _fA, LSREAL &_fSine, LSREAL &_fCosine ) {
#if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT || LSM_BASETYPE == LSM_DOUBLE)
		LSE_ASM_BEGIN
			fld		_fA
			fsincos
			mov		ecx, _fCosine
			mov		edx, _fSine
#if LSM_BASETYPE == LSM_FLOAT
			fstp	dword ptr [ecx]
			fstp	dword ptr [edx]
#elif LSM_BASETYPE == LSM_DOUBLE
			fstp	qword ptr [ecx]
			fstp	qword ptr [edx]
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		LSE_ASM_END
#else	// #if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT || LSM_BASETYPE == LSM_DOUBLE)
		int32_t i32I = int32_t( _fA * LSM_RCPPI );
		_fA = (_fA - LSREAL( i32I ) * LSM_PI);

		LSREAL fX2 = _fA * _fA;
		if ( i32I & 1 ) {
			_fSine = -_fA * (LSREAL( 1.00000000000000000000e+00 ) +
				fX2 * (LSREAL( -1.66666671633720397949e-01 ) +
				fX2 * (LSREAL( 8.33333376795053482056e-03 ) +
				fX2 * (LSREAL( -1.98412497411482036114e-04 ) +
				fX2 * (LSREAL( 2.75565571428160183132e-06 ) +
				fX2 * (LSREAL( -2.50368472620721149724e-08 ) +
				fX2 * (LSREAL( 1.58849267073435385100e-10 ) +
				fX2 * LSREAL( -6.58925550841432672300e-13 ))))))));
			_fCosine = -LSREAL( 1.00000000000000000000e+00 ) -
				fX2 * (LSREAL( -5.00000000000000000000e-01 ) +
				fX2 * (LSREAL( 4.16666641831398010254e-02 ) +
				fX2 * (LSREAL( -1.38888671062886714935e-03 ) +
				fX2 * (LSREAL( 2.48006890615215525031e-05 ) +
				fX2 * (LSREAL( -2.75369927749125054106e-07 ) +
				fX2 * (LSREAL( 2.06207229069832465029e-09 ) +
				fX2 * LSREAL( -9.77507137733812925262e-12 )))))));
		}
		else {
			_fSine = _fA * (LSREAL( 1.00000000000000000000e+00 ) +
				fX2 * (LSREAL( -1.66666671633720397949e-01 ) +
				fX2 * (LSREAL( 8.33333376795053482056e-03 ) +
				fX2 * (LSREAL( -1.98412497411482036114e-04 ) +
				fX2 * (LSREAL( 2.75565571428160183132e-06 ) +
				fX2 * (LSREAL( -2.50368472620721149724e-08 ) +
				fX2 * (LSREAL( 1.58849267073435385100e-10 ) +
				fX2 * LSREAL( -6.58925550841432672300e-13 ))))))));
			_fCosine = LSREAL( 1.00000000000000000000e+00 ) +
				fX2 * (LSREAL( -5.00000000000000000000e-01 ) +
				fX2 * (LSREAL( 4.16666641831398010254e-02 ) +
				fX2 * (LSREAL( -1.38888671062886714935e-03 ) +
				fX2 * (LSREAL( 2.48006890615215525031e-05 ) +
				fX2 * (LSREAL( -2.75369927749125054106e-07 ) +
				fX2 * (LSREAL( 2.06207229069832465029e-09 ) +
				fX2 * LSREAL( -9.77507137733812925262e-12 )))))));
		}
#endif	// #if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT || LSM_BASETYPE == LSM_DOUBLE)
	}

	/**
	 * Float sine.
	 */
	LSE_INLINE float LSE_FCALL CMathLib::SinF( float _fX ) {
		int32_t i32I = int32_t( _fX * LSM_RCPPI );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fX = (_fX - float( i32I ) * LSM_PI);

		float fX2 = _fX * _fX;

		return (i32I & 1) ?
			-_fX * (float( 1.00000000000000000000e+00 ) +
				fX2 * (float( -1.66666671633720397949e-01 ) +
				fX2 * (float( 8.33333376795053482056e-03 ) +
				fX2 * (float( -1.98412497411482036114e-04 ) +
				fX2 * (float( 2.75565571428160183132e-06 ) +
				fX2 * (float( -2.50368472620721149724e-08 ) +
				fX2 * (float( 1.58849267073435385100e-10 ) +
				fX2 * float( -6.58925550841432672300e-13 )))))))) :
			_fX * (float( 1.00000000000000000000e+00 ) +
				fX2 * (float( -1.66666671633720397949e-01 ) +
				fX2 * (float( 8.33333376795053482056e-03 ) +
				fX2 * (float( -1.98412497411482036114e-04 ) +
				fX2 * (float( 2.75565571428160183132e-06 ) +
				fX2 * (float( -2.50368472620721149724e-08 ) +
				fX2 * (float( 1.58849267073435385100e-10 ) +
				fX2 * float( -6.58925550841432672300e-13 ))))))));
	}

	/**
	 * Float cosine.
	 */
	LSE_INLINE float LSE_FCALL CMathLib::CosF( float _fX ) {
		int32_t i32I = int32_t( _fX * LSM_RCPPI );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fX = (_fX - float( i32I ) * LSM_PI);

		float fX2 = _fX * _fX;

		return (i32I & 1) ?
			float( -1.00000000000000000000e+00 ) -
				fX2 * (float( -5.00000000000000000000e-01 ) +
				fX2 * (float( 4.16666641831398010254e-02 ) +
				fX2 * (float( -1.38888671062886714935e-03 ) +
				fX2 * (float( 2.48006890615215525031e-05 ) +
				fX2 * (float( -2.75369927749125054106e-07 ) +
				fX2 * (float( 2.06207229069832465029e-09 ) +
				fX2 * float( -9.77507137733812925262e-12 ))))))) :
			float( 1.00000000000000000000e+00 ) +
				fX2 * (float( -5.00000000000000000000e-01 ) +
				fX2 * (float( 4.16666641831398010254e-02 ) +
				fX2 * (float( -1.38888671062886714935e-03 ) +
				fX2 * (float( 2.48006890615215525031e-05 ) +
				fX2 * (float( -2.75369927749125054106e-07 ) +
				fX2 * (float( 2.06207229069832465029e-09 ) +
				fX2 * float( -9.77507137733812925262e-12 )))))));
	}

	/**
	 * Float sine and cosine at once.
	 */
	LSE_INLINE void LSE_FCALL CMathLib::SinCosF( float _fA, float &_fSine, float &_fCosine ) {
		int32_t i32I = int32_t( _fA * LSM_RCPPI );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fA = (_fA - float( i32I ) * LSM_PI);

		float fX2 = _fA * _fA;

		if ( i32I & 1 ) {
			_fSine = -_fA * (float( 1.00000000000000000000e+00 ) +
				fX2 * (float( -1.66666671633720397949e-01 ) +
				fX2 * (float( 8.33333376795053482056e-03 ) +
				fX2 * (float( -1.98412497411482036114e-04 ) +
				fX2 * (float( 2.75565571428160183132e-06 ) +
				fX2 * (float( -2.50368472620721149724e-08 ) +
				fX2 * (float( 1.58849267073435385100e-10 ) +
				fX2 * float( -6.58925550841432672300e-13 ))))))));
			_fCosine = float( -1.00000000000000000000e+00 ) -
				fX2 * (float( -5.00000000000000000000e-01 ) +
				fX2 * (float( 4.16666641831398010254e-02 ) +
				fX2 * (float( -1.38888671062886714935e-03 ) +
				fX2 * (float( 2.48006890615215525031e-05 ) +
				fX2 * (float( -2.75369927749125054106e-07 ) +
				fX2 * (float( 2.06207229069832465029e-09 ) +
				fX2 * float( -9.77507137733812925262e-12 )))))));
		}
		else {
			_fSine = _fA * (float( 1.00000000000000000000e+00 ) +
				fX2 * (float( -1.66666671633720397949e-01 ) +
				fX2 * (float( 8.33333376795053482056e-03 ) +
				fX2 * (float( -1.98412497411482036114e-04 ) +
				fX2 * (float( 2.75565571428160183132e-06 ) +
				fX2 * (float( -2.50368472620721149724e-08 ) +
				fX2 * (float( 1.58849267073435385100e-10 ) +
				fX2 * float( -6.58925550841432672300e-13 ))))))));
			_fCosine = float( 1.00000000000000000000e+00 ) +
				fX2 * (float( -5.00000000000000000000e-01 ) +
				fX2 * (float( 4.16666641831398010254e-02 ) +
				fX2 * (float( -1.38888671062886714935e-03 ) +
				fX2 * (float( 2.48006890615215525031e-05 ) +
				fX2 * (float( -2.75369927749125054106e-07 ) +
				fX2 * (float( 2.06207229069832465029e-09 ) +
				fX2 * float( -9.77507137733812925262e-12 )))))));
		}
	}

	/**
	 * Double sine.
	 */
	LSE_INLINE double LSE_FCALL CMathLib::SinD( double _fX ) {
		int64_t i32I = int64_t( _fX * LSM_RCPPI_DOUBLE );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fX = (_fX - double( i32I ) * LSM_PI_DOUBLE);

		double fX2 = _fX * _fX;

		return (i32I & 1) ?
			-_fX * (double( 9.99999999958141594369e-01 ) +
				fX2 * (double( -1.66666666451352446510e-01 ) +
				fX2 * (double( 8.33333301181456848061e-03 ) +
				fX2 * (double( -1.98412483604294194060e-04 ) +
				fX2 * (double( 2.75565598751558709709e-06 ) +
				fX2 * (double( -2.50368914257412183582e-08 ) +
				fX2 * (double( 1.58850007480674172780e-10 ) +
				fX2 * double( -6.58076002675210620158e-13 )))))))) :
			_fX * (double( 9.99999999958141594369e-01 ) +
				fX2 * (double( -1.66666666451352446510e-01 ) +
				fX2 * (double( 8.33333301181456848061e-03 ) +
				fX2 * (double( -1.98412483604294194060e-04 ) +
				fX2 * (double( 2.75565598751558709709e-06 ) +
				fX2 * (double( -2.50368914257412183582e-08 ) +
				fX2 * (double( 1.58850007480674172780e-10 ) +
				fX2 * double( -6.58076002675210620158e-13 ))))))));
	}

	/**
	 * Double cosine.
	 */
	LSE_INLINE double LSE_FCALL CMathLib::CosD( double _fX ) {
		int64_t i32I = int64_t( _fX * LSM_RCPPI_DOUBLE );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fX = (_fX - double( i32I ) * LSM_PI_DOUBLE);

		double fX2 = _fX * _fX;

		return (i32I & 1) ?
			double( -1.00000000040559489101e+00 ) -
				fX2 * (double( -4.99999998886526908493e-01 ) +
				fX2 * (double( 4.16666641590361999659e-02 ) +
				fX2 * (double( -1.38888674687691338061e-03 ) +
				fX2 * (double( 2.48006913718665254265e-05 ) +
				fX2 * (double( -2.75369918573799524591e-07 ) +
				fX2 * (double( 2.06207503915813507868e-09 ) +
				fX2 * double( -9.77507131527006491862e-12 ))))))) :
			double( 1.00000000040559489101e+00 ) +
				fX2 * (double( -4.99999998886526908493e-01 ) +
				fX2 * (double( 4.16666641590361999659e-02 ) +
				fX2 * (double( -1.38888674687691338061e-03 ) +
				fX2 * (double( 2.48006913718665254265e-05 ) +
				fX2 * (double( -2.75369918573799524591e-07 ) +
				fX2 * (double( 2.06207503915813507868e-09 ) +
				fX2 * double( -9.77507131527006491862e-12 )))))));
	}

	/**
	 * Double sine and cosine at once.
	 */
	LSE_INLINE void LSE_FCALL CMathLib::SinCosD( double _fA, double &_fSine, double &_fCosine ) {
		int64_t i32I = int64_t( _fA * LSM_RCPPI_DOUBLE );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fA = (_fA - double( i32I ) * LSM_PI_DOUBLE);

		double fX2 = _fA * _fA;

		if ( i32I & 1 ) {
			_fSine = -_fA * (double( 9.99999999958141594369e-01 ) +
				fX2 * (double( -1.66666666451352446510e-01 ) +
				fX2 * (double( 8.33333301181456848061e-03 ) +
				fX2 * (double( -1.98412483604294194060e-04 ) +
				fX2 * (double( 2.75565598751558709709e-06 ) +
				fX2 * (double( -2.50368914257412183582e-08 ) +
				fX2 * (double( 1.58850007480674172780e-10 ) +
				fX2 * double( -6.58076002675210620158e-13 ))))))));
			_fCosine = double( -1.00000000040559489101e+00 ) -
				fX2 * (double( -4.99999998886526908493e-01 ) +
				fX2 * (double( 4.16666641590361999659e-02 ) +
				fX2 * (double( -1.38888674687691338061e-03 ) +
				fX2 * (double( 2.48006913718665254265e-05 ) +
				fX2 * (double( -2.75369918573799524591e-07 ) +
				fX2 * (double( 2.06207503915813507868e-09 ) +
				fX2 * double( -9.77507131527006491862e-12 )))))));
		}
		else {
			_fSine = _fA * (double( 9.99999999958141594369e-01 ) +
				fX2 * (double( -1.66666666451352446510e-01 ) +
				fX2 * (double( 8.33333301181456848061e-03 ) +
				fX2 * (double( -1.98412483604294194060e-04 ) +
				fX2 * (double( 2.75565598751558709709e-06 ) +
				fX2 * (double( -2.50368914257412183582e-08 ) +
				fX2 * (double( 1.58850007480674172780e-10 ) +
				fX2 * double( -6.58076002675210620158e-13 ))))))));
			_fCosine = double( 1.00000000040559489101e+00 ) +
				fX2 * (double( -4.99999998886526908493e-01 ) +
				fX2 * (double( 4.16666641590361999659e-02 ) +
				fX2 * (double( -1.38888674687691338061e-03 ) +
				fX2 * (double( 2.48006913718665254265e-05 ) +
				fX2 * (double( -2.75369918573799524591e-07 ) +
				fX2 * (double( 2.06207503915813507868e-09 ) +
				fX2 * double( -9.77507131527006491862e-12 )))))));
		}
	}

	/**
	 * 11th-degree float sine.
	 */
	LSE_INLINE float LSE_FCALL CMathLib::SinF_Medium( float _fX ) {
		int32_t i32I = int32_t( _fX * LSM_RCPPI );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fX = (_fX - float( i32I ) * LSM_PI);

		float fX2 = _fX * _fX;
		// Average error:
		//	0.000000071991304556528727213541666666667
		// Max error:
		//	0.000000543892383575439453125
		return (i32I & 1) ?
			-_fX * (float( 9.99999701976776123047e-01 ) +
				fX2 * (float( -1.66665777564048767090e-01 ) +
				fX2 * (float( 8.33255797624588012695e-03 ) +
				fX2 * (float( -1.98125766473822295666e-04 ) +
				fX2 * (float( 2.70405212177138309926e-06 ) +
				fX2 * float( -2.05329886426852681325e-08 )))))) :
			_fX * (float( 9.99999701976776123047e-01 ) +
				fX2 * (float( -1.66665777564048767090e-01 ) +
				fX2 * (float( 8.33255797624588012695e-03 ) +
				fX2 * (float( -1.98125766473822295666e-04 ) +
				fX2 * (float( 2.70405212177138309926e-06 ) +
				fX2 * float( -2.05329886426852681325e-08 ))))));
	}

	/**
	 * 10th-degree float cosine.
	 */
	LSE_INLINE float LSE_FCALL CMathLib::CosF_Medium( float _fX ) {
		int32_t i32I = int32_t( _fX * LSM_RCPPI );	// 0.31830988618379067153776752674503 = 1 / PI.
		_fX = (_fX - float( i32I ) * LSM_PI);

		float fX2 = _fX * _fX;

		// Average error:
		//	0.00000079472732583333333333333333333333
		// Max error:
		//	0.00000262260437
		return (i32I & 1) ?
			float( -9.99999463558197021484e-01 ) -
				fX2 * (float( -4.99995589256286621094e-01 ) +
				fX2 * (float( 4.16610352694988250732e-02 ) +
				fX2 * (float( -1.38627504929900169373e-03 ) +
				fX2 * (float( 2.42532332777045667171e-05 ) +
				fX2 * float( -2.21941789391166821588e-07 ))))) :
			float( 9.99999463558197021484e-01 ) +
				fX2 * (float( -4.99995589256286621094e-01 ) +
				fX2 * (float( 4.16610352694988250732e-02 ) +
				fX2 * (float( -1.38627504929900169373e-03 ) +
				fX2 * (float( 2.42532332777045667171e-05 ) +
				fX2 * float( -2.21941789391166821588e-07 )))));
	}

	/**
	 * Float sine and cosine at once.
	 */
	LSE_INLINE void LSE_FCALL CMathLib::SinCosF_Medium( float /*_fA*/, float &/*_fSine*/, float &/*_fCosine*/ ) {
	}

	/**
	 * Tangent.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Tan( LSREAL _fX ) {
		return LSMTAN( _fX );
	}

	/**
	 * Arc sine.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::ASin( LSREAL _fX ) {
		if ( _fX <= static_cast<LSREAL>(-1.0) ) { return -LSM_HALFPI; }
		return (_fX >= LSM_ONE) ? LSM_HALFPI : LSMASIN( _fX );
	}

	/**
	 * Arc cosine.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::ACos( LSREAL _fX ) {
		if ( _fX <= static_cast<LSREAL>(-1.0) ) { return LSM_PI; }
		return (_fX >= LSM_ONE) ? LSM_ZERO : LSMACOS( _fX );
	}

	/**
	 * Arc tangent.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::ATan( LSREAL _fX ) {
		return LSMATAN( _fX );
	}

	/**
	 * Arc tangent.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::ATan( LSREAL _fY, LSREAL _fX ) {
		return LSMATAN2F( _fY, _fX );
	}

	/**
	 * _fX raised to the power of fY.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Pow( LSREAL _fX, LSREAL _fY ) {
		return LSMPOW( _fX, _fY );
	}

	/**
	 * E raised to the power of _fX.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Exp( LSREAL _fX ) {
		return LSMEXP( _fX );
	}

	/**
	 *  LOGy( X ).  Determines by what exponent _fX was raised to get _fY result.
	 *
	 * \param _fX The base.
	 * \param _fY The result.
	 * \return Returns LOGy( X ).
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Log( LSREAL _fX, LSREAL _fY ) {
		return LSMLOG( _fX ) / LSMLOG( _fY );
	}

	/**
	 * Absolute value of _fX.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Abs( LSREAL _fX ) {
#if LSM_BASETYPE == LSM_FIXED
		return LSREAL::CFixedRawType( (_fX.GetRawValue() & LSREAL::SignMask()) ? (0 - _fX.GetRawValue()) : _fX.GetRawValue() );
#else	// #if LSM_BASETYPE == LSM_FIXED
		return LSMABS( _fX );
#endif	// #if LSM_BASETYPE == LSM_FIXED
	}

	/**
	 * Absolute value of _fX.
	 */
	template <typename _tType>
	_tType LSE_FCALL CMathLib::AbsT( _tType _fX ) {
		return _fX < static_cast<_tType>(0.0) ? -_fX : _fX;
	}

	/**
	 * Test two numbers for having the same sign.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CMathLib::SameSign( LSREAL _fLeft, LSREAL _fRight ) {
#if LSM_BASETYPE == LSM_FLOAT
		return ((*reinterpret_cast<uint32_t *>(&_fLeft)) & 0x80000000) ==
			((*reinterpret_cast<uint32_t *>(&_fRight)) & 0x80000000);
#elif LSM_BASETYPE == LSM_DOUBLE
		return ((*reinterpret_cast<uint64_t *>(&_fLeft)) & 0x8000000000000000ULL) ==
			((*reinterpret_cast<uint64_t *>(&_fRight)) & 0x8000000000000000ULL);
#elif LSM_BASETYPE == LSM_FIXED
		return (_fLeft.GetRawValue() & LSREAL::SignMask()) == (_fRight.GetRawValue() & LSREAL::SignMask());
#else	// #if LSM_BASETYPE == LSM_FLOAT
		return (_fLeft >= LSM_ZERO && _fRight >= LSM_ZERO) ||
			(_fLeft < LSM_ZERO && _fRight < LSM_ZERO);
#endif	// #if LSM_BASETYPE == LSM_FLOAT
	}

	/**
	 * Floating-point modulus.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::FMod( LSREAL _fX, LSREAL _fMod ) {
#if LSM_BASETYPE == LSM_FIXED
		return LSREAL::CFixedRawType( _fX.GetRawValue() % _fMod.GetRawValue() );
#else	// #if LSM_BASETYPE == LSM_FIXED
		return LSMFMOD( _fX, _fMod );
#endif	// #if LSM_BASETYPE == LSM_FIXED
	}

	/**
	 * Nearest integer.
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Floor( LSREAL _fX ) {
		return LSMFLOOR( _fX );
	}

	/**
	 * Nearest integer upwards.
	 *
	 * \param _fX The value to round upwards.
	 * \return Returns ceil( X ).
	 */
	LSE_INLINE LSREAL LSE_FCALL CMathLib::Ceil( LSREAL _fX ) {
		return LSMCEIL( _fX );
	}

	/**
	 * Max between two numbers.
	 */
	template <typename _tType>
	_tType LSE_FCALL CMathLib::Max( _tType _fX, _tType _fY ) {
		return _fX > _fY ? _fX : _fY;
	}

	/**
	 * Min between two numbers.
	 */
	template <typename _tType>
	_tType LSE_FCALL CMathLib::Min( _tType _fX, _tType _fY ) {
		return _fX < _fY ? _fX : _fY;
	}

	/**
	 * Swap two numbers.
	 *
	 * \param _fLeft The left operand.
	 * \param _fRight The right operand.
	 */
	template <typename _tType>
	void LSE_FCALL CMathLib::Swap( LSE_UNALIGNED _tType &_fLeft, LSE_UNALIGNED _tType &_fRight ) {
		_tType fTemp = _fLeft;
		_fLeft = _fRight;
		_fRight = fTemp;
	}

	/**
	 * Clamp a value between two others.
	 *
	 * \param _fValue The value to be clamped.
	 * \param _fLow Lower limit for the value to clamp, inclusive.
	 * \param _fHigh Upper limit for the value to clamp, inclusive.
	 * \return Returns the clamped value.
	 */
	template <typename _tType>
	_tType LSE_FCALL CMathLib::Clamp( _tType _fValue, _tType _fLow, _tType _fHigh ) {
		if ( _fValue < _fLow ) { return _fLow; }
		return (_fValue > _fHigh) ? _fHigh : _fValue;
	}

	/**
	 * Compare values for equality using an epsilon factor.
	 *
	 * \param _fX The left value to compare using epsilon.
	 * \param _fY The right value to compare using epsilon.
	 * \param _fEpsilon The epsilon to use for the comparison.
	 * \return Returns true if the numbers are equal given an epsilon value.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CMathLib::Equals( LSREAL _fX, LSREAL _fY, LSREAL _fEpsilon ) {
		LSREAL fMax = CMathLib::Max( CMathLib::Abs( _fX ), CMathLib::Abs( _fY ) );
		return CMathLib::Abs( _fX - _fY ) <= _fEpsilon * CMathLib::Max( fMax, LSM_ONE );
	}

	/**
	 * Compare values for equality using an epsilon factor.
	 *
	 * \param _fX The left value to compare using epsilon.
	 * \param _fY The right value to compare using epsilon.
	 * \param _fEpsilon The epsilon to use for the comparison.
	 * \return Returns true if the numbers are equal given an epsilon value.
	 */
	template <typename _tType>
	LSBOOL LSE_FCALL CMathLib::EqualsT( _tType _fX, _tType _fY, _tType _fEpsilon ) {
		_tType fMax = CMathLib::Max<_tType>( CMathLib::AbsT( _fX ), CMathLib::AbsT( _fY ) );
		return CMathLib::AbsT( _fX - _fY ) <= _fEpsilon * CMathLib::Max<_tType>( fMax, _tType( 1 ) );
	}

}	// namespace lsm

#pragma warning( pop )

#endif	// __LSM_MATHLIB_H__
