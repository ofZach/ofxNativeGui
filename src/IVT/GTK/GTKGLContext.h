// ****************************************************************************
// Filename:  GTKGLContext.h
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


#ifndef _GTK_GL_CONTEXT_H_
#define _GTK_GL_CONTEXT_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <GL/glx.h>
#include <GL/gl.h>


// ****************************************************************************
// Forward declarations and typedefs
// ****************************************************************************

struct _GdkDrawable;
typedef _GdkDrawable GdkDrawable;

typedef _GdkDrawable GdkPixmap;

typedef _GdkDrawable GdkWindow;

struct _GdkVisual;
typedef _GdkVisual GdkVisual;



// ****************************************************************************
// CGTKGLContext
// ****************************************************************************

class CGTKGLContext
{
public:
	CGTKGLContext(CGTKGLContext *shared_context = NULL);
	~CGTKGLContext();
	
	bool IsInitialized();
	
	void MakeCurrent(GdkDrawable *drawable);
	void SwapBuffers(GdkDrawable *drawable);
	void DoneCurrent();
	
	GdkVisual *GetVisual() { return visual; }

private:
	bool		initialized;
	Display		*xdisplay;
	GLXContext	glxcontext;
	GdkVisual	*visual;
};



#endif /* _GTK_GL_CONTEXT_H_ */
