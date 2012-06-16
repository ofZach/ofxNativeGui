// ****************************************************************************
// Filename:  GTKMainWindowWidget.cpp
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


#include "GTKMainWindowWidget.h"
#include "GTKMainWindow.h"

#include "Interfaces/MainWindowEventInterface.h"
#include "Image/ByteImage.h"


CGTKMainWindowWidget::CGTKMainWindowWidget(CGTKMainWindow *main_window, GTKWidgetType type)
: m_main_window(main_window), m_type(type), m_widget(NULL), m_obj(NULL)
{
}

CGTKMainWindowWidget::~CGTKMainWindowWidget()
{
	if (m_type == eImage)
	{
		delete ((IVTImage*)m_widget)->image;
		((IVTImage*)m_widget)->image = NULL;
	}
}

void CGTKMainWindowWidget::Clicked()
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && m_type == eButton)
	{
		ec->ButtonPushed(this);
	}
}
void CGTKMainWindowWidget::ValueChanged(int value)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && (m_type == eSlider || m_type == eComboBox))
	{
		ec->ValueChanged(this, value);
	}
}
void CGTKMainWindowWidget::Toggled(bool flag)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && m_type == eCheckBox)
	{
		ec->ValueChanged(this, (flag ? 1 : 0));
	}
}
void CGTKMainWindowWidget::TextChanged(const char *str)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && m_type == eTextEdit)
	{
		ec->ValueChanged(this, -1);
	}
}


static void ivtimage_class_init          (IVTImageClass *klass);
static void ivtimage_init                (IVTImage      *ttt);

GType ivtimage_get_type(void)
{
  static GType ttt_type = 0;

  if (!ttt_type)
    {
      static const GTypeInfo ttt_info =
      {
	sizeof (IVTImageClass),
	NULL, /* base_init */
	NULL, /* base_finalize */
	(GClassInitFunc) ivtimage_class_init,
	NULL, /* class_finalize */
	NULL, /* class_data */
	sizeof (IVTImage),
	0,    /* n_preallocs */
	(GInstanceInitFunc) ivtimage_init,
      };

      ttt_type = g_type_register_static (GTK_TYPE_DRAWING_AREA,
                                         "IVTImage",
                                         &ttt_info,
                                         (GTypeFlags)0);
    }

  return ttt_type;
}

static void ivtimage_class_init (IVTImageClass *klass)
{

}

static void ivtimage_init (IVTImage *ttt)
{
	ttt->pixmap = NULL;
	ttt->image = NULL;
	ttt->main_window_widget = NULL;
	
	ttt->mouse_down = 0;
	
	GTK_WIDGET_SET_FLAGS(ttt, GTK_CAN_FOCUS);
}

GtkWidget* ivtimage_new ()
{
	return GTK_WIDGET (g_object_new (ivtimage_get_type (), NULL));
}


#ifdef USE_OPENGL

#include "GTKGLContext.h"

static void ivtglwidget_class_init	(IVTGLWidgetClass	*klass);
static void ivtglwidget_init		(IVTGLWidget		*ttt);
static void ivtglwidget_destroy		(GtkObject		*object);

static GtkDrawingAreaClass *parent_class = NULL;

GType ivtglwidget_get_type(void)
{
  static GType ttt_type = 0;

  if (!ttt_type)
    {
      static const GTypeInfo ttt_info =
      {
	sizeof (IVTGLWidgetClass),
	NULL, /* base_init */
	NULL, /* base_finalize */
	(GClassInitFunc) ivtglwidget_class_init,
	NULL, /* class_finalize */
	NULL, /* class_data */
	sizeof (IVTGLWidget),
	0,    /* n_preallocs */
	(GInstanceInitFunc) ivtglwidget_init,
      };

      ttt_type = g_type_register_static (GTK_TYPE_DRAWING_AREA,
                                         "IVTGLWidget",
                                         &ttt_info,
                                         (GTypeFlags)0);
    }

  return ttt_type;
}

static void ivtglwidget_class_init (IVTGLWidgetClass *klass)
{
	GtkObjectClass *object_class;

	parent_class = (GtkDrawingAreaClass*)g_type_class_peek_parent(klass);
	object_class = (GtkObjectClass*) klass;

	object_class->destroy = ivtglwidget_destroy;
}

static void ivtglwidget_init (IVTGLWidget *ttt)
{
	ttt->glcontext = NULL;
	ttt->main_window_widget = NULL;
	
	GTK_WIDGET_SET_FLAGS(ttt, GTK_CAN_FOCUS);
}

static void ivtglwidget_destroy(GtkObject *object)
{
	IVTGLWidget *glw;

	g_return_if_fail (object != NULL);
	g_return_if_fail (IS_IVTGLWIDGET(object));

	glw = IVTGLWIDGET(object);

	if (glw->glcontext)
		delete glw->glcontext;
	glw->glcontext = NULL;

	if (GTK_OBJECT_CLASS (parent_class)->destroy)
		(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}
GtkWidget* ivtglwidget_new ()
{
	IVTGLWidget *glw = (IVTGLWidget*)g_object_new (ivtglwidget_get_type (), NULL);
	
	return (GtkWidget*)glw;
}

#endif /* USE_OPENGL */


