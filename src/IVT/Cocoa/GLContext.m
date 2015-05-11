// ****************************************************************************
// Filename:  GLContext.m
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#import <Cocoa/Cocoa.h>

// global autorelease pool with reference conting
NSAutoreleasePool* myPool = nil;
int contextCount = 0;

void* CocoaCreateGLContext(int width, int height, unsigned char *buffer) {
	if (myPool == nil)
		myPool = [[NSAutoreleasePool alloc] init];
		
	NSOpenGLPixelFormatAttribute attribs[] = {
		NSOpenGLPFAOffScreen,
		NSOpenGLPFAColorSize, 32,
		NSOpenGLPFADepthSize, 32,
		0};
		
	NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes: attribs];
	if (format == nil) {
		if (contextCount == 0) {
			[myPool release];
			myPool = nil;
		}
		
		printf("couldn't allocate format\n");
		return NULL;
	}
	
	NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat: format shareContext: nil];
	if (context == nil) {
		if (contextCount == 0) {
			[myPool release];
			myPool = nil;
		}
		
		printf("couldn't create context\n");
			
		return NULL;
	}
		
	[format dealloc];
	[context setOffScreen: buffer width: width height: height rowbytes: width*4];
	
	contextCount++;
	
	return context;
}

void CocoaDeleteGLContext(void* ptr) {
	NSOpenGLContext *context = (NSOpenGLContext*)ptr;
	
	contextCount--;
	
	if (contextCount <= 0) {
		[myPool release];
		myPool = nil;
	}
}

void CocoaMakeCurrentGLContext(void* ptr) {
	NSOpenGLContext *context = (NSOpenGLContext*)ptr;
	[context makeCurrentContext];
}

void CocoaDoneCurrentGLContext(void* ptr) {
	[NSOpenGLContext clearCurrentContext];
}

void CocoaSwapBuffersGLContext(void* ptr) {
	NSOpenGLContext *context = (NSOpenGLContext*)ptr;
	[context flushBuffer];
}

