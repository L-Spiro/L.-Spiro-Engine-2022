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

#include "LSGShaderTranslationProcessorBase.h"


/**
 * Help print errors.
 */
#define LSG_PRINT_ERROR( _pcText )	PrintError( _ncData.sError, _pcText, _ncData );


namespace lsg {

	// == Members.
	/** Semantic checks. */
	CShaderTranslationProcessorBase::LSG_GLOBAL_SEMANTIC_CHECK CShaderTranslationProcessorBase::m_gscCheckMe[] = {
		{ CShaderParser::token::LSG_SLT_NORMALMATRIX,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, 3 ),						false,		"NORMALMATRIX" },
		{ CShaderParser::token::LSG_SLT_TEXMATRIX,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ),						true,		"TEXMATRIX" },
		{ CShaderParser::token::LSG_SLT_WORLD,					CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ),						false,		"WORLD" },
		{ CShaderParser::token::LSG_SLT_WORLDVIEW,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ),						false,		"WORLDVIEW" },
		{ CShaderParser::token::LSG_SLT_WORLDVIEWPROJ,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ),						false,		"WORLDVIEWPROJ" },
		{ CShaderParser::token::LSG_SLT_VIEW,					CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ),						false,		"VIEW" },
		{ CShaderParser::token::LSG_SLT_CAMPOS,					CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, LSE_MAXU32 ),				false,		"CAMPOS" },
		{ CShaderParser::token::LSG_SLT_TIMESINCELASTFRAME,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	false,		"TIMESINCELASTFRAME" },
		{ CShaderParser::token::LSG_SLT_AMBIENTLIGHT,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"AMBIENTLIGHT" },
		{ CShaderParser::token::LSG_SLT_GROUNDCOLOR,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"GROUNDCOLOR" },
		{ CShaderParser::token::LSG_SLT_SKYCOLOR,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"SKYCOLOR" },
		{ CShaderParser::token::LSG_SLT_DIRLIGHTCOUNT,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ),		false,		"DIRLIGHTCOUNT" },
		{ CShaderParser::token::LSG_SLT_POINTLIGHTCOUNT,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ),		false,		"POINTLIGHTCOUNT" },
		{ CShaderParser::token::LSG_SLT_SPOTLIGHTCOUNT,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ),		false,		"SPOTLIGHTCOUNT" },
		{ CShaderParser::token::LSG_SLT_LIGHTCOUNT,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ),		false,		"LIGHTCOUNT" },
		{ CShaderParser::token::LSG_SLT_DIRLIGHTDIRS,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"DIRLIGHTDIRS" },
		{ CShaderParser::token::LSG_SLT_DIRLIGHTDIFFUSES,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"DIRLIGHTDIFFUSES" },
		{ CShaderParser::token::LSG_SLT_POINTLIGHTPOS,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"POINTLIGHTPOS" },
		{ CShaderParser::token::LSG_SLT_POINTLIGHTDIFFUSES,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"POINTLIGHTDIFFUSES" },
		{ CShaderParser::token::LSG_SLT_POINTLIGHTSIZES,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	true,		"POINTLIGHTSIZES" },
		{ CShaderParser::token::LSG_SLT_SPOTLIGHTDIRS,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, LSE_MAXU32 ),				true,		"SPOTLIGHTDIRS" },
		{ CShaderParser::token::LSG_SLT_LIGHTAMBIENT,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"LIGHTAMBIENT" },
		{ CShaderParser::token::LSG_SLT_LIGHTDIFFUSE,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"LIGHTDIFFUSE" },
		{ CShaderParser::token::LSG_SLT_LIGHTSPECULAR,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"LIGHTSPECULAR" },
		{ CShaderParser::token::LSG_SLT_SPOTLIGHTRANGE,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				true,		"SPOTLIGHTRANGE" },
		{ CShaderParser::token::LSG_SLT_MATAMBIENT,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"MATAMBIENT" },
		{ CShaderParser::token::LSG_SLT_MATDIFFUSE,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"MATDIFFUSE" },
		{ CShaderParser::token::LSG_SLT_MATEMISSIVE,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"MATEMISSIVE" },
		{ CShaderParser::token::LSG_SLT_MATSPECULAR,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"MATSPECULAR" },
		{ CShaderParser::token::LSG_SLT_MATPOWER,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	false,		"MATPOWER" },
		{ CShaderParser::token::LSG_SLT_REFLECTIVITY,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	false,		"REFLECTIVITY" },
		{ CShaderParser::token::LSG_SLT_ROUGHNESS,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"ROUGHNESS" },
		{ CShaderParser::token::LSG_SLT_ASHIKFACTORS,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"ASHIKFACTORS" },
		{ CShaderParser::token::LSG_SLT_FOGSTART,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	false,		"FOGSTART" },
		{ CShaderParser::token::LSG_SLT_FOGEND,					CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	false,		"FOGEND" },
		{ CShaderParser::token::LSG_SLT_FOGCOLOR,				CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"FOGCOLOR" },
		{ CShaderParser::token::LSG_SLT_SHADOWMAPPING,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 ),		false,		"SHADOWMAPPING" },
		{ CShaderParser::token::LSG_SLT_SHADOWMAPUNIT,			CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 ),false,		"SHADOWMAPUNIT" },
		{ CShaderParser::token::LSG_SLT_SHADOWMAPMATRIX,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ),						false,		"SHADOWMAPMATRIX" },
		{ CShaderParser::token::LSG_SLT_SHADOWMAPUVDEPTHSCALES,	CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, LSE_MAXU32 ),				false,		"SHADOWMAPUVDEPTHSCALES" },
		{ CShaderParser::token::LSG_SLT_SHADOWMAPCASTERSIZE,	CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ),	false,		"SHADOWMAPCASTERSIZE" },
		{ CShaderParser::token::LSG_SLT_SHADOWMAPTEXDIM,		CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ),				false,		"SHADOWMAPTEXDIM" },
	};
	
	// == Various constructors.
	LSE_CALLCTOR CShaderTranslationProcessorBase::CShaderTranslationProcessorBase() :
		m_bHaveMain( false ),
		m_bDefiningMain( false ),
		m_pslpContainer( NULL ),
		m_bMultiPixelOutputs( false ) {
	}
	LSE_CALLCTOR CShaderTranslationProcessorBase::~CShaderTranslationProcessorBase() {
	}

	// == Various contrsuctors.
	LSE_CALLCTOR CShaderTranslationProcessorBase::LSG_NODE_TYPE::LSG_NODE_TYPE() {
		tType = CShaderScope::LSG_T_UNDEFINED;
		bUniform = false;
	}

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
	CShaderTranslationProcessorBase::LSG_ERROR_CODES LSE_CALL CShaderTranslationProcessorBase::ProcessSyntaxTree( CShaderLanguageParser &_slpContainer, CString &_sResult, CString &_sError ) {
		m_sssScopes.Reset();
		m_pslpContainer = &_slpContainer;
		m_bMultiPixelOutputs = false;

		// Build a set of referenced functions.
		if ( !RefFunc( m_sFuncRefs, "Main" ) ) {
			_sError = LSG_STB_OOM;
			return LSG_EC_OUTOFMEMORY;
		}


		// Recursive calls can lead to stack overflow if the syntax tree is large (in other words, if the shader file is large).
		//	We use an explicit stack.
		if ( _slpContainer.TotalNodes() == 0 ) {
			_sError = "Syntax tree is invalid.";
			return LSG_EC_INVALIDTREE;
		}
		LSG_NODE_STACK nsStack;
		// Pre-allocating is absolutely 100% vital.  It WILL crash if this is not done.
		// Pointers to objects on the stack are used, so if re-allocation ever occurs the pointers will become
		//	invalid but may still be in use.  Pre-allocation solves this.  Additionally, the allocation size
		//	must be large enough to ensure that re-allocation never happens.  To ensure this, the stack is
		//	allocated to the number of nodes + 1.
		if ( !nsStack.Allocate( _slpContainer.TotalNodes() + 1 ) ) {
			_sError = LSG_STB_OOM;
			return LSG_EC_OUTOFMEMORY;
		}

		// It is impossible for PushNode() to fail.
		if ( !PushNode( nsStack, _slpContainer.TotalNodes() - 1, LSE_MAXU32 ) ) {
			_sError = LSG_STB_OOM;
			return LSG_EC_OUTOFMEMORY;
		}

		// Create the global scope.
		if ( !m_sssScopes.Push() ) {
			_sError = LSG_STB_OOM;
			return LSG_EC_OUTOFMEMORY;
		}

		_sResult.Reset();

		LSG_NODE_CALLBACK ncCallBack;
		ncCallBack.pslpContainer = &_slpContainer;
		ncCallBack.psShaderText = &_sResult;
		ncCallBack.pnsNodeStack = &nsStack;

#define LSG_PUSH( _ui32Index )								if ( !PushNode( nsStack, _ui32Index, ui32ThisIndex ) ) { _sError = LSG_STB_OOM; return LSG_EC_OUTOFMEMORY; }
#define LSG_PUSH_WITH_POSTFIX( _ui32Index, _pcPostFix )		if ( !PushNode( nsStack, _ui32Index, ui32ThisIndex, 0, _pcPostFix ) ) { _sError = LSG_STB_OOM; return LSG_EC_OUTOFMEMORY; }
#define LSG_COMMONCASES										case LSG_PD_SKIP_SUB_NODES : { continue; } case LSG_PD_ERROR : _sError = ncCallBack.sError; return LSG_EC_PROCESSINGERROR

		while ( nsStack.Length() ) {
			// Process the top of the stack.
			uint32_t ui32ThisIndex = nsStack.Length() - 1;
			LSG_STACK_OBJ & soProcessMe = nsStack[ui32ThisIndex];
			LSBOOL bFirstPassOnThisNode = !soProcessMe.bPassThrough;
			// Revisit this node after processing its children, but with the already-processed flag set to true.
			soProcessMe.bPassThrough = true;

			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndThisNode = _slpContainer.GetNode( soProcessMe.ui32Index );

			ncCallBack.pndNode = &ndThisNode;
			ncCallBack.psResult = &soProcessMe.sNodeString;
			ncCallBack.ui32Index = ui32ThisIndex;
			ncCallBack.ui32ParentIndex = soProcessMe.ui32ParentIndex;
			ncCallBack.ui32Data = soProcessMe.u.ui32Data;

			if ( bFirstPassOnThisNode ) {
				// First the default node handler.
				switch ( DefaultNodeHandler( ncCallBack ) ) {
					LSG_COMMONCASES;
					default : {}
				}

				// Per-node-type handler.
				switch ( ndThisNode.nNodeType ) {
					case LSG_N_EXTERNALDECLARATIONLIST : {
						// Process the left node then the right.  Push in opposite order because the top of the stack is
						//	what gets processed next.
						switch ( HandleExternalDeclarationList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_DISCARD : {
						switch ( HandleDiscard( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_RETURN : {
						switch ( HandleReturn( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_BREAK : {
						switch ( HandleBreak( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_CONTINUE : {
						switch ( HandleContinue( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_FOR : {
						switch ( HandleFor( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.ui32SingleDeclName );
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_DOWHILE : {
						switch ( HandleDoWhile( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_WHILE : {
						switch ( HandleWhile( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_FORINIT : {
						switch ( HandleForInit( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, ";" );
							}
						}
						break;
					}
					case LSG_N_SELECTION : {
						switch ( HandleIfElse( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_FUNCDEFINITION : {
						switch ( PreHandleFunctionDefinition( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						switch ( HandleFunctionDefinition( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_FUNCDECL : {
						switch ( HandleFunctionDeclaration( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_PARMDECLARATIONLIST : {
						switch ( HandleParamaterDeclarationList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "," );
							}
						}
						break;
					}
					case LSG_N_PARMDECLARATION : {
						switch ( HandleParamaterDeclaration( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_PARMDECLARATOR : {
						switch ( HandleParamaterDeclarator( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.ui32SingleDeclName );
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
								LSG_PUSH( ndThisNode.nuNodeData.ui32IdentifierIndex );
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_STRUCT : {
						switch ( HandleStruct( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_STRUCTDECLARATIONLIST : {
						switch ( HandleStructDeclarationList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_STRUCTDECLARATION : {
						switch ( HandleStructDeclaration( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nueNodeDataEx.ui32NodeIndexEx, ";\n" );
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
							}
						}
						break;
					}
					case LSG_N_STRUCTDECLARATORLIST : {
						switch ( HandleStructDeclaratorList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "," );
							}
						}
						break;
					}
					case LSG_N_STRUCTDECLARATOR : {
						switch ( HandleStructDeclarator( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_BLANKSTATEMENT : {
						switch ( HandleBlankStatement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
							}
						}
						break;
					}
					case LSG_N_NEWSCOPESTATEMENT : {
						switch ( HandleNewScopeStatement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_STATEMENTLIST : {
						switch ( HandleStatementList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_INITLIST : {
						switch ( HandleInitList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "," );
							}
						}
						break;
					}
					case LSG_N_INITLISTWRAP : {
						switch ( HandleInitListWrap( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_SINGLEDECL : {
						switch ( HandleSingleDeclaration( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.ui32SingleDeclName );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_SEMANTIC : {
						switch ( HandleSemantic( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_FULLYSPECIFIEDTYPE : {
						switch ( HandleFullySpecifiedType( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_TYPESPECIFIERNOPREC : {
						switch ( HandleTypeSpecifierNoPrecision( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_DECLARATIONSTATEMENT : {
						switch ( HandleDeclarationStatement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, ";\n" );
							}
						}
						break;
					}
					case LSG_N_EXPRESSIONSTATEMENT : {
						switch ( HandleExpressionStatement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, ";\n" );
							}
						}
						break;
					}
					case LSG_N_CONSTRUCTOR : {
						switch ( HandleConstructor( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_FIELDSELECT : {
						switch ( HandleFieldSelect( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "." );
							}
						}
						break;
					}
					case LSG_N_FUNCTIONCALL : {
						switch ( PreHandleFunctionCall( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						switch ( HandleFunctionCall( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_ARGLIST : {
						switch ( HandleArgList( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "," );
							}
						}
						break;
					}
					case LSG_N_INTRINSIC : {
						switch ( HandleIntrinsic( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_COMMA : {
						switch ( HandleComma( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "," );
							}
						}
						break;
					}
					case LSG_N_ASSIGNMENT : {
						switch ( HandleAssignment( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_TERTIARY : {
						switch ( HandleTertiary( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuoOp.ui32NodeIndex3 );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nueNodeDataEx.ui32NodeIndexEx, ":" );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "?" );
							}
						}
						break;
					}
					case LSG_N_MATH : {
						switch ( HandleMath( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_CAST : {
						switch ( HandleCast( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_UNARY : {
						switch ( HandleUnary( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nueNodeDataEx.ui32NodeIndexEx );
							}
						}
						break;
					}
					case LSG_N_PREFIXDEC : {
						switch ( HandlePrefixDecrement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_PREFIXINC : {
						switch ( HandlePrefixIncrement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH( ndThisNode.nuNodeData.ui32NodeIndex );
							}
						}
						break;
					}
					case LSG_N_POSTFIXDEC : {
						switch ( HandlePostfixDecrement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "--" );
							}
						}
						break;
					}
					case LSG_N_POSTFIXINC : {
						switch ( HandlePostfixIncrement( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "++" );
							}
						}
						break;
					}
					case LSG_N_ARRAYACCESS : {
						switch ( HandleArrayAccess( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nueNodeDataEx.ui32NodeIndexEx, "]" );
								LSG_PUSH_WITH_POSTFIX( ndThisNode.nuNodeData.ui32NodeIndex, "[" );
							}
						}
						break;
					}
					case LSG_N_IDENTIFIER : {
						switch ( PreHandleIdentifier( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						switch ( HandleIdentifier( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_BOOLEAN : {
						switch ( HandleBoolean( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_NUMERICCONSTANT : {
						switch ( HandleNumeric( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
					case LSG_N_STRINGCONSTANT : {
						switch ( HandleString( ncCallBack ) ) {
							LSG_COMMONCASES;
							default : {}
						}
						break;
					}
				}
			}
			else {
				// Already handled this node and coming back to it.  Call the post-processing handler.
				switch ( DefaultNodePostHandler( ncCallBack ) ) {
					case LSG_PD_ERROR : {
						_sError = ncCallBack.sError;
						return LSG_EC_PROCESSINGERROR;
					}
					default : {}
				}
				CString * psTarget = (ncCallBack.ui32ParentIndex == LSE_MAXU32) ? &_sResult : &nsStack[ncCallBack.ui32ParentIndex].sNodeString;

				// Add the prefix (to the start of the parent node).
				if ( !psTarget->Append( soProcessMe.sPreFix ) ) {
					_sError = LSG_STB_OOM;
					return LSG_EC_OUTOFMEMORY;
				}
				// Need to give back some RAM.
				soProcessMe.sPreFix.Reset();

				// Add the string to the result.
				if ( !psTarget->Append( (*ncCallBack.psResult) ) ) {
					_sError = LSG_STB_OOM;
					return LSG_EC_OUTOFMEMORY;
				}
				// Need to give back some RAM.
				(*ncCallBack.psResult).Reset();

				// Add the postfix.
				if ( !psTarget->Append( soProcessMe.sPostFix ) ) {
					_sError = LSG_STB_OOM;
					return LSG_EC_OUTOFMEMORY;
				}
				// Need to give back some RAM.
				soProcessMe.sPostFix.Reset();


				psTarget->Snap();
				// Remove it from the stack.
				nsStack.PopNoDealloc();
			}
		}

		// Should be at global scope.  Remove it.
		// Assert is just to make sure I correctly handled all of the pushing and popping.
		assert( m_sssScopes.TotalScopes() == 1 );


		// At the global scope there must be a Main() function.
		if ( !m_bHaveMain ) {
			_sError = "No \"Main\" function found in shader.  Shader must include a function with the following format: \"void Main( [IN/OUT PARAMETERS] )\".";
			return LSG_EC_NOMAIN;
		}


		// Final modification on the shader text.
		if ( !PostShaderString( _sResult, _sError ) ) {
			return LSG_EC_PROCESSINGERROR;
		}


		m_sssScopes.Pop();
#ifdef _DEBUG
		CStd::DebugPrintA( _sResult.CStr() );
#endif	// #ifdef _DEBUG
		return LSG_EC_SUCCESS;
#undef LSG_COMMONCASES
#undef LSG_PUSH_WITH_POSTFIX
#undef LSG_PUSH
	}

	/**
	 * Copies the referenced global semantics to the given output.
	 *
	 * \param _uslOut The destination to which to copy the global semantics.
	 * \return Returns true if all semantics were copied successfully.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::CopyGlobalSemantics( LSG_UNIFORM_SEMANTIC_LIST &_uslOut ) const {
		_uslOut.ResetNoDealloc();
		if ( !_uslOut.Allocate( m_uslGlobalSemantics.Length() ) ) { return false; }
		for ( uint32_t I = 0; I < m_uslGlobalSemantics.Length(); ++I ) {
			if ( !_uslOut.Push( m_uslGlobalSemantics[I] ) ) { return false; }
			if ( _uslOut[I].sName != m_uslGlobalSemantics[I].sName ) { return false; }
		}
		_uslOut.Snap();
		return true;
	}

	/**
	 * Copies the global uniforms to the given output.
	 *
	 * \param _ulOut The destination to which to copy the global semantics.
	 * \return Returns true if all uniforms were copied successfully.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::CopyGlobalUniforms( LSG_UNIFORM_LIST &_ulOut ) const {
		_ulOut.ResetNoDealloc();
		if ( !_ulOut.Allocate( m_ulGlobalUniforms.Length() ) ) { return false; }
		for ( uint32_t I = 0; I < m_ulGlobalUniforms.Length(); ++I ) {
			if ( !_ulOut.Push( m_ulGlobalUniforms[I] ) ) { return false; }
			if ( _ulOut[I].sName != m_ulGlobalUniforms[I].sName ) { return false; }
		}
		_ulOut.Snap();
		return true;
	}

	/**
	 * Copy the attributes to the given output.
	 *
	 * \param _aOut The destination to which to copy the attributes.
	 * \return Returns true if all attributes were copied successfully.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::CopyAttributes( LSG_ATTRIBUTE_LIST &_aOut ) const {
		_aOut.ResetNoDealloc();
		if ( !_aOut.Allocate( m_alAttributes.Length() ) ) { return false; }
		for ( uint32_t I = 0; I < m_alAttributes.Length(); ++I ) {
			if ( !_aOut.Push( m_alAttributes[I] ) ) { return false; }
			if ( _aOut[I].sName != m_alAttributes[I].sName ) { return false; }
		}
		_aOut.Snap();
		return true;
	}

	/**
	 * Gets all possible types of any node.  Returns more than one type when the given node can be implicitly cast to
	 *	multiple types, or when an identifier is declared within the current scope in multiple forms.
	 *
	 * \param _ui32NodeIndex Node index whose type information is to be obtained.
	 * \param _ntlRet Termination node, if the node type
	 * \return Returns true if type information could be obtained from the node.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::GetNodeType( uint32_t _ui32NodeIndex, LSG_NODE_TYPE_LIST &_ntlRet ) const {
		if ( _ui32NodeIndex == LSE_MAXU32 ) {
			return false;
		}
		const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndThis = m_pslpContainer->GetNode( _ui32NodeIndex );
		switch ( ndThis.nNodeType ) {
			case LSG_N_NUMERICCONSTANT : {
				switch ( ndThis.nueNodeDataEx.ncConstantType ) {
					case LSG_NC_UNSIGNED : {}
					case LSG_NC_SIGNED : {
						LSG_NODE_TYPE ntThis;
						// Integer types are explicit.
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 );
						ntThis.bIsExplicit = true;
						ntThis.ui32Array = LSE_MAXU32;
						_ntlRet.Push( ntThis );
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );

						// Floating types are not.
						ntThis.bIsExplicit = false;
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );
						return true;
					}
					case LSG_NC_FLOATING : {
						LSG_NODE_TYPE ntThis;
						// Floating types are explicit.
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 );
						ntThis.bIsExplicit = true;
						ntThis.ui32Array = LSE_MAXU32;
						_ntlRet.Push( ntThis );

						// Integer types are not.
						ntThis.bIsExplicit = false;
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );
						return true;
					}
					default : {}
				}
				return false;
			}
			case LSG_N_IDENTIFIER : {
				// Get every form of the identifier.
				// Start with structures.
				CString sName = m_pslpContainer->GetName( ndThis );
				LSG_NODE_TYPE ntThis;
				ntThis.bIsExplicit = true;
				ntThis.ui32Array = LSE_MAXU32;
				ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( m_pslpContainer->GetStructTypeId( sName.CStr() ), LSE_MAXU32, LSE_MAXU32 );
				uint32_t ui32Len = _ntlRet.Length();
				if ( ntThis.ui64Type != CShaderLanguageParser::CombineTypesAndArrays( 0, LSE_MAXU32, LSE_MAXU32 ) ) {
					// It is a registered structure type.
					ntThis.tType = CShaderScope::LSG_T_STRUCT;
					ntThis.bUniform = false;
					_ntlRet.Push( ntThis );
				}


				// Get variable types.
				CVectorPoD<CShaderScopeStack::LSG_TYPE_INDEX_AND_INDEX> vTypes;
				m_sssScopes.GetIdentifiersAs( CShaderScope::LSG_T_VARIABLE, sName, vTypes );
				for ( uint32_t I = vTypes.Length(); I--; ) {
					ntThis.ui64Type = m_sssScopes.GetScope( vTypes[I].ui32StackIndex )->GetVariable( vTypes[I].taiScopeIndex.ui32Index ).ui64Type;
					ntThis.ui32Array = m_sssScopes.GetScope( vTypes[I].ui32StackIndex )->GetVariable( vTypes[I].taiScopeIndex.ui32Index ).ui32Array;
					ntThis.tType = CShaderScope::LSG_T_VARIABLE;
					ntThis.ui32Scope = vTypes[I].ui32StackIndex;	// 0 indicates global scope, everything else is inside a function.
					ntThis.bUniform = (m_sssScopes.GetScope( vTypes[I].ui32StackIndex )->GetVariable( vTypes[I].taiScopeIndex.ui32Index ).ui64Qualifiers & LSG_SQ_UNIFORM) != 0;
					_ntlRet.Push( ntThis );
				}

				// Get function types.
				vTypes.ResetNoDealloc();
				m_sssScopes.GetIdentifiersAs( CShaderScope::LSG_T_FUNCTION, sName, vTypes );
				ntThis.ui32Array = LSE_MAXU32;
				for ( uint32_t I = vTypes.Length(); I--; ) {
					ntThis.ui64Type = m_sssScopes.GetScope( vTypes[I].ui32StackIndex )->GetFunction( vTypes[I].taiScopeIndex.ui32Index ).ui64Type;
					ntThis.tType = CShaderScope::LSG_T_FUNCTION;
					ntThis.bUniform = false;
					_ntlRet.Push( ntThis );
				}

				// Return true if we added anything.
				return _ntlRet.Length() > ui32Len;
			}
			case LSG_N_ARRAYACCESS : {
				uint32_t ui32Len = _ntlRet.Length();

				// Get the base type.
				LSG_NODE_TYPE_LIST ntlTemp;
				if ( !GetNodeType( ndThis.nuNodeData.ui32NodeIndex, ntlTemp ) ) {
					return false;
				}

				for ( uint32_t I = ntlTemp.Length(); I--; ) {
					// For anything that is an array, remove the array part.
					if ( ntlTemp[I].ui32Array != LSE_MAXU32 ) {
						ntlTemp[I].ui32Array = LSE_MAXU32;
						_ntlRet.Push( ntlTemp[I] );
					}
				}

				// Return true if we added anything.
				return _ntlRet.Length() > ui32Len;
			}
			case LSG_N_PREFIXINC : {}
			case LSG_N_PREFIXDEC : {}
			case LSG_N_POSTFIXINC : {}
			case LSG_N_POSTFIXDEC : {
				return GetNodeType( ndThis.nuNodeData.ui32NodeIndex, _ntlRet );
			}
			case LSG_N_UNARY : {
				uint32_t ui32Len = _ntlRet.Length();

				switch ( ndThis.nuNodeData.ui32UnaryOp ) {
					case '!' : {
						LSG_NODE_TYPE ntThis;
						ntThis.bIsExplicit = true;
						ntThis.ui32Array = LSE_MAXU32;
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );

						// Can also be integer form.
						ntThis.bIsExplicit = false;
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );
						ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 );
						_ntlRet.Push( ntThis );
						break;
					}
					case '~' : {
						LSG_NODE_TYPE_LIST ntlTemp;
						if ( !GetNodeType( ndThis.nuNodeData.ui32NodeIndex, ntlTemp ) ) { return false; }

						// Keep all of the integer values.
						for ( uint32_t I = ntlTemp.Length(); I--; ) {
							switch ( ntlTemp[I].ui64Type & 0xFFFFFFFFULL ) {
								case CShaderParser::token::LSG_SLT_INT : {}
								case CShaderParser::token::LSG_SLT_UINT : {
									_ntlRet.Push( ntlTemp[I] );
									break;
								}
							}
						}
					}
					case '-' : {}
					case '+' : {
						return GetNodeType( ndThis.nuNodeData.ui32NodeIndex, _ntlRet );
					}
				}

				// Return true if we added anything.
				return _ntlRet.Length() > ui32Len;
			}
			case LSG_N_CAST : {
				LSG_NODE_TYPE_LIST ntlTemp;
				if ( !GetNodeType( ndThis.nueNodeDataEx.ui32NodeIndexEx, ntlTemp ) ) { return false; }
				// Change everything to explicit.  Actually they should be explicit already given how a cast works.
				for ( uint32_t I = ntlTemp.Length(); I--; ) {
					ntlTemp[I].bIsExplicit = true;
					_ntlRet.Push( ntlTemp[I] );
				}

				return true;
			}
			case LSG_N_MATH : {
				uint32_t ui32Len = _ntlRet.Length();

				LSG_NODE_TYPE_LIST ntlLeft;
				LSG_NODE_TYPE_LIST ntlRight;
				if ( !GetNodeType( ndThis.nuNodeData.ui32NodeIndex, ntlLeft ) ) {
					return false;
				}
				if ( !GetNodeType( ndThis.nueNodeDataEx.ui32NodeIndexEx, ntlRight ) ) {
					return false;
				}

				// Find every valid combination between the two.
				for ( uint32_t I = ntlRight.Length(); I--; ) {
					// Arrays are out.
					if ( ntlRight[I].ui32Array != LSE_MAXU32 ) { continue; }
					for ( uint32_t J = ntlLeft.Length(); J--; ) {
						// Structures are out.
						if ( CShaderLanguageParser::IsEitherStruct( ntlLeft[J].ui64Type, ntlRight[I].ui64Type ) ) { continue; }

						// Arrays are out.
						if ( ntlLeft[J].ui32Array != LSE_MAXU32 ) { continue; }

						// Must be of type matrix or lower, so check for compatibilities.
						switch ( ndThis.nuoOp.ui32Op ) {
							case '>' : {}
							case '<' : {}
							case CShaderParser::token::LSG_SLT_GE : {}
							case CShaderParser::token::LSG_SLT_LE : {}
							case CShaderParser::token::LSG_SLT_EQUALS : {}
							case CShaderParser::token::LSG_SLT_NOT_EQUALS : {}
							case CShaderParser::token::LSG_SLT_AND : {}
							case CShaderParser::token::LSG_SLT_OR : {
								LSG_NODE_TYPE ntThis;
								ntThis.ui32Array = LSE_MAXU32;
								ntThis.bIsExplicit = true;
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 );
								_ntlRet.Push( ntThis );
								continue;
							}
						}

						if ( CShaderLanguageParser::BothAreMatrices( ntlLeft[J].ui64Type, ntlRight[I].ui64Type ) ) {
							// Result will be a matrix type.
							LSG_NODE_TYPE ntThis = ntlLeft[J];
							ntThis.bIsExplicit = true;
							_ntlRet.Push( ntThis );
						}
						else if ( CShaderLanguageParser::IsEitherMatrix( ntlLeft[J].ui64Type, ntlRight[I].ui64Type ) ) {
							// One is a matrix but the other is not.
							if ( CShaderLanguageParser::IsEitherVector( ntlLeft[J].ui64Type, ntlRight[I].ui64Type ) ) {
								// Matrix * Vector = Vector.
								LSG_NODE_TYPE ntThis;
								ntThis.ui32Array = LSE_MAXU32;
								ntThis.bIsExplicit = true;
								uint32_t ui32Col, ui32Rows;
								CShaderLanguageParser::GetMatrixSize( ntlLeft[J].ui64Type, ntlRight[I].ui64Type, ui32Col, ui32Rows );
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, ui32Rows, LSE_MAXU32 );
								_ntlRet.Push( ntThis );
							}
							else {
								// Matrix * Scalar = Matrix.
								LSG_NODE_TYPE ntThis;
								ntThis.ui32Array = LSE_MAXU32;
								ntThis.bIsExplicit = true;
								uint32_t ui32Col, ui32Rows;
								CShaderLanguageParser::GetMatrixSize( ntlLeft[J].ui64Type, ntlRight[I].ui64Type, ui32Col, ui32Rows );
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, ui32Col, ui32Rows );
								_ntlRet.Push( ntThis );
							}
						}
						else if ( CShaderLanguageParser::BothAreVectors( ntlLeft[J].ui64Type, ntlRight[I].ui64Type ) ) {
							// Result will be a vector type.
							LSG_NODE_TYPE ntThis = ntlLeft[J];
							ntThis.bIsExplicit = true;
							_ntlRet.Push( ntThis );
						}
						else if ( CShaderLanguageParser::IsEitherVector( ntlLeft[J].ui64Type, ntlRight[I].ui64Type ) ) {
							// Vector * Scalar = Vector.
							LSG_NODE_TYPE ntThis;
							ntThis.ui32Array = LSE_MAXU32;
							ntThis.bIsExplicit = true;
							uint32_t ui32Size;
							CShaderLanguageParser::GetVectorSize( ntlLeft[J].ui64Type, ntlRight[I].ui64Type, ui32Size );
							ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, ui32Size, LSE_MAXU32 );
							_ntlRet.Push( ntThis );
						}
						else {
							// Scalar * scalar = scalar.
							// Find the higher type.
							LSG_NUM_CONSTANTS ncType = CShaderLanguageParser::GetHighType( CShaderLanguageParser::TypeToConstantType( ntlLeft[J].ui64Type ),
								CShaderLanguageParser::TypeToConstantType( ntlRight[I].ui64Type ) );
							if ( ncType != LSG_NC_INVALID ) {

								LSG_NODE_TYPE ntThis;
								ntThis.ui32Array = LSE_MAXU32;
								ntThis.bIsExplicit = ntlLeft[J].bIsExplicit && ntlRight[I].bIsExplicit && ntlLeft[J].ui64Type == ntlRight[I].ui64Type;
								ntThis.ui64Type = CShaderLanguageParser::ConstantTypeToType( ncType );
								_ntlRet.Push( ntlLeft[J] );
							}
						}
					}
				}

				// Return true if we added anything.
				return _ntlRet.Length() > ui32Len;
			}
			case LSG_N_TERTIARY : {
				uint32_t ui32Len = _ntlRet.Length();

				LSG_NODE_TYPE_LIST ntlLeft;
				LSG_NODE_TYPE_LIST ntlRight;
				if ( !GetNodeType( ndThis.nueNodeDataEx.ui32NodeIndexEx, ntlLeft ) ) {
					return false;
				}
				if ( !GetNodeType( ndThis.nuoOp.ui32NodeIndex3, ntlRight ) ) {
					return false;
				}

				// Find every valid combination between the two.
				for ( uint32_t I = ntlRight.Length(); I--; ) {
					// Arrays are out.
					if ( ntlRight[I].ui32Array != LSE_MAXU32 ) { continue; }
					for ( uint32_t J = ntlLeft.Length(); J--; ) {
						// Arrays are out.
						if ( ntlLeft[J].ui32Array != LSE_MAXU32 ) { continue; }
						// If their types match exactly, we win.
						if ( ntlRight[I].ui64Type == ntlLeft[J].ui64Type ) {
							LSG_NODE_TYPE ntThis;
							ntThis.ui32Array = LSE_MAXU32;
							ntThis.bIsExplicit = true;
							ntThis.ui64Type = ntlRight[I].ui64Type;
							_ntlRet.Push( ntlLeft[J] );
						}

						// Otherwise they must be castable.
						CShaderLanguageParser::LSG_SCALAR sLeft, sRight;
						LSG_NUM_CONSTANTS ncLeft = CShaderLanguageParser::TypeToConstantType( ntlLeft[J].ui64Type );
						LSG_NUM_CONSTANTS ncRight = CShaderLanguageParser::TypeToConstantType( ntlRight[I].ui64Type );
						if ( ncLeft == LSG_NC_INVALID || ncRight == LSG_NC_INVALID ) { continue; }

						// Cast to the common type.
						LSBOOL bSuccess;
						LSG_NUM_CONSTANTS ncType = CShaderLanguageParser::GetHighType( ncLeft, ncRight );
						sLeft = CShaderLanguageParser::CastScalar( sLeft, CShaderLanguageParser::ConstantTypeToType( ncLeft ), CShaderLanguageParser::ConstantTypeToType( ncType ), bSuccess );
						if ( !bSuccess ) { continue; }
						sRight = CShaderLanguageParser::CastScalar( sRight, CShaderLanguageParser::ConstantTypeToType( ncRight ), CShaderLanguageParser::ConstantTypeToType( ncType ), bSuccess );
						if ( !bSuccess ) { continue; }

						switch ( ncType ) {
							case LSG_NC_UNSIGNED : {}
							case LSG_NC_SIGNED : {
								LSG_NODE_TYPE ntThis;
								// Integer types are explicit.
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 );
								ntThis.bIsExplicit = true;
								ntThis.ui32Array = LSE_MAXU32;
								_ntlRet.Push( ntThis );
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 );
								_ntlRet.Push( ntThis );

								// Floating types are not.
								ntThis.bIsExplicit = false;
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 );
								_ntlRet.Push( ntThis );
								continue;
							}
							case LSG_NC_FLOATING : {
								LSG_NODE_TYPE ntThis;
								// Floating types are explicit.
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 );
								ntThis.bIsExplicit = true;
								ntThis.ui32Array = LSE_MAXU32;
								_ntlRet.Push( ntThis );

								// Integer types are not.
								ntThis.bIsExplicit = false;
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 );
								_ntlRet.Push( ntThis );
								ntThis.ui64Type = CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 );
								_ntlRet.Push( ntThis );
								continue;
							}
							default : {}
						}
					}
				}
				// Return true if we added anything.
				return _ntlRet.Length() > ui32Len;
			}
			case LSG_N_ASSIGNMENT : {
				// The type of the left node is the return type.
				return GetNodeType( ndThis.nuNodeData.ui32NodeIndex, _ntlRet );
			}
			case LSG_N_COMMA : {
				// The type of the right node is the return type.
				return GetNodeType( ndThis.nueNodeDataEx.ui32NodeIndexEx, _ntlRet );
			}
			case LSG_N_INTRINSIC : {
				// TODO.
				return false;
			}
			case LSG_N_FUNCTIONCALL : {
				uint32_t ui32Len = _ntlRet.Length();

				// Look up the type of the function.
				CString sName = m_pslpContainer->GetName( m_pslpContainer->GetNode( ndThis.nuNodeData.ui32NodeIndex ) );


				// Get function types.
				CVectorPoD<CShaderScopeStack::LSG_TYPE_INDEX_AND_INDEX> vTypes;
				m_sssScopes.GetIdentifiersAs( CShaderScope::LSG_T_FUNCTION, sName, vTypes );
				LSG_NODE_TYPE ntThis;
				ntThis.bIsExplicit = true;
				ntThis.ui32Array = LSE_MAXU32;
				for ( uint32_t I = vTypes.Length(); I--; ) {
					ntThis.ui64Type = m_sssScopes.GetScope( vTypes[I].ui32StackIndex )->GetFunction( vTypes[I].taiScopeIndex.ui32Index ).ui64Type;
					_ntlRet.Push( ntThis );
				}

				// Return true if we added anything.
				return _ntlRet.Length() > ui32Len;
			}
			case LSG_N_CONSTRUCTOR : {
				LSG_NODE_TYPE ntThis;
				ntThis.bIsExplicit = true;
				ntThis.ui32Array = LSE_MAXU32;
				ntThis.ui64Type = ndThis.nuNodeData.ui64Const;
				return _ntlRet.Push( ntThis );
			}
			case LSG_N_INITLISTWRAP : {
				return false;
			}
			default : {}
		}
		return false;
	}

	/**
	 * Adds a function reference recursively.
	 *
	 * \param _sFuncRefs The set to which to add the function reference.
	 * \param _sName The name of the current function to add.
	 * \return Returns false if there are any memory failures during the operation.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::RefFunc( CSet<CString> &_sFuncRefs, const CString &_sName ) {
		// Find the function in the multi-map.
		for ( uint32_t I = m_pslpContainer->GetFuncRefs().Length(); I--; ) {
			if ( m_pslpContainer->GetString( m_pslpContainer->GetFuncRefs().GetKeyByIndex( I ) ) == _sName ) {
				// Now add the functions it references.
				for ( uint32_t J = m_pslpContainer->GetFuncRefs().ItemsOnKey( I ); J--; ) {
					CString sThis = m_pslpContainer->GetString( m_pslpContainer->GetFuncRefs().GetByIndex( I, J ) );
					if ( !_sFuncRefs.ItemExists( sThis ) && sThis != _sName ) {
						if ( !RefFunc( _sFuncRefs, sThis ) ) { return false; }
					}
				}
			}
		}

		return _sFuncRefs.Insert( _sName );
	}

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
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::PushNode( LSG_NODE_STACK &_nsStack, uint32_t _ui32Index, uint32_t _ui32ParentIndex, uint32_t _ui32Data, const char * _pcPostFix, const char * _pcPreFix ) {
		if ( _ui32Index == LSE_MAXU32 ) { return true; }	// Blank nodes cannot be processed but are valid for passing to this function.

		LSG_STACK_OBJ soDoMeNext;
		soDoMeNext.bPassThrough = false;
		soDoMeNext.ui32Index = _ui32Index;
		soDoMeNext.ui32ParentIndex = _ui32ParentIndex;

		uint32_t ui32ParentData = 0;
		if ( soDoMeNext.ui32ParentIndex != LSE_MAXU32 ) {
			ui32ParentData = _nsStack[soDoMeNext.ui32ParentIndex].u.ui32Data;
		}

		soDoMeNext.u.ui32Data = ui32ParentData | _ui32Data;
		if ( _pcPostFix ) {
			if ( !soDoMeNext.sPostFix.Set( _pcPostFix ) ) { return false; }
		}
		if ( _pcPreFix ) {
			if ( !soDoMeNext.sPreFix.Set( _pcPreFix ) ) { return false; }
		}
		return _nsStack.Push( soDoMeNext );
	}

	/**
	 * Default node handler.  Simply tells the processor to go into each node.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns LSG_PD_CONTINUE.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::DefaultNodeHandler( LSG_NODE_CALLBACK &_ncData ) {
		switch ( _ncData.pndNode->nNodeType ) {
			case LSG_N_NEWSCOPESTATEMENT : {
				if ( !_ncData.psResult->Insert( 0, "{\n", 2 ) || !m_sssScopes.Push() ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				return LSG_PD_CONTINUE;
			}
			case LSG_N_FOR : {}
			case LSG_N_DOWHILE : {}
			case LSG_N_WHILE : {
				if ( !m_sssScopes.Push() ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				return LSG_PD_CONTINUE;
			}
			default : {}
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Default post-processing node handler.  Called after processing a node and its children.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns LSG_PD_CONTINUE.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::DefaultNodePostHandler( LSG_NODE_CALLBACK &_ncData ) {
		switch ( _ncData.pndNode->nNodeType ) {
			case LSG_N_DECLARATIONSTATEMENT : {}
			case LSG_N_EXPRESSIONSTATEMENT : {
				CString sFile;
				if ( _ncData.pndNode->ui32NodeFile != LSE_MAXU32 ) {
					sFile = _ncData.pslpContainer->GetString( _ncData.pndNode->ui32NodeFile );
				}
				if ( !_ncData.psResult->Insert( 0, (CString( "\n#line " ) + CString::CreateFromUI64( _ncData.pndNode->ui32NodeLine - 1 ) + CString( " // " ) + sFile + "\n").CStr() ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				return LSG_PD_CONTINUE;
			}
			case LSG_N_NEWSCOPESTATEMENT : {
				if ( _ncData.psResult->Append( "}\n" ) ) {
					m_sssScopes.Pop();
					return LSG_PD_CONTINUE;
				}
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			case LSG_N_FOR : {}
			case LSG_N_DOWHILE : {}
			case LSG_N_WHILE : {
				CString sFile;
				if ( _ncData.pndNode->ui32NodeFile != LSE_MAXU32 ) {
					sFile = _ncData.pslpContainer->GetString( _ncData.pndNode->ui32NodeFile );
				}
				if ( !_ncData.psResult->Insert( 0, (CString( "\n#line " ) + CString::CreateFromUI64( _ncData.pndNode->ui32NodeLine - 1 ) + CString( " // " ) + sFile + "\n").CStr() ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				m_sssScopes.Pop();
				return LSG_PD_CONTINUE;
			}
			case LSG_N_FUNCDEFINITION : {
				CString sFile;
				if ( _ncData.pndNode->ui32NodeFile != LSE_MAXU32 ) {
					sFile = _ncData.pslpContainer->GetString( _ncData.pndNode->ui32NodeFile );
				}
				if ( !_ncData.psResult->Insert( 0, (CString( "\n#line " ) + CString::CreateFromUI64( _ncData.pndNode->ui32NodeLine - 1 ) + CString( " // " ) + sFile + "\n").CStr() ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				m_sssScopes.Pop();
				return LSG_PD_CONTINUE;
			}
			default : {}
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Modify the final shader string after it has been completed.  Override to modify it.
	 *
	 * \param _sShader The final string, modifiable in-place.
	 * \param _sError The error string to be filled if there is an error modifying the shader text.
	 * \return Returns false if an error occurred.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::PostShaderString( CString &/*_sShader*/, CString &/*_sError*/ ) {
		return true;
	}

	/**
	 * Handles nodes of type LSG_N_EXTERNALDECLARATIONLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleExternalDeclarationList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_DISCARD.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleDiscard( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( "discard;" ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_RETURN.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleReturn( LSG_NODE_CALLBACK &_ncData ) {
		if ( !_ncData.psResult->Append( "return" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( _ncData.pndNode->nuNodeData.ui32NodeIndex != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, ";", " " ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		else {
			if ( !_ncData.psResult->Append( ";" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_BREAK.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleBreak( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( "break;" ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_CONTINUE.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleContinue( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( "continue;" ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_FOR.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleFor( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32SingleDeclName, _ncData.ui32Index, 0, "}\n", "{\n" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, 0, ")" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}

			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		else {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, ")" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}

		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, NULL, "for(" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_DOWHILE.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleDoWhile( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, ");\n", "while(" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "}\n", "do{\n" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_WHILE.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleWhile( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "}\n", "{\n" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, ")", "while(" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_FORINIT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleForInit( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_SELECTION.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleIfElse( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, 0, "}\n", "else{\n" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "}\n", "{\n" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, ")", "if(" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_FUNCDEFINITION.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleFunctionDefinition( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "}\n", "{\n" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		else {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "{}\n" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_FUNCDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		// Register the function in the current scope.
		m_bDefiningMain = false;
		LSBOOL bIsMain = m_bHaveMain;
		LSG_PROCESSING_DIRECTIVE pdReg = RegisterFunctionWithCurrentScope( _ncData );
		if ( pdReg != LSG_PD_CONTINUE ) { return pdReg; }
		if ( m_bHaveMain && !bIsMain ) {
			m_bDefiningMain = true;
		}

		// If the owner of this node is a function definition, push the parameters.
		if ( HasParentOfType( _ncData, LSG_N_FUNCDEFINITION ) ) {
			LSG_PROCESSING_DIRECTIVE pdRet = RegisterParmsWithNewScope( _ncData );
			if ( pdRet != LSG_PD_CONTINUE ) {
				return pdRet;
			}
		}

		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, 0, ")" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "(" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		else {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "()" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_PARMDECLARATIONLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleParamaterDeclarationList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_PARMDECLARATION.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleParamaterDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		if ( !DecodeTypeQualifier( _ncData.pndNode->nuNodeData.ui64Const, (*_ncData.psResult) ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_PARMDECLARATOR.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleParamaterDeclarator( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_STRUCT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleStruct( LSG_NODE_CALLBACK &_ncData ) {
		LSG_PROCESSING_DIRECTIVE pdRet = RegisterStructWithCurrentScope( _ncData );
		if ( pdRet != LSG_PD_CONTINUE ) { return pdRet; }

		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "}", "{\n" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !(*_ncData.pnsNodeStack)[_ncData.ui32Index].sNodeString.Append( "struct " ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32IdentifierIndex, _ncData.ui32Index, LSG_STACK_OBJ::LSG_B_DONOTREPLACE ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		//if ( _ncData.pndNode->nuNodeData.ui32IdentifierIndex != LSE_MAXU32 ) {
		//	// Append the name.
		//	uint32_t ui32Name = _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui32Name;
		//	if ( !(*_ncData.psResult).Append( _ncData.pslpContainer->GetString( ui32Name ) ) ) {
		//		_ncData.sError = LSG_STB_OOM;
		//		return LSG_PD_ERROR;
		//	}
		//}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_STRUCTDECLARATIONLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleStructDeclarationList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_STRUCTDECLARATION.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleStructDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			CString sTypeName;
			if ( !DecodeTypeQualifier( _ncData.pndNode->nuNodeData.ui64Const, sTypeName ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			if ( !(*_ncData.psResult).Append( sTypeName ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, ";\n" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_STRUCTDECLARATORLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleStructDeclaratorList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_STRUCTDECLARATOR.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleStructDeclarator( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "]", "[" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, LSG_STACK_OBJ::LSG_B_DONOTREPLACE ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_BLANKSTATEMENT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleBlankStatement( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_NEWSCOPESTATEMENT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleNewScopeStatement( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_STATEMENTLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleStatementList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_INITLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleInitList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_INITLISTWRAP.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleInitListWrap( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_SINGLEDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		// Add the variable to the current scope.
		LSG_PROCESSING_DIRECTIVE pdRet = RegisterVariableWithCurrentScope( _ncData );
		if ( pdRet != LSG_PD_CONTINUE ) { return pdRet; }
		// Global or local scope.  Do not allow in/out modifiers.
		uint64_t ui64ThisTypeQualifier = _ncData.pslpContainer->GetTypeQualifier( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
		if ( ui64ThisTypeQualifier & (LSG_SQ_IN | LSG_SQ_OUT) ) {
			LSG_PRINT_ERROR( "The in and out qualifiers may only be used in parameter declarations." );
			return LSG_PD_ERROR;
		}

		// Variable names may not begin with gl_ or lsg_.
		if ( CStd::StrNICmp( _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) ).CStr(),
			"gl_", 3 ) == 0 ||
			CStd::StrNICmp( _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) ).CStr(),
			"lsg_", 4 ) == 0 ) {
			LSG_PRINT_ERROR( "Variable names may not begin with \"gl_\" or \"lsg_\", regardless of case." );
			return LSG_PD_ERROR;
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_SEMANTIC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleSemantic( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_FULLYSPECIFIEDTYPE.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleFullySpecifiedType( LSG_NODE_CALLBACK &_ncData ) {
		if ( !DecodeTypeQualifier( _ncData.pndNode->nuNodeData.ui64Const, (*_ncData.psResult) ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_TYPESPECIFIERNOPREC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleTypeSpecifierNoPrecision( LSG_NODE_CALLBACK &_ncData ) {
		if ( !DecodeType( _ncData.pndNode->nuNodeData.ui64Const, (*_ncData.psResult) ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_DECLARATIONSTATEMENT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleDeclarationStatement( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_EXPRESSIONSTATEMENT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleExpressionStatement( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.pndNode->nuNodeData.ui32NodeIndex != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		// Append a ';' to this node text.
		if ( !(*_ncData.pnsNodeStack)[_ncData.ui32Index].sPostFix.Insert( 0, ";\n", 2 ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_CONSTRUCTOR.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleConstructor( LSG_NODE_CALLBACK &_ncData ) {
		CString sTypeName;
		if ( !DecodeType( _ncData.pndNode->nuNodeData.ui64Const, sTypeName ) ) {
			LSG_PRINT_ERROR( "Unknown type name." );
			return LSG_PD_ERROR;
		}
		sTypeName.RemoveWhitespace();

		if ( !_ncData.psResult->Append( sTypeName ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, ")", "(" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_FIELDSELECT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleFieldSelect( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, LSG_STACK_OBJ::LSG_B_DONOTREPLACE ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "." ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_FUNCTIONCALL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleFunctionCall( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, ")" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "(" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		else {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "()" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_ARGLIST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleArgList( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_INTRINSIC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleIntrinsic( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_COMMA.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleComma( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_ASSIGNMENT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleAssignment( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0 ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		switch ( _ncData.pndNode->nuoOp.ui32Op ) {
			case '=' : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_PLUS_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "+=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_MINUS_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "-=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_TIMES_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "*=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_DIV_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "/=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_SHIFT_LEFT_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "<<=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_SHIFT_RIGHT_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, ">>=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_AND_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "&=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_XOR_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "^=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
			case CShaderParser::token::LSG_SLT_OR_EQUALS : {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "|=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_TERTIARY.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleTertiary( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_MATH.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleMath( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, ")" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
#define LSG_PUSHME( CASE, STRING )																													\
			case CASE : {																															\
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, STRING, "(" ) ) {		\
					_ncData.sError = LSG_STB_OOM;																									\
					return LSG_PD_ERROR;																											\
				}																																	\
				break;																																\
			}
		switch ( _ncData.pndNode->nuoOp.ui32Op ) {
			LSG_PUSHME( '*', "*" )
			LSG_PUSHME( '/', "/" )
			LSG_PUSHME( '%', "%" )
			LSG_PUSHME( '+', "+" )
			LSG_PUSHME( '-', "-" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_SHIFT_LEFT, "<<" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_SHIFT_RIGHT, ">>" )
			LSG_PUSHME( '>', ">" )
			LSG_PUSHME( '<', "<" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_GE, ">=" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_LE, "<=" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_EQUALS, "==" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_NOT_EQUALS, "!=" )
			LSG_PUSHME( '&', "&" )
			LSG_PUSHME( '^', "^" )
			LSG_PUSHME( '|', "|" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_AND, "&&" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_XOR, "^^" )
			LSG_PUSHME( CShaderParser::token::LSG_SLT_OR, "||" )
		}
		return LSG_PD_SKIP_SUB_NODES;
#undef LSG_PUSHME
	}

	/**
	 * Handles nodes of type LSG_N_CAST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleCast( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_UNARY.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleUnary( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( static_cast<char>(_ncData.pndNode->nuNodeData.ui32UnaryOp) ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_PREFIXDEC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandlePrefixDecrement( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( "--" ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_PREFIXINC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandlePrefixIncrement( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( "++" ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_POSTFIXDEC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandlePostfixDecrement( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_POSTFIXINC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandlePostfixIncrement( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_ARRAYACCESS.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleArrayAccess( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_IDENTIFIER.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleIdentifier( LSG_NODE_CALLBACK &_ncData ) {
		const CString & sString = _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui32Name );
		const char * pcNew = NULL;
		if ( !(_ncData.ui32Data & LSG_STACK_OBJ::LSG_B_DONOTREPLACE) && m_sssScopes.GetNameAssociation( sString, pcNew ) ) {
			if ( _ncData.psResult->Append( pcNew ) ) {
				return LSG_PD_CONTINUE;
			}
		}
		else {
			if ( _ncData.psResult->Append( sString ) ) {
				return LSG_PD_CONTINUE;
			}
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_BOOLEAN.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleBoolean( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.pndNode->nuNodeData.ui32UnaryOp ) {
			if ( !_ncData.psResult->Append( "true" ) ) {
				return LSG_PD_ERROR;
			}
		}
		else {
			if ( !_ncData.psResult->Append( "false" ) ) {
				return LSG_PD_ERROR;
			}
		}

		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_NUMERICCONSTANT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleNumeric( LSG_NODE_CALLBACK &_ncData ) {
		switch ( _ncData.pndNode->nueNodeDataEx.ncConstantType ) {
			case LSG_NC_UNSIGNED : {
				if ( _ncData.psResult->FromUI64( _ncData.pndNode->nuNodeData.ui64Const ) ) {
					return LSG_PD_CONTINUE;
				}
				break;
			}
			case LSG_NC_SIGNED : {
				if ( _ncData.psResult->FromI64( _ncData.pndNode->nuNodeData.i64Const ) ) {
					return LSG_PD_CONTINUE;
				}
				break;
			}
			case LSG_NC_FLOATING : {
				if ( _ncData.psResult->FromDouble( _ncData.pndNode->nuNodeData.dConst ) ) {
					return LSG_PD_CONTINUE;
				}
				break;
			}
			default : {}
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_STRINGCONSTANT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::HandleString( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Register a variable with the current scope.  Node must be of type LSG_N_SINGLEDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::RegisterVariableWithCurrentScope( LSG_NODE_CALLBACK &_ncData ) {
		CShaderScope::LSG_VARIABLE vVar;
		vVar.ui32DeclNode = _ncData.pndNode->ui32NodeIndex;
		vVar.ui64Qualifiers = _ncData.pslpContainer->GetTypeQualifier( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
		vVar.ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
		vVar.ui32Name = _ncData.pslpContainer->GetNameIndex( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );
		vVar.ui32Semantic = m_sssScopes.TotalScopes() == 1 ? _ncData.pndNode->ui32Semantic : LSE_MAXU32;	// Semantics ignored on variables not at global scope.
		// Want to see the actual name?  Uncomment.
		//const char * pcName = _ncData.pslpContainer->GetString( vVar.ui32Name ).CStr();

		// If it exists as a structure already, error.
		if ( m_sssScopes.IdentifierExistsAs( CShaderScope::LSG_T_VARIABLE, _ncData.pslpContainer->GetString( vVar.ui32Name ) ) ) {
			LSG_PRINT_ERROR( (CString( "Variable \"" ) + _ncData.pslpContainer->GetString( vVar.ui32Name ) + CString( "\" already declared." )).CStr() );
			return LSG_PD_ERROR;
		}

		// Get the array size if any.
		if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
			CShaderLanguageParser::LSG_SCALAR sValue;
			LSG_NUM_CONSTANTS ncArray = _ncData.pslpContainer->GetNodeValue( _ncData.pslpContainer->GetNode( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx ), sValue );
			LSBOOL bSuccess;
			CShaderLanguageParser::LSG_SCALAR sNewValue = _ncData.pslpContainer->CastScalar( sValue,
				_ncData.pslpContainer->ConstantTypeToType( ncArray ), _ncData.pslpContainer->ConstantTypeToType( LSG_NC_UNSIGNED ), bSuccess );
			if ( !bSuccess || sNewValue.ui64Value >= LSE_MAXU32 ) {
				LSG_PRINT_ERROR( "Invalid constant array size." );
				return LSG_PD_ERROR;
			}
			vVar.ui32Array = static_cast<uint32_t>(sNewValue.ui64Value);
		}
		else {
			vVar.ui32Array = LSE_MAXU32;
		}

		if ( !m_sssScopes.GetCurrentScope()->AddVariable( _ncData.pslpContainer->GetString( vVar.ui32Name ), vVar ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		return LSG_PD_CONTINUE;
	}

	/**
	 * Register a function with the current scope.  Node must be of type LSG_N_FUNCDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::RegisterFunctionWithCurrentScope( LSG_NODE_CALLBACK &_ncData ) {
		assert( _ncData.pndNode->nNodeType == LSG_N_FUNCDECL );

		CShaderScope::LSG_FUNCTION fFunc;
		fFunc.ui32DeclNode = _ncData.pndNode->ui32NodeIndex;
		fFunc.ui32Name = _ncData.pslpContainer->GetNameIndex( _ncData.pslpContainer->GetNode( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx ) );
		fFunc.ui64Qualifiers = _ncData.pslpContainer->GetTypeQualifier( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
		fFunc.ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );

		m_vLastParms.ResetNoDealloc();
		fFunc.ui32ParmCount = _ncData.pslpContainer->CountParms( _ncData.pndNode->nuoOp.ui32NodeIndex3, &m_vLastParms );
		if ( fFunc.ui32ParmCount == LSE_MAXU32 ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		if ( !m_sssScopes.GetCurrentScope()->AddFunction( _ncData.pslpContainer->GetString( fFunc.ui32Name ), fFunc ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		if ( CStd::StrCmp( _ncData.pslpContainer->GetString( fFunc.ui32Name ).CStr(), "Main" ) == 0 ) {
			if ( m_bHaveMain ) {
				LSG_PRINT_ERROR( "The \"Main\" function cannot be overloaded." );
				return LSG_PD_ERROR;
			}

			// Verify each parameter.
			uint32_t ui32In = 0;
			uint32_t ui32Out = 0;
			// Needs to have one output of POSITION or POSITIONT.
			LSBOOL bHasPosOutput = false;
			for ( uint32_t I = m_vLastParms.Length(); I--; ) {
				uint64_t ui64Sem = _ncData.pslpContainer->GetSemantic( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) );
				if ( ui64Sem == LSE_MAXU64 ) {
					LSG_PRINT_ERROR( "All \"Main\" paramaters must have a usage type (for example, \": POSITION\")." );
					return LSG_PD_ERROR;
				}
				if ( _ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) ) & LSG_SQ_IN ) {
					++ui32In;
					//uint32_t ui32Type = static_cast<uint32_t>(ui64Sem & 0xFFFFFFFFULL);
					////uint32_t ui32Number = static_cast<uint32_t>((ui64Sem >> 32ULL) & 0xFFFFFFFFULL);
					//if ( ui32Type == CShaderParser::token::LSG_SLT_POSITION || ui32Type == CShaderParser::token::LSG_SLT_POSITIONT ) {
					//	if ( bHasPosInput ) {
					//		LSG_PRINT_ERROR( "\"Main\" can only have one input \"POSITION\" usage type." );
					//		return LSG_PD_ERROR;
					//	}
					//	//bHasPosInput = true;
					//}
					//if ( ui32Number >
				}
				if ( _ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) ) & LSG_SQ_OUT ) {
					++ui32Out;

					uint32_t ui32Type = static_cast<uint32_t>(ui64Sem & 0xFFFFFFFFULL);
					if ( ui32Type == CShaderParser::token::LSG_SLT_POSITION || ui32Type == CShaderParser::token::LSG_SLT_POSITIONT ) {
						bHasPosOutput = true;
					}
				}
			}

			// There must be at least 1 input and 1 output.
			if ( ui32In < 1 && _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
				LSG_PRINT_ERROR( "The \"Main\" function must have at least one input (IN) parameter." );
				return LSG_PD_ERROR;
			}

			if ( ui32Out < 1 ) {
				LSG_PRINT_ERROR( "The \"Main\" function must have at least one output (OUT) parameter." );
				return LSG_PD_ERROR;
			}

			// Every parameter must be either IN our OUT, and not both or neither.
			if ( ui32In + ui32Out != m_vLastParms.Length() ) {
				LSG_PRINT_ERROR( "All \"Main\" paramaters must be qualified as either IN or OUT." );
				return LSG_PD_ERROR;
			}

			// It must have at least one input argument of type POSITION or POSITIONT for vertex shaders.
			if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
				if ( !bHasPosOutput ) {
					LSG_PRINT_ERROR( "\"Main\" must have one output of usage type POSITION." );
					return LSG_PD_ERROR;
				}
			}


			m_bHaveMain = true;
			m_vMainParms = m_vLastParms;
		}

		// Want to see the actual name?  Uncomment.
		//const char * pcName = _ncData.pslpContainer->GetString( fFunc.ui32Name ).CStr();

		return LSG_PD_CONTINUE;
	}

	/**
	 * Register a structure with the current scope.  Node must be of type LSG_N_STRUCT.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::RegisterStructWithCurrentScope( LSG_NODE_CALLBACK &_ncData ) {
		assert( _ncData.pndNode->nNodeType == LSG_N_STRUCT );

		CShaderScope::LSG_STRUCT sStruct;
		sStruct.ui32DeclNode = _ncData.pndNode->ui32NodeIndex;
		sStruct.ui32Name = _ncData.pslpContainer->GetNameIndex( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ) );

		// If it exists as a structure already, error.
		if ( m_sssScopes.IdentifierExistsAs( CShaderScope::LSG_T_STRUCT, _ncData.pslpContainer->GetString( sStruct.ui32Name ) ) ) {
			LSG_PRINT_ERROR( (CString( "Struct \"" ) + _ncData.pslpContainer->GetString( sStruct.ui32Name ) + CString( "\" already defined." )).CStr() );
			return LSG_PD_ERROR;
		}

		if ( !m_sssScopes.GetCurrentScope()->AddStruct( _ncData.pslpContainer->GetString( sStruct.ui32Name ), sStruct ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		// Want to see the actual name?  Uncomment.
		//const char * pcName = _ncData.pslpContainer->GetString( sStruct.ui32Name ).CStr();

		return LSG_PD_CONTINUE;
	}

	/**
	 * Register the last parameters with a new scope.  Pushes a new scope and registers the parameters in m_vLastParms.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::RegisterParmsWithNewScope( LSG_NODE_CALLBACK &_ncData ) {
		// Push the parameters as variables.
		if ( !m_sssScopes.Push() ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}


		for ( uint32_t I = m_vLastParms.Length(); I--; ) {
			CShaderScope::LSG_VARIABLE vVar;
			vVar.ui32DeclNode = m_vLastParms[I];
			vVar.ui64Qualifiers = _ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) );
			vVar.ui64Type = _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) );
			vVar.ui32Name = _ncData.pslpContainer->GetNameIndexOfParm( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) );
			vVar.ui32Semantic = LSE_MAXU32;	// Semantics not applicable except on globals.
			// Want to see the actual name?  Uncomment.
			//const char * pcName = _ncData.pslpContainer->GetString( vVar.ui32Name ).CStr();

			// If it exists as a variable already, error.
			if ( m_sssScopes.IdentifierExistsAs( CShaderScope::LSG_T_VARIABLE, _ncData.pslpContainer->GetString( vVar.ui32Name ) ) ) {
				LSG_PRINT_ERROR( (CString( "Variable \"" ) + _ncData.pslpContainer->GetString( vVar.ui32Name ) + CString( "\" already declared." )).CStr() );
				return LSG_PD_ERROR;
			}

			// Get the array size if any.
			uint32_t ui32ArrayIndex = _ncData.pslpContainer->GetArrayIndexOfParm( _ncData.pslpContainer->GetNode( m_vLastParms[I] ) );
			if ( ui32ArrayIndex != LSE_MAXU32 ) {
				CShaderLanguageParser::LSG_SCALAR sValue;
				LSG_NUM_CONSTANTS ncArray = _ncData.pslpContainer->GetNodeValue( _ncData.pslpContainer->GetNode( ui32ArrayIndex ), sValue );
				LSBOOL bSuccess;
				CShaderLanguageParser::LSG_SCALAR sNewValue = _ncData.pslpContainer->CastScalar( sValue,
					_ncData.pslpContainer->ConstantTypeToType( ncArray ), _ncData.pslpContainer->ConstantTypeToType( LSG_NC_UNSIGNED ), bSuccess );
				if ( !bSuccess || sNewValue.ui64Value >= LSE_MAXU32 ) {
					LSG_PRINT_ERROR( "Invalid constant array size." );
					return LSG_PD_ERROR;
				}
				vVar.ui32Array = static_cast<uint32_t>(sNewValue.ui64Value);
			}
			else {
				vVar.ui32Array = LSE_MAXU32;
			}

			if ( !m_sssScopes.GetCurrentScope()->AddVariable( _ncData.pslpContainer->GetString( vVar.ui32Name ), vVar ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Decode a type qualifier to a given string.
	 *
	 * \param _ui64Qualifier The type qualifier.
	 * \param _sString The string to which to print the data.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::DecodeTypeQualifier( uint64_t /*_ui64Qualifier*/, CString &/*_sString*/ ) {
		return false;
	}

	/**
	 * Decode a type.
	 *
	 * \param _ui64Type The type information.
	 * \param _sString The string to which to print the data.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::DecodeType( uint64_t _ui64Type, CString &_sString ) {
		// Here we handle only user-defined types.
		CString sTypeName = m_pslpContainer->GetStructById( static_cast<uint32_t>(_ui64Type) );
		if ( sTypeName.Length() == 0 ) { return false; }
		_sString = sTypeName;
		return _sString.Append( " " );
	}

	/**
	 * Print an error which includes line information.
	 *
	 * \param _sError The error string to which to print the error.
	 * \param _pcError The error to print.
	 * \param _ncData Data required for adding line and file to the error report.
	 */
	void LSE_CALL CShaderTranslationProcessorBase::PrintError( CString &_sError, const char * _pcError, const LSG_NODE_CALLBACK &_ncData ) const {
		_sError = CString( "Error on line " ) + CString::CreateFromUI64( _ncData.pndNode->ui32NodeLine ) + CString( ": " ) + CString( _pcError );
		_sError.Snap();
	}

	/**
	 * Determines if the given noce is a child node of a given type.  That is, if any parent nodes to the given node have the given type.
	 *
	 * \param _ncData The node.
	 * \param _nType The type to check against all parents of the given node for a match.
	 * \return If no parents of the given node have the given type, false is returned, otherwise true is returned.
	 */
	LSBOOL LSE_CALL CShaderTranslationProcessorBase::HasParentOfType( const LSG_NODE_CALLBACK &_ncData, LSG_NODE _nType ) const {
		uint32_t ui32Parent = _ncData.ui32ParentIndex;
		while ( ui32Parent != LSE_MAXU32 ) {
			if ( (*_ncData.pnsNodeStack)[ui32Parent].ui32Index == LSE_MAXU32 ) { return false; }
			LSG_NODE nType = _ncData.pslpContainer->GetNode( (*_ncData.pnsNodeStack)[ui32Parent].ui32Index ).nNodeType;
			if ( nType == _nType ) {
				return true;
			}
			ui32Parent = (*_ncData.pnsNodeStack)[ui32Parent].ui32ParentIndex;
		}

		return false;
	}

	/**
	 * Gets a parent node of a given type.  Returns the first node found matching the given type.
	 *
	 * \param _ncData The node whose parent of the given type is to be found.
	 * \param _nType The type to check against all parents of the given node for a match.
	 * \return Returns a pointer to the parent node of the given type, if found, or NULL otherwise.
	 */
	const CShaderTranslationProcessorBase::LSG_STACK_OBJ * LSE_CALL CShaderTranslationProcessorBase::GetParentOfType( const LSG_NODE_CALLBACK &_ncData, LSG_NODE _nType ) const {
		uint32_t ui32Parent = _ncData.ui32ParentIndex;
		while ( ui32Parent != LSE_MAXU32 ) {
			if ( (*_ncData.pnsNodeStack)[ui32Parent].ui32Index == LSE_MAXU32 ) { return NULL; }
			LSG_NODE nType = _ncData.pslpContainer->GetNode( (*_ncData.pnsNodeStack)[ui32Parent].ui32Index ).nNodeType;
			if ( nType == _nType ) {
				return &(*_ncData.pnsNodeStack)[ui32Parent];
			}
			ui32Parent = (*_ncData.pnsNodeStack)[ui32Parent].ui32ParentIndex;
		}

		return NULL;
	}
	
	/**
	 * Gets a node with the given node index.
	 *
	 * \param _ncData The node.
	 * \param _ui32Index Node index/ID.
	 * \return Returns a pointer to the node with the given ID, if found, or NULL otherwise.
	 */
	 const CShaderTranslationProcessorBase::LSG_STACK_OBJ * LSE_CALL CShaderTranslationProcessorBase::FindNodeById( const LSG_NODE_CALLBACK &_ncData, uint32_t _ui32Index ) const {
		for ( uint32_t I = 0; I < (*_ncData.pnsNodeStack).Length(); ++I ) {
			uint32_t ui32Index = (*_ncData.pnsNodeStack)[I].ui32Index;
			if ( ui32Index == _ui32Index ) {
				return &(*_ncData.pnsNodeStack)[I];
			}
		}
		return NULL;
	 }

	/**
	 * Pre-call to HandleFunctionDefinition().  Sets the current function.
	 *
	 * \param _ncData The node.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::PreHandleFunctionDefinition( LSG_NODE_CALLBACK &_ncData ) {
		const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndDeclNode = _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex );
		const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndIdentifierNode = _ncData.pslpContainer->GetNode( ndDeclNode.nueNodeDataEx.ui32NodeIndexEx );
		if ( ndIdentifierNode.nNodeType != LSG_N_IDENTIFIER ) {
			return LSG_PD_ERROR;
		}
		//m_sCurFunc = _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( ndIdentifierNode.nuNodeData.ui32IdentifierIndex ).ui32Name );
		if ( !m_sFuncRefs.ItemExists( _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( ndIdentifierNode.nuNodeData.ui32IdentifierIndex ).ui32Name ) ) ) {
			m_sssScopes.Push();
			return LSG_PD_SKIP_SUB_NODES;
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Pre-call to HandleFunctionCall().  Adds a reference to the called function.
	 *
	 * \param _ncData The node.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::PreHandleFunctionCall( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		/*const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndNode = _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex );
		switch ( ndNode.nNodeType ) {
			case LSG_N_IDENTIFIER : {
				CString sFuncName = _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( ndNode.nuNodeData.ui32IdentifierIndex ).ui32Name );
				if ( !m_mmFuncRefs.Insert( m_sCurFunc, sFuncName ) ) {
					return LSG_PD_ERROR;
				}
				break;
			}
		}*/
		return LSG_PD_CONTINUE;
	}

	/**
	 * Pre-call to HandleIdentifier().  Adds references to global uniforms.
	 *
	 * \param _ncData The node.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_PROCESSING_DIRECTIVE LSE_CALL CShaderTranslationProcessorBase::PreHandleIdentifier( LSG_NODE_CALLBACK &_ncData ) {
		// Is it a uniform at all?
		const CString & sName = _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui32Name );
		CVectorPoD<CShaderScopeStack::LSG_TYPE_INDEX_AND_INDEX> vTypeAndIndex;
		if ( !m_sssScopes.GetIdentifiersAs( CShaderScope::LSG_T_VARIABLE, sName, vTypeAndIndex ) ) { return LSG_PD_CONTINUE; }

		// Locals could have the same name as globals.  If that happens, there will be more than 1 entry in the returned list.
		//	If there is only 1 entry, it is not duplicate symbol, and that is necessary for referencing globals.
		if ( vTypeAndIndex.Length() != 1 ) { return LSG_PD_CONTINUE; }

		// The scope of the single referenced variable must be 0.
		if ( vTypeAndIndex[0].ui32StackIndex != 0 ) { return LSG_PD_CONTINUE; }

		// Accessing members of structures does not count as referencing a uniform.
		// For example:
		//	uniform g_bCool;
		//	struct LSG_FUN {
		//		bool g_bCool;
		//	};
		//	LSG_FUN fFun;
		//	fFun.g_bCool;
		if ( _ncData.ui32Data & LSG_STACK_OBJ::LSG_B_DONOTREPLACE ) { return LSG_PD_CONTINUE; }

		// Find all the uniforms with this name.
		for ( uint32_t I = m_ulGlobalUniforms.Length(); I--; ) {
			if ( m_ulGlobalUniforms[I].sName == sName ) {
				++m_ulGlobalUniforms[I].ui32RefCount;
				break;
			}
		}

		return LSG_PD_CONTINUE;
	}
	
	/**
	 * Gets a pointer to an attribute by semantic and index, or NULL if the gven combination does not exist.
	 *
	 * \param _ui32Semantic The semantic.
	 * \param _ui32Index The semantic index.
	 * \param _bInput If true, inputs are found, otherwise outputs are found.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should halt or not.
	 */
	CShaderTranslationProcessorBase::LSG_ATTRIBUTE * LSE_CALL CShaderTranslationProcessorBase::FindAttribute( uint32_t _ui32Semantic, uint32_t _ui32Index, LSBOOL _bInput ) {
		for ( uint32_t I = m_alAttributes.Length(); I--; ) {
			if ( m_alAttributes[I].ui32SemanticType == _ui32Semantic && m_alAttributes[I].ui32SemanticIndex == _ui32Index && m_alAttributes[I].bInput == _bInput ) {
				return &m_alAttributes[I];
			}
		}
		return NULL;
	}

	/**
	 * Gets the length of m_gscCheckMe.
	 *
	 * \return Returns The length of m_gscCheckMe.
	 */
	uint32_t LSE_CALL CShaderTranslationProcessorBase::SemanticCheckLength() {
		return LSE_ELEMENTS( m_gscCheckMe );
	}

}	// namespace lsg
