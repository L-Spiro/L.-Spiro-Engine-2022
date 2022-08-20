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
 * Description: Atomic operations for Windows.
 */


#ifndef __LSSTD_WINDOWSATOMIC_H__
#define __LSSTD_WINDOWSATOMIC_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_WINDOWS

#include "../Windows/LSSTDWindows.h"

namespace lsstd {

	/**
	 * Class CWindowsAtomic
	 * \brief Atomic operations for Windows.
	 *
	 * Description: Atomic operations for Windows.
	 */
	class CWindowsAtomic {
	public :
		// == Types.
		/** The operand type. */
		typedef LONG								ATOM_OP;

		/** The atom type. */
		typedef LSE_ALIGN( 16 ) volatile ATOM_OP	ATOM LSE_POSTALIGN( 16 );


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

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** The CAtomic type. */
	typedef CWindowsAtomic							CAtomic;

	// == Functions.
	/**
	 * Performs an atomic addition of two 32-bit values.
	 *
	 * \param _aAddend A reference to the first operand.  This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedAdd( ATOM &_aAddend, ATOM_OP _aoValue ) {
		return ::InterlockedExchangeAdd( &_aAddend, _aoValue );
	}

	/**
	 * Performs an atomic subtraction of two 32-bit values.
	 *
	 * \param _aAddend A reference to a variable.  The value of this variable is replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedSubtract( ATOM &_aAddend, ATOM_OP _aoValue ) {
		return ::InterlockedExchangeAdd( &_aAddend, -_aoValue );
	}

	/**
	 * Increments (increases by one) the value of the specified 32-bit variable as an atomic operation.
	 *
	 * \param _aAddend A reference to the variable to be incremented.
	 * \return Returns the resulting incremented value.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedIncrement( ATOM &_aAddend ) {
		return ::InterlockedIncrement( &_aAddend );
	}

	/**
	 * Decrements (decreases by one) the value of the specified 32-bit variable as an atomic operation.
	 *
	 * \param _aAddend A reference to the variable to be decremented.
	 * \return Returns the resulting decremented value.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedDecrement( ATOM &_aAddend ) {
		return ::InterlockedDecrement( &_aAddend );
	}

	/**
	 * Performs an atomic AND operation on the specified ATOM values.
	 *
	 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedAnd( ATOM &_aDestination, ATOM_OP _aoValue ) {
		ATOM_OP aoOld;

		do {
			aoOld = _aDestination;
		} while ( ::InterlockedCompareExchange( &_aDestination, aoOld & _aoValue, aoOld ) != aoOld );

		return aoOld;
	}

	/**
	 * Performs an atomic OR operation on the specified ATOM values.  The function prevents more than one thread from using the same variable simultaneously.
	 *
	 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedOr( ATOM &_aDestination, ATOM_OP _aoValue ) {
		ATOM_OP aoOld;

		do {
			aoOld = _aDestination;
		} while ( ::InterlockedCompareExchange( &_aDestination, aoOld | _aoValue, aoOld ) != aoOld );

		return aoOld;
	}

	/**
	 * Performs an atomic XOR operation on the specified ATOM values.  The function prevents more than one thread from using the same variable simultaneously.
	 *
	 * \param _aDestination A reference to the first operand. This value will be replaced with the result of the operation.
	 * \param _aoValue The second operand.
	 * \return Returns the original value of the _aDestination parameter.
	 */
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedXor( ATOM &_aDestination, ATOM_OP _aoValue ) {
		ATOM_OP aoOld;

		do {
			aoOld = _aDestination;
		} while ( ::InterlockedCompareExchange( &_aDestination, aoOld ^ _aoValue, aoOld ) != aoOld );

		return aoOld;
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
	LSE_INLINE CWindowsAtomic::ATOM LSE_CALL CWindowsAtomic::InterlockedCompareExchange( ATOM &_aDestination, ATOM_OP _aoExchange, ATOM_OP _aoComparand ) {
		return ::InterlockedCompareExchange( &_aDestination, _aoExchange, _aoComparand );
	}

}	// namespace lsstd

#endif	// #ifdef LSE_WINDOWS

#endif	// __LSSTD_WINDOWSATOMIC_H__
