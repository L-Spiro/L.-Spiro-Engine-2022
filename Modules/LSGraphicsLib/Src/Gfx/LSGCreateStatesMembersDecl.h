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
 * Description: This is meant to be included directly into the graphics API somewhere after it defines its
 *	API-specific constants and enumerations.
 * These are the members needed for methods that create and release various types of states.
 */


// == Functions.
/** Sampler states. */
static CLinearMap<LSG_SAMPLER_DESC, LSG_SAMPLER_STATE_REF>
											m_mSamplerStateMap;

/** Rasterizer states. */
static CLinearMap<LSG_RASTERIZER_DESC, LSG_RASTERIZER_STATE_REF>
											m_mRasterizerStateMap;

/** Blend states. */
static CLinearMap<LSG_BLEND_DESC, LSG_BLEND_STATE_REF>
											m_mBlendStateMap;

/** Depth/stencil states. */
static CLinearMap<LSG_DEPTH_STENCIL_DESC, LSG_DEPTH_STENCIL_STATE_REF>
											m_mDepthStencilStateMap;

/** Sampler-state creator critical section. */
static CCriticalSection						m_csSamplerStateCrit;

/** Rasterizer-state creator critical section. */
static CCriticalSection						m_csRasterizerStateCrit;

/** Blend-state creator critical section. */
static CCriticalSection						m_csBlendStateCrit;

/** Depth/stencil-state creator critical section. */
static CCriticalSection						m_csDepthStencilStateCrit;