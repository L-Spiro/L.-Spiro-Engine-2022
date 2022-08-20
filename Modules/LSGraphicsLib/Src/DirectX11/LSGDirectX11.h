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
 * Description: Interface with the DirectX 11 API.
 */


#ifndef __LSG_DIRECTX11_H__
#define __LSG_DIRECTX11_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../Gfx/LSGGfxBase.h"
#include "Map/LSTLLinearMap.h"

#include <d3d11.h>
#include <d3dx11.h>

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Enumerations.
	/** The blend types we support.  These are for blending to the backbuffer only. */
	enum LSG_BLEND_MODE {
		LSG_BM_ZERO									= D3D11_BLEND_ZERO,								/**< Blend factor is (0, 0, 0, 0). */
		LSG_BM_ONE									= D3D11_BLEND_ONE,								/**< Blend factor is (1, 1, 1, 1). */
		LSG_BM_SRC_COLOR							= D3D11_BLEND_SRC_COLOR,						/**< Blend factor is (Rs, Gs, Bs, As). */
		LSG_BM_INV_SRC_COLOR						= D3D11_BLEND_INV_SRC_COLOR,					/**< Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		LSG_BM_DST_COLOR							= D3D11_BLEND_DEST_COLOR,						/**< Blend factor is (Rd, Gd, Bd, Ad). */
		LSG_BM_INV_DST_COLOR						= D3D11_BLEND_INV_DEST_COLOR,					/**< Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		LSG_BM_SRC_ALPHA							= D3D11_BLEND_SRC_ALPHA,						/**< Blend factor is (As, As, As, As). */
		LSG_BM_INV_SRC_ALPHA						= D3D11_BLEND_INV_SRC_ALPHA,					/**< Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		LSG_BM_DST_ALPHA							= D3D11_BLEND_DEST_ALPHA,						/**< Blend factor is (Ad Ad Ad Ad). */
		LSG_BM_INV_DEST_ALPHA						= D3D11_BLEND_INV_DEST_ALPHA,					/**< Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		LSG_BM_SRC_ALPHA_SAT						= D3D11_BLEND_SRC_ALPHA_SAT,					/**< Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */

		LSG_BM_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** The blend operations we support. */
	enum LSG_BLEND_OP {
		LSG_BO_ADD									= D3D11_BLEND_OP_ADD,							/**< Add source 1 and source 2. */
		LSG_BO_SUBTRACT								= D3D11_BLEND_OP_SUBTRACT,						/**< Subtract source 1 and source 2. */
		LSG_BO_REV_SUBTRACT							= D3D11_BLEND_OP_REV_SUBTRACT,					/**< Subtract source 2 from source 1. */
		LSG_BO_MIN									= D3D11_BLEND_OP_MIN,							/**< Find the minimum of source 1 and source 2. */
		LSG_BO_MAX									= D3D11_BLEND_OP_MAX,							/**< Find the maximum of source 1 and source 2. */

		LSG_BO_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** Buffers. */
	enum LSG_BUFFER {
		LSG_B_COLOR_BUFFER							= (1 << 16),									/**< Clear a render target. */
		LSG_B_STENCIL_BUFFER						= D3D11_CLEAR_STENCIL,							/**< Clear the stencil buffer. */
		LSG_B_DEPTH_BUFFER							= D3D11_CLEAR_DEPTH,							/**< Clear the depth buffer. */
		LSG_B_ALL									= LSG_B_COLOR_BUFFER | LSG_B_STENCIL_BUFFER |
			LSG_B_DEPTH_BUFFER,																		/**< Clear all buffers. */
	};

	/** The color-write modes. */
	enum LSG_COLOR_WRITE_ENABLE {
		LSG_CWM_RED									= D3D11_COLOR_WRITE_ENABLE_RED,					/**< Allow data to be stored in the red component. */
		LSG_CWM_GREEN								= D3D11_COLOR_WRITE_ENABLE_GREEN,				/**< Allow data to be stored in the green component. */
		LSG_CWM_BLUE								= D3D11_COLOR_WRITE_ENABLE_BLUE,				/**< Allow data to be stored in the blue component. */
		LSG_CWM_ALPHA								= D3D11_COLOR_WRITE_ENABLE_ALPHA,				/**< Allow data to be stored in the alpha component. */
		LSG_CWM_ALL									= D3D11_COLOR_WRITE_ENABLE_ALL,					/**< Allow data to be stored in all components. */
	};

	/** Comparison functions. */
	enum LSG_COMPARISON_FUNC {
		LSG_CF_NEVER								= D3D11_COMPARISON_NEVER,						/**< Never pass the comparison. */
		LSG_CF_ALWAYS								= D3D11_COMPARISON_ALWAYS,						/**< Always pass the comparison. */
		LSG_CF_LESS									= D3D11_COMPARISON_LESS,						/**< If the source data is less than the destination data, the comparison passes. */
		LSG_CF_EQUAL								= D3D11_COMPARISON_EQUAL,						/**< If the source data is equal to the destination data, the comparison passes. */
		LSG_CF_NOTEQUAL								= D3D11_COMPARISON_NOT_EQUAL,					/**< If the source data is not equal to the destination data, the comparison passes. */
		LSG_CF_LESSEQUAL							= D3D11_COMPARISON_LESS_EQUAL,					/**< If the source data is less than or equal to the destination data, the comparison passes. */
		LSG_CF_GREATER								= D3D11_COMPARISON_GREATER,						/**< If the source data is greater than the destination data, the comparison passes. */
		LSG_CF_GREATEREQUAL							= D3D11_COMPARISON_GREATER_EQUAL,				/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	};

	/** Culling modes. */
	enum LSG_CULL_MODE {
		LSG_CM_NONE									= D3D11_CULL_NONE,								/**< No culling. */
		LSG_CM_FRONT								= D3D11_CULL_FRONT,								/**< Cull front faces. */
		LSG_CM_BACK									= D3D11_CULL_BACK,								/**< Cull back faces. */
	};

	/** Depth write masks. */
	enum LSG_DEPTH_WRITE_MASK {
		LSG_DRM_ZERO								= D3D11_DEPTH_WRITE_MASK_ZERO,					/**< Turn off writes to the depth-stencil buffer. */
		LSG_DRM_ALL									= D3D11_DEPTH_WRITE_MASK_ALL,					/**< Turn on writes to the depth-stencil buffer. */
	};

	/** Fill modes. */
	enum LSG_FILL_MODE {
		LSG_FM_SOLID								= D3D11_FILL_SOLID,								/**< Solid filling mode. */
		LSG_FM_WIRE									= D3D11_FILL_WIREFRAME,							/**< Wire-frame filling mode. */
	};

	/** Filters. */
	enum LSG_FILTER {
		LSG_F_MIN_MAG_MIP_POINT						= D3D11_FILTER_MIN_MAG_MIP_POINT,				/**< Use point sampling for minification, magnification, and mip-level sampling. */
		LSG_F_MIN_MAG_POINT_MIP_LINEAR				= D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,		/**< Use point sampling for minification and magnification; use linear interpolation for mip-level sampling. */
		LSG_F_MIN_POINT_MAG_LINEAR_MIP_POINT		= D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,	/**< Use point sampling for minification; use linear interpolation for magnification; use point sampling for mip-level sampling. */
		LSG_F_MIN_POINT_MAG_MIP_LINEAR				= D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,		/**< Use point sampling for minification; use linear interpolation for magnification and mip-level sampling. */
		LSG_F_MIN_LINEAR_MAG_MIP_POINT				= D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,		/**< Use linear interpolation for minification; use point sampling for magnification and mip-level sampling. */
		LSG_F_MIN_LINEAR_MAG_POINT_MIP_LINEAR		= D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,	/**< Use linear interpolation for minification; use point sampling for magnification; use linear interpolation for mip-level sampling. */
		LSG_F_MIN_MAG_LINEAR_MIP_POINT				= D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,		/**< Use linear interpolation for minification and magnification; use point sampling for mip-level sampling. */
		LSG_F_MIN_MAG_MIP_LINEAR					= D3D11_FILTER_MIN_MAG_MIP_LINEAR,				/**< Use linear interpolation for minification, magnification, and mip-level sampling. */
		LSG_F_ANISOTROPIC							= D3D11_FILTER_ANISOTROPIC,						/**< Use anisotropic interpolation for minification, magnification, and mip-level sampling. */
	};

	/** Stencil operations. */
	enum LSG_STENCIL_OP {
		LSG_SO_KEEP									= D3D11_STENCIL_OP_KEEP,						/**< Keep the existing stencil data. */
		LSG_SO_ZERO									= D3D11_STENCIL_OP_ZERO,						/**< Set the stencil data to 0. */
		LSG_SO_REPLACE								= D3D11_STENCIL_OP_REPLACE,						/**< Set the stencil data to the reference value. */
		LSG_SO_INC_SAT								= D3D11_STENCIL_OP_INCR_SAT,					/**< Increment the stencil value by 1, and clamp the result. */
		LSG_SO_DEC_SAT								= D3D11_STENCIL_OP_DECR_SAT,					/**< Decrement the stencil value by 1, and clamp the result. */
		LSG_SO_INVERT								= D3D11_STENCIL_OP_INVERT,						/**< Invert the stencil data. */
		LSG_SO_INC									= D3D11_STENCIL_OP_INCR,						/**< Increment the stencil value by 1, and wrap the result if necessary. */
		LSG_SO_DEC									= D3D11_STENCIL_OP_DECR,						/**< Decrement the stencil value by 1, and wrap the result if necessary. */
	};

	/** Texture addressing modes. */
	enum LSG_TEXTURE_ADDRESS_MODE {
		LSG_TAM_WRAP								= D3D11_TEXTURE_ADDRESS_WRAP,					/**< Tile the texture at every (u,v) integer junction. */
		LSG_TAM_MIRROR								= D3D11_TEXTURE_ADDRESS_MIRROR,					/**< Flip the texture at every (u,v) integer junction. */
		LSG_TAM_CLAMP								= D3D11_TEXTURE_ADDRESS_CLAMP,					/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		LSG_TAM_BORDER								= D3D11_TEXTURE_ADDRESS_BORDER,					/**< Texture coordinates outside the range [0.0, 1.0] are set to the border color. */
		LSG_TAM_MIRROR_ONCE							= D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,			/**< Similar to LSG_TAM_MIRROR and LSG_TAM_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */
	};

	/** Compile-time metrics.  For run-time, use the metrics structure returned by the graphics device. */
	enum LSG_METRICS {
		LSG_MAX_TEXTURE_UNITS						= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT,	/**< Total texture units bindable to a shader at a time. */
		LSG_MAX_SHADER_SAMPLERS						= D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT,		/**< Total number of samplers that can be set on a shader at once. */
		LSG_MAX_RENDER_TARGET_COUNT					= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,		/**< Total simultaneous render-target count. */
		LSG_MAX_VIEWPORTS_SCISSORS					= D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1,	/**< Maximum number of viewports and scissors. */
		LSG_VIEWPORT_MIN							= D3D11_VIEWPORT_BOUNDS_MIN,					/**< Minimum viewport bounds. */
		LSG_VIEWPORT_MAX							= D3D11_VIEWPORT_BOUNDS_MAX,					/**< Maximum viewport bounds. */
		LSG_MAX_VERTEX_ELEMENT_COUNT				= D3D11_STANDARD_VERTEX_ELEMENT_COUNT,			/**< Maximum number of inputs to a vertex shader. */
		LSG_MAX_VERTEX_STREAMS						= 16,											/**< Maximum number of simultaneous vertex buffers bound. */
		LSG_MAX_VERTEX_SEMANTIC						= 16,											/**< Maximum number of vertex buffer semantic indices. */
		
		LSG_FORCE_DWORD								= 0x7FFFFFFF
	};

	// Floating-point metrics.
#define LSG_MIN_DEPTH								D3D11_MIN_DEPTH									/**< Minimum depth value. */
#define LSG_MAX_DEPTH								D3D11_MAX_DEPTH									/**< Maximum depth value. */

	// Default values.
#define LSG_DEFAULT_STENCIL_READ_MASK				D3D11_DEFAULT_STENCIL_READ_MASK					/**< Default stencil read mask. */
#define LSG_DEFAULT_STENCIL_WRITE_MASK				D3D11_DEFAULT_STENCIL_WRITE_MASK				/**< Default stencil write mask. */


	// == Types.
	/** The shader manager type. */
	class											CFramebuffer;
	typedef class CDirectX11ShaderManager			CShaderManager;
	class											CShader;
	class											CVertexBuffer;

	/** Sampler description. */
	typedef D3D11_SAMPLER_DESC						LSG_SAMPLER_DESC;

	/** Rasterizer description. */
	typedef D3D11_RASTERIZER_DESC					LSG_RASTERIZER_DESC;

	/** Render-target blend description. */
	typedef D3D11_RENDER_TARGET_BLEND_DESC			LSG_RENDER_TARGET_BLEND_DESC;

	/** Blend description. */
	typedef D3D11_BLEND_DESC						LSG_BLEND_DESC;

	/** Depth/stencil operation description. */
	typedef D3D11_DEPTH_STENCILOP_DESC				LSG_DEPTH_STENCILOP_DESC;

	/** Depth/stencil description. */
	typedef D3D11_DEPTH_STENCIL_DESC				LSG_DEPTH_STENCIL_DESC;

	/** The 1D texture type. */
	typedef ID3D11Texture1D							LSG_TEXTURE_1D;

	/** The 2D texture type. */
	typedef ID3D11Texture2D							LSG_TEXTURE_2D;

	/** The 3D texture type. */
	typedef ID3D11Texture3D							LSG_TEXTURE_3D;

	/** The sampler type. */
	typedef ID3D11SamplerState						LSG_SAMPLER_STATE;

	/** The shader resource view type. */
	typedef ID3D11ShaderResourceView				LSG_SHADER_RESOURCE;

	/** The render target type. */
	typedef ID3D11RenderTargetView					LSG_RENDER_TARGET;

	/** The depth/stencil type. */
	typedef ID3D11DepthStencilView					LSG_DEPTH_STENCIL;

	/** The rasterizer state type. */
	typedef ID3D11RasterizerState					LSG_RASTERIZER_STATE;

	/** The blend state type. */
	typedef ID3D11BlendState						LSG_BLEND_STATE;

	/** The depth/stencil state. */
	typedef ID3D11DepthStencilState					LSG_DEPTH_STENCIL_STATE;

	/** The viewport type. */
	typedef D3D11_VIEWPORT							LSG_VIEWPORT;

	/** The scissor rectangle type. */
	typedef D3D11_RECT								LSG_RECT;

	/** The input layout type. */
	typedef ID3D11InputLayout						LSG_INPUT_LAYOUT;

	/** The vertex buffer type. */
	typedef struct LSG_VERTEX_BUFFER {
		const CVertexBuffer *						pvbVertBuffer;									/**< The vertex buffer pointer. */
		uint32_t									ui32Offset;										/**< Offset of the vertex buffer. */
	} * LPLSG_VERTEX_BUFFER, * const LPCLSG_VERTEX_BUFFER;

	// Create the render-state structure.
#include "../Gfx/LSGGfxState.h"

	/**
	 * Class CDirectX11
	 * \brief Interface with the DirectX 11 API.
	 *
	 * Description: Interface with the DirectX 11 API.
	 */
	class CDirectX11 : public CGfxBase {
		friend class								CDirectX11FramebufferBase;
		friend class								CDirectX11Texture2d;
	public :
		// == Types.
		/** Data needed to initialize the device. */
		typedef struct LSG_DIRECTX11_INIT {
			/** Handle to the window. */
			HWND									hWnd;

			/** Windowed? */
			LSBOOL									bWindowed;
		} * LPLSG_DIRECTX11_INIT, * const LPCLSG_DIRECTX11_INIT;

		/** A 4-element floating-point vector. */
		typedef struct LSG_VECTOR4 {
			/** X. */
			FLOAT									x;

			/** Y. */
			FLOAT									y;

			/** Z. */
			FLOAT									z;

			/** W. */
			FLOAT									w;
		} * LPLSG_VECTOR4, * const LPCLSG_VECTOR4;

		/** A 4-by-4 matrix. */
		typedef struct LSG_MATRIX {
			/** Row 0. */
			FLOAT									_11, _12, _13, _14;

			/** Row 1. */
			FLOAT									_21, _22, _23, _24;

			/** Row 2. */
			FLOAT									_31, _32, _33, _34;

			/** Row 3. */
			FLOAT									_41, _42, _43, _44;
		} * LPLSG_MATRIX, * const LPCLSG_MATRIX;


		// == Functions.
		/**
		 * Initialize DirectX 11.
		 *
		 * \param _lpdiInit Data required to initialize the device.
		 * \return Returns false if there are any problem during initialization.
		 */
		static LSBOOL LSE_CALL						InitDirectX11( LPCLSG_DIRECTX11_INIT _lpdiInit );

		/**
		 * Secondary initialization of DirectX 11, after a device has been created.
		 */
		static void LSE_CALL						PostInitDirectX11();

		/**
		 * Destroy DirectX 11 one time at engine shutdown.
		 */
		static void LSE_CALL						DestroyDirectX11();

		/**
		 * Sets whether we are toggling full-screen now.
		 *
		 * \param _bVal Whether full-screen is being toggled or not.
		 */
		static LSE_INLINE void LSE_CALL			TogglingFullScreen( LSBOOL _bVal );

		/**
		 * Sets the screen settings.  Used when toggling fullscreen mode.
		 *
		 * \param _bWindowed Windowed mode?
		 * \param _hWnd New window handle.
		 */
		static LSE_INLINE void LSE_CALL			SetWindowParms( LSBOOL _bWindowed, HWND _hWnd );

		/**
		 * Sets the screen dimensions.
		 *
		 * \param _ui32Width Screen width, in pixels.
		 * \param _ui32Height Screen height, in pixels.
		 */
		static void LSE_CALL						SetScreenSize( uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Creates a perspective projection matrix based on a field of view.
		 *
		 * \param _mIn The output matrix.
		 * \param _fFov Field of view in the y direction, in radians.
		 * \param _fAspect Aspect ratio, defined as view space width divided by height.
		 * \param _fNear Z-value of the near view-plane.
		 * \param _fFar Z-value of the far view-plane.
		 * \return Returns the input matrix so that this function can be used as a parameter to another function.
		 */
		static LSE_INLINE CMatrix4x4 & LSE_CALL		PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _fFov, LSREAL _fAspect, LSREAL _fNear, LSREAL _fFar );

		/**
		 * Creates an orthogonal projection matrix.
		 *
		 * \param _mIn The output matrix.
		 * \param _fLeft Minimum x-value of the view volume.
		 * \param _fRight Maximum x-value of the view volume.
		 * \param _fBottom Minimum y-value of the view volume.
		 * \param _fTop Maximum y-value of the view volume.
		 * \param _fNear Minimum z-value of the view volume.
		 * \param _fFar Maximum z-value of the view volume.
		 * \return Returns the input matrix so that this function can be used as a parameter to another function.
		 */
		static LSE_INLINE CMatrix4x4 & LSE_CALL		OrthoOffCenter( CMatrix4x4 &_mIn,
			LSREAL _fLeft, LSREAL _fRight,
			LSREAL _fBottom, LSREAL _fTop,
			LSREAL _fNear, LSREAL _fFar );

		/**
		 * Extracts the frustum planes from the given projection matrix.
		 *
		 * \param _mMatrix Matrix from which to extract the frustum planes.
		 * \param _fFrustum The frustum to which the planes are extracted.
		 * \param _bNormalize If true, the planes are normalized.
		 * \return Returns the reference to _fFrustum.
		 */
		static LSE_INLINE CFrustum & LSE_CALL		ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize = true );

		/**
		 * Sets default states for between-object rendering.  Does not change viewports, render targets, shader
		 *	resources, etc.
		 */
		static LSE_INLINE void LSE_FCALL			SetDefaultObjectRenderStates();

		/**
		 * Sets all default render states.
		 */
		static LSE_INLINE void LSE_FCALL			SetAllDefaultRenderStates();

		/**
		 * Sets and applies all default render states.
		 */
		static void LSE_CALL						ApplyDefaultStates();

		/**
		 * Applies all current render states.
		 *
		 * \param _bForce If true, all states are applied, even if they are redundant.
		 */
		static void LSE_CALL						ApplyRenderStates( LSBOOL _bForce = false );

		/**
		 * Gets the back buffer surface pointer.
		 *
		 * \return Returns the back buffer surface pointer.
		 */
		static LSE_INLINE ID3D11RenderTargetView * LSE_CALL
													GetBackBufferSurface();

		/**
		 * Gets the depth/stencil surface pointer.
		 *
		 * \return Returns the depth/stencil surface pointer.
		 */
		static LSE_INLINE ID3D11DepthStencilView * LSE_CALL
													GetDepthStencilSurface();

		/**
		 * Activates the default backbuffer and depth/stencil buffers.
	 	 */
		static LSE_INLINE void LSE_CALL			ActivateBackBuffers();

		/**
		 * Applies the currently set framebuffer. The render targets in the framebuffer are not set until this
		 *	is called.
		 *
		 * \param _bForce If true, all render targets are applied, even if they are redundant.
		 * \return Returns true if any buffers were activated.
		 */
		static LSBOOL LSE_CALL						ApplyFramebuffer( LSBOOL _bForce = false );

		/**
		 * Gets the DirectX 11 device.
		 *
		 * \return Returns the DirectX 11 device.
		 */
		static LSE_INLINE ID3D11Device *			GetDirectX11Device();

		/**
		 * Gets the DirectX 11 context pointer.
		 *
		 * \return Returns the DirectX 11 context pointer.
		 */
		static LSE_INLINE ID3D11DeviceContext * LSE_CALL
													GetDirectX11Context();

		/**
		 * Gets a pointer to the shader manager.
		 *
		 * \return Returns a pointer to the shader manager used by this class.
		 */
		static LSE_INLINE CShaderManager * LSE_FCALL
													GetShaderManager();

		/**
		 * Sets the current shader.  A shader must be set in order to render.
		 *
		 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
		 *	without a shader is not permitted.
		 */
		static void LSE_FCALL						SetShader( CShader * _psShader );

		/**
		 * Gets the current shader.  If no shader is set, NULL is returned.
		 *
		 * \return Returns the current shader or NULL.
		 */
		static CShader * LSE_FCALL					GetShader();

		/**
		 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
		 *
		 * \return Returns true if the rendering device is ready.
		 */
		static LSE_INLINE LSBOOL LSE_FCALL			BeginRender();

		/**
		 * End rendering.  Must be called once per frame only, after all rendering is done.
		 */
		static LSE_INLINE void LSE_FCALL			EndRender();

		/**
		 * Presents the backbuffer to the screen.
		 */
		static LSE_INLINE void LSE_FCALL			Present();

		/**
		 * Clears the given color render target.
		 *
		 * \param _prtRenderTarget The render target to clear.
		 */
		static LSE_INLINE void LSE_FCALL			Clear( LSG_RENDER_TARGET * _prtRenderTarget );

		/**
		 * Clears the given depth/stencil buffer.
		 *
		 * \param _pdsDepthStencil The depth/stencil buffer to clear.
		 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
		 */
		static LSE_INLINE void LSE_FCALL			Clear( LSG_DEPTH_STENCIL * _pdsDepthStencil, uint32_t _ui32Mask );

		/**
		 * Clears the active buffer(s) of the given type(s).
		 *
		 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
		 */
		static LSE_INLINE void LSE_FCALL			Clear( uint32_t _ui32Mask );

		/**
		 * Clears all active buffers.
		 */
		static LSE_INLINE void LSE_FCALL			Clear();

		/**
		 * Sets the clear color.
		 *
		 * \param _fR Red value.
		 * \param _fG Green value.
		 * \param _fB Blue value.
		 * \param _fA Alpha value.
		 */
		static LSE_INLINE void LSE_CALL			SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA );

		/**
		 * Sets the depth clear value.
		 *
		 * \param _fValue The new depth clear value.
		 */
		static LSE_INLINE void LSE_FCALL			SetClearDepth( LSREAL _fValue );

		/**
		 * Sets the stencil clear value.
		 *
		 * \param _ui8Val The new stencil clear value.
		 */
		static LSE_INLINE void LSE_FCALL			SetClearStencil( uint8_t _ui8Val );

		/**
		 * Sets the viewport dimensions for the given render target.
		 *
		 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fWidth Width of the viewport.
		 * \param _fHeight Height of the viewport.
		 * \param _ui32Target Index of the viewport to set.
		 */
		static LSE_INLINE void LSE_CALL			SetViewport( LSREAL _fX, LSREAL _fY,
			LSREAL _fWidth, LSREAL _fHeight, uint32_t _ui32Target = 0 );

		/**
		 * Sets the viewport dimensions.
		 *
		 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fWidth Width of the viewport.
		 * \param _fHeight Height of the viewport.
		 * \param _fMin Minimum Z of the viewport.
		 * \param _fMax Maximum Z of the viewport.
		 * \param _ui32Target Index of the viewport to set.
		 */
		static LSE_INLINE void LSE_CALL			SetViewport( LSREAL _fX, LSREAL _fY,
			LSREAL _fWidth, LSREAL _fHeight,
			LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target = 0 );

		/**
		 * Sets the depth range (minimum and maximum Z) of the viewport.
		 *
		 * \param _fMin Minimum Z of the viewport.
		 * \param _fMax Maximum Z of the viewport.
		 * \param _ui32Target Index of the depth range to set.
		 */
		static LSE_INLINE void LSE_FCALL			SetDepthRange( LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target = 0 );

		/**
		 * Sets the total number of viewports.
		 *
		 * \param _ui32Total The total number of viewports rectangles set.
		 */
		static LSE_INLINE void LSE_FCALL			SetTotalViewports( uint32_t _ui32Total );

		/**
		 * Gets the total number of vertex buffers set for read and write.
		 *
		 * \return Returns a reference to the currently set number of vertex buffers.
		 */
		static LSE_INLINE uint32_t & LSE_CALL		TotalActiveVertexBuffers();

		/**
		 * Gets the array of current vertex buffers for read and write.
		 *
		 * \return Returns the array of current vertex buffers for read and write.
		 */
		static LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL
													VertexBuffers();

		/**
		 * Sets the input layout.
		 *
		 * \param _pilLayout The input layout to set.
		 * \return Returns true if the layout was properly set.
		 */
		static LSE_INLINE LSBOOL LSE_CALL			SetInputLayout( const LSG_INPUT_LAYOUT * _pilLayout );

		/**
		 * Safely release an object.
		 *
		 * \param _tObj The object to release.
		 */
		template <typename tnType>
		static LSE_INLINE void LSE_CALL			SafeRelease( tnType &_tObj );

		/**
		 * Creates a sampler-state object that encapsulates sampling information for a texture.
		 *
		 * \param _sdSamplerDesc Reference to a sampler state description.
		 * \param _ppssSamplerState Address of a pointer to the sampler state object created.
		 * \return Returns an error code indicating success or failure.
		 */
		static LSE_INLINE HRESULT LSE_CALL			CreateSamplerState( const LSG_SAMPLER_DESC &_sdSamplerDesc, LSG_SAMPLER_STATE ** _ppssSamplerState );

		/**
		 * Creates a rasterizer state object that tells the rasterizer stage how to behave.
		 *
		 * \param _rdRasterizerDesc Reference to a rasterizer state description.
		 * \param _pprsRasterizerState Address of a pointer to the rasterizer state object created.
		 * \return Returns an error code indicating success or failure.
		 */
		static LSE_INLINE HRESULT LSE_CALL			CreateRasterizerState( const LSG_RASTERIZER_DESC &_rdRasterizerDesc, LSG_RASTERIZER_STATE ** _pprsRasterizerState );

		/**
		 * Creates a blend-state object that encapsules blend state for the output-merger stage.
		 *
		 * \param _bdBlendDesc Reference to a blend state description.
		 * \param _ppbsBlendState Address of a pointer to the blend state object created.
		 * \return Returns an error code indicating success or failure.
		 */
		static LSE_INLINE HRESULT LSE_CALL			CreateBlendState( const LSG_BLEND_DESC &_bdBlendDesc, LSG_BLEND_STATE ** _ppbsBlendState );

		/**
		 * Creates a depth-stencil state object that encapsulates depth-stencil test information for the output-merger stage.
		 *
		 * \param _dsdDepthStencilDesc Reference to a depth/stencil state description.
		 * \param _ppdssDepthStencilState Address of a pointer to the depth/stencil state object created.
		 * \return Returns an error code indicating success or failure.
		 */
		static LSE_INLINE HRESULT LSE_CALL			CreateDepthStencilState( const LSG_DEPTH_STENCIL_DESC &_dsdDepthStencilDesc, LSG_DEPTH_STENCIL_STATE ** _ppdssDepthStencilState );

		/**
		 * Releases a sampler state.
		 *
		 * \param _pssSamplerState The sampler state to release.
		 */
		static LSE_INLINE void LSE_CALL			ReleaseState( LSG_SAMPLER_STATE * &_pssSamplerState );

		/**
		 * Releases a rasterizer state.
		 *
		 * \param _prsRasterizerState The rasterizer state to release.
		 */
		static LSE_INLINE void LSE_CALL			ReleaseState( LSG_RASTERIZER_STATE * &_prsRasterizerState );

		/**
		 * Releases a blend state.
		 *
		 * \param _pbsBlendState The blend state to release.
		 */
		static LSE_INLINE void LSE_CALL			ReleaseState( LSG_BLEND_STATE * &_pbsBlendState );

			/**
		 * Releases a depth/stencil state.
		 *
		 * \param _pdssDepthStencilState The depth/stencil state to release.
		 */
		static LSE_INLINE void LSE_CALL			ReleaseState( LSG_DEPTH_STENCIL_STATE * &_pdssDepthStencilState );

#include "../Gfx/LSGDescFuncDecl.h"


		// == Members.
		/** The current shader signature. */
		static const void *							m_pvShaderSignature;

		/** The current shader size. */
		static SIZE_T								m_stShaderSize;

	protected :
		// == Members.
		/** The DirectX device. */
		static ID3D11Device *						m_pdDevice;

		/** The Direct 3D context. */
		static ID3D11DeviceContext *				m_pdcContext;

		/** Our swap chain. */
		static IDXGISwapChain *						m_pscSwapChain;

		/** View to the back color buffer. */
		static ID3D11RenderTargetView *				m_prtvRenderTargetView;

		/** The depth-stencil texture. */
		static ID3D11Texture2D *					m_pt2dDepthStencilBuffer;

		/** View to the back depth/stencil buffer. */
		static ID3D11DepthStencilView *				m_pdsvDepthStencilView;

		/** Default sampler state. */
		static ID3D11SamplerState *					m_pssDefaultSamplerState;

		/** Default rasterizer state. */
		static ID3D11RasterizerState *				m_prsDefaultRasterizerState;

		/** Default blend state. */
		static ID3D11BlendState *					m_pbsDefaultBlendState;

		/** Default depth/stencil state. */
		static ID3D11DepthStencilState *			m_pdssDefaultDepthStencilState;

		/** The shader manager. */
		static CShaderManager *						m_psmShaderMan;

		/** The driver type. */
		static D3D_DRIVER_TYPE						m_dtDriver;

		/** Initialization parameters. */
		static LSG_DIRECTX11_INIT					m_diInitParms;

		/** The swap-chain creation parameters. */
		static DXGI_SWAP_CHAIN_DESC					m_dscdSwapChainDesc;

		/** The last render state. */
		static LSG_RENDER_STATE						m_rsLastRenderState;

		/** The current render state. */
		static LSG_RENDER_STATE						m_rsCurRenderState;

		/** Are we taggling full-screen? */
		static LSBOOL								m_bToggling;


		// == Functions.
		/**
		 * Release all resources and reset our internal state.
		 */
		static void LSE_CALL						ResetInternal();

		/**
		 * Release only resources related to the back buffers (but not the swap chain).
		 */
		static void LSE_CALL						ReleaseBackBuffers();

		/**
		 * Creates a swap chain and render targets.
		 *
		 * \param _hWnd The window for which to create the swap chain.
		 * \return Returns false if it is unable to acquire enough resources.
		 */
		static LSBOOL LSE_CALL						CreateSwapChainBackBuffers( HWND _hWnd );

		/**
		 * Converts a LSG_SAMPLER_DESC structure to a D3D11_SAMPLER_DESC structure.
		 *
		 * \param _sdIn The input LSG_SAMPLER_DESC structure.
		 * \return Returns a D3D11_SAMPLER_DESC representing the input _sdIn.
		 */
		static D3D11_SAMPLER_DESC LSE_FCALL			ConvertDesc( const LSG_SAMPLER_DESC &_sdIn );

		/**
		 * Converts a LSG_RASTERIZER_DESC structure to a D3D11_RASTERIZER_DESC structure.
		 *
		 * \param _rdIn The input LSG_RASTERIZER_DESC structure.
		 * \return Returns a D3D11_RASTERIZER_DESC representing the input _rdIn.
		 */
		static D3D11_RASTERIZER_DESC LSE_FCALL		ConvertDesc( const LSG_RASTERIZER_DESC &_rdIn );

		/**
		 * Converts a LSG_RENDER_TARGET_BLEND_DESC structure to a D3D11_RENDER_TARGET_BLEND_DESC structure.
		 *
		 * \param _rtbdIn The input LSG_RENDER_TARGET_BLEND_DESC structure.
		 * \return Returns a D3D11_RENDER_TARGET_BLEND_DESC representing the input _rtbdIn.
		 */
		static D3D11_RENDER_TARGET_BLEND_DESC LSE_FCALL
													ConvertDesc( const LSG_RENDER_TARGET_BLEND_DESC &_rtbdIn );

		/**
		 * Converts a LSG_BLEND_DESC structure to a D3D11_BLEND_DESC structure.
		 *
		 * \param _bdIn The input LSG_BLEND_DESC structure.
		 * \return Returns a D3D11_BLEND_DESC representing the input _bdIn.
		 */
		static D3D11_BLEND_DESC LSE_FCALL			ConvertDesc( const LSG_BLEND_DESC &_bdIn );

		/**
		 * Converts a LSG_DEPTH_STENCILOP_DESC structure to a D3D11_DEPTH_STENCILOP_DESC structure.
		 *
		 * \param _dsodIn The input LSG_DEPTH_STENCILOP_DESC structure.
		 * \return Returns a D3D11_DEPTH_STENCILOP_DESC representing the input _dsodIn.
		 */
		static D3D11_DEPTH_STENCILOP_DESC LSE_FCALL	ConvertDesc( const LSG_DEPTH_STENCILOP_DESC &_dsodIn );

		/**
		 * Converts a LSG_DEPTH_STENCIL_DESC structure to a D3D11_DEPTH_STENCIL_DESC structure.
		 *
		 * \param _dsdIn The input LSG_DEPTH_STENCIL_DESC structure.
		 * \return Returns a D3D11_DEPTH_STENCIL_DESC representing the input _dsdIn.
		 */
		static D3D11_DEPTH_STENCIL_DESC LSE_FCALL	ConvertDesc( const LSG_DEPTH_STENCIL_DESC &_dsdIn );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Sets whether we are toggling full-screen now.
	 *
	 * \param _bVal Whether full-screen is being toggled or not.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::TogglingFullScreen( LSBOOL _bVal ) {
		m_bToggling = _bVal;
	}

	/**
	 * Sets the screen settings.  Used when toggling fullscreen mode.
	 *
	 * \param _bWindowed Windowed mode?
	 * \param _hWnd New window handle.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::SetWindowParms( LSBOOL _bWindowed, HWND _hWnd ) {
		// Get the width and height of the window.
		RECT rClient;
		::GetClientRect( _hWnd, &rClient );
		m_dscdSwapChainDesc.BufferDesc.Width	= rClient.right - rClient.left;
		m_dscdSwapChainDesc.BufferDesc.Height	= rClient.bottom - rClient.top;
		m_dscdSwapChainDesc.OutputWindow		= _hWnd;
		m_dscdSwapChainDesc.Windowed			= _bWindowed;

		ReleaseBackBuffers();
		if ( m_pscSwapChain ) {
			m_pscSwapChain->SetFullscreenState( FALSE, NULL );
		}
		SafeRelease( m_pscSwapChain );



		HRESULT hSwapRes = 0;
		IDXGIFactory1 * pfFactory = nullptr; 
		{ 
			IDXGIDevice * pdxgiDevice = nullptr; 
			HRESULT hRes = m_pdDevice->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void **>(&pdxgiDevice) );
			if ( SUCCEEDED( hRes ) ) {
				IDXGIAdapter * paAdapter = nullptr;
				hRes = pdxgiDevice->GetAdapter( &paAdapter );
				if ( SUCCEEDED( hRes ) ) {
					hRes = paAdapter->GetParent( __uuidof( IDXGIFactory1 ), reinterpret_cast<void **>(&pfFactory) );
					SafeRelease( paAdapter );
				}
				SafeRelease( pdxgiDevice );
			}
		}
		if ( pfFactory ) {
			hSwapRes = pfFactory->CreateSwapChain( m_pdDevice, &m_dscdSwapChainDesc,
				&m_pscSwapChain );
			SafeRelease( pfFactory );
		}
		/*IDXGIFactory * pfFactory;
		if ( SUCCEEDED( ::CreateDXGIFactory( __uuidof( IDXGIFactory ), reinterpret_cast<void **>(&pfFactory) ) ) ) {
			pfFactory->CreateSwapChain( m_pdDevice, &m_dscdSwapChainDesc,
				&m_pscSwapChain );
			SafeRelease( pfFactory );
		}*/
		CreateSwapChainBackBuffers( _hWnd );

		Clear();
		Present();
	}

	/**
	 * Creates a perspective projection matrix based on a field of view.
	 *
	 * \param _mIn The output matrix.
	 * \param _fFov Field of view in the y direction, in radians.
	 * \param _fAspect Aspect ratio, defined as view space width divided by height.
	 * \param _fNear Z-value of the near view-plane.
	 * \param _fFar Z-value of the far view-plane.
	 * \return Returns the input matrix so that this function can be used as a parameter to another function.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CDirectX11::PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _fFov, LSREAL _fAspect, LSREAL _fNear, LSREAL _fFar ) {
		// Right-handed.
		// Zero out the ones that need to be zero.
		_mIn._12 = _mIn._13 = _mIn._14 = LSM_ZERO;
		_mIn._21 = _mIn._23 = _mIn._24 = LSM_ZERO;
		_mIn._31 = _mIn._32 = LSM_ZERO;
		_mIn._41 = _mIn._42 = _mIn._44 = LSM_ZERO;
		
		LSREAL fInvDist = LSM_ONE / (_fNear - _fFar);
		
		// Set the remaining ones.
		_mIn._22 = LSM_ONE / CMathLib::Tan( LSM_HALF * _fFov );
		_mIn._11 = _mIn._22 / _fAspect;

		_mIn._33 = _fFar * fInvDist;
		_mIn._34 = -LSM_ONE;

		_mIn._43 = (_fFar * _fNear) * fInvDist;

		return _mIn;
	}

	/**
	 * Creates an orthogonal projection matrix.
	 *
	 * \param _mIn The output matrix.
	 * \param _fLeft Minimum x-value of the view volume.
	 * \param _fRight Maximum x-value of the view volume.
	 * \param _fBottom Minimum y-value of the view volume.
	 * \param _fTop Maximum y-value of the view volume.
	 * \param _fNear Minimum z-value of the view volume.
	 * \param _fFar Maximum z-value of the view volume.
	 * \return Returns the input matrix so that this function can be used as a parameter to another function.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CDirectX11::OrthoOffCenter( CMatrix4x4 &_mIn,
		LSREAL _fLeft, LSREAL _fRight,
		LSREAL _fBottom, LSREAL _fTop,
		LSREAL _fNear, LSREAL _fFar ) {
		// Left-handed.
		// Zero out the ones that need to be zero.
		_mIn._12 = _mIn._13 = _mIn._14 = LSM_ZERO;
		_mIn._21 = _mIn._23 = _mIn._24 = LSM_ZERO;
		_mIn._31 = _mIn._32 = _mIn._34 = LSM_ZERO;

		register LSREAL fWidth = _fRight - _fLeft;
		register LSREAL fHeight = _fTop - _fBottom;
		register LSREAL fDepth = _fFar - _fNear;

		_mIn._11 = LSM_TWO / fWidth;
		_mIn._22 = LSM_TWO / fHeight;
		_mIn._33 = LSM_ONE / fDepth;

		_mIn._41 = (_fRight + _fLeft) / -fWidth;
		_mIn._42 = (_fTop + _fBottom) / -fHeight;
		_mIn._43 = _fNear * -_mIn._33;
		_mIn._44 = LSM_ONE;

		return _mIn;
	}

	/**
	 * Extracts the frustum planes from the given projection matrix.
	 *
	 * \param _mMatrix Matrix from which to extract the frustum planes.
	 * \param _fFrustum The frustum to which the planes are extracted.
	 * \param _bNormalize If true, the planes are normalized.
	 * \return Returns the reference to _fFrustum.
	 */
	LSE_INLINE CFrustum & LSE_CALL CDirectX11::ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize ) {
		// Left clipping plane.
		_fFrustum.m_pLeftPlane.n = _mMatrix.GetRow( 3 ) + _mMatrix.GetRow( 0 );
		_fFrustum.m_pLeftPlane.dist = -(_mMatrix._44 + _mMatrix._41);

		// Right clipping plane.
		_fFrustum.m_pRightPlane.n = _mMatrix.GetRow( 3 ) - _mMatrix.GetRow( 0 );
		_fFrustum.m_pRightPlane.dist = -(_mMatrix._44 - _mMatrix._41);

		// Bottom clipping plane.
		_fFrustum.m_pBottomPlane.n = _mMatrix.GetRow( 3 ) + _mMatrix.GetRow( 1 );
		_fFrustum.m_pBottomPlane.dist = -(_mMatrix._44 + _mMatrix._42);

		// Top clipping plane.
		_fFrustum.m_pTopPlane.n = _mMatrix.GetRow( 3 ) - _mMatrix.GetRow( 1 );
		_fFrustum.m_pTopPlane.dist = -(_mMatrix._44 - _mMatrix._42);

		// Near clipping plane.
		_fFrustum.m_pNearPlane.n = _mMatrix.GetRow( 2 );
		_fFrustum.m_pNearPlane.dist = -_mMatrix._43;

		// Far clipping plane.
		_fFrustum.m_pFarPlane.n = _mMatrix.GetRow( 3 ) - _mMatrix.GetRow( 2 );
		_fFrustum.m_pFarPlane.dist = -(_mMatrix._44 - _mMatrix._43);

		// It is not always nessesary to normalize
		//	the planes of the frustum.  Non-normalized
		//	planes can still be used for basic
		//	intersection tests.
		if ( _bNormalize ) {
			_fFrustum.m_pLeftPlane.Normalize();
			_fFrustum.m_pRightPlane.Normalize();
			_fFrustum.m_pBottomPlane.Normalize();
			_fFrustum.m_pTopPlane.Normalize();
			_fFrustum.m_pNearPlane.Normalize();
			_fFrustum.m_pFarPlane.Normalize();
		}
		return _fFrustum;
	}

	/**
	 * Sets default states for between-object rendering.  Does not change viewports, render targets, shader
	 *	resources, etc.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::SetDefaultObjectRenderStates() {
		m_rsCurRenderState.prsRasterState = m_prsDefaultRasterizerState;
		m_rsCurRenderState.pbsBlendState = m_pbsDefaultBlendState;
		m_rsCurRenderState.pdssDepthStencilState = m_pdssDefaultDepthStencilState;
		m_rsCurRenderState.ui32StencilRef = 0;
		m_rsCurRenderState.fBlendFactors[0] = m_rsCurRenderState.fBlendFactors[1] =
			m_rsCurRenderState.fBlendFactors[2] = m_rsCurRenderState.fBlendFactors[3] = LSGREAL( 1.0 );
		m_rsCurRenderState.ui32SampleMask = 0xFFFFFFFFUL;
		/*for ( uint32_t I = 0; I < LSE_ELEMENTS( m_rsCurRenderState.pvbVertexBuffers ) && m_rsCurRenderState.pvbVertexBuffers[I]; ++I ) {
			m_rsCurRenderState.pvbVertexBuffers[I] = NULL;
		}*/
	}

	/**
	 * Sets all default render states.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::SetAllDefaultRenderStates() {
		// General states.
		SetDefaultObjectRenderStates();

		// Clear values.
		m_rsCurRenderState.fClearColor[0] = m_rsCurRenderState.fClearColor[1] =
			m_rsCurRenderState.fClearColor[2] = m_rsCurRenderState.fClearColor[3] = LSGREAL( 1.0 );
		m_rsCurRenderState.fClearDepth = LSGREAL( 1.0 );
		m_rsCurRenderState.ui8ClearStencil = 0;

		// Viewports and scissors.
		m_rsCurRenderState.ui8TotalViewportsAndScissors = 1;
		for ( uint32_t I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
			m_rsCurRenderState.vViewports[I].TopLeftX = 0.0f;
			m_rsCurRenderState.vViewports[I].TopLeftY = 0.0f;

			m_rsCurRenderState.vViewports[I].Width = static_cast<FLOAT>(m_dscdSwapChainDesc.BufferDesc.Width);
			m_rsCurRenderState.vViewports[I].Height = static_cast<FLOAT>(m_dscdSwapChainDesc.BufferDesc.Height);

			m_rsCurRenderState.vViewports[I].MinDepth = static_cast<FLOAT>(LSG_MIN_DEPTH);
			m_rsCurRenderState.vViewports[I].MaxDepth = static_cast<FLOAT>(LSG_MAX_DEPTH);


			m_rsCurRenderState.rScissors[I].left = 0;
			m_rsCurRenderState.rScissors[I].top = 0;

			m_rsCurRenderState.rScissors[I].right = m_dscdSwapChainDesc.BufferDesc.Width;
			m_rsCurRenderState.rScissors[I].bottom = m_dscdSwapChainDesc.BufferDesc.Height;
		}

		// Render targets.
		m_rsCurRenderState.ui8TotalRenderTargets = 1;
		for ( uint32_t I = 1; I < LSG_MAX_RENDER_TARGET_COUNT; ++I ) {
			m_rsCurRenderState.prtRenderTargets[I] = NULL;
		}
		m_rsCurRenderState.prtRenderTargets[0] = m_prtvRenderTargetView;
		m_rsCurRenderState.pdsDepthStencil = m_pdsvDepthStencilView;

		// Shader resources.
		for ( uint32_t I = LSG_MAX_TEXTURE_UNITS; I--; ) {
			m_rsCurRenderState.psrShaderResources[I] = NULL;
		}
		for ( uint32_t I = LSG_MAX_SHADER_SAMPLERS; I--; ) {
			m_rsCurRenderState.pssSamplers[I] = m_pssDefaultSamplerState;
		}


		// Vertex buffers.
		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			VertexBuffers()[I].pvbVertBuffer = NULL;
			VertexBuffers()[I].ui32Offset = 0;
		}
		m_rsCurRenderState.pilInputLayout = NULL;
		TotalActiveVertexBuffers() = 0;
	}

	/**
	 * Gets the back buffer surface pointer.
	 *
	 * \return Returns the back buffer surface pointer.
	 */
	LSE_INLINE ID3D11RenderTargetView * LSE_CALL CDirectX11::GetBackBufferSurface() {
		return m_prtvRenderTargetView;
	}

	/**
	 * Gets the depth/stencil surface pointer.
	 *
	 * \return Returns the depth/stencil surface pointer.
	 */
	LSE_INLINE ID3D11DepthStencilView * LSE_CALL CDirectX11::GetDepthStencilSurface() {
		return m_pdsvDepthStencilView;
	}

	/**
	 * Activates the default backbuffer and depth/stencil buffers.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::ActivateBackBuffers() {
		CStd::MemSet( m_rsCurRenderState.prtRenderTargets, 0, sizeof( m_rsCurRenderState.prtRenderTargets ) );
		m_rsCurRenderState.prtRenderTargets[0] = GetBackBufferSurface();
		m_rsCurRenderState.pdsDepthStencil = GetDepthStencilSurface();
		SetViewport( LSM_ZERO, LSM_ZERO, LSREAL( m_ui32ScreenWidth ), LSREAL( m_ui32ScreenHeight ) );
	}

	/**
	 * Gets the DirectX 11 device.
	 *
	 * \return Returns the DirectX 11 device.
	 */
	LSE_INLINE ID3D11Device * CDirectX11::GetDirectX11Device() {
		return m_pdDevice;
	}

	/**
	 * Gets the DirectX 11 context pointer.
	 *
	 * \return Returns the DirectX 11 context pointer.
	 */
	LSE_INLINE ID3D11DeviceContext * LSE_CALL CDirectX11::GetDirectX11Context() {
		return m_pdcContext;
	}

	/**
	 * Gets a pointer to the shader manager.
	 *
	 * \return Returns a pointer to the shader manager used by this class.
	 */
	LSE_INLINE CShaderManager * LSE_FCALL CDirectX11::GetShaderManager() {
		return m_psmShaderMan;
	}

	/**
	 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
	 *
	 * \return Returns true if the rendering device is ready.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CDirectX11::BeginRender() {
		return true;
	}

	/**
	 * End rendering.  Must be called once per frame only, after all rendering is done.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::EndRender() {
	}

	/**
	 * Presents the backbuffer to the screen.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::Present() {
		m_pscSwapChain->Present( 0, 0 );
	}

	/**
	 * Clears the given color render target.
	 *
	 * \param _prtRenderTarget The render target to clear.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::Clear( LSG_RENDER_TARGET * _prtRenderTarget ) {
		GetDirectX11Context()->ClearRenderTargetView( _prtRenderTarget, m_rsCurRenderState.fClearColor );
	}

	/**
	 * Clears the given depth/stencil buffer.
	 *
	 * \param _pdsDepthStencil The depth/stencil buffer to clear.
	 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::Clear( LSG_DEPTH_STENCIL * _pdsDepthStencil, uint32_t _ui32Mask ) {
		GetDirectX11Context()->ClearDepthStencilView( _pdsDepthStencil,
			_ui32Mask & (LSG_B_DEPTH_BUFFER | LSG_B_STENCIL_BUFFER),
			m_rsCurRenderState.fClearDepth, m_rsCurRenderState.ui8ClearStencil );
	}

	/**
	 * Clears the active buffer(s) of the given type(s).
	 *
	 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::Clear( uint32_t _ui32Mask ) {
		ApplyFramebuffer();
		if ( _ui32Mask & LSG_B_COLOR_BUFFER ) {
			// Clear all bound render targets.
			for ( uint32_t I = LSG_MAX_RENDER_TARGET_COUNT; I--; ) {
				if ( m_rsCurRenderState.prtRenderTargets[I] ) {
					Clear( m_rsCurRenderState.prtRenderTargets[I] );
				}
			}
		}
		if ( _ui32Mask & (LSG_B_DEPTH_BUFFER | LSG_B_STENCIL_BUFFER) && m_rsCurRenderState.pdsDepthStencil ) {
			Clear( m_rsCurRenderState.pdsDepthStencil, _ui32Mask );
		}
	}

	/**
	 * Clears all active buffers.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::Clear() {
		Clear( LSG_B_ALL );
	}

	/**
	 * Sets the clear color.
	 *
	 * \param _fR Red value.
	 * \param _fG Green value.
	 * \param _fB Blue value.
	 * \param _fA Alpha value.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA ) {
		m_rsCurRenderState.fClearColor[0] = static_cast<LSGREAL>(_fR);
		m_rsCurRenderState.fClearColor[1] = static_cast<LSGREAL>(_fG);
		m_rsCurRenderState.fClearColor[2] = static_cast<LSGREAL>(_fB);
		m_rsCurRenderState.fClearColor[3] = static_cast<LSGREAL>(_fA);
	}

	/**
	 * Sets the depth clear value.
	 *
	 * \param _fValue The new depth clear value.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::SetClearDepth( LSREAL _fValue ) {
		m_rsCurRenderState.fClearDepth = static_cast<LSGREAL>(_fValue);
	}

	/**
	 * Sets the stencil clear value.
	 *
	 * \param _ui8Val The new stencil clear value.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::SetClearStencil( uint8_t _ui8Val ) {
		m_rsCurRenderState.ui8ClearStencil = _ui8Val;
	}

	/**
	 * Sets the viewport dimensions.
	 *
	 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fWidth Width of the viewport.
	 * \param _fHeight Height of the viewport.
	 * \param _ui32Target Index of the viewport to set.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<FLOAT>(_fX) >= static_cast<FLOAT>(LSG_VIEWPORT_MIN) && static_cast<FLOAT>(_fX) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );
		assert( static_cast<FLOAT>(_fY) >= static_cast<FLOAT>(LSG_VIEWPORT_MIN) && static_cast<FLOAT>(_fY) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );

		assert( static_cast<FLOAT>(_fWidth) >= 0.0f && static_cast<FLOAT>(_fX + _fWidth) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );
		assert( static_cast<FLOAT>(_fHeight) >= 0.0f && static_cast<FLOAT>(_fY + _fHeight) <= static_cast<FLOAT>(LSG_VIEWPORT_MAX) );

		m_rsCurRenderState.vViewports[_ui32Target].TopLeftX = static_cast<FLOAT>(_fX);
		m_rsCurRenderState.vViewports[_ui32Target].TopLeftY = static_cast<FLOAT>(_fY);
		m_rsCurRenderState.vViewports[_ui32Target].Width = static_cast<FLOAT>(_fWidth);
		m_rsCurRenderState.vViewports[_ui32Target].Height = static_cast<FLOAT>(_fHeight);
	}

	/**
	 * Sets the viewport dimensions.
	 *
	 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
	 * \param _fWidth Width of the viewport.
	 * \param _fHeight Height of the viewport.
	 * \param _fMin Minimum Z of the viewport.
	 * \param _fMax Maximum Z of the viewport.
	 * \param _ui32Target Index of the viewport to set.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight,
		LSREAL _fMin, LSREAL _fMax,
		uint32_t _ui32Target ) {
		SetViewport( _fX, _fY, _fWidth, _fHeight, _ui32Target );
		SetDepthRange( _fMin, _fMax, _ui32Target );
	}

	/**
	 * Sets the depth range (minimum and maximum Z) of the viewport.
	 *
	 * \param _fMin Minimum Z of the viewport.
	 * \param _fMax Maximum Z of the viewport.
	 * \param _ui32Target Index of the depth range to set.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::SetDepthRange( LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<FLOAT>(_fMin) >= static_cast<FLOAT>(LSG_MIN_DEPTH) && static_cast<FLOAT>(_fMin) <= static_cast<FLOAT>(LSG_MAX_DEPTH) );
		assert( static_cast<FLOAT>(_fMax) >= static_cast<FLOAT>(LSG_MIN_DEPTH) && static_cast<FLOAT>(_fMax) <= static_cast<FLOAT>(LSG_MAX_DEPTH) );

		m_rsCurRenderState.vViewports[_ui32Target].MinDepth = static_cast<FLOAT>(_fMin);
		m_rsCurRenderState.vViewports[_ui32Target].MaxDepth = static_cast<FLOAT>(_fMax);
	}

	/**
	 * Sets the total number of viewports.
	 *
	 * \param _ui32Total The total number of viewports rectangles set.
	 */
	LSE_INLINE void LSE_FCALL CDirectX11::SetTotalViewports( uint32_t _ui32Total ) {
		assert( _ui32Total < LSG_MAX_VIEWPORTS_SCISSORS );

		m_rsCurRenderState.ui8TotalViewportsAndScissors = static_cast<uint8_t>(_ui32Total);
	}

	/**
	 * Gets the total number of vertex buffers set for read and write.
	 *
	 * \return Returns a reference to the currently set number of vertex buffers.
	 */
	LSE_INLINE uint32_t & LSE_CALL CDirectX11::TotalActiveVertexBuffers() {
		return m_rsCurRenderState.ui32TotalVertexBuffers;
	}

	/**
	 * Gets the array of current vertex buffers for read and write.
	 *
	 * \return Returns the array of current vertex buffers for read and write.
	 */
	LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL CDirectX11::VertexBuffers() {
		return m_rsCurRenderState.pvbVertexBuffers;
	}

	/**
	 * Sets the input layout.
	 *
	 * \param _pilLayout The input layout to set.
	 * \return Returns true if the layout was properly set.
	 */
	LSE_INLINE LSBOOL LSE_CALL CDirectX11::SetInputLayout( const LSG_INPUT_LAYOUT * _pilLayout ) {
		if ( m_rsCurRenderState.pilInputLayout == _pilLayout ) { return true; }
		m_rsCurRenderState.pilInputLayout = _pilLayout;
		GetDirectX11Context()->IASetInputLayout( static_cast<ID3D11InputLayout *>(const_cast<LSG_INPUT_LAYOUT *>(m_rsCurRenderState.pilInputLayout)) );
		return true;
	}

	/**
	 * Safely release an object.
	 *
	 * \param _tObj The object to release.
	 */
	template <typename tnType>
	LSE_INLINE void LSE_CALL CDirectX11::SafeRelease( tnType &_tObj ) {
		if ( _tObj ) {
			_tObj->Release();
			_tObj = NULL;
		}
	}

	/**
	 * Creates a sampler-state object that encapsulates sampling information for a texture.
	 *
	 * \param _sdSamplerDesc Reference to a sampler state description.
	 * \param _ppssSamplerState Address of a pointer to the sampler state object created.
	 * \return Returns an error code indicating success or failure.
	 */
	LSE_INLINE HRESULT LSE_CALL CDirectX11::CreateSamplerState( const LSG_SAMPLER_DESC &_sdSamplerDesc, LSG_SAMPLER_STATE ** _ppssSamplerState ) {
		return GetDirectX11Device()->CreateSamplerState( &_sdSamplerDesc, _ppssSamplerState );
	}

	/**
	 * Creates a rasterizer state object that tells the rasterizer stage how to behave.
	 *
	 * \param _rdRasterizerDesc Reference to a rasterizer state description.
	 * \param _pprsRasterizerState Address of a pointer to the rasterizer state object created.
	 * \return Returns an error code indicating success or failure.
	 */
	LSE_INLINE HRESULT LSE_CALL CDirectX11::CreateRasterizerState( const LSG_RASTERIZER_DESC &_rdRasterizerDesc, LSG_RASTERIZER_STATE ** _pprsRasterizerState ) {
		return GetDirectX11Device()->CreateRasterizerState( &_rdRasterizerDesc, _pprsRasterizerState );
	}

	/**
	 * Creates a blend-state object that encapsules blend state for the output-merger stage.
	 *
	 * \param _bdBlendDesc Reference to a blend state description.
	 * \param _ppbsBlendState Address of a pointer to the blend state object created.
	 * \return Returns an error code indicating success or failure.
	 */
	LSE_INLINE HRESULT LSE_CALL CDirectX11::CreateBlendState( const LSG_BLEND_DESC &_bdBlendDesc, LSG_BLEND_STATE ** _ppbsBlendState ) {
		return GetDirectX11Device()->CreateBlendState( &_bdBlendDesc, _ppbsBlendState );
	}

	/**
	 * Creates a depth-stencil state object that encapsulates depth-stencil test information for the output-merger stage.
	 *
	 * \param _dsdDepthStencilDesc Reference to a depth/stencil state description.
	 * \param _ppdssDepthStencilState Address of a pointer to the depth/stencil state object created.
	 * \return Returns an error code indicating success or failure.
	 */
	LSE_INLINE HRESULT LSE_CALL CDirectX11::CreateDepthStencilState( const LSG_DEPTH_STENCIL_DESC &_dsdDepthStencilDesc, LSG_DEPTH_STENCIL_STATE ** _ppdssDepthStencilState ) {
		return GetDirectX11Device()->CreateDepthStencilState( &_dsdDepthStencilDesc, _ppdssDepthStencilState );
	}

	/**
	 * Releases a sampler state.
	 *
	 * \param _pssSamplerState The sampler state to release.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::ReleaseState( LSG_SAMPLER_STATE * &_pssSamplerState ) {
		SafeRelease( _pssSamplerState );
	}

	/**
	 * Releases a rasterizer state.
	 *
	 * \param _prsRasterizerState The rasterizer state to release.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::ReleaseState( LSG_RASTERIZER_STATE * &_prsRasterizerState ) {
		SafeRelease( _prsRasterizerState );
	}

	/**
	 * Releases a blend state.
	 *
	 * \param _pbsBlendState The blend state to release.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::ReleaseState( LSG_BLEND_STATE * &_pbsBlendState ) {
		SafeRelease( _pbsBlendState );
	}

	/**
	 * Releases a depth/stencil state.
	 *
	 * \param _pdssDepthStencilState The depth/stencil state to release.
	 */
	LSE_INLINE void LSE_CALL CDirectX11::ReleaseState( LSG_DEPTH_STENCIL_STATE * &_pdssDepthStencilState ) {
		SafeRelease( _pdssDepthStencilState );
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11_H__
