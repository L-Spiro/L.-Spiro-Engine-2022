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
 * Description: A Metal object wrapper that can be cast to a void *.
 */


#ifndef __LSG_METALOBJECT_H__
#define __LSG_METALOBJECT_H__

#ifdef LSE_METAL

#include "../MetalCommandBuffer/LSGMetalCommandBuffer.h"

#import <Metal/Metal.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/CAMetalLayer.h>

@interface CMetalObject : NSObject {
	id <MTLDevice>												m_dDevice;			/**< Metal device. */
	CAMetalLayer *												m_pmaLayer;			/**< Metal layer. */
	id <MTLCommandQueue>										m_cqCommandQueue;	/**< Command queue. */
	CMetalCommandBuffer *										m_mcbCommandBuffer;	/**< Command buffer. */
	id <CAMetalDrawable>										m_mdBackBuffer;		/**< The current back buffer. */
	id <MTLTexture>												m_tDepthBuffer;		/**< The depth buffer. */
	id <MTLTexture>												m_tStencilBuffer;	/**< The stencil buffer. */
	MTLRenderPipelineDescriptor *								m_rpdPipelineDesc;	/**< The pipeline descriptor. */
}


// == Members.
@property (readonly, nonatomic) id <MTLDevice>					device;
@property (readonly, nonatomic) CAMetalLayer *					layer;
@property (readonly, nonatomic) id <MTLCommandQueue>			commandQueue;
@property (readonly, nonatomic) CMetalCommandBuffer *			commandBuffer;
@property (readonly, nonatomic) id <CAMetalDrawable>			backBuffer;
@property (readonly, nonatomic) id <MTLTexture>					depthBuffer;
@property (readonly, nonatomic) id <MTLTexture>					stencilBuffer;
@property (nonatomic) MTLRenderPipelineDescriptor *				renderPipelineDesc;


// == Functions.
/**
 * Creates the device and layer and adds the layer to the given view.
 */
- (CMetalObject *) initWithView:(UIView *)_pvView;

/**
 * Generates the depth and stencil buffers.
 */
- (CMetalObject *) createDepthStencilBuffers:(UIView *)_pvView;

/**
 * Begins a frame (creates the command buffer from scratch).
 */
- (CMetalObject *) beginFrame;

/**
 * Ends a frame.
 */
- (CMetalObject *) endFrame;

@end

#endif	// #ifdef LSE_METAL

#endif	// __LSG_METALOBJECT_H__
