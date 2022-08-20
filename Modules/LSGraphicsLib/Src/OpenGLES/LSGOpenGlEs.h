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
 * Description: Interface with the OpenGL ES * API.
 */


#ifndef __LSG_OPENGLES_H__
#define __LSG_OPENGLES_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGLES

#include "../Gfx/LSGGfxBase.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "Map/LSTLLinearMap.h"

#ifdef LSE_IPHONE
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#error "Unrecognized OpenGL ES platform."
#endif	// #ifdef LSE_IPHONE

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Enumerations.
	/** The blend types we support.  These are for blending to the backbuffer only. */
	enum LSG_BLEND_MODE {
		LSG_BM_ZERO									= GL_ZERO,										/**< Blend factor is (0, 0, 0, 0). */
		LSG_BM_ONE									= GL_ONE,										/**< Blend factor is (1, 1, 1, 1). */
		LSG_BM_SRC_COLOR							= GL_SRC_COLOR,									/**< Blend factor is (Rs, Gs, Bs, As). */
		LSG_BM_INV_SRC_COLOR						= GL_ONE_MINUS_SRC_COLOR,						/**< Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		LSG_BM_DST_COLOR							= GL_DST_COLOR,									/**< Blend factor is (Rd, Gd, Bd, Ad). */
		LSG_BM_INV_DST_COLOR						= GL_ONE_MINUS_DST_COLOR,						/**< Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		LSG_BM_SRC_ALPHA							= GL_SRC_ALPHA,									/**< Blend factor is (As, As, As, As). */
		LSG_BM_INV_SRC_ALPHA						= GL_ONE_MINUS_SRC_ALPHA,						/**< Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		LSG_BM_DST_ALPHA							= GL_DST_ALPHA,									/**< Blend factor is (Ad Ad Ad Ad). */
		LSG_BM_INV_DEST_ALPHA						= GL_ONE_MINUS_DST_ALPHA,						/**< Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		LSG_BM_SRC_ALPHA_SAT						= GL_SRC_ALPHA_SATURATE,						/**< Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */

		LSG_BM_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** The blend operations we support. */
	enum LSG_BLEND_OP {
		LSG_BO_ADD									= GL_FUNC_ADD,									/**< Add source 1 and source 2. */
		LSG_BO_SUBTRACT								= GL_FUNC_SUBTRACT,								/**< Subtract source 1 and source 2. */
		LSG_BO_REV_SUBTRACT							= GL_FUNC_REVERSE_SUBTRACT,						/**< Subtract source 2 from source 1. */
		LSG_BO_MIN									= GL_MIN_EXT,									/**< Find the minimum of source 1 and source 2. */
		LSG_BO_MAX									= GL_MAX_EXT,									/**< Find the maximum of source 1 and source 2. */

		LSG_BO_FORCE_DWORD							= 0x7FFFFFFF
	};

	/** Buffers. */
	enum LSG_BUFFER {
		LSG_B_COLOR_BUFFER							= GL_COLOR_BUFFER_BIT,							/**< Clear a render target. */
		LSG_B_STENCIL_BUFFER						= GL_STENCIL_BUFFER_BIT,						/**< Clear the stencil buffer. */
		LSG_B_DEPTH_BUFFER							= GL_DEPTH_BUFFER_BIT,							/**< Clear the depth buffer. */
		LSG_B_ALL									= LSG_B_COLOR_BUFFER | LSG_B_STENCIL_BUFFER |
			LSG_B_DEPTH_BUFFER,																		/**< Clear all buffers. */
	};

	/** The color-write modes. */
	enum LSG_COLOR_WRITE_ENABLE {
		LSG_CWM_RED									= 1 << 0,										/**< Allow data to be stored in the red component. */
		LSG_CWM_GREEN								= 1 << 1,										/**< Allow data to be stored in the green component. */
		LSG_CWM_BLUE								= 1 << 2,										/**< Allow data to be stored in the blue component. */
		LSG_CWM_ALPHA								= 1 << 3,										/**< Allow data to be stored in the alpha component. */
		LSG_CWM_ALL									= LSG_CWM_RED | LSG_CWM_GREEN | LSG_CWM_BLUE |
			LSG_CWM_ALPHA,																			/**< Allow data to be stored in all components. */
	};

	/** Comparison functions. */
	enum LSG_COMPARISON_FUNC {
		LSG_CF_NEVER								= GL_NEVER,										/**< Never pass the comparison. */
		LSG_CF_ALWAYS								= GL_ALWAYS,									/**< Always pass the comparison. */
		LSG_CF_LESS									= GL_LESS,										/**< If the source data is less than the destination data, the comparison passes. */
		LSG_CF_EQUAL								= GL_EQUAL,										/**< If the source data is equal to the destination data, the comparison passes. */
		LSG_CF_NOTEQUAL								= GL_NOTEQUAL,									/**< If the source data is not equal to the destination data, the comparison passes. */
		LSG_CF_LESSEQUAL							= GL_LEQUAL,									/**< If the source data is less than or equal to the destination data, the comparison passes. */
		LSG_CF_GREATER								= GL_GREATER,									/**< If the source data is greater than the destination data, the comparison passes. */
		LSG_CF_GREATEREQUAL							= GL_GEQUAL,									/**< If the source data is greater than or equal to the destination data, the comparison passes. */
	};

	/** Culling modes. */
	enum LSG_CULL_MODE {
		LSG_CM_NONE									= 0,											/**< No culling. */
		LSG_CM_FRONT								= GL_FRONT,										/**< Cull front faces. */
		LSG_CM_BACK									= GL_BACK,										/**< Cull back faces. */
	};

	/** Depth write masks. */
	enum LSG_DEPTH_WRITE_MASK {
		LSG_DRM_ZERO								= 0,											/**< Turn off writes to the depth-stencil buffer. */
		LSG_DRM_ALL									= 1,											/**< Turn on writes to the depth-stencil buffer. */
	};

	/** Fill modes. */
	enum LSG_FILL_MODE {
		LSG_FM_SOLID								= 0,											/**< Solid filling mode. */
		LSG_FM_WIRE									= 1,											/**< Wire-frame filling mode. */
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
		LSG_SO_KEEP									= GL_KEEP,										/**< Keep the existing stencil data. */
		LSG_SO_ZERO									= GL_ZERO,										/**< Set the stencil data to 0. */
		LSG_SO_REPLACE								= GL_REPLACE,									/**< Set the stencil data to the reference value. */
		LSG_SO_INC_SAT								= GL_INCR,										/**< Increment the stencil value by 1, and clamp the result. */
		LSG_SO_DEC_SAT								= GL_DECR,										/**< Decrement the stencil value by 1, and clamp the result. */
		LSG_SO_INVERT								= GL_INVERT,									/**< Invert the stencil data. */
		LSG_SO_INC									= GL_INCR_WRAP,									/**< Increment the stencil value by 1, and wrap the result if necessary. */
		LSG_SO_DEC									= GL_DECR_WRAP,									/**< Decrement the stencil value by 1, and wrap the result if necessary. */
	};

	/** Texture addressing modes. */
	enum LSG_TEXTURE_ADDRESS_MODE {
		LSG_TAM_WRAP								= GL_REPEAT,									/**< Tile the texture at every (u,v) integer junction. */
		LSG_TAM_MIRROR								= GL_MIRRORED_REPEAT,							/**< Flip the texture at every (u,v) integer junction. */
		LSG_TAM_CLAMP								= GL_CLAMP_TO_EDGE,								/**< Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		LSG_TAM_BORDER								= 0,											/**< Texture coordinates outside the range [0.0, 1.0] are set to the border color. */
		LSG_TAM_MIRROR_ONCE							= 0,											/**< Similar to LSG_TAM_MIRROR and LSG_TAM_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around 0), and then clamps to the maximum value. */
	};

	/** Compile-time metrics.  For run-time, use the metrics structure returned by the graphics device. */
	enum LSG_METRICS {
		LSG_MAX_TEXTURE_UNITS						= 32,											/**< Total texture units bindable to a shader at a time. */
		LSG_MAX_SHADER_SAMPLERS						= 32,											/**< Total number of samplers that can be set on a shader at once. */
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
	typedef class COpenGlEsShaderManager			CShaderManager;
	class											CShader;
	class											CVertexBuffer;

#include "../Gfx/LSGDescStructs.h"

	/** The 1D texture type. */
	typedef GLuint									LSG_TEXTURE_1D;

	/** The 2D texture type. */
	typedef GLuint									LSG_TEXTURE_2D;

	/** The 3D texture type. */
	typedef GLuint									LSG_TEXTURE_3D;

	/** The sampler type. */
	typedef LSG_SAMPLER_DESC						LSG_SAMPLER_STATE;

	/** The shader resource view type. */
	typedef GLuint									LSG_SHADER_RESOURCE;

	/** The render target type. */
	typedef GLuint									LSG_RENDER_TARGET;

	/** The depth/stencil type. */
	typedef GLuint									LSG_DEPTH_STENCIL;

	/** The rasterizer state type. */
	typedef LSG_RASTERIZER_DESC						LSG_RASTERIZER_STATE;

	/** The blend state type. */
	typedef LSG_BLEND_DESC							LSG_BLEND_STATE;

	/** The depth/stencil state. */
	typedef LSG_DEPTH_STENCIL_DESC					LSG_DEPTH_STENCIL_STATE;

	/** The viewport type. */
	typedef struct LSG_VIEWPORT {
		GLint										iX;												/**< The left coordinate of the viewport in pixels. */
		GLint										iY;												/**< The upper coordinate of the viewport in pixels. */
		GLsizei										sWidth;											/**< Width of the viewport in pixels. */
		GLsizei										sHeight;										/**< Height of the viewport in pixels. */
		GLfloat										fMinZ;											/**< Together with MaxZ, value describing the range of depth values into which a scene is to be rendered, the minimum and maximum values of the clip volume. */
		GLfloat										fMaxZ;											/**< Together with MinZ, value describing the range of depth values into which a scene is to be rendered, the minimum and maximum values of the clip volume. */
	} * LPLSG_VIEWPORT, * const LPCLSG_VIEWPORT;

	/** The scissor rectangle type. */
	typedef struct LSG_RECT {
		GLint										left;											/**< The x-coordinate of the upper-left corner of the rectangle. */
		GLint										top;											/**< The y-coordinate of the upper-left corner of the rectangle. */
		GLsizei										right;											/**< The x-coordinate of the lower-right corner of the rectangle. */
		GLsizei										bottom;											/**< The y-coordinate of the lower-right corner of the rectangle. */
	} * LPLSG_RECT, * const LPCLSG_RECT;

	/** The input layout type. */
	typedef void *								LSG_INPUT_LAYOUT;

	/** The vertex buffer type. */
	typedef struct LSG_VERTEX_BUFFER {
		const CVertexBuffer *						pvbVertBuffer;									/**< The vertex buffer pointer. */
		uint32_t									ui32Offset;										/**< Offset of the vertex buffer. */
	} * LPLSG_VERTEX_BUFFER, * const LPCLSG_VERTEX_BUFFER;

	// Create the render-state structure.
#include "../Gfx/LSGGfxState.h"

	/**
	 * Class COpenGlEs
	 * \brief Interface with the OpenGL ES * API.
	 *
	 * Description: Interface with the OpenGL ES * API.
	 */
	class COpenGlEs : public CGfxBase {
		friend class								COpenGlEsFramebuffer;
		friend class								COpenGlEsIndexBuffer;
		friend class								COpenGlEsTexture2d;
		friend class								COpenGlEsVertexBuffer;
	public :
		// == Types.
		/** A helper for temporarily binding a 2D texture (for creating it, modifying it, etc.) */
		class CTempTexture2dBinder {
		public :
			// == Various contructors.
			LSE_CALLCTOR							CTempTexture2dBinder( GLuint _uiId );
			LSE_CALLCTOR							~CTempTexture2dBinder();


		protected :
			// == Members.
			/** The last bound texture when this instance was created. */
			GLuint									m_uiLastTexture;
		};
		
		/** A helper for temporarily binding a framebuffer. */
		class CTempFramebufferBinder {
		public :
			// == Various contructors.
			LSE_CALLCTOR							CTempFramebufferBinder( GLuint _uiId );
			LSE_CALLCTOR							~CTempFramebufferBinder();


		protected :
			// == Members.
			/** The last bound framebuffer when this instance was created. */
			GLuint									m_uiLastFramebuffer;
		};
		
		
		// == Functions.
		/**
		 * Initialize OpenGL ES.
		 *
		 * \param _pvParm Varies by operating system.
		 */
		static LSBOOL LSE_CALL						InitOpenGlEs( void * _pvParm );

		/**
		 * Secondary initialization of OpenGL ES, after a context has been created.
		 */
		static void LSE_CALL						PostInitOpenGlEs();

		/**
		 * Destroy the OpenGL ES render window.
		 */
		static void LSE_CALL						DestroyOpenGlEs();
		
		/**
		 * Sets the OpenGL ES context.
		 *
		 * \param _pvContext The context to set as active.
		 */
		static LSE_INLINE void LSE_CALL			SetContext( void * _pvContext );
		
		/**
		 * Sets the back framebuffer and renderbuffer.
		 *
		 * \param _uiFramebuffer The back framebuffer.
		 * \param _uiRenderbuffer The back renderbuffer.
		 */
		static LSE_INLINE void LSE_CALL			SetBackBuffers( GLuint _uiFramebuffer, GLuint _uiRenderbuffer );

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
		static void LSE_FCALL						Present();

		/**
		 * Make this OpenGL context current.
		 */
		static void LSE_CALL						MakeCurrent();

		/**
		 * In debug mode, this checks and prints any OpenGL errors.
		 *
		 * \param _pcHeader Header to be printed with any error messages returned by ::glGetError().
		 * \param _pcFile File in which the error was detected.
		 * \param _ui32Line Line in the file in which the error was detected.
		 * \return Returns true if there were any errors reported.
		 */
		static LSBOOL LSE_CALL						WarnErrorEx( const char * _pcHeader, const char * _pcFile, uint32_t _ui32Line );
#ifdef _DEBUG
#define glWarnError( _pcHeader )					COpenGlEs::WarnErrorEx( _pcHeader, __FILE__, __LINE__ )
#define glAssertError( _pcHeader )					assert( !COpenGlEs::WarnErrorEx( _pcHeader, __FILE__, __LINE__ ) )
#else
#define glWarnError( _pcHeader )					true
#define glAssertError( _pcHeader )
#endif	// #ifdef _DEBUG
#define glWarnErrorAlways( _pcHeader )				COpenGlEs::WarnErrorEx( _pcHeader, __FILE__, __LINE__ )

		/**
		 * Returns the string form of an error code returned by glCheckFramebufferStatus().
		 */
		static const LSE_INLINE GLchar *			FramebufferStatusToString( GLenum _eStatus );

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
		 * Activates the given texture slot.
		 *
		 * \param _ui32Slot The slot to activate.
		 */
		static LSE_INLINE void LSE_CALL			ActivateTexture( uint32_t _ui32Slot );
	
		/**
		 * Creates a framebuffer and returns its ID.
		 *
		 * \return Returns the ID of the created framebuffer.
		 */
		static LSE_INLINE GLuint					GenFramebuffer();
		
		/**
		 * Binds a framebuffer if it is not already bound.
		 *
		 * \param _uiId ID of the framebuffer to bind.
		 * \param _bForce If true, the bind is forced.
		 */
		static LSE_INLINE void					BindFramebuffer( GLuint _uiId, LSBOOL _bForce = false );
		
		/**
		 * Deletes the given framebuffer.
		 *
		 * \param _uiId ID of the framebuffer to delete.
		 */
		static LSE_INLINE void					DeleteFramebuffer( GLuint _uiId );
		
		/**
		 * Gets the last framebuffer set.
		 *
		 * \return Returns the ast framebuffer set.
		 */
		static LSE_INLINE GLuint					LastFramebuffer();
		
		/**
		 * Creates a renderbuffer and returns its ID.
		 *
		 * \return Returns the ID of the created renderbuffer.
		 */
		static LSE_INLINE GLuint					GenRenderbuffer();
		
		/**
		 * Deletes the given renderbuffer.
		 *
		 * \param _uiId ID of the renderbuffer to delete.
		 */
		static LSE_INLINE void					DeleteRenderbuffer( GLuint _uiId );
		
		/**
		 * Binds a renderbuffer if it is not already bound.
		 *
		 * \param _uiId ID of the renderbuffer to bind.
		 * \param _bForce If true, the bind is forced.
		 */
		static LSE_INLINE void					BindRenderbuffer( GLuint _uiId, LSBOOL _bForce = false );
		
		/**
		 * Gets the last renderbuffer set.
		 *
		 * \return Returns the ast renderbuffer set.
		 */
		static LSE_INLINE GLuint					LastRenderbuffer();
		
		/**
		 * Generates a texture ID.
		 *
		 * \return Returns the generated texture ID.
		 */
		static LSE_INLINE GLuint LSE_CALL			GenTexture();

		/**
		 * Deletes a texture.
		 *
		 * \param _uiId The OpenGL texture ID to delete.
		 */
		static LSE_INLINE void LSE_CALL			DeleteTexture( GLuint &_uiId );

		/**
		 * Binds a texture 2D to a slot.
		 *
		 * \param _uiId The texture to bind.
		 * \param _ui32Slot The slot into which to bind the texture.
		 */
		static LSE_INLINE void LSE_CALL			BindTexture2d( GLuint _uiId, uint32_t _ui32Slot );

		/**
		 * Gets the ID of the texture bound to the given slot.
		 *
		 * \param _ui32Slot The slot.
		 * \return Returns the OpenGL ID of the texture bound to _ui32Slot.
		 */
		static LSE_INLINE GLuint LSE_CALL			TextureBinding2d( uint32_t _ui32Slot );
		
		/**
		 * Enables or disables a vertex attribute.
		 *
		 * \param _uiIndex The index of the attribute to enable or disable.
		 * \param _bValue The enables the attribute if non-zero, disables it otherwise.
		 */
		static LSE_INLINE void LSE_CALL				SetVertexAttribArray( GLuint _uiIndex, GLboolean _bValue );

		/**
		 * Binds a vertex buffer.
		 *
		 * \param _uiVbo The ID of the vertex buffer to bind.
		 */
		static LSE_INLINE void LSE_CALL				BindVertexBuffer( GLuint _uiVbo );

		/**
		 * Binds an index buffer.
		 *
		 * \param _uiIbo The ID of the index buffer to bind.
		 */
		static LSE_INLINE void LSE_CALL				BindIndexBuffer( GLuint _uiIbo );

#include "../Gfx/LSGDescFuncDecl.h"
		
		
		// == Members.
		/** The ::glGetError() critical section. */
		static CCriticalSection						m_csCrit;
		
		/** The critical section for binding textures. */
		static CCriticalSection						m_csBindTextureCrit;

		/** The critical section for binding framebuffers. */
		static CCriticalSection						m_csFramebufferCrit;
		
	protected :
		// == Types.
		/** Render states specific to OpenGL. */
		typedef struct LSG_OPENGL_RENDER_STATE : public LSG_RENDER_STATE {
			GLenum									eActiveTexture;									/**< Last call to glActiveTexture(). */
			GLuint									uiTexture2d[LSG_MAX_TEXTURE_UNITS];				/**< List of active 2D OpenGL textures. */
			GLuint									uiFramebuffer;									/**< Last framebuffer set. */
			GLuint									uiRenderbuffer;									/**< Last renderbuffer set. */
			GLuint									uiMipLevels[LSG_MAX_TEXTURE_UNITS];				/**< Number of mipmap levels on the active slot. */
			GLboolean								bVertAttr[LSG_MAX_VERTEX_ELEMENT_COUNT];		/**< Enabled vertex attributes. */
			GLuint									uiMaxVertAttrib;								/**< Highest attribute set. */
			GLuint									uiVertexBuffer;									/**< Last vertex buffer set. */
			GLuint									uiIndexBuffer;									/**< Last index buffer set. */
		} * LPLSG_OPENGL_RENDER_STATE, * const LPCLSG_OPENGL_RENDER_STATE;

#include "../Gfx/LSGStateRefDecl.h"


		// == Members.
		/** The main OpenGL ES framebuffer. */
		static GLuint								m_uiBackFramebuffer;
		
		/** The main OpenGL ES renderbuffer. */
		static GLuint								m_uiBackRenderbuffer;
		
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
		static LSG_OPENGL_RENDER_STATE				m_rsLastRenderState;

		/** The current render state. */
		static LSG_OPENGL_RENDER_STATE				m_rsCurRenderState;
		
		/** Last samplers actually set on the rendering API. */
		static LSG_SAMPLER_STATE					m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS];

		/** Last rasterizer states actually set on the rendering API. */
		static LSG_RASTERIZER_STATE					m_rsLastRaster;

		/** Last blend state actually set on the rendering API. */
		static LSG_BLEND_STATE						m_bsLastBlend;

		/** Last depth/stencil state actually set on the rendering API. */
		static LSG_DEPTH_STENCIL_STATE				m_dssLastDepthStencil;
		
		/** The main OpenGL ES context. */
		static void *								m_pvContext0;
		
		
		// == Functions.
		/**
		 * Determine if an extension is present.
		 *
		 * \param _pcExt The name of the extension for which to check.
		 * \return Returns true if the given extension is supported.
		 */
		static LSBOOL LSE_CALL						HasExtension( const char * _pcExt );

		/**
		 * Release all resources and reset our internal state.
		 *
		 * \param _bResetContext If true, the context is also deleted.
		 */
		static void LSE_CALL						ResetInternal( LSBOOL _bResetContext = true );
		
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
		
		/**
		 * Updates vertex attributes for OpneGL ES.
		 */
		static LSE_INLINE void LSE_CALL			FinalizeVertAttrib();

	};
	
	// == Functions.
	/**
	 * Sets the OpenGL ES context.
	 *
	 * \param _pvContext The context to set as active.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::SetContext( void * _pvContext ) {
		m_pvContext0 = _pvContext;
	}
	
	/**
	 * Sets the back framebuffer and renderbuffer.
	 *
	 * \param _uiFramebuffer The back framebuffer.
	 * \param _uiRenderbuffer The back renderbuffer.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::SetBackBuffers( GLuint _uiFramebuffer, GLuint _uiRenderbuffer ) {
		m_uiBackFramebuffer = _uiFramebuffer;
		m_uiBackRenderbuffer = _uiRenderbuffer;
	}
	
	/**
	 * Gets a pointer to the shader manager.
	 *
	 * \return Returns a pointer to the shader manager used by this class.
	 */
	LSE_INLINE CShaderManager * LSE_FCALL COpenGlEs::GetShaderManager() {
		return m_psmShaderMan;
	}
	
	/**
	 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
	 *
	 * \return Returns true if the rendering device is ready.
	 */
	LSE_INLINE LSBOOL LSE_FCALL COpenGlEs::BeginRender() {
		return true;
	}

	/**
	 * End rendering.  Must be called once per frame only, after all rendering is done.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::EndRender() {
	}
	
	/**
	 * Returns the string form of an error code returned by glCheckFramebufferStatus().
	 */
	const LSE_INLINE GLchar * COpenGlEs::FramebufferStatusToString( GLenum _eStatus ) {
		if ( _eStatus >= GL_FRAMEBUFFER_COMPLETE && _eStatus <= GL_FRAMEBUFFER_UNSUPPORTED ) {
			static const GLchar * pcNames[] = {
				"GL_FRAMEBUFFER_COMPLETE",
				"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT",
				"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT",
				"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS",
				"GL_FRAMEBUFFER_UNSUPPORTED",
			};
			return pcNames[_eStatus-GL_FRAMEBUFFER_COMPLETE];
		}
		return "Unknown framebuffer error";
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
	LSE_INLINE CMatrix4x4 & LSE_CALL COpenGlEs::PerspectiveFov( CMatrix4x4 &_mIn, LSREAL _fFov, LSREAL _fAspect, LSREAL _fNear, LSREAL _fFar ) {
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
	LSE_INLINE CMatrix4x4 & LSE_CALL COpenGlEs::OrthoOffCenter( CMatrix4x4 &_mIn,
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
	LSE_INLINE CFrustum & LSE_CALL COpenGlEs::ExtractFrustumPlanes( CMatrix4x4 &_mMatrix, CFrustum &_fFrustum, LSBOOL _bNormalize ) {
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
	LSE_INLINE void LSE_FCALL COpenGlEs::SetDefaultObjectRenderStates() {
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
	LSE_INLINE void LSE_FCALL COpenGlEs::SetAllDefaultRenderStates() {
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
			m_rsCurRenderState.vViewports[I].iX = 0;
			m_rsCurRenderState.vViewports[I].iY = 0;

			m_rsCurRenderState.vViewports[I].sWidth = static_cast<GLsizei>(m_ui32ScreenWidth);
			m_rsCurRenderState.vViewports[I].sHeight = static_cast<GLsizei>(m_ui32ScreenHeight);

			m_rsCurRenderState.vViewports[I].fMinZ = static_cast<GLfloat>(LSG_MIN_DEPTH);
			m_rsCurRenderState.vViewports[I].fMaxZ = static_cast<GLfloat>(LSG_MAX_DEPTH);


			m_rsCurRenderState.rScissors[I].left = 0;
			m_rsCurRenderState.rScissors[I].top = 0;

			m_rsCurRenderState.rScissors[I].right = static_cast<GLsizei>(m_ui32ScreenWidth);
			m_rsCurRenderState.rScissors[I].bottom = static_cast<GLsizei>(m_ui32ScreenHeight);
		}

		// Render targets.
		m_rsCurRenderState.ui8TotalRenderTargets = 1;
		for ( uint32_t I = 1; I < LSG_MAX_RENDER_TARGET_COUNT; ++I ) {
			m_rsCurRenderState.prtRenderTargets[I] = NULL;
		}
		m_rsCurRenderState.prtRenderTargets[0] = 0;
		m_rsCurRenderState.pdsDepthStencil = 0;

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
	 * Clear the active buffer of the given type(s).
	 *
	 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::Clear( uint32_t _ui32Mask ) {
		if ( !m_bActive ) { return; }
		glWarnError( "Uncaught" );
#define LSG_COMP( MEM )		(m_rsCurRenderState.MEM != m_rsLastRenderState.MEM)
#define LSG_COPY( MEM )		m_rsLastRenderState.MEM = m_rsCurRenderState.MEM
		// Update stencil clear.
		if ( _ui32Mask & LSG_B_STENCIL_BUFFER ) {
			if ( LSG_COMP( ui8ClearStencil ) ) {
				::glClearStencil( m_rsCurRenderState.ui8ClearStencil );
				LSG_COPY( ui8ClearStencil );
			}
		}
		// Update depth clear.
		if ( _ui32Mask & LSG_B_DEPTH_BUFFER ) {
			if ( LSG_COMP( fClearDepth ) ) {
				::glClearDepthf( m_rsCurRenderState.fClearDepth );
				LSG_COPY( fClearDepth );
				if ( m_rsCurRenderState.pdssDepthStencilState &&
					m_rsCurRenderState.pdssDepthStencilState->DepthWriteMask != LSG_DRM_ALL ) {
					// Depth-write needs to be enabled.
					::glDepthMask( GL_TRUE );
				}
			}
		}
		// Update clear color.
		if ( _ui32Mask & LSG_B_COLOR_BUFFER ) {
			if ( LSG_COMP( fClearColor[0] ) || LSG_COMP( fClearColor[1] ) ||
				LSG_COMP( fClearColor[2] ) || LSG_COMP( fClearColor[3] ) ) {
				::glClearColor( m_rsCurRenderState.fClearColor[0],
					m_rsCurRenderState.fClearColor[1],
					m_rsCurRenderState.fClearColor[2],
					m_rsCurRenderState.fClearColor[3] );
				LSG_COPY( fClearColor[0] );
				LSG_COPY( fClearColor[1] );
				LSG_COPY( fClearColor[2] );
				LSG_COPY( fClearColor[3] );
			}
		}
		::glClear( _ui32Mask );
		glWarnError( "Invalid clear mask" );

		if ( _ui32Mask & LSG_B_DEPTH_BUFFER ) {
			if ( m_rsCurRenderState.pdssDepthStencilState &&
				m_rsCurRenderState.pdssDepthStencilState->DepthWriteMask != LSG_DRM_ALL ) {
				// Depth-write needs to be enabled.
				::glDepthMask( GL_FALSE );
			}
		}

#undef LSG_COPY
#undef LSG_COMP
	}

	/**
	 * Clear all active buffers.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::Clear() {
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
	LSE_INLINE void LSE_CALL COpenGlEs::SetClearColor( LSREAL _fR, LSREAL _fG, LSREAL _fB, LSREAL _fA ) {
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
	LSE_INLINE void LSE_FCALL COpenGlEs::SetClearDepth( LSREAL _fValue ) {
		m_rsCurRenderState.fClearDepth = static_cast<LSGREAL>(_fValue);
	}

	/**
	 * Sets the stencil clear value.
	 *
	 * \param _ui8Val The new stencil clear value.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetClearStencil( uint8_t _ui8Val ) {
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
	LSE_INLINE void LSE_CALL COpenGlEs::SetViewport( LSREAL _fX, LSREAL _fY,
		LSREAL _fWidth, LSREAL _fHeight, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<GLint>(_fX) >= static_cast<GLint>(LSG_VIEWPORT_MIN) && static_cast<GLint>(_fX) <= static_cast<GLint>(LSG_VIEWPORT_MAX) );
		assert( static_cast<GLint>(_fY) >= static_cast<GLint>(LSG_VIEWPORT_MIN) && static_cast<GLint>(_fY) <= static_cast<GLint>(LSG_VIEWPORT_MAX) );

		assert( static_cast<GLsizei>(_fWidth) >= 0.0f && static_cast<GLsizei>(_fX + _fWidth) <= static_cast<GLsizei>(LSG_VIEWPORT_MAX) );
		assert( static_cast<GLsizei>(_fHeight) >= 0.0f && static_cast<GLsizei>(_fY + _fHeight) <= static_cast<GLsizei>(LSG_VIEWPORT_MAX) );

		m_rsCurRenderState.vViewports[_ui32Target].iX = static_cast<GLint>(_fX);
		m_rsCurRenderState.vViewports[_ui32Target].iY = static_cast<GLint>(_fY);
		m_rsCurRenderState.vViewports[_ui32Target].sWidth = static_cast<GLsizei>(_fWidth);
		m_rsCurRenderState.vViewports[_ui32Target].sHeight = static_cast<GLsizei>(_fHeight);
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
	LSE_INLINE void LSE_CALL COpenGlEs::SetViewport( LSREAL _fX, LSREAL _fY,
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
	LSE_INLINE void LSE_FCALL COpenGlEs::SetDepthRange( LSREAL _fMin, LSREAL _fMax, uint32_t _ui32Target ) {
		assert( _ui32Target < LSG_MAX_VIEWPORTS_SCISSORS );

		assert( static_cast<GLfloat>(_fMin) >= static_cast<GLfloat>(LSG_MIN_DEPTH) && static_cast<GLfloat>(_fMin) <= static_cast<GLfloat>(LSG_MAX_DEPTH) );
		assert( static_cast<GLfloat>(_fMax) >= static_cast<GLfloat>(LSG_MIN_DEPTH) && static_cast<GLfloat>(_fMax) <= static_cast<GLfloat>(LSG_MAX_DEPTH) );

		m_rsCurRenderState.vViewports[_ui32Target].fMinZ = static_cast<GLfloat>(_fMin);
		m_rsCurRenderState.vViewports[_ui32Target].fMaxZ = static_cast<GLfloat>(_fMax);
	}

	/**
	 * Sets the total number of viewports.
	 *
	 * \param _ui32Total The total number of viewports rectangles set.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetTotalViewports( uint32_t _ui32Total ) {
		assert( _ui32Total < LSG_MAX_VIEWPORTS_SCISSORS );

		m_rsCurRenderState.ui8TotalViewportsAndScissors = static_cast<uint8_t>(_ui32Total);
	}

	/**
	 * Gets the reverse culling for the given input cull mode.
	 *
	 * \param _cmMode The input cull mode whose reverse is to be returned.
	 * \return Returns LSG_CM_FRONT for LSG_CM_BACK, LSG_CM_BACK for LSG_CM_FRONT, and LSG_CM_NONE for LSG_CM_NONE.
	 */
	LSE_INLINE LSG_CULL_MODE LSE_FCALL COpenGlEs::ReverseCull( LSG_CULL_MODE _cmMode ) {
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
	LSE_INLINE uint32_t & LSE_CALL COpenGlEs::TotalActiveVertexBuffers() {
		return m_rsCurRenderState.ui32TotalVertexBuffers;
	}

	/**
	 * Gets the array of current vertex buffers for read and write.
	 *
	 * \return Returns the array of current vertex buffers for read and write.
	 */
	LSE_INLINE LSG_VERTEX_BUFFER * LSE_CALL COpenGlEs::VertexBuffers() {
		return m_rsCurRenderState.pvbVertexBuffers;
	}
	
	/**
	 * Activates the given texture slot.
	 *
	 * \param _ui32Slot The slot to activate.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::ActivateTexture( uint32_t _ui32Slot ) {
		assert( _ui32Slot < m_mMetrics.ui32MaxTexSlot );
		if ( m_rsCurRenderState.eActiveTexture != static_cast<GLenum>(_ui32Slot) ) {
			glWarnError( "Uncaught" );

			m_rsCurRenderState.eActiveTexture = static_cast<GLenum>(_ui32Slot);
			::glActiveTexture( m_rsCurRenderState.eActiveTexture + GL_TEXTURE0 );

			glWarnError( "COpenGlEs::ActivateTexture() failed" );
		}
	}
	
	/**
	 * Creates a framebuffer and returns its ID.
	 *
	 * \return Returns the ID of the created framebuffer.
	 */
	LSE_INLINE GLuint COpenGlEs::GenFramebuffer() {
		GLuint uiRet;
		::glGenFramebuffers( 1, &uiRet );
		return uiRet;
	}
	
	/**
	 * Binds a framebuffer if it is not already bound.
	 *
	 * \param _uiId ID of the framebuffer to bind.
	 * \param _bForce If true, the bind is forced.
	 */
	LSE_INLINE void COpenGlEs::BindFramebuffer( GLuint _uiId, LSBOOL _bForce ) {
		if ( _uiId != m_rsCurRenderState.uiFramebuffer || _bForce ) {
			::glBindFramebuffer( GL_FRAMEBUFFER, _uiId );
			glWarnError( "COpenGlEs::BindFramebuffer()" );
			m_rsCurRenderState.uiFramebuffer = _uiId;
		}
	}
	
	/**
	 * Deletes the given framebuffer.
	 *
	 * \param _uiId ID of the framebuffer to delete.
	 */
	LSE_INLINE void COpenGlEs::DeleteFramebuffer( GLuint _uiId ) {
		::glDeleteFramebuffers( 1, &_uiId );
		if ( m_rsCurRenderState.uiFramebuffer == _uiId ) {
			m_rsCurRenderState.uiFramebuffer = 0;
		}
	}
	
	/**
	 * Gets the last framebuffer set.
	 *
	 * \return Returns the ast framebuffer set.
	 */
	LSE_INLINE GLuint COpenGlEs::LastFramebuffer() {
		return m_rsCurRenderState.uiFramebuffer;
	}
	
	/**
	 * Creates a renderbuffer and returns its ID.
	 *
	 * \return Returns the ID of the created renderbuffer.
	 */
	LSE_INLINE GLuint COpenGlEs::GenRenderbuffer() {
		GLuint uiRet;
		::glGenRenderbuffers( 1, &uiRet );
		return uiRet;
	}
	
	/**
	 * Binds a renderbuffer if it is not already bound.
	 *
	 * \param _uiId ID of the renderbuffer to bind.
	 * \param _bForce If true, the bind is forced.
	 */
	LSE_INLINE void COpenGlEs::BindRenderbuffer( GLuint _uiId, LSBOOL _bForce ) {
		if ( _uiId != m_rsCurRenderState.uiRenderbuffer || _bForce ) {
			::glBindRenderbuffer( GL_RENDERBUFFER, _uiId );
			glWarnError( "COpenGlEs::BindRenderbuffer()" );
			m_rsCurRenderState.uiRenderbuffer = _uiId;
		}
	}
	
	/**
	 * Gets the last renderbuffer set.
	 *
	 * \return Returns the ast renderbuffer set.
	 */
	LSE_INLINE GLuint COpenGlEs::LastRenderbuffer() {
		return m_rsCurRenderState.uiRenderbuffer;
	}
	
	/**
	 * Deletes the given renderbuffer.
	 *
	 * \param _uiId ID of the renderbuffer to delete.
	 */
	LSE_INLINE void COpenGlEs::DeleteRenderbuffer( GLuint _uiId ) {
		::glDeleteRenderbuffers( 1, &_uiId );
		if ( m_rsCurRenderState.uiRenderbuffer == _uiId ) {
			m_rsCurRenderState.uiRenderbuffer = 0;
		}
	}
	
	/**
	 * Generates a texture ID.
	 *
	 * \return Returns the generated texture ID.
	 */
	LSE_INLINE GLuint LSE_CALL COpenGlEs::GenTexture() {
		GLuint uiId;
		::glGenTextures( 1, &uiId );
		return uiId;
	}

	/**
	 * Deletes a texture.
	 *
	 * \param _uiId The OpenGL texture ID to delete.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::DeleteTexture( GLuint &_uiId ) {
		if ( _uiId ) {
			CCriticalSection::CLocker lLock( m_csBindTextureCrit );
			for ( uint32_t I = LSE_ELEMENTS( m_rsCurRenderState.uiTexture2d ); I--; ) {
				if ( m_rsCurRenderState.uiTexture2d[I] == _uiId ) {
					m_rsCurRenderState.uiTexture2d[I] = 0;
				}
			}
			::glDeleteTextures( 1, &_uiId );
			_uiId = 0;
		}
	}

	/**
	 * Binds a texture 2D to a slot.
	 *
	 * \param _uiId The texture to bind.
	 * \param _ui32Slot The slot into which to bind the texture.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::BindTexture2d( GLuint _uiId, uint32_t _ui32Slot ) {
		assert( _ui32Slot < m_mMetrics.ui32MaxTexSlot );
		if ( m_rsCurRenderState.uiTexture2d[_ui32Slot] != _uiId ) {
			CCriticalSection::CLocker lLock( m_csBindTextureCrit );
			ActivateTexture( _ui32Slot );
			::glBindTexture( GL_TEXTURE_2D, _uiId );
			m_rsCurRenderState.uiTexture2d[_ui32Slot] = _uiId;
		}
	}

	/**
	 * Gets the ID of the texture bound to the given slot.
	 *
	 * \param _ui32Slot The slot.
	 * \return Returns the OpenGL ID of the texture bound to _ui32Slot.
	 */
	LSE_INLINE GLuint LSE_CALL COpenGlEs::TextureBinding2d( uint32_t _ui32Slot ) {
		assert( _ui32Slot < m_mMetrics.ui32MaxTexSlot );
		return m_rsCurRenderState.uiTexture2d[_ui32Slot];
	}
	
	/**
	 * Enables or disables a vertex attribute.
	 *
	 * \param _uiIndex The index of the attribute to enable or disable.
	 * \param _bValue The enables the attribute if non-zero, disables it otherwise.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::SetVertexAttribArray( GLuint _uiIndex, GLboolean _bValue ) {
		assert( _uiIndex < LSE_ELEMENTS( m_rsCurRenderState.bVertAttr ) );
		m_rsCurRenderState.bVertAttr[_uiIndex] = _bValue;
		m_rsCurRenderState.uiMaxVertAttrib = CStd::Max( m_rsCurRenderState.uiMaxVertAttrib, _uiIndex + 1 );
	}

	/**
	 * Binds a vertex buffer.
	 *
	 * \param _uiVbo The ID of the vertex buffer to bind.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::BindVertexBuffer( GLuint _uiVbo ) {
		if ( m_rsCurRenderState.uiVertexBuffer != _uiVbo ) {
			m_rsCurRenderState.uiVertexBuffer = _uiVbo;
			::glBindBuffer( GL_ARRAY_BUFFER, _uiVbo );
		}
	}

	/**
	 * Binds an index buffer.
	 *
	 * \param _uiIbo The ID of the index buffer to bind.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::BindIndexBuffer( GLuint _uiIbo ) {
		if ( m_rsCurRenderState.uiIndexBuffer != _uiIbo ) {
			m_rsCurRenderState.uiIndexBuffer = _uiIbo;
			::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _uiIbo );
		}
	}
	
	/**
	 * Sets a sampler state in a given slot, avoiding redundant sets.  The index is to the sampler array inside
	 *	m_rsCurRenderState and redundancies are based off the values in m_rsLastRenderState.
	 *
	 * \param _ui32Slot The sampler-state slot from the sampler array in m_rsCurRenderState.
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetSamplerState( uint32_t _ui32Slot, LSBOOL _bForce ) {
		// If no texture set and not forced, do nothing.
		if ( !_bForce ) {
			if ( !m_rsCurRenderState.psrShaderResources[_ui32Slot] ) { return; }
		}
		glWarnError( "Uncaught" );
		if ( !m_rsCurRenderState.pssSamplers[_ui32Slot] ) {
			m_rsCurRenderState.pssSamplers[_ui32Slot] = m_pssDefaultSamplerState;
		}
		
		ActivateTexture( _ui32Slot );

		const LSG_SAMPLER_STATE & ssCurState = (*m_rsCurRenderState.pssSamplers[_ui32Slot]);
		//LSG_SAMPLER_STATE & ssLastState = m_ssLastSamplers[_ui32Slot];

/*#define LSG_CHANGED( MEMBER )	((ssCurState.MEMBER != ssLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	ssLastState.MEMBER = ssCurState.MEMBER*/


		static const GLint iMinFilter[] = {
			GL_NEAREST,							// Point, no mipmaps.
			GL_NEAREST_MIPMAP_NEAREST,			// Point, point mipmaps.
			GL_NEAREST_MIPMAP_LINEAR,			// Point, linear mipmaps.
			GL_LINEAR,							// Linear, no mipmaps.
			GL_LINEAR_MIPMAP_NEAREST,			// Linear, point mipmaps.
			GL_LINEAR_MIPMAP_LINEAR,			// Linear, linear mipmaps.
		};
		static const GLint iMagFilter[] = {
			GL_NEAREST,							// Point.
			GL_LINEAR,							// Linear.
		};

		// Always set filters because they are a property of textures.
		// Min filter.
		uint32_t ui32CurMinFilter = LSG_DECODE_MIN_FILTER( ssCurState.Filter ) - 1;
		// Mag filter.
		uint32_t ui32CurMagFilter = LSG_DECODE_MAG_FILTER( ssCurState.Filter ) - 1;
		
		uint32_t ui32OglMinFilter = ui32CurMinFilter * 3;
		if ( m_rsCurRenderState.uiMipLevels[_ui32Slot] > 1 ) {
			// Mip filter.
			uint32_t ui32CurMipFilter = LSG_DECODE_MIP_FILTER( ssCurState.Filter );
			ui32OglMinFilter += ui32CurMipFilter;
		}
		uint32_t ui32OglMagFilter = ui32CurMagFilter;
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, iMinFilter[ui32OglMinFilter] );
		glWarnError( "GL_TEXTURE_MIN_FILTER" );
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, iMagFilter[ui32OglMagFilter] );
		glWarnError( "GL_TEXTURE_MAG_FILTER" );

		// Anisotropy.
		LSBOOL bCurIsAniso = LSG_DECODE_IS_ANISOTROPIC_FILTER( ssCurState.Filter );
		if ( !bCurIsAniso ) {
			::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1 );
		}
		else {
			::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, CStd::Min<uint32_t>( ssCurState.MaxAnisotropy, CGfxBase::m_mMetrics.ui32MaxAniso ) );
		}
		glWarnError( "GL_TEXTURE_MAX_ANISOTROPY_EXT" );

		// Address modes.
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ssCurState.AddressU );
		glWarnError( "GL_TEXTURE_WRAP_S" );
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ssCurState.AddressV );
		glWarnError( "GL_TEXTURE_WRAP_T" );
		/*::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, ssCurState.AddressW );
		glWarnError( "GL_TEXTURE_WRAP_R" );

		// LOD bias.
		::glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, ssCurState.MipLODBias );
		glWarnError( "GL_TEXTURE_LOD_BIAS" );

		// Border color.
		::glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, ssCurState.BorderColor );
		glWarnError( "GL_TEXTURE_BORDER_COLOR" );

		// Minimum/maximum mip-map level.
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(ssCurState.MinLOD) );
		glWarnError( "GL_TEXTURE_BASE_LEVEL" );
		::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(ssCurState.MaxLOD) );
		glWarnError( "GL_TEXTURE_MAX_LEVEL" );*/

/*#undef LSG_UPDATE
#undef LSG_CHANGED*/
	}

	/**
	 * Sets sampler states, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetSamplerStates( LSBOOL _bForce ) {
		for ( uint32_t I = CStd::Min<uint32_t>( m_mMetrics.ui32MaxTexSlot, LSG_MAX_SHADER_SAMPLERS ); I--; ) {
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
	LSE_INLINE void LSE_FCALL COpenGlEs::SetRasterizerState( LSBOOL _bForce ) {
		glWarnError( "Uncaught" );
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
		/*if ( LSG_CHANGED( FillMode ) ) {
			::glPolygonMode( GL_FRONT_AND_BACK, rsCurState.FillMode );
			glWarnError( "glPolygonMode" );
			LSG_UPDATE( FillMode );
		}*/

		// Check the cull mode.
		if ( LSG_CHANGED( CullMode ) ) {
			if ( rsCurState.CullMode == LSG_CM_NONE ) {
				::glDisable( GL_CULL_FACE );
				glWarnError( "GL_CULL_FACE" );
			}
			else {
				if ( rsLastState.CullMode == LSG_CM_NONE ) {
					// Were off, now is on.
					::glEnable( GL_CULL_FACE );
					glWarnError( "GL_CULL_FACE" );
				}
				::glCullFace( rsCurState.CullMode );
				glWarnError( "glCullFace" );
			}

			LSG_UPDATE( CullMode );
		}

		// Check the front-face mode.
		if ( LSG_CHANGED( FrontCounterClockwise ) ) {
			::glFrontFace( rsCurState.FrontCounterClockwise ? GL_CCW : GL_CW );
			glWarnError( "glFrontFace" );
			LSG_UPDATE( FrontCounterClockwise );
		}

		// Check the depth bias and the slope scaled depth bias.
		if ( LSG_CHANGED( DepthBias ) || LSG_CHANGED( SlopeScaledDepthBias ) || LSG_CHANGED( DepthBiasClamp ) ) {
			GLfloat fBias = rsCurState.SlopeScaledDepthBias * rsCurState.DepthBiasClamp;
			::glPolygonOffset( fBias, static_cast<GLfloat>(rsCurState.DepthBias) );
			glWarnError( "glPolygonOffset" );
			// Enable or disable polygon offset if necessary.
			LSBOOL bOn = fBias || rsCurState.DepthBias;
			GLfloat fLastBias = rsLastState.SlopeScaledDepthBias * rsLastState.DepthBiasClamp;
			LSBOOL bLastOn = fLastBias || rsLastState.DepthBias;
			if ( bOn && !bLastOn ) {
				::glEnable( GL_POLYGON_OFFSET_FILL );
				glWarnError( "GL_POLYGON_OFFSET_FILL" );
			}
			else if ( !bOn && bLastOn ) {
				::glDisable( GL_POLYGON_OFFSET_FILL );
				glWarnError( "GL_POLYGON_OFFSET_FILL" );
			}
			LSG_UPDATE( DepthBias );
			LSG_UPDATE( SlopeScaledDepthBias );
			LSG_UPDATE( DepthBiasClamp );
		}

		// Check the depth-clip mode.
		/*if ( LSG_CHANGED( DepthClipEnable ) ) {
			// Not supported.
			LSG_UPDATE( DepthClipEnable );
		}*/

		// Check the scissor mode.
		if ( LSG_CHANGED( ScissorEnable ) ) {
			if ( rsCurState.ScissorEnable ) { ::glEnable( GL_SCISSOR_TEST ); }
			else { ::glDisable( GL_SCISSOR_TEST ); }
			glWarnError( "GL_SCISSOR_TEST" );
			LSG_UPDATE( ScissorEnable );
		}

		// Check the multi-sample mode.
		/*if ( LSG_CHANGED( MultisampleEnable ) ) {
			if ( rsCurState.MultisampleEnable ) { ::glEnable( GL_MULTISAMPLE ); }
			else { ::glDisable( GL_MULTISAMPLE ); }
			glWarnError( "GL_MULTISAMPLE" );
			LSG_UPDATE( MultisampleEnable );
		}

		// Check the antialiased line mode.
		if ( LSG_CHANGED( AntialiasedLineEnable ) ) {
			if ( rsCurState.AntialiasedLineEnable ) { ::glEnable( GL_LINE_SMOOTH ); }
			else { ::glDisable( GL_LINE_SMOOTH ); }
			glWarnError( "GL_LINE_SMOOTH" );
			LSG_UPDATE( AntialiasedLineEnable );
		}*/


