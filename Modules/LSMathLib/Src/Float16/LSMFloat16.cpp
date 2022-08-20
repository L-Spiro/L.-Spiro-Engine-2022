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
 * Description: A basic class for converting to and from 16-bit floating-point values.  Does not provide
 *	operators for working directly on 16-bit floats.
 */


#include "LSMFloat16.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define LSM_SHIFT						13
#define LSM_SHIFT_SIGN					16
#define LSM_INF_N						0x7F800000
#define LSM_MAX_N						0x477FE000
#define LSM_MIN_N						0x38800000
#define LSM_SIGN_N						0x80000000
#define LSM_INF_C						(LSM_INF_N >> LSM_SHIFT)
#define LSM_NAN_N						((LSM_INF_C + 1) << LSM_SHIFT)
#define LSM_MAX_C						(LSM_MAX_N >> LSM_SHIFT)
#define LSM_MIN_C						(LSM_MIN_N >> LSM_SHIFT)
#define LSM_SIGN_C						(LSM_SIGN_N >> LSM_SHIFT_SIGN)
#define LSM_MUL_N						0x52000000
#define LSM_MUL_C						0x33800000
#define LSM_SUB_C						0x003FF
#define LSM_NOR_C						0x00400
#define LSM_MAX_D						(LSM_INF_C - LSM_MAX_C - 1)
#define LSM_MIN_D						(LSM_MIN_C - LSM_SUB_C - 1)



namespace lsm {

	// == Various constructors.
	LSE_CALLCTOR CFloat16::CFloat16() {
	}
	LSE_CALLCTOR CFloat16::CFloat16( float _fValue ) {
		FromFloat( _fValue );
	}
	LSE_CALLCTOR CFloat16::CFloat16( const CFloat16 &_f16Value ) :
		m_ui16Value( _f16Value.m_ui16Value ) {
	}

	// == Operators.
	/**
	 * Converts to a 32-bit floating-point number.
	 *
	 * \return Returns the 32-bit floating-point representation of this number.
	 */
	LSE_CALL CFloat16::operator float() const {
		return ToFloat();
	}

	// == Functions.
	/**
	 * Converts from a 32-bit floating-point number to a 16-bit floating-point number.
	 *
	 * \param _fValue The value to convert.
	 */
	void LSE_CALL CFloat16::FromFloat( float _fValue ) {
		LSM_FLOAT_INT fiVal, fiSub;
        fiVal.fValue = _fValue;
        uint32_t ui32Sign = fiVal.ui32Value & LSM_SIGN_N;
        fiVal.i32Value ^= ui32Sign;
        ui32Sign >>= LSM_SHIFT_SIGN;										// Logical shift.
        fiSub.i32Value = LSM_MUL_N;
        fiSub.i32Value = static_cast<int32_t>(fiSub.fValue * fiVal.fValue);	// Correct subnormals.
        fiVal.i32Value ^= (fiSub.i32Value ^ fiVal.i32Value) & -(LSM_MIN_N > fiVal.i32Value);
        fiVal.i32Value ^= (LSM_INF_N ^ fiVal.i32Value) & -((LSM_INF_N > fiVal.i32Value) & (fiVal.i32Value > LSM_MAX_N));
        fiVal.i32Value ^= (LSM_NAN_N ^ fiVal.i32Value) & -((LSM_NAN_N > fiVal.i32Value) & (fiVal.i32Value > LSM_INF_N));
        fiVal.ui32Value >>= LSM_SHIFT;										// Logical shift.
        fiVal.i32Value ^= ((fiVal.i32Value - LSM_MAX_D) ^ fiVal.i32Value) & -(fiVal.i32Value > LSM_MAX_C);
        fiVal.i32Value ^= ((fiVal.i32Value - LSM_MIN_D) ^ fiVal.i32Value) & -(fiVal.i32Value > LSM_SUB_C);
        m_ui16Value = static_cast<uint16_t>(fiVal.ui32Value | ui32Sign);
	}

	/**
	 * Converts from a 16-bit floating-point number to a 32-bit floating-point number.
	 *
	 * \return Returns the 32-bit converted number.
	 */
	float LSE_CALL CFloat16::ToFloat() const {
		LSM_FLOAT_INT fiVal;
        fiVal.ui32Value = m_ui16Value;
        int32_t i32Sign = fiVal.ui32Value & LSM_SIGN_C;
        fiVal.i32Value ^= i32Sign;
        i32Sign <<= LSM_SHIFT_SIGN;
        fiVal.i32Value ^= ((fiVal.i32Value + LSM_MIN_D) ^ fiVal.i32Value) & -(fiVal.i32Value > LSM_SUB_C);
        fiVal.i32Value ^= ((fiVal.i32Value + LSM_MAX_D) ^ fiVal.i32Value) & -(fiVal.i32Value > LSM_MAX_C);
        LSM_FLOAT_INT fiSub;
        fiSub.i32Value = LSM_MUL_C;
        fiSub.fValue *= fiVal.i32Value;
        int32_t i32Mask = -(LSM_NOR_C > fiVal.i32Value);
        fiVal.ui32Value <<= LSM_SHIFT;
        fiVal.i32Value ^= (fiSub.i32Value ^ fiVal.i32Value) & i32Mask;
        fiVal.i32Value |= i32Sign;
        return fiVal.fValue;
	}

}	// namespace lsm

#undef LSM_MIN_D
#undef LSM_MAX_D
#undef LSM_NOR_C
#undef LSM_SUB_C
#undef LSM_MUL_C
#undef LSM_MUL_N
#undef LSM_SIGN_C
#undef LSM_MIN_C
#undef LSM_MAX_C
#undef LSM_NAN_N
#undef LSM_INF_C
#undef LSM_SIGN_N
#undef LSM_MIN_N
#undef LSM_MAX_N
#undef LSM_INF_N
#undef LSM_SHIFT_SIGN
#undef LSM_SHIFT
