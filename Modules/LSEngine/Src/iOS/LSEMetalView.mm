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
 * Description: The main view.
 */

#import "LSEMetalView.h"

#if defined( LSE_IPHONE ) && defined( LSE_METAL )

@interface LSMetalView()

// == Functions.
#ifdef LSG_OGLES
/**
 * Creates the framebuffers.
 *
 * \return Returns an error code indicate success or type of failure.
 */
- (LSSTD_ERRORS) createFramebuffer;

/**
 * Destroys the framebuffers.
 */
- (void) deleteFramebuffer;
#endif	// #ifdef LSG_OGLES

@end


@implementation LSMetalView

// == Properties.
@synthesize game = m_pgGame;
#ifdef LSG_OGLES
@synthesize context = m_pcContext;
@synthesize framebufferId = m_uiFramebuffer;
@synthesize renderbufferId = m_uiRenderbuffer;
@synthesize bufferWidth = m_iWidth;
@synthesize bufferHeight = m_iHeight;
#endif	// #ifdef LSG_OGLES


// == Functions.
/**
 * Allows the system to create the type of layer we need.
 *
 * \return Returns the type of layer necessary for Metal rendering.
 */
+ (Class) layerClass {
	return [CAEAGLLayer class];
}

/**
 * Initializes the view with a coder.
 *
 * \param coder The coder for initialization.
 * \return Returns nil if creation failed or a self pointer otherwise.
 */
- (id) initWithCoder:(NSCoder *)coder {
	self = [super initWithCoder:coder];
	if ( self ) {
		CAEAGLLayer * plLayer = reinterpret_cast<CAEAGLLayer *>(self.layer);
		plLayer.opaque = YES;
		plLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
			kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
			nil];
		
		//[self sizeToFit];
	}
	
	return self;
}

/**
 * Initializes the view with a frame.
 *
 * \param frame The frame for initialization.
 * \return Returns nil if creation failed or a self pointer otherwise.
 */
-(id) initWithFrame:(CGRect)frame {
	self = [super initWithFrame:frame];
	if ( self ) {
		CAEAGLLayer * plLayer = reinterpret_cast<CAEAGLLayer *>(self.layer);
		plLayer.opaque = YES;
		plLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
			kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
			nil];
		
		m_piqInputQueue = LSENEW lse::CInputQueue();
		//[self sizeToFit];
	}
	
	return self;
}

/**
 * Destructor.
 */
- (void) dealloc {
#ifdef LSG_OGLES
	[self deleteFramebuffer];
#endif	// #ifdef LSG_OGLES
	LSEDELETE m_piqInputQueue;
	m_piqInputQueue = NULL;
	//[EAGLContext setCurrentContext:nil];
	//[super dealloc];
}

/**
 * Generally called only once at start-up.
 */
- (void) layoutSubviews {
	//[self createFramebuffer];
}

/** 
 * Touch-begins.
 *
 * \param touches The set of touches, all of which must be handled here.
 * \param event Event information.
 */
- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	m_tInputTime.Update( false );
	
	// For each input.
	for ( UITouch * touch in touches ) {
		CGPoint pPos = [touch locationInView:self];
		m_piqInputQueue->OnTouchBegin( static_cast<LSREAL>(pPos.x), static_cast<LSREAL>(pPos.y),
			m_tInputTime.GetCurMicros() );
	}
}

/**
 * Touch-moves.
 *
 * \param touches The set of touches, all of which must be handled here.
 * \param event Event information.
 */
- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	m_tInputTime.Update( false );
	
	// For each input.
	for ( UITouch * touch in touches ) {
		CGPoint pStart = [touch previousLocationInView:self];
		CGPoint pEnd = [touch locationInView:self];
		m_piqInputQueue->OnTouchMove( static_cast<LSREAL>(pStart.x), static_cast<LSREAL>(pStart.y),
			static_cast<LSREAL>(pEnd.x), static_cast<LSREAL>(pEnd.y),
			m_tInputTime.GetCurMicros() );
	}
}

/**
 * Touch-end.
 *
 * \param touches The set of touches, all of which must be handled here.
 * \param event Event information.
 */
- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	m_tInputTime.Update( false );
	
	// For each input.
	for ( UITouch * touch in touches ) {
		CGPoint pStart = [touch previousLocationInView:self];
		CGPoint pEnd = [touch locationInView:self];
		m_piqInputQueue->OnTouchEnd( static_cast<LSREAL>(pStart.x), static_cast<LSREAL>(pStart.y),
			static_cast<LSREAL>(pEnd.x), static_cast<LSREAL>(pEnd.y),
			m_tInputTime.GetCurMicros() );
	}
}

/**
 * Touch-cancel.
 *
 * \param touches The set of touches, all of which must be handled here.
 * \param event Event information.
 */
- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	m_tInputTime.Update( false );
	
	// For each input.
	for ( UITouch * touch in touches ) {
		CGPoint pStart = [touch previousLocationInView:self];
		CGPoint pEnd = [touch locationInView:self];
		m_piqInputQueue->OnTouchCancel( static_cast<LSREAL>(pStart.x), static_cast<LSREAL>(pStart.y),
			static_cast<LSREAL>(pEnd.x), static_cast<LSREAL>(pEnd.y),
			m_tInputTime.GetCurMicros() );
	}
}

/**
 * Sets the game pointer.  Must be set before the game can begin.
 *
 * \param game The game pointer.
 */
- (void) setGame:(lse::CGame *)game {
	m_pgGame = game;
	if ( m_pgGame ) {
		m_tInputTime = m_pgGame->Time();
		m_pgGame->SetInputQueue( m_piqInputQueue );
	}
}

