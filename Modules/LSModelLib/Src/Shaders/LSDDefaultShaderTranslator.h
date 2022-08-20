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
 * Description: The default system shader-key translator.
 */


#ifndef __LSD_DEFAULTSHADERTRANSLATOR_H__
#define __LSD_DEFAULTSHADERTRANSLATOR_H__

#include "../LSDModelLib.h"
#include "LSDShaderKeyTranslator.h"

namespace lsd {

	/**
	 * Class CDefaultShaderTranslator
	 * \brief The default system shader-key translator.
	 *
	 * Description: The default system shader-key translator.
	 */
	class CDefaultShaderTranslator {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Gets a constant reference to the default shader-key translator structure.
		 *
		 * \return Returns a constant reference to the default shader-key translator structure.
		 */
		static const CShaderKeyTranslator::LSD_SHADER_TRANSLATOR & LSE_CALL
															Translator();

		/**
		 * The default function for translating texture inputs (connecting input textures to UV's).
		 *
		 * \param _skKey The key to parse.
		 * \param _vDefines Holds generated macros.
		 * \return Returns true if there was enough memory to add all necessary macros to the dictionary.
		 */
		static LSBOOL LSE_CALL								DefaultTextureTranslator( const LSD_SHADERKEY &_skKey,
			CVector<CShaderKeyTranslator::LSD_SHADER_DEFINE> &_vDefines );


	protected :
		// == Members.
		/** The array of default functions. */
		static CShaderKeyTranslator::PfTransFunc			m_tfFuncs[];

		/** The structure for creating actual shader-key translators. */
		static CShaderKeyTranslator::LSD_SHADER_TRANSLATOR	m_stTranslator;
	};

}	// namespace lsd

#endif	// __LSD_DEFAULTSHADERTRANSLATOR_H__
