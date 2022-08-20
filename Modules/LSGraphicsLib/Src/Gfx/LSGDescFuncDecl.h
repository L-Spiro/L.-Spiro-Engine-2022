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
 *	API-specific constants and enumerations.
 * These are the declarations for methods that set default values for the structures in LSGDescStructs.h.
 */


// == Functions.
/**
 * Fills an LSG_SAMPLER_DESC structure with default values and returns the input structure.
 *
 * \param _sdDesc The sampler description whose default values are to be set.
 * \return Returns the input _sdDesc.
 */
static LSG_SAMPLER_DESC & LSE_FCALL			DefaultSamplerDesc( LSG_SAMPLER_DESC &_sdDesc );

/**
 * Fills an LSG_RASTERIZER_DESC structure with default values and returns the input structure.
 *
 * \param _rdDesc The rasterizer description whose default values are to be set.
 * \return Returns the input _rdDesc.
 */
static LSG_RASTERIZER_DESC & LSE_FCALL		DefaultRasterizerDesc( LSG_RASTERIZER_DESC &_rdDesc );

/**
 * Fills an LSG_RENDER_TARGET_BLEND_DESC structure with default values and returns the input structure.
 *
 * \param _rtbdDesc The render-target blend description whose default values are to be set.
 * \return Returns the input _rtbdDesc.
 */
static LSG_RENDER_TARGET_BLEND_DESC & LSE_FCALL
											DefaultRenderTargetBlendDesc( LSG_RENDER_TARGET_BLEND_DESC &_rtbdDesc );

/**
 * Fills an LSG_BLEND_DESC structure with default values and returns the input structure.
 *
 * \param _bdDesc The blend description whose default values are to be set.
 * \return Returns the input _bdDesc.
 */
static LSG_BLEND_DESC & LSE_FCALL			DefaultBlendDesc( LSG_BLEND_DESC &_bdDesc );

/**
 * Fills an LSG_DEPTH_STENCILOP_DESC structure with default values and returns the input structure.
 *
 * \param _dsodDesc The depth/stencil operation description whose default values are to be set.
 * \return Returns the input _dsodDesc.
 */
static LSG_DEPTH_STENCILOP_DESC & LSE_FCALL	DefaultDepthStencilOpDesc( LSG_DEPTH_STENCILOP_DESC &_dsodDesc );

/**
 * Fills an LSG_DEPTH_STENCIL_DESC structure with default values and returns the input structure.
 *
 * \param _dsdDesc The depth/stencil description whose default values are to be set.
 * \return Returns the input _dsdDesc.
 */
static LSG_DEPTH_STENCIL_DESC & LSE_FCALL	DefaultDepthStencilDesc( LSG_DEPTH_STENCIL_DESC &_dsdDesc );
