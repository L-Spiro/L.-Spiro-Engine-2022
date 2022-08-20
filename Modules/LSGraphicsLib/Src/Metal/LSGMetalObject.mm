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

#include "LSGMetalObject.h"

#ifdef LSE_METAL

@implementation CMetalObject

// == Members.
@synthesize device = m_dDevice;
@synthesize layer = m_pmaLayer;
@synthesize commandQueue = m_cqCommandQueue;
@synthesize commandBuffer = m_mcbCommandBuffer;
@synthesize backBuffer = m_mdBackBuffer;
@synthesize depthBuffer = m_tDepthBuffer;
@synthesize stencilBuffer = m_tStencilBuffer;
@synthesize renderPipelineDesc = m_rpdPipelineDesc;


// == Functions.
/**
 * Creates the device and layer and adds the layer to the given view.
 */
- (CMetalObject *) initWithView:(UIView *)_pvView {
	if ( m_dDevice == nil ) {
		// Create the device and command queue.
		m_dDevice = ::MTLCreateSystemDefaultDevice();
		m_mcbCommandBuffer = [[CMetalCommandBuffer new] init];
		
		[_pvView setOpaque:YES];
		[_pvView setBackgroundColor:nil];
		[_pvView setContentScaleFactor:[UIScreen mainScreen].scale];
		
		// Create the layer.
		m_pmaLayer = [CAMetalLayer layer];
		m_pmaLayer.framebufferOnly = YES;
		[m_pmaLayer setDevice:m_dDevice];
		[m_pmaLayer setPixelFormat:MTLPixelFormatBGRA8Unorm];
		
		CGFloat fScale = _pvView.contentScaleFactor;

		CGSize sSize = _pvView.bounds.size;
		sSize.width *= fScale;
		sSize.height *= fScale;
		m_pmaLayer.drawableSize = sSize;
	
		[m_pmaLayer setFrame:_pvView.layer.frame];
		[_pvView.layer addSublayer:m_pmaLayer];
		
		[self createDepthStencilBuffers:_pvView];
		
		m_cqCommandQueue = [m_dDevice newCommandQueue];
		m_rpdPipelineDesc = [MTLRenderPipelineDescriptor new];
	}
	return self;
}

/**
 * Generates the depth and stencil buffers.
 */
- (CMetalObject *) createDepthStencilBuffers:(UIView *)_pvView {
	CGSize sSize = m_pmaLayer.drawableSize;
	NSUInteger uiWidth = static_cast<NSUInteger>(sSize.width);
	NSUInteger uiHeight = static_cast<NSUInteger>(sSize.height);
	MTLTextureDescriptor * ptDepthTex = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
		width:uiWidth height:uiHeight mipmapped:NO];
	m_tDepthBuffer = [m_dDevice newTextureWithDescriptor:ptDepthTex];
	
	MTLTextureDescriptor * ptStencilTex = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatStencil8
		width:uiWidth height:uiHeight mipmapped:NO];
	m_tStencilBuffer = [m_dDevice newTextureWithDescriptor:ptStencilTex];
	
	return self;
}

/**
 * Begins a frame (creates the command buffer from scratch).
 */
- (CMetalObject *) beginFrame {
	[m_mcbCommandBuffer beginFrame:m_cqCommandQueue];
	m_mdBackBuffer = nil;
	while ( m_mdBackBuffer == nil ) {
		m_mdBackBuffer = [m_pmaLayer nextDrawable];
	}
	return self;
}

/**
 * Ends a frame.
 */
- (CMetalObject *) endFrame {
	return self;
}

@end

#endif	// #ifdef LSE_METAL
