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

#include "LSGDirectX11ShaderTranslator.h"
#include "../Gfx/LSGGfx.h"

#ifdef LSG_DX11

/**
 * Help print errors.
 */
#define LSG_PRINT_ERROR( _pcText )	PrintError( _ncData.sError, _pcText, _ncData );

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11ShaderTranslator::CDirectX11ShaderTranslator() {
	}
	LSE_CALLCTOR CDirectX11ShaderTranslator::~CDirectX11ShaderTranslator() {
	}

	// == Functions.
	/**
	 * Modify the final shader string after it has been completed.  Override to modify it.
	 *
	 * \param _sShader The final string, modifiable in-place.
	 * \param _sError The error string to be filled if there is an error modifying the shader text.
	 * \return Returns false if an error occurred.
	 */
	LSBOOL LSE_CALL CDirectX11ShaderTranslator::PostShaderString( CString &_sShader, CString &_sError ) {
		static const char * pcHeader =
			"SamplerState lsg_SamplerBiLinearRepeat:register(s0){Filter=MIN_MAG_LINEAR_MIP_POINT;AddressU=WRAP;AddressV=WRAP;};\n"
			"SamplerState lsg_SamplerBiLinearClamp:register(s1){Filter=MIN_MAG_LINEAR_MIP_POINT;AddressU=CLAMP;AddressV=CLAMP;};\n";
		CString sHeader;
		/*if ( !sHeader.Insert( 0, pcHeader ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}*/
		if ( !sHeader.Append( CString( "SamplerState lsg_SamplerShadow:register(s" ) + CString::CreateFromUI64( CGfx::GetMetrics().ui32MaxTexSlot - 1 ) + CString( "){Filter=MIN_MAG_LINEAR_MIP_POINT;AddressU=CLAMP;AddressV=CLAMP;};\n" ) ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sHeader.Append( CString( "float shadow2dDepth( Texture2D _tTexture, float2 _vCoord ){ return _tTexture.SampleLevel( lsg_SamplerShadow, _vCoord, 0 ).x; }\n"
			"float tex2DShadowCompare( Texture2D _tTexture, float2 _vCoord, float _fComp ){"
				"float fDepth = shadow2dDepth( _tTexture, _vCoord );"
				"if ( fDepth == 1.0f ){ fDepth = _fComp; }"
				"return step( _fComp, fDepth ); }\n"
			"float tex2DShadow( Texture2D _tTexture, float4 _vInvTexSize, float2 _vCoord, float _fComp){"
				"float fX = _vInvTexSize.x * 0.5f;"
				"float fY = _vInvTexSize.y * 0.5f;"
				"float fLb = tex2DShadowCompare(_tTexture,_vCoord+float2(-fX,-fY),_fComp);"
				"float fLt = tex2DShadowCompare(_tTexture,_vCoord+float2(-fX,fY),_fComp);"
				"float fRb = tex2DShadowCompare(_tTexture,_vCoord+float2(fX,-fY),_fComp);"
				"float fRt = tex2DShadowCompare(_tTexture,_vCoord+float2(fX,fY),_fComp);"
				"float2 fFrac = frac( _vCoord * _vInvTexSize.zw + float2( 0.5f, 0.5f ) );"
				"float fA = lerp( fLb, fLt, fFrac.y );"
				"float fB = lerp( fRb, fRt, fFrac.y );"
				"return lerp( fA, fB, fFrac.x );"
			"}\n"
			"float linstep( float fLow, float fHigh, float fV ) { return saturate( (fV - fLow) / (fHigh - fLow) ); }\n" ) ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}

		// Remove all unreferenced globals.
		for ( uint32_t I = m_ulGlobalUniforms.Length(); I--; ) {
			if ( !m_ulGlobalUniforms[I].ui32RefCount ) {
				for ( uint32_t J = m_uslGlobalSemantics.Length(); J--; ) {
					if ( m_uslGlobalSemantics[J].sName == m_ulGlobalUniforms[I].sName ) {
						m_uslGlobalSemantics.RemoveNoDealloc( J );
					}
				}
				m_ulGlobalUniforms.RemoveNoDealloc( I );
			}
		}

		// Resize 3x3 and 2x2 matrices.  And minimize samplers.
		for ( uint32_t I = m_ulGlobalUniforms.Length(); I--; ) {
			LSBOOL bAdjustArray = false;
			if ( m_ulGlobalUniforms[I].ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 3, 3 ) ) {
				m_ulGlobalUniforms[I].ui32Size = 4 * 3 * sizeof( float );
				bAdjustArray = true;
			}
			else if ( m_ulGlobalUniforms[I].ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, 2, 2 ) ) {
				m_ulGlobalUniforms[I].ui32Size = 4 * 2 * sizeof( float );
				bAdjustArray = true;
			}
			else if ( CShaderLanguageParser::IsSampler( m_ulGlobalUniforms[I].ui64Type ) ||
				CShaderLanguageParser::IsTexture( m_ulGlobalUniforms[I].ui64Type ) ) {
				m_ulGlobalUniforms[I].ui32Size = 0;
			}
			else if ( m_ulGlobalUniforms[I].ui32Array != LSE_MAXU32 && m_ulGlobalUniforms[I].ui32Array ) {
				// Anything that is an array needs to be readjusted.
				m_ulGlobalUniforms[I].ui32Size /= m_ulGlobalUniforms[I].ui32Array;
				bAdjustArray = true;
			}

			// Additionally, on all array types, the basic type is no fewer than 16 bytes for each element.
			if ( bAdjustArray && m_ulGlobalUniforms[I].ui32Array != LSE_MAXU32 ) {
				uint32_t ui32Size = m_ulGlobalUniforms[I].ui32Size;
				m_ulGlobalUniforms[I].ui32Size = LSE_ROUND_UP( ui32Size, 16 ) * m_ulGlobalUniforms[I].ui32Array;
			}
		}

		// Group uniforms into constant buffers based off known update frequencies.
		// ui32Frequent changes several times per frame.
		static const uint32_t ui32Frequent[] = {
			CShaderParser::token::LSG_SLT_MATAMBIENT,
			CShaderParser::token::LSG_SLT_MATDIFFUSE,
			CShaderParser::token::LSG_SLT_MATSPECULAR,
			CShaderParser::token::LSG_SLT_MATPOWER,
			CShaderParser::token::LSG_SLT_REFLECTIVITY,
			CShaderParser::token::LSG_SLT_ROUGHNESS,
			CShaderParser::token::LSG_SLT_ASHIKFACTORS,
		};
		// ui32MaybeFrequent might change several times per frame under some conditions.
		static const uint32_t ui32MaybeFrequent[] = {
			CShaderParser::token::LSG_SLT_LIGHTCOUNT,
			CShaderParser::token::LSG_SLT_DIRLIGHTDIRS,
			CShaderParser::token::LSG_SLT_POINTLIGHTPOS,
			CShaderParser::token::LSG_SLT_SPOTLIGHTDIRS,
			CShaderParser::token::LSG_SLT_LIGHTAMBIENT,
			CShaderParser::token::LSG_SLT_LIGHTDIFFUSE,
			CShaderParser::token::LSG_SLT_LIGHTSPECULAR,
			CShaderParser::token::LSG_SLT_LIGHTATT,
			CShaderParser::token::LSG_SLT_SPOTLIGHTEXP,
			CShaderParser::token::LSG_SLT_SPOTLIGHTRANGE,
		};
		// ui32MedFrequent changes around once per frame.
		static const uint32_t ui32MedFrequent[] = {
			CShaderParser::token::LSG_SLT_NORMALMATRIX,
			CShaderParser::token::LSG_SLT_TEXMATRIX,
			CShaderParser::token::LSG_SLT_WORLD,
			CShaderParser::token::LSG_SLT_WORLDVIEW,
			CShaderParser::token::LSG_SLT_WORLDVIEWPROJ,
			CShaderParser::token::LSG_SLT_VIEW,
			CShaderParser::token::LSG_SLT_CAMPOS,
			CShaderParser::token::LSG_SLT_TIMESINCELASTFRAME,
			CShaderParser::token::LSG_SLT_SHADOWMAPPING,
		};
		if ( !m_vCBuffers.Resize( 4 ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}

		CString sBuffer0, sBuffer1, sBuffer2, sBuffer3;
		if ( !sBuffer0.Set( "cbuffer cb0:register(b0){\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sBuffer1.Set( "cbuffer cb1:register(b1){\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sBuffer2.Set( "cbuffer cb2:register(b2){\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sBuffer3.Set( "cbuffer cb3:register(b3){\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		static const uint32_t * ui32CBuffers[] = {
			ui32Frequent,
			ui32MaybeFrequent,
			ui32MedFrequent,
		};
		static const uint32_t ui32Sizes[] = {
			LSE_ELEMENTS( ui32Frequent ),
			LSE_ELEMENTS( ui32MaybeFrequent ),
			LSE_ELEMENTS( ui32MedFrequent ),
		};

		uint32_t ui32Bases[LSE_ELEMENTS( ui32CBuffers )+1] = {
			0,
		};

		uint32_t ui32Base = 0;
		for ( uint32_t K = 0; K < LSE_ELEMENTS( ui32CBuffers ); ++K ) {
			ui32Bases[K] = ui32Base;
			// Fill the cbuffers in order.
			for ( uint32_t I = 0; I < m_ulGlobalUniforms.Length(); ++I ) {
				// Is it a semantic?
				uint32_t ui32Semantic = LSE_MAXU32;
				//uint32_t ui32SemanticIndex = LSE_MAXU32;
				for ( uint32_t J = m_uslGlobalSemantics.Length(); J--; ) {
					if ( m_uslGlobalSemantics[J].sName == m_ulGlobalUniforms[I].sName ) {
						// Yes.
						ui32Semantic = m_uslGlobalSemantics[J].ui32SemanticType;
						break;
					}
				}
				if ( ui32Semantic != LSE_MAXU32 && ui32Semantic != CShaderParser::token::LSG_SLT_TEXUNIT && ui32Semantic != CShaderParser::token::LSG_SLT_SHADOWMAPUNIT ) {
					for ( uint32_t J = ui32Sizes[K]; J--; ) {
						if ( ui32Semantic == ui32CBuffers[K][J] ) {
							uint32_t ui32Offset = m_vCBuffers[K].AddUniform( m_ulGlobalUniforms[I], LSE_MAXU32 );
							m_ulGlobalUniforms[I].uiptrApiSpecific = ui32Offset;
							m_ulGlobalUniforms[I].uiptrApiParam = K;
							if ( ui32Offset == LSE_MAXU32 ) {
								_sError = LSG_STB_OOM;
								return false;
							}
							break;
						}
					}
				}
			}
			if ( m_vCBuffers[K].Uniforms().Length() ) {
				uint32_t ui32Index = m_vCBuffers[K].Uniforms().Length() - 1;
				ui32Base = LSE_ROUND_UP( m_vCBuffers[K].Uniforms()[ui32Index].ui32BufferOffset + m_vCBuffers[K].Uniforms()[ui32Index].ui32Size, 16 );
			}
		}
		// Everything not added to a cbuffer goes to the last cbuffer.
		ui32Bases[LSE_ELEMENTS( ui32CBuffers )] = ui32Base;
		for ( uint32_t I = 0; I < m_ulGlobalUniforms.Length(); ++I ) {
			// Is it a semantic?
			uint32_t ui32Semantic = LSE_MAXU32;
			for ( uint32_t J = m_uslGlobalSemantics.Length(); J--; ) {
				if ( m_uslGlobalSemantics[J].sName == m_ulGlobalUniforms[I].sName ) {
					// Yes.
					ui32Semantic = m_uslGlobalSemantics[J].ui32SemanticType;
					break;
				}
			}
			if ( ui32Semantic != CShaderParser::token::LSG_SLT_TEXUNIT && ui32Semantic != CShaderParser::token::LSG_SLT_SHADOWMAPUNIT ) {
				if ( static_cast<uint32_t>(m_ulGlobalUniforms[I].uiptrApiSpecific) == LSE_MAXU32 ) {
					uint32_t ui32Offset = m_vCBuffers[3].AddUniform( m_ulGlobalUniforms[I], LSE_MAXU32 );
					m_ulGlobalUniforms[I].uiptrApiSpecific = ui32Offset;
					m_ulGlobalUniforms[I].uiptrApiParam = 3;
					if ( ui32Offset == LSE_MAXU32 ) {
						_sError = LSG_STB_OOM;
						return false;
					}
				}
			}
		}



		for ( uint32_t I = 0; I < m_ulGlobalUniforms.Length(); ++I ) {
			
			// Is it a semantic?
			uint32_t ui32Semantic = LSE_MAXU32;
			uint32_t ui32SemanticIndex = LSE_MAXU32;
			for ( uint32_t J = m_uslGlobalSemantics.Length(); J--; ) {
				if ( m_uslGlobalSemantics[J].sName == m_ulGlobalUniforms[I].sName ) {
					// Yes.
					ui32Semantic = m_uslGlobalSemantics[J].ui32SemanticType;
					ui32SemanticIndex = m_uslGlobalSemantics[J].ui32SemanticIndex;
					break;
				}
			}

			// Samplers are a special case.
			CString * psUseMe = NULL;
			uint32_t ui32CBufferIndex = LSE_MAXU32;
			if ( ui32Semantic == CShaderParser::token::LSG_SLT_TEXUNIT || ui32Semantic == CShaderParser::token::LSG_SLT_SHADOWMAPUNIT ) {
				// Don't add it to a cbuffer.
				psUseMe = &sHeader;
			}
			// Not a semantic?  Add it to the last cbuffer.
			if ( ui32Semantic != LSE_MAXU32 && !psUseMe ) {
				// Find it in one of the lists.
				for ( uint32_t J = LSE_ELEMENTS( ui32Frequent ); J--; ) {
					if ( ui32Semantic == ui32Frequent[J] ) {
						psUseMe = &sBuffer0;
						ui32CBufferIndex = 0;
						break;
					}
				}
				if ( !psUseMe ) {
					for ( uint32_t J = LSE_ELEMENTS( ui32MaybeFrequent ); J--; ) {
						if ( ui32Semantic == ui32MaybeFrequent[J] ) {
							psUseMe = &sBuffer1;
							ui32CBufferIndex = 1;
							break;
						}
					}
				}
				if ( !psUseMe ) {
					for ( uint32_t J = LSE_ELEMENTS( ui32MedFrequent ); J--; ) {
						if ( ui32Semantic == ui32MedFrequent[J] ) {
							psUseMe = &sBuffer2;
							ui32CBufferIndex = 2;
							break;
						}
					}
				}
			}
			if ( !psUseMe ) {
				// Not in any buffers.
				psUseMe = &sBuffer3;
				ui32CBufferIndex = 3;
			}

			// Type.
			if ( !DecodeType( m_ulGlobalUniforms[I].ui64Type, (*psUseMe) ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}

			// Name.
			if ( !psUseMe->Append( m_ulGlobalUniforms[I].sName ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}

			// Optional array.
			if ( m_ulGlobalUniforms[I].ui32Array != LSE_MAXU32 ) {
				if ( !psUseMe->Append( CString( "[" ) + CString::CreateFromUI64( m_ulGlobalUniforms[I].ui32Array ) + CString( "]" ) ) ) {
					_sError = LSG_STB_OOM;
					return false;
				}
			}

			// Samplers.
			if ( ui32Semantic == CShaderParser::token::LSG_SLT_TEXUNIT ) {
				if ( CShaderLanguageParser::IsSampler( m_ulGlobalUniforms[I].ui64Type ) ) {
					if ( !psUseMe->Append( CString( ":register(s" ) + CString::CreateFromUI64( ui32SemanticIndex ) + CString( ")" ) ) ) {
						_sError = LSG_STB_OOM;
						return false;
					}
				}
				else if ( CShaderLanguageParser::IsTexture( m_ulGlobalUniforms[I].ui64Type ) ) {
					if ( !psUseMe->Append( CString( ":register(t" ) + CString::CreateFromUI64( ui32SemanticIndex ) + CString( ")" ) ) ) {
						_sError = LSG_STB_OOM;
						return false;
					}
				}
			}
			else if ( ui32Semantic == CShaderParser::token::LSG_SLT_SHADOWMAPUNIT ) {
				if ( !psUseMe->Append( CString( ":register(t" ) + CString::CreateFromUI64( CStd::Min<uint32_t>( LSG_MAX_TEXTURE_UNITS, CGfx::GetMetrics().ui32MaxTexSlot ) - 1 ) + CString( ")" ) ) ) {
					_sError = LSG_STB_OOM;
					return false;
				}
			}
			else {
				uint32_t ui32Offset = static_cast<uint32_t>(m_ulGlobalUniforms[I].uiptrApiSpecific);// + ui32Bases[m_ulGlobalUniforms[I].uiptrApiParam];
				uint32_t ui32BaseOffset = ui32Offset / 16;
				uint32_t ui32Shift = (ui32Offset >> 2) & 0x3;
				static const char * pcLetter[] = {
					".x",
					".y",
					".z",
					".w",
				};
				CString sLetter = pcLetter[ui32Shift];
				if ( !psUseMe->Append( CString( ":packoffset(c" ) + CString::CreateFromUI64( ui32BaseOffset ) + sLetter + CString( ")" ) ) ) {
					_sError = LSG_STB_OOM;
					return false;
				}
			}


			// End of declaration.
			if ( !psUseMe->Append( ";\r\n" ) ) {
				_sError = LSG_STB_OOM;
				return false;
			}
			
		}

		if ( !sBuffer0.Append( "};\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sBuffer1.Append( "};\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sBuffer2.Append( "};\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sBuffer3.Append( "};\r\n" ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}

		// Add all of the cbuffers.
		if ( !sHeader.Append( sBuffer0 ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sHeader.Append( sBuffer1 ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sHeader.Append( sBuffer2 ) ) {
			_sError = LSG_STB_OOM;
			return false;
		}
		if ( !sHeader.Append( sBuffer3 ) ) {
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
	CDirectX11ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX11ShaderTranslator::HandleFunctionDeclaration( LSG_NODE_CALLBACK &_ncData ) {
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


		//// If the owner of this node is a function definition, push the parameters.
		//if ( HasParentOfType( _ncData, LSG_N_FUNCDEFINITION ) ) {
		//	LSG_PROCESSING_DIRECTIVE pdRet = RegisterParmsWithNewScope( _ncData );
		//	if ( pdRet != LSG_PD_CONTINUE ) {
		//		return pdRet;
		//	}
		//}
	}

	/**
	 * Handles nodes of type LSG_N_INITLISTWRAP.
	 *
	 * \param _ncData All data needed for the parsing.
	 * \return Returns a LSG_PROCESSING_DIRECTIVE enumerated value indicating whether the processor should go into the sub-nodes
	 *	of the node or not.
	 */
	CDirectX11ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX11ShaderTranslator::HandleInitListWrap( LSG_NODE_CALLBACK &_ncData ) {
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
	CDirectX11ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX11ShaderTranslator::HandleSingleDeclaration( LSG_NODE_CALLBACK &_ncData ) {
		// The default handler for this adds the value to the current scope.
		LSG_PROCESSING_DIRECTIVE pdRet = RegisterVariableWithCurrentScope( _ncData );
		if ( pdRet != LSG_PD_CONTINUE ) { return pdRet; }


		// Variable names may not begin with gl_ or lsg_.
		if ( CStd::StrNICmp( _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) ).CStr(),
			"gl_", 3 ) == 0 ||
			CStd::StrNICmp( _ncData.pslpContainer->GetName( _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32SingleDeclName ) ).CStr(),
			"lsg_", 4 ) == 0 ) {
			LSG_PRINT_ERROR( "Variable names may not begin with \"gl_\" or \"lsg_\", regardless of case." );
			return LSG_PD_ERROR;
		}


		// Check the semantic (only on globals).
		if ( m_sssScopes.TotalScopes() == 1 && _ncData.pndNode->ui32Semantic != LSE_MAXU32 ) {
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndSemantic = _ncData.pslpContainer->GetNode( _ncData.pndNode->ui32Semantic );
			
			LSG_UNIFORM_SEMANTIC usThis;
			usThis.ui64Type = _ncData.pslpContainer->GetType( _ncData.pslpContainer->GetNode( _ncData.pndNode->nuNodeData.ui32NodeIndex ) );
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
			uThis.uiptrApiSpecific = LSE_MAXU32;
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

		else {
			if ( _ncData.pndNode->ui32Semantic != LSE_MAXU32 ) {
				// Semantic.
				if ( !PushNode( (*_ncData.pnsNodeStack), _ncData.pndNode->ui32Semantic, _ncData.ui32Index ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
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
	CDirectX11ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX11ShaderTranslator::HandleIntrinsic( LSG_NODE_CALLBACK &_ncData ) {
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
			{ "Sample",				CShaderParser::token::LSG_SLT_TEX1D,				3 },
			{ "Sample",				CShaderParser::token::LSG_SLT_TEX2D,				3 },
			{ "Sample",				CShaderParser::token::LSG_SLT_TEX3D,				3 },
			{ "Sample",				CShaderParser::token::LSG_SLT_TEXCUBE,				3 },
			{ "tex2DShadowCompare",	CShaderParser::token::LSG_SLT_TEX2DSHADOWCOMPARE,	4 },
			{ "tex2DShadow",		CShaderParser::token::LSG_SLT_TEX2DSHADOW,			5 },
			{ "transpose",			CShaderParser::token::LSG_SLT_TRANSPOSE,			1 },
		};

		for ( uint32_t I = LSE_ELEMENTS( sFuncs ); I--; ) {
			if ( sFuncs[I].ui32Id == _ncData.pndNode->nuNodeData.ui32NodeIndex ) {
				// Found it.
				// Verify the number of parameters.
				char szPrefix[128];
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

					// texX( TextureX, Sampler, Uv ) = ui32Args[2].Sample(ui32Args[1],ui32Args[0])

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
	CDirectX11ShaderTranslator::LSG_PROCESSING_DIRECTIVE LSE_CALL CDirectX11ShaderTranslator::HandleSemantic( LSG_NODE_CALLBACK &_ncData ) {
		const LSG_STACK_OBJ * psoObj = GetParentOfType( _ncData, LSG_N_PARMDECLARATION );
		static const struct LSG_DIRECTX11_SEMANTIC {
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
			{ "SV_POSITION%u",						CShaderParser::token::LSG_SLT_POSITION },
			{ "POSITIONT",							CShaderParser::token::LSG_SLT_POSITIONT },
			{ "SV_ISFRONTFACE",						CShaderParser::token::LSG_SLT_ISFRONTFACE },
			{ "PSIZE%u",							CShaderParser::token::LSG_SLT_PSIZE },
			{ "TANGENT%u",							CShaderParser::token::LSG_SLT_TANGENT },
			{ "TEXCOORD%u",							CShaderParser::token::LSG_SLT_TEXCOORD },
			{ "FOG",								CShaderParser::token::LSG_SLT_FOG },
			{ "DEPTH",								CShaderParser::token::LSG_SLT_DEPTH },
		};

		// Handle special cases.

		// Output from pixel shader as COLOR = SV_Target.
		if ( psoObj ) {
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndThis = _ncData.pslpContainer->GetNode( psoObj->ui32Index );
			if ( m_pslpContainer->GetShaderType() == LSG_ST_PIXEL && (ndThis.nuNodeData.ui64Const & LSG_SQ_OUT) && _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx == CShaderParser::token::LSG_SLT_COLOR ) {
				
				if ( !_ncData.psResult->Append( CString( ":SV_Target" ) + CString::CreateFromUI64( _ncData.pndNode->nuoOp.ui32NodeIndex3 ) ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				return LSG_PD_SKIP_SUB_NODES;
			}
			if ( m_pslpContainer->GetShaderType() == LSG_ST_PIXEL && (ndThis.nuNodeData.ui64Const & LSG_SQ_OUT) && _ncData.pndNode->nueNodeDataEx.ui32NodeIndexEx == CShaderParser::token::LSG_SLT_DEPTH && _ncData.pndNode->nuoOp.ui32NodeIndex3 == 0 ) {
				
				if ( !_ncData.psResult->Append( ":SV_Depth" ) ) {
					_ncData.sError = LSG_STB_OOM;
					return LSG_PD_ERROR;
				}
				return LSG_PD_SKIP_SUB_NODES;
			}
		}
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
	LSBOOL LSE_CALL CDirectX11ShaderTranslator::DecodeTypeQualifier( uint64_t _ui64Qualifier, CString &_sString ) {
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
	LSBOOL LSE_CALL CDirectX11ShaderTranslator::DecodeType( uint64_t _ui64Type, CString &_sString ) {
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
			{ CShaderParser::token::LSG_SLT_SAMPLER,			"SamplerState ","",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE1D,			"Texture1D ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE2D,			"Texture2D ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURE3D,			"Texture3D ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_TEXTURECUBE,		"TextureCube ",	"",						"" },
			{ CShaderParser::token::LSG_SLT_FLOAT,				"float ",		"vector<float,%u>",		"matrix<float,%u,%u>" },
			{ CShaderParser::token::LSG_SLT_INT,				"int ",			"vector<int,%u>",		"" },
			{ CShaderParser::token::LSG_SLT_UINT,				"uint ",		"vector<uint,%u>",		"" },
			{ CShaderParser::token::LSG_SLT_BOOL,				"bool ",		"vector<bool,%u>",		"" },
		};
		uint32_t ui32Type = CShaderLanguageParser::GetType( _ui64Type );
		for ( uint32_t I = 0; I < LSE_ELEMENTS( sType ); I++ ) {
			if ( sType[I].ui32Type == ui32Type ) {
				char szBuffer[32];
				uint32_t ui32Array0 = CShaderLanguageParser::GetArrayUnit( _ui64Type, 0 );
				uint32_t ui32Array1 = CShaderLanguageParser::GetArrayUnit( _ui64Type, 1 );
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

}	// namespace lsg

#endif	// #ifdef LSG_DX11
