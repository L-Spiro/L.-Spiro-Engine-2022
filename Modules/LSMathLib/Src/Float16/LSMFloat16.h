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


#ifndef __LSM_FLOAT16_H__
#define __LSM_FLOAT16_H__

#include "../LSMMathLib.h"

namespace lsm {

	/**
	 * Class CFloat16
	 * \brief A basic class for converting to and from 16-bit floating-point values.
	 *
	 * Description: A basic class for converting to and from 16-bit floating-point values.  Does not provide
	 *	operators for working directly on 16-bit floats.
	 */
	LSE_ALIGN( 2 )
	class CFloat16 {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CFloat16();
		LSE_CALLCTOR					CFloat16( float _fValue );
		LSE_CALLCTOR					CFloat16( const CFloat16 &_f16Value );


		// == Operators.
		/**
		 * Converts to a 32-bit floating-point number.
		 *
		 * \return Returns the 32-bit floating-point representation of this number.
		 */
		LSE_CALL						operator float() const;


		// == Functions.
		/**
		 * Gets the 16-bit integer representation of the value.
		 *
		 * \return Returns the 16-bit integer representation of the value.
		 */
		uint16_t LSE_CALL				GetInt() const;


	protected :
		// == Types.
		/** A float as a singned or unsigned integer (32 bits). */
		typedef union LSM_FLOAT_INT {
			/** Value as a float. */
			float						fValue;

			/** Value as a signed integer. */
			int32_t						i32Value;

			/** Value as an unsigned integer. */
			uint32_t					ui32Value;
		} * LPLSM_FLOAT_INT, * const LPCLSM_FLOAT_INT;


		// == Members.
		/** This value. */
		uint16_t						m_ui16Value;


		// == Functions.
		/**
		 * Converts from a 32-bit floating-point number to a 16-bit floating-point number.
		 *
		 * \param _fValue The value to convert.
		 */
		void LSE_CALL					FromFloat( float _fValue );

		/**
		 * Converts from a 16-bit floating-point number to a 32-bit floating-point number.
		 *
		 * \return Returns the 32-bit converted number.
		 */
		float LSE_CALL					ToFloat() const;
	} LSE_POSTALIGN( 2 );


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the 16-bit integer representation of the value.
	 *
	 * \return Returns the 16-bit integer representation of the value.
	 */
	LSE_INLINE uint16_t LSE_CALL CFloat16::GetInt() const {
		return m_ui16Value;
	}

}	// namespace lsm

#endif	// __LSM_FLOAT16_H__
