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
 * Description: Interface with the Metal API.
 */


#ifndef __LSG_METAL_H__
#define __LSG_METAL_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_METAL

#include "../Gfx/LSGGfxBase.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "Map/LSTLLinearMap.h"

#ifdef LSE_IPHONE

#else
#error "Unrecognized Metal platform."
#endif	// #ifdef LSE_IPHONE

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** Gets the device object from the void pointer. */
#define GetDeviceObject()							((__bridge CMetalObject *)CMetal::GetDevice())
/** Gets the device from the void pointer. */
#define GetMetalDevice()							[GetDeviceObject() device]
/** Gets the layer from the void pointer. */
#define GetLayer()									[GetDeviceObject() layer]
/** Gets the command queue from the void pointer. */
#define GetCommandQueue()							[GetDeviceObject() commandQueue]
/** Gets the command buffer from the void pointer. */
#define GetCommandBuffer()							[GetDeviceObject() commandBuffer]
/** Gets the back buffer from the void pointer. */
#define GetBackBuffer()								[GetDeviceObject() backBuffer]
/** Gets the depth buffer from the void pointer. */
#define GetDepthBuffer()							[GetDeviceObject() depthBuffer]
/** Gets the stencil buffer from the void pointer. */
#define GetStencilBuffer()							[GetDeviceObject() stencilBuffer]
/** Gets the stencil buffer from the void pointer. */
#define GetCommandEncoder()							[GetCommandBuffer() commandEncoder]
/** Gets the render pipeline descriptor from the void pointer. */
#define GetRenderPipelineDescriptor()				[GetDeviceObject() renderPipelineDesc]

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Enumerations.
	/** The blend types we support.  These are for blending to the backbuffer only. */
	enum LSG_BLEND_MODE {
		LSG_BM_ZERO									= 0 /*MTLBlendFactorZero*/,						/**< Blend factor is (0, 0, 0, 0). */
		LSG_BM_ONE									= 1 /*MTLBlendFactorOne*/,						/**< Blend factor is (1, 1, 1, 1). */
		LSG_BM_SRC_COLOR							= 2 /*MTLBlendFactorSourceColor*/,				/**< Blend factor is (Rs, Gs, Bs, As). */
		LSG_BM_INV_SRC_COLOR						= 3 /*MTLBlendFactorOneMinusSourceColor*/,		/**< Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		LSG_BM_DST_COLOR							= 6 /*MTLBlendFactorDestinationColor*/,			/**< Blend factor is (Rd, Gd, Bd, Ad). */
		LSG_BM_INV_DST_COLOR						= 7 /*MTLBlendFactorOneMinusDestinationColor*/,	/**< Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		LSG_BM_SRC_ALPHA							= 4 /*MTLBlendFactorSourceAlpha*/,				/**< Blend factor is (As, As, As, As). */
		LSG_BM_INV_SRC_ALPHA						= 5 /*MTLBlendFactorOneMinusSourceAlpha*/,		/**< Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		LSG_BM_DST_ALPHA							= 8 /*MTLBlendFactorDestinationAlpha*/,			/**< Blend factor is (Ad Ad Ad Ad). */
		LSG_BM_INV_DEST_ALPHA						= 9 /*MTLBlendFactorOneMinusDestinationAlpha*/,	/**< Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		LSG_BM_SRC_ALPHA_SAT						= 10 /*MTLBlendFactorSourceAlphaSaturated*/,	/**< Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */

		LSG_BM_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** The blend operations we support. */
	enum LSG_BLEND_OP {
		LSG_BO_ADD									= 0 /*MTLBlendOperationAdd*/,					/**< Add source 1 and source 2. */
		LSG_BO_SUBTRACT								= 1 /*MTLBlendOperationSubtract*/,				/**< Subtract source 1 and source 2. */
		LSG_BO_REV_SUBTRACT							= 2 /*MTLBlendOperationReverseSubtract*/,		/**< Subtract source 2 from source 1. */
		LSG_BO_MIN									= 3 /*MTLBlendOperationMin*/,					/**< Find the minimum of source 1 and source 2. */
		LSG_BO_MAX									= 4 /*MTLBlendOperationMax*/,					/**< Find the maximum of source 1 and source 2. */

		LSG_BO_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** Buffers. */
	enum LSG_BUFFER {
		LSG_B_COLOR_BUFFER							= 1 << 0,										/**< Clear a render target. */
		LSG_B_STENCIL_BUFFER						= 1 << 2,										/**< Clear the stencil buffer. */
		LSG_B_DEPTH_BUFFER							= 1 << 3,										/**< Clear the depth buffer. */
		LSG_B_ALL									= LSG_B_COLOR_BUFFER | LSG_B_STENCIL_BUFFER |
			LSG_B_DEPTH_BUFFER,																		/**< Clear all buffers. */
	};

	/** The color-write modes. */
	enum LSG_COLOR_WRITE_ENABLE {
		LSG_CWM_RED									= 0x1 << 3 /*MTLColorWriteMaskRed*/,			/**< Allow data to be stored in the red component. */
		LSG_CWM_GREEN								= 0x1 << 2 /*MTLColorWriteMaskGreen*/,			/**< Allow data to be stored in the green component. */
		LSG_CWM_BLUE								= 0x1 << 1 /*MTLColorWriteMaskBlue*/,			/**< Allow data to be stored in the blue component. */
		LSG_CWM_ALPHA								= 0x1 << 0 /*MTLColorWriteMaskAlpha*/,			/**< Allow data to be stored in the alpha component. */
		LSG_CWM_ALL									= 0xf /*MTLColorWriteMaskAll*/,					/**< Allow data to be stored in all components. */
	};

	/** Comparison functions. */
	enum LSG_COMPARISON_FUNC {
		LSG_CF_NEVER								= 0 /*MTLCompareFunctionNever*/,				/**< Never pass the comparison. */
		LSG_CF_ALWAYS								= 7 /*MTLCompareFunctionAlways*/,				/**< Always pass the comparison. */
		LSG_CF_LESS									= 1 /*MTLCompareFunctionLess*/,					/**< If the source data is less than the destination data, the comparison passes. */
		LSG_CF_EQUAL								= 2 /*MTLCompareFunctionEqual*/,				/**< If the source data is equal to the destination data, the comparison passes. */
		LSG_CF_NOTEQUAL								= 5 /*MTLCompareFunctionNotEqual*/,				/**< If the source data is not equal to the destination data, the comparison passes. */
		LSG_CF_LESSEQUAL							= 3 /*MTLCompareFunctionLessEqual*/,			/**< If the source data is less than or equal to the destination data, the comparison passes. */
		LSG_CF_GREATER								= 4 /*MTLCompareFunctionGreater*/,				/**< If the source data is greater than the destination data, the comparison passes. */
		LSG_CF_GREATEREQUAL							= 6 /*MTLCompareFunctionGreaterEqual*/,			/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	};

	/** Culling modes. */
	enum LSG_CULL_MODE {
		LSG_CM_NONE									= 0 /*MTLCullModeNone*/,						/**< No culling. */
		LSG_CM_FRONT								= 1 /*MTLCullModeFront*/,						/**< Cull front faces. */
		LSG_CM_BACK									= 2 /*MTLCullModeBack*/,						/**< Cull back faces. */
	};

	/** Depth write masks. */
	enum LSG_DEPTH_WRITE_MASK {
		LSG_DRM_ZERO								= 0,											/**< Turn off writes to the depth-stencil buffer. */
		LSG_DRM_ALL									= 1,											/**< Turn on writes to the depth-stencil buffer. */
	};

	/** Fill modes. */
	enum LSG_FILL_MODE {
		LSG_FM_SOLID								= 0 /*MTLTriangleFillModeFill*/,				/**< Solid filling mode. */
		LSG_FM_WIRE									= 1 /*MTLTriangleFillModeLines*/,				/**< Wire-frame filling mode. */
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
		LSG_SO_KEEP									= 0 /*MTLStencilOperationKeep*/,				/**< Keep the existing stencil data. */
		LSG_SO_ZERO									= 1 /*MTLStencilOperationZero*/,				/**< Set the stencil data to 0. */
		LSG_SO_REPLACE								= 2 /*MTLStencilOperationReplace*/,				/**< Set the stencil data to the reference value. */
		LSG_SO_INC_SAT								= 3 /*MTLStencilOperationIncrementClamp*/,		/**< Increment the stencil value by 1, and clamp the result. */
		LSG_SO_DEC_SAT								= 4 /*MTLStencilOperationDecrementClamp*/,		/**< Decrement the stencil value by 1, and clamp the result. */
		LSG_SO_INVERT								= 5 /*MTLStencilOperationInvert*/,				/**< Invert the stencil data. */
		LSG_SO_INC									= 6 /*MTLStencilOperationIncrementWrap*/,		/**< Increment the stencil value by 1, and wrap the result if necessary. */
		LSG_SO_DEC									= 7 /*MTLStencilOperationDecrementWrap*/,		/**< Decrement the stencil value by 1, and wrap the result if necessary. */
	};

	/** Texture addressing modes. */
	enum LSG_TEXTURE_ADDRESS_MODE {
		LSG_TAM_WRAP								= 2 /*MTLSamplerAddressModeRepeat*/,			/**< Tile the texture at every (u,v) integer junction. */
		LSG_TAM_MIRROR								= 3 /*MTLSamplerAddressModeMirrorRepeat*/,		/**< Flip the texture at every (u,v) integer junction. */
		LSG_TAM_CLAMP								= 0 /*MTLSamplerAddressModeClampToEdge*/,		/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		LSG_TAM_BORDER								= 0,											/**< Texture coordinates outside the range [0.0, 1.0] are set to the border color. */
		LSG_TAM_MIRROR_ONCE							= 0,											/**< Similar to LSG_TAM_MIRROR and LSG_TAM_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */
	};

	/** Compile-time metrics.  For run-time, use the metrics structure returned by the graphics device. */
	enum LSG_METRICS {
		LSG_MAX_TEXTURE_UNITS						= 32,											/**< Total texture units bindable to a shader at a time. */
		LSG_MAX_SHADER_SAMPLERS						= 16,											/**< Total number of samplers that can be set on a shader at once. */
		LSG_MAX_RENDER_TARGET_COUNT					= 8,											/**< Total simultaneous render-target count. */
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
#define LSG_DEFAULT_STENCIL_READ_MASK				(1)												/**< Default stencil read mask. */
#define LSG_DEFAULT_STENCIL_WRITE_MASK				(1)												/**< Default stencil write mask. */


	// == Types.
	/** The shader manager type. */
	class											CFramebuffer;
	typedef class CMetalShaderManager				CShaderManager;
	class											CShader;
	class											CVertexBuffer;

#include "../Gfx/LSGDescStructs.h"

	/** The 1D texture type. */
	typedef LSVOID									LSG_TEXTURE_1D;

	/** The 2D texture type. */
	typedef LSVOID									LSG_TEXTURE_2D;

	/** The 3D texture type. */
	typedef LSVOID									LSG_TEXTURE_3D;

	/** The sampler type. */
	typedef LSG_SAMPLER_DESC						LSG_SAMPLER_STATE;

	/** The shader resource view type. */
	typedef LSVOID									LSG_SHADER_RESOURCE;

	/** The render target type. */
	typedef LSVOID									LSG_RENDER_TARGET;

	/** The depth/stencil type. */
	typedef LSVOID									LSG_DEPTH_STENCIL;

	/** The rasterizer state type. */
	typedef LSG_RASTERIZER_DESC						LSG_RASTERIZER_STATE;

	/** The blend state type. */
	typedef LSG_BLEND_DESC							LSG_BLEND_STATE;

	/** The depth/stencil state. */
	typedef LSG_DEPTH_STENCIL_DESC					LSG_DEPTH_STENCIL_STATE;

	/** The viewport type. */
	typedef struct LSG_VIEWPORT {
		LSINT32										iX;												/**< The left coordinate of the viewport in pixels. */
		LSINT32										iY;												/**< The upper coordinate of the viewport in pixels. */
		LSUINT32									sWidth;											/**< Width of the viewport in pixels. */
		LSUINT32									sHeight;										/**< Height of the viewport in pixels. */
		LSFLOAT										fMinZ;											/**< Together with MaxZ, value describing the range of depth values into which a scene is to be rendered, the minimum and maximum values of the clip volume. */
		LSFLOAT										fMaxZ;											/**< Together with MinZ, value describing the range of depth values into which a scene is to be rendered, the minimum and maximum values of the clip volume. */
	} * LPLSG_VIEWPORT, * const LPCLSG_VIEWPORT;

	/** The scissor rectangle type. */
	typedef struct LSG_RECT {
		LSINT32										left;											/**< The x-coordinate of the upper-left corner of the rectangle. */
		LSINT32										top;											/**< The y-coordinate of the upper-left corner of the rectangle. */
		LSUINT32									right;											/**< The x-coordinate of the lower-right corner of the rectangle. */
		LSUINT32									bottom;											/**< The y-coordinate of the lower-right corner of the rectangle. */
	} * LPLSG_RECT, * const LPCLSG_RECT;

	/** The input layout type. */
	typedef LSVOID *								LSG_INPUT_LAYOUT;

	/** The vertex buffer type. */
	typedef struct LSG_VERTEX_BUFFER {
		const CVertexBuffer *						pvbVertBuffer;									/**< The vertex buffer pointer. */
		LSUINT32									ui32Offset;										/**< Offset of the vertex buffer. */
	} * LPLSG_VERTEX_BUFFER, * const LPCLSG_VERTEX_BUFFER;
	
	/** The render-pass descriptor. */
	typedef struct LSG_RENDER_PASS_DESCRIPTOR {
		struct LSG_DISPLAY_BUFFER {
			LSBOOL									bClear;											/**< Clear on load? */
			LSGREAL									fClearColor[4];									/**< Clear color. */
			LSGREAL									fClearDepth;									/**< Value to which to clear the depth buffer. */
			LSUINT32								ui32DepthPlane;									/**< Depth plane. */
			LSUINT32								ui32Level;										/**< Level of the texture. */
			LSUINT32								ui32ResolveDepthPlane;							/**< Resolve depth plane. */
			LSUINT32								ui32ResolveLevel;								/**< Resolve level. */
			LSUINT32								ui32ResolveSlice;								/**< Resolve slice. */
			LSUINT32								ui32Slice;										/**< Slice. */
			LSG_TEXTURE_2D *						tTexture;										/**< Texture to load. */
			LSUINT8									ui8ClearStencil;								/**< Value to which to clear the stencil buffer. */
			
			
			// == Various constructors.
			LSE_CALLCTOR							LSG_DISPLAY_BUFFER() :
				bClear( true ),
				fClearDepth( 1.0f ),
				ui32DepthPlane( 0 ),
				ui32Level( 0 ),
				ui32ResolveDepthPlane( 0 ),
				ui32ResolveLevel( 0 ),
				ui32ResolveSlice( 0 ),
				ui32Slice( 0 ),
				tTexture( NULL ),
				ui8ClearStencil( 1 ) {
				fClearColor[0] = fClearColor[1] = fClearColor[2] = 0.0f;
				fClearColor[3] = 1.0f;
			}
		}											dbColors[LSG_MAX_RENDER_TARGET_COUNT];
		LSG_DISPLAY_BUFFER							dbDepth;
		LSG_DISPLAY_BUFFER							dbStencil;
	} * LPLSG_RENDER_PASS_DESCRIPTOR, * const LPCLSG_RENDER_PASS_DESCRIPTOR;

	// Create the render-state structure.
#include "../Gfx/LSGGfxState.h"

	/**
	 * Class CMetal
	 * \brief Interface with the Metal API.
	 *
	 * Description: Interface with the Metal API.
	 */
	class CMetal : public CGfxBase {
		friend class								CMetalFramebufferBase;
		friend class								CMetalIndexBuffer;
		friend class								CMetalTexture2d;
		friend class								CMetalVertexBuffer;
	public :
		// == Types.
		
		
		// == Functions.
		/**
		 * Initialize Metal.
		 *
		 * \param _pvParm Varies by operating system.
		 */
		static LSBOOL LSE_CALL						InitMetal( LSVOID * _pvParm );

		/**
		 * Secondary initialization of Metal, after a context has been created.
		 */
		static LSVOID LSE_CALL						PostInitMetal();

		/**
		 * Destroy the Metal render window.
		 */
		static LSVOID LSE_CALL						DestroyMetal();
		
		/**
		 * Sets the Metal context.
		 *
		 * \param _pvContext The context to set as active.
		 */
		static LSE_INLINE LSVOID LSE_CALL			SetDevice( LSVOID * _pvContext );
		
		/**
		 * Gets the Metal device.
		 *
		 * \return Returns a pointer to the Metal device stored as an LSVOID *.  The pointer can be cast to
		 *	CMetalObject * via (__bridge CMetalObject *).
		 */
		static LSE_INLINE LSVOID * LSE_CALL			GetDevice();

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
		static LSVOID LSE_FCALL						SetShader( CShader * _psShader );

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
		static LSBOOL LSE_FCALL						BeginRender();

		/**
		 * End rendering.  Must be called once per frame only, after all rendering is done.
		 */
		static LSVOID LSE_FCALL						EndRender();

		/**
		 * Present the backbuffer to the screen.
		 */
		static LSVOID LSE_FCALL						Present();

		/**
		 * Creates a perspective projection matrix based on a field of view.
		 *
		 * \param _mIn The input and output matrix.
		 * \param _fFov Field of view in the y direction, in radians.
		 * \param _fAspect Aspect ratio, defined as view space width divided by height.
		 * \param _fNear Z-value of the near view-plane.
		 * \param _fFar Z-value of the far view-plane.
		 * \return Returns the input matrix so that this function can be used as a parameter to another function.
		 */
		static LSE_INLINE CMatrix4x4 & LSE_CALL		PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _tFov, LSREAL _tAspect, LSREAL _tNear, LSREAL _tFar );
		
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
		static LSE_INLINE LSVOID LSE_FCALL			SetDefaultObjectRenderStates();

		/**
		 * Sets all default render states.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetAllDefaultRenderStates();

		/**
		 * Sets and applies all default render states.
		 */
		static LSVOID LSE_CALL						ApplyDefaultStates();

		/**
		 * Applies all current render states.
		 *
		 * \param _bForce If true, all states are applied, even if they are redundant.
		 */
		static LSVOID LSE_CALL						ApplyRenderStates( LSBOOL _bForce = false );

		/**
		 * Clear the active buffer of the given type(s).
		 *
		 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
		 */
		static LSVOID LSE_FCALL						Clear( LSUINT32 _ui32Mask );

		/**
		 * Clear all active buffers.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			Clear();

		/**
		 * Sets the clear color.
		 *
		 * \param _fR Red value.
		 * \param _fG Green value.
		 * \param _fB Blue value.
		 * \param _fA Alpha value.
		 */
		static LSE_INLINE LSVOID LSE_CALL			SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA );

		/**
		 * Sets the depth clear value.
		 *
		 * \param _fValue The new depth clear value.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetClearDepth( LSREAL _fValue );

		/**
		 * Sets the stencil clear value.
		 *
		 * \param _ui8Val The new stencil clear value.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetClearStencil( LSUINT8 _ui8Val );

		/**
		 * Sets the viewport dimensions for the given render target.
		 *
		 * \param _fX X component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fY Y component of the lower-left view rectangle in pixels.  Defaults to 0.
		 * \param _fWidth Width of the viewport.
		 * \param _fHeight Height of the viewport.
		 * \param _ui32Target Index of the viewport to set.
		 */
		static LSE_INLINE LSVOID LSE_CALL			SetViewport( LSREAL _fX, LSREAL _fY,
			LSREAL _fWidth, LSREAL _fHeight, LSUINT32 _ui32Target = 0 );

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
		static LSE_INLINE LSVOID LSE_CALL			SetViewport( LSREAL _fX, LSREAL _fY,
			LSREAL _fWidth, LSREAL _fHeight,
			LSREAL _fMin, LSREAL _fMax, LSUINT32 _ui32Target = 0 );

		/**
		 * Sets the depth range (minimum and maximum Z) of the viewport.
		 *
		 * \param _fMin Minimum Z of the viewport.
		 * \param _fMax Maximum Z of the viewport.
		 * \param _ui32Target Index of the depth range to set.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetDepthRange( LSREAL _fMin, LSREAL _fMax, LSUINT32 _ui32Target = 0 );

		/**
		 * Sets the total number of viewports.
		 *
		 * \param _ui32Total The total number of viewports rectangles set.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetTotalViewports( LSUINT32 _ui32Total );

		
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
		static LSE_INLINE LSUINT32 & LSE_CALL		TotalActiveVertexBuffers();

		/**
		 * Gets the array of current vertex buffers for read and write.
		 *
		 * \return Returns the array of current vertex buffers for read and write.
		 */
		static LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL
													VertexBuffers();

#include "../Gfx/LSGDescFuncDecl.h"
		
	protected :
		// == Types.
		/** Render states specific to Metal. */
		typedef struct LSG_METAL_RENDER_STATE : public LSG_RENDER_STATE {

		} * LPLSG_METAL_RENDER_STATE, * const LPCLSG_METAL_RENDER_STATE;

#include "../Gfx/LSGStateRefDecl.h"


		// == Members.
		/** The shader manager. */
		static CShaderManager *						m_psmShaderMan;
		
#include "../Gfx/LSGCreateStatesMembersDecl.h"
		
		/** Default sampler state. */
		static LSG_SAMPLER_STATE *					m_pssDefaultSamplerState;

		/** Default rasterizer state. */
		static LSG_RASTERIZER_STATE *				m_prsDefaultRasterizerState;

		/** Default blend state. */
		static LSG_BLEND_STATE *					m_pbsDefaultBlendState;

		/** Default depth/stencil state. */
		static LSG_DEPTH_STENCIL_STATE *			m_pdssDefaultDepthStencilState;

		/** The last render state. */
		static LSG_METAL_RENDER_STATE				m_rsLastRenderState;

		/** The current render state. */
		static LSG_METAL_RENDER_STATE				m_rsCurRenderState;
		
		/** Last samplers actually set on the rendering API. */
		static LSG_SAMPLER_STATE					m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS];

		/** Last rasterizer states actually set on the rendering API. */
		static LSG_RASTERIZER_STATE					m_rsLastRaster;

		/** Last blend state actually set on the rendering API. */
		static LSG_BLEND_STATE						m_bsLastBlend;

		/** Last depth/stencil state actually set on the rendering API. */
		static LSG_DEPTH_STENCIL_STATE				m_dssLastDepthStencil;
		
		/** The main Metal device. */
		static LSVOID *								m_pvMetalDevice;
		
		/** The descriptor for clearing render targets. */
		static LSG_RENDER_PASS_DESCRIPTOR			m_rpdClearDesc;
		
		
		// == Functions.
		/**
		 * Release all resources and reset our internal state.
		 *
		 * \param _bResetContext If true, the context is also deleted.
		 */
		static LSVOID LSE_CALL						ResetInternal( LSBOOL _bResetContext = true );
		
		/**
		 * Sets a sampler state in a given slot, avoiding redundant sets.  The index is to the sampler array inside
		 *	m_rsCurRenderState and redundancies are based off the values in m_rsLastRenderState.
		 *
		 * \param _ui32Slot The sampler-state slot from the sampler array in m_rsCurRenderState.
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetSamplerState( LSUINT32 _ui32Slot, LSBOOL _bForce = false );

		/**
		 * Sets sampler states, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetSamplerStates( LSBOOL _bForce = false );

		/**
		 * Sets the rasterizer state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetRasterizerState( LSBOOL _bForce = false );

		/**
		 * Sets the render-target blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetRenderTargetBlendState( LSUINT32 _ui32Slot, LSBOOL _bForce = false );

		/**
		 * Sets the blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetBlendState( LSBOOL _bForce = false );

		/**
		 * Sets the depth-stencil operator state, avoiding redundant sets.
		 *
		 * \param _bFront Front-facing or back-facing.
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetDepthStencilOpState( LSBOOL _bFront, LSBOOL _bForce = false );

		/**
		 * Sets the depth-stencil state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
		 *
		 * \param _bForce Forces changes.
		 */
		static LSE_INLINE LSVOID LSE_FCALL			SetDepthStencilState( LSBOOL _bForce = false );

	};
	
	// == Functions.
	/**
	 * Sets the Metal context.
	 *
	 * \param _pvContext The context to set as active.
	 */
	LSE_INLINE LSVOID LSE_CALL CMetal::SetDevice( LSVOID * _pvContext ) {
		m_pvMetalDevice = _pvContext;
	}
	
	/**
	 * Gets the Metal device.
	 *
	 * \return Returns a pointer to the Metal device stored as an LSVOID *.  The pointer can be cast to
	 *	CMetalObject * via (__bridge CMetalObject *).
	 */
	LSE_INLINE LSVOID * LSE_CALL CMetal::GetDevice() {
		return m_pvMetalDevice;
	}
	
	/**
	 * Gets a pointer to the shader manager.
	 *
	 * \return Returns a pointer to the shader manager used by this class.
	 */
	LSE_INLINE CShaderManager * LSE_FCALL CMetal::GetShaderManager() {
		return m_psmShaderMan;
	}

	/**
	 * Creates a perspective projection matrix based on a field of view.
	 *
	 * \param _mIn The input and output matrix.
	 * \param _fFov Field of view in the y direction, in radians.
	 * \param _fAspect Aspect ratio, defined as view space width divided by height.
	 * \param _fNear Z-value of the near view-plane.
	 * \param _fFar Z-value of the far view-plane.
	 * \return Returns the input matrix so that this function can be used as a parameter to another function.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CMetal::PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _fFov, LSREAL _fAspect, LSREAL _fNear, LSREAL _fFar ) {
		// Zero out the ones that need to be zero.
		_mIn._12 = _mIn._13 = _mIn._14 = LSM_ZERO;
		_mIn._21 = _mIn._23 = _mIn._24 = LSM_ZERO;
		_mIn._31 = _mIn._32 = LSM_ZERO;
		_mIn._41 = _mIn._42 = _mIn._44 = LSM_ZERO;
		
		LSREAL fInvDist = LSM_ONE / (_fNear - _fFar);
		
		// Set the remaining ones.
		_mIn._22 = LSM_ONE / CMathLib::Tan( LSM_HALF * _fFov );
		_mIn._11 = _mIn._22 / _fAspect;

		_mIn._33 = (_fNear + _fFar) * fInvDist;
		_mIn._34 = -LSM_ONE;

		_mIn._43 = (LSM_TWO * _fFar * _fNear) * fInvDist;

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
	LSE_INLINE CMatrix4x4 & LSE_CALL CMetal::OrthoOffCenter( CMatrix4x4 &_mIn,
		LSREAL _fLeft, LSREAL _fRight,
		LSREAL _fBottom, LSREAL _fTop,
		LSREAL _fNear, LSREAL _fFar ) {
		// Right-handed.
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
		_mIn._43 = (_fFar + _fNear) * -_mIn._33;
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
	LSE_INLINE CFrustum & LSE_CALL CMetal::ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize ) {
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
		_fFrustum.m_pNearPlane.n = _mMatrix.GetRow( 3 ) + _mMatrix.GetRow( 2 );
		_fFrustum.m_pNearPlane.dist = -(_mMatrix._44 + _mMatrix._43);

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
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetDefaultObjectRenderStates() {
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
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetAllDefaultRenderStates() {
		// General states.
		SetDefaultObjectRenderStates();

		// Clear values.
		m_rsCurRenderState.fClearColor[0] = m_rsCurRenderState.fClearColor[1] =
			m_rsCurRenderState.fClearColor[2] = m_rsCurRenderState.fClearColor[3] = LSGREAL( 1.0 );
		m_rsCurRenderState.fClearDepth = LSGREAL( 1.0 );
		m_rsCurRenderState.ui8ClearStencil = 0;

		// Viewports and scissors.
		m_rsCurRenderState.ui8TotalViewportsAndScissors = 1;
		for ( LSUINT32 I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
			m_rsCurRenderState.vViewports[I].iX = 0;
			m_rsCurRenderState.vViewports[I].iY = 0;

			m_rsCurRenderState.vViewports[I].sWidth = static_cast<LSUINT32>(m_ui32ScreenWidth);
			m_rsCurRenderState.vViewports[I].sHeight = static_cast<LSUINT32>(m_ui32ScreenHeight);

			m_rsCurRenderState.vViewports[I].fMinZ = static_cast<LSFLOAT>(LSG_MIN_DEPTH);
			m_rsCurRenderState.vViewports[I].fMaxZ = static_cast<LSFLOAT>(LSG_MAX_DEPTH);


			m_rsCurRenderState.rScissors[I].left = 0;
			m_rsCurRenderState.rScissors[I].top = 0;

			m_rsCurRenderState.rScissors[I].right = static_cast<LSUINT32>(m_ui32ScreenWidth);
			m_rsCurRenderState.rScissors[I].bottom = static_cast<LSUINT32>(m_ui32ScreenHeight);
		}

		// Render targets.
		m_rsCurRenderState.ui8TotalRenderTargets = 1;
		for ( LSUINT32 I = 1; I < LSG_MAX_RENDER_TARGET_COUNT; ++I ) {
			m_rsCurRenderState.prtRenderTargets[I] = NULL;
		}
		m_rsCurRenderState.prtRenderTargets[0] = 0;
		m_rsCurRenderState.pdsDepthStencil = 0;

		// Shader resources.
		for ( LSUINT32 I = LSG_MAX_TEXTURE_UNITS; I--; ) {
			m_rsCurRenderState.psrShaderResources[I] = NULL;
		}
		for ( LSUINT32 I = LSG_MAX_SHADER_SAMPLERS; I--; ) {
			m_rsCurRenderState.pssSamplers[I] = m_pssDefaultSamplerState;
		}


		// Vertex buffers.
		for ( LSUINT32 I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			VertexBuffers()[I].pvbVertBuffer = NULL;
			VertexBuffers()[I].ui32Offset = 0;
		}
		m_rsCurRenderState.pilInputLayout = NULL;
		TotalActiveVertexBuffers() = 0;
	}

	/**
	 * Clear all active buffers.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::Clear() {
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
	LSE_INLINE LSVOID LSE_CALL CMetal::SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA ) {
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
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetClearDepth( LSREAL _fValue ) {
		m_rsCurRenderState.fClearDepth = static_cast<LSGREAL>(_fValue);
	}

	/**
	 * Sets the stencil clear value.
	 *
	 * \param _ui8Val The new stencil clear value.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetClearStencil( LSUINT8 _ui8Val ) {
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
	LSE_INLINE LSVOID LSE_CALL CMetal::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight, LSUINT32 _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<LSINT32>(_fX) >= static_cast<LSINT32>(LSG_VIEWPORT_MIN) && static_cast<LSINT32>(_fX) <= static_cast<LSINT32>(LSG_VIEWPORT_MAX) );
		assert( static_cast<LSINT32>(_fY) >= static_cast<LSINT32>(LSG_VIEWPORT_MIN) && static_cast<LSINT32>(_fY) <= static_cast<LSINT32>(LSG_VIEWPORT_MAX) );

		assert( static_cast<LSUINT32>(_fWidth) >= 0.0f && static_cast<LSUINT32>(_fX + _fWidth) <= static_cast<LSUINT32>(LSG_VIEWPORT_MAX) );
		assert( static_cast<LSUINT32>(_fHeight) >= 0.0f && static_cast<LSUINT32>(_fY + _fHeight) <= static_cast<LSUINT32>(LSG_VIEWPORT_MAX) );

		m_rsCurRenderState.vViewports[_ui32Target].iX = static_cast<LSINT32>(_fX);
		m_rsCurRenderState.vViewports[_ui32Target].iY = static_cast<LSINT32>(_fY);
		m_rsCurRenderState.vViewports[_ui32Target].sWidth = static_cast<LSUINT32>(_fWidth);
		m_rsCurRenderState.vViewports[_ui32Target].sHeight = static_cast<LSUINT32>(_fHeight);
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
	LSE_INLINE LSVOID LSE_CALL CMetal::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight,
		LSREAL _fMin, LSREAL _fMax,
		LSUINT32 _ui32Target ) {
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
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetDepthRange( LSREAL _fMin, LSREAL _fMax, LSUINT32 _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<LSFLOAT>(_fMin) >= static_cast<LSFLOAT>(LSG_MIN_DEPTH) && static_cast<LSFLOAT>(_fMin) <= static_cast<LSFLOAT>(LSG_MAX_DEPTH) );
		assert( static_cast<LSFLOAT>(_fMax) >= static_cast<LSFLOAT>(LSG_MIN_DEPTH) && static_cast<LSFLOAT>(_fMax) <= static_cast<LSFLOAT>(LSG_MAX_DEPTH) );

		m_rsCurRenderState.vViewports[_ui32Target].fMinZ = static_cast<LSFLOAT>(_fMin);
		m_rsCurRenderState.vViewports[_ui32Target].fMaxZ = static_cast<LSFLOAT>(_fMax);
	}

	/**
	 * Sets the total number of viewports.
	 *
	 * \param _ui32Total The total number of viewports rectangles set.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetTotalViewports( LSUINT32 _ui32Total ) {
		assert( _ui32Total < LSG_MAX_VIEWPORTS_SCISSORS );

		m_rsCurRenderState.ui8TotalViewportsAndScissors = static_cast<LSUINT8>(_ui32Total);
	}

	/**
	 * Gets the reverse culling for the given input cull mode.
	 *
	 * \param _cmMode The input cull mode whose reverse is to be returned.
	 * \return Returns LSG_CM_FRONT for LSG_CM_BACK, LSG_CM_BACK for LSG_CM_FRONT, and LSG_CM_NONE for LSG_CM_NONE.
	 */
	LSE_INLINE LSG_CULL_MODE LSE_FCALL CMetal::ReverseCull( LSG_CULL_MODE _cmMode ) {
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
	LSE_INLINE LSUINT32 & LSE_CALL CMetal::TotalActiveVertexBuffers() {
		return m_rsCurRenderState.ui32TotalVertexBuffers;
	}

	/**
	 * Gets the array of current vertex buffers for read and write.
	 *
	 * \return Returns the array of current vertex buffers for read and write.
	 */
	LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL CMetal::VertexBuffers() {
		return m_rsCurRenderState.pvbVertexBuffers;
	}
	
	/**
	 * Sets a sampler state in a given slot, avoiding redundant sets.  The index is to the sampler array inside
	 *	m_rsCurRenderState and redundancies are based off the values in m_rsLastRenderState.
	 *
	 * \param _ui32Slot The sampler-state slot from the sampler array in m_rsCurRenderState.
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetSamplerState( LSUINT32 _ui32Slot, LSBOOL _bForce ) {
		// If no texture set and not forced, do nothing.
		if ( !_bForce ) {
			if ( !m_rsCurRenderState.psrShaderResources[_ui32Slot] ) { return; }
		}
		if ( !m_rsCurRenderState.pssSamplers[_ui32Slot] ) {
			m_rsCurRenderState.pssSamplers[_ui32Slot] = m_pssDefaultSamplerState;
		}

		//const LSG_SAMPLER_STATE & ssCurState = (*m_rsCurRenderState.pssSamplers[_ui32Slot]);
		//LSG_SAMPLER_STATE & ssLastState = m_ssLastSamplers[_ui32Slot];

/*#define LSG_CHANGED( MEMBER )	((ssCurState.MEMBER != ssLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	ssLastState.MEMBER = ssCurState.MEMBER*/


/*#undef LSG_UPDATE
#undef LSG_CHANGED*/
	}

	/**
	 * Sets sampler states, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetSamplerStates( LSBOOL _bForce ) {
		for ( LSUINT32 I = CStd::Min<LSUINT32>( m_mMetrics.ui32MaxTexSlot, LSG_MAX_SHADER_SAMPLERS ); I--; ) {
			if ( _bForce || m_rsCurRenderState.psrShaderResources[I] ) {
				SetSamplerState( I, _bForce );
			}
		}
	}

	/**
	 * Sets the rasterizer state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetRasterizerState( LSBOOL _bForce ) {
		if ( !m_rsCurRenderState.prsRasterState ) {
			// NULL states are the same as default states.
			m_rsCurRenderState.prsRasterState = m_prsDefaultRasterizerState;
		}
		// Early out if this state is the same as last time.
		if ( m_rsCurRenderState.prsRasterState == m_rsLastRenderState.prsRasterState && !_bForce ) { return; }
		m_rsLastRenderState.prsRasterState = m_rsCurRenderState.prsRasterState;
		
		/*const LSG_RASTERIZER_STATE & rsCurState = (*m_rsCurRenderState.prsRasterState);
		LSG_RASTERIZER_STATE & rsLastState = m_rsLastRaster;*/

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER



#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the render-target blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetRenderTargetBlendState( LSUINT32 _ui32Slot, LSBOOL _bForce ) {
		/*const LSG_RENDER_TARGET_BLEND_DESC & rtbsCurState = m_rsCurRenderState.pbsBlendState->RenderTarget[_ui32Slot];
		LSG_RENDER_TARGET_BLEND_DESC & rtbsLastState = m_bsLastBlend.RenderTarget[_ui32Slot];*/

#define LSG_CHANGED( MEMBER )	((rtbsCurState.MEMBER != rtbsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rtbsLastState.MEMBER = rtbsCurState.MEMBER

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetBlendState( LSBOOL _bForce ) {
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

		for ( LSUINT32 I = LSG_MAX_RENDER_TARGET_COUNT; I--; ) {
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
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetDepthStencilOpState( LSBOOL _bFront, LSBOOL _bForce ) {
		/*const LSG_DEPTH_STENCILOP_DESC & _dsodCurState = _bFront ? m_rsCurRenderState.pdssDepthStencilState->FrontFace :
			m_rsCurRenderState.pdssDepthStencilState->BackFace;
		LSG_DEPTH_STENCILOP_DESC & _dsodLastState = _bFront ? m_dssLastDepthStencil.FrontFace :
			m_dssLastDepthStencil.BackFace;*/

#define LSG_CHANGED( MEMBER )	((_dsodCurState.MEMBER != _dsodLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	_dsodLastState.MEMBER = _dsodCurState.MEMBER

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the depth-stencil state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE LSVOID LSE_FCALL CMetal::SetDepthStencilState( LSBOOL _bForce ) {
		if ( !m_rsCurRenderState.pdssDepthStencilState ) {
			// NULL states are the same as default states.
			m_rsCurRenderState.pdssDepthStencilState = m_pdssDefaultDepthStencilState;
		}
		// Early out if this state is the same as last time.
		if ( m_rsCurRenderState.pdssDepthStencilState == m_rsLastRenderState.pdssDepthStencilState && !_bForce ) { return; }
		m_rsLastRenderState.pdssDepthStencilState = m_rsCurRenderState.pdssDepthStencilState;
		
		/*const LSG_DEPTH_STENCIL_STATE & dssCurState = (*m_rsCurRenderState.pdssDepthStencilState);
		LSG_DEPTH_STENCIL_STATE & dssLastState = m_dssLastDepthStencil;*/

#define LSG_CHANGED( MEMBER )	((dssCurState.MEMBER != dssLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	dssLastState.MEMBER = dssCurState.MEMBER
	
		//LSUINT8										StencilReadMask;								/**< Identify a portion of the depth-stencil buffer for reading stencil data. */

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

}	// namespace lsg

#endif	// #ifdef LSG_METAL

#endif	// __LSG_METAL_H__
