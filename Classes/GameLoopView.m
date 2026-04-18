#import <QuartzCore/CAEAGLLayer.h>
#import "GameLoopView.h"
#include "ShaderUtilities.h"

enum {
    ATTRIB_VERTEX,
    ATTRIB_TEXTUREPOSITON,
    NUM_ATTRIBUTES
};

@implementation GameLoopView


+ (Class)layerClass 
{
    return [CAEAGLLayer class];
}

- (const GLchar *)readFile:(NSString *)name
{
    NSString *path;
    const GLchar *source;
    
    path = [[NSBundle mainBundle] pathForResource:name ofType: nil];
    source = (GLchar *)[[NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    return source;
}

- (BOOL)initializeBuffers
{
    [EAGLContext setCurrentContext:oglContext];

    BOOL success = YES;

    glDisable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &frameBufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

    glGenRenderbuffers(1, &colorBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBufferHandle);

    if (![self resizeBuffers]) {
        NSLog(@"Failure resizing framebuffer during init");
        success = NO;
    }

    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, oglContext, NULL, &videoTextureCache);
    if (err) {
        NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", err);
        success = NO;
    }

    const GLchar *vertSrc = [self readFile:@"passThrough.vsh"];
    const GLchar *fragSrc = [self readFile:@"passThrough.fsh"];

    GLint attribLocation[NUM_ATTRIBUTES] = {
        ATTRIB_VERTEX, ATTRIB_TEXTUREPOSITON,
    };
    GLchar *attribName[NUM_ATTRIBUTES] = {
        "position", "textureCoordinate",
    };

    glueCreateProgram(vertSrc, fragSrc,
                      NUM_ATTRIBUTES, (const GLchar **)&attribName[0], attribLocation,
                      0, 0, 0,
                      &passThroughProgram);

    if (!passThroughProgram)
        success = NO;

    return success;
}

- (BOOL)resizeBuffers
{
    NSLog(@"resizeBuffers: fb=%u rb=%u context=%@", frameBufferHandle, colorBufferHandle, oglContext);
    [EAGLContext setCurrentContext:oglContext];

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBufferHandle);

    [oglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderBufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderBufferHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBufferHandle);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    NSLog(@"resizeBuffers: framebuffer=%u renderbuffer=%u size=%d x %d status=0x%04x",
          frameBufferHandle, colorBufferHandle, renderBufferWidth, renderBufferHeight, status);

    return (status == GL_FRAMEBUFFER_COMPLETE);
}

- (void)layoutSubviews
{
    [super layoutSubviews];

    [EAGLContext setCurrentContext:oglContext];

    if (frameBufferHandle == 0 || colorBufferHandle == 0) {
        [self initializeBuffers];
    } else {
        [self resizeBuffers];
    }
}

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self != nil) {
		// Use 2x scale factor on Retina displays.
		self.contentScaleFactor = [[UIScreen mainScreen] scale];

        // Initialize OpenGL ES 2
        CAEAGLLayer* eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
        oglContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!oglContext || ![EAGLContext setCurrentContext:oglContext]) {
            NSLog(@"Problem with OpenGL context.");
            [self release];
            
            return nil;
        }
    }
	
    return self;
}

- (void)renderWithSquareVertices:(const GLfloat*)squareVertices textureVertices:(const GLfloat*)textureVertices
{
    // Use shader program.
    glUseProgram(passThroughProgram);
    
    // Update attribute values.
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_TEXTUREPOSITON, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXTUREPOSITON);
    
    // Update uniform values if there are any
    
    // Validate program before drawing. This is a good check, but only really necessary in a debug build.
    // DEBUG macro must be defined in your debug configurations if that's not already the case.
#if defined(DEBUG)    
    if (glueValidateProgram(passThroughProgram) != 0) {
        NSLog(@"Failed to validate program: %d", passThroughProgram);
        return;
    }    
