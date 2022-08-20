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
 * Description: This is meant to be included directly into the graphics API immediately after it defines its
 *	API-specific constants and enumerations.  For example, the enumerated values in LSG_BLEND_MODE will be
 *	different values for each API, though each enumerated literal (LSG_BM_ADD, etc.) will be the same.  Since
 *	these enumerated values change depending on the API, this structure cannot be defined until after the
 *	API interface has been defined, which means this structure would have to be either manually redefined for
 *	each API or just put into a header file and #included at the appropriate point in each API header.
 * It defines the state of the renderer.
 */

// == Types.
/** The render-state copy that allows us to know which states have changed since the last system update. */
struct LSG_RENDER_STATE {
	/** Shader resources. */
	LSG_SHADER_RESOURCE *				psrShaderResources[LSG_MAX_TEXTURE_UNITS];

	/** Sampler states. */
	LSG_SAMPLER_STATE *					pssSamplers[LSG_MAX_SHADER_SAMPLERS];

	/** Render targets. */
	LSG_RENDER_TARGET *					prtRenderTargets[LSG_MAX_RENDER_TARGET_COUNT];

	/** Input layouts. */
	const LSG_INPUT_LAYOUT *			pilInputLayout;

	/** Vertex buffers. */
	LSG_VERTEX_BUFFER					pvbVertexBuffers[LSG_MAX_VERTEX_STREAMS];

	/** Depth/stencil buffer. */
	LSG_DEPTH_STENCIL *					pdsDepthStencil;
	
	/** Stencil buffer (when not combined with the depth buffer). */
	LSG_DEPTH_STENCIL *					pdsStencil;

	/** Rasterizer state. */
	LSG_RASTERIZER_STATE *				prsRasterState;

	/** Blend state. */
	LSG_BLEND_STATE *					pbsBlendState;

	/** Depth/stencil state. */
	LSG_DEPTH_STENCIL_STATE *			pdssDepthStencilState;

	/** Viewports. */
	LSG_VIEWPORT						vViewports[LSG_MAX_VIEWPORTS_SCISSORS];

	/** Scissors. */
	LSG_RECT							rScissors[LSG_MAX_VIEWPORTS_SCISSORS];

	/** Blend factors. */
	LSGREAL								fBlendFactors[4];

	/** Clear color. */
	LSGREAL								fClearColor[4];

	/** Clear depth. */
	LSGREAL								fClearDepth;

	/** Stencil reference value. */
	uint32_t							ui32StencilRef;

	/** 32-bit sample coverage for the output-merger state. */
	uint32_t							ui32SampleMask;

	/** Total active vertex buffers. */
	uint32_t							ui32TotalVertexBuffers;

	/** Stencil clear value. */
	uint8_t								ui8ClearStencil;

	/** Total viewports/scissors. */
	uint8_t								ui8TotalViewportsAndScissors;

	/** Total render targets. */
	uint8_t								ui8TotalRenderTargets;

	/** Texture IDs. */
	uint32_t							ui32TextureId[LSG_MAX_TEXTURE_UNITS];

	/** Highest texture set. */
	uint32_t							ui32MaxTextureSet;

	/** Framebuffer. */
	CFramebuffer *						pfFramebuffer;

};
