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
 * Description: Parses a single expression related to #if and #elif.
 */


#ifndef __LSG_PREPROC_H__
#define __LSG_PREPROC_H__

#include "../LSGGraphicsLib.h"
#include "LSGPreProcSyntaxNodes.h"		// Must be included before LSGPreProcParser.h.
#include "../PreProcGen/LSGPreProcParser.h"
#include "LSGPreProcContainer.h"
#include "Map/LSTLLinearMap.h"

namespace lsg {

	/**
	 * Class CPreProc
	 * \brief Parses a single expression related to #if and #elif.
	 *
	 * Description: Parses a single expression related to #if and #elif.
	 */
	class CPreProc {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CPreProc();

		
		// == Enumerations.
		/**
		 * Error codes.
		 */
		enum LSG_ERRORS {
			LSG_E_FAILURE,
			LSG_E_SUCCESS,
			LSG_E_ERROR,
		};


		// == Types.
		/**
		 * A define.
		 */
		typedef struct LSG_PREPROC_DEFINE {
			/**
			 * The name of the define.
			 */
			CString									sName;

			/**
			 * Optional parameters.
			 */
			CVector<CString>						sParms;


			// == Operators.
			/**
			 * Copy operator.
			 *
			 * \param _pdOther The define to copy.
			 * \return Returns this object after the copy.
			 */
			LSG_PREPROC_DEFINE & LSE_CALL			operator = ( const LSG_PREPROC_DEFINE &_pdOther );

			/**
			 * Compare this object against other.  Returns true if the name of this object comes before
			 *	the name of the given object.
			 *
			 * \param _pdOther The object against which to compare this object.
			 * \return Returns true if the name of this object comes before the name of the given object.
			 */
			LSBOOL LSE_CALL							operator < ( const LSG_PREPROC_DEFINE &_pdOther ) const;

			/**
			 * Check for equality between this object and another.
			 *
			 * \param _pdOther The object against which to check for equality.
			 * \return Returns true if the names of both objects are the same.
			 */
			LSBOOL LSE_CALL							operator == ( const LSG_PREPROC_DEFINE &_pdOther ) const;
		} * LPLSG_PREPROC_DEFINE, * const LPCLSG_PREPROC_DEFINE;

		/**
		 * The dictionary.
		 */
		typedef CLinearMap<LSG_PREPROC_DEFINE, CString>	LSG_MACROS;


		// == Functions.
		/**
		 * Gets the container.
		 *
		 * \return Returns the container.
		 */
		CPreProcContainer & LSE_CALL				GetContainer();

		/**
		 * Parse an expression, returning an error code.
		 *
		 * \param _sExp The expression to parse.
		 * \return Returns an error code depending on the result of the expression.  LSG_E_ERROR is returned
		 *	if the expression is bad.  LSG_E_FAILURE indicates the expression is valid, but evaluates to 0.
		 *	LSG_E_SUCCESS indicates that the expression is valid and evaluates to 1.
		 */
		LSG_ERRORS LSE_CALL							Parse( const CString &_sExp );

		/**
		 * Evaluate the parsed expression with the given macro dictionary.
		 *
		 * \param _mMacros The macros.
		 * \param _bRet Holds the result of the expression.
		 * \return Returns true if the expression was successfully evaluated.
		 */
		LSBOOL LSE_CALL								GetResult( const LSG_MACROS &_mMacros, LSBOOL &_bRet ) const;


	protected :
		// == Members.
		/**
		 * The container for holding the parsed data.
		 */
		CPreProcContainer							m_ppcContainer;


		// == Functions.
		/**
		 * Process a single node and return the result of that node.
		 *
		 * \param _ui32Index The index of the node to process.
		 * \param _i64Return Holds the return value of the node.
		 * \param _mMacros The macros.
		 * \return Returns true if the node was evaluated.
		 */
		LSBOOL LSE_CALL								EvalNode( uint32_t _ui32Index, int64_t &_i64Return, const LSG_MACROS &_mMacros ) const;
	};

}	// namespace lsg

#endif	// __LSG_PREPROC_H__

