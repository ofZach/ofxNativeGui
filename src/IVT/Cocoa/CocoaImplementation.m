// *****************************************************************
// Filename:  CocoaImplementation.m
// Copyright: Pedram Azad, Chair Prof. Dillmann (IAIM),
//            Institute for Computer Science and Engineering (CSE),
//            University of Karlsruhe. All rights reserved.
// Author:    Florian Hecht
// Date:      2008
// *****************************************************************

#import "CocoaImplementation.h"

#import <Cocoa/Cocoa.h>

extern void EventCallback(void* main_window_ptr, void* widget, int type, int* params);

@implementation MyDelegate

-(BOOL) launched {
	return bLaunched;
}

-(void) setLaunched:(BOOL) launched {
	bLaunched = launched;
}

-(BOOL) shouldExit {
	return bExit;
}

-(void) setShouldExit:(BOOL) exit {
	bExit = exit;
}

-(int) numberOfOpenWindows {
	return nOpenWindows;
}

-(void) setNumberOfOpenWindows:(int) num {
	nOpenWindows = num;
}

@end


@implementation MyWindow

-(NSWindow*) window {
	return _window;
}

-(void) setWindow:(NSWindow*) win {
	_window = win;
}

-(void*) mainWindowPtr {
	return _main_window_ptr;
}

-(void) setMainWindowPtr:(void*) mainWindowPtr {
	_main_window_ptr = mainWindowPtr;
}

- (void) windowWillClose: (id)sender {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id delegate = [NSApp delegate];
	
	int nOpenWindows = [delegate numberOfOpenWindows];
	nOpenWindows--;
	[delegate setNumberOfOpenWindows: nOpenWindows];
	
	if (nOpenWindows <= 0)
		[delegate setShouldExit: TRUE];
  [pool release];
}

-(void) eventNotify: sender
{
	EventCallback(_main_window_ptr, sender, 0, NULL);
}

@end




@implementation MyImageView

-(id) initWithFrame:(NSRect) frame {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	self = [super initWithFrame: frame];
	
    if (self) {
		dragging = FALSE;
    }
	
  [pool release];
  return self;
}

-(void) drawRect:(NSRect) rect {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	[super drawRect: rect];
	
	if (dragging) {
		NSRect frame = [self bounds];
		
		int x1 = dragStartLocation.x;
		int y1 = dragStartLocation.y;
		int x2 = lastDragLocation.x;
		int y2 = lastDragLocation.y;
		
		if (x1 > x2) {
			int swap = x1;
			x1 = x2;
			x2 = swap;
		}
		
		if (y1 > y2) {
			int swap = y1;
			y1 = y2;
			y2 = swap;
		}
		
		NSRect r = NSMakeRect(x1, y1, x2 - x1, y2 - y1);
		
		[[NSColor whiteColor] set];
		[NSBezierPath setDefaultLineWidth: 2.0];
		[NSBezierPath strokeRect: r];
	}
  [pool release];
}

-(void) mouseDown:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	if (!dragging) {
        dragStartLocation = [self convertPoint:[event locationInWindow] fromView:nil];
		lastDragLocation = dragStartLocation;
		
		dragging = YES;
		
		NSRect frame = [self bounds];
		int height = frame.size.height;
		
		MyWindow* myWin = (MyWindow*)[[self window] delegate];
		int params[3] = {0, lastDragLocation.x, height - lastDragLocation.y};
		EventCallback([myWin mainWindowPtr], self, 3, params);
	}
  [pool release];
}

-(void) rightMouseDown:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {1, mousePos.x, height - mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 3, params);
  [pool release];
}

-(void) otherMouseDown:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {2, mousePos.x, height - mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 3, params);
  [pool release];
}

-(void) mouseDragged:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (dragging) {
		lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
		[self setNeedsDisplay: YES];
	}
  [pool release];
}

-(void) mouseMoved:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	
	int params[2] = {mousePos.x, mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 5, params);
  [pool release];
}