#endif
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Present
    glBindRenderbuffer(GL_RENDERBUFFER, colorBufferHandle);
    [oglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (CGRect)textureSamplingRectForCroppingTextureWithAspectRatio:(CGSize)textureAspectRatio toAspectRatio:(CGSize)croppingAspectRatio
{
	CGRect normalizedSamplingRect = CGRectZero;	
	CGSize cropScaleAmount = CGSizeMake(croppingAspectRatio.width / textureAspectRatio.width, croppingAspectRatio.height / textureAspectRatio.height);
	CGFloat maxScale = fmax(cropScaleAmount.width, cropScaleAmount.height);
	CGSize scaledTextureSize = CGSizeMake(textureAspectRatio.width * maxScale, textureAspectRatio.height * maxScale);
	
	if ( cropScaleAmount.height > cropScaleAmount.width ) {
		normalizedSamplingRect.size.width = croppingAspectRatio.width / scaledTextureSize.width;
		normalizedSamplingRect.size.height = 1.0;
	}
	else {
		normalizedSamplingRect.size.height = croppingAspectRatio.height / scaledTextureSize.height;
		normalizedSamplingRect.size.width = 1.0;
	}
	// Center crop
	normalizedSamplingRect.origin.x = (1.0 - normalizedSamplingRect.size.width)/2.0;
	normalizedSamplingRect.origin.y = (1.0 - normalizedSamplingRect.size.height)/2.0;
	
	return normalizedSamplingRect;
}

- (void)displayPixelBuffer:(CVImageBufferRef)pixelBuffer
{
    if (frameBufferHandle == 0) {
        BOOL success = [self initializeBuffers];
        if (!success) {
            NSLog(@"Problem initializing OpenGL buffers.");
            return;
        }
    }

    [EAGLContext setCurrentContext:oglContext];

    if (videoTextureCache == NULL)
        return;

    size_t frameWidth  = CVPixelBufferGetWidth(pixelBuffer);
    size_t frameHeight = CVPixelBufferGetHeight(pixelBuffer);

    CVOpenGLESTextureRef texture = NULL;
    CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                                videoTextureCache,
                                                                pixelBuffer,
                                                                NULL,
                                                                GL_TEXTURE_2D,
                                                                GL_RGBA,
                                                                frameWidth,
                                                                frameHeight,
                                                                GL_BGRA,
                                                                GL_UNSIGNED_BYTE,
                                                                0,
                                                                &texture);

    if (!texture || err) {
        NSLog(@"CVOpenGLESTextureCacheCreateTextureFromImage failed (error: %d)", err);
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
    glBindTexture(CVOpenGLESTextureGetTarget(texture), CVOpenGLESTextureGetName(texture));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Clear the FULL framebuffer first
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Aspect-fit viewport
    float viewAspect  = (float)renderBufferWidth / (float)renderBufferHeight;
    float imageAspect = (float)frameWidth / (float)frameHeight;

    GLint vpX = 0;
    GLint vpY = 0;
    GLsizei vpW = renderBufferWidth;
    GLsizei vpH = renderBufferHeight;

    if (viewAspect > imageAspect) {
        // wider than game -> bars left/right
        vpH = renderBufferHeight;
        vpW = (GLsizei)lrintf((float)vpH * imageAspect);
        vpX = (renderBufferWidth - vpW) / 2;
        vpY = 0;
    } else {
        // taller than game -> bars top/bottom
        vpW = renderBufferWidth;
        vpH = (GLsizei)lrintf((float)vpW / imageAspect);
        vpX = 0;
        vpY = (renderBufferHeight - vpH) / 2;
    }

    glViewport(vpX, vpY, vpW, vpH);

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    NSLog(@"renderBuffer=%d x %d  frame=%zu x %zu  viewport=%d %d %d %d",
          renderBufferWidth, renderBufferHeight,
          frameWidth, frameHeight,
          vp[0], vp[1], vp[2], vp[3]);

    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    static const GLfloat textureVertices[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    [self renderWithSquareVertices:squareVertices textureVertices:textureVertices];

    glBindTexture(CVOpenGLESTextureGetTarget(texture), 0);
    CVOpenGLESTextureCacheFlush(videoTextureCache, 0);
    CFRelease(texture);
}

- (void)dealloc
{
    
	if (frameBufferHandle) {
        glDeleteFramebuffers(1, &frameBufferHandle);
        frameBufferHandle = 0;
    }
	
    if (colorBufferHandle) {
        glDeleteRenderbuffers(1, &colorBufferHandle);
        colorBufferHandle = 0;
    }
	
    if (passThroughProgram) {
        glDeleteProgram(passThroughProgram);
        passThroughProgram = 0;
    }
	
    if (videoTextureCache) {
        CFRelease(videoTextureCache);
        videoTextureCache = 0;
    }
    
    [super dealloc];
}

@end
