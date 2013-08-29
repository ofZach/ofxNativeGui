// ****************************************************************************
// Filename:  GTKGLContext.cpp
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


#include "GTKGLContext.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

CGTKGLContext::CGTKGLContext(CGTKGLContext *shared_context)
: initialized(false), xdisplay(NULL)
{
	if (glXQueryExtension(GDK_DISPLAY(), NULL, NULL) == FALSE)
	{
		printf("CGTKGLContext: no OpenGL support.\n");
		return;
	}
	
	xdisplay = GDK_DISPLAY();

	int attrlist[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 16, None};
	
	XVisualInfo *vi = glXChooseVisual(xdisplay, DefaultScreen(xdisplay), attrlist);
	if (!vi)
	{
		printf("CGTKGLContext: couldn't find suitable VisualInfo for the GL context. (%d)\n", (size_t) xdisplay);
		return;
	}

	if (shared_context)
		glxcontext = glXCreateContext(xdisplay, vi, shared_context->glxcontext, True);
	else
		glxcontext = glXCreateContext(xdisplay, vi, 0, True);
	
	if (glxcontext == NULL)
	{
		printf("CGTKGLContext: creation of GL context failed.\n");
		XFree(vi);
		return;
	}
	
	visual = gdkx_visual_get(vi->visualid);
	XFree(vi);
	
	initialized = true;
}

CGTKGLContext::~CGTKGLContext()
{
	if (!initialized)
		return;
  
	if (glxcontext == glXGetCurrentContext())
		glXMakeCurrent(xdisplay, None, NULL);

	glXDestroyContext(xdisplay, glxcontext);
}

bool CGTKGLContext::IsInitialized()
{
	return initialized;
}

void CGTKGLContext::MakeCurrent(GdkDrawable *drawable)
{
	if (!initialized)
		return;
	
	if (drawable)
	{
		if (glXMakeCurrent(xdisplay, GDK_WINDOW_XWINDOW(drawable), glxcontext) == False)
		{
			printf("GTKGLContext: make current failed\n");
		}
	}
}

void CGTKGLContext::SwapBuffers(GdkDrawable *drawable)
{
	if (!initialized)
		return;
	
	glXSwapBuffers(GDK_WINDOW_XDISPLAY(drawable), GDK_WINDOW_XWINDOW(drawable));
}

void CGTKGLContext::DoneCurrent()
{
	if (!initialized)
		return;
		
	glXMakeCurrent(xdisplay, None, NULL);
}

