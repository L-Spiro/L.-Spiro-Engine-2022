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
 * Description: Interface with the DirectX 9 API.
 */


#ifndef __LSG_DIRECTX9_H__
#define __LSG_DIRECTX9_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9

#include "../Gfx/LSGGfxBase.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "Map/LSTLLinearMap.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Enumerations.
	/** The blend types we support.  These are for blending to the backbuffer only. */
	enum LSG_BLEND_MODE {
		LSG_BM_ZERO									= D3DBLEND_ZERO,								/**< Blend factor is (0, 0, 0, 0). */
		LSG_BM_ONE									= D3DBLEND_ONE,									/**< Blend factor is (1, 1, 1, 1). */
		LSG_BM_SRC_COLOR							= D3DBLEND_SRCCOLOR,							/**< Blend factor is (Rs, Gs, Bs, As). */
		LSG_BM_INV_SRC_COLOR						= D3DBLEND_INVSRCCOLOR,							/**< Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		LSG_BM_DST_COLOR							= D3DBLEND_DESTCOLOR,							/**< Blend factor is (Rd, Gd, Bd, Ad). */
		LSG_BM_INV_DST_COLOR						= D3DBLEND_INVDESTCOLOR,						/**< Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		LSG_BM_SRC_ALPHA							= D3DBLEND_SRCALPHA,							/**< Blend factor is (As, As, As, As). */
		LSG_BM_INV_SRC_ALPHA						= D3DBLEND_INVSRCALPHA,							/**< Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		LSG_BM_DST_ALPHA							= D3DBLEND_DESTALPHA,							/**< Blend factor is (Ad Ad Ad Ad). */
		LSG_BM_INV_DEST_ALPHA						= D3DBLEND_INVDESTALPHA,						/**< Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		LSG_BM_SRC_ALPHA_SAT						= D3DBLEND_SRCALPHASAT,							/**< Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */

		LSG_BM_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** The blend operations we support. */
	enum LSG_BLEND_OP {
		LSG_BO_ADD									= D3DBLENDOP_ADD,								/**< Add source 1 and source 2. */
		LSG_BO_SUBTRACT								= D3DBLENDOP_SUBTRACT,							/**< Subtract source 1 and source 2. */
		LSG_BO_REV_SUBTRACT							= D3DBLENDOP_REVSUBTRACT,						/**< Subtract source 2 from source 1. */
		LSG_BO_MIN									= D3DBLENDOP_MIN,								/**< Find the minimum of source 1 and source 2. */
		LSG_BO_MAX									= D3DBLENDOP_MAX,								/**< Find the maximum of source 1 and source 2. */

		LSG_BO_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** Buffers. */
	enum LSG_BUFFER {
		LSG_B_COLOR_BUFFER							= D3DCLEAR_TARGET,								/**< Clear a render target. */
		LSG_B_STENCIL_BUFFER						= D3DCLEAR_STENCIL,								/**< Clear the stencil buffer. */
		LSG_B_DEPTH_BUFFER							= D3DCLEAR_ZBUFFER,								/**< Clear the depth buffer. */
		LSG_B_ALL									= LSG_B_COLOR_BUFFER | LSG_B_STENCIL_BUFFER |
			LSG_B_DEPTH_BUFFER,																		/**< Clear all buffers. */
	};

	/** The color-write modes. */
	enum LSG_COLOR_WRITE_ENABLE {
		LSG_CWM_RED									= D3DCOLORWRITEENABLE_RED,						/**< Allow data to be stored in the red component. */
		LSG_CWM_GREEN								= D3DCOLORWRITEENABLE_GREEN,					/**< Allow data to be stored in the green component. */
		LSG_CWM_BLUE								= D3DCOLORWRITEENABLE_BLUE,						/**< Allow data to be stored in the blue component. */
		LSG_CWM_ALPHA								= D3DCOLORWRITEENABLE_ALPHA,					/**< Allow data to be stored in the alpha component. */
		LSG_CWM_ALL									= LSG_CWM_RED | LSG_CWM_GREEN | LSG_CWM_BLUE |
			LSG_CWM_ALPHA,																			/**< Allow data to be stored in all components. */
	};

	/** Comparison functions. */
	enum LSG_COMPARISON_FUNC {
		LSG_CF_NEVER								= D3DCMP_NEVER,									/**< Never pass the comparison. */
		LSG_CF_ALWAYS								= D3DCMP_ALWAYS,								/**< Always pass the comparison. */
		LSG_CF_LESS									= D3DCMP_LESS,									/**< If the source data is less than the destination data, the comparison passes. */
		LSG_CF_EQUAL								= D3DCMP_EQUAL,									/**< If the source data is equal to the destination data, the comparison passes. */
		LSG_CF_NOTEQUAL								= D3DCMP_NOTEQUAL,								/**< If the source data is not equal to the destination data, the comparison passes. */
		LSG_CF_LESSEQUAL							= D3DCMP_LESSEQUAL,								/**< If the source data is less than or equal to the destination data, the comparison passes. */
		LSG_CF_GREATER								= D3DCMP_GREATER,								/**< If the source data is greater than the destination data, the comparison passes. */
		LSG_CF_GREATEREQUAL							= D3DCMP_GREATEREQUAL,							/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	};

	/** Culling modes. */
	enum LSG_CULL_MODE {
		LSG_CM_NONE									= D3DCULL_NONE,									/**< No culling. */
		LSG_CM_FRONT								= D3DCULL_CCW,									/**< Cull front faces. */
		LSG_CM_BACK									= D3DCULL_CW,									/**< Cull back faces. */
	};

	/** Depth write masks. */
	enum LSG_DEPTH_WRITE_MASK {
		LSG_DRM_ZERO								= 0,											/**< Turn off writes to the depth-stencil buffer. */
		LSG_DRM_ALL									= 1,											/**< Turn on writes to the depth-stencil buffer. */
	};

	/** Fill modes. */
	enum LSG_FILL_MODE {
		LSG_FM_SOLID								= D3DFILL_SOLID,								/**< Solid filling mode. */
		LSG_FM_WIRE									= D3DFILL_WIREFRAME,							/**< Wire-frame filling mode. */
	};

	/** Filters. */
	enum LSG_FILTER {
		/** Use point sampling for minification, magnification, and mip-level sampling. */
		LSG_F_MIN_MAG_MIP_POINT						= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_POINT, LSG_FILTER_POINT, false ),
		/** Use point sampling for minification and magnification; use linear interpolation for mip-level sampling. */
		LSG_F_MIN_MAG_POINT_MIP_LINEAR				= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_POINT, LSG_FILTER_LINEAR, false ),
		/** Use point sampling for minification; use linear interpolation for magnification; use point sampling for mip-level sampling. */
		LSG_F_MIN_POINT_MAG_LINEAR_MIP_POINT		= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_LINEAR, LSG_FILTER_POINT, false ),
		/** Use point sampling for minification; use linear interpolation for magnification and mip-level sampling. */
		LSG_F_MIN_POINT_MAG_MIP_LINEAR				= LSG_ENC_BASIC_FILTER( LSG_FILTER_POINT, LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, false ),
		/** Use linear interpolation for minification; use point sampling for magnification and mip-level sampling. */
		LSG_F_MIN_LINEAR_MAG_MIP_POINT				= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_POINT, LSG_FILTER_POINT, false ),
		/** Use linear interpolation for minification; use point sampling for magnification; use linear interpolation for mip-level sampling. */
		LSG_F_MIN_LINEAR_MAG_POINT_MIP_LINEAR		= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_POINT, LSG_FILTER_LINEAR, false ),
		/** Use linear interpolation for minification and magnification; use point sampling for mip-level sampling. */
		LSG_F_MIN_MAG_LINEAR_MIP_POINT				= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, LSG_FILTER_POINT, false ),
		/** Use linear interpolation for minification, magnification, and mip-level sampling. */
		LSG_F_MIN_MAG_MIP_LINEAR					= LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, LSG_FILTER_LINEAR, false ),
		/** Use anisotropic interpolation for minification, magnification, and mip-level sampling. */
		LSG_F_ANISOTROPIC							= LSG_ENCODE_ANISOTROPIC_FILTER( false ),
	};

	/** Stencil operations. */
	enum LSG_STENCIL_OP {
		LSG_SO_KEEP									= D3DSTENCILOP_KEEP,							/**< Keep the existing stencil data. */
		LSG_SO_ZERO									= D3DSTENCILOP_ZERO,							/**< Set the stencil data to 0. */
		LSG_SO_REPLACE								= D3DSTENCILOP_REPLACE,							/**< Set the stencil data to the reference value. */
		LSG_SO_INC_SAT								= D3DSTENCILOP_INCRSAT,							/**< Increment the stencil value by 1, and clamp the result. */
		LSG_SO_DEC_SAT								= D3DSTENCILOP_DECRSAT,							/**< Decrement the stencil value by 1, and clamp the result. */
		LSG_SO_INVERT								= D3DSTENCILOP_INVERT,							/**< Invert the stencil data. */
		LSG_SO_INC									= D3DSTENCILOP_INCR,							/**< Increment the stencil value by 1, and wrap the result if necessary. */
		LSG_SO_DEC									= D3DSTENCILOP_DECR,							/**< Decrement the stencil value by 1, and wrap the result if necessary. */
	};

	/** Texture addressing modes. */
	enum LSG_TEXTURE_ADDRESS_MODE {
		LSG_TAM_WRAP								= D3DTADDRESS_WRAP,								/**< Tile the texture at every (u,v) integer junction. */
		LSG_TAM_MIRROR								= D3DTADDRESS_MIRROR,							/**< Flip the texture at every (u,v) integer junction. */
		LSG_TAM_CLAMP								= D3DTADDRESS_CLAMP,							/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		LSG_TAM_BORDER								= D3DTADDRESS_BORDER,							/**< Texture coordinates outside the range [0.0, 1.0] are set to the border color. */
		LSG_TAM_MIRROR_ONCE							= D3DTADDRESS_MIRRORONCE,						/**< Similar to LSG_TAM_MIRROR and LSG_TAM_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */
	};

	/** Compile-time metrics.  For run-time, use the metrics structure returned by the graphics device. */
	enum LSG_METRICS {
		LSG_MAX_TEXTURE_UNITS						= 16,											/**< Total texture units bindable to a shader at a time. */
		LSG_MAX_SHADER_SAMPLERS						= 16,											/**< Total number of samplers that can be set on a shader at once. */
		LSG_MAX_RENDER_TARGET_COUNT					= 4,											/**< Total simultaneous render-target count. */
		LSG_MAX_VIEWPORTS_SCISSORS					= 1,											/**< Maximum number of viewports and scissors. */
		LSG_VIEWPORT_MIN							= 0,											/**< Minimum viewport bounds. */
		LSG_VIEWPORT_MAX							= 4096,											/**< Maximum viewport bounds. */
		LSG_MAX_VERTEX_ELEMENT_COUNT				= 16,											/**< Maximum number of inputs to a vertex shader. */
		LSG_MAX_VERTEX_STREAMS						= 16,											/**< Maximum number of simultaneous vertex buffers bound. */
		LSG_MAX_VERTEX_SEMANTIC						= 16,											/**< Maximum number of vertex buffer semantic indices. */
		
		LSG_FORCE_DWORD								= 0x7FFFFFFF
	};

	// Floating-point metrics.
