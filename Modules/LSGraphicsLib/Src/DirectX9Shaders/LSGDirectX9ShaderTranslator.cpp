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

#include "LSGDirectX9ShaderTranslator.h"

#if defined( LSG_DX9 )

#include "../Gfx/LSGGfx.h"

/**
 * Help print errors.
 */
#define LSG_PRINT_ERROR( _pcText )	PrintError( _ncData.sError, _pcText, _ncData );

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9ShaderTranslator::CDirectX9ShaderTranslator() {
	}
	LSE_CALLCTOR CDirectX9ShaderTranslator::~CDirectX9ShaderTranslator() {
	}

	// == Functions.
	/**
	 * Modify the final shader string after it has been completed.  Override to modify it.
	 *
	 * \param _sShader The final string, modifiable in-place.
	 * \param _sError The error string to be filled if there is an error modifying the shader text.
	 * \return Returns false if an error occurred.
	 */
	LSBOOL LSE_CALL CDirectX9ShaderTranslator::PostShaderString( CString &_sShader, CString &_sError ) {
		static const char * pcHeader =
			"float shadow2dDepth( sampler2D _sSampler, float2 _vCoord ){ return tex2D( _sSampler, _vCoord ).x; }\n"
			"float tex2DShadowCompare( sampler2D _sSampler, float2 _vCoord, float _fComp ){"
				"float fDepth = shadow2dDepth( _sSampler, _vCoord );"
				"if ( fDepth == 1.0f ){ fDepth = _fComp; }"
				"return step( _fComp, fDepth ); }\n"
			"float tex2DShadow( sampler2D _sSampler, float4 _vInvTexSize, float2 _vCoord, float _fComp ){"
				"float fX = _vInvTexSize.x * 0.5f;"
				"float fY = _vInvTexSize.y * 0.5f;"
				"float fLb = tex2DShadowCompare(_sSampler,_vCoord+float2(-fX,-fY),_fComp);"
				"float fLt = tex2DShadowCompare(_sSampler,_vCoord+float2(-fX,fY),_fComp);"
				"float fRb = tex2DShadowCompare(_sSampler,_vCoord+float2(fX,-fY),_fComp);"
				"float fRt = tex2DShadowCompare(_sSampler,_vCoord+float2(fX,fY),_fComp);"
				"float2 fFrac = frac( _vCoord * _vInvTexSize.zw + float2( 0.5f, 0.5f ) );"
				"float fA = lerp( fLb, fLt, fFrac.y );"
				"float fB = lerp( fRb, fRt, fFrac.y );"
				"return lerp( fA, fB, fFrac.x );"
			"}\n"
			"float linstep( float _fLow, float _fHigh, float _fV ) { return saturate( (_fV - _fLow) / (_fHigh - _fLow) ); }\n"
			"float rcp( float _fVal ) { return 1.0f / _fVal; }\n"
			"float2 rcp( float2 _fVal ) { return float2( 1.0f / _fVal.x, 1.0f / _fVal.y ); }\n"
			"float3 rcp( float3 _fVal ) { return float3( 1.0f / _fVal.x, 1.0f / _fVal.y, 1.0f / _fVal.x ); }\n"
			"float4 rcp( float4 _fVal ) { return float4( 1.0f / _fVal.x, 1.0f / _fVal.y, 1.0f / _fVal.x, 1.0f / _fVal.w ); }\n";
		CString sHeader;
		if ( !sHeader.Insert( 0, pcHeader ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}

		if ( !_sShader.Insert( 0, sHeader.CStr(), sHeader.Length() ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}


		// Since we use registers to map samplers and textures, don't send sampler/texture data to the shader; it will try
		//	to remap them.
		for ( uint32_t I = m_ulGlobalUniforms.Length(); I--; ) {
			if ( CShaderLanguageParser::IsSampler( m_ulGlobalUniforms[I].ui64Type ) ||
				CShaderLanguageParser::IsTexture( m_ulGlobalUniforms[I].ui64Type ) ) {
				m_ulGlobalUniforms.Remove( I );
			}
		}
		return true;
	}

	/**
	 * Handles nodes of type LSG_N_FUNCDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CDirectX9ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX9ShaderTranslator::HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		m_bDefiningMain = false;
		// We need to detect if this is the main function or not.
		// If Main() has already been found, just process normally.
		if ( m_bHaveMain ) {
			return Parent::HandleFunctionDeclaration( _ncData );
		}
		// Register the function in the current scope.
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

		if ( m_bDefiningMain ) {
			for ( uint32_t I = m_vMainParms.Length(); I--; ) {
				const CString & sThisName = _ncData.pslpContainer->GetNameOfParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
				uint64_t ui64Sem = _ncData.pslpContainer->GetSemantic( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
				uint32_t ui32Name = static_cast<uint32_t>(ui64Sem & 0xFFFFFFFFULL);
				uint32_t ui32Count = static_cast<uint32_t>(ui64Sem >> 32ULL);


				if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL &&
					ui32Name == CShaderParser::token::LSG_SLT_ISFRONTFACE && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) ) {
					// The primary input must be of type bool.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The front-face bool (in ISFRONTFACE) must be of type bool." );
						return LSG_PD_ERROR;
					}
					// in ISFRONTFACE = (sThisName == 1).
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, (CString( "(" ) + sThisName + CString( "==1.0)" )).CStr() ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
			}
		}

		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, LSG_STACK_OBJ::LSG_B_DONOTREPLACE, ")" ) ) {
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


		//// If the owner of this node is a function definition, push the parameters.
		//if ( HasParentOfType( _ncData, LSG_N_FUNCDEFINITION ) ) {
		//	LSG_PROCESSING_DIRECTIVE pdRet = RegisterParmsWithNewScope( _ncData );
		//	if ( pdRet != LSG_PD_CONTINUE ) {
		//		return pdRet;
		//	}
		//}
	}

	/**
	 * Handles nodes of type LSG_N_PARMDECLARATOR.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	/*CDirectX9ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX9ShaderTranslator::HandleParamaterDeclarator( LSG_NODE_CALLBACK &_ncData ) {
	}*/

	/**
	 * Handles nodes of type LSG_N_INITLISTWRAP.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CDirectX9ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX9ShaderTranslator::HandleInitListWrap( LSG_NODE_CALLBACK &_ncData ) {
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index, 0, "}", "{" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_SINGLEDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CDirectX9ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX9ShaderTranslator::HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		// The default handler for this adds the value to the current scope.
		LSG_PROCESSING_DIRECTIVE pdRet = Parent::HandleSingleDeclaration( _ncData );
		if ( pdRet != LSG_PD_CONTINUE ) { return pdRet; }


		uint64_t ui64ThisTypeQualifier = _ncData.pslpContainer->GetTypeQualifier( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
		uint64_t ui64ThisType = 0;
		// Check the semantic (only on globals).
		if ( m_sssScopes.TotalScopes() == 1 && _ncData.pndNode->ui32Semantic != LSE_MAXU32 ) {
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndSemantic = _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32Semantic );
			
			LSG_UNIFORM_SEMANTIC usThis;
			usThis.ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
			ui64ThisType = usThis.ui64Type;
			LSBOOL bFound = false;
			for ( int32_t I = SemanticCheckLength(); I--; ) {
				if ( ndSemantic.nueNodeDataEx.ui32NodeIndexEx == m_gscCheckMe[I].ui32Semantic ) {
					if ( usThis.ui64Type != m_gscCheckMe[I].ui64Type ) {
						CString sError = CString( "The " ) + CString( m_gscCheckMe[I].szName ) + CString( " usage type can only be applied to type " ) +
							_ncData.pslpContainer->BasicTypeToString( m_gscCheckMe[I].ui64Type ) + CString( "." );
						LSG_PRINT_ERROR( sError.CStr() );
						return LSG_PD_ERROR;
					}
					if ( m_gscCheckMe[I].bIsArray ) {
						if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx == LSE_MAXU32 ) {
							CString sError = CString( "The " ) + CString( m_gscCheckMe[I].szName ) + CString( " usage type must be applied to arrays." );
							LSG_PRINT_ERROR( sError.CStr() );
							return LSG_PD_ERROR;
						}
					}
					else {
						if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
							CString sError = CString( "The " ) + CString( m_gscCheckMe[I].szName ) + CString( " usage type cannot be applied to arrays." );
							LSG_PRINT_ERROR( sError.CStr() );
							return LSG_PD_ERROR;
						}
					}
					bFound = true;
					break;
				}
			}

			if ( bFound ) {
				usThis.sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );
				usThis.sName.Snap();
				usThis.ui32SemanticType = ndSemantic.nueNodeDataEx.ui32NodeIndexEx;
				usThis.ui32SemanticIndex = ndSemantic.nuoOp.ui32NodeIndex3;
				if ( !m_uslGlobalSemantics.Push( usThis ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
			}
			else {
				switch ( ndSemantic.nueNodeDataEx.ui32NodeIndexEx ) {
					case CShaderParser::token::LSG_SLT_TEXUNIT : {
						// Must be a sampler or texture and no array.
						if ( !CShaderLanguageParser::IsSampler( usThis.ui64Type ) &&
							!CShaderLanguageParser::IsTexture( usThis.ui64Type  ) ) {
							LSG_PRINT_ERROR( "The TEXUNIT usage type can only be applied to sampler and texture types." );
							return LSG_PD_ERROR;
						}

						if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
							LSG_PRINT_ERROR( "The TEXUNIT usage type cannot be applied to an array." );
							return LSG_PD_ERROR;
						}

						if ( !CShaderLanguageParser::IsSampler( usThis.ui64Type ) ) {
							usThis.sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );
							usThis.sName.Snap();
							usThis.ui32SemanticType = ndSemantic.nueNodeDataEx.ui32NodeIndexEx;
							usThis.ui32SemanticIndex = ndSemantic.nuoOp.ui32NodeIndex3;
							if ( !m_uslGlobalSemantics.Push( usThis ) ) {
								_ncData.sError = LSG_STB_OOM;
								return LSG_PD_ERROR;
							}
						}
						break;
					}
				}
			}
		}

		if ( CShaderLanguageParser::IsSampler( ui64ThisType ) ) {
			// Samplers not invited.
			return LSG_PD_SKIP_SUB_NODES;
		}

		// Check for being a global uniform.
		if ( m_sssScopes.TotalScopes() == 1 && ui64ThisTypeQualifier & LSG_SQ_UNIFORM ) {
			if ( ui64ThisTypeQualifier & (LSG_SQ_IN | LSG_SQ_OUT) ) {
				LSG_PRINT_ERROR( "The uniform type qualifier cannot be used with the in or out qualifiers." );
				return LSG_PD_ERROR;
			}
			LSG_UNIFORM uThis;
			uThis.sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );
			uThis.sName.Snap();
			uThis.ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
			uThis.ui32Array = _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx;
			uThis.ui32Size = _ncData.pslpContainer->GetSize( uThis.ui64Type );
			uThis.uiptrApiSpecific = 0;
			uThis.uiptrApiParam = 0;
			uThis.ui32RefCount = 0;
			if ( uThis.ui32Array != LSE_MAXU32 ) {
				CShaderLanguageParser::LSG_SCALAR sValue;
				LSG_NUM_CONSTANTS ncType = _ncData.pslpContainer->GetNodeValue( _ncData.pslpContainer->GetNode( uThis.ui32Array ), sValue );
				LSBOOL bRet;
				CShaderLanguageParser::LSG_SCALAR sFinalValue = CShaderLanguageParser::CastScalar( sValue, CShaderLanguageParser::ConstantTypeToType( ncType ), CShaderLanguageParser::ConstantTypeToType( LSG_NC_UNSIGNED ), bRet );
				if ( !bRet ) {
					LSG_PRINT_ERROR( "Invalid array size." );
					return LSG_PD_ERROR;
				}
				uThis.ui32Array = static_cast<uint32_t>(sFinalValue.ui64Value);
				uThis.ui32Size *= uThis.ui32Array;
			}
			
			if ( !m_ulGlobalUniforms.Push( uThis ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}

		// Check for being an in or out.
		/*if ( m_sssScopes.TotalScopes() == 1 && ui64ThisTypeQualifier & (LSG_SQ_IN | LSG_SQ_OUT) ) {
			if ( (ui64ThisTypeQualifier & LSG_SQ_IN) && _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
				LSG_PRINT_ERROR( "Invalid array size." );
				return LSG_PD_ERROR;
			}
		}*/

		if ( _ncData.pndNode->ui32Semantic != LSE_MAXU32 ) {
			// Semantic.
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndSemantic = _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32Semantic );
			if ( ndSemantic.nueNodeDataEx.ui32NodeIndexEx == CShaderParser::token::LSG_SLT_TEXUNIT ) {
				char szBuffer[48];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ":register(s%u)", ndSemantic.nuoOp.ui32NodeIndex3 );
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32Semantic, _ncData.ui32Index, 0, szBuffer ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
			}
			if ( ndSemantic.nueNodeDataEx.ui32NodeIndexEx == CShaderParser::token::LSG_SLT_SHADOWMAPUNIT ) {
				char szBuffer[48];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ":register(s%u)", CGfx::GetMetrics().ui32MaxTexSlot - 1 );
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32Semantic, _ncData.ui32Index, 0, szBuffer ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
			}
			else {
				// Semantic.
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32Semantic, _ncData.ui32Index ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
			}
		}

		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			// Initializer.
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, 0, NULL, "=" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}

		// Optional array.
		if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "]", "[" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
		}

		// Name.
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32SingleDeclName, _ncData.ui32Index ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		// Full type.
		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32NodeIndex, _ncData.ui32Index ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_INTRINSIC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CDirectX9ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX9ShaderTranslator::HandleIntrinsic( LSG_NODE_CALLBACK &_ncData ) {
		// No intrinsic function has more than 4 arguments.
		uint32_t ui32Args[4];
		uint32_t ui32ArgCount = _ncData.pslpContainer->CountArgs( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, 4, ui32Args );
		if ( ui32ArgCount == LSE_MAXU32 ) {
			LSG_PRINT_ERROR( "Too many parameters for intrinsic function." );
			return LSG_PD_ERROR;
		}

		static const struct LSG_INTRINSIC {
			/** Function name. */
			char					szName[32];
			/** Intrinsic identifier. */
			uint32_t				ui32Id;
			/** Number of arguments. */
			uint32_t				ui32Args;
		} sFuncs[] = {
			{ "abs",				CShaderParser::token::LSG_SLT_ABS,					1 },
			{ "acos",				CShaderParser::token::LSG_SLT_ACOS,					1 },
			{ "any",				CShaderParser::token::LSG_SLT_ANY,					1 },
			{ "asin",				CShaderParser::token::LSG_SLT_ASIN,					1 },
			{ "atan",				CShaderParser::token::LSG_SLT_ATAN,					1 },
			{ "atan2",				CShaderParser::token::LSG_SLT_ATAN2,				2 },
			{ "ceil",				CShaderParser::token::LSG_SLT_CEIL,					1 },
			{ "clamp",				CShaderParser::token::LSG_SLT_CLAMP,				3 },
			{ "cos",				CShaderParser::token::LSG_SLT_COS,					1 },
			{ "cross",				CShaderParser::token::LSG_SLT_CROSS,				2 },
			{ "ddx",				CShaderParser::token::LSG_SLT_DDX,					1 },
			{ "ddy",				CShaderParser::token::LSG_SLT_DDY,					1 },
			{ "degrees",			CShaderParser::token::LSG_SLT_DEGREES,				1 },
			{ "determinant",		CShaderParser::token::LSG_SLT_DETERMINANT,			1 },
			{ "distance",			CShaderParser::token::LSG_SLT_DISTANCE,				2 },
			{ "dot",				CShaderParser::token::LSG_SLT_DOT,					2 },
			{ "exp",				CShaderParser::token::LSG_SLT_EXP,					1 },
			{ "exp2",				CShaderParser::token::LSG_SLT_EXP2,					1 },
			{ "floor",				CShaderParser::token::LSG_SLT_FLOOR,				1 },
			{ "frac",				CShaderParser::token::LSG_SLT_FRAC,					1 },
			{ "isinf",				CShaderParser::token::LSG_SLT_ISINF,				1 },
			{ "isnan",				CShaderParser::token::LSG_SLT_ISNAN,				1 },
			{ "length",				CShaderParser::token::LSG_SLT_LENGTH,				1 },
			{ "lerp",				CShaderParser::token::LSG_SLT_LERP,					3 },
			{ "log",				CShaderParser::token::LSG_SLT_LOG,					1 },
			{ "log2",				CShaderParser::token::LSG_SLT_LOG2,					1 },
			{ "max",				CShaderParser::token::LSG_SLT_MAX,					2 },
			{ "min",				CShaderParser::token::LSG_SLT_MIN,					2 },
			{ "lerp",				CShaderParser::token::LSG_SLT_MIX,					3 },
			{ "fmod",				CShaderParser::token::LSG_SLT_MOD,					2 },
			{ "mul",				CShaderParser::token::LSG_SLT_MUL,					2 },
			{ "normalize",			CShaderParser::token::LSG_SLT_NORMALIZE,			1 },
			{ "pow",				CShaderParser::token::LSG_SLT_POW,					2 },
			{ "radians",			CShaderParser::token::LSG_SLT_RADIANS,				1 },
			{ "rcp",				CShaderParser::token::LSG_SLT_RCP,					1 },
			{ "reflect",			CShaderParser::token::LSG_SLT_REFLECT,				2 },
			{ "refract",			CShaderParser::token::LSG_SLT_REFRACT,				3 },
			{ "rsqrt",				CShaderParser::token::LSG_SLT_RSQRT,				1 },
			{ "saturate",			CShaderParser::token::LSG_SLT_SATURATE,				1 },
			{ "sin",				CShaderParser::token::LSG_SLT_SIN,					1 },
			{ "smoothstep",			CShaderParser::token::LSG_SLT_SMOOTHSTEP,			3 },
			{ "sqrt",				CShaderParser::token::LSG_SLT_SQRT,					1 },
			{ "step",				CShaderParser::token::LSG_SLT_STEP,					2 },
			{ "tan",				CShaderParser::token::LSG_SLT_TAN,					1 },
			{ "tex1D",				CShaderParser::token::LSG_SLT_TEX1D,				3 },
			{ "tex2D",				CShaderParser::token::LSG_SLT_TEX2D,				3 },
			{ "tex3D",				CShaderParser::token::LSG_SLT_TEX3D,				3 },
			{ "texCUBE",			CShaderParser::token::LSG_SLT_TEXCUBE,				3 },
			{ "tex2DShadowCompare",	CShaderParser::token::LSG_SLT_TEX2DSHADOWCOMPARE,	4 },
			{ "tex2DShadow",		CShaderParser::token::LSG_SLT_TEX2DSHADOW,			5 },
			{ "transpose",			CShaderParser::token::LSG_SLT_TRANSPOSE,			1 },
		};

		for ( uint32_t I = LSE_ELEMENTS( sFuncs ); I--; ) {
			if ( sFuncs[I].ui32Id == _ncData.pndNode->nuNodeData.ui32NodeIndex ) {
				// Found it.
				// Verify the number of parameters.
				char szPrefix[16];
				if ( sFuncs[I].ui32Args != ui32ArgCount ) {
					LSG_PRINT_ERROR( "Intrinsic function " );
					_ncData.sError += sFuncs[I].szName;
					_ncData.sError += "() requires ";
					_ncData.sError += CStd::UI32toA( sFuncs[I].ui32Args, szPrefix );
					_ncData.sError += " parameters.";
					return LSG_PD_ERROR;
				}

				CStd::SPrintF( szPrefix, LSE_ELEMENTS( szPrefix ), "%s(", sFuncs[I].szName );

				if ( (_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEX1D) ||
					(_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEX2D) ||
					(_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEX3D) ||
					(_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEXCUBE) ) {

					// texX( TextureX, Sampler, Uv ) = texX(ui32Args[2],ui32Args[0])

					// ui32Args[0])
					if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, ")", NULL ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					// texX(ui32Args[2]
					if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[2], _ncData.ui32Index, 0, ",", szPrefix ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}

					break;
				}

				switch ( sFuncs[I].ui32Args ) {
					case 1 : {
						// Only one parameter.
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, ")", szPrefix ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}
					case 2 : {
						// Two parameters.
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, ")", "," ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[1], _ncData.ui32Index, 0, NULL, szPrefix ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}
					case 3 : {
						// Three parameters.
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, ")", "," ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[1], _ncData.ui32Index, 0, NULL, "," ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[2], _ncData.ui32Index, 0, NULL, szPrefix ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}
					case 4 : {
						// Four parameters.
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, ")", "," ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[1], _ncData.ui32Index, 0, NULL, "," ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[2], _ncData.ui32Index, 0, NULL, "," ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[3], _ncData.ui32Index, 0, NULL, szPrefix ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}
				}
			}
		}

		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_SEMANTIC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CDirectX9ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX9ShaderTranslator::HandleSemantic( LSG_NODE_CALLBACK &_ncData ) {
		static const struct LSG_DIRECTX9_SEMANTIC {
			/** Name of the semantic. */
			char									szName[24];
			/** Semantic type. */
			uint32_t								ui32TypeId;
		} dsSemanticNames[] = {
			{ "BINORMAL%u",							CShaderParser::token::LSG_SLT_BINORMAL },
			{ "BLENDINDICES%u",						CShaderParser::token::LSG_SLT_BLENDINDICES },
			{ "BLENDWEIGHT%u",						CShaderParser::token::LSG_SLT_BLENDWEIGHT },
			{ "COLOR%u",							CShaderParser::token::LSG_SLT_COLOR },
			{ "NORMAL%u",							CShaderParser::token::LSG_SLT_NORMAL },
			{ "POSITION%u",							CShaderParser::token::LSG_SLT_POSITION },
			{ "POSITIONT",							CShaderParser::token::LSG_SLT_POSITIONT },
			{ "VFACE",								CShaderParser::token::LSG_SLT_ISFRONTFACE },
			{ "PSIZE%u",							CShaderParser::token::LSG_SLT_PSIZE },
			{ "TANGENT%u",							CShaderParser::token::LSG_SLT_TANGENT },
			{ "TEXCOORD%u",							CShaderParser::token::LSG_SLT_TEXCOORD },
			{ "FOG",								CShaderParser::token::LSG_SLT_FOG },
			{ "DEPTH",								CShaderParser::token::LSG_SLT_DEPTH },
		};

		for ( uint32_t I = LSE_ELEMENTS( dsSemanticNames ); I--; ) {
			if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx == dsSemanticNames[I].ui32TypeId ) {
				char szBuffer[48];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), dsSemanticNames[I].szName, _ncData.pndNode->nuoOp.ui32NodeIndex3 );
				if ( !_ncData.psResult->Append( ":" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				if ( !_ncData.psResult->Append( szBuffer ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				break;
			}
		}

		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Decode a type qualifier to a given string.
	 *
	 * \param _ui64Qualifier The type qualifier.
	 * \param _sString The string to which to print the data.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CDirectX9ShaderTranslator::DecodeTypeQualifier( uint64_t _ui64Qualifier, CString &_sString ) {
		uint64_t ui64ExclusionBits = 0ULL;
		static const struct LSG_TYPE_QUALIFIERS {
			/** Bit. */
			uint64_t					ui64Bit;

			/** Name. */
			char						szName[20];
		} sTypeQualifiers[] = {
			{ LSG_SQ_CONST,				"const " },
			{ LSG_SQ_IN | LSG_SQ_OUT,	"inout " },
			{ LSG_SQ_IN,				"in " },
			{ LSG_SQ_OUT,				"out " },
			{ LSG_IQ_SMOOTH,			"linear " },
			{ LSG_IQ_FLAT,				"nointerpolation " },
		};
		for ( uint32_t I = 0; I < LSE_ELEMENTS( sTypeQualifiers ); I++ ) {
			if ( ((_ui64Qualifier & sTypeQualifiers[I].ui64Bit) == sTypeQualifiers[I].ui64Bit) &&
				(sTypeQualifiers[I].ui64Bit & ui64ExclusionBits) == 0 ) {
				if ( !_sString.Append( sTypeQualifiers[I].szName ) ) {
					return false;
				}
				ui64ExclusionBits |= sTypeQualifiers[I].ui64Bit;
			}
		}
		return true;
	}

	/**
	 * Decode a type.
	 *
	 * \param _ui64Type The type information.
	 * \param _sString The string to which to print the data.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CDirectX9ShaderTranslator::DecodeType( uint64_t _ui64Type, CString &_sString ) {
		static const struct LSG_TYPES {
			/** Basic type to find in the input. */
			uint32_t					ui32Type;
			/** Name if not vectored/matrixed. */
			char						szScalarName[16];
			/** Name if vectored. */
			char						szArrayName[20];
			/** Name if matrixed. */
			char						szMatName[24];
		} sType[] = {
			{ CShaderParser::token::LSG_SLT_VOID,				"void ",		"",						"" },
			{ CShaderParser::token::LSG_SLT_SAMPLER,			"",				"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE1D,			"sampler1D ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE2D,			"sampler2D ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE3D,			"sampler3D ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURECUBE,		"samplerCUBE ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_FLOAT,				"float ",		"vector<float,%u>",		"matrix<float,%u,%u>" },
			{ CShaderParser::token::LSG_SLT_INT,				"int ",			"vector<int,%u>",		"" },
			{ CShaderParser::token::LSG_SLT_UINT,				"uint ",		"vector<uint,%u>",		"" },
			{ CShaderParser::token::LSG_SLT_BOOL,				"bool ",		"vector<bool,%u>",		"" },
		};
		uint32_t ui32Type = CShaderLanguageParser::GetType( _ui64Type );
		for ( uint32_t I = 0; I < LSE_ELEMENTS( sType ); I++ ) {
			if ( sType[I].ui32Type == ui32Type ) {
				char szBuffer[32];
				if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) == LSE_MAXU32 && CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) == LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szScalarName );
					return _sString.Append( szBuffer );
				}
				if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 && CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) == LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szArrayName, CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) );
					return _sString.Append( szBuffer );
				}
				if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 && CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) != LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szMatName,
						CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ), CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) );
					return _sString.Append( szBuffer );
				}
				return false;
			}
		}

		// It is a user type.
		return Parent::DecodeType( _ui64Type, _sString );
	}

}	// namespace lsg

#endif	// #if defined( LSG_DX9 )