-(void) mouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (dragging) {
		lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
		
		NSRect frame = [self bounds];
		int dx = lastDragLocation.x - dragStartLocation.x;
		int dy = lastDragLocation.y - dragStartLocation.y;
		
		if (dx == 0 && dy == 0) {
			int x = lastDragLocation.x;
			int y = frame.size.height - lastDragLocation.y;
			
			MyWindow* myWin = (MyWindow*)[[self window] delegate];
			int params[2] = {x, y};
			EventCallback([myWin mainWindowPtr], self, 1, params);
		} else {
			int x1 = dragStartLocation.x;
			int y1 = frame.size.height - dragStartLocation.y;
			int x2 = lastDragLocation.x;
			int y2 = frame.size.height - lastDragLocation.y;
			
			if (x1 < 0) x1 = 0;
			if (x1 >= frame.size.width) x1 = frame.size.width - 1;
			if (y1 < 0) y1 = 0;
			if (y1 >= frame.size.height) y1 = frame.size.height - 1;
			if (x2 < 0) x2 = 0;
			if (x2 >= frame.size.width) x2 = frame.size.width - 1;
			if (y2 < 0) y2 = 0;
			if (y2 >= frame.size.height) y2 = frame.size.height - 1;
			
			if (x1 > x2) {
				int swap = x1;
				x1 = x2;
				x2 = swap;
			}
			
			if (y1 > y2) {
				int swap = y1;
				y1 = y2;
				y2 = swap;
			}
			
			MyWindow* myWin = (MyWindow*)[[self window] delegate];
			int params[4] = {x1, y1, x2, y2};
			EventCallback([myWin mainWindowPtr], self, 2, params);

		}
	}
	
	dragging = FALSE;
	
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {0, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 4, params);
  [pool release];
}

-(void) rightMouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {1, mousePos.x, height - mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 4, params);
  [pool release];
}

-(void) otherMouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {2, mousePos.x, height - mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 4, params);
  [pool release];
}

-(void) keyDown:(NSEvent *) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	
	NSString *c = [event characters];
	if ([c length]) {
        unichar character = [c characterAtIndex:0];
		
		int key = 0;
		if (character < 128)
			key = character;
		
		int params[1] = {key};
		EventCallback([myWin mainWindowPtr], self, 6, params);
	}
  [pool release];
}

-(void) keyUp:(NSEvent *) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	
	NSString *c = [event characters];
	if ([c length]) {
        unichar character = [c characterAtIndex:0];
		
		int key = 0;
		if (character < 128)
			key = character;
		
		int params[1] = {key};
		EventCallback([myWin mainWindowPtr], self, 7, params);
	}
	[pool release];
}

@end


@implementation MyOpenGLView

-(void) mouseDown:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {0, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 3, params);
  [pool release];
}

-(void) rightMouseDown:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {1, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 3, params);
  [pool release];
}

-(void) otherMouseDown:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {2, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 3, params);
  [pool release];
}

-(void) mouseDragged:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[2] = {mousePos.x, height - mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 5, params);
  [pool release];
}

-(void) mouseMoved:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[2] = {mousePos.x, height - mousePos.y};
	EventCallback([myWin mainWindowPtr], self, 5, params);
  [pool release];
}

-(void) mouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {0, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 4, params);
  [pool release];
}

-(void) rightMouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {1, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 4, params);
  [pool release];
}

-(void) otherMouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint lastDragLocation = [self convertPoint:[event locationInWindow] fromView:nil];
	NSRect frame = [self bounds];
	int height = frame.size.height;
	
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	int params[3] = {2, lastDragLocation.x, height - lastDragLocation.y};
	EventCallback([myWin mainWindowPtr], self, 4, params);
  [pool release];
}

-(void) keyDown:(NSEvent *) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	
	NSString *c = [event characters];
	if ([c length]) {
        unichar character = [c characterAtIndex:0];
		
		int key = 0;
		if (character < 128)
			key = character;
		
		int params[1] = {key};
		EventCallback([myWin mainWindowPtr], self, 6, params);
	}
  [pool release];
}

-(void) keyUp:(NSEvent *) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)[[self window] delegate];
	
	NSString *c = [event characters];
	if ([c length]) {
        unichar character = [c characterAtIndex:0];
		
		int key = 0;
		if (character < 128)
			key = character;
		
		int params[1] = {key};
		EventCallback([myWin mainWindowPtr], self, 7, params);
	}
	[pool release];
}

@end


@implementation MySlider

-(BOOL) dragging {
	return _dragging;
}

-(void) setDragging:(BOOL) flag {
	_dragging = flag;
}