#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the render-target blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetRenderTargetBlendState( uint32_t _ui32Slot, LSBOOL _bForce ) {
		glWarnError( "Uncaught" );
		const LSG_RENDER_TARGET_BLEND_DESC & rtbsCurState = m_rsCurRenderState.pbsBlendState->RenderTarget[_ui32Slot];
		LSG_RENDER_TARGET_BLEND_DESC & rtbsLastState = m_bsLastBlend.RenderTarget[_ui32Slot];

#define LSG_CHANGED( MEMBER )	((rtbsCurState.MEMBER != rtbsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rtbsLastState.MEMBER = rtbsCurState.MEMBER

		// Use glBlendFuncSeparatei() and glBlendEquationSeparatei() in OpenGL 4.0.
		if ( _ui32Slot == 0 ) {
			// Check the blend mode.
			if ( LSG_CHANGED( BlendEnable ) ) {
				if ( rtbsCurState.BlendEnable ) { ::glEnable( GL_BLEND ); }
				else { ::glDisable( GL_BLEND ); }
				glWarnError( "GL_BLEND" );
				LSG_UPDATE( BlendEnable );
			}
			
			// Check blend functions.
			if ( LSG_CHANGED( SrcBlend ) || LSG_CHANGED( DestBlend ) || LSG_CHANGED( SrcBlendAlpha ) || LSG_CHANGED( DestBlendAlpha ) ) {
				::glBlendFuncSeparate( rtbsCurState.SrcBlend, rtbsCurState.DestBlend, rtbsCurState.SrcBlendAlpha, rtbsCurState.DestBlendAlpha );
				glWarnError( "glBlendFuncSeparate" );
				LSG_UPDATE( SrcBlend );
				LSG_UPDATE( DestBlend );
				LSG_UPDATE( SrcBlendAlpha );
				LSG_UPDATE( DestBlendAlpha );
			}

			// Check the blend operators.
			if ( LSG_CHANGED( BlendOp ) || LSG_CHANGED( BlendOpAlpha ) ) {
				::glBlendEquationSeparate( rtbsCurState.BlendOp, rtbsCurState.BlendOpAlpha );
				glWarnError( "glBlendEquationSeparate" );
				LSG_UPDATE( BlendOp );
				LSG_UPDATE( BlendOpAlpha );
			}


			// Use glColorMaski() in OpenGL 4.0.
			// Check the write mask.
			if ( LSG_CHANGED( RenderTargetWriteMask ) ) {
				::glColorMask( (rtbsCurState.RenderTargetWriteMask & LSG_CWM_RED) != 0,
					(rtbsCurState.RenderTargetWriteMask & LSG_CWM_GREEN) != 0,
					(rtbsCurState.RenderTargetWriteMask & LSG_CWM_BLUE) != 0,
					(rtbsCurState.RenderTargetWriteMask & LSG_CWM_ALPHA) != 0 );
				glWarnError( "glColorMask" );
				LSG_UPDATE( RenderTargetWriteMask );
			}
		}


