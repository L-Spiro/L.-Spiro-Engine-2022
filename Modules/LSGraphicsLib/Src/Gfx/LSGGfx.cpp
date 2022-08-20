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
 * Description: The upper-level interface with the underlying graphics API.
 */

#include "LSGGfx.h"
#include "../IndexBuffer/LSGIndexBuffer.h"
#include "../ShaderLanguage/LSGShader.h"
#include "../VertexBuffer/LSGVertexBuffer.h"

namespace lsg {

	// == Members.
	/** The number of triangles drawn. */
	uint64_t CGfx::m_ui64TrianglesDrawn = 0ULL;

	// == Functions.
	/**
	 * Renders the scene with the optionally provided index buffer.
	 *
	 * \param _pibIndexBuffer The optional index buffer.
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32PrimitiveCount Number of primitives to render.  If 0, the total number of primitives in vertex buffer 0 are drawn.
	 */
	void LSE_CALL CGfx::Render( void * _pibIndexBuffer,
		uint32_t _ui32StartVertex,
		uint32_t _ui32PrimitiveCount ) {
		if ( !m_bActive || !TotalActiveVertexBuffers() ) { return; }	// Nothing to draw.
		assert( VertexBuffers()[0].pvbVertBuffer );		// Must be a vertex buffer in slot 0 at least.
		CShader * psShader = GetShader();
		if ( psShader ) {

			const CIndexBuffer * pibIndexBuffer = NULL;
			if ( _pibIndexBuffer ) {
				pibIndexBuffer = static_cast<const CIndexBuffer *>(_pibIndexBuffer);
			}

			if ( !_ui32PrimitiveCount ) {
				if ( _pibIndexBuffer ) {
					_ui32PrimitiveCount = pibIndexBuffer->Primitives();
				}
				else {
					_ui32PrimitiveCount = VertexBuffers()[0].pvbVertBuffer->Primitives();
				}
			}


			// Update the states.
			if ( m_bWorldDirty || m_bViewDirty ) {
				// Create the world-view matrix.
				m_mWorldViewMatrix = m_mWorldMatrix * m_mViewMatrix;
				m_mInverseWorldViewMatrix = m_mWorldViewMatrix;
				m_mInverseWorldViewMatrix.Transpose();
				m_mInverseWorldViewMatrix = m_mInverseWorldViewMatrix.InverseAccurate();
				m_mNormalMatrix = m_mWorldViewMatrix.Get3x3Matrix();
				/*m_mNormalMatrix.GetRow( 0 ).Normalize();
				m_mNormalMatrix.GetRow( 1 ).Normalize();
				m_mNormalMatrix.GetRow( 2 ).Normalize();*/
				//m_mNormalMatrix.Transpose();
				/*m_mNormalMatrix = m_mNormalMatrix.Inverse();*/
			}
			if ( m_bWorldDirty || m_bViewDirty || m_bProjDirty ) {
				// Create the world-view-projection matrix.
				/*if ( m_bFlipRender ) {
					m_mProjMatrix._22 = -m_mProjMatrix._22;
				}*/
				m_mWorldViewProjMatrix = m_mWorldViewMatrix * m_mProjMatrix;
			}

			// Update the shader.
			psShader->SetMatrix4x4( psShader->m_euEngineHandles.hWorldMatrix, GetWorldMatrix() );
			psShader->SetMatrix4x4( psShader->m_euEngineHandles.hWorldViewMatrix, m_mWorldViewMatrix );
			psShader->SetMatrix4x4( psShader->m_euEngineHandles.hWorldViewProjMatrix, m_mWorldViewProjMatrix );
			psShader->SetMatrix4x4( psShader->m_euEngineHandles.hViewMatrix, m_mViewMatrix );
			psShader->SetMatrix3x3( psShader->m_euEngineHandles.hNormalMatrix, m_mNormalMatrix );
			psShader->SetVector3( psShader->m_euEngineHandles.hCamPos, m_vCamPos );

			// Materials.
			psShader->SetVector4( psShader->m_euEngineHandles.hDiffuseMaterial, m_vDiffuseMaterial );
			psShader->SetVector4( psShader->m_euEngineHandles.hEmissiveMaterial, m_vEmissiveMaterial );
			psShader->SetVector4( psShader->m_euEngineHandles.hSpecularMaterial, m_vSpecularMaterial );
			psShader->SetFloat( psShader->m_euEngineHandles.hPower, static_cast<float>(m_fPower) );
			psShader->SetVector4( psShader->m_euEngineHandles.hRoughness, m_vRoughness );
			//psShader->SetFloat( psShader->m_euEngineHandles.hReflectivity, static_cast<float>(m_fReflectivity) );
			psShader->SetVector4( psShader->m_euEngineHandles.hAshikFactor, m_vAshikFactors );

			// Lights.
			{
				CVector4 vDirsAndPos[LSG_MAX_LIGHTS];
				CMatrix4x4 mInvTransCam = m_mViewMatrix;
				if ( m_ui32TotalDirLights ) {
					CMatrix4x4::MultiplyVec4Batch( mInvTransCam, m_vDirLightDir, m_ui32TotalDirLights, vDirsAndPos );

					psShader->SetVector4Array( psShader->m_euEngineHandles.hDirLightDirs, vDirsAndPos, m_ui32TotalDirLights );
					psShader->SetVector4Array( psShader->m_euEngineHandles.hDirLightColors, m_vDirLightColor, m_ui32TotalDirLights );
					psShader->SetInt( psShader->m_euEngineHandles.hDirLights, m_ui32TotalDirLights );
				}

				if ( m_ui32TotalPointLights ) {
					CMatrix4x4::MultiplyVec4Batch( mInvTransCam, m_vPointLightPos, m_ui32TotalPointLights, vDirsAndPos );
					psShader->SetVector4Array( psShader->m_euEngineHandles.hPointLightPos, vDirsAndPos, m_ui32TotalPointLights );
					psShader->SetVector4Array( psShader->m_euEngineHandles.hPointLightColors, m_vPointLightColor, m_ui32TotalPointLights );
					psShader->SetFloatArray( psShader->m_euEngineHandles.hPointLightSizes, m_fPointLightSize, m_ui32TotalPointLights );
					psShader->SetInt( psShader->m_euEngineHandles.hPointLights, m_ui32TotalPointLights );
				}
			}

			// Activate the vertex buffers on each stream.
			for ( uint32_t I = 0; I < TotalActiveVertexBuffers(); ++I ) {
				uint32_t ui32CurOff = VertexBuffers()[I].pvbVertBuffer ? VertexBuffers()[I].ui32Offset : 0;
				uint32_t ui32LastOff = m_rsLastRenderState.pvbVertexBuffers[I].pvbVertBuffer ? m_rsLastRenderState.pvbVertexBuffers[I].ui32Offset : 0;

				if ( VertexBuffers()[I].pvbVertBuffer == m_rsLastRenderState.pvbVertexBuffers[I].pvbVertBuffer && ui32CurOff == ui32LastOff ) {
					// Vertex buffer is the same as last time (which means the last vertex buffer is implicitly a valid pointer) so
					//	make sure the vertex buffer itself has not been modified (new ID etc.) since last render.
					uint32_t ui32CurId = VertexBuffers()[I].pvbVertBuffer ? VertexBuffers()[I].pvbVertBuffer->GetId() : 0;
					uint32_t ui32LastId = m_rsLastRenderState.pvbVertexBuffers[I].pvbVertBuffer ? m_rsLastRenderState.pvbVertexBuffers[I].pvbVertBuffer->GetId() : 0;

					if ( ui32CurId == ui32LastId ) { continue; }
				}

				if ( VertexBuffers()[I].pvbVertBuffer ) {
					if ( !VertexBuffers()[I].pvbVertBuffer->SetStreamApi( I, VertexBuffers()[I].ui32Offset ) ) {
						CStd::DebugPrintA( "CGfx::Render(): Failed to activate vertex on stream." );	// TODO: Make a better message.
					}
				}

				m_rsLastRenderState.pvbVertexBuffers[I] = VertexBuffers()[I];
			}

			// Apply render states.
#ifdef LSA_TOOLS
			ApplyRenderStates( true );
#else
			ApplyRenderStates();
#endif	// #ifdef LSA_TOOLS

			// Render.
			psShader->PreRender();
			for ( uint32_t I = 0; I < TotalActiveVertexBuffers(); ++I ) {
				if ( VertexBuffers()[I].pvbVertBuffer ) {
					VertexBuffers()[I].pvbVertBuffer->PrepareToRenderApi();
				}
			}

			if ( _pibIndexBuffer ) {
				VertexBuffers()[0].pvbVertBuffer->RenderApi( 0, 0 );		// Makes it apply certain necessary settings before rendering.
				pibIndexBuffer->RenderApi( _ui32StartVertex, _ui32PrimitiveCount );
			}
			else {
				VertexBuffers()[0].pvbVertBuffer->RenderApi( _ui32StartVertex, _ui32PrimitiveCount );
			}
		}

		// Clear the vertex buffers.
		for ( uint32_t I = TotalActiveVertexBuffers(); I--; ) {
			if ( VertexBuffers()[I].pvbVertBuffer ) {
				VertexBuffers()[I].pvbVertBuffer->EndRenderApi();
			}
			VertexBuffers()[I].pvbVertBuffer = NULL;
		}
		TotalActiveVertexBuffers() = 0;

		// Clear textures.
		/*for ( uint32_t I = RenderState().ui32MaxTextureSet; I--; ) {
			RenderState().psrShaderResources[I] = 0;
			RenderState().ui32TextureId[I] = 0;
		}
		m_rsLastRenderState.ui32MaxTextureSet = m_rsCurRenderState.ui32MaxTextureSet;
		RenderState().ui32MaxTextureSet = 0;*/

		m_ui64TrianglesDrawn += _ui32PrimitiveCount;
	}

	/**
	 * Sets the screen dimensions.
	 *
	 * \param _ui32Width Screen width, in pixels.
	 * \param _ui32Height Screen height, in pixels.
	 */
	void LSE_CALL CGfx::SetScreenSize( uint32_t _ui32Width, uint32_t _ui32Height ) {
		m_ui32ScreenWidth = _ui32Width;
		m_ui32ScreenHeight = _ui32Height;
#ifdef LSG_DX9
		CDirectX9::SetScreenSize( _ui32Width, _ui32Height );
#endif	// #ifdef LSG_DX9
#ifdef LSG_DX11
		CDirectX11::SetScreenSize( _ui32Width, _ui32Height );
#endif	// #ifdef LSG_DX11
	}

}	// namespace lsg
