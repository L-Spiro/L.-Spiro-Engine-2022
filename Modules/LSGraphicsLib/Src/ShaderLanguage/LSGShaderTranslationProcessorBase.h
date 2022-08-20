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
 * Description: After a shader is syntactically parsed and a syntax tree generated, this class runs over the
 *	tree and allows custom actions to be taken at each node type.  For translation to HLSL and GLSL, primarily
 *	major nodes are visited and refactored.  For example, function declarations and member declarations.
 */


#ifndef __LSG_SHADERTRANSLATIONPROCESSORBASE_H__
#define __LSG_SHADERTRANSLATIONPROCESSORBASE_H__

#include "../LSGGraphicsLib.h"
#include "LSGShaderLanguageParser.h"
#include "LSGShaderParserWrapper.h"
#include "LSGShaderSyntaxNodes.h"
#include "Set/LSTLSet.h"

/**
 * The standard out-of-memory-message.
 */
#define LSG_STB_OOM											"Out of memory."

namespace lsg {

	/**
	 * Class CShaderTranslationProcessorBase
	 * \brief Provides a set of base callbacks that other platform-specific classes can use to process the
	 *	syntax tree differently.
	 *
	 * Description: After a shader is syntactically parsed and a syntax tree generated, this class runs over the
	 *	tree and allows custom actions to be taken at each node type.  For translation to HLSL and GLSL, primarily
	 *	major nodes are visited and refactored.  For example, function declarations and member declarations.
	 */
	class CShaderTranslationProcessorBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CShaderTranslationProcessorBase();
		virtual LSE_CALLCTOR								~CShaderTranslationProcessorBase();


		// == Enumerations.
		/** Error codes. */
		enum LSG_ERROR_CODES {
			LSG_EC_SUCCESS,									/**< No problem. */
			LSG_EC_INVALIDTREE,								/**< The syntax tree is invalid. */
			LSG_EC_OUTOFMEMORY,								/**< Out of memory. */
			LSG_EC_PROCESSINGERROR,							/**< General error. */
			LSG_EC_NOMAIN,									/**< No Main() function found in shader. */
			LSG_EC_MAINOVERLOADED,							/**< The Main() function has been overloaded. */
		};

		/** Processing directives. */
		enum LSG_PROCESSING_DIRECTIVE {
			LSG_PD_CONTINUE,								/**< Continue processing the sub-nodes of the current node. */
			LSG_PD_SKIP_SUB_NODES,							/**< Skip over the sub-nodes of the current node. */
			LSG_PD_ERROR,									/**< Error out. */
		};


		// == Types.
		/** Semantic properties for verification purposes. */
		struct LSG_GLOBAL_SEMANTIC_CHECK {
			uint32_t						ui32Semantic;	/**< The semantic type. */
			uint64_t						ui64Type;		/**< The type that it must be. */
			LSBOOL							bIsArray;		/**< If it should be an array or not. */
			char							szName[32];		/**< Name of the semantic. */
		};

		/** For use with the explicit stack during parsing. */
		typedef struct LSG_STACK_OBJ {
			// == Enumerations.
			/** Bit masks. */
			enum LSG_BITS {
				LSG_B_INMAIN								= (1 << 0),
				LSG_B_INSTRUCTDEF							= (1 << 1),
				LSG_B_ISFUNCDEF								= (1 << 2),
				LSG_B_DONOTREPLACE							= (1 << 3),
			};


			/** String for this node. */
			CString											sNodeString;

			/** Index of the stack item. */
			uint32_t										ui32Index;

			/** Index of the parent stack item. */
			uint32_t										ui32ParentIndex;

			/** Special data associated with this node. */
			union {
				uint32_t									ui32Data;
				uint32_t									ui32InMain				: 1;
				uint32_t									ui32InStructDef			: 1;
				uint32_t									ui32IsFuncDef			: 1;
				uint32_t									ui32DoNotReplace		: 1;
			}												u;

			/** Passing through? */
			LSBOOL											bPassThrough;

			/** Optional prefix characters. */
			CString											sPreFix;

			/** Optional postfix characters. */
			CString											sPostFix;
		} * LPLSG_STACK_OBJ, * const LPCLSG_STACK_OBJ;

