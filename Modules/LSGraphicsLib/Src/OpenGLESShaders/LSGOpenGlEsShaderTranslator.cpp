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
 * Description: Translates from the engine shader language to GLSL.
 */

#include "LSGOpenGlEsShaderTranslator.h"
#include "../Gfx/LSGGfx.h"

#if defined( LSG_OGLES )

/**
 * Help print errors.
 */
#define LSG_PRINT_ERROR( _pcText )	PrintError( _ncData.sError, _pcText, _ncData );


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsShaderTranslator::COpenGlEsShaderTranslator() :
		m_bIsMainNow( false ) {
	}
	LSE_CALLCTOR COpenGlEsShaderTranslator::~COpenGlEsShaderTranslator() {
	}

	// == Functions.
	/**
	 * Modify the final shader string after it has been completed.  Override to modify it.
	 *
	 * \param _sShader The final string, modifiable in-place.
	 * \param _sError The error string to be filled if there is an error modifying the shader text.
	 * \return Returns false if an error occurred.
	 */
	LSBOOL LSE_CALL COpenGlEsShaderTranslator::PostShaderString( CString &_sShader, CString &_sError ) {
		static const char * pcHeader = "precision mediump float;\n"
			"#ifdef GL_OES_standard_derivatives\n"
			"#extension GL_OES_standard_derivatives : enable\n"
			"#endif	// #ifdef GL_OES_standard_derivatives\n"
			"float saturate(float _fX){return clamp(_fX,0.0,1.0);}\n"
			"vec2 saturate(vec2 _vX){return clamp(_vX,vec2(0.0,0.0),vec2(1.0,1.0));}\n"
			"vec3 saturate(vec3 _vX){return clamp(_vX,vec3(0.0,0.0,0.0),vec3(1.0,1.0,1.0));}\n"
			"vec4 saturate(vec4 _vX){return clamp(_vX,vec4(0.0,0.0,0.0,0.0),vec4(1.0,1.0,1.0,1.0));}\n"
			"bool isnan(float _fVal){return _fVal!=_fVal;}\n"
			"bvec2 isnan(vec2 _vVal){return bvec2(_vVal.x!=_vVal.x,_vVal.y!=_vVal.y);}\n"
			"bvec3 isnan(vec3 _vVal){return bvec3(_vVal.x!=_vVal.x,_vVal.y!=_vVal.y,_vVal.z!=_vVal.z);}\n"
			"bvec4 isnan(vec4 _vVal){return bvec4(_vVal.x!=_vVal.x,_vVal.y!=_vVal.y,_vVal.z!=_vVal.z,_vVal.w!=_vVal.w);}\n"
			"bool isinf(float _fVal){return false;}\n"
			"bvec2 isinf(vec2 _vVal){return bvec2(false,false);}\n"
			"bvec3 isinf(vec3 _vVal){return bvec3(false,false,false);}\n"
			"bvec4 isinf(vec4 _vVal){return bvec4(false,false,false,false);}\n"
			"float rcp(float _fVal){return 1.0/_fVal;}\n"
			"vec2 rcp(vec2 _fVal){return vec2(1.0/_fVal.x,1.0/_fVal.y);}\n"
			"vec3 rcp(vec3 _fVal){return vec3(1.0/_fVal.x,1.0/_fVal.y,1.0/_fVal.z);}\n"
			"vec4 rcp(vec4 _fVal){return vec4(1.0/_fVal.x,1.0/_fVal.y,1.0/_fVal.z,1.0/_fVal.w);}\n"
			/*"float dFdx(float _fVal){return 0.0;}\n"
			"vec2 dFdx(vec2 _fVal){return vec2(0.0,0.0);}\n"
			"vec3 dFdx(vec3 _fVal){return vec3(0.0,0.0,0.0);}\n"
			"vec4 dFdx(vec4 _fVal){return vec4(0.0,0.0,0.0,0.0);}\n"
			"float dFdy(float _fVal){return 0.0;}\n"
			"vec2 dFdy(vec2 _fVal){return vec2(0.0,0.0);}\n"
			"vec3 dFdy(vec3 _fVal){return vec3(0.0,0.0,0.0);}\n"
			"vec4 dFdy(vec4 _fVal){return vec4(0.0,0.0,0.0,0.0);}\n"*/;
		CString sHeader;
		if ( !sHeader.Insert( 0, pcHeader ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( m_pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
			sHeader += "invariant gl_Position;\n";
		}
		else if ( m_pslpContainer->GetShaderType() == LSG_ST_PIXEL ) {
			sHeader += CString( "varying vec4 lsg_vColor[" ) + CString::CreateFromUI64( CGfx::GetMetrics().ui32MaxRenderTargets ) + CString( "];\n" );
		}

		// Insert shadowmapping intrinsics.
		sHeader += "float shadow2dDepth(sampler2D s,vec2 v){return texture2D(s,v).z;}\n"
			"float tex2DShadowCompare( sampler2D _sSampler, vec2 _vCoord, float _fComp ){"
				"float fDepth = shadow2dDepth( _sSampler, _vCoord );"
				"if ( fDepth == 1.0 ){ fDepth = _fComp; }"
				"return step( _fComp, fDepth ); }\n"
			"float tex2DShadow( sampler2D _sSampler, vec4 _vInvTexSize, vec2 _vCoord, float _fComp){"
				"float fX = _vInvTexSize.x * 0.5;"
				"float fY = _vInvTexSize.y * 0.5;"
				"float fLb = tex2DShadowCompare(_sSampler,_vCoord+vec2(-fX,-fY),_fComp);"
				"float fLt = tex2DShadowCompare(_sSampler,_vCoord+vec2(-fX,fY),_fComp);"
				"float fRb = tex2DShadowCompare(_sSampler,_vCoord+vec2(fX,-fY),_fComp);"
				"float fRt = tex2DShadowCompare(_sSampler,_vCoord+vec2(fX,fY),_fComp);"
				"vec2 fFrac = fract( _vCoord * _vInvTexSize.zw + vec2( 0.5, 0.5 ) );"
				"float fA = mix( fLb, fLt, fFrac.y );"
				"float fB = mix( fRb, fRt, fFrac.y );"
				"return mix( fA, fB, fFrac.x );"
			"}\n"
			"float linstep( float fLow, float fHigh, float fV ) { return clamp( (fV - fLow) / (fHigh - fLow), 0.0, 1.0 ); }\n";



		// Declare attributes.
		for ( uint32_t I = m_alAttributes.Length(); I--; ) {
			// If we have multiple render targets it is possible that we will skip the name
			//	of the uniform entirely.
			if ( m_bMultiPixelOutputs ) {
				if ( m_alAttributes[I].ui32SemanticType == CShaderParser::token::LSG_SLT_COLOR &&
					(m_alAttributes[I].bInput && m_pslpContainer->GetShaderType() == LSG_ST_VERTEX) ) { continue; }
			}


			if ( m_alAttributes[I].bInput ) {
				if ( m_pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
					if ( !sHeader.Append( "attribute " ) ) {
						_sError = LSG_STB_OOM;
						return false;
					}
				}
				else {
					if ( !sHeader.Append( "varying " ) ) {
						_sError = LSG_STB_OOM;
						return false;
					}
				}
			}
			else {
				if ( m_pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
					if ( !sHeader.Append( "varying " ) ) {
						_sError = LSG_STB_OOM;
						return false;
					}
				}
				else {
					// Nothing in fragment shader outputs.
					continue;
				}
			}

			if ( !DecodeType( m_alAttributes[I].ui64Type, sHeader ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}

			if ( (m_alAttributes[I].bInput && m_pslpContainer->GetShaderType() == LSG_ST_PIXEL) ||
				(!m_alAttributes[I].bInput && m_pslpContainer->GetShaderType() == LSG_ST_VERTEX) ) {
				CString sNewName = CString( "LSG_VERT_OUT_PIXEL_IN_" ) + CString::CreateFromUI64( m_alAttributes[I].ui32SemanticType ) + CString( "_" ) + CString::CreateFromUI64( m_alAttributes[I].ui32SemanticIndex ) +
					CString( "/*" ) + m_alAttributes[I].sName + CString( "*/" );
				if ( !sHeader.Append( sNewName ) ) {
					_sError = LSG_STB_OOM;
					return false;
				}
			}
			else {
				if ( !sHeader.Append( m_alAttributes[I].sName ) ) {
					_sError = LSG_STB_OOM;
					return false;
				}
			}


			if ( !sHeader.Append( ";\n" ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}
		}


		if ( !_sShader.Insert( 0, sHeader.CStr(), sHeader.Length() ) ) {
			_sError = LSG_STB_OOM;
			return false;
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
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		m_bIsMainNow = false;
		// We need to detect if this is the main function or not.
		// If Main() has already been found, just process normally.
		if ( m_bHaveMain ) {
			return Parent::HandleFunctionDeclaration( _ncData );
		}
		// Until the main function is found, have to handle this ourselves.
		// Register the function in the current scope.
		LSG_PROCESSING_DIRECTIVE pdReg = RegisterFunctionWithCurrentScope( _ncData );
		if ( pdReg != LSG_PD_CONTINUE ) { return pdReg; }

		// This is the Main() function.  We have some refactoring to do.
		m_bIsMainNow = m_bHaveMain;


		// If the owner of this node is a function definition, push the parameters.
		if ( HasParentOfType( _ncData, LSG_N_FUNCDEFINITION ) ) {
			LSG_PROCESSING_DIRECTIVE pdRet = RegisterParmsWithNewScope( _ncData );
			if ( pdRet != LSG_PD_CONTINUE ) {
				return pdRet;
			}
		}


		if ( m_bIsMainNow ) {
			if ( !_ncData.psResult->Append( "void main()" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}

			// Paramaters are all kept global in scope, but must be renamed.
			LSBOOL bHaveOutPos = false;
			for ( uint32_t I = m_vMainParms.Length(); I--; ) {
				const CString & sThisName = _ncData.pslpContainer->GetNameOfParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
				uint64_t ui64Sem = _ncData.pslpContainer->GetSemantic( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
				uint32_t ui32Name = static_cast<uint32_t>(ui64Sem & 0xFFFFFFFFULL);
				uint32_t ui32Count = static_cast<uint32_t>(ui64Sem >> 32ULL);

				// Not allowed to have inout on Main().
				if ( (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) ) {
					LSG_PRINT_ERROR( "Parameters in the Main() function can be in our out, but not both." );
					return LSG_PD_ERROR;
				}

				if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX &&
					ui32Name == CShaderParser::token::LSG_SLT_POSITION && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					// The primary output must be of type vec4.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The primary output position must be of type float[4]." );
						return LSG_PD_ERROR;
					}
					// out POSITION0 = gl_Position.
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, "gl_Position" ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					bHaveOutPos = true;
				}
				else if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX &&
					ui32Name == CShaderParser::token::LSG_SLT_PSIZE && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					// The primary output must be of type vec4.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The primary output point size (out PSIZE0) must be of type float." );
						return LSG_PD_ERROR;
					}
					// out PSIZE0 = gl_PointSize.
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, "gl_PointSize" ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				else if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL &&
					ui32Name == CShaderParser::token::LSG_SLT_DEPTH && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					// The primary output must be of type float.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The primary output point size (out DEPTH) must be of type float." );
						return LSG_PD_ERROR;
					}
					// out DEPTH = gl_FragDepth.
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, "gl_FragDepth" ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				else if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL &&
					ui32Name == CShaderParser::token::LSG_SLT_COLOR && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					// The primary output must be of type float.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The primary output color (out COLOR0) must be of type float[4]." );
						return LSG_PD_ERROR;
					}
					// out COLOR0 = gl_FragColor.
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, "gl_FragColor" ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				else if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL &&
					ui32Name == CShaderParser::token::LSG_SLT_VPOS && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) ) {
					// The primary input must be of type vec2.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 2, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The screen position (in VPOS) must be of type float[2]." );
						return LSG_PD_ERROR;
					}
					// in VPOS0 = gl_FragCoord.xy.
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, "gl_FragCoord.xy" ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				else if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL &&
					ui32Name == CShaderParser::token::LSG_SLT_ISFRONTFACE && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) ) {
					// The primary input must be of type bool.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The front-face bool (in ISFRONTFACE) must be of type bool." );
						return LSG_PD_ERROR;
					}
					// in ISFRONTFACE = gl_FrontFacing.
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, "gl_FrontFacing" ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				else {
					LSG_ATTRIBUTE aAttr;
					aAttr.sName = sThisName;
					aAttr.sName.Snap();
					aAttr.ui32SemanticType = ui32Name;
					aAttr.ui32SemanticIndex = ui32Count;
					aAttr.ui64Type = _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
					aAttr.bInput = (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) != 0;
					if ( !m_alAttributes.Push( aAttr ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					if ( (aAttr.bInput && _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL) ||
						(!aAttr.bInput && _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX) ) {
						CString sNewName = CString( "LSG_VERT_OUT_PIXEL_IN_" ) + CString::CreateFromUI64( aAttr.ui32SemanticType ) + CString( "_" ) + CString::CreateFromUI64( aAttr.ui32SemanticIndex ) +
							CString( "/*" ) + sThisName + CString( "*/" );
						if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, sNewName.CStr() ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
					}
				}

				// Color outputs other than index 0 indicate multiple render targets.
				if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL &&
					ui32Name == CShaderParser::token::LSG_SLT_COLOR && ui32Count != 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					if ( !m_bMultiPixelOutputs ) {
						// Find COLOR0 and rename it.
						for ( uint32_t K = m_alAttributes.Length(); K--; ) {
							if ( m_alAttributes[K].ui32SemanticType == CShaderParser::token::LSG_SLT_COLOR && m_alAttributes[K].ui32SemanticIndex == 0 ) {
								// out COLOR0 = lsg_vColor[0].
								if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( m_alAttributes[K].sName, "lsg_vColor[0]" ) ) {
									_ncData.sError = LSG_STB_OOM;
									return LSG_PD_ERROR;
								}
								break;
							}
						}
					}
					m_bMultiPixelOutputs = true;
					
				}

				// Once multiple render targets are detected on a pixel shader, all color outputs get renamed.
				if ( m_bMultiPixelOutputs && ui32Name == CShaderParser::token::LSG_SLT_COLOR && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					// out COLORn = lsg_vColor[n].
					char szBuffer[32];
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "lsg_vColor[%u]", ui32Count );
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, szBuffer ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
			}
			if ( !bHaveOutPos && _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
				LSG_PRINT_ERROR( "An output of type POSITION0 must be used and all components written." );
				return LSG_PD_ERROR;
			}
		}
		else {

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
		}
		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_PARMDECLARATOR.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleParamaterDeclarator( LSG_NODE_CALLBACK &/*_ncData*/ ) {
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_SINGLEDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		// The default handler for this adds the value to the current scope.
		LSG_PROCESSING_DIRECTIVE pdRet = Parent::HandleSingleDeclaration( _ncData );
		if ( pdRet != LSG_PD_CONTINUE ) { return pdRet; }

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
					/*case CShaderParser::token::LSG_SLT_LIGHTATT : {
						if ( usThis.ui64Type != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ) ) {
							LSG_PRINT_ERROR( "The LIGHTATT usage type can only be applied to float[4] or vector types." );
							return LSG_PD_ERROR;
						}

						if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx == LSE_MAXU32 ) {
							LSG_PRINT_ERROR( "The LIGHTATT usage type must be applied to an array." );
							return LSG_PD_ERROR;
						}

						usThis.sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );
						usThis.sName.Snap();
						usThis.ui32SemanticType = ndSemantic.nueNodeDataEx.ui32NodeIndexEx;
						usThis.ui32SemanticIndex = ndSemantic.nuoOp.ui32NodeIndex3;
						if ( !m_uslGlobalSemantics.Push( usThis ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}*/
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
			// Blank statements on global scope are not allowed.
			if ( m_sssScopes.TotalScopes() == 1 ) {
				if ( !_ncData.psResult->Append( "//" ) ) { return LSG_PD_ERROR; }
				if ( !_ncData.psResult->Append( _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) ) ) ) { return LSG_PD_ERROR; }
			}
			return LSG_PD_SKIP_SUB_NODES;
		}

		// Check for being a global uniform.
		if ( m_sssScopes.TotalScopes() == 1 && _ncData.pslpContainer->GetTypeQualifier( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) ) & LSG_SQ_UNIFORM ) {
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

		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			// Create a constructor.
			CString sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );

			uint64_t ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
			LSG_NODE_TYPE_LIST ntlTypes;
			LSBOOL bIsExplicit = GetNodeType( _ncData.pndNode->nuoOp.ui32NodeIndex3, ntlTypes );
			if ( bIsExplicit ) {
				bIsExplicit = false;
				// Check each possible type for a real match.
				for ( uint32_t I = ntlTypes.Length(); I--; ) {
					if ( ntlTypes[I].ui64Type == ui64Type && ntlTypes[I].bIsExplicit ) {
						bIsExplicit = true;
						break;
					}
				}
			}
			// If the type is explicit, do not convert to a constructor.
			if ( bIsExplicit ) {
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, 0, NULL, "=" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
			}
			else {
				CString sTypeName;
				if ( !DecodeType( ui64Type, sTypeName ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				sTypeName.RemoveWhitespace();
				if ( !sTypeName.Insert( 0, "=", 1 ) || !sTypeName.Append( '(' ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index, 0, ")", sTypeName.CStr() ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
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
	 * Handles nodes of type LSG_N_FULLYSPECIFIEDTYPE.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleFullySpecifiedType( LSG_NODE_CALLBACK &_ncData ) {
		if ( !DecodeTypeQualifier( _ncData.pndNode->nuNodeData.ui64Const, (*_ncData.psResult) ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_CONTINUE;
		/*if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_SKIP_SUB_NODES;*/
	}

	/**
	 * Handles nodes of type LSG_N_INTRINSIC.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleIntrinsic( LSG_NODE_CALLBACK &_ncData ) {
		// No intrinsic function has more than 3 arguments.
		uint32_t ui32Args[3];
		uint32_t ui32ArgCount = _ncData.pslpContainer->CountArgs( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, 3, ui32Args );
		if ( ui32ArgCount == LSE_MAXU32 ) {
			LSG_PRINT_ERROR( "Too many parameters for intrinsic function." );
			return LSG_PD_ERROR;
		}
		
		switch ( _ncData.pndNode->nuNodeData.ui32NodeIndex ) {
			case CShaderParser::token::LSG_SLT_MUL : {
				// Handle the special case of mul() which needs to become Arg0 * Arg1.
				if ( ui32ArgCount != 2 ) {
					LSG_PRINT_ERROR( "Intrinsic function mul() requires 2 parameters." );
					return LSG_PD_ERROR;
				}

				// Right side.
				if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, "))", "(" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}

				// Left side.
				if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[1], _ncData.ui32Index, 0, ")*", "((" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				return LSG_PD_SKIP_SUB_NODES;
			}
			case CShaderParser::token::LSG_SLT_SINCOS : {
				// Handle the special case of sincos() which needs to become Arg1=sin(Arg0);Arg2=cos(Arg0);
				if ( ui32ArgCount != 3 ) {
					LSG_PRINT_ERROR( "Intrinsic function sincos() requires 3 parameters." );
					return LSG_PD_ERROR;
				}


				return LSG_PD_SKIP_SUB_NODES;
			}
		}

		// All other functions behave like regular functions.
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
			{ "atan",				CShaderParser::token::LSG_SLT_ATAN2,				2 },
			{ "ceil",				CShaderParser::token::LSG_SLT_CEIL,					1 },
			{ "clamp",				CShaderParser::token::LSG_SLT_CLAMP,				3 },
			{ "cos",				CShaderParser::token::LSG_SLT_COS,					1 },
			{ "cross",				CShaderParser::token::LSG_SLT_CROSS,				2 },
			{ "dFdx",				CShaderParser::token::LSG_SLT_DDX,					1 },
			{ "dFdy",				CShaderParser::token::LSG_SLT_DDY,					1 },
			{ "degrees",			CShaderParser::token::LSG_SLT_DEGREES,				1 },
			{ "determinant",		CShaderParser::token::LSG_SLT_DETERMINANT,			1 },
			{ "distance",			CShaderParser::token::LSG_SLT_DISTANCE,				2 },
			{ "dot",				CShaderParser::token::LSG_SLT_DOT,					2 },
			{ "exp",				CShaderParser::token::LSG_SLT_EXP,					1 },
			{ "exp2",				CShaderParser::token::LSG_SLT_EXP2,					1 },
			{ "floor",				CShaderParser::token::LSG_SLT_FLOOR,				1 },
			{ "fract",				CShaderParser::token::LSG_SLT_FRAC,					1 },
			{ "isinf",				CShaderParser::token::LSG_SLT_ISINF,				1 },
			{ "isnan",				CShaderParser::token::LSG_SLT_ISNAN,				1 },
			{ "length",				CShaderParser::token::LSG_SLT_LENGTH,				1 },
			{ "mix",				CShaderParser::token::LSG_SLT_LERP,					3 },
			{ "log",				CShaderParser::token::LSG_SLT_LOG,					1 },
			{ "log2",				CShaderParser::token::LSG_SLT_LOG2,					1 },
			{ "max",				CShaderParser::token::LSG_SLT_MAX,					2 },
			{ "min",				CShaderParser::token::LSG_SLT_MIN,					2 },
			{ "mix",				CShaderParser::token::LSG_SLT_MIX,					3 },
			{ "mod",				CShaderParser::token::LSG_SLT_MOD,					2 },
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
			{ "texture1D",			CShaderParser::token::LSG_SLT_TEX1D,				3 },
			{ "texture2D",			CShaderParser::token::LSG_SLT_TEX2D,				3 },
			{ "texture3D",			CShaderParser::token::LSG_SLT_TEX3D,				3 },
			{ "textureCube",		CShaderParser::token::LSG_SLT_TEXCUBE,				3 },
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
				}
			}
		}

		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_CAST.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleCast( LSG_NODE_CALLBACK &_ncData ) {
		// Turn it into a constructor.
		CString sType;
		if ( !DecodeType( _ncData.pndNode->nuNodeData.ui64CastType, sType ) ) {
			LSG_PRINT_ERROR( "Unrecognized type cast." );
			return LSG_PD_ERROR;
		}
		if ( !sType.Append( "(" ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}

		if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, ")", sType.CStr() ) ) {
			_ncData.sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}


		return LSG_PD_SKIP_SUB_NODES;
	}

	/**
	 * Handles nodes of type LSG_N_IDENTIFIER.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::HandleIdentifier( LSG_NODE_CALLBACK &_ncData ) {
		//if ( _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui8Type == 
		const CString & sString = _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui32Name );
		const char * pcNew = NULL;
		if ( !(_ncData.ui32Data & LSG_STACK_OBJ::LSG_B_DONOTREPLACE) && m_sssScopes.GetNameAssociation( sString, pcNew ) ) {
			if ( _ncData.psResult->Append( pcNew ) ) {
				return LSG_PD_CONTINUE;
			}
		}
		else {
			// Replace bool types.
			// Get the type from the current scope.
			//if ( !HasParentOfType( _ncData, LSG_N_SINGLEDECL ) ) {
			//	LSG_NODE_TYPE_LIST ntlType;
			//	if ( GetNodeType( _ncData.pndNode->ui32NodeIndex, ntlType ) ) {
			//		// If there is an identifier typed as bool, change it to (NAME!=0).
			//		for ( uint32_t I = ntlType.Length(); I--; ) {
			//			if ( ntlType[I].tType != CShaderScope::LSG_T_VARIABLE ) { continue; }
			//			if ( !ntlType[I].bUniform ) { continue; }
			//			uint32_t ui32Type = CShaderLanguageParser::GetType( ntlType[I].ui64Type );
			//			if ( ui32Type == CShaderParser::token::LSG_SLT_BOOL ) {
			//				if ( CShaderLanguageParser::GetArrayUnit( ntlType[I].ui64Type, 0 ) == LSE_MAXU32 ) {
			//					if ( ntlType[I].ui32Array == LSE_MAXU32 ) {
			//						// Non-array bool.  Change to (NAME!=0).
			//						if ( _ncData.psResult->Append( CString( "(" ) + sString + CString( "!=0.0)" )) ) {
			//							return LSG_PD_CONTINUE;
			//						}
			//					}
			//				}
			//			}
			//		}
			//	}
			//}
			if ( _ncData.psResult->Append( sString ) ) {
				return LSG_PD_CONTINUE;
			}
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Decode a type qualifier to a given string.
	 *
	 * \param _ui64Qualifier The type qualifier.
	 * \param _sString The string to which to print the data.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL COpenGlEsShaderTranslator::DecodeTypeQualifier( uint64_t _ui64Qualifier, CString &_sString ) {
		uint64_t ui64ExclusionBits = 0ULL;
		static const struct LSG_TYPE_QUALIFIERS {
			/** Bit. */
			uint64_t					ui64Bit;

			/** Name. */
			char						szName[16];
		} sTypeQualifiers[] = {
			{ LSG_SQ_CONST,				"const " },
			{ LSG_SQ_IN | LSG_SQ_OUT,	"inout " },
			{ LSG_SQ_IN,				"in " },
			{ LSG_SQ_OUT,				"out " },
			{ LSG_SQ_UNIFORM,			"uniform " },
			{ LSG_IQ_SMOOTH,			"smooth " },
			{ LSG_IQ_FLAT,				"flat " },
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
	LSBOOL LSE_CALL COpenGlEsShaderTranslator::DecodeType( uint64_t _ui64Type, CString &_sString ) {
		static struct LSG_TYPES {
			/** Basic type to find in the input. */
			uint32_t					ui32Type;
			/** Name if not vectored/matrixed. */
			char						szScalarName[16];
			/** Name if vectored. */
			char						szArrayName[16];
			/** Name if matrixed. */
			char						szMatName[16];
		} sType[] = {
			{ CShaderParser::token::LSG_SLT_VOID,				"void ",		"",			"" },
			{ CShaderParser::token::LSG_SLT_SAMPLER,			"",				"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE1D,			"sampler1D ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE2D,			"sampler2D ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE3D,			"sampler3D ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURECUBE,		"samplerCube ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_FLOAT,				"float ",		"vec%u ",	"mat%u " },
			{ CShaderParser::token::LSG_SLT_INT,				"int ",			"ivec%u ",	"" },
			{ CShaderParser::token::LSG_SLT_UINT,				"uint ",		"uvec%u ",	"" },
			{ CShaderParser::token::LSG_SLT_BOOL,				"bool ",		"bvec%u ",	"" },
		};
		uint32_t ui32Type = CShaderLanguageParser::GetType( _ui64Type );
		for ( uint32_t I = 0; I < LSE_ELEMENTS( sType ); I++ ) {
			if ( sType[I].ui32Type == ui32Type ) {
				char szBuffer[16];
				if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) == LSE_MAXU32 && CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) == LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szScalarName );
					return _sString.Append( szBuffer );
				}
				if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 && CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) == LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szArrayName, CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) );
					return _sString.Append( szBuffer );
				}
				if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 && CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) != LSE_MAXU32 ) {
					// In OpenGL ES, both sizes must be the same.
					if ( CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) != CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 ) ) {
						return false;
					}
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szMatName,
						CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 ) );
					return _sString.Append( szBuffer );
				}
				return false;
			}
		}

		// It is a user type.
		return Parent::DecodeType( _ui64Type, _sString );
	}

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
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::VertexInTexCoord( LSG_NODE_CALLBACK &/*_ncData*/,
		uint32_t /*_ui32ParmNodeIndex*/,
		uint32_t _ui32Index, CString &_sRet, CString &_sError ) {
		_sRet = "gl_MultiTexCoord";
		CString sNumber;
		if ( !sNumber.FromUI64( _ui32Index ) ) {
			_sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		_sRet += sNumber;
		return LSG_PD_CONTINUE;
	}

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
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::VertexOutTexCoord( LSG_NODE_CALLBACK &_ncData,
		uint32_t _ui32ParmNodeIndex,
		uint32_t _ui32Index, CString &_sRet, CString &_sError ) {
		if ( _ncData.pslpContainer->GetArrayUnit( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( _ui32ParmNodeIndex ) ), 1 ) != LSE_MAXU32 ) {
			// Must not be a matrix.
			_sError = "TEXCOORD variables must not be matrices.";
			return LSG_PD_ERROR;
		}
		uint32_t ui32Total = _ncData.pslpContainer->GetArrayUnit( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( _ui32ParmNodeIndex ) ), 0 );
		ui32Total = CStd::Max<uint32_t>( ui32Total, 1 );
		if ( ui32Total > 4 ) {
			_sError = "TEXCOORD variables must be vectors with 1, 2, 3, or 4 scalars.";
			return LSG_PD_ERROR;
		}
		static const char * pcTypePostFix = "stpq";
		_sRet = "gl_TexCoord[";
		_sRet += CString::CreateFromUI64( _ui32Index );
		_sRet += "].";
		if ( !_sRet.Append( pcTypePostFix, ui32Total ) ) {
			_sError = LSG_STB_OOM;
			return LSG_PD_ERROR;
		}
		return LSG_PD_CONTINUE;
	}

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
	COpenGlEsShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL COpenGlEsShaderTranslator::PixelInTexCoord( LSG_NODE_CALLBACK &_ncData,
		uint32_t _ui32ParmNodeIndex,
		uint32_t _ui32Index, CString &_sRet, CString &_sError ) {
		return VertexOutTexCoord( _ncData,
			_ui32ParmNodeIndex,
			_ui32Index, _sRet, _sError );
	}

}	// namespace lsg

#endif	// #if defined( LSG_OGLES )

#undef LSG_PRINT_ERROR
