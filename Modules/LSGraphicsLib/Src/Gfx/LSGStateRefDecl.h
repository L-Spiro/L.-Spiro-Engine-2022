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
 * These are the structures needed for methods that create and release various types of states.
 */


// == Types.
/** Reference-counted LSG_SAMPLER_STATE object. */
typedef struct LSG_SAMPLER_STATE_REF {
	uint32_t								ui32Ref;										/**< Reference count. */
	LSG_SAMPLER_STATE *						psObj;											/**< Reference-counted object. */
} * LPLSG_SAMPLER_STATE, * const LPCLSG_SAMPLER_STATE;

/** Reference-counted LSG_RASTERIZER_STATE object. */
typedef struct LSG_RASTERIZER_STATE_REF {
	uint32_t								ui32Ref;										/**< Reference count. */
	LSG_RASTERIZER_STATE *					psObj;											/**< Reference-counted object. */
} * LPLSG_RASTERIZER_STATE, * const LPCLSG_RASTERIZER_STATE;

/** Reference-counted LSG_BLEND_STATE object. */
typedef struct LSG_BLEND_STATE_REF {
	uint32_t								ui32Ref;										/**< Reference count. */
	LSG_BLEND_STATE *						psObj;											/**< Reference-counted object. */
} * LPLSG_BLEND_STATE, * const LPCLSG_BLEND_STATE;

/** Reference-counted LSG_DEPTH_STENCIL_STATE object. */
typedef struct LSG_DEPTH_STENCIL_STATE_REF {
	uint32_t								ui32Ref;										/**< Reference count. */
	LSG_DEPTH_STENCIL_STATE *				psObj;											/**< Reference-counted object. */
} * LPLSG_DEPTH_STENCIL_STATE, * const LPCLSG_DEPTH_STENCIL_STATE;
