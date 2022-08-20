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
 * Description: Wrapper for the Bison generated syntax parser of the engine script language.
 */

#include "LSGShaderParserWrapper.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CShaderParserWrapper::CShaderParserWrapper( class CShaderLexer * _pslLexer, LSG_SHADER_TYPES _stShaderType ) :
		Parent( _pslLexer, &m_slpContainer ),
		m_slpContainer( _stShaderType, _pslLexer ) {
	}

	// == Functions.
	/**
	 * Gets the parser container.  The container is the primary work object behind parsing
	 *	the shader file.  It stores all the data from the read file and everything needed
	 *	to create the final product (which varies per platform).
	 *
	 * \return Returns the parser container.
	 */
	CShaderLanguageParser & LSE_CALL CShaderParserWrapper::GetContainer() {
		return m_slpContainer;
	}

}	// namespace lsg