		/** A sematic global uniform found while processing the file.
		 *	We keep a list of these and update them automatically so that the
		 *	user does not have to. */
		typedef struct LSG_UNIFORM_SEMANTIC {
			/** The name of the semantic. */
			CString											sName;

			/** Variable type. */
			uint64_t										ui64Type;

			/** Semantic type. */
			uint32_t										ui32SemanticType;

			/** Semantic index. */
			uint32_t										ui32SemanticIndex;
		} * LPLSG_UNIFORM_SEMANTIC, * const LPCLSG_UNIFORM_SEMANTIC;

		/**
		 * An array of global semantics.
		 */
		typedef CVector<LSG_UNIFORM_SEMANTIC>				LSG_UNIFORM_SEMANTIC_LIST;

		/**
		 * A global uniform variable.
		 */
		typedef struct LSG_UNIFORM {
			/** Name of the uniform. */
			CString											sName;

			/** Type. */
			uint64_t										ui64Type;

			/** Array size. */
			uint32_t										ui32Array;

			/** Total size. */
			uint32_t										ui32Size;

			/** Miscellaneous data that can be used by each API however it pleases. */
			uintptr_t										uiptrApiSpecific;

			/** Miscellaneous parameter that can be used by each API however it pleases. */
			uintptr_t										uiptrApiParam;

			/** Reference count. */
			uint32_t										ui32RefCount;
		} * LPLSG_UNIFORM, * const LPCLSG_UNIFORM;

		/**
		 * An array of uniforms.
		 */
		typedef CVector<LSG_UNIFORM>						LSG_UNIFORM_LIST, * LPLSG_UNIFORM_LIST, * const LPCLSG_UNIFORM_LIST;

		/**
		 * An attribute.
		 */
		typedef struct LSG_ATTRIBUTE {
			/** Name of the attribute. */
			CString											sName;

			/** Variable type. */
			uint64_t										ui64Type;

			/** Semantic type. */
			uint32_t										ui32SemanticType;

			/** Semantic index. */
			uint32_t										ui32SemanticIndex;

			/** Is it an input or an output? */
			LSBOOL											bInput;
		} * LPLSG_ATTRIBUTE, * const LPCLSG_ATTRIBUTE;

		/**
		 * An array of attributes.
		 */
		typedef CVector<LSG_ATTRIBUTE>						LSG_ATTRIBUTE_LIST;

		/**
		 * Type information on a node.  Includes one possible type for a node and whether that type is explicitly declared or just implied.
		 */
		typedef struct LSG_NODE_TYPE {
			/** Type information. */
			uint64_t										ui64Type;

			/** Array information. */
			uint32_t										ui32Array;

			/** Is the type explicit? */
			LSBOOL											bIsExplicit;

			/** What is the scope of the type?  Valid only if the type is an identifier. */
			uint32_t										ui32Scope;

			/** If this is an identifier, what type is it? */
			CShaderScope::LSG_TYPE							tType;

			/** Is it a uniform? */
			LSBOOL											bUniform;


			// == Various contrsuctors.
			LSE_CALLCTOR									LSG_NODE_TYPE();
		} * LPLSG_NODE_TYPE, * const LPCLSG_NODE_TYPE;

		/**
		 * An array of node types.
		 */
		typedef CVectorPoD<LSG_NODE_TYPE>					LSG_NODE_TYPE_LIST, * LPLSG_NODE_TYPE_LIST, * const LPCLSG_NODE_TYPE_LIST;

		
		// == Functions.
		/**
		 * Process a syntax tree.  Calls virtual functions at key nodes which are overridden differently depending
		 *	on the target platform.
		 *
		 * \param _slpContainer Container of the syntax tree to process.
		 * \param _sResult Holds the result of processing the syntax tree.  This should be the shader translated into HLSH or GLSL etc.
		 * \param _sError Returned error string.
		 * \return Returns An error code.
		 */
		LSG_ERROR_CODES LSE_CALL							ProcessSyntaxTree( CShaderLanguageParser &_slpContainer, CString &_sResult, CString &_sError );

