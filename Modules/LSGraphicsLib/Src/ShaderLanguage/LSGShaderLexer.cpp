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

#include "LSGShaderLexer.h"
#include "LSGShaderSyntaxNodes.h"	// Must be included before LSGShaderParser.h.
#include "../ShaderLangGen/LSGShaderParser.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CShaderLexer::CShaderLexer( std::istream * _pisStream, std::ostream * _posStream ) :
		Parent( _pisStream, _posStream ),
		m_pslpContainer( NULL ) {
	}
	LSE_CALLCTOR CShaderLexer::~CShaderLexer() {
	}

	// == Functions.
	/**
	 * Perform the work of scanning input for tokens.
	 *
	 * \return Returns the next token.
	 */
	uint32_t CShaderLexer::NewLex() {
		uint32_t ui32Ret = Parent::yylex();
		if ( ui32Ret == CShaderParser::token::LSG_SLT_IDENTIFIER ) {
			uint32_t ui32Id = m_pslpContainer->GetStructTypeId( YYText() );

			// Check the dictionary.
			return ui32Id ? CShaderParser::token::LSG_SLT_TYPENAME : CShaderParser::token::LSG_SLT_IDENTIFIER;
		}
		return ui32Ret;
	}

	/**
	 * Sets the container.  This is used to access everything the lexer needs about the current state of the
	 *	compiled data.
	 *
	 * \param _slpParserContainer Reference to the container to be used by this lexer.
	 */
	void LSE_CALL CShaderLexer::SetContainer( const CShaderLanguageParser &_slpParserContainer ) {
		m_pslpContainer = &_slpParserContainer;
	}

}	// namespace lsg
