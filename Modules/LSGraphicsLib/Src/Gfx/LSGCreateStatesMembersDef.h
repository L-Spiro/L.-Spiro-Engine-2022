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


#ifndef LSG_CREATESTATESDEF_CLASS
#error "LSG_CREATESTATESDEF_CLASS must be defined (for example as CDirectX9 or COpenGl)."
#endif	// #ifndef LSG_DESCFUNCDEF_CLASS

// == Members.
/** Sampler states. */
CLinearMap<LSG_SAMPLER_STATE, LSG_CREATESTATESDEF_CLASS::LSG_SAMPLER_STATE_REF> LSG_CREATESTATESDEF_CLASS::m_mSamplerStateMap;

/** Rasterizer states. */
CLinearMap<LSG_RASTERIZER_STATE, LSG_CREATESTATESDEF_CLASS::LSG_RASTERIZER_STATE_REF> LSG_CREATESTATESDEF_CLASS::m_mRasterizerStateMap;

/** Blend states. */
CLinearMap<LSG_BLEND_STATE, LSG_CREATESTATESDEF_CLASS::LSG_BLEND_STATE_REF> LSG_CREATESTATESDEF_CLASS::m_mBlendStateMap;

/** Depth/stencil states. */
CLinearMap<LSG_DEPTH_STENCIL_STATE, LSG_CREATESTATESDEF_CLASS::LSG_DEPTH_STENCIL_STATE_REF> LSG_CREATESTATESDEF_CLASS::m_mDepthStencilStateMap;

/** Sampler-state creator critical section. */
CCriticalSection LSG_CREATESTATESDEF_CLASS::m_csSamplerStateCrit;

/** Rasterizer-state creator critical section. */
CCriticalSection LSG_CREATESTATESDEF_CLASS::m_csRasterizerStateCrit;

/** Blend-state creator critical section. */
CCriticalSection LSG_CREATESTATESDEF_CLASS::m_csBlendStateCrit;

/** Depth/stencil-state creator critical section. */
CCriticalSection LSG_CREATESTATESDEF_CLASS::m_csDepthStencilStateCrit;

#undef	LSG_CREATESTATESDEF_CLASS
