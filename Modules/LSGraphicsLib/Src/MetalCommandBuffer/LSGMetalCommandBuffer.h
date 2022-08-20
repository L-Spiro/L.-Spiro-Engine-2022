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


#ifndef __LSG_METALCOMMANDBUFFER_H__
#define __LSG_METALCOMMANDBUFFER_H__

#ifdef LSE_METAL

#include "../Metal/LSGMetal.h"
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/CAMetalLayer.h>

@interface CMetalCommandBuffer : NSObject {
	id <MTLCommandBuffer>								m_cbCommandBuffer;	/**< Command buffer. */
	MTLRenderPassDescriptor *							m_prpdDesc;			/**< Render-pass descriptor. */
	id <MTLRenderCommandEncoder>						m_rceEncoder;		/**< Command encoder. */
}


// == Members.
@property (nonatomic) id <MTLCommandBuffer>				commandBuffer;
@property (nonatomic) id <MTLRenderCommandEncoder>		commandEncoder;


// == Functions.
/**
 * Initializes the object.
 */
- (CMetalCommandBuffer *) init;

/**
 * Begins a frame (creates the command buffer from scratch).
 */
- (CMetalCommandBuffer *) beginFrame:(id <MTLCommandQueue>)_cqQueue;

/**
 * Ends a frame.
 */
- (CMetalCommandBuffer *) endFrame;

/**
 * Starts a render pass.
 */
- (CMetalCommandBuffer *) startPass:(const lsg::LSG_RENDER_PASS_DESCRIPTOR &)_rpdDesc;

/**
 * Presents to the given drawable.
 */
- (CMetalCommandBuffer *) present:(id <CAMetalDrawable>)_mdDrawable;

/**
 * Copies the C++ LSG_RENDER_PASS_DESCRIPTOR structure into this object's MTLRenderPassDescriptor object.
 */
- (MTLRenderPassDescriptor *) copyRenderPassDescriptor:(const lsg::LSG_RENDER_PASS_DESCRIPTOR &)_rpdDesc;

@end

#endif	// #ifdef LSE_METAL

#endif	// __LSG_METALCOMMANDBUFFER_H__
