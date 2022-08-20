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
 * Description: Translates from the engine shader language to GLSL ES.
 */


#ifndef __LSG_OPENGLESSHADERTRANSLATOR_H__
#define __LSG_OPENGLESSHADERTRANSLATOR_H__

#include "../LSGGraphicsLib.h"
#include "../ShaderLanguage/LSGShaderTranslationProcessorBase.h"

#if defined( LSG_OGLES )

namespace lsg {

	/**
	 * Class COpenGlEsShaderTranslator
	 * \brief Translates from the engine shader language to GLSL ES.
	 *
	 * Description: Translates from the engine shader language to GLSL ES.
	 */
	class COpenGlEsShaderTranslator : public CShaderTranslationProcessorBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR										COpenGlEsShaderTranslator();
		virtual LSE_CALLCTOR								~COpenGlEsShaderTranslator();


	protected :
		// == Members.
		/**
		 * Is the next parameter declaration list part of the Main()?
		 */
		LSBOOL												m_bIsMainNow;


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
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PARMDECLARATOR.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleParamaterDeclarator( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_SINGLEDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FULLYSPECIFIEDTYPE.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFullySpecifiedType( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_INTRINSIC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleIntrinsic( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_CAST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleCast( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_IDENTIFIER.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleIdentifier( LSG_NODE_CALLBACK &_ncData );

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

		/**
		 * Decode a TEXCOORD value to a string for vertex shaders as IN.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \param _ui32ParmNodeIndex Index of the parameter node.
		 * \param _ui32Index TEXCOORD index.
		 * \param _sRet The return string.
		 * \param _sError The error string in case of error.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating success or failure. 
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			VertexInTexCoord( LSG_NODE_CALLBACK &_ncData,
			uint32_t _ui32ParmNodeIndex,
			uint32_t _ui32Index, CString &_sRet, CString &_sError );

		/**
		 * Decode a TEXCOORD value to a string for vertex shaders as OUT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \param _ui32ParmNodeIndex Index of the parameter node.
		 * \param _ui32Index TEXCOORD index.
		 * \param _sRet The return string.
		 * \param _sError The error string in case of error.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating success or failure. 
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			VertexOutTexCoord( LSG_NODE_CALLBACK &_ncData,
			uint32_t _ui32ParmNodeIndex,
			uint32_t _ui32Index, CString &_sRet, CString &_sError );

		/**
		 * Decode a TEXCOORD value to a string for pixel shaders as IN.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \param _ui32ParmNodeIndex Index of the parameter node.
		 * \param _ui32Index TEXCOORD index.
		 * \param _sRet The return string.
		 * \param _sError The error string in case of error.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating success or failure. 
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			PixelInTexCoord( LSG_NODE_CALLBACK &_ncData,
			uint32_t _ui32ParmNodeIndex,
			uint32_t _ui32Index, CString &_sRet, CString &_sError );


	private :
		typedef CShaderTranslationProcessorBase				Parent;

	};

}	// namespace lsg

#endif	// #if defined( LSG_OGLES )

#endif	// __LSG_OPENGLESSHADERTRANSLATOR_H__