#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the blend state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetBlendState( LSBOOL _bForce ) {
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
	LSE_INLINE void LSE_FCALL COpenGlEs::SetDepthStencilOpState( LSBOOL _bFront, LSBOOL _bForce ) {
		glWarnError( "Uncaught" );
		const LSG_DEPTH_STENCILOP_DESC & _dsodCurState = _bFront ? m_rsCurRenderState.pdssDepthStencilState->FrontFace :
			m_rsCurRenderState.pdssDepthStencilState->BackFace;
		LSG_DEPTH_STENCILOP_DESC & _dsodLastState = _bFront ? m_dssLastDepthStencil.FrontFace :
			m_dssLastDepthStencil.BackFace;

#define LSG_CHANGED( MEMBER )	((_dsodCurState.MEMBER != _dsodLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	_dsodLastState.MEMBER = _dsodCurState.MEMBER

		GLenum eFace = _bFront ? GL_FRONT : GL_BACK;

		// Check the stencil fail operation.
		if ( LSG_CHANGED( StencilFailOp ) || LSG_CHANGED( StencilDepthFailOp ) || LSG_CHANGED( StencilPassOp ) ) {
			::glStencilOpSeparate( eFace, _dsodCurState.StencilFailOp, _dsodCurState.StencilDepthFailOp, _dsodCurState.StencilPassOp );
			glWarnError( "glStencilOpSeparate" );
			LSG_UPDATE( StencilFailOp );
			LSG_UPDATE( StencilDepthFailOp );
			LSG_UPDATE( StencilPassOp );
		}

		// Check the stencil function.
		if ( LSG_CHANGED( StencilFunc ) ||
			m_rsCurRenderState.pdssDepthStencilState->StencilReadMask != m_rsLastRenderState.pdssDepthStencilState->StencilReadMask ||
			m_rsCurRenderState.ui32StencilRef != m_rsLastRenderState.ui32StencilRef ) {
			::glStencilFuncSeparate( eFace, _dsodCurState.StencilFunc,
				m_rsCurRenderState.ui32StencilRef,
				m_rsCurRenderState.pdssDepthStencilState->StencilReadMask );
			glWarnError( "glStencilFuncSeparate" );
			LSG_UPDATE( StencilFunc );
			// ui32StencilRef and StencilReadMask updated outside of this call.
		}

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the depth-stencil state, avoiding redundant sets.  States are set based off the current values in m_rsCurRenderState.
	 *
	 * \param _bForce Forces changes.
	 */
	LSE_INLINE void LSE_FCALL COpenGlEs::SetDepthStencilState( LSBOOL _bForce ) {
		glWarnError( "Uncaught" );
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
			if ( dssCurState.DepthEnable ) { ::glEnable( GL_DEPTH_TEST ); }
			else { ::glDisable( GL_DEPTH_TEST ); }
			glWarnError( "GL_DEPTH_TEST" );
			LSG_UPDATE( DepthEnable );
		}

		// Check depth-writing.
		if ( LSG_CHANGED( DepthWriteMask ) ) {
			::glDepthMask( static_cast<GLboolean>(dssCurState.DepthWriteMask) );
			glWarnError( "glDepthMask" );
			LSG_UPDATE( DepthWriteMask );
		}

		// Check the depth function.
		if ( LSG_CHANGED( DepthFunc ) ) {
			::glDepthFunc( dssCurState.DepthFunc );
			glWarnError( "glDepthFunc" );
			LSG_UPDATE( DepthFunc );
		}

		// Check the stencil-enable.
		if ( LSG_CHANGED( StencilEnable ) ) {
			if ( dssCurState.StencilEnable ) { ::glEnable( GL_STENCIL_TEST ); }
			else { ::glDisable( GL_STENCIL_TEST ); }
			glWarnError( "GL_STENCIL_TEST" );
			LSG_UPDATE( StencilEnable );
		}

		// Check the stencil write mask.
		if ( LSG_CHANGED( StencilWriteMask ) ) {
			::glStencilMask( dssCurState.StencilWriteMask );
			glWarnError( "glStencilMask" );
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
		LSG_UPDATE( ui32StencilRef );
		LSG_UPDATE( pdssDepthStencilState->StencilReadMask );
	
		//uint8_t										StencilReadMask;								/**< Identify a portion of the depth-stencil buffer for reading stencil data. */

#undef LSG_UPDATE
#undef LSG_CHANGED
	}
	
	/**
	 * Updates vertex attributes for OpneGL ES.
	 */
	LSE_INLINE void LSE_CALL COpenGlEs::FinalizeVertAttrib() {
		// Enable and disable delayed vertex attributes.
		for ( GLuint I = CStd::Max( m_rsCurRenderState.uiMaxVertAttrib, m_rsLastRenderState.uiMaxVertAttrib ); I--; ) {
			GLboolean bCur = m_rsCurRenderState.bVertAttr[I];
			GLboolean bPrev = m_rsLastRenderState.bVertAttr[I];
			if ( bCur != bPrev ) {
				if ( !bCur ) {
					::glDisableVertexAttribArray( I );
				}
				else {
					::glEnableVertexAttribArray( I );
				}
				m_rsLastRenderState.bVertAttr[I] = bCur;
			}
		}
		m_rsLastRenderState.uiMaxVertAttrib = m_rsCurRenderState.uiMaxVertAttrib;
		m_rsCurRenderState.uiMaxVertAttrib = 0;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES

#endif	// __LSG_OPENGLES_H__