		/**
		 * Copies the referenced global semantics to the given output.
		 *
		 * \param _uslOut The destination to which to copy the global semantics.
		 * \return Returns true if all semantics were copied successfully.
		 */
		LSBOOL LSE_CALL										CopyGlobalSemantics( LSG_UNIFORM_SEMANTIC_LIST &_uslOut ) const;

		/**
		 * Copies the global uniforms to the given output.
		 *
		 * \param _ulOut The destination to which to copy the global semantics.
		 * \return Returns true if all uniforms were copied successfully.
		 */
		LSBOOL LSE_CALL										CopyGlobalUniforms( LSG_UNIFORM_LIST &_ulOut ) const;

		/**
		 * Copy the attributes to the given output.
		 *
		 * \param _aOut The destination to which to copy the attributes.
		 * \return Returns true if all attributes were copied successfully.
		 */
		LSBOOL LSE_CALL										CopyAttributes( LSG_ATTRIBUTE_LIST &_aOut ) const;

		/**
		 * Gets all possible types of any node.  Returns more than one type when the given node can be implicitly cast to
		 *	multiple types, or when an identifier is declared within the current scope in multiple forms.
		 *
		 * \param _ui32NodeIndex Node index whose type information is to be obtained.
		 * \param _ntlRet Termination node, if the node type
		 * \return Returns true if type information could be obtained from the node.
		 */
		LSBOOL LSE_CALL										GetNodeType( uint32_t _ui32NodeIndex, LSG_NODE_TYPE_LIST &_ntlRet ) const;

	protected :
		// == Types.
		/** The explicit stack. */
		typedef CVector<LSG_STACK_OBJ, uint32_t>			LSG_NODE_STACK;

		/** Node data for callback routines. */
		typedef struct LSG_NODE_CALLBACK {
			/** Container of the syntax tree to process. */
			CShaderLanguageParser *							pslpContainer;

			/** The node to be parsed. */
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA *	pndNode;

			/** The node stack. */
			LSG_NODE_STACK *								pnsNodeStack;

			/** The current shader string. */
			CString *										psShaderText;

			/** Holds the result of processing the node.  This should be the shader translated into HLSH or GLSL etc. */
			CString *										psResult;

			/** The error string if an error occurs. */
			CString											sError;

			/** The index of this node in the stack of nodes to process. */
			uint32_t										ui32Index;

			/** The index of the parent node in the stack of nodes to process. */
			uint32_t										ui32ParentIndex;

			/** Node data. */
			uint32_t										ui32Data;
		} * LPLSG_NODE_CALLBACK, * const LPCLSG_NODE_CALLBACK;
		


		// == Members.
		/** Stack of scopes.  We maintain at least one scope always (the global scope) during parsing
		 *	of the data.  After the data has been parsed, the scope stack is cleared. */
		CShaderScopeStack									m_sssScopes;
		
		/** Do we have a Main()? */
		LSBOOL												m_bHaveMain;

		/** Are we declaring Main() now? */
		LSBOOL												m_bDefiningMain;

		/** The Main() parameters. */
		CVectorPoD<uint32_t, uint32_t>						m_vMainParms;

		/** The last function's parameters. */
		CVectorPoD<uint32_t, uint32_t>						m_vLastParms;

		/** Do we output multiple colors?  Related to multiple render targets. */
		LSBOOL												m_bMultiPixelOutputs;

		/** The container we are using while parsing. */
		CShaderLanguageParser *								m_pslpContainer;

		/** Global semantic uniforms found while parsing the file. */
		LSG_UNIFORM_SEMANTIC_LIST							m_uslGlobalSemantics;

		/** Global uniforms found while parsing the file. */
		LSG_UNIFORM_LIST									m_ulGlobalUniforms;

		/** Main-function attributes. */
		LSG_ATTRIBUTE_LIST									m_alAttributes;

		/** A multi-map of which functions call which other functions. */
		CMultiMap<CString, CString>							m_mmFuncRefs;

		/** The current function. */
		CString												m_sCurFunc;

		/** List of referenced functions. */
		CSet<CString>										m_sFuncRefs;

