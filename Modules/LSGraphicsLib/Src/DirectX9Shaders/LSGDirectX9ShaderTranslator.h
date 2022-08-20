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
 * Description: Translates from the engine shader language to HLSL.
 */


#ifndef __LSG_DIRECTX9SHADERTRANSLATOR_H__
#define __LSG_DIRECTX9SHADERTRANSLATOR_H__

#include "../LSGGraphicsLib.h"
#include "../ShaderLanguage/LSGShaderTranslationProcessorBase.h"

#if defined( LSG_DX9 )

namespace lsg {

	/**
	 * Class CDirectX9ShaderTranslator
	 * \brief Translates from the engine shader language to HLSL.
	 *
	 * Description: Translates from the engine shader language to HLSL.
	 */
	class CDirectX9ShaderTranslator : public CShaderTranslationProcessorBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CDirectX9ShaderTranslator();
		virtual LSE_CALLCTOR								~CDirectX9ShaderTranslator();

	
		// == Functions.
	protected :
		// == Members.


		// == Functions.
		/**
		 * Modify the final shader string after it has been completed.  Override to modify it.
		 *
		 * \param _sShader The final string, modifiable in-place.
		 * \param _sError The error string to be filled if there is an error modifying the shader text.
		 * \return Returns false if an error occurred.
		 */
		virtual LSBOOL LSE_CALL								PostShaderString( CString &_sShader, CString &_sError );

		/**
		 * Handles nodes of type LSG_N_FUNCDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PARMDECLARATOR.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		//LSG_PROCESSING_DIRECTIVE LSE_CALL					HandleParamaterDeclarator( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_INITLISTWRAP.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					HandleInitListWrap( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_SINGLEDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_INTRINSIC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					HandleIntrinsic( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_SEMANTIC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					HandleSemantic( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Decode a type qualifier to a given string.
		 *
		 * \param _ui64Qualifier The type qualifier.
		 * \param _sString The string to which to print the data.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		virtual LSBOOL LSE_CALL								DecodeTypeQualifier( uint64_t _ui64Qualifier, CString &_sString );

		/**
		 * Decode a type.
		 *
		 * \param _ui64Type The type information.
		 * \param _sString The string to which to print the data.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		virtual LSBOOL LSE_CALL								DecodeType( uint64_t _ui64Type, CString &_sString );


	private :
		typedef CShaderTranslationProcessorBase				Parent;
	};

}	// namespace lsg

#endif	// #if defined( LSG_DX9 )

#endif	// __LSG_DIRECTX9SHADERTRANSLATOR_H__