#define LSG_MIN_DEPTH								(0.0f)											/**< Minimum depth value. */
#define LSG_MAX_DEPTH								(1.0f)											/**< Maximum depth value. */

	// Default values.
#define LSG_DEFAULT_STENCIL_READ_MASK				(0xFF)											/**< Default stencil read mask. */
#define LSG_DEFAULT_STENCIL_WRITE_MASK				(0xFF)											/**< Default stencil write mask. */


	// == Types.
	/** The shader manager type. */
	class											CFramebuffer;
	typedef class CDirectX9ShaderManager			CShaderManager;
	class											CShader;
	class											CVertexBuffer;

#include "../Gfx/LSGDescStructs.h"

	/** The 1D texture type. */
	typedef IDirect3DTexture9						LSG_TEXTURE_1D;

	/** The 2D texture type. */
	typedef IDirect3DTexture9						LSG_TEXTURE_2D;

	/** The 3D texture type. */
	typedef IDirect3DTexture9						LSG_TEXTURE_3D;

	/** The sampler type. */
	typedef LSG_SAMPLER_DESC						LSG_SAMPLER_STATE;

	/** The shader resource view type. */
	typedef IDirect3DTexture9						LSG_SHADER_RESOURCE;

	/** The render target type. */
	typedef IDirect3DSurface9						LSG_RENDER_TARGET;

	/** The depth/stencil type. */
	typedef IDirect3DSurface9						LSG_DEPTH_STENCIL;

	/** The rasterizer state type. */
	typedef LSG_RASTERIZER_DESC						LSG_RASTERIZER_STATE;

	/** The blend state type. */
	typedef LSG_BLEND_DESC							LSG_BLEND_STATE;

	/** The depth/stencil state. */
	typedef LSG_DEPTH_STENCIL_DESC					LSG_DEPTH_STENCIL_STATE;

	/** The viewport type. */
	typedef D3DVIEWPORT9							LSG_VIEWPORT;

	/** The scissor rectangle type. */
	typedef RECT									LSG_RECT;

	/** The input layout type. */
	typedef IDirect3DVertexDeclaration9				LSG_INPUT_LAYOUT;

	/** The vertex buffer type. */
	typedef struct LSG_VERTEX_BUFFER {
		const CVertexBuffer *						pvbVertBuffer;									/**< The vertex buffer pointer. */
		uint32_t									ui32Offset;										/**< Offset of the vertex buffer. */
	} * LPLSG_VERTEX_BUFFER, * const LPCLSG_VERTEX_BUFFER;

	// Create the render-state structure.