		/** Semantic checks. */
		static LSG_GLOBAL_SEMANTIC_CHECK					m_gscCheckMe[];


		// == Functions.
		/**
		 * Adds a function reference recursively.
		 *
		 * \param _sFuncRefs The set to which to add the function reference.
		 * \param _sName The name of the current function to add.
		 * \return Returns false if there are any memory failures during the operation.
		 */
		LSBOOL LSE_CALL										RefFunc( CSet<CString> &_sFuncRefs, const CString &_sName );

		/**
		 * Push a node onto the explicit stack.
		 *
		 * \param _nsStack Stack onto which to push the node.
		 * \param _ui32Index Index of the node to push.
		 * \param _ui32ParentIndex Index of the parent node.
		 * \param _ui32Data Optional data to associated with the node.
		 * \param _pcPostFix A postfix string to add to the result when this node text is added.
		 * \param _pcPreFix A prefix string to add to the result when this node text is added.
		 * \return Returns true if there was enough memory to push the node, which is always the case.
		 */
		static LSBOOL LSE_CALL								PushNode( LSG_NODE_STACK &_nsStack, uint32_t _ui32Index, uint32_t _ui32ParentIndex, uint32_t _ui32Data = 0, const char * _pcPostFix = NULL, const char * _pcPreFix = NULL );


	protected :
		// == Functions.
		/**
		 * Default node handler.  Simply tells the processor to go into each node.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns LSG_PD_CONTINUE.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			DefaultNodeHandler( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Default post-processing node handler.  Called after processing a node and its children.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns LSG_PD_CONTINUE.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			DefaultNodePostHandler( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Modify the final shader string after it has been completed.  Override to modify it.
		 *
		 * \param _sShader The final string, modifiable in-place.
		 * \param _sError The error string to be filled if there is an error modifying the shader text.
		 * \return Returns false if an error occurred.
		 */
		virtual LSBOOL LSE_CALL								PostShaderString( CString &_sShader, CString &_sError );

		/**
		 * Handles nodes of type LSG_N_EXTERNALDECLARATIONLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleExternalDeclarationList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_DISCARD.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleDiscard( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_RETURN.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleReturn( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_BREAK.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleBreak( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_CONTINUE.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleContinue( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FOR.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFor( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_DOWHILE.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleDoWhile( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_WHILE.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleWhile( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FORINIT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleForInit( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_SELECTION.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleIfElse( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FUNCDEFINITION.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFunctionDefinition( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FUNCDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PARMDECLARATIONLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleParamaterDeclarationList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PARMDECLARATION.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleParamaterDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PARMDECLARATOR.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleParamaterDeclarator( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STRUCT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleStruct( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STRUCTDECLARATIONLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleStructDeclarationList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STRUCTDECLARATION.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleStructDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STRUCTDECLARATORLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleStructDeclaratorList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STRUCTDECLARATOR.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleStructDeclarator( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_BLANKSTATEMENT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleBlankStatement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_NEWSCOPESTATEMENT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleNewScopeStatement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STATEMENTLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleStatementList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_INITLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleInitList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_INITLISTWRAP.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleInitListWrap( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_SINGLEDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_SEMANTIC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleSemantic( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FULLYSPECIFIEDTYPE.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFullySpecifiedType( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_TYPESPECIFIERNOPREC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleTypeSpecifierNoPrecision( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_DECLARATIONSTATEMENT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleDeclarationStatement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_EXPRESSIONSTATEMENT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleExpressionStatement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_CONSTRUCTOR.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleConstructor( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FIELDSELECT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFieldSelect( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_FUNCTIONCALL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleFunctionCall( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_ARGLIST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleArgList( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_INTRINSIC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleIntrinsic( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_COMMA.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleComma( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_ASSIGNMENT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleAssignment( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_TERTIARY.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleTertiary( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_MATH.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleMath( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_CAST.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleCast( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_UNARY.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleUnary( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PREFIXDEC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandlePrefixDecrement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_PREFIXINC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandlePrefixIncrement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_POSTFIXDEC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandlePostfixDecrement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_POSTFIXINC.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandlePostfixIncrement( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_ARRAYACCESS.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleArrayAccess( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_IDENTIFIER.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleIdentifier( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_BOOLEAN.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleBoolean( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_NUMERICCONSTANT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleNumeric( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Handles nodes of type LSG_N_STRINGCONSTANT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
		 *	of the node or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			HandleString( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Register a variable with the current scope.  Node must be of type LSG_N_SINGLEDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			RegisterVariableWithCurrentScope( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Register a function with the current scope.  Node must be of type LSG_N_FUNCDECL.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			RegisterFunctionWithCurrentScope( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Register a structure with the current scope.  Node must be of type LSG_N_STRUCT.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			RegisterStructWithCurrentScope( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Register the last parameters with a new scope.  Pushes a new scope and registers the parameters in m_vLastParms.
		 *
		 * \param _ncData All data needed for the parsing.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		virtual LSG_PROCESSING_DIRECTIVE LSE_CALL			RegisterParmsWithNewScope( LSG_NODE_CALLBACK &_ncData );

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
		 * Print an error which includes line information.
		 *
		 * \param _sError The error string to which to print the error.
		 * \param _pcError The error to print.
		 * \param _ncData Data required for adding line and file to the error report.
		 */
		void LSE_CALL										PrintError( CString &_sError, const char * _pcError, const LSG_NODE_CALLBACK &_ncData ) const;

