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


#ifndef __LSG_SHADERPARSERWRAPPER_H__
#define __LSG_SHADERPARSERWRAPPER_H__

#include "../LSGGraphicsLib.h"
#include "LSGShaderSyntaxNodes.h"		// Not alphabetical; must be included before LSGShaderParser.h.
#include "../ShaderLangGen/LSGShaderParser.h"
#include "LSGShaderLanguageParser.h"

namespace lsg {

	/**
	 * Class CShaderParserWrapper
	 * \brief Wrapper for the Bison generated syntax parser of the engine script language.
	 *
	 * Description: Wrapper for the Bison generated syntax parser of the engine script language.
	 */
	class CShaderParserWrapper : public lsg::CShaderParser {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CShaderParserWrapper( class CShaderLexer * _pslLexer, LSG_SHADER_TYPES _stShaderType );


		// == Functions.
		/**
		 * Gets the parser container.  The container is the primary work object behind parsing
		 *	the shader file.  It stores all the data from the read file and everything needed
		 *	to create the final product (which varies per platform).
		 *
		 * \return Returns the parser container.
		 */
		CShaderLanguageParser & LSE_CALL	GetContainer();


	protected :
		// == Members.
		/**
		 * The container of the shader data during compilation/parsing.
		 */
		CShaderLanguageParser				m_slpContainer;



	private :
		typedef lsg::CShaderParser			Parent;
	};

}	// namespace lsg

#endif	// __LSG_SHADERPARSERWRAPPER_H__
