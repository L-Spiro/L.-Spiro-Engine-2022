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


#ifndef __LSG_GFX_H__
#define __LSG_GFX_H__

#include "../LSGGraphicsLib.h"
#ifdef LSG_DX11
#include "../DirectX11/LSGDirectX11.h"
#elif defined( LSG_DX9 )
#include "../DirectX9/LSGDirectX9.h"
#elif defined( LSG_OGL )
#include "../OpenGL/LSGOpenGl.h"
#elif defined( LSG_OGLES )
#include "../OpenGLES/LSGOpenGlEs.h"
#elif defined( LSG_METAL )
#include "../Metal/LSGMetal.h"
#elif defined( LSG_VKN )
#include "../Vulkan/LSGVulkan.h"
#endif	// #ifdef LSG_DX11

namespace lsg {

	/**
	 * Class CGfx
	 * \brief The upper-level interface with the underlying graphics API.
	 *
	 * Description: The upper-level interface with the underlying graphics API.
	 */
	class CGfx : public
#ifdef LSG_DX11
		CDirectX11
#elif defined( LSG_DX9 )
		CDirectX9
#elif defined( LSG_OGL )
		COpenGl
#elif defined( LSG_OGLES )
		COpenGlEs
#elif defined( LSG_METAL )
		CMetal
#elif defined( LSG_VKN )
		CVulkan
#endif	// #ifdef LSG_DX11
	{
	public :
		// == Functions.
		/**
		 * Renders the scene with the optionally provided index buffer.
		 *
		 * \param _pibIndexBuffer The optional index buffer.
		 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
		 * \param _ui32PrimitiveCount Number of primitives to render.  If 0, the total number of primitives in vertex buffer 0 are drawn.
		 */
		static void LSE_CALL									Render( void * _pibIndexBuffer = NULL,
			uint32_t _ui32StartVertex = 0,
			uint32_t _ui32PrimitiveCount = 0 );

		/**
		 * Sets the current framebuffer (or NULL to activate the backbuffers).
		 *
		 * \param _pfFramebuffer The framebuffer to set, or NULL to activate the backbuffers.
		 */
		static LSE_INLINE void LSE_CALL							SetFramebuffer( CFramebuffer * _pfFramebuffer );

		/**
		 * Gets the current framebuffer.
		 *
		 * \return Returns the currently set framebuffer, which may be NULL.
		 */
		static LSE_INLINE CFramebuffer * LSE_CALL				GetFramebuffer();

		/**
		 * Gets the system metrics.
		 *
		 * \return Returns the system metrics.
		 */
		static LSE_INLINE const LSG_METRICS LSE_CALL			GetMetrics();

		/**
		 * Sets the screen dimensions.
		 *
		 * \param _ui32Width Screen width, in pixels.
		 * \param _ui32Height Screen height, in pixels.
		 */
		static void LSE_CALL									SetScreenSize( uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Gets the screen width in pixels.
		 *
		 * \return Returns the screen width in pixels.
		 */
		static LSE_INLINE uint32_t LSE_CALL						ScreenWidth();

		/**
		 * Gets the screen height in pixels.
		 *
		 * \return Returns the screen height in pixels.
		 */
		static LSE_INLINE uint32_t LSE_CALL						ScreenHeight();

		/**
		 * Gets the number of triangles drawn.
		 *
		 * \return Returns the number of triangles drawn.
		 */
		static LSE_INLINE uint64_t LSE_CALL						GetTrianglesDrawn();

		/**
		 * Clear the number of triangles drawn.
		 */
		static LSE_INLINE void LSE_CALL							ClearTriangles();

		/**
		 * Sets the current rasterizer state.
		 *
		 * \param _prsState Pointer to a rasterizer-state interface.
		 */
		static LSE_INLINE void LSE_CALL							RSSetState( LSG_RASTERIZER_STATE * _prsState );

		/**
		 * Sets the current blend state.
		 *
		 * \param _pbsState Pointer to a blend-state interface.
		 * \param _pfBlendFactor Array of blend factors, one for each RGBA component.  The blend factors modulate values for the pixel shader, render target, or both.
		 * \param _ui32SampleMask 32-bit sample coverage. The default value is 0xFFFFFFFF.
		 */
		static LSE_INLINE void LSE_CALL							OMSetBlendState( LSG_BLEND_STATE * _pbsState,
			const float * _pfBlendFactor = NULL,
			uint32_t _ui32SampleMask = 0xFFFFFFFF );

		/**
		 * Sets the current blend state.
		 *
		 * \param _pdssState Pointer to a depth-stencil state interface.
		 * \param _ui32StencilRef Reference value to perform against when doing a depth-stencil test.
		 */
		static LSE_INLINE void LSE_CALL							OMSetDepthStencilState( LSG_DEPTH_STENCIL_STATE * _pdssState,
			uint32_t _ui32StencilRef = 0 );


	protected :
		// == Members.
		/** The number of triangles drawn. */
		static uint64_t											m_ui64TrianglesDrawn;


		// == Functions.
		/**
		 * Gets a reference to the current render state for reading and writing.
		 *
		 * \return Returns a reference to the current render state for reading and writing.
		 */
		static LSE_INLINE LSG_RENDER_STATE & LSE_FCALL			RenderState();


	private :
		friend class											CFramebuffer;
		friend class											CTexture2d;
		friend class											CSampler;
		friend class											CDirectX11Texture2d;
		friend class											CDirectX9Texture2d;
		friend class											COpenGlTexture2d;
		friend class											COpenGlEsTexture2d;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	* Sets the current framebuffer (or NULL to activate the backbuffers).
	*
	* \param _pfFramebuffer The framebuffer to set, or NULL to activate the backbuffers.
	*/
	LSE_INLINE void LSE_CALL CGfx::SetFramebuffer( CFramebuffer * _pfFramebuffer ) {
		m_rsCurRenderState.pfFramebuffer = _pfFramebuffer;
	}

	/**
	* Gets the current framebuffer.
	*
	* \return Returns the currently set framebuffer, which may be NULL.
	*/
	LSE_INLINE CFramebuffer * LSE_CALL CGfx::GetFramebuffer() {
		return m_rsCurRenderState.pfFramebuffer;
	}

	/**
	 * Gets the system metrics.
	 *
	 * \return Returns the system metrics.
	 */
	LSE_INLINE const CGfxBase::LSG_METRICS LSE_CALL CGfx::GetMetrics() {
		return CGfxBase::m_mMetrics;
	}

	/**
	 * Gets the screen width in pixels.
	 *
	 * \return Returns the screen width in pixels.
	 */
	LSE_INLINE uint32_t LSE_CALL CGfx::ScreenWidth() {
		return m_ui32ScreenWidth;
	}

	/**
	 * Gets the screen height in pixels.
	 *
	 * \return Returns the screen height in pixels.
	 */
	LSE_INLINE uint32_t LSE_CALL CGfx::ScreenHeight() {
		return m_ui32ScreenHeight;
	}

	/**
	 * Gets the number of triangles drawn.
	 *
	 * \return Returns the number of triangles drawn.
	 */
	LSE_INLINE uint64_t LSE_CALL CGfx::GetTrianglesDrawn() {
		return m_ui64TrianglesDrawn;
	}

	/**
	 * Clear the number of triangles drawn.
	 */
	LSE_INLINE void LSE_CALL CGfx::ClearTriangles() {
		m_ui64TrianglesDrawn = 0ULL;
	}

	/**
	 * Sets the current rasterizer state.
	 *
	 * \param _prsState Pointer to a rasterizer-state interface.
	 */
	LSE_INLINE void LSE_CALL CGfx::RSSetState( LSG_RASTERIZER_STATE * _prsState ) {
		m_rsCurRenderState.prsRasterState = _prsState;
	}

	/**
	 * Sets the current blend state.
	 *
	 * \param _pbsState Pointer to a blend-state interface.
	 * \param _pfBlendFactor Array of blend factors, one for each RGBA component.  The blend factors modulate values for the pixel shader, render target, or both.
	 * \param _ui32SampleMask 32-bit sample coverage. The default value is 0xFFFFFFFF.
	 */
	LSE_INLINE void LSE_CALL CGfx::OMSetBlendState( LSG_BLEND_STATE * _pbsState,
		const float * _pfBlendFactor,
		uint32_t _ui32SampleMask ) {
		if ( _pfBlendFactor ) {
			m_rsCurRenderState.fBlendFactors[0] = _pfBlendFactor[0];
			m_rsCurRenderState.fBlendFactors[1] = _pfBlendFactor[1];
			m_rsCurRenderState.fBlendFactors[2] = _pfBlendFactor[2];
			m_rsCurRenderState.fBlendFactors[3] = _pfBlendFactor[3];
		}
		m_rsCurRenderState.ui32SampleMask = _ui32SampleMask;
		m_rsCurRenderState.pbsBlendState = _pbsState;
	}

	/**
	 * Sets the current blend state.
	 *
	 * \param _pdssState Pointer to a depth-stencil state interface.
	 * \param _ui32StencilRef Reference value to perform against when doing a depth-stencil test.
	 */
	LSE_INLINE void LSE_CALL CGfx::OMSetDepthStencilState( LSG_DEPTH_STENCIL_STATE * _pdssState,
		uint32_t _ui32StencilRef ) {
		m_rsCurRenderState.ui32StencilRef = _ui32StencilRef;
		m_rsCurRenderState.pdssDepthStencilState = _pdssState;
	}

	/**
	 * Gets a reference to the current render state for reading and writing.
	 *
	 * \return Returns a reference to the current render state for reading and writing.
	 */
	LSE_INLINE LSG_RENDER_STATE & LSE_FCALL CGfx::RenderState() {
		return m_rsCurRenderState;
	}

}	// namespace lsg

#endif	// __LSG_GFX_H__