-(void) mouseDown:(NSEvent*) theEvent {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSPoint mousePos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	NSSliderCell *sc = [self cell];
	
	[[self window] makeFirstResponder: self];
		
	if ([sc startTrackingAt: mousePos inView: self]) {
		_dragging = TRUE;
		lastPos = mousePos;
		
		[self sendAction: [self action] to: [self target]];
	}
  [pool release];
}

-(void) mouseDragged:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (_dragging) {
		NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
		NSSliderCell *sc = [self cell];
		
		[sc continueTracking: lastPos at: mousePos inView: self];
		lastPos = mousePos;
		
		[self sendAction: [self action] to: [self target]];
	}
  [pool release];
}

-(void) mouseUp:(NSEvent*) event {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	if (_dragging) {
		NSPoint mousePos = [self convertPoint:[event locationInWindow] fromView:nil];
		NSSliderCell *sc = [self cell];
		
		[sc stopTracking: lastPos at: mousePos inView: self mouseIsUp: TRUE];
		_dragging = FALSE;
		
		[self sendAction: [self action] to: [self target]];
	}
  [pool release];
}

@end


/* setAppleMenu disappeared from the headers in 10.4 */
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
@interface NSApplication(NSAppleMenu)
- (void)setAppleMenu:(NSMenu *)menu;
@end
#endif

@implementation NSApplication(SDL)
- (void)setRunning
{
  _running = 1;
}
@end

void CreateApplicationMenu(void) {
  NSString *appName;
  NSString *title;
  NSMenu *appleMenu;
  NSMenu *windowMenu;
  NSMenuItem *menuItem;
  
  /* Create the main menu bar */
  [NSApp setMainMenu:[[NSMenu alloc] init]];
  
  /* Create the application menu */
  appName = [[NSProcessInfo processInfo] processName];
  appleMenu = [[NSMenu alloc] initWithTitle:@""];
  
  /* Add menu items */
  title = [@"About " stringByAppendingString:appName];
  [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
  
  [appleMenu addItem:[NSMenuItem separatorItem]];
  
  title = [@"Hide " stringByAppendingString:appName];
  [appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];
  
  menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@/*"h"*/""];
  [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];
  
  [appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
  
  [appleMenu addItem:[NSMenuItem separatorItem]];
  
  title = [@"Quit " stringByAppendingString:appName];
  [appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
  
  /* Put menu into the menubar */
  menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
  [menuItem setSubmenu:appleMenu];
  [[NSApp mainMenu] addItem:menuItem];
  [menuItem release];
  
  /* Tell the application object that this is now the application menu */
  [NSApp setAppleMenu:appleMenu];
  [appleMenu release];
  
  
  /* Create the window menu */
  windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
  
  /* "Minimize" item */
  menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
  [windowMenu addItem:menuItem];
  [menuItem release];
  
  /* Put menu into the menubar */
  menuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
  [menuItem setSubmenu:windowMenu];
  [[NSApp mainMenu] addItem:menuItem];
  [menuItem release];
  
  /* Tell the application object that this is now the window menu */
  [NSApp setWindowsMenu:windowMenu];
  [windowMenu release];

}

// CocoaApplicationHandler functions
BOOL CocoaInitApplication(void) {
  
  ProcessSerialNumber psn;
  
  if (!GetCurrentProcess(&psn)) {
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    SetFrontProcess(&psn);
  }
  
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  
	[NSApplication sharedApplication];
  //NSApplicationLoad();
  
	CreateApplicationMenu();
  
  [NSApp finishLaunching];
  
  id delegate = [[MyDelegate alloc] init];
	[NSApp setDelegate: delegate];
	
	[delegate setLaunched: FALSE];
	[delegate setShouldExit: FALSE];
	[delegate setNumberOfOpenWindows: 0];
	
	[NSApp activateIgnoringOtherApps: YES];
  [NSApp setRunning];
	
  [pool release];
	return TRUE;
}

BOOL CocoaProcessEventsAndGetExit(void) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id delegate = [NSApp delegate];
	
	if (![delegate launched]) {
		[delegate setLaunched: TRUE];
	}
	
	if ([delegate shouldExit]) {
    [pool release];
		return TRUE;
  }
		
	NSDate* pDate = [NSDate distantPast];  
	NSEvent* pEvent = nil;
	do {
		pEvent = [NSApp nextEventMatchingMask: NSAnyEventMask untilDate: pDate inMode: NSEventTrackingRunLoopMode /*NSDefaultRunLoopMode*/ dequeue: YES];
		if (pEvent != nil) {
			[NSApp sendEvent: pEvent];
		}
	} while (pEvent);                           
	[NSApp updateWindows];
	
  [pool release];
	return FALSE;
}

void CocoaShutdownApplication(void) {
}


// CocoaMainWindow functions

void* CocoaCreateMainWindow(int x, int y, int width, int height, const char *title, void* main_window_ptr) {

  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSRect screenRect = [[NSScreen mainScreen] visibleFrame];
	
	int style = NSClosableWindowMask | NSTitledWindowMask | NSMiniaturizableWindowMask;
	NSWindow* win = [[NSWindow alloc] initWithContentRect:NSMakeRect(x, screenRect.size.height - y - height, width, height)
									  styleMask:style
									  backing:NSBackingStoreBuffered
									  defer:NO];
	
	[win setAcceptsMouseMovedEvents: YES];
	[win makeKeyWindow];
									  
	NSString* str = [[NSString alloc] initWithCString: title encoding: NSASCIIStringEncoding];
	[win setTitle: str];
  [str release];
	
	id myWin = [[MyWindow alloc] init];
	[myWin setWindow: win];
	[myWin setMainWindowPtr: main_window_ptr];
	
	[win setDelegate: myWin];
	
	
	NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
	[nc addObserver: myWin selector: @selector(windowWillClose:) name: NSWindowWillCloseNotification object: win];
	
	id delegate = [NSApp delegate];
	int nOpenWindows = [delegate numberOfOpenWindows];
	nOpenWindows++;
	[delegate setNumberOfOpenWindows: nOpenWindows];
	
  [pool release];
	return myWin;
}

void CocoaDestroyMainWindow(void* window)
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  MyWindow* myWin = (MyWindow*)window;
  NSWindow* win = [myWin window];
  
  [myWin release];
  
  [pool release];
}