/**
 * Sets the frame.
 */
/*- (void) setFrame:(CGRect)_rFrame {
	[super setFrame:_rFrame];
	CGFloat fScale = self.contentScaleFactor;
	
	CGSize sSize = self.bounds.size;
	sSize.width *= fScale;
	sSize.height *= fScale;
	
	self.metalLayer.drawableSize = sSize;
}*/

#ifdef LSG_OGLES
/**
 * Creates the framebuffers.
 *
 * \return Returns an error code indicate success or type of failure.
 */
- (LSSTD_ERRORS) createFramebuffer {
	if ( !m_pcContext ) {
		return LSSTD_E_SUCCESS;
	}
	COpenGlEs::MakeCurrent();
	
	[self deleteFramebuffer];	// Clear existing resources if any.
	
	glWarnError( "Uncaught" );
	
	// Generate the framebuffer.
	m_uiFramebuffer = COpenGlEs::GenFramebuffer();
	glWarnError( "Failed to generate framebuffer" );
	if ( !m_uiFramebuffer ) {
		[self deleteFramebuffer];
		return LSSTD_E_OUTOFMEMORY;
	}
	
	// Generate the primary renderbuffer.
	m_uiRenderbuffer = COpenGlEs::GenRenderbuffer();
	glWarnError( "Failed to generate renderbuffer" );
	if ( !m_uiRenderbuffer ) {
		[self deleteFramebuffer];
		return LSSTD_E_OUTOFMEMORY;
	}
	
	GLint iLastRenderbuffer = COpenGlEs::LastRenderbuffer();	// Rebind before exit.

	COpenGlEs::BindRenderbuffer( m_uiRenderbuffer );
	
	// Create a renderbuffer that can be presented.
	if ( [m_pcContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:reinterpret_cast<CAEAGLLayer *>(self.layer)] == NO ) {
		glWarnError( "Failed to allocate storage for main renderbuffer" );
		COpenGlEs::BindRenderbuffer( iLastRenderbuffer );
		[self deleteFramebuffer];
		return LSSTD_E_OUTOFMEMORY;
	}
	
	// Attach it to the framebuffer.
	GLint iLastFramebuffer = COpenGlEs::LastFramebuffer();		// Rebind before exit.
	COpenGlEs::BindFramebuffer( m_uiFramebuffer );
	::glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_uiRenderbuffer );
	glWarnError( "Failed to attach renderbuffer to framebuffer" );
	
	::glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_iWidth );
	::glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_iHeight );
	
	if ( true ) {
		// Generate the depth/stencil buffer.
		m_uiDepthStencilBuffer = COpenGlEs::GenRenderbuffer();
		glWarnError( "Failed to depth/stencil renderbuffer" );
		if ( !m_uiDepthStencilBuffer ) {
			COpenGlEs::BindFramebuffer( iLastFramebuffer );
			COpenGlEs::BindRenderbuffer( iLastRenderbuffer );
			[self deleteFramebuffer];
			return LSSTD_E_OUTOFMEMORY;
		}
		COpenGlEs::BindRenderbuffer( m_uiDepthStencilBuffer );
		::glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, m_iWidth, m_iHeight );
		glWarnError( "Failed to allocate storage for depth/stencil renderbuffer" );
		::glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uiDepthStencilBuffer );
		glWarnError( "Failed to attach depth/stencil to framebuffer" );
	}
	
	GLenum eStatus = ::glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( eStatus != GL_FRAMEBUFFER_COMPLETE ) {
		NSLog( @"Framebuffer incomplete: %s.", COpenGlEs::FramebufferStatusToString( eStatus ) );
		COpenGlEs::BindFramebuffer( iLastFramebuffer );
		COpenGlEs::BindRenderbuffer( iLastRenderbuffer );
		[self deleteFramebuffer];
		return LSSTD_E_OUTOFMEMORY;
	}
	
	
	// Put back the old buffers that were there.
	/*COpenGlEs::BindFramebuffer( iLastFramebuffer );
	COpenGlEs::BindRenderbuffer( iLastRenderbuffer );*/
	COpenGlEs::BindRenderbuffer( m_uiRenderbuffer );
	COpenGlEs::SetBackBuffers( m_uiFramebuffer, m_uiRenderbuffer );
	::glFlush();
	return LSSTD_E_SUCCESS;
}

/**
 * Destroys the framebuffers.
 */
- (void) deleteFramebuffer {
	if ( m_uiRenderbuffer ) {
		// Bind and detach our render buffer.
		COpenGlEs::BindRenderbuffer( m_uiRenderbuffer );
		[m_pcContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:nil];
		// Delete our color render buffer.
		COpenGlEs::DeleteRenderbuffer( m_uiRenderbuffer );
		m_uiRenderbuffer = 0;
	}
	
	if ( m_uiDepthStencilBuffer ) {
		COpenGlEs::DeleteRenderbuffer( m_uiDepthStencilBuffer );
		m_uiDepthStencilBuffer = 0;
	}
	
	if ( m_uiFramebuffer ) {
		COpenGlEs::DeleteFramebuffer( m_uiFramebuffer );
		m_uiFramebuffer = 0;
	}
	m_iWidth = m_iHeight = 0;
	COpenGlEs::SetBackBuffers( m_uiFramebuffer, m_uiRenderbuffer );
}

#endif	// #ifdef LSG_OGLES

/*- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}*/

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end

#endif	// #if defined( LSE_IPHONE ) && defined( LSE_METAL )
