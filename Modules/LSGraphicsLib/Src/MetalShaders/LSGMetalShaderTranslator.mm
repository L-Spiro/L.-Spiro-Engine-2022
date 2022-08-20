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

#include "LSGMetalShaderTranslator.h"
#include "../Gfx/LSGGfx.h"

#if defined( LSG_METAL )

/**
 * Help print errors.
 */
#define LSG_PRINT_ERROR( _pcText )	PrintError( _ncData.sError, _pcText, _ncData );


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CMetalShaderTranslator::CMetalShaderTranslator() :
		m_bIsMainNow( false ) {
	}
	LSE_CALLCTOR CMetalShaderTranslator::~CMetalShaderTranslator() {
	}

	// == Functions.
	/**
	 * Modify the final shader string after it has been completed.  Override to modify it.
	 *
	 * \param _sShader The final string, modifiable in-place.
	 * \param _sError The error string to be filled if there is an error modifying the shader text.
	 * \return Returns false if an error occurred.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::PostShaderString( CString &_sShader, CString &_sError ) {
		static const LSCHAR * pcHeader =
			"#include <metal_atomic>\n"
			"#include <metal_geometric>\n"
			"#include <metal_graphics>\n"
			"#include <metal_integer>\n"
			"#include <metal_math>\n"
			"#include <metal_matrix>\n"
			"#include <metal_pack>\n"
			"#include <metal_relational>\n"
			"#include <metal_stdlib>\n"
			"#include <metal_texture>\n"
			"using namespace metal;\n";
		CString sHeader;
		if ( !sHeader.Insert( 0, pcHeader ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( m_pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
			//sHeader += "invariant gl_Position;\n";
		}
		else if ( m_pslpContainer->GetShaderType() == LSG_ST_PIXEL ) {
			//sHeader += CString( "varying vec4 lsg_vColor[" ) + CString::CreateFromUI64( CGfx::GetMetrics().ui32MaxRenderTargets ) + CString( "];\n" );
		}

		// Insert shadowmapping intrinsics.
		/*sHeader += "float shadow2dDepth(sampler2D s,vec2 v){return texture2D(s,v).z;}\n"
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
			"float linstep( float fLow, float fHigh, float fV ) { return clamp( (fV - fLow) / (fHigh - fLow), 0.0, 1.0 ); }\n"
			"float rcp( float _fVal ) { return 1.0f / _fVal; }\n"
			"float2 rcp( float2 _fVal ) { return float2( 1.0f / _fVal.x, 1.0f / _fVal.y ); }\n"
			"float3 rcp( float3 _fVal ) { return float3( 1.0f / _fVal.x, 1.0f / _fVal.y, 1.0f / _fVal.x ); }\n"
			"float4 rcp( float4 _fVal ) { return float4( 1.0f / _fVal.x, 1.0f / _fVal.y, 1.0f / _fVal.x, 1.0f / _fVal.w ); }\n";
		*/
		
		
		// Remove all unreferenced globals.
		for ( LSUINT32 I = m_ulGlobalUniforms.Length(); I--; ) {
			if ( !m_ulGlobalUniforms[I].ui32RefCount ) {
				for ( LSUINT32 J = m_uslGlobalSemantics.Length(); J--; ) {
					if ( m_uslGlobalSemantics[J].sName == m_ulGlobalUniforms[I].sName ) {
						m_uslGlobalSemantics.RemoveNoDealloc( J );
					}
				}
				m_ulGlobalUniforms.RemoveNoDealloc( I );
			}
		}
		
		
		
		CString sStructTemp;
		for ( LSUINT32 I = 0; I < 4 /* Number of buffers (see GetBufferId()). */; ++I ) {
			LSCHAR szBuffer[32];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "struct lsg_buffer%u{\n", I );
			if ( !sStructTemp.Set( szBuffer ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}

			// Go over each global.
			LSUINT32 ui32Offset = 0;
			for ( LSUINT32 J = 0; J < m_ulGlobalUniforms.Length(); ++J ) {
				LSUINT32 ui32Sem, ui32SemIdx;
				GetSemanticFromUniform( m_ulGlobalUniforms[J].sName, ui32Sem, ui32SemIdx );
				if ( GetBufferId( ui32Sem ) == I ) {
					LSUINT64 ui64Type = m_ulGlobalUniforms[J].ui64Type;
					
					if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_SAMPLER, LSE_MAXU32, LSE_MAXU32 ) ) {
						// Handle a sampler.
					}
					else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE1D, LSE_MAXU32, LSE_MAXU32 ) ) {
					}
					else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 ) ) {
					}
					else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE3D, LSE_MAXU32, LSE_MAXU32 ) ) {
					}
					else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURECUBE, LSE_MAXU32, LSE_MAXU32 ) ) {
					}
					else {
						LSUINT32 ui32Size, ui32Align;
						GetUniformBufferSizeAndAlign( m_ulGlobalUniforms[J], ui32Size, ui32Align );
						ui32Offset = LSE_ROUND_UP( ui32Offset, ui32Align );
						
						m_ulGlobalUniforms[J].uiptrApiSpecific = ui32Offset;
						m_ulGlobalUniforms[J].uiptrApiParam = I;
						
						// Type.
						if ( !DecodeType( m_ulGlobalUniforms[J].ui64Type, sStructTemp ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						// Name.
						if ( !sStructTemp.Append( m_ulGlobalUniforms[J].sName ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						// Optional array.
						if ( m_ulGlobalUniforms[J].ui32Array != LSE_MAXU32 ) {
							if ( !sStructTemp.Append( CString( "[" ) + CString::CreateFromUI64( m_ulGlobalUniforms[J].ui32Array ) + CString( "]" ) ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
						
						
						// End of declaration.
						if ( !sStructTemp.Append( ";\n" ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						
						ui32Offset += ui32Size;
					}
				}
			}
			
			if ( !sStructTemp.Append( "};\n" ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}
			
			if ( !sHeader.Append( sStructTemp ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}
		}
		
		

		struct LSG_METAL_SEMANTICS {
			LSUINT32				ui32SemType;
			const LSCHAR *			pcVertOut;
			const LSCHAR *			pcPixelIn;
			const LSCHAR *			pcPixelOut;
		};
		static const LSG_METAL_SEMANTICS msInputSemantics[] = {
			{ CShaderParser::token::LSG_SLT_POSITION,		"[[position]]",		NULL, },
			{ CShaderParser::token::LSG_SLT_POSITIONT,		NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_NORMAL,			NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_COLOR,			NULL,				NULL,				"[[color(%u)]]" },
			{ CShaderParser::token::LSG_SLT_TEXCOORD,		NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_BINORMAL,		NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_TANGENT,		NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_PSIZE,			"[[point_size]]",	NULL, },
			{ CShaderParser::token::LSG_SLT_BLENDINDICES,	NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_BLENDWEIGHT,	NULL,				NULL, },
			{ CShaderParser::token::LSG_SLT_ISFRONTFACE,	NULL,				"[[front_facing]]", },
			{ CShaderParser::token::LSG_SLT_VPOS,			NULL,				"[[position]]", },
			{ CShaderParser::token::LSG_SLT_DEPTH,			NULL,				NULL, },
		};

		// Vertex-shader inputs.
		if ( m_pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
			// Vertex in.
			sHeader += "struct lsg_vs_in{\n";
			
			LSUINT32 ui32AttrCounter = 0;
			for ( LSUINT32 I = 0; I < LSE_ELEMENTS( msInputSemantics ); ++I ) {
				for ( LSUINT32 J = 0; J < LSG_MAX_VERTEX_SEMANTIC; ++J ) {
					LSG_ATTRIBUTE * paAttr = FindAttribute( msInputSemantics[I].ui32SemType, J, true );
					if ( paAttr ) {
						LSCHAR szBuffer[32];
						CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "[[attribute(%u)]];\n", ui32AttrCounter++ );
						if ( !DecodeType( paAttr->ui64Type, sHeader ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						if ( !sHeader.Append( paAttr->sName.CStr() ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						if ( !sHeader.Append( szBuffer ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
					}
				}
			}
			
			sHeader += "};\n";
			
			
			// Vertex out, pixel in.
			sHeader += "struct lsg_vs_out_ps_in{\n";
			
			for ( LSUINT32 I = 0; I < LSE_ELEMENTS( msInputSemantics ); ++I ) {
				for ( LSUINT32 J = 0; J < LSG_MAX_VERTEX_SEMANTIC; ++J ) {
					LSG_ATTRIBUTE * paAttr = FindAttribute( msInputSemantics[I].ui32SemType, J, false );
					if ( paAttr ) {
						if ( !DecodeType( paAttr->ui64Type, sHeader ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						if ( !sHeader.Append( paAttr->sName.CStr() ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						
						if ( msInputSemantics[I].pcVertOut && J == 0 ) {
							// Specified output.
							LSCHAR szBuffer[32];
							CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%s;\n", msInputSemantics[I].pcVertOut );
							if ( !sHeader.Append( szBuffer ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
						else {
							// User output.
							LSCHAR szBuffer[64];
							CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "[[user(sem%u_%u)]];\n", msInputSemantics[I].ui32SemType, J );
							if ( !sHeader.Append( szBuffer ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
					}
				}
			}
			
			sHeader += "};\n";
		}
		else if ( m_pslpContainer->GetShaderType() == LSG_ST_PIXEL ) {
			// Vertex out, pixel in.
			sHeader += "struct lsg_vs_out_ps_in{\n";
			
			for ( LSUINT32 I = 0; I < LSE_ELEMENTS( msInputSemantics ); ++I ) {
				for ( LSUINT32 J = 0; J < LSG_MAX_VERTEX_SEMANTIC; ++J ) {
					LSG_ATTRIBUTE * paAttr = FindAttribute( msInputSemantics[I].ui32SemType, J, true );
					if ( paAttr ) {
						if ( !DecodeType( paAttr->ui64Type, sHeader ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						if ( !sHeader.Append( paAttr->sName.CStr() ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						
						if ( msInputSemantics[I].pcPixelIn && J == 0 ) {
							// Specified input.
							LSCHAR szBuffer[32];
							CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%s;\n", msInputSemantics[I].pcPixelIn );
							if ( !sHeader.Append( szBuffer ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
						else {
							// User input.
							LSCHAR szBuffer[64];
							CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "[[user(sem%u_%u)]];\n", msInputSemantics[I].ui32SemType, J );
							if ( !sHeader.Append( szBuffer ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
					}
				}
			}
			
			sHeader += "};\n";
			
			
			// Pixel out.
			sHeader += "struct lsg_ps_out{\n";
			
			for ( LSUINT32 I = 0; I < LSE_ELEMENTS( msInputSemantics ); ++I ) {
				for ( LSUINT32 J = 0; J < LSG_MAX_VERTEX_SEMANTIC; ++J ) {
					LSG_ATTRIBUTE * paAttr = FindAttribute( msInputSemantics[I].ui32SemType, J, false );
					if ( paAttr ) {
						if ( !DecodeType( paAttr->ui64Type, sHeader ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						if ( !sHeader.Append( paAttr->sName.CStr() ) ) {
							_sError = LSG_STB_OOM;
							return false;
						}
						
						if ( msInputSemantics[I].pcPixelOut ) {
							// Specified output.
							LSCHAR szFormat[32];
							CStd::SPrintF( szFormat, LSE_ELEMENTS( szFormat ), "%s;\n", msInputSemantics[I].pcPixelOut );
							LSCHAR szBuffer[32];
							CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), szFormat, J );
							if ( !sHeader.Append( szBuffer ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
						else {
							// User output.
							LSCHAR szBuffer[64];
							CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "[[user(sem%u_%u)]];\n", msInputSemantics[I].ui32SemType, J );
							if ( !sHeader.Append( szBuffer ) ) {
								_sError = LSG_STB_OOM;
								return false;
							}
						}
					}
				}
			}
			
			sHeader += "};\n";
		}

		// Declare attributes.
		if ( !_sShader.Insert( 0, sHeader.CStr(), sHeader.Length() ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}


		return true;
	}
	
	/**
	 * Handles nodes of type LSG_N_DISCARD.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleDiscard( LSG_NODE_CALLBACK &_ncData ) {
		if ( _ncData.psResult->Append( "discard_fragment();" ) ) {
			return LSG_PD_CONTINUE;
		}
		_ncData.sError = LSG_STB_OOM;
		return LSG_PD_ERROR;
	}

	/**
	 * Handles nodes of type LSG_N_FUNCDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData ) {
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
			if ( m_pslpContainer->GetShaderType() == LSG_ST_VERTEX ) {
				if ( !_ncData.psResult->Append( "vertex lsg_vs_out_ps_in Main(lsg_vs_in lsg_vertex_input[[stage_in]]" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				
				// Get the function definition.
				const CShaderTranslationProcessorBase::LSG_STACK_OBJ * psoFuncDef = GetParentOfType( _ncData, LSG_N_FUNCDEFINITION );
				if ( psoFuncDef ) {
				
					const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndData = _ncData.pslpContainer->GetNode( psoFuncDef->ui32Index );
					// If there is a body.
					if ( ndData.nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
						CShaderTranslationProcessorBase::LSG_STACK_OBJ * psoFuncBody = const_cast<CShaderTranslationProcessorBase::LSG_STACK_OBJ *>(FindNodeById( _ncData, ndData.nueNodeDataEx.ui32NodeIndexEx ));
						if ( psoFuncBody ) {
							// Add the return type structure and the return statement.
							psoFuncBody->sPreFix = psoFuncBody->sPreFix + CString( "lsg_vs_out_ps_in lsg_Out;\n" );
							psoFuncBody->sPostFix = CString( "return lsg_Out;\n" ) + psoFuncBody->sPostFix;
						}
					}
				}
			}
			else if ( m_pslpContainer->GetShaderType() == LSG_ST_PIXEL ) {
				if ( !_ncData.psResult->Append( "fragment lsg_ps_out Main(lsg_vs_out_ps_in lsg_pixel_input[[stage_in]]" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				
				// Get the function definition.
				const CShaderTranslationProcessorBase::LSG_STACK_OBJ * psoFuncDef = GetParentOfType( _ncData, LSG_N_FUNCDEFINITION );
				if ( psoFuncDef ) {
				
					const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndData = _ncData.pslpContainer->GetNode( psoFuncDef->ui32Index );
					// If there is a body.
					if ( ndData.nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
						CShaderTranslationProcessorBase::LSG_STACK_OBJ * psoFuncBody = const_cast<CShaderTranslationProcessorBase::LSG_STACK_OBJ *>(FindNodeById( _ncData, ndData.nueNodeDataEx.ui32NodeIndexEx ));
						if ( psoFuncBody ) {
							// Add the return type structure and the return statement.
							psoFuncBody->sPreFix = psoFuncBody->sPreFix + CString( "lsg_ps_out lsg_Out;\n" );
							psoFuncBody->sPostFix = CString( "return lsg_Out;\n" ) + psoFuncBody->sPostFix;
						}
					}
				}
			}


			LSUINT32 ui32BufferCount = 0;
			// For up to 32 buffers, although we currently do not support anywhere close to that amount.
			for ( LSUINT32 I = 0; I < 32; ++I ) {
				for ( LSUINT32 J = 0; J < m_ulGlobalUniforms.Length(); ++J ) {
					if ( IsTexture( m_ulGlobalUniforms[J] ) || IsSampler( m_ulGlobalUniforms[J] ) ) { continue; }
					LSUINT32 ui32Sem, ui32SemIndex;
					GetSemanticFromUniform( m_ulGlobalUniforms[J].sName, ui32Sem, ui32SemIndex );
					if ( GetBufferId( ui32Sem ) == I ) {
						LSCHAR szBuffer[64];
						CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ",constant lsg_buffer%u&lsg_buf%u[[buffer(%u)]]",
							I, I, ui32BufferCount++ );
						if ( !_ncData.psResult->Append( szBuffer ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}
				}
			}
			// Textures and samplers.
			for ( LSUINT32 J = 0; J < m_ulGlobalUniforms.Length(); ++J ) {
				if ( IsTexture( m_ulGlobalUniforms[J] ) ) {
					if ( !_ncData.psResult->Append( "," ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					if ( !PrintTexture( m_ulGlobalUniforms[J], (*_ncData.psResult), true ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				else if ( IsSampler( m_ulGlobalUniforms[J] ) ) {
					if ( !_ncData.psResult->Append( "," ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					if ( !PrintSampler( m_ulGlobalUniforms[J], (*_ncData.psResult), true ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
			}
			
			
			if ( !_ncData.psResult->Append( ")" ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}


			// Paramaters are moved to a structure, uniforms supplied here as parameters.
			LSBOOL bHaveOutPos = false;
			for ( LSUINT32 I = m_vMainParms.Length(); I--; ) {
				const CString & sThisName = _ncData.pslpContainer->GetNameOfParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
				LSUINT64 ui64Sem = _ncData.pslpContainer->GetSemantic( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) );
				LSUINT32 ui32Name = static_cast<LSUINT32>(ui64Sem & 0xFFFFFFFFULL);
				LSUINT32 ui32Count = static_cast<LSUINT32>(ui64Sem >> 32ULL);

				// Not allowed to have inout on Main().
				if ( (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & (LSG_SQ_OUT | LSG_SQ_IN)) == (LSG_SQ_OUT | LSG_SQ_IN) ) {
					LSG_PRINT_ERROR( "Parameters in the Main() function can be in our out, but not both." );
					return LSG_PD_ERROR;
				}
				if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX &&
					(_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) ) {
					// Vertex-shader input.
					CString sNewName = CString( "lsg_vertex_input." ) + sThisName;
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, sNewName.CStr() ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				LSBOOL bInput = (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_IN) != 0;
				if ( (_ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX || _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL) &&
					!bInput ) {
					// Vertex- or pixel- shader output.
					CString sNewName = CString( "lsg_Out." ) + sThisName;
					if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( sThisName, sNewName.CStr() ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
				}
				if ( _ncData.pslpContainer->GetShaderType() == LSG_ST_VERTEX &&
					ui32Name == CShaderParser::token::LSG_SLT_POSITION && ui32Count == 0 && (_ncData.pslpContainer->GetTypeQualifiersOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) & LSG_SQ_OUT) ) {
					// The primary output must be of type vec4.
					if ( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( m_vMainParms[I] ) ) != CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ) ) {
						LSG_PRINT_ERROR( "The primary output position must be of type float[4]." );
						return LSG_PD_ERROR;
					}
					bHaveOutPos = true;
				}
				{
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
					if ( (aAttr.bInput && _ncData.pslpContainer->GetShaderType() == LSG_ST_PIXEL) ) {
						CString sNewName = CString( "lsg_pixel_input." ) + sThisName;
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
						for ( LSUINT32 K = m_alAttributes.Length(); K--; ) {
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
					LSCHAR szBuffer[32];
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleParamaterDeclarator( LSG_NODE_CALLBACK &_ncData ) {
		//return LSG_PD_CONTINUE;
		// Get the parameter declaration.
		const CShaderTranslationProcessorBase::LSG_STACK_OBJ * psoFuncDef = GetParentOfType( _ncData, LSG_N_PARMDECLARATION );
		if ( !psoFuncDef ) {
			return LSG_PD_CONTINUE;
		}
		const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndData = _ncData.pslpContainer->GetNode( psoFuncDef->ui32Index );
		LSUINT64 ui64Qualifiers = ndData.nuNodeData.ui64Const;
		if ( (ui64Qualifiers & LSG_SQ_OUT) ) {
			// Have to add & before the identifier.
			// Optional semantic.
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32SingleDeclName, _ncData.ui32Index ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			// Optional array.
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuoOp.ui32NodeIndex3, _ncData.ui32Index ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			// Name/identifier.
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nuNodeData.ui32IdentifierIndex, _ncData.ui32Index ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			// Types & qualifiers.
			if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, _ncData.ui32Index, 0, "&", "thread " ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			return LSG_PD_SKIP_SUB_NODES;
		}
		return LSG_PD_CONTINUE;
	}

	/**
	 * Handles nodes of type LSG_N_SINGLEDECL.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		// The default handler for this adds the value to the current scope.
		LSG_PROCESSING_DIRECTIVE pdRet = Parent::HandleSingleDeclaration( _ncData );
		if ( pdRet != LSG_PD_CONTINUE ) { return pdRet; }

		LSUINT64 ui64ThisType = 0;
		// Check the semantic (only on globals).
		if ( m_sssScopes.TotalScopes() == 1 && _ncData.pndNode->ui32Semantic != LSE_MAXU32 ) {
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndSemantic = _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32Semantic );
			struct LSG_GLOBAL_SEMANTIC_CHECK {
				/** The semantic type. */
				LSUINT32						ui32Semantic;
				/** The type that it must be. */
				LSUINT64						ui64Type;
				/** If it should be an array or not. */
				LSBOOL							bIsArray;
				/** Name of the semantic. */
				LSCHAR							szName[32];
			};
			static LSG_GLOBAL_SEMANTIC_CHECK gscCheckMe[] = {
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


			LSG_UNIFORM_SEMANTIC usThis;
			usThis.ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
			ui64ThisType = usThis.ui64Type;
			LSBOOL bFound = false;
			for ( LSINT32 I = LSE_ELEMENTS( gscCheckMe ); I--; ) {
				if ( ndSemantic.nueNodeDataEx.ui32NodeIndexEx == gscCheckMe[I].ui32Semantic ) {
					if ( usThis.ui64Type != gscCheckMe[I].ui64Type ) {
						CString sError = CString( "The " ) + CString( gscCheckMe[I].szName ) + CString( " usage type can only be applied to type " ) +
							_ncData.pslpContainer->BasicTypeToString( gscCheckMe[I].ui64Type ) + CString( "." );
						LSG_PRINT_ERROR( sError.CStr() );
						return LSG_PD_ERROR;
					}
					if ( gscCheckMe[I].bIsArray ) {
						if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx == LSE_MAXU32 ) {
							CString sError = CString( "The " ) + CString( gscCheckMe[I].szName ) + CString( " usage type must be applied to arrays." );
							LSG_PRINT_ERROR( sError.CStr() );
							return LSG_PD_ERROR;
						}
					}
					else {
						if ( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx != LSE_MAXU32 ) {
							CString sError = CString( "The " ) + CString( gscCheckMe[I].szName ) + CString( " usage type cannot be applied to arrays." );
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

						usThis.sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );
						usThis.sName.Snap();
						usThis.ui32SemanticType = ndSemantic.nueNodeDataEx.ui32NodeIndexEx;
						usThis.ui32SemanticIndex = ndSemantic.nuoOp.ui32NodeIndex3;
						if ( !m_uslGlobalSemantics.Push( usThis ) ) {
							_ncData.sError = LSG_STB_OOM;
							return LSG_PD_ERROR;
						}
						break;
					}
				}
			}
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
				uThis.ui32Array = static_cast<LSUINT32>(sFinalValue.ui64Value);
				uThis.ui32Size *= uThis.ui32Array;
			}
			
			if ( !m_ulGlobalUniforms.Push( uThis ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}
			
			if ( !_ncData.psResult->Append( "//" ) ) { return LSG_PD_ERROR; }
			if ( !_ncData.psResult->Append( _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) ) ) ) { return LSG_PD_ERROR; }
			
			if ( CShaderLanguageParser::IsSampler( ui64ThisType ) || CShaderLanguageParser::IsTexture( ui64ThisType ) ) {
				// Samplers not invited for aliasing.
				return LSG_PD_SKIP_SUB_NODES;
			}
			
			// Create an alias.
			LSUINT32 ui32Sem = LSE_MAXU32;
			if ( _ncData.pndNode->ui32Semantic != LSE_MAXU32 ) {
				const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndSemantic = _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32Semantic );
				ui32Sem = ndSemantic.nueNodeDataEx.ui32NodeIndexEx;
			}
			LSUINT32 ui32BufId = GetBufferId( ui32Sem );
			CString sTemp = CString( "lsg_buf" ) + CString::CreateFromUI64( ui32BufId ) + CString( "." ) + uThis.sName;
			if ( !m_sssScopes.GetCurrentScope()->CreateNameAssociation( uThis.sName, sTemp.CStr() ) ) {
				_ncData.sError = LSG_STB_OOM;
				return LSG_PD_ERROR;
			}

			return LSG_PD_SKIP_SUB_NODES;
		}

		if ( _ncData.pndNode->nuoOp.ui32NodeIndex3 != LSE_MAXU32 ) {
			// Create a constructor.
			CString sName = _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) );

			LSUINT64 ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
			LSG_NODE_TYPE_LIST ntlTypes;
			LSBOOL bIsExplicit = GetNodeType( _ncData.pndNode->nuoOp.ui32NodeIndex3, ntlTypes );
			if ( bIsExplicit ) {
				bIsExplicit = false;
				// Check each possible type for a real match.
				for ( LSUINT32 I = ntlTypes.Length(); I--; ) {
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleFullySpecifiedType( LSG_NODE_CALLBACK &_ncData ) {
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleIntrinsic( LSG_NODE_CALLBACK &_ncData ) {
		// No intrinsic function has more than 3 arguments.
		LSUINT32 ui32Args[3];
		LSUINT32 ui32ArgCount = _ncData.pslpContainer->CountArgs( _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx, 3, ui32Args );
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
			LSCHAR					szName[32];

			/** Intrinsic identifier. */
			LSUINT32				ui32Id;

			/** Number of arguments. */
			LSUINT32				ui32Args;
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
			{ "sample",				CShaderParser::token::LSG_SLT_TEX1D,				3 },
			{ "sample",				CShaderParser::token::LSG_SLT_TEX2D,				3 },
			{ "sample",				CShaderParser::token::LSG_SLT_TEX3D,				3 },
			{ "sample",				CShaderParser::token::LSG_SLT_TEXCUBE,				3 },
			{ "tex2DShadowCompare",	CShaderParser::token::LSG_SLT_TEX2DSHADOWCOMPARE,	4 },
			{ "tex2DShadow",		CShaderParser::token::LSG_SLT_TEX2DSHADOW,			5 },
			{ "transpose",			CShaderParser::token::LSG_SLT_TRANSPOSE,			1 },
		};

		for ( LSUINT32 I = LSE_ELEMENTS( sFuncs ); I--; ) {
			if ( sFuncs[I].ui32Id == _ncData.pndNode->nuNodeData.ui32NodeIndex ) {
				// Found it.
				// Verify the number of parameters.
				LSCHAR szPrefix[16];
				if ( sFuncs[I].ui32Args != ui32ArgCount ) {
					LSG_PRINT_ERROR( "Intrinsic function " );
					_ncData.sError += sFuncs[I].szName;
					_ncData.sError += "() requires ";
					_ncData.sError += CStd::UI32toA( sFuncs[I].ui32Args, szPrefix );
					_ncData.sError += " parameters.";
					return LSG_PD_ERROR;
				}
				
				if ( (_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEX1D) ||
					(_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEX2D) ||
					(_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEX3D) ||
					(_ncData.pndNode->nuNodeData.ui32NodeIndex == CShaderParser::token::LSG_SLT_TEXCUBE) ) {
					CStd::SPrintF( szPrefix, LSE_ELEMENTS( szPrefix ), ".%s(", sFuncs[I].szName );

					// texX( TextureX, Sampler, Uv ) = ui32Args[2].sample(ui32Args[1],ui32Args[0])

					// ui32Args[0])
					if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[0], _ncData.ui32Index, 0, ")", NULL ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					// ui32Args[1],
					if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[1], _ncData.ui32Index, 0, ",", NULL ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					// ui32Args[2].Sample(
					if ( !PushNode( (*_ncData.pnsNodeStack), ui32Args[2], _ncData.ui32Index, 0, szPrefix, NULL ) ) {
						_ncData.sError = LSG_STB_OOM;
						return LSG_PD_ERROR;
					}
					break;
				}

				
				CStd::SPrintF( szPrefix, LSE_ELEMENTS( szPrefix ), "%s(", sFuncs[I].szName );
				
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleCast( LSG_NODE_CALLBACK &_ncData ) {
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::HandleIdentifier( LSG_NODE_CALLBACK &_ncData ) {
		//if ( _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui8Type == 
		const CString & sString = _ncData.pslpContainer->GetString( _ncData.pslpContainer->GetIdentifier( _ncData.pndNode->nuNodeData.ui32IdentifierIndex ).ui32Name );
		const LSCHAR * pcNew = NULL;
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
	 * Decode a type qualifier to a given string.
	 *
	 * \param _ui64Qualifier The type qualifier.
	 * \param _sString The string to which to print the data.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::DecodeTypeQualifier( LSUINT64 _ui64Qualifier, CString &_sString ) {
		LSUINT64 ui64ExclusionBits = 0ULL;
		static const struct LSG_TYPE_QUALIFIERS {
			/** Bit. */
			LSUINT64					ui64Bit;

			/** Name. */
			LSCHAR						szName[16];
		} sTypeQualifiers[] = {
			{ LSG_SQ_CONST,				"const " },
			{ LSG_SQ_IN | LSG_SQ_OUT,	"" },
			{ LSG_SQ_IN,				"" },
			{ LSG_SQ_OUT,				"" },
			{ LSG_SQ_UNIFORM,			"uniform " },
			{ LSG_IQ_SMOOTH,			"smooth " },
			{ LSG_IQ_FLAT,				"flat " },
		};
		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( sTypeQualifiers ); I++ ) {
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
	LSBOOL LSE_CALL CMetalShaderTranslator::DecodeType( LSUINT64 _ui64Type, CString &_sString ) {
		static struct LSG_TYPES {
			/** Basic type to find in the input. */
			LSUINT32					ui32Type;
			/** Name if not vectored/matrixed. */
			LSCHAR						szScalarName[32];
			/** Name if vectored. */
			LSCHAR						szArrayName[16];
			/** Name if matrixed. */
			LSCHAR						szMatName[16];
		} sType[] = {
			{ CShaderParser::token::LSG_SLT_VOID,				"void ",				"",			"" },
			{ CShaderParser::token::LSG_SLT_SAMPLER,			"sampler ",				"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE1D,			"texture1d<float> ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE2D,			"texture2d<float> ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE3D,			"texture3d<float> ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_TEXTURECUBE,		"texturecube<float> ",	"",			"" },
			{ CShaderParser::token::LSG_SLT_FLOAT,				"float ",				"float%u ",	"float%ux%u " },
			{ CShaderParser::token::LSG_SLT_INT,				"int ",					"int%u ",	"" },
			{ CShaderParser::token::LSG_SLT_UINT,				"uint ",				"uint%u ",	"" },
			{ CShaderParser::token::LSG_SLT_BOOL,				"bool ",				"bool%u ",	"" },
		};
		LSUINT32 ui32Type = CShaderLanguageParser::GetType( _ui64Type );
		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( sType ); I++ ) {
			if ( sType[I].ui32Type == ui32Type ) {
				LSCHAR szBuffer[32];
				LSUINT32 ui32Array0 = CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 );
				LSUINT32 ui32Array1 = CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 );
				if ( ui32Array0 == LSE_MAXU32 && ui32Array1 == LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szScalarName );
					return _sString.Append( szBuffer );
				}
				if ( ui32Array0 != LSE_MAXU32 && ui32Array1 == LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szArrayName, ui32Array0 );
					return _sString.Append( szBuffer );
				}
				if ( ui32Array0 != LSE_MAXU32 && ui32Array1 != LSE_MAXU32 ) {
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), sType[I].szMatName,
						ui32Array0, ui32Array1 );
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::VertexInTexCoord( LSG_NODE_CALLBACK &/*_ncData*/,
		LSUINT32 /*_ui32ParmNodeIndex*/,
		LSUINT32 _ui32Index, CString &_sRet, CString &_sError ) {
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::VertexOutTexCoord( LSG_NODE_CALLBACK &_ncData,
		LSUINT32 _ui32ParmNodeIndex,
		LSUINT32 _ui32Index, CString &_sRet, CString &_sError ) {
		if ( _ncData.pslpContainer->GetArrayUnit( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( _ui32ParmNodeIndex ) ), 1 ) != LSE_MAXU32 ) {
			// Must not be a matrix.
			_sError = "TEXCOORD variables must not be matrices.";
			return LSG_PD_ERROR;
		}
		LSUINT32 ui32Total = _ncData.pslpContainer->GetArrayUnit( _ncData.pslpContainer->GetTypeOnParm( _ncData.pslpContainer->GetNode( _ui32ParmNodeIndex ) ), 0 );
		ui32Total = CStd::Max<LSUINT32>( ui32Total, 1 );
		if ( ui32Total > 4 ) {
			_sError = "TEXCOORD variables must be vectors with 1, 2, 3, or 4 scalars.";
			return LSG_PD_ERROR;
		}
		static const LSCHAR * pcTypePostFix = "stpq";
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
	CMetalShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CMetalShaderTranslator::PixelInTexCoord( LSG_NODE_CALLBACK &_ncData,
		LSUINT32 _ui32ParmNodeIndex,
		LSUINT32 _ui32Index, CString &_sRet, CString &_sError ) {
		return VertexOutTexCoord( _ncData,
			_ui32ParmNodeIndex,
			_ui32Index, _sRet, _sError );
	}
	
	/**
	 * Determines the buffer index into which to place the given value based on its semantic.
	 *
	 * \param _ui32Semantic The semantic of the data.
	 * \return Returns the index of the buffer into which to put the data based on semantic.
	 */
	LSUINT32 LSE_CALL CMetalShaderTranslator::GetBufferId( LSUINT32 _ui32Semantic ) const {
		// Similar to the list in LSGDirectX11ShaderTranslator.cpp.
		switch ( _ui32Semantic ) {
			case CShaderParser::token::LSG_SLT_MATAMBIENT : {}
			case CShaderParser::token::LSG_SLT_MATDIFFUSE : {}
			case CShaderParser::token::LSG_SLT_MATSPECULAR : {}
			case CShaderParser::token::LSG_SLT_MATPOWER : {}
			case CShaderParser::token::LSG_SLT_REFLECTIVITY : {}
			case CShaderParser::token::LSG_SLT_ROUGHNESS : {}
			case CShaderParser::token::LSG_SLT_ASHIKFACTORS : {
				return 0;
			}
			case CShaderParser::token::LSG_SLT_LIGHTCOUNT : {}
			case CShaderParser::token::LSG_SLT_DIRLIGHTDIRS : {}
			case CShaderParser::token::LSG_SLT_POINTLIGHTPOS : {}
			case CShaderParser::token::LSG_SLT_SPOTLIGHTDIRS : {}
			case CShaderParser::token::LSG_SLT_LIGHTAMBIENT : {}
			case CShaderParser::token::LSG_SLT_LIGHTDIFFUSE : {}
			case CShaderParser::token::LSG_SLT_LIGHTSPECULAR : {}
			case CShaderParser::token::LSG_SLT_LIGHTATT : {}
			case CShaderParser::token::LSG_SLT_SPOTLIGHTEXP : {}
			case CShaderParser::token::LSG_SLT_SPOTLIGHTRANGE : {
				return 1;
			}
			case CShaderParser::token::LSG_SLT_NORMALMATRIX : {}
			case CShaderParser::token::LSG_SLT_TEXMATRIX : {}
			case CShaderParser::token::LSG_SLT_WORLD : {}
			case CShaderParser::token::LSG_SLT_WORLDVIEW : {}
			case CShaderParser::token::LSG_SLT_WORLDVIEWPROJ : {}
			case CShaderParser::token::LSG_SLT_VIEW : {}
			case CShaderParser::token::LSG_SLT_CAMPOS : {}
			case CShaderParser::token::LSG_SLT_TIMESINCELASTFRAME : {}
			case CShaderParser::token::LSG_SLT_SHADOWMAPPING : {
				return 2;
			}
			default : { return 3; }
		}
	}
	
	/**
	 * Gets the semantic type and index, or returns false.
	 *
	 * \param _sName Name of the global uniform.
	 * \param _ui32SemanticType Returned semantic type.
	 * \param _ui32SemanticIndex Returned semantic index.
	 * \return Returns true if there was a matching global semantic.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::GetSemanticFromUniform( const CString &_sName, LSUINT32 &_ui32SemanticType, LSUINT32 &_ui32SemanticIndex ) const {
		_ui32SemanticType = LSE_MAXU32;
		_ui32SemanticIndex = 0;
		for ( LSUINT32 J = m_uslGlobalSemantics.Length(); J--; ) {
			if ( m_uslGlobalSemantics[J].sName == _sName ) {
				// Yes.
				_ui32SemanticType = m_uslGlobalSemantics[J].ui32SemanticType;
				_ui32SemanticIndex = m_uslGlobalSemantics[J].ui32SemanticIndex;
				return true;
			}
		}
		return false;
	}
	
	/**
	 * Gets the size and alignment of a given uniform as it would be in a buffer.
	 *
	 * \param _uUniform Uniform whose size and alignment when inside a buffer is to be gotten.
	 * \param _ui32Size Holds the returned size of the type, adjusted for arrays etc.
	 * \param _ui32Align Holds the returned alignment of the type.
	 */
	LSVOID LSE_CALL CMetalShaderTranslator::GetUniformBufferSizeAndAlign( const LSG_UNIFORM &_uUniform, LSUINT32 &_ui32Size, LSUINT32 &_ui32Align ) {
		/*LSUINT64 ui64Type = _uUniform.ui64Type;
		LSUINT32 ui32BasicId = static_cast<LSUINT32>(ui64Type & 0xFFFFFFFFUL);
		LSUINT32 ui32Array0 = CShaderLanguageParser::GetArrayUnit( ui64Type, 0 );
		LSUINT32 ui32Array1 = CShaderLanguageParser::GetArrayUnit( ui64Type, 1 );
		if ( ui32Array0 == LSE_MAXU32 ) {
			ui32Array0 = 1;
		}
		if ( ui32Array1 == LSE_MAXU32 ) {
			ui32Array1 = 1;
		}*/
		
		static const struct LSG_SIZE_ALIGN_TABLE {
			LSUINT64				ui64Type;
			LSUINT32				ui32Size;
			LSUINT32				ui32Align;
		} satTable[] = {
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, LSE_MAXU32, LSE_MAXU32 ), 1, 1 },		// bool
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ), 4, 4 },		// int
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 ), 4, 4 },		// uint
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ), 4, 4 },		// float
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, 2, LSE_MAXU32 ), 2, 2 },				// bool2
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, 3, LSE_MAXU32 ), 4, 4 },				// bool3
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_BOOL, 4, LSE_MAXU32 ), 4, 4 },				// bool4
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, 2, LSE_MAXU32 ), 8, 8 },					// int2
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, 3, LSE_MAXU32 ), 16, 16 },				// int3
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, 4, LSE_MAXU32 ), 16, 16 },				// int4
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, 2, LSE_MAXU32 ), 8, 8 },				// uint2
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, 3, LSE_MAXU32 ), 16, 16 },				// uint3
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, 4, LSE_MAXU32 ), 16, 16 },				// uint4
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 2, LSE_MAXU32 ), 8, 8 },				// float2
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, LSE_MAXU32 ), 16, 16 },				// float3
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, LSE_MAXU32 ), 16, 16 },				// float4
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 2, 2 ), 16, 8 },						// float2x2
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, 3 ), 48, 16 },						// float3x3
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 4, 4 ), 64, 16 },						// float4x4
			// Invalid types for this function.
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_VOID, LSE_MAXU32, LSE_MAXU32 ), 0, 0 },		// void
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_SAMPLER, LSE_MAXU32, LSE_MAXU32 ), 0, 0 },	// sampler
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE1D, LSE_MAXU32, LSE_MAXU32 ), 0, 0 },	// texture1d
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 ), 0, 0 },	// texture2d
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE3D, LSE_MAXU32, LSE_MAXU32 ), 0, 0 },	// texture3d
			{ CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURECUBE, LSE_MAXU32, LSE_MAXU32 ), 0, 0 },// textureCube
		};
		LSBOOL bFound = false;
		for ( LSUINT32 I = 0; I < LSE_ELEMENTS( satTable ); ++I ) {
			if ( _uUniform.ui64Type == satTable[I].ui64Type ) {
				_ui32Size = satTable[I].ui32Size;
				_ui32Align = satTable[I].ui32Align;
				bFound = true;
				break;
			}
		}
		if ( !bFound || !_ui32Size || !_ui32Align ) {
			assert( 0 );
			_ui32Size = _ui32Align = _uUniform.ui32Size;
		}
		
		if ( _uUniform.ui32Array != LSE_MAXU32 ) {
			_ui32Size = LSE_ROUND_UP( _ui32Size, _ui32Align ) * _uUniform.ui32Array;
		}
	}
	
	/**
	 * Prints a textureX to a given string (appending it).
	 *
	 * \param _uUniform Uniform to print to the given string.
	 * \param _sString String to which to append the given texture.
	 * \param _bIncludeIdx If true, [[texture(X)]] is written.
	 * \return Returns true if there were no memory problems printing the string.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::PrintTexture( const LSG_UNIFORM &_uUniform, CString &_sString, LSBOOL _bIncludeIdx ) {
		if ( IsTexture( _uUniform ) ) {
			LSUINT64 ui64Type = _uUniform.ui64Type;
			
			if ( !DecodeType( ui64Type, _sString ) ) {
				return false;
			}
			LSUINT32 ui32Sem, ui32SemIdx;
			GetSemanticFromUniform( _uUniform.sName, ui32Sem, ui32SemIdx );
			CString sTempTex = _uUniform.sName;
			if ( _bIncludeIdx ) {
				sTempTex += CString( "[[texture(" ) +
					CString::CreateFromUI64( ui32SemIdx ) + CString( ")]]" );
			}
			if ( !_sString.Append( sTempTex ) ) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Prints a sampler to a given string (appending it).
	 *
	 * \param _uUniform Uniform to print to the given string.
	 * \param _sString String to which to append the given sampler.
	 * \param _bIncludeIdx If true, [[sampler(X)]] is written.
	 * \return Returns true if there were no memory problems printing the string.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::PrintSampler( const LSG_UNIFORM &_uUniform, CString &_sString, LSBOOL _bIncludeIdx ) {
		if ( IsSampler( _uUniform ) ) {
			LSUINT64 ui64Type = _uUniform.ui64Type;
			
			if ( !DecodeType( ui64Type, _sString ) ) {
				return false;
			}
			LSUINT32 ui32Sem, ui32SemIdx;
			GetSemanticFromUniform( _uUniform.sName, ui32Sem, ui32SemIdx );
			CString sTempTex = _uUniform.sName;
			if ( _bIncludeIdx ) {
				sTempTex += CString( "[[sampler(" ) +
					CString::CreateFromUI64( ui32SemIdx ) + CString( ")]]" );
			}
			if ( !_sString.Append( sTempTex ) ) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Determines if the given uniform is a texture.
	 *
	 * \param _uUniform Uniform to check for being a texture.
	 * \return Returns true if teh given uniform is a texture.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::IsTexture( const LSG_UNIFORM &_uUniform ) const {
		return CShaderLanguageParser::IsTexture( _uUniform.ui64Type );
	}
	
	/**
	 * Determines if the given uniform is a sampler.
	 *
	 * \param _uUniform Uniform to check for being a sampler.
	 * \return Returns true if teh given uniform is a sampler.
	 */
	LSBOOL LSE_CALL CMetalShaderTranslator::IsSampler( const LSG_UNIFORM &_uUniform ) const {
		return CShaderLanguageParser::IsSampler( _uUniform.ui64Type );
	}

}	// namespace lsg

#endif	// #if defined( LSG_METAL )

#undef LSG_PRINT_ERROR
