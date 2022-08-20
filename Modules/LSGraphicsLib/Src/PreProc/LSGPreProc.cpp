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

#include "LSGPreProc.h"
#include "LSGPreProcLexer.h"
#include <sstream>


namespace lsg {

	// == Operators.
	/**
	 * Copy operator.
	 *
	 * \param _pdOther The define to copy.
	 * \return Returns this object after the copy.
	 */
	CPreProc::LSG_PREPROC_DEFINE & LSE_CALL CPreProc::LSG_PREPROC_DEFINE::operator = ( const LSG_PREPROC_DEFINE &_pdOther ) {
		sName = _pdOther.sName;
		sParms = _pdOther.sParms;
		return (*this);
	}

	/**
	 * Compare this object against other.  Returns true if the name of this object comes before
	 *	the name of the given object.
	 *
	 * \param _pdOther The object against which to compare this object.
	 * \return Returns true if the name of this object comes before the name of the given object.
	 */
	LSBOOL LSE_CALL CPreProc::LSG_PREPROC_DEFINE::operator < ( const LSG_PREPROC_DEFINE &_pdOther ) const {
		return sName < _pdOther.sName;
	}

	/**
	 * Check for equality between this object and another.
	 *
	 * \param _pdOther The object against which to check for equality.
	 * \return Returns true if the names of both objects are the same.
	 */
	LSBOOL LSE_CALL CPreProc::LSG_PREPROC_DEFINE::operator == ( const LSG_PREPROC_DEFINE &_pdOther ) const {
		return sName == _pdOther.sName;
	}

	// == Various constructors.
	LSE_CALLCTOR CPreProc::CPreProc() {
	}

	// == Functions.
	/**
	 * Gets the container.
	 *
	 * \return Returns the container.
	 */
	CPreProcContainer & LSE_CALL CPreProc::GetContainer() {
		return m_ppcContainer;
	}

	/**
	 * Parse an expression, returning an error code.
	 *
	 * \param _sExp The expression to parse.
	 * \return Returns an error code depending on the result of the expression.  LSG_E_ERROR is returned
	 *	if the expression is bad.  LSG_E_FAILURE indicates the expression is valid, but evaluates to 0.
	 *	LSG_E_SUCCESS indicates that the expression is valid and evaluates to 1.
	 */
	CPreProc::LSG_ERRORS LSE_CALL CPreProc::Parse( const CString &_sExp ) {
		std::string strShaderText( _sExp.CStr(), _sExp.Length() );
		std::istringstream isStream( strShaderText );

		// Streams created.  Create the parsers.
		CPreProcLexer * ppplLexer = LSENEW CPreProcLexer( &isStream, NULL );
		if ( !ppplLexer ) {
			return LSG_E_ERROR;
		}
		CPreProcParser * ppppParser = LSENEW CPreProcParser( ppplLexer, &m_ppcContainer );
		if ( !ppppParser ) {
			LSEDELETE ppplLexer;
			return LSG_E_ERROR;
		}


		if ( ppppParser->parse() == 0 ) {
			// Parsed.

			LSEDELETE ppppParser;
			LSEDELETE ppplLexer;
			return LSG_E_SUCCESS;
		}
		LSEDELETE ppppParser;
		LSEDELETE ppplLexer;
		return LSG_E_ERROR;
	}

	/**
	 * Evaluate the parsed expression with the given macro dictionary.
	 *
	 * \param _mMacros The macros.
	 * \param _bRet Holds the result of the expression.
	 * \return Returns true if the expression was successfully evaluated.
	 */
	LSBOOL LSE_CALL CPreProc::GetResult( const LSG_MACROS &_mMacros, LSBOOL &_bRet ) const {
		int64_t i64Ret;
		if ( !EvalNode( m_ppcContainer.TotalNodes() - 1, i64Ret, _mMacros ) ) { return false; }
		_bRet = i64Ret != 0ULL;
		return true;
	}

