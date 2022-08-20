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
 * Description: Translates from the engine shader language to Metal Shader Language.
 */


#ifndef __LSG_METALSHADERTRANSLATOR_H__
#define __LSG_METALSHADERTRANSLATOR_H__

#include "../LSGGraphicsLib.h"
#include "../ShaderLanguage/LSGShaderTranslationProcessorBase.h"

#if defined( LSG_METAL )

namespace lsg {

	/**
	 * Class CMetalShaderTranslator
	 * \brief Translates from the engine shader language to Metal Shader Language.
	 *
	 * Description: Translates from the engine shader language to Metal Shader Language.
	 */
	class CMetalShaderTranslator : public CShaderTranslationProcessorBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CMetalShaderTranslator();
		virtual LSE_CALLCTOR								~CMetalShaderTranslator();


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
		 * Handles nodes of type LSG_N_DISCARD.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleDiscard( LSG_NODE_CALLBACK &_ncData );

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
		virtual LSBOOL LSE_CALL								DecodeTypeQualifier( LSUINT64 _ui64Qualifier, CString &_sString );

		/**
		 * Decode a type.
		 *
		 * \param _ui64Type The type information.
		 * \param _sString The string to which to print the data.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		virtual LSBOOL LSE_CALL								DecodeType( LSUINT64 _ui64Type, CString &_sString );

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
			LSUINT32 _ui32ParmNodeIndex,
			LSUINT32 _ui32Index, CString &_sRet, CString &_sError );

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
			LSUINT32 _ui32ParmNodeIndex,
			LSUINT32 _ui32Index, CString &_sRet, CString &_sError );

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
			LSUINT32 _ui32ParmNodeIndex,
			LSUINT32 _ui32Index, CString &_sRet, CString &_sError );
		
		/**
		 * Determines the buffer index into which to place the given value based on its semantic.
		 *
		 * \param _ui32Semantic The semantic of the data.
		 * \return Returns the index of the buffer into which to put the data based on semantic.
		 */
		LSUINT32 LSE_CALL									GetBufferId( LSUINT32 _ui32Semantic ) const;
		
		/**
		 * Gets the semantic type and index, or returns false.
		 *
		 * \param _sName Name of the global uniform.
		 * \param _ui32SemanticType Returned semantic type.
		 * \param _ui32SemanticIndex Returned semantic index.
		 * \return Returns true if there was a matching global semantic.
		 */
		LSBOOL LSE_CALL										GetSemanticFromUniform( const CString &_sName, LSUINT32 &_ui32SemanticType, LSUINT32 &_ui32SemanticIndex ) const;
		
		/**
		 * Gets the size and alignment of a given uniform as it would be in a buffer.
		 *
		 * \param _uUniform Uniform whose size and alignment when inside a buffer is to be gotten.
		 * \param _ui32Size Holds the returned size of the type, adjusted for arrays etc.
		 * \param _ui32Align Holds the returned alignment of the type.
		 */
		LSVOID LSE_CALL										GetUniformBufferSizeAndAlign( const LSG_UNIFORM &_uUniform, LSUINT32 &_ui32Size, LSUINT32 &_ui32Align );
		
		/**
		 * Prints a textureX to a given string (appending it).
		 *
		 * \param _uUniform Uniform to print to the given string.
		 * \param _sString String to which to append the given texture.
		 * \param _bIncludeIdx If true, [[texture(X)]] is written.
		 * \return Returns true if there were no memory problems printing the string.
		 */
		LSBOOL LSE_CALL										PrintTexture( const LSG_UNIFORM &_uUniform, CString &_sString, LSBOOL _bIncludeIdx );
		
		/**
		 * Prints a sampler to a given string (appending it).
		 *
		 * \param _uUniform Uniform to print to the given string.
		 * \param _sString String to which to append the given sampler.
		 * \param _bIncludeIdx If true, [[sampler(X)]] is written.
		 * \return Returns true if there were no memory problems printing the string.
		 */
		LSBOOL LSE_CALL										PrintSampler( const LSG_UNIFORM &_uUniform, CString &_sString, LSBOOL _bIncludeIdx );
		
		/**
		 * Determines if the given uniform is a texture.
		 *
		 * \param _uUniform Uniform to check for being a texture.
		 * \return Returns true if teh given uniform is a texture.
		 */
		LSBOOL LSE_CALL										IsTexture( const LSG_UNIFORM &_uUniform ) const;
		
		/**
		 * Determines if the given uniform is a sampler.
		 *
		 * \param _uUniform Uniform to check for being a sampler.
		 * \return Returns true if teh given uniform is a sampler.
		 */
		LSBOOL LSE_CALL										IsSampler( const LSG_UNIFORM &_uUniform ) const;


	private :
		typedef CShaderTranslationProcessorBase				Parent;

	};

}	// namespace lsg

#endif	// #if defined( LSG_METAL )

#endif	// __LSG_METALSHADERTRANSLATOR_H__
