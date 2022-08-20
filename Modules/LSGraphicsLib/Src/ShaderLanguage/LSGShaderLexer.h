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
 * Description: Defines the lexer base class by #define'ing yyFlexLexer before including LSGShaderLexBase.h.
 */


#ifndef __LSG_SHADERLEXER_H__
#define __LSG_SHADERLEXER_H__

#include "../LSGGraphicsLib.h"
#include "LSGShaderLanguageParser.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer			CShaderLexBase
#include "../ShaderLangGen/LSGShaderLexBase.h"

namespace lsg {

	/**
	 * Class CShaderLexer
	 * \brief Defines the lexer base class by #define'ing yyFlexLexer before including LSGShaderLexBase.h.
	 *
	 * Description: Defines the lexer base class by #define'ing yyFlexLexer before including LSGShaderLexBase.h.
	 */
	class CShaderLexer : public CShaderLexBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CShaderLexer( std::istream * _pisStream, std::ostream * _posStream = NULL );
		virtual LSE_CALLCTOR			~CShaderLexer();


		// == Functions.
		/**
		 * Perform the work of scanning input for tokens.
		 *
		 * \return Returns the next token.
		 */
		virtual uint32_t LSE_CALL		NewLex();

		/**
		 * Sets the container.  This is used to access everything the lexer needs about the current state of the
		 *	compiled data.
		 *
		 * \param _slpParserContainer Reference to the container to be used by this lexer.
		 */
		void LSE_CALL					SetContainer( const CShaderLanguageParser &_slpParserContainer );


	protected :
		// == Members.
		/**
		 * The container that handles collection of all of the script data.
		 */
		const CShaderLanguageParser *	m_pslpContainer;

	private :
		typedef CShaderLexBase			Parent;
	};

}	// namespace lsg {

#endif	// __LSG_SHADERLEXER_H__