	/**
	 * Process a single node and return the result of that node.
	 *
	 * \param _ui32Index The index of the node to process.
	 * \param _i64Return Holds the return value of the node.
	 * \param _mMacros The macros.
	 * \return Returns true if the node was evaluated.
	 */
	LSBOOL LSE_CALL CPreProc::EvalNode( uint32_t _ui32Index, int64_t &_i64Return, const LSG_MACROS &_mMacros ) const {
		const LSG_PREPROC_SYNTAX_NODES::LSG_NODE_DATA & ndData = m_ppcContainer.GetNode( _ui32Index );
		switch ( ndData.ppnNodeType ) {
			case LSG_PPN_NUMERICCONSTANT : {
				_i64Return = ndData.nuNodeData.i64Const;
				return true;
			}
			case LSG_PPN_IDENTIFIER : {
				// Should be in the macro dictionary.
				LSG_PREPROC_DEFINE pdThis;
				pdThis.sName = m_ppcContainer.GetString( ndData.nuNodeData.ui32IdentifierIndex );
				uint32_t ui32Index;
				if ( !_mMacros.ItemExists( pdThis, ui32Index ) ) {
					_i64Return = 0LL;
				}
				else {
					if ( _mMacros.GetKeyByIndex( ui32Index ).sParms.Length() ) {
						_i64Return = 0LL;
					}
					else if ( _mMacros.GetByIndex( ui32Index ).Length() == 0 ) {
						return false;
					}
					else {
						_i64Return = CStd::AtoI64( _mMacros.GetByIndex( ui32Index ).CStr() );
					}
				}
				return true;
			}
			case LSG_PPN_UNARY : {
				if ( !EvalNode( ndData.nueNodeDataEx.ui32NodeIndexEx, _i64Return, _mMacros ) ) { return false; }
				switch ( ndData.nuNodeData.ui32UnaryOp ) {
					case '!' : {
						_i64Return = !_i64Return;
						break;
					}
					case '~' : {
						_i64Return = ~_i64Return;
						break;
					}
					case '-' : {
						_i64Return = -_i64Return;
						break;
					}
					case '+' : {
						_i64Return = +_i64Return;
						break;
					}
				}

				return true;
			}
			case LSG_PPN_MATH : {
				int64_t i64Left, i64Right;
				if ( !EvalNode( ndData.nuNodeData.ui32NodeIndex, i64Left, _mMacros ) ) { return false; }
				if ( !EvalNode( ndData.nueNodeDataEx.ui32NodeIndexEx, i64Right, _mMacros ) ) { return false; }
#define LSG_HELP_ME_HERE( CASE, OP )		case CASE : { _i64Return = i64Left OP i64Right; break; }
				switch ( ndData.nuoOp.ui32Op ) {
					LSG_HELP_ME_HERE( '*', * )
					LSG_HELP_ME_HERE( '/', / )
					LSG_HELP_ME_HERE( '%', % )
					LSG_HELP_ME_HERE( '+', + )
					LSG_HELP_ME_HERE( '-', - )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_SHIFT_RIGHT, >> )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_SHIFT_LEFT, << )
					LSG_HELP_ME_HERE( '>', > )
					LSG_HELP_ME_HERE( '<', < )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_GE, >= )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_LE, <= )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_EQUALS, == )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_NOT_EQUALS, != )
					LSG_HELP_ME_HERE( '|', | )
					LSG_HELP_ME_HERE( '&', & )
					LSG_HELP_ME_HERE( '^', ^ )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_AND, && )
					LSG_HELP_ME_HERE( CPreProcParser::token::LSG_PPT_OR, || )
					default : { return false; }
				}
				return true;
			}
			case LSG_PPN_TERTIARY : {
				// Condition.
				if ( !EvalNode( ndData.nuNodeData.ui32NodeIndex, _i64Return, _mMacros ) ) { return false; }
				if ( _i64Return ) {
					// Left.
					return EvalNode( ndData.nueNodeDataEx.ui32NodeIndexEx, _i64Return, _mMacros );
				}
				else {
					return EvalNode( ndData.nueNodeDataEx.ui32NodeIndexEx, _i64Return, _mMacros );
				}
			}
			case LSG_PPN_DEFINED : {
				// Should be in the macro dictionary.
				LSG_PREPROC_DEFINE pdThis;
				const LSG_PREPROC_SYNTAX_NODES::LSG_NODE_DATA & ndString = m_ppcContainer.GetNode( ndData.nuNodeData.ui32NodeIndex );
				pdThis.sName = m_ppcContainer.GetString( ndString.nuNodeData.ui32IdentifierIndex );
				uint32_t ui32Index;
				if ( !_mMacros.ItemExists( pdThis, ui32Index ) ) {
					_i64Return = 0LL;
				}
				else {
					_i64Return = 1LL;
				}
				return true;
			}
			case LSG_PPN_TU : { return false; }
		}
		return false;
	}

}	// namespace lsg