#include "../Gfx/LSGGfxState.h"

	/**
	 * Class CDirectX9
	 * \brief Interface with the DirectX 9 API.
	 *
	 * Description: Interface with the DirectX 9 API.
	 */
	class CDirectX9 : public CGfxBase {
		friend class								CDirectX9FramebufferBase;
		friend class								CDirectX9Texture2d;
	public :
		// == Types.
		/** Data needed to initialize the device. */
		typedef struct LSG_DIRECTX9_INIT {
			/** Handle to the window. */
			HWND									hWnd;

			/** Windowed? */
			LSBOOL									bWindowed;
		} * LPLSG_DIRECTX9_INIT, * const LPCLSG_DIRECTX9_INIT;


		// == Functions.
		/**
		 * Initialize DirectX 9.
		 *
		 * \param _lpdiInit Data required to initialize the device.
		 * \return Returns false if there are any problem during initialization.
		 */
		static LSBOOL LSE_CALL						InitDirectX9( LPCLSG_DIRECTX9_INIT _lpdiInit );

		/**
		 * Secondary initialization of DirectX 9, after a device has been created.
		 */
		static void LSE_CALL						PostInitDirectX9();

		/**
		 * Destroy DirectX 9 one time at engine shutdown.
		 */
		static void LSE_CALL						DestroyDirectX9();

		/**
		 * Called when a device is lost.
		 */
		static void LSE_CALL						LoseDevice();

		/**
		 * Reset the device.
		 */
		static LSBOOL LSE_CALL						ResetDevice();

		/**
		 * Determines whether or not the device is lost.
		 *
		 * \return Returns true if the device is lost.
		 */
		static LSE_INLINE LSBOOL LSE_CALL			DeviceIsLost();

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
		 * Adds a window as a possible render destination.
		 *
		 * \param _hWnd The handle to the window.
		 * \return Returns true if there were enough resources available to register the window with
		 *	the system.
		 */
		static LSBOOL LSE_CALL						AddWindow( HWND _hWnd );

		/**
		 * Unregisters a window from the system.  Does not close the window.
		 *
		 * \param _hWnd The window to remove from the engine registry.
		 */
		static void LSE_CALL						RemoveWindow( HWND _hWnd );

		/**
		 * Makes the given window, which must have previously been registered via a call to AddWindow(), the
		 *	current window.
		 *
		 * \param _hWnd The window to make current.
		 */
		static void LSE_CALL						MakeWindowCurrent( HWND _hWnd );

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
		static LSE_INLINE IDirect3DSurface9 * LSE_CALL
													GetBackBufferSurface();

		/**
		 * Gets the depth/stencil surface pointer.
		 *
		 * \return Returns the depth/stencil surface pointer.
		 */
		static LSE_INLINE IDirect3DSurface9 * LSE_CALL
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
		 * Gets the DirectX 9 device.
		 *
		 * \return Returns the DirectX 9 device.
		 */
		static LSE_INLINE IDirect3DDevice9 *		GetDirectX9Device();

		/**
		 * Gets the DirectX 9 object.
		 *
		 * \return Returns the DirectX 9 object.
		 */
		static LSE_INLINE IDirect3D9 *				GetDirectX9Object();

		/**
		 * Gets the parameters used to create the DirectX 9 device.
		 *
		 * \return Returns the parameters used to create the DirectX 9 device.
		 */
		static LSE_INLINE const D3DDEVICE_CREATION_PARAMETERS &
													GetCreationParms();

		/**
		 * Gets the device caps.
		 *
		 * \return Returns the device caps for read only.
		 */
		static LSE_INLINE const D3DCAPS9 & LSE_CALL	GetDeviceCaps();

		/**
		 * Gets the presentation parameters.
		 *
		 * \return Returns the presentation parameters used to create the window.
		 */
		static LSE_INLINE const D3DPRESENT_PARAMETERS & LSE_CALL
													GetPresentParms();

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
		 * Present the backbuffer to the screen.
		 */
		static LSE_INLINE void LSE_FCALL			Present();

		/**
		 * Clear the active buffer of the given type(s).
		 *
		 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
		 */
		static LSE_INLINE void LSE_FCALL			Clear( uint32_t _ui32Mask );

		/**
		 * Clear all active buffers.
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

#include "../Gfx/LSGCreateStatesDecl.h"

		/**
		 * Gets the reverse culling for the given input cull mode.
		 *
		 * \param _cmMode The input cull mode whose reverse is to be returned.
		 * \return Returns LSG_CM_FRONT for LSG_CM_BACK, LSG_CM_BACK for LSG_CM_FRONT, and LSG_CM_NONE for LSG_CM_NONE.
		 */
		static LSE_INLINE LSG_CULL_MODE LSE_FCALL	ReverseCull( LSG_CULL_MODE _cmMode );

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

#include "../Gfx/LSGDescFuncDecl.h"

	protected :
		// == Types.
		/** Window information in the case where there are multiple windows to which it is
		 *	possible to render. */
		typedef struct LSG_WINDOW {
			/** The window handle. */
			HWND									hWnd;

			/** The back buffer to the window. */
			IDirect3DSurface9 *						pd3ds9BackBuffer;

			/** The depth/stencil buffer to the window. */
			IDirect3DSurface9 *						pd3ds9DepthStencilBuffer;

			/** The presentation parameters for the window. */
			D3DPRESENT_PARAMETERS					ppPresent;

			/** The swap chain for the window. */
			IDirect3DSwapChain9 *					pd3dsc9SwapChain;
		} * LPLSG_WINDOW, * const LPCLSG_WINDOW;

		/** Render states specific to Direct3D 9. */
		typedef struct LSG_DX9_RENDER_STATE : public LSG_RENDER_STATE {
			/** sRGB sampler state should be set? */
			LSBOOL									bSRgb[LSG_MAX_SHADER_SAMPLERS];
		} * LPLSG_DX9_RENDER_STATE, * const LPCLSG_DX9_RENDER_STATE;

#include "../Gfx/LSGStateRefDecl.h"


		// == Members.
		/** The DirectX 9 device. */
		static IDirect3DDevice9 *					m_pd3dDevice;

		/** The DirectX 9 object. */
		static IDirect3D9 *							m_d3dObject;

		/** The main/default window with which the device was created. */
		static HWND									m_hMainWindow;

		/** The currently active window. */
		static HWND									m_hCurWindow;

		/** The back buffer surface. */
		static IDirect3DSurface9 *					m_pd3s9Back;

		/** The main depth/stencil buffer. */
		static IDirect3DSurface9 *					m_pd3s9DepthStencil;

		/** Default sampler state. */
		static LSG_SAMPLER_STATE *					m_pssDefaultSamplerState;

		/** Default rasterizer state. */
		static LSG_RASTERIZER_STATE *				m_prsDefaultRasterizerState;

		/** Default blend state. */
		static LSG_BLEND_STATE *					m_pbsDefaultBlendState;

		/** Default depth/stencil state. */
		static LSG_DEPTH_STENCIL_STATE *			m_pdssDefaultDepthStencilState;

		/** The device capabilities. */
		static D3DCAPS9								m_cCaps;

		/** Device creation parameters. */
		static D3DDEVICE_CREATION_PARAMETERS		m_dcpCreationParms;

		/** The last presentation parameters we used. */
		static D3DPRESENT_PARAMETERS				m_ppPresentParms;

		/** The shader manager. */
		static CShaderManager *						m_psmShaderMan;

		/** D3DCREATE_HARDWARE_VERTEXPROCESSING or D3DCREATE_SOFTWARE_VERTEXPROCESSING. */
		static int32_t								m_i32VertProcType;

		/** In the event in which there are multiple windows, this tracks the information
		 *	for each window. */
		static CLinearMap<HWND, LSG_WINDOW>			m_mWindows;

#include "../Gfx/LSGCreateStatesMembersDecl.h"

		/** Is our device lost? */
		static LSBOOL								m_bDeviceLost;

		/** Are we toggling full-screen? */
		static LSBOOL								m_bToggling;

		/** The last render state. */
		static LSG_DX9_RENDER_STATE					m_rsLastRenderState;

		/** The current render state. */
		static LSG_DX9_RENDER_STATE					m_rsCurRenderState;

		/** Last samplers actually set on the rendering API. */
		static LSG_SAMPLER_STATE					m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS];

		/** Last rasterizer states actually set on the rendering API. */
		static LSG_RASTERIZER_STATE					m_rsLastRaster;

		/** Last blend state actually set on the rendering API. */
		static LSG_BLEND_STATE						m_bsLastBlend;

		/** Last depth/stencil state actually set on the rendering API. */
		static LSG_DEPTH_STENCIL_STATE				m_dssLastDepthStencil;


		// == Functions.
		/**
		 * Release all resources and reset our internal state.
		 */
		static void LSE_CALL						ResetInternal();

		/**
		 * Safely deletes an LSG_WINDOW object.
		 *
		 * \param _wWindow The object to delete.
		 */
		static void LSE_CALL						DeleteWindow( LSG_WINDOW &_wWindow );

		/**
		 * Creates a swap chain, back buffer, and depth/stencil buffer for a window object.  The ppPresent
		 *	member must be fully filled.
		 *
		 * \param _wWindow The object whose swap-chain features are to be created.
		 * \return Returns false if the creation of any of the resources fail.  Upon failure, the any created
		 *	resources will be freed.
		 */
		static LSBOOL LSE_CALL						CreateWindowSwapChain( LSG_WINDOW &_wWindow );

		/**
		 * Sets a sampler state in a given slot, avoiding redundant sets.  The index is to the sampler array inside
		 *	m_rsCurRenderState and redundancies are based off the values in m_rsLastRenderState.
		 *
		 * \param _ui32Slot The sampler-state slot from the sampler array in m_rsCurRenderState.
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetSamplerState( uint32_t _ui32Slot, LSBOOL _bForce = false );

		/**
		 * Sets sampler states, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetSamplerStates( LSBOOL _bForce = false );

		/**
		 * Sets the rasterizer state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetRasterizerState( LSBOOL _bForce = false );

		/**
		 * Sets the render-target blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetRenderTargetBlendState( uint32_t _ui32Slot, LSBOOL _bForce = false );

		/**
		 * Sets the blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetBlendState( LSBOOL _bForce = false );

		/**
		 * Sets the depth-stencil operator state, avoiding redundant sets.
		 *
		 * \param _bFront Front-facing or back-facing.
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetDepthStencilOpState( LSBOOL _bFront, LSBOOL _bForce = false );

		/**
		 * Sets the depth-stencil state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE void LSE_FCALL			SetDepthStencilState( LSBOOL _bForce = false );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Determines whether or not the device is lost.
	 *
	 * \return Returns true if the device is lost.
	 */
	LSE_INLINE LSBOOL LSE_CALL CDirectX9::DeviceIsLost() {
		return m_bDeviceLost;
	}

	/**
	 * Sets whether we are toggling full-screen now.
	 *
	 * \param _bVal Whether full-screen is being toggled or not.
	 */
	LSE_INLINE void LSE_CALL CDirectX9::TogglingFullScreen( LSBOOL _bVal ) {
		m_bToggling = _bVal;
	}

	/**
	 * Sets the screen settings.  Used when toggling fullscreen mode.
	 *
	 * \param _bWindowed Windowed mode?
	 * \param _hWnd New window handle.
	 */
	LSE_INLINE void LSE_CALL CDirectX9::SetWindowParms( LSBOOL _bWindowed, HWND _hWnd ) {
		// Get the width and height of the window.
		RECT rClient;
		::GetClientRect( _hWnd, &rClient );
		m_ppPresentParms.BackBufferWidth	= rClient.right - rClient.left;
		m_ppPresentParms.BackBufferHeight	= rClient.bottom - rClient.top;
		m_ppPresentParms.hDeviceWindow		= _hWnd;
		m_ppPresentParms.Windowed			= _bWindowed;
		m_ppPresentParms.BackBufferFormat	= D3DFMT_A8R8G8B8;
		m_ppPresentParms.BackBufferCount	= 1;
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
	LSE_INLINE CMatrix4x4 & LSE_CALL CDirectX9::PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _fFov, LSREAL _fAspect, LSREAL _fNear, LSREAL _fFar ) {
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
	LSE_INLINE CMatrix4x4 & LSE_CALL CDirectX9::OrthoOffCenter( CMatrix4x4 &_mIn,
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
	LSE_INLINE CFrustum & LSE_CALL CDirectX9::ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize ) {
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
	LSE_INLINE void LSE_FCALL CDirectX9::SetDefaultObjectRenderStates() {
		m_rsCurRenderState.prsRasterState = m_prsDefaultRasterizerState;
		m_rsCurRenderState.pbsBlendState = m_pbsDefaultBlendState;
		m_rsCurRenderState.pdssDepthStencilState = m_pdssDefaultDepthStencilState;
		m_rsCurRenderState.ui32StencilRef = 0;
		m_rsCurRenderState.fBlendFactors[0] = m_rsCurRenderState.fBlendFactors[1] =
			m_rsCurRenderState.fBlendFactors[2] = m_rsCurRenderState.fBlendFactors[3] = LSGREAL( 1.0 );
		m_rsCurRenderState.ui32SampleMask = 0xFFFFFFFFUL;
	}

	/**
	 * Sets all default render states.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetAllDefaultRenderStates() {
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
			m_rsCurRenderState.vViewports[I].X = 0;
			m_rsCurRenderState.vViewports[I].Y = 0;

			m_rsCurRenderState.vViewports[I].Width = m_ppPresentParms.BackBufferWidth;
			m_rsCurRenderState.vViewports[I].Height = m_ppPresentParms.BackBufferHeight;

			m_rsCurRenderState.vViewports[I].MinZ = static_cast<float>(LSG_MIN_DEPTH);
			m_rsCurRenderState.vViewports[I].MaxZ = static_cast<float>(LSG_MAX_DEPTH);


			m_rsCurRenderState.rScissors[I].left = 0;
			m_rsCurRenderState.rScissors[I].top = 0;

			m_rsCurRenderState.rScissors[I].right = m_ppPresentParms.BackBufferWidth;
			m_rsCurRenderState.rScissors[I].bottom = m_ppPresentParms.BackBufferHeight;
		}


		// Render targets.
		m_rsCurRenderState.ui8TotalRenderTargets = 1;
		for ( uint32_t I = 1; I < LSG_MAX_RENDER_TARGET_COUNT; ++I ) {
			m_rsCurRenderState.prtRenderTargets[I] = NULL;
		}
		m_rsCurRenderState.prtRenderTargets[0] = GetBackBufferSurface();
		m_rsCurRenderState.pdsDepthStencil = GetDepthStencilSurface();

		// Shader resources.
		for ( uint32_t I = LSG_MAX_TEXTURE_UNITS; I--; ) {
			m_rsCurRenderState.psrShaderResources[I] = NULL;
		}
		for ( uint32_t I = LSG_MAX_SHADER_SAMPLERS; I--; ) {
			m_rsCurRenderState.pssSamplers[I] = m_pssDefaultSamplerState;
			m_rsCurRenderState.bSRgb[I] = false;
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
	LSE_INLINE IDirect3DSurface9 * LSE_CALL CDirectX9::GetBackBufferSurface() {
		return m_pd3s9Back;
	}

	/**
	 * Gets the depth/stencil surface pointer.
	 *
	 * \return Returns the depth/stencil surface pointer.
	 */
	LSE_INLINE IDirect3DSurface9 * LSE_CALL CDirectX9::GetDepthStencilSurface() {
		return m_pd3s9DepthStencil;
	}

	/**
	 * Activates the default backbuffer and depth/stencil buffers.
	 */
	LSE_INLINE void LSE_CALL CDirectX9::ActivateBackBuffers() {
		CStd::MemSet( m_rsCurRenderState.prtRenderTargets, 0, sizeof( m_rsCurRenderState.prtRenderTargets ) );
		m_rsCurRenderState.prtRenderTargets[0] = GetBackBufferSurface();
		m_rsCurRenderState.pdsDepthStencil = GetDepthStencilSurface();
		SetViewport( LSM_ZERO, LSM_ZERO, LSREAL( m_ui32ScreenWidth ), LSREAL( m_ui32ScreenHeight ) );
	}

	/**
	 * Gets the DirectX 9 device.
	 *
	 * \return Returns the DirectX 9 device.
	 */
	LSE_INLINE IDirect3DDevice9 * CDirectX9::GetDirectX9Device() {
		return m_pd3dDevice;
	}
	/**
	 * Gets the DirectX 9 object.
	 *
	 * \return Returns the DirectX 9 object.
	 */
	LSE_INLINE IDirect3D9 * CDirectX9::GetDirectX9Object() {
		return m_d3dObject;
	}

	/**
	 * Gets the parameters used to create the DirectX 9 device.
	 *
	 * \return Returns the parameters used to create the DirectX 9 device.
	 */
	LSE_INLINE const D3DDEVICE_CREATION_PARAMETERS & CDirectX9::GetCreationParms() {
		return m_dcpCreationParms;
	}

	/**
	 * Gets the device caps.
	 *
	 * \return Returns the device caps for read only.
	 */
	LSE_INLINE const D3DCAPS9 & LSE_CALL CDirectX9::GetDeviceCaps() {
		return m_cCaps;
	}

	/**
	 * Gets the presentation parameters.
	 *
	 * \return Returns the presentation parameters used to create the window.
	 */
	LSE_INLINE const D3DPRESENT_PARAMETERS & LSE_CALL CDirectX9::GetPresentParms() {
		return m_ppPresentParms;
	}

	/**
	 * Gets a pointer to the shader manager.
	 *
	 * \return Returns a pointer to the shader manager used by this class.
	 */
	LSE_INLINE CShaderManager * LSE_FCALL CDirectX9::GetShaderManager() {
		return m_psmShaderMan;
	}

	/**
	 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
	 *
	 * \return Returns true if the rendering device is ready.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CDirectX9::BeginRender() {
		return SUCCEEDED( m_pd3dDevice->BeginScene() );
	}

	/**
	 * End rendering.  Must be called once per frame only, after all rendering is done.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::EndRender() {
		m_pd3dDevice->EndScene();
	}

	/**
	 * Present the backbuffer to the screen.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::Present() {
		if ( m_hCurWindow == m_hMainWindow ) {
			if ( m_pd3dDevice->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST ) {
				// Handle a lost device.
				LoseDevice();
			}
		}
		else {
			uint32_t ui32Index;
			if ( m_mWindows.ItemExists( m_hCurWindow, ui32Index ) ) {
				LSG_WINDOW & wWindow = m_mWindows.GetByIndex( ui32Index );
				if ( wWindow.pd3dsc9SwapChain ) {
					if ( wWindow.pd3dsc9SwapChain->Present( NULL, NULL, NULL, NULL, 0 ) == D3DERR_DEVICELOST ) {
						// Handle a lost device.
						LoseDevice();
					}
				}
			}
		}
		

		if ( DeviceIsLost() ) {
			ResetDevice();
		}
	}

	/**
	 * Clear the active buffer of the given type(s).
	 *
	 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::Clear( uint32_t _ui32Mask ) {
		ApplyFramebuffer();
		D3DCOLOR cClear = D3DCOLOR_RGBA( static_cast<int>(m_rsCurRenderState.fClearColor[0] * 255.0f),
			static_cast<int>(m_rsCurRenderState.fClearColor[1] * 255.0f),
			static_cast<int>(m_rsCurRenderState.fClearColor[2] * 255.0f),
			static_cast<int>(m_rsCurRenderState.fClearColor[3] * 255.0f) );
		m_pd3dDevice->Clear( 0, NULL,
			_ui32Mask & (LSG_B_COLOR_BUFFER | LSG_B_DEPTH_BUFFER | LSG_B_STENCIL_BUFFER),
			cClear,
			m_rsCurRenderState.fClearDepth,
			m_rsCurRenderState.ui8ClearStencil );
	}

	/**
	 * Clear all active buffers.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::Clear() {
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
	LSE_INLINE void LSE_CALL CDirectX9::SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA ) {
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
	LSE_INLINE void LSE_FCALL CDirectX9::SetClearDepth( LSREAL _fValue ) {
		m_rsCurRenderState.fClearDepth = static_cast<LSGREAL>(_fValue);
	}

	/**
	 * Sets the stencil clear value.
	 *
	 * \param _ui8Val The new stencil clear value.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetClearStencil( uint8_t _ui8Val ) {
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
	LSE_INLINE void LSE_CALL CDirectX9::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<DWORD>(_fX) >= static_cast<DWORD>(LSG_VIEWPORT_MIN) && static_cast<DWORD>(_fX) <= static_cast<DWORD>(LSG_VIEWPORT_MAX) );
		assert( static_cast<DWORD>(_fY) >= static_cast<DWORD>(LSG_VIEWPORT_MIN) && static_cast<DWORD>(_fY) <= static_cast<DWORD>(LSG_VIEWPORT_MAX) );

		assert( static_cast<DWORD>(_fWidth) >= 0.0f && static_cast<DWORD>(_fX + _fWidth) <= static_cast<DWORD>(LSG_VIEWPORT_MAX) );
		assert( static_cast<DWORD>(_fHeight) >= 0.0f && static_cast<DWORD>(_fY + _fHeight) <= static_cast<DWORD>(LSG_VIEWPORT_MAX) );

		m_rsCurRenderState.vViewports[_ui32Target].X = static_cast<DWORD>(_fX);
		m_rsCurRenderState.vViewports[_ui32Target].Y = static_cast<DWORD>(_fY);
		m_rsCurRenderState.vViewports[_ui32Target].Width = static_cast<DWORD>(_fWidth);
		m_rsCurRenderState.vViewports[_ui32Target].Height = static_cast<DWORD>(_fHeight);
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
	LSE_INLINE void LSE_CALL CDirectX9::SetViewport( LSREAL _fX, LSREAL _fY,
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
	LSE_INLINE void LSE_FCALL CDirectX9::SetDepthRange( LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<float>(_fMin) >= static_cast<float>(LSG_MIN_DEPTH) && static_cast<float>(_fMin) <= static_cast<float>(LSG_MAX_DEPTH) );
		assert( static_cast<float>(_fMax) >= static_cast<float>(LSG_MIN_DEPTH) && static_cast<float>(_fMax) <= static_cast<float>(LSG_MAX_DEPTH) );

		m_rsCurRenderState.vViewports[_ui32Target].MinZ = static_cast<float>(_fMin);
		m_rsCurRenderState.vViewports[_ui32Target].MaxZ = static_cast<float>(_fMax);
	}

	/**
	 * Sets the total number of viewports.
	 *
	 * \param _ui32Total The total number of viewports rectangles set.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetTotalViewports( uint32_t _ui32Total ) {
		assert( _ui32Total < LSG_MAX_VIEWPORTS_SCISSORS );

		m_rsCurRenderState.ui8TotalViewportsAndScissors = static_cast<uint8_t>(_ui32Total);
	}

	/**
	 * Gets the reverse culling for the given input cull mode.
	 *
	 * \param _cmMode The input cull mode whose reverse is to be returned.
	 * \return Returns LSG_CM_FRONT for LSG_CM_BACK, LSG_CM_BACK for LSG_CM_FRONT, and LSG_CM_NONE for LSG_CM_NONE.
	 */
	LSE_INLINE LSG_CULL_MODE LSE_FCALL CDirectX9::ReverseCull( LSG_CULL_MODE _cmMode ) {
		switch ( _cmMode ) {
			case LSG_CM_NONE : { return LSG_CM_NONE; }
			case LSG_CM_BACK : { return LSG_CM_FRONT; }
			default : { return LSG_CM_BACK; }
		}
	}

	/**
	 * Gets the total number of vertex buffers set for read and write.
	 *
	 * \return Returns a reference to the currently set number of vertex buffers.
	 */
	LSE_INLINE uint32_t & LSE_CALL CDirectX9::TotalActiveVertexBuffers() {
		return m_rsCurRenderState.ui32TotalVertexBuffers;
	}

	/**
	 * Gets the array of current vertex buffers for read and write.
	 *
	 * \return Returns the array of current vertex buffers for read and write.
	 */
	LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL CDirectX9::VertexBuffers() {
		return m_rsCurRenderState.pvbVertexBuffers;
	}

	/**
	 * Sets the input layout.
	 *
	 * \param _pilLayout The input layout to set.
	 * \return Returns true if the layout was properly set.
	 */
	LSE_INLINE LSBOOL LSE_CALL CDirectX9::SetInputLayout( const LSG_INPUT_LAYOUT * _pilLayout ) {
		if ( m_rsCurRenderState.pilInputLayout == _pilLayout ) { return true; }
		m_rsCurRenderState.pilInputLayout = _pilLayout;
		return SUCCEEDED( GetDirectX9Device()->SetVertexDeclaration( static_cast<IDirect3DVertexDeclaration9 *>(const_cast<LSG_INPUT_LAYOUT *>(m_rsCurRenderState.pilInputLayout)) ) );
	}

	/**
	 * Safely release an object.
	 *
	 * \param _tObj The object to release.
	 */
	template <typename tnType>
	LSE_INLINE void LSE_CALL CDirectX9::SafeRelease( tnType &_tObj ) {
		if ( _tObj ) {
			_tObj->Release();
			_tObj = NULL;
		}
	}

	/**
	 * Sets a sampler state in a given slot, avoiding redundant sets.  The index is to the sampler array inside
	 *	m_rsCurRenderState and redundancies are based off the values in m_rsLastRenderState.
	 *
	 * \param _ui32Slot The sampler-state slot from the sampler array in m_rsCurRenderState.
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetSamplerState( uint32_t _ui32Slot, LSBOOL _bForce ) {
		if ( !m_rsCurRenderState.pssSamplers[_ui32Slot] ) {
			m_rsCurRenderState.pssSamplers[_ui32Slot] = m_pssDefaultSamplerState;
		}
		if ( !m_rsLastRenderState.pssSamplers[_ui32Slot] ) {
			m_rsLastRenderState.pssSamplers[_ui32Slot] = m_pssDefaultSamplerState;
			_bForce = true;
		}
		const LSG_SAMPLER_STATE & ssCurState = (*m_rsCurRenderState.pssSamplers[_ui32Slot]);
		LSG_SAMPLER_STATE & ssLastState = m_ssLastSamplers[_ui32Slot];

#define LSG_CHANGED( MEMBER )	((ssCurState.MEMBER != ssLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	ssLastState.MEMBER = ssCurState.MEMBER

		// Check filters.
		if ( LSG_CHANGED( Filter ) ) {
			// Min filter.
			DWORD dwCurMinFilter = LSG_DECODE_MIN_FILTER( ssCurState.Filter );
			DWORD dwLastMinFilter = LSG_DECODE_MIN_FILTER( ssLastState.Filter );
			if ( dwCurMinFilter != dwLastMinFilter || _bForce ) {
				GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MINFILTER, dwCurMinFilter );
			}

			// Mag filter.
			DWORD dwCurMagFilter = LSG_DECODE_MAG_FILTER( ssCurState.Filter );
			DWORD dwLastMagFilter = LSG_DECODE_MAG_FILTER( ssLastState.Filter );
			if ( dwCurMagFilter != dwLastMagFilter || _bForce ) {
				GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MAGFILTER, dwCurMagFilter );
			}

			// Mip filter.
			DWORD dwCurMipFilter = LSG_DECODE_MIP_FILTER( ssCurState.Filter );
			DWORD dwLastMipFilter = LSG_DECODE_MIP_FILTER( ssLastState.Filter );
			if ( dwCurMipFilter != dwLastMipFilter || _bForce ) {
				GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MIPFILTER, dwCurMipFilter );
			}
		}
		
		// Check anisotropy.
		LSBOOL bCurIsAniso = LSG_DECODE_IS_ANISOTROPIC_FILTER( ssCurState.Filter );
		LSBOOL bLastIsAniso = LSG_DECODE_IS_ANISOTROPIC_FILTER( ssLastState.Filter );
		if ( LSG_CHANGED( MaxAnisotropy ) || bCurIsAniso != bLastIsAniso ) {
			if ( bCurIsAniso ) {
				GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MAXANISOTROPY, ssCurState.MaxAnisotropy );
			}
			else {
				GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MAXANISOTROPY, 1 );
			}
		}
		LSG_UPDATE( Filter );
		LSG_UPDATE( MaxAnisotropy );

		// Check the U address mode.
		if ( LSG_CHANGED( AddressU ) ) {
			GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_ADDRESSU, ssCurState.AddressU );
			LSG_UPDATE( AddressU );
		}

		// Check the V address mode.
		if ( LSG_CHANGED( AddressV ) ) {
			GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_ADDRESSV, ssCurState.AddressV );
			LSG_UPDATE( AddressV );
		}

		// Check the W address mode.
		if ( LSG_CHANGED( AddressW ) ) {
			GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_ADDRESSW, ssCurState.AddressW );
			LSG_UPDATE( AddressW );
		}

		// Check the mip LOD bias.
		if ( LSG_CHANGED( MipLODBias ) ) {
			GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MIPMAPLODBIAS, (*reinterpret_cast<const DWORD *>(&ssCurState.MipLODBias)) );
			LSG_UPDATE( MipLODBias );
		}

		// Check the border color. 
		if ( LSG_CHANGED( BorderColor[0] ) || LSG_CHANGED( BorderColor[1] ) || LSG_CHANGED( BorderColor[2] ) || LSG_CHANGED( BorderColor[3] ) ) {
			D3DCOLOR cColor = D3DCOLOR_RGBA( static_cast<int>(ssCurState.BorderColor[0] * 255.0f),
				static_cast<int>(ssCurState.BorderColor[1] * 255.0f),
				static_cast<int>(ssCurState.BorderColor[2] * 255.0f),
				static_cast<int>(ssCurState.BorderColor[3] * 255.0f) );
			GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_BORDERCOLOR, cColor );
			LSG_UPDATE( BorderColor[0] );
			LSG_UPDATE( BorderColor[1] );
			LSG_UPDATE( BorderColor[2] );
			LSG_UPDATE( BorderColor[3] );
		}

		// Most detailed mip-map level to use (with 0 being the most detailed).
		if ( LSG_CHANGED( MinLOD ) ) {
			GetDirectX9Device()->SetSamplerState( _ui32Slot, D3DSAMP_MAXMIPLEVEL, static_cast<DWORD>(ssCurState.MinLOD) );
			LSG_UPDATE( MinLOD );
		}

		/*
		LSG_COMPARISON_FUNC							ComparisonFunc;
		LSGREAL										MaxLOD;
		*/

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets sampler states, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetSamplerStates( LSBOOL _bForce ) {
		for ( uint32_t I = CStd::Min<uint32_t>( m_mMetrics.ui32MaxTexSlot, LSG_MAX_SHADER_SAMPLERS ); I--; ) {
			if ( m_rsCurRenderState.pssSamplers[I] != m_rsLastRenderState.pssSamplers[I] || _bForce ) {
				SetSamplerState( I, _bForce );
				m_rsLastRenderState.pssSamplers[I] = m_rsCurRenderState.pssSamplers[I];
			}

			// sRGB flag.
			if ( m_rsCurRenderState.bSRgb[I] != m_rsLastRenderState.bSRgb[I] || _bForce ) {
				GetDirectX9Device()->SetSamplerState( I, D3DSAMP_SRGBTEXTURE, m_rsCurRenderState.bSRgb[I] );
				m_rsLastRenderState.bSRgb[I] = m_rsCurRenderState.bSRgb[I];
			}
		}
	}

	/**
	 * Sets the rasterizer state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetRasterizerState( LSBOOL _bForce ) {
		if ( !m_rsCurRenderState.prsRasterState ) {
			// NULL states are the same as default states.
			m_rsCurRenderState.prsRasterState = m_prsDefaultRasterizerState;
		}
		// Early out if this state is the same as last time.
		if ( m_rsCurRenderState.prsRasterState == m_rsLastRenderState.prsRasterState && !_bForce ) { return; }
		m_rsLastRenderState.prsRasterState = m_rsCurRenderState.prsRasterState;
		
		const LSG_RASTERIZER_STATE & rsCurState = (*m_rsCurRenderState.prsRasterState);
		LSG_RASTERIZER_STATE & rsLastState = m_rsLastRaster;

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Check the fill mode.
		if ( LSG_CHANGED( FillMode ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_FILLMODE, rsCurState.FillMode );
			LSG_UPDATE( FillMode );
		}

		// Check the cull mode.
		if ( LSG_CHANGED( CullMode ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_CULLMODE, rsCurState.CullMode );
			//LSG_UPDATE( CullMode );
		}

		// Check the front-face mode.
		if ( LSG_CHANGED( CullMode ) || LSG_CHANGED( FrontCounterClockwise ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_CULLMODE, rsCurState.FrontCounterClockwise ? rsCurState.CullMode : ReverseCull( rsCurState.CullMode ) );
			LSG_UPDATE( CullMode );
			LSG_UPDATE( FrontCounterClockwise );
		}

		// Check the depth bias.
		if ( LSG_CHANGED( DepthBias ) ) {
			// TODO: Scale rsCurState.DepthBias by the minimum non-zero depth-buffer value first instead of hard-coding it to 1.0f / 16777216.0f.
			FLOAT fBias = static_cast<FLOAT>(rsCurState.DepthBias) * (1.0f / 16777216.0f);
			GetDirectX9Device()->SetRenderState( D3DRS_DEPTHBIAS, (*reinterpret_cast<DWORD *>(&fBias)) );
			LSG_UPDATE( DepthBias );
		}

		// Check the slope scaled depth bias.
		if ( LSG_CHANGED( SlopeScaledDepthBias ) || LSG_CHANGED( DepthBiasClamp ) ) {
			FLOAT fBias = rsCurState.SlopeScaledDepthBias * rsCurState.DepthBiasClamp;
			GetDirectX9Device()->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, (*reinterpret_cast<DWORD *>(&fBias)) );
			LSG_UPDATE( SlopeScaledDepthBias );
			LSG_UPDATE( DepthBiasClamp );
		}

		// Check the depth-clip mode.
		if ( LSG_CHANGED( DepthClipEnable ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_CLIPPING, rsCurState.DepthClipEnable );
			LSG_UPDATE( DepthClipEnable );
		}

		// Check the scissor mode.
		if ( LSG_CHANGED( ScissorEnable ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_SCISSORTESTENABLE, rsCurState.ScissorEnable );
			LSG_UPDATE( ScissorEnable );
		}

		// Check the multi-sample mode.
		if ( LSG_CHANGED( MultisampleEnable ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, rsCurState.MultisampleEnable );
			LSG_UPDATE( MultisampleEnable );
		}

		// Check the antialiased line mode.
		if ( LSG_CHANGED( AntialiasedLineEnable ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, rsCurState.AntialiasedLineEnable );
			LSG_UPDATE( AntialiasedLineEnable );
		}


#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the render-target blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetRenderTargetBlendState( uint32_t _ui32Slot, LSBOOL _bForce ) {
		const LSG_RENDER_TARGET_BLEND_DESC & rtbsCurState = m_rsCurRenderState.pbsBlendState->RenderTarget[_ui32Slot];
		LSG_RENDER_TARGET_BLEND_DESC & rtbsLastState = m_bsLastBlend.RenderTarget[_ui32Slot];

#define LSG_CHANGED( MEMBER )	((rtbsCurState.MEMBER != rtbsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rtbsLastState.MEMBER = rtbsCurState.MEMBER

		if ( _ui32Slot == 0 ) {
			// Check the blend mode.
			if ( LSG_CHANGED( BlendEnable ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_ALPHABLENDENABLE, rtbsCurState.BlendEnable );
				LSG_UPDATE( BlendEnable );
			}

			// Check the source blend.
			if ( LSG_CHANGED( SrcBlend ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_SRCBLEND, rtbsCurState.SrcBlend );
				LSG_UPDATE( SrcBlend );
			}

			// Check the destination blend.
			if ( LSG_CHANGED( DestBlend ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_DESTBLEND, rtbsCurState.DestBlend );
				LSG_UPDATE( DestBlend );
			}

			// Check the blend operator.
			if ( LSG_CHANGED( BlendOp ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_BLENDOP, rtbsCurState.BlendOp );
				LSG_UPDATE( BlendOp );
			}

			// Check the source alpha blend.
			if ( LSG_CHANGED( SrcBlendAlpha ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_SRCBLENDALPHA, rtbsCurState.SrcBlendAlpha );
				LSG_UPDATE( SrcBlendAlpha );
			}

			// Check the destination alpha blend.
			if ( LSG_CHANGED( DestBlendAlpha ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_DESTBLENDALPHA, rtbsCurState.DestBlendAlpha );
				LSG_UPDATE( DestBlendAlpha );
			}

			// Check the alpha blend operator.
			if ( LSG_CHANGED( BlendOpAlpha ) ) {
				GetDirectX9Device()->SetRenderState( D3DRS_BLENDOPALPHA, rtbsCurState.BlendOpAlpha );
				LSG_UPDATE( BlendOpAlpha );
			}
		}

		// Check the write mask.
		if ( LSG_CHANGED( RenderTargetWriteMask ) ) {
			static const D3DRENDERSTATETYPE rstTarget[] = {
				D3DRS_COLORWRITEENABLE,
				D3DRS_COLORWRITEENABLE1,
				D3DRS_COLORWRITEENABLE2,
				D3DRS_COLORWRITEENABLE3
			};
			assert( _ui32Slot < LSE_ELEMENTS( rstTarget ) );
			GetDirectX9Device()->SetRenderState( rstTarget[_ui32Slot], rtbsCurState.RenderTargetWriteMask );
			LSG_UPDATE( RenderTargetWriteMask );
		}


#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetBlendState( LSBOOL _bForce ) {
		if ( !m_rsCurRenderState.pbsBlendState ) {
			// NULL states are the same as default states.
			m_rsCurRenderState.pbsBlendState = m_pbsDefaultBlendState;
		}
		// Early out if this state is the same as last time.
		if ( m_rsCurRenderState.pbsBlendState == m_rsLastRenderState.pbsBlendState && !_bForce ) { return; }
		m_rsLastRenderState.pbsBlendState = m_rsCurRenderState.pbsBlendState;
		
		/*const LSG_BLEND_STATE & bsCurState = (*m_rsCurRenderState.pbsBlendState);
		LSG_BLEND_STATE & bsLastState = m_bsLastBlend;*/

#define LSG_CHANGED( MEMBER )	((bsCurState.MEMBER != bsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	bsLastState.MEMBER = bsCurState.MEMBER

		// Direct3D 9.0c supports neither alpha-to-coverage nor independent blending,
		//	so ignore both AlphaToCoverageEnable and IndependentBlendEnable.  Still
		//	have to go over the render targets because RenderTarget[I].RenderTargetWriteMask
		//	is supported.
		for ( uint32_t I = LSG_MAX_RENDER_TARGET_COUNT; I--; ) {
			SetRenderTargetBlendState( I, _bForce );
		}

		//LSBOOL										AlphaToCoverageEnable;							/**< Specifies whether to use alpha-to-coverage as a multisampling technique when setting a pixel to a render target. */
		//LSBOOL										IndependentBlendEnable;							/**< Specifies whether to enable independent blending in simultaneous render targets. Set to TRUE to enable independent blending. */

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the depth-stencil operator state, avoiding redundant sets.
	 *
	 * \param _bFront Front-facing or back-facing.
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetDepthStencilOpState( LSBOOL _bFront, LSBOOL _bForce ) {
		const LSG_DEPTH_STENCILOP_DESC & _dsodCurState = _bFront ? m_rsCurRenderState.pdssDepthStencilState->FrontFace :
			m_rsCurRenderState.pdssDepthStencilState->BackFace;
		LSG_DEPTH_STENCILOP_DESC & _dsodLastState = _bFront ? m_dssLastDepthStencil.FrontFace :
			m_dssLastDepthStencil.BackFace;

#define LSG_CHANGED( MEMBER )	((_dsodCurState.MEMBER != _dsodLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	_dsodLastState.MEMBER = _dsodCurState.MEMBER

		// Check the stencil fail operation.
		if ( LSG_CHANGED( StencilFailOp ) ) {
			GetDirectX9Device()->SetRenderState( _bFront ? D3DRS_CCW_STENCILFAIL : D3DRS_STENCILFAIL, _dsodCurState.StencilFailOp );
			LSG_UPDATE( StencilFailOp );
		}

		// Check the stencil depth-fail operation.
		if ( LSG_CHANGED( StencilDepthFailOp ) ) {
			GetDirectX9Device()->SetRenderState( _bFront ? D3DRS_CCW_STENCILZFAIL : D3DRS_STENCILZFAIL, _dsodCurState.StencilDepthFailOp );
			LSG_UPDATE( StencilDepthFailOp );
		}

		// Check the stencil pass operation.
		if ( LSG_CHANGED( StencilPassOp ) ) {
			GetDirectX9Device()->SetRenderState( _bFront ? D3DRS_CCW_STENCILPASS : D3DRS_STENCILPASS, _dsodCurState.StencilPassOp );
			LSG_UPDATE( StencilPassOp );
		}

		// Check the stencil function.
		if ( LSG_CHANGED( StencilFunc ) ) {
			GetDirectX9Device()->SetRenderState( _bFront ? D3DRS_CCW_STENCILFUNC : D3DRS_STENCILFUNC, _dsodCurState.StencilFunc );
			LSG_UPDATE( StencilFunc );
		}

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the depth-stencil state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL CDirectX9::SetDepthStencilState( LSBOOL _bForce ) {
		if ( !m_rsCurRenderState.pdssDepthStencilState ) {
			// NULL states are the same as default states.
			m_rsCurRenderState.pdssDepthStencilState = m_pdssDefaultDepthStencilState;
		}
		// Early out if this state is the same as last time.
		if ( m_rsCurRenderState.pdssDepthStencilState == m_rsLastRenderState.pdssDepthStencilState && !_bForce ) { return; }
		m_rsLastRenderState.pdssDepthStencilState = m_rsCurRenderState.pdssDepthStencilState;
		
		const LSG_DEPTH_STENCIL_STATE & dssCurState = (*m_rsCurRenderState.pdssDepthStencilState);
		LSG_DEPTH_STENCIL_STATE & dssLastState = m_dssLastDepthStencil;

#define LSG_CHANGED( MEMBER )	((dssCurState.MEMBER != dssLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	dssLastState.MEMBER = dssCurState.MEMBER

		// Check the depth-enable.
		if ( LSG_CHANGED( DepthEnable ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_ZENABLE, dssCurState.DepthEnable );
			LSG_UPDATE( DepthEnable );
		}

		// Check depth-writing.
		if ( LSG_CHANGED( DepthWriteMask ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_ZWRITEENABLE, dssCurState.DepthWriteMask );
			LSG_UPDATE( DepthWriteMask );
		}

		// Check the depth function.
		if ( LSG_CHANGED( DepthFunc ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_ZFUNC, dssCurState.DepthFunc );
			LSG_UPDATE( DepthFunc );
		}

		// Check the stencil-enable.
		if ( LSG_CHANGED( StencilEnable ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_STENCILENABLE, dssCurState.StencilEnable );
			LSG_UPDATE( StencilEnable );
		}

		// Check the stencil write mask.
		if ( LSG_CHANGED( StencilWriteMask ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_STENCILWRITEMASK, dssCurState.StencilWriteMask );
			LSG_UPDATE( StencilWriteMask );
		}

		SetDepthStencilOpState( true, _bForce );	// Does FrontFace.
		SetDepthStencilOpState( false, _bForce );	// Does BackFace.
#undef LSG_UPDATE
#undef LSG_CHANGED

		const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;
#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Stencil reference.
		if ( LSG_CHANGED( ui32StencilRef ) ) {
			GetDirectX9Device()->SetRenderState( D3DRS_STENCILREF, rsCurState.ui32StencilRef );
			LSG_UPDATE( ui32StencilRef );
		}
	
		//uint8_t										StencilReadMask;								/**< Identify a portion of the depth-stencil buffer for reading stencil data. */

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9_H__
