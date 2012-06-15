// ****************************************************************************
// Filename:  CocoaImplementation.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _COCOA_IMPLEMENTATION_H_
#define _COCOA_IMPLEMENTATION_H_

#import <Cocoa/Cocoa.h>


@interface MyDelegate : NSObject {
	BOOL bExit;
	BOOL bLaunched;
	int nOpenWindows;
}

-(BOOL) shouldExit;
-(void) setShouldExit:(BOOL) exit;
-(BOOL) launched;
-(void) setLaunched:(BOOL) launched;
-(int) numberOfOpenWindows;
-(void) setNumberOfOpenWindows:(int) num;
@end



@interface MyWindow : NSObject {
	NSWindow* _window;
	void* _main_window_ptr;
}

-(NSWindow*) window;
-(void) setWindow:(NSWindow*) win;
-(void*) mainWindowPtr;
-(void) setMainWindowPtr:(void*) mainWindowPtr;

-(void) windowWillClose:(id) sender;
@end



@interface MyImageView : NSImageView {
    NSPoint dragStartLocation;
	NSPoint lastDragLocation;
    BOOL dragging;
}
-(id) initWithFrame:(NSRect) frame;
-(void) drawRect:(NSRect) rect;
-(void) mouseDown:(NSEvent*) event;
-(void) mouseDragged:(NSEvent*) event;
-(void) mouseMoved:(NSEvent*) event;
-(void) mouseUp:(NSEvent*) event;
-(void) keyDown:(NSEvent *) event;
-(void) keyUp:(NSEvent *) event;
@end


@interface MySlider : NSSlider {
	BOOL _dragging;
	NSPoint lastPos;
}
-(BOOL) dragging;
-(void) setDragging:(BOOL) flag;
-(void) mouseDown:(NSEvent*) theEvent;
@end


@interface MyOpenGLView : NSOpenGLView {
  
}
-(void) mouseDown:(NSEvent*) event;
-(void) rightMouseDown:(NSEvent*) event;
-(void) otherMouseDown:(NSEvent*) event;
-(void) mouseDragged:(NSEvent*) event;
-(void) mouseMoved:(NSEvent*) event;
-(void) mouseUp:(NSEvent*) event;
-(void) rightMouseUp:(NSEvent*) event;
-(void) otherMouseUp:(NSEvent*) event;
-(void) keyDown:(NSEvent *) event;
-(void) keyUp:(NSEvent *) event;
@end

#endif /* _COCOA_IMPLEMENTATION_H_ */
