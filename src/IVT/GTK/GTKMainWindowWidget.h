// ****************************************************************************
// Filename:  GTKMainWindowWidget.h
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


#ifndef _GTK_MAIN_WINDOW_WIDGET_H_
#define _GTK_MAIN_WINDOW_WIDGET_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <gtk/gtk.h>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CGTKMainWindow;
class CByteImage;


// ****************************************************************************
// Enums
// ****************************************************************************

enum GTKWidgetType
{
	eImage = 0,
	eButton,
	eLabel,
	eCheckBox,
	eTextEdit,
	eSlider,
	eComboBox,
	eGLWidget
};



// ****************************************************************************
// CGTKMainWindowWidget
// ****************************************************************************

class CGTKMainWindowWidget
{
public:
	CGTKMainWindowWidget(CGTKMainWindow *main_window, GTKWidgetType type);
	~CGTKMainWindowWidget();

public:
	void Clicked();
	void ValueChanged(int value);
	void Toggled(bool flag);
	void TextChanged(const char *str);
	
public:
	CGTKMainWindow	*m_main_window;
	GTKWidgetType	m_type;
	GtkWidget	*m_widget;
	GtkObject	*m_obj;
};

// *****************************************************************
// New GTK classes: IVTImage and IVTGLWidget
// *****************************************************************

#include <glib.h>

G_BEGIN_DECLS

// *****************************************************************
// IVTImage
// *****************************************************************

#define IVTIMAGE_TYPE            (ivtimage_get_type ())
#define IVTIMAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IVTIMAGE_TYPE, IVTImage))
#define IVTIMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IVTIMAGE_TYPE, IVTImageClass))
#define IS_IVTIMAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IVTIMAGE_TYPE))
#define IS_IVTIMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IVTIMAGE_TYPE))


typedef struct _IVTImage       IVTImage;
typedef struct _IVTImageClass  IVTImageClass;

struct _IVTImage
{
	GtkDrawingArea parent;

	GdkPixmap *pixmap;
	CByteImage *image;
	CGTKMainWindowWidget *main_window_widget;

	int mouse_down;
	int mouse_start_x;
	int mouse_start_y;
	int mouse_x;
	int mouse_y;
};

struct _IVTImageClass
{
	GtkDrawingAreaClass parent_class;

	void (* ivtimage) (IVTImage *ttt);
};

GType          ivtimage_get_type        (void);
GtkWidget*     ivtimage_new             (void);

#ifdef USE_OPENGL

// *****************************************************************
// IVTGLWidget
// *****************************************************************

class CGTKGLContext;

#define IVTGLWIDGET_TYPE            (ivtglwidget_get_type ())
#define IVTGLWIDGET(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IVTGLWIDGET_TYPE, IVTGLWidget))
#define IVTGLWIDGET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IVTGLWIDGET_TYPE, IVTGLWidgetClass))
#define IS_IVTGLWIDGET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IVTGLWIDGET_TYPE))
#define IS_IVTGLWIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IVTGLWIDGET_TYPE))


typedef struct _IVTGLWidget       IVTGLWidget;
typedef struct _IVTGLWidgetClass  IVTGLWidgetClass;

struct _IVTGLWidget
{
	GtkDrawingArea parent;

	CGTKGLContext *glcontext;
	CGTKMainWindowWidget *main_window_widget;
};

struct _IVTGLWidgetClass
{
	GtkDrawingAreaClass parent_class;

	void (* ivtglwidget) (IVTGLWidget *ttt);
};

GType          ivtglwidget_get_type        (void);
GtkWidget*     ivtglwidget_new             (void);

#endif /* USE_OPENGL */

G_END_DECLS

#endif /* _GTK_MAIN_WINDOW_WIDGET_H_ */
