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
 * Description: Defines the lexer base class for the shader preprocessor by #define'ing yyFlexLexer before
 *	including LSGPreProcLexBase.h.
 */


#ifndef __LSG_PREPROCLEXER_H__
#define __LSG_PREPROCLEXER_H__

#include "../LSGGraphicsLib.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer			CPreProcLexBase
#include "../PreProcGen/LSGPreProcLexBase.h"

namespace lsg {

	/**
	 * Class CPreProcLexer
	 * \brief Defines the lexer base class for the shader preprocessor by #define'ing yyFlexLexer before
	 *	including LSGPreProcLexBase.h.
	 *
	 * Description: Defines the lexer base class for the shader preprocessor by #define'ing yyFlexLexer before
	 *	including LSGPreProcLexBase.h.
	 */
	class CPreProcLexer : public CPreProcLexBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CPreProcLexer( std::istream * _pisStream, std::ostream * _posStream = NULL );


		// == Functions.
		/**
		 * Perform the work of scanning input for tokens.
		 *
		 * \return Returns the next token.
		 */
		virtual uint32_t LSE_CALL		NewLex();


	protected :
		// == Members.


	private :
		typedef CPreProcLexBase			Parent;
	};

}	// namespace lsg

#endif	// __LSG_PREPROCLEXER_H__