void* CocoaCreateImage(void* window, int x, int y, int width, int height, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	MyImageView* image = [[MyImageView alloc] initWithFrame: NSMakeRect(x, y, width, height)];
	if (image != nil) {
		[image setEditable: NO];
		[image setAllowsCutCopyPaste: NO];
		
		[image setTarget: myWin];
		[image setAction: @selector(eventNotify:)];
		[view addSubview: image];
		
		NSBitmapImageRep *bmp = [[NSBitmapImageRep alloc]	initWithBitmapDataPlanes: NULL
															pixelsWide: width
															pixelsHigh: height
															bitsPerSample: 8
															samplesPerPixel: 3
															hasAlpha: NO
															isPlanar: NO
															colorSpaceName: NSDeviceRGBColorSpace
															bytesPerRow: 3*width
															bitsPerPixel: 3*8];
		
		NSImage *nsimage = [[NSImage alloc] initWithSize: NSMakeSize(width, height)];
		[nsimage addRepresentation: bmp];
	
		[image setImage: nsimage];
		
    [pool release];
		return image;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateButton(void* window, int x, int y, int width, int height, const char *text, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	NSButton* newButton = [[NSButton alloc] initWithFrame: NSMakeRect(x, y, width, height)];
	if (newButton != nil) {
		[newButton setButtonType: NSMomentaryPushInButton];
		[newButton setBezelStyle: NSTexturedRoundedBezelStyle]; // NSRoundedBezelStyle, NSTexturedRoundedBezelStyle, NSRecessedBezelStyle, 
		
		NSString* str = [[NSString alloc] initWithCString: text encoding: NSASCIIStringEncoding];
		[newButton setTitle: str];
		
		[newButton setTarget: myWin];
		[newButton setAction: @selector(eventNotify:)];
		[view addSubview: newButton];
		
    [str release];
    [pool release];
		return newButton;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateLabel(void* window, int x, int y, int width, int height, const char *text, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	NSTextField* label = [[NSTextField alloc] initWithFrame: NSMakeRect(x, y, width, height)];
	if (label != nil) {
		[label setEditable: NO];
		[label setSelectable: YES];
		[label setBezeled: NO];
		[label setDrawsBackground: NO]; 
		
		NSString* str = [[NSString alloc] initWithCString: text encoding: NSASCIIStringEncoding];
		[label setStringValue: str];
		
		[view addSubview: label];
		
    [str release];
    [pool release];
		return label;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateCheckBox(void* window, int x, int y, int width, int height, const char *text, BOOL checked, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	NSButton* newButton = [[NSButton alloc] initWithFrame: NSMakeRect(x, y, width, height)];
	if (newButton != nil) {
		[newButton setButtonType: NSSwitchButton];
		[newButton setBezelStyle: NSTexturedRoundedBezelStyle];
		
		NSString* str = [[NSString alloc] initWithCString: text encoding: NSASCIIStringEncoding];
		[newButton setTitle: str];
		
		[newButton setIntValue: (checked ? 1 : 0)];
		
		[newButton setTarget: myWin];
		[newButton setAction: @selector(eventNotify:)];
		[view addSubview: newButton];
		
    [str release];
    [pool release];
		return newButton;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateTextEdit(void* window, int x, int y, int width, int height, const char *text, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	NSTextField* textEdit = [[NSTextField alloc] initWithFrame: NSMakeRect(x, y, width, height)];
	if (textEdit != nil) {
		[textEdit setEditable: YES];
		[textEdit setBezeled: YES];
		[textEdit setBezelStyle: NSTexturedRoundedBezelStyle];
		
		NSString* str = [[NSString alloc] initWithCString: text encoding: NSASCIIStringEncoding];
		[textEdit setStringValue: str];
		
		[textEdit setTarget: myWin];
		[textEdit setAction: @selector(eventNotify:)];
		[view addSubview: textEdit];
		
    [str release];
    [pool release];
		return textEdit;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateSlider(void* window, int x, int y, int width, int height, int min_value, int max_value, int step, int value, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	MySlider* slider = [[MySlider alloc] initWithFrame: NSMakeRect(x, y, width, height)];
	if (slider != nil) {
		[slider setDragging: FALSE];
		
		[slider setMinValue: min_value];
		[slider setMaxValue: max_value];
		[slider setNumberOfTickMarks: ((max_value - min_value) / step + 1)];
		[slider setAltIncrementValue: step];
		
		[slider setIntValue: value];
		
		[slider setTarget: myWin];
		[slider setAction: @selector(eventNotify:)];
		[view addSubview: slider];
		
    [pool release];
		return slider;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateComboBox(void* window, int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
	
	NSPopUpButton* comboBox = [[NSPopUpButton alloc] initWithFrame: NSMakeRect(x, y, width, height) pullsDown: NO];
	if (comboBox != nil) {
		int i;
		for (i = 0; i < num_entries; i++) {
			NSString* str = [[NSString alloc] initWithCString: entries[i] encoding: NSASCIIStringEncoding];
			[comboBox addItemWithTitle: str];
		}
		
		[comboBox selectItemAtIndex: current_entry];
		
		[comboBox setTarget: myWin];
		[comboBox setAction: @selector(eventNotify:)];
		[view addSubview: comboBox];
		
    [pool release];
		return comboBox;
	}
	
  [pool release];
	return NULL;
}

void* CocoaCreateOpenGLWidget(void* window, int x, int y, int width, int height, void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)window;
	NSWindow* win = [myWin window];
	
	NSView* view;
	if (ptr == NULL)
		view = [win contentView];
	else
		view = [(id)ptr contentView];
		
	NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 32,
		0
	};
	 
	NSOpenGLPixelFormat* pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: attrs];
	
	MyOpenGLView* ogl = [[MyOpenGLView alloc] initWithFrame: NSMakeRect(x, y, width, height) pixelFormat: pixFmt];
	if (ogl != nil) {		
		[view addSubview: ogl];
		
		NSOpenGLContext *context = [ogl openGLContext];
		[context makeCurrentContext];
		
    [pool release];
		return ogl;
	}
	
  [pool release];
	return NULL;
}




int CocoaGetInt(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
  [pool release];
	return [widget intValue];
}

void CocoaGetText(void* ptr, char *str, int len) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	[[widget stringValue] getCString: str maxLength: len encoding: NSASCIIStringEncoding];
  [pool release];
}

void CocoaGetTitle(void* ptr, char *str, int len) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	[[widget title] getCString: str maxLength: len encoding: NSASCIIStringEncoding];
  [pool release];
}

int CocoaGetComboBoxSelection(void *ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
  [pool release];
	return [widget indexOfSelectedItem];
}


void CocoaSetInt(void* ptr, int value) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	[widget setIntValue: value];
  [pool release];
}

void CocoaSetText(void* ptr, const char *str) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	NSString* string = [[NSString alloc] initWithCString: str encoding: NSASCIIStringEncoding];
	[widget setStringValue: string];
  [string release];
  [pool release];
}

void CocoaSetTitle(void* ptr, const char *str) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	NSString* string = [[NSString alloc] initWithCString: str encoding: NSASCIIStringEncoding];
	[widget setTitle: string];
  [string release];
  [pool release];
}

void CocoaSetComboBoxSelection(void *ptr, int value) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	[widget selectItemAtIndex: value];
  [pool release];
}

void CocoaSetImage(void* ptr, int width, int height, unsigned char *pixels) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	
	NSImage *nsimage = [widget image];
	NSBitmapImageRep *bmp = (NSBitmapImageRep*)[[nsimage representations] objectAtIndex: 0];
	unsigned char *bytes = [bmp bitmapData];
	if (bytes) {
		memcpy(bytes, pixels, width*height*3);
	}
	[widget setNeedsDisplay: YES];
  [pool release];
}


void CocoaShow(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	[widget setHidden: NO];
  [pool release];
}

void CocoaHide(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	[widget setHidden: YES];
  [pool release];
}

void CocoaShowWindow(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)ptr;
	NSWindow* widget = [myWin window];
	
	[widget makeKeyAndOrderFront: nil];
	
	[widget makeMainWindow];
	[widget setAcceptsMouseMovedEvents: YES];

	[NSApp activateIgnoringOtherApps: YES];
  [pool release];
}

void CocoaHideWindow(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MyWindow* myWin = (MyWindow*)ptr;
	id widget = [myWin window];
	[widget orderOut: widget];
  [pool release];
}

int CocoaGetModifierKeyState() {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSUInteger mod = [[NSApp currentEvent] modifierFlags];

	BOOL shiftKeyDown = (mod & (NSShiftKeyMask | NSAlphaShiftKeyMask)) != 0;
	BOOL controlKeyDown = (mod & NSControlKeyMask) != 0;
	BOOL altKeyDown = (mod & NSAlternateKeyMask) != 0;

	int result = 0;
	if (shiftKeyDown) result |= 1;
	if (controlKeyDown) result |= 2;
	if (altKeyDown) result |= 4;

  [pool release];
	return result;
}

void CocoaSwapBuffers(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	NSOpenGLContext *context = [widget openGLContext];
	[context flushBuffer];
	[widget setNeedsDisplay: YES];
  [pool release];
}

void CocoaMakeCurrent(void* ptr) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	id widget = (id)ptr;
	NSOpenGLContext *context = [widget openGLContext];
	[context makeCurrentContext];
  [pool release];
}



void CocoaGetCurrentWorkingDirectory(char* str, int len) {
	NSAutoreleasePool* myPool = [[NSAutoreleasePool alloc] init];
	
	NSString* path = [[NSBundle mainBundle] bundlePath];
	NSString* cwd = [[NSFileManager defaultManager] currentDirectoryPath];
	
	/* // debug print the two paths
	char buf[1024];
	[path getCString: buf maxLength: 1024 encoding: NSASCIIStringEncoding];
	printf("path = '%s'\n", buf);
	[cwd getCString: buf maxLength: 1024 encoding: NSASCIIStringEncoding];
	printf("cwd = '%s'\n", buf);
	*/

	// if the two paths match, then we're not starting from within an application bundle
	// if we start from within a bundle, the cwd is '/' and we would like to have the path
	// to our application which we extract from the path to the bundle, which is in 'path'
	// if we're starting from the console 'path' equals the current working directory we
	// like to have.
	if ([path compare: cwd] != NSOrderedSame) {
		path = [path stringByDeletingLastPathComponent];
	}
	[path getCString: str maxLength: len encoding: NSASCIIStringEncoding];
	
	[myPool release];
}

