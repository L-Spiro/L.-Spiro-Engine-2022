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
 * Description: A Metal command buffer.
 */

#include "LSGMetalCommandBuffer.h"

#ifdef LSE_METAL

@implementation CMetalCommandBuffer

// == Members.
@synthesize commandBuffer = m_cbCommandBuffer;
@synthesize commandEncoder = m_rceEncoder;


// == Functions.
/**
 * Initializes the object.
 */
- (CMetalCommandBuffer *) init {
	m_prpdDesc = [MTLRenderPassDescriptor new];
	m_cbCommandBuffer = nil;
	return self;
}

/**
 * Ends an encoding.
 */
- (CMetalCommandBuffer *) endEncoding {
	if ( m_cbCommandBuffer != nil ) {
		if ( m_rceEncoder != nil ) {
			[m_rceEncoder endEncoding];
			m_rceEncoder = nil;
		}
	}
	return self;
}

/**
 * Begins a frame (creates the command buffer from scratch).
 */
- (CMetalCommandBuffer *) beginFrame:(id <MTLCommandQueue>)_cqQueue {
	[self endEncoding];
	m_cbCommandBuffer = [_cqQueue commandBuffer];
	return self;
}

/**
 * Ends a frame.
 */
- (CMetalCommandBuffer *) endFrame {
	[self endEncoding];
	return self;
}

/**
 * Starts a render pass.
 */
- (CMetalCommandBuffer *) startPass:(const lsg::LSG_RENDER_PASS_DESCRIPTOR &)_rpdDesc {
	[self copyRenderPassDescriptor:_rpdDesc];
	if ( m_cbCommandBuffer != nil ) {
		[self endEncoding];
		m_rceEncoder = [m_cbCommandBuffer renderCommandEncoderWithDescriptor:m_prpdDesc];
	}
	
	return self;
}

/**
 * Presents to the given drawable.
 */
- (CMetalCommandBuffer *) present:(id <CAMetalDrawable>)_mdDrawable {
	if ( m_cbCommandBuffer != nil ) {
		[self endEncoding];
		[m_cbCommandBuffer presentDrawable:_mdDrawable];
		[m_cbCommandBuffer commit];
	}
	return self;
}

/**
 * Copies the C++ LSG_RENDER_PASS_DESCRIPTOR structure into this object's MTLRenderPassDescriptor object.
 */
- (MTLRenderPassDescriptor *) copyRenderPassDescriptor:(const lsg::LSG_RENDER_PASS_DESCRIPTOR &)_rpdDesc {
/*	LSUINT32 ui32Left = [m_prpdDesc.colorAttachments count];
	LSUINT32 ui32Right = LSE_ELEMENTS( _rpdDesc.dbColors );
	assert( ui32Left == ui32Right );*/
#define LSG_COPY( DST, SRC )													\
	DST.depthPlane = SRC.ui32DepthPlane;										\
	DST.level = SRC.ui32Level;													\
	DST.resolveDepthPlane = SRC.ui32ResolveDepthPlane;							\
	DST.resolveLevel = SRC.ui32ResolveLevel;									\
	DST.resolveSlice = SRC.ui32ResolveSlice;									\
	DST.slice = SRC.ui32Slice;													\
	DST.texture = (__bridge id <MTLTexture>)SRC.tTexture;						\
	DST.loadAction = SRC.bClear ? MTLLoadActionClear : MTLLoadActionDontCare;

	for ( LSUINT32 I = 0; I < LSE_ELEMENTS( _rpdDesc.dbColors ); ++I ) {
		LSG_COPY( m_prpdDesc.colorAttachments[I], _rpdDesc.dbColors[I] );
		m_prpdDesc.colorAttachments[I].clearColor = ::MTLClearColorMake( _rpdDesc.dbColors[I].fClearColor[0],
			_rpdDesc.dbColors[I].fClearColor[1],
			_rpdDesc.dbColors[I].fClearColor[2],
			_rpdDesc.dbColors[I].fClearColor[3] );
	}
	LSG_COPY( m_prpdDesc.depthAttachment, _rpdDesc.dbDepth );
	m_prpdDesc.depthAttachment.clearDepth = _rpdDesc.dbDepth.fClearDepth;
	LSG_COPY( m_prpdDesc.stencilAttachment, _rpdDesc.dbStencil );
	m_prpdDesc.stencilAttachment.clearStencil = _rpdDesc.dbStencil.ui8ClearStencil;
#undef LSG_COPY
	return m_prpdDesc;
}

@end

#endif	// #ifdef LSE_METAL
