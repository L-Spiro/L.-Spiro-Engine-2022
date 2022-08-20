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
 * Description: Atomic operations using the GCC __sync_* intrinsics.
 */


#ifndef __LSSTD_GCCATOMIC_H__
#define __LSSTD_GCCATOMIC_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_GCC

namespace lsstd {

	/**
	 * Class CGccAtomic
	 * \brief Atomic operations using the GCC __sync_* intrinsics.
	 *
	 * Description: Atomic operations using the GCC __sync_* intrinsics.
	 */
	class CGccAtomic {
	public :
		// == Types.
		/** The operand type. */
		typedef int32_t								ATOM_OP;

		/** The atom type. */
		typedef ATOM_OP								ATOM;

		// == Functions.
		/**
		 * Performs an atomic addition of two 32-bit values.
		 *
		 * \param _aAddend A reference to the first operand.  This value will be replaced with the result of the operation.
		 * \param _aoValue The second operand.
		 * \return Returns the original value of the _aDestination parameter.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedAdd( ATOM &_aAddend, ATOM_OP _aoValue );

		/**
		 * Performs an atomic subtraction of two 32-bit values.
		 *
		 * \param _aAddend A reference to a variable.  The value of this variable is replaced with the result of the operation.
		 * \param _aoValue The second operand.
		 * \return Returns the original value of the _aDestination parameter.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedSubtract( ATOM &_aAddend, ATOM_OP _aoValue );

		/**
		 * Increments (increases by one) the value of the specified 32-bit variable as an atomic operation.
		 *
		 * \param _aAddend A reference to the variable to be incremented.
		 * \return Returns the resulting incremented value.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedIncrement( ATOM &_aAddend );

		/**
		 * Decrements (decreases by one) the value of the specified 32-bit variable as an atomic operation.
		 *
		 * \param _aAddend A reference to the variable to be decremented.
		 * \return Returns the resulting decremented value.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedDecrement( ATOM &_aAddend );

		/**
		 * Performs an atomic AND operation on the specified ATOM values.
		 *
		 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
		 * \param _aoValue The second operand.
		 * \return Returns the original value of the _aDestination parameter.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedAnd( ATOM &_aDestination, ATOM_OP _aoValue );

		/**
		 * Performs an atomic OR operation on the specified ATOM values.  The function prevents more than one thread from using the same variable simultaneously.
		 *
		 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
		 * \param _aoValue The second operand.
		 * \return Returns the original value of the _aDestination parameter.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedOr( ATOM &_aDestination, ATOM_OP _aoValue );

		/**
		 * Performs an atomic XOR operation on the specified ATOM values.  The function prevents more than one thread from using the same variable simultaneously.
		 *
		 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
		 * \param _aoValue The second operand.
		 * \return Returns the original value of the _aDestination parameter.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedXor( ATOM &_aDestination, ATOM_OP _aoValue );

		/**
		 * Performs an atomic compare-and-exchange operation on the specified values.  The function compares two specified 32-bit values and exchanges with another
		 *	32-bit value based on the outcome of the comparison.
		 *
		 * \param _aDestination A reference to the destination value.
		 * \param _aoExchange The exchange value.
		 * \param _aoComparand The value to compare to _aDestination.
		 * \return Returns the original value of the _aDestination parameter.
		 */
		static LSE_INLINE ATOM LSE_CALL				InterlockedCompareExchange( ATOM &_aDestination, ATOM_OP _aoExchange, ATOM_OP _aoComparand );


	protected :
		// == Members.
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** The CAtomic type. */
	typedef CGccAtomic								CAtomic;

	// == Functions.
	/**
	 * Performs an atomic addition of two 32-bit values.
	 *
	 * \param _aAddend A reference to the first operand.  This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedAdd( ATOM &_aAddend, ATOM_OP _aoValue ) {
		return __sync_fetch_and_add( &_aAddend, _aoValue );
	}

	/**
	 * Performs an atomic subtraction of two 32-bit values.
	 *
	 * \param _aAddend A reference to a variable.  The value of this variable is replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedSubtract( ATOM &_aAddend, ATOM_OP _aoValue ) {
		return __sync_fetch_and_sub( &_aAddend, _aoValue );
	}

	/**
	 * Increments (increases by one) the value of the specified 32-bit variable as an atomic operation.
	 *
	 * \param _aAddend A reference to the variable to be incremented.
	 * \return Returns the resulting incremented value.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedIncrement( ATOM &_aAddend ) {
		return InterlockedAdd( _aAddend, 1 ) + 1;
	}

	/**
	 * Decrements (decreases by one) the value of the specified 32-bit variable as an atomic operation.
	 *
	 * \param _aAddend A reference to the variable to be decremented.
	 * \return Returns the resulting decremented value.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedDecrement( ATOM &_aAddend ) {
		return InterlockedSubtract( _aAddend, 1 ) - 1;
	}

	/**
	 * Performs an atomic AND operation on the specified ATOM values.
	 *
	 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedAnd( ATOM &_aDestination, ATOM_OP _aoValue ) {
		return __sync_fetch_and_and( &_aDestination, _aoValue );
	}

	/**
	 * Performs an atomic OR operation on the specified ATOM values.  The function prevents more than one thread from using the same variable simultaneously.
	 *
	 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedOr( ATOM &_aDestination, ATOM_OP _aoValue ) {
		return __sync_fetch_and_or( &_aDestination, _aoValue );
	}

	/**
	 * Performs an atomic XOR operation on the specified ATOM values.  The function prevents more than one thread from using the same variable simultaneously.
	 *
	 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedXor( ATOM &_aDestination, ATOM_OP _aoValue ) {
		return __sync_fetch_and_xor( &_aDestination, _aoValue );
	}

	/**
	 * Performs an atomic compare-and-exchange operation on the specified values.  The function compares two specified 32-bit values and exchanges with another
	 *	32-bit value based on the outcome of the comparison.
	 *
	 * \param _aDestination A reference to the destination value.
	 * \param _aoExchange The exchange value.
	 * \param _aoComparand The value to compare to _aDestination.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CGccAtomic::ATOM LSE_CALL CGccAtomic::InterlockedCompareExchange( ATOM &_aDestination, ATOM_OP _aoExchange, ATOM_OP _aoComparand ) {
		return __sync_val_compare_and_swap( &_aDestination, _aoExchange, _aoComparand );
	}

}	// namespace lsstd

#endif	// #ifdef LSE_GCC

#endif	// __LSSTD_GCCATOMIC_H__