		/**
		 * Determines if the given noce is a child node of a given type.  That is, if any parent nodes to the given node have the given type.
		 *
		 * \param _ncData The node.
		 * \param _nType The type to check against all parents of the given node for a match.
		 * \return If no parents of the given node have the given type, false is returned, otherwise true is returned.
		 */
		LSBOOL LSE_CALL										HasParentOfType( const LSG_NODE_CALLBACK &_ncData, LSG_NODE _nType ) const;

		/**
		 * Gets a parent node of a given type.  Returns the first node found matching the given type.
		 *
		 * \param _ncData The node whose parent of the given type is to be found.
		 * \param _nType The type to check against all parents of the given node for a match.
		 * \return Returns a pointer to the parent node of the given type, if found, or NULL otherwise.
		 */
		const LSG_STACK_OBJ * LSE_CALL						GetParentOfType( const LSG_NODE_CALLBACK &_ncData, LSG_NODE _nType ) const;
		
		/**
		 * Gets a node with the given node index.
		 *
		 * \param _ncData The node.
		 * \param _ui32Index Node index/ID.
		 * \return Returns a pointer to the node with the given ID, if found, or NULL otherwise.
		 */
		 const LSG_STACK_OBJ * LSE_CALL						FindNodeById( const LSG_NODE_CALLBACK &_ncData, uint32_t _ui32Index ) const;

		/**
		 * Pre-call to HandleFunctionDefinition().  Sets the current function.
		 *
		 * \param _ncData The node.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					PreHandleFunctionDefinition( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Pre-call to HandleFunctionCall().  Adds a reference to the called function.
		 *
		 * \param _ncData The node.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					PreHandleFunctionCall( LSG_NODE_CALLBACK &_ncData );

		/**
		 * Pre-call to HandleIdentifier().  Adds references to global uniforms.
		 *
		 * \param _ncData The node.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		LSG_PROCESSING_DIRECTIVE LSE_CALL					PreHandleIdentifier( LSG_NODE_CALLBACK &_ncData );
		
		/**
		 * Gets a pointer to an attribute by semantic and index, or NULL if the gven combination does not exist.
		 *
		 * \param _ui32Semantic The semantic.
		 * \param _ui32Index The semantic index.
		 * \param _bInput If true, inputs are found, otherwise outputs are found.
		 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
		 */
		LSG_ATTRIBUTE * LSE_CALL							FindAttribute( uint32_t _ui32Semantic, uint32_t _ui32Index, LSBOOL _bInput );

		/**
		 * Gets the length of m_gscCheckMe.
		 *
		 * \return Returns The length of m_gscCheckMe.
		 */
		static uint32_t LSE_CALL							SemanticCheckLength();


	};

}	// namespace lsg

#endif	// __LSG_SHADERTRANSLATIONPROCESSORBASE_H__
