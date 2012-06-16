// ****************************************************************************
// Filename:  GTKMainWindow.cpp
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "GTKMainWindow.h"
#include "GTKMainWindowWidget.h"
#include "GTKApplicationHandler.h"
#include "Interfaces/MainWindowEventInterface.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"

#ifdef USE_OPENGL
#include "GTKGLContext.h"
#endif

#include <string.h>



static unsigned int last_modifier_key_state = 0;

static void button_pressed_callback(GtkWidget *widget, gpointer data)
{
	CGTKMainWindowWidget *ptr = (CGTKMainWindowWidget*)data;
	if (ptr)
		ptr->Clicked();
}

static void checkbox_toggled_callback(GtkWidget *widget, gpointer data)
{
    	int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	CGTKMainWindowWidget *ptr = (CGTKMainWindowWidget*)data;
	if (ptr)
		ptr->Toggled(state > 0);
}

static void slider_changed_callback(GtkWidget *widget, gpointer data)
{
    	CGTKMainWindowWidget *ptr = (CGTKMainWindowWidget*)data;
    	
	if (ptr)
	{
		int value = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(ptr->m_obj));
		ptr->ValueChanged(value);
	}
}

static void combo_box_changed_callback(GtkWidget *widget, gpointer data)
{
    	CGTKMainWindowWidget *ptr = (CGTKMainWindowWidget*)data;
    	
	if (ptr)
	{
		int value = gtk_combo_box_get_active(GTK_COMBO_BOX(ptr->m_widget));
		ptr->ValueChanged(value);
	}
}

static void text_edit_changed_callback(GtkWidget *widget, gpointer data)
{
    	CGTKMainWindowWidget *ptr = (CGTKMainWindowWidget*)data;
    	
	if (ptr)
	{
		ptr->TextChanged(NULL);
	}
}

static gboolean ivtimage_configure_event(GtkWidget *widget, GdkEventConfigure *event)
{
	IVTImage *ptr = (IVTImage *)widget;

	if (ptr->pixmap)
		g_object_unref(ptr->pixmap);

	ptr->pixmap = gdk_pixmap_new(widget->window,
			  widget->allocation.width,
			  widget->allocation.height,
			  -1);
	gdk_draw_rectangle(ptr->pixmap,
		      widget->style->black_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);

	return TRUE;
}

static gboolean ivtimage_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
	IVTImage *ptr = (IVTImage *)widget;
	
	GdkPixmap *pixmap = ptr->pixmap;
	
	if (ptr->image == 0)
	{
		gdk_draw_drawable(widget->window,
			    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			    pixmap,
			    event->area.x, event->area.y,
			    event->area.x, event->area.y,
			    event->area.width, event->area.height);
	}
	else
	{
		gdk_draw_rgb_image(widget->window,
			    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			    0, 0, ptr->image->width, ptr->image->height,
			    GDK_RGB_DITHER_NONE, ptr->image->pixels, ptr->image->width*3);
	}
	
	if (ptr->mouse_down == 1)
	{
		int x0 = ptr->mouse_start_x;
		int y0 = ptr->mouse_start_y;
		int x1 = ptr->mouse_x;
		int y1 = ptr->mouse_y;
		
		if (x0 > x1)
		{
			int tmp = x0;
			x0 = x1;
			x1 = tmp;
		}
		if (y0 > y1)
		{
			int tmp = y0;
			y0 = y1;
			y1 = tmp;
		}
		
		int dx = x1 - x0;
		int dy = y1 - y0;
		
		GdkGC *gc = widget->style->white_gc;
		gdk_gc_set_function(gc, GDK_XOR);
		gdk_draw_rectangle(widget->window,
			      gc,
			      FALSE,
			      x0, y0,
			      dx, dy);
		gdk_gc_set_function(gc, GDK_COPY);
	}

	return FALSE;
}

static gboolean ivtimage_button_press_event(GtkWidget *widget, GdkEventButton *event)
{
	IVTImage *ptr = (IVTImage *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;

	gtk_widget_grab_focus(widget);

	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		int btn = 0;
		if (event->button == 1) btn = IVT_LEFT_BUTTON;
		if (event->button == 3) btn = IVT_RIGHT_BUTTON;
		if (event->button == 2) btn = IVT_MIDDLE_BUTTON;
		
		callback->MouseDown((WIDGET_HANDLE)ptr->main_window_widget, btn, (int)event->x, (int)event->y);
	}
		
	if (event->button == 1)
	{
		ptr->mouse_down = 1;
		ptr->mouse_start_x = event->x;
		ptr->mouse_start_y = event->y;
		ptr->mouse_x = event->x;
		ptr->mouse_y = event->y;
	}

	return TRUE;
}

static gboolean ivtimage_button_release_event(GtkWidget *widget, GdkEventButton *event)
{
	IVTImage *ptr = (IVTImage *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		int btn = 0;
		if (event->button == 1) btn = IVT_LEFT_BUTTON;
		if (event->button == 3) btn = IVT_RIGHT_BUTTON;
		if (event->button == 2) btn = IVT_MIDDLE_BUTTON;
		
		callback->MouseUp((WIDGET_HANDLE)ptr->main_window_widget, btn, (int)event->x, (int)event->y);
	}
	
	if (ptr->mouse_down == 1 && event->button == 1)
	{
		ptr->mouse_down = 0;
		
		int x0 = ptr->mouse_start_x;
		int y0 = ptr->mouse_start_y;
		int x1 = ptr->mouse_x;
		int y1 = ptr->mouse_y;
		
		if (x0 > x1)
		{
			int tmp = x0;
			x0 = x1;
			x1 = tmp;
		}
		if (y0 > y1)
		{
			int tmp = y0;
			y0 = y1;
			y1 = tmp;
		}
		
		int dx = x1 - x0;
		int dy = y1 - y0;
		
		CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
		if (callback)
		{
			if (dx < 5 && dy < 5)
			{
				callback->PointClicked((WIDGET_HANDLE)ptr->main_window_widget, (x0 + x1) / 2, (y0 + y1) / 2);
			}
			else
			{
				callback->RectSelected((WIDGET_HANDLE)ptr->main_window_widget, x0, y0, x1, y1);
			}
		}
		
		gtk_widget_queue_draw_area(widget, x0, y0, dx, dy);
		
	}

	return TRUE;
}

static gboolean ivtimage_motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
	IVTImage *ptr = (IVTImage *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		int x, y;
		GdkModifierType state;

		if (event->is_hint)
			gdk_window_get_pointer(event->window, &x, &y, &state);
		else
		{
			x = (int)event->x;
			y = (int)event->y;
			state = (GdkModifierType)event->state;
		}
		
		callback->MouseMove((WIDGET_HANDLE)ptr->main_window_widget, x, y);
	}
	
	if (ptr->mouse_down != 1)
		return FALSE;
		
	int x, y;
	GdkModifierType state;

	if (event->is_hint)
		gdk_window_get_pointer(event->window, &x, &y, &state);
	else
	{
		x = event->x;
		y = event->y;
		state = (GdkModifierType)event->state;
	}

	if (state & GDK_BUTTON1_MASK)
	{
		ptr->mouse_x = x;
		ptr->mouse_y = y;
		
		gtk_widget_queue_draw_area(widget, 0, 0, ptr->image->width, ptr->image->height);
	}

	return TRUE;
}

static gboolean ivtimage_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
	IVTImage *ptr = (IVTImage *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		if (event->length != 1)
		{
			callback->KeyDown((WIDGET_HANDLE)ptr->main_window_widget, -1);
		}
		else
		{
			callback->KeyDown((WIDGET_HANDLE)ptr->main_window_widget, event->string[0]);
		}
	}

	return TRUE;
}

static gboolean ivtimage_key_release_event(GtkWidget *widget, GdkEventKey *event)
{
	IVTImage *ptr = (IVTImage *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		if (event->length != 1)
		{
			callback->KeyUp((WIDGET_HANDLE)ptr->main_window_widget, -1);
		}
		else
		{
			callback->KeyUp((WIDGET_HANDLE)ptr->main_window_widget, event->string[0]);
		}
	}

	return TRUE;
}


#ifdef USE_OPENGL


static gboolean ivtglwidget_configure_event(GtkWidget *widget, GdkEventConfigure *event)
{
	IVTGLWidget *ptr = (IVTGLWidget *)widget;

	if (ptr->glcontext)
	{
		// the first time the widget becomes visible we make it the current gl context
		ptr->glcontext->MakeCurrent(widget->window);
	}

	return TRUE;
}

static gboolean ivtglwidget_button_press_event(GtkWidget *widget, GdkEventButton *event)
{
	IVTGLWidget *ptr = (IVTGLWidget *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	gtk_widget_grab_focus(widget);

	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		int btn = 0;
		if (event->button == 1) btn = IVT_LEFT_BUTTON;
		if (event->button == 3) btn = IVT_RIGHT_BUTTON;
		if (event->button == 2) btn = IVT_MIDDLE_BUTTON;
		
		callback->MouseDown((WIDGET_HANDLE)ptr->main_window_widget, btn, (int)event->x, (int)event->y);
	}

	return TRUE;
}

static gboolean ivtglwidget_button_release_event(GtkWidget *widget, GdkEventButton *event)
{
	IVTGLWidget *ptr = (IVTGLWidget *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		int btn = 0;
		if (event->button == 1) btn = IVT_LEFT_BUTTON;
		if (event->button == 3) btn = IVT_RIGHT_BUTTON;
		if (event->button == 2) btn = IVT_MIDDLE_BUTTON;
		
		callback->MouseUp((WIDGET_HANDLE)ptr->main_window_widget, btn, (int)event->x, (int)event->y);
	}

	return TRUE;
}

static gboolean ivtglwidget_motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
	IVTGLWidget *ptr = (IVTGLWidget *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		int x, y;
		GdkModifierType state;

		if (event->is_hint)
			gdk_window_get_pointer(event->window, &x, &y, &state);
		else
		{
			x = (int)event->x;
			y = (int)event->y;
			state = (GdkModifierType)event->state;
		}
		
		callback->MouseMove((WIDGET_HANDLE)ptr->main_window_widget, x, y);
	}

	return TRUE;
}

static gboolean ivtglwidget_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
	IVTGLWidget *ptr = (IVTGLWidget *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		if (event->length != 1)
		{
			callback->KeyDown((WIDGET_HANDLE)ptr->main_window_widget, -1);
		}
		else
		{
			callback->KeyDown((WIDGET_HANDLE)ptr->main_window_widget, event->string[0]);
		}
	}

	return TRUE;
}

static gboolean ivtglwidget_key_release_event(GtkWidget *widget, GdkEventKey *event)
{
	IVTGLWidget *ptr = (IVTGLWidget *)widget;
	
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	last_modifier_key_state = event->state & modifiers;
	
	CMainWindowEventInterface *callback = ptr->main_window_widget->m_main_window->GetEventCallback();
	if (callback)
	{
		if (event->length != 1)
		{
			callback->KeyUp((WIDGET_HANDLE)ptr->main_window_widget, -1);
		}
		else
		{
			callback->KeyUp((WIDGET_HANDLE)ptr->main_window_widget, event->string[0]);
		}
	}

	return TRUE;
}

#endif /* USE_OPENGL */


static int destroy_window_callback(GtkWidget *widget, gpointer data)
{
	// process internal things (reference counter, exit flag)
	CGTKMainWindow *pMainWindow = (CGTKMainWindow *) data;
	pMainWindow->Destroy(widget);
	
	// destroy widget
	gtk_widget_destroy(widget);
	
	return 0;
}


int CGTKMainWindow::m_ref_count = 0;

void CGTKMainWindow::Destroy(GtkWidget *widget)
{
	// set flag, so that the destructor knows it doesn't have to destroy m_main_window again
	if (widget == m_main_window)
		m_bDestroyed = true;
	
	if (--m_ref_count == 0)
	{
		CGTKApplicationHandler *pApplicationHandler = CGTKApplicationHandler::GetApplicationHandler();
		if (pApplicationHandler)
			pApplicationHandler->SetExit(true);
	}
}


CGTKMainWindow::CGTKMainWindow(int x, int y, int width, int height, const char *title)
{
	m_main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(m_main_window), title);
	gtk_window_set_default_size(GTK_WINDOW(m_main_window), width, height);
	gtk_window_set_position(GTK_WINDOW(m_main_window), GTK_WIN_POS_NONE);
	gtk_window_move(GTK_WINDOW(m_main_window), x, y);

	g_signal_connect(G_OBJECT(m_main_window), "destroy", G_CALLBACK(destroy_window_callback), this);
	
	m_fixed_container = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(m_main_window), m_fixed_container);
	gtk_widget_show(m_fixed_container);
	
	m_event_callback = NULL;
	m_ref_count++;
	
	m_bDestroyed = false;
}

CGTKMainWindow::~CGTKMainWindow()
{
	// delete widgets in inverse order
	const int nWidgets = m_widgets.size();
	for (int i = nWidgets - 1; i >= 0; i--)
		delete m_widgets[i];
	
	// clear list
	m_widgets.clear();
	
	// check if m_main_window has already been destroyed (by closing the window)
	if (!m_bDestroyed)
	{
		// m_main_window has not been destroyed yet, destroy it now
		destroy_window_callback(m_main_window, this); // has same effect
	
		// process pending events (otherwise window the won't close)
		while (gtk_events_pending())
			gtk_main_iteration();
	}
}

// create widgets
WIDGET_HANDLE CGTKMainWindow::AddImage(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eImage);
	
	GtkWidget *image;
	image = ivtimage_new();
	
	gtk_signal_connect(GTK_OBJECT(image), "expose_event", (GtkSignalFunc)ivtimage_expose_event, NULL);
	gtk_signal_connect(GTK_OBJECT(image), "configure_event", (GtkSignalFunc)ivtimage_configure_event, NULL);
	gtk_signal_connect(GTK_OBJECT(image), "motion_notify_event", (GtkSignalFunc)ivtimage_motion_notify_event, NULL);
	gtk_signal_connect(GTK_OBJECT(image), "button_press_event", (GtkSignalFunc)ivtimage_button_press_event, NULL);
	gtk_signal_connect(GTK_OBJECT(image), "button_release_event", (GtkSignalFunc)ivtimage_button_release_event, NULL);
	gtk_signal_connect(GTK_OBJECT(image), "key_press_event", (GtkSignalFunc)ivtimage_key_press_event, NULL);
	gtk_signal_connect(GTK_OBJECT(image), "key_release_event", (GtkSignalFunc)ivtimage_key_release_event, NULL);

	gtk_widget_set_events(image, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_KEY_PRESS_MASK
			 | GDK_KEY_RELEASE_MASK);

	
	gtk_fixed_put(GTK_FIXED(m_fixed_container), image, x, y);
	gtk_drawing_area_size(GTK_DRAWING_AREA(image), width, height);
	
	((IVTImage*)image)->image = new CByteImage(width, height, CByteImage::eRGB24);
	((IVTImage*)image)->main_window_widget = w;
	
	gtk_widget_show(image);
	
	w->m_widget = image;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddButton(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eButton);
	
	GtkWidget *button;
	button = gtk_button_new_with_label(text);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_pressed_callback), w);
	gtk_fixed_put(GTK_FIXED(m_fixed_container), button, x, y);
	gtk_widget_set_size_request(button, width, height);
	gtk_widget_show(button);
	
	w->m_widget = button;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddLabel(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eLabel);
	
	GtkWidget *label;
	label = gtk_label_new(text);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0f, 0.5f);
	gtk_fixed_put(GTK_FIXED(m_fixed_container), label, x, y);
	gtk_widget_set_size_request(label, width, height);
	gtk_widget_show(label);
	
	w->m_widget = label;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddCheckBox(int x, int y, int width, int height, const char *text, bool checked, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eCheckBox);
	
	GtkWidget *check_button;
	check_button = gtk_check_button_new_with_label(text);
	g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(checkbox_toggled_callback), w);
	gtk_fixed_put(GTK_FIXED(m_fixed_container), check_button, x, y);
	gtk_widget_set_size_request(check_button, width, height);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), checked);
	gtk_widget_show(check_button);
	
	w->m_widget = check_button;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddTextEdit(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eTextEdit);
	
	GtkWidget *edit;
	edit = gtk_entry_new();
	
	gtk_entry_set_text(GTK_ENTRY(edit), text);
	
	g_signal_connect(G_OBJECT(edit), "changed", G_CALLBACK(text_edit_changed_callback), w);
	
	gtk_fixed_put(GTK_FIXED(m_fixed_container), edit, x, y);
	gtk_widget_set_size_request(edit, width, height);
	gtk_widget_show(edit);
	
	w->m_widget = edit;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddSlider(int x, int y, int width, int height, int min_value, int max_value, int step, int value, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eSlider);
	
	GtkObject *adjustment;
	adjustment = gtk_adjustment_new(value, min_value, max_value, 1.0, step, 1.0);
	g_signal_connect(G_OBJECT(adjustment), "value_changed", G_CALLBACK(slider_changed_callback), w);

	GtkWidget *slider;
	slider = gtk_hscale_new(GTK_ADJUSTMENT(adjustment));
	gtk_scale_set_draw_value(GTK_SCALE(slider), FALSE);
	
	gtk_fixed_put(GTK_FIXED(m_fixed_container), slider, x, y);
	gtk_widget_set_size_request(slider, width, height);
	gtk_widget_show(slider);
	
	w->m_widget = slider;
	w->m_obj = adjustment;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddComboBox(int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, WIDGET_HANDLE parent)
{
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eComboBox);
	
	GtkWidget *cbox;
	cbox = gtk_combo_box_new_text();
	
	for (int i = 0; i < num_entries; i++)
	{
		gtk_combo_box_append_text(GTK_COMBO_BOX(cbox), entries[i]);
	}
	
	gtk_combo_box_set_active(GTK_COMBO_BOX(cbox), current_entry);
	
	g_signal_connect(G_OBJECT(cbox), "changed", G_CALLBACK(combo_box_changed_callback), w);
	
	gtk_fixed_put(GTK_FIXED(m_fixed_container), cbox, x, y);
	gtk_widget_set_size_request(cbox, width, height);
	gtk_widget_show(cbox);
	
	w->m_widget = cbox;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CGTKMainWindow::AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
#ifdef USE_OPENGL
	CGTKMainWindowWidget *w = new CGTKMainWindowWidget(this, eGLWidget);
	
	CGTKGLContext *glcontext = new CGTKGLContext();
	if (!glcontext->IsInitialized())
	{
		printf("CGTKMainWindow::AddGLWidget: creation of GLWidget failed\n");
		return NULL;
	}
	
	// get the visual for the OpenGL context
	GdkVisual *visual = glcontext->GetVisual();
	
	// use a colormap and the visual that is used for the OpenGL context
	gtk_widget_push_colormap(gdk_colormap_new(visual, TRUE));
	gtk_widget_push_visual(visual);

	GtkWidget *glw;
	glw = ivtglwidget_new();
	
	// pop back the default visual settings
	gtk_widget_pop_visual();
	gtk_widget_pop_colormap();
	
	gtk_signal_connect(GTK_OBJECT(glw), "configure_event", (GtkSignalFunc)ivtglwidget_configure_event, NULL);
	gtk_signal_connect(GTK_OBJECT(glw), "motion_notify_event", (GtkSignalFunc)ivtglwidget_motion_notify_event, NULL);
	gtk_signal_connect(GTK_OBJECT(glw), "button_press_event", (GtkSignalFunc)ivtglwidget_button_press_event, NULL);
	gtk_signal_connect(GTK_OBJECT(glw), "button_release_event", (GtkSignalFunc)ivtglwidget_button_release_event, NULL);
	gtk_signal_connect(GTK_OBJECT(glw), "key_press_event", (GtkSignalFunc)ivtglwidget_key_press_event, NULL);
	gtk_signal_connect(GTK_OBJECT(glw), "key_release_event", (GtkSignalFunc)ivtglwidget_key_release_event, NULL);

	gtk_widget_set_events(glw, GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_KEY_PRESS_MASK
			 | GDK_KEY_RELEASE_MASK);

	
	gtk_fixed_put(GTK_FIXED(m_fixed_container), glw, x, y);
	gtk_drawing_area_size(GTK_DRAWING_AREA(glw), width, height);
	
	((IVTGLWidget*)glw)->main_window_widget = w;
	((IVTGLWidget*)glw)->glcontext = glcontext;
	
	gtk_widget_show(glw);
	
	// this forces the window to be created, so we can call MakeCurrent
	gtk_widget_realize(glw);
	((IVTGLWidget*)glw)->glcontext->MakeCurrent(glw->window);
	
	w->m_widget = glw;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
#else
	printf("WARNING: CGTKMainWindow::AddGLWidget has been called, but USE_OPENGL = 0 in Makefile.base\n");
	return (WIDGET_HANDLE)NULL;

#endif
}


// access to widget attributes
bool CGTKMainWindow::GetText(WIDGET_HANDLE widget, char *text, int len)
{
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eLabel)
	{
		const char *ptr = gtk_label_get_text(GTK_LABEL(w->m_widget));
		strncpy(text, ptr, len);
		
		return true;
	}
	else if (w->m_type == eButton || w->m_type == eCheckBox)
	{
		const char *ptr = gtk_button_get_label(GTK_BUTTON(w->m_widget));
		strncpy(text, ptr, len);
		
		return true;
	}
	else if (w->m_type == eTextEdit)
	{
		const char *ptr = gtk_entry_get_text(GTK_ENTRY(w->m_widget));
		strncpy(text, ptr, len);
		
		return true;
	}
	
	return false;
}
bool CGTKMainWindow::SetText(WIDGET_HANDLE widget, const char *text)
{
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eLabel)
	{
		gtk_label_set_text(GTK_LABEL(w->m_widget), text);
		
		return true;
	}
	else if (w->m_type == eButton || w->m_type == eCheckBox)
	{
		gtk_button_set_label(GTK_BUTTON(w->m_widget), text);
		
		return true;
	}
	else if (w->m_type == eTextEdit)
	{
		gtk_entry_set_text(GTK_ENTRY(w->m_widget), text);
		
		return true;
	}
	
	return false;
}

bool CGTKMainWindow::SetImage(WIDGET_HANDLE widget, const CByteImage *pImage)
{
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eImage)
	{
		CByteImage *bi = ((IVTImage*)w->m_widget)->image;
		
		if (bi->width != pImage->width || bi->height != pImage->height)
		{
			printf("error: SetImage: image size does not match size of image widget\n");
			return false;
		}
		
		if (bi->type != pImage->type)
		{
			ImageProcessor::ConvertImage(pImage, bi);
		}
		else
		{
			ImageProcessor::CopyImage(pImage, bi);
		}
		
		// init redraw
		gtk_widget_queue_draw_area(w->m_widget, 0, 0, bi->width, bi->height);

	}
	
	return false;
}

bool CGTKMainWindow::GetValue(WIDGET_HANDLE widget, int &value)
{
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eCheckBox)
	{
		value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->m_widget));
		return true;
	}
	else if (w->m_type == eSlider)
	{
		value = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(w->m_obj));
		return true;
	}
	else if (w->m_type == eComboBox)
	{
		value = gtk_combo_box_get_active(GTK_COMBO_BOX(w->m_widget));
		return true;
	}
	
	return false;
}

bool CGTKMainWindow::SetValue(WIDGET_HANDLE widget, int value)
{
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eCheckBox)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w->m_widget), value);
		return true;
	}
	else if (w->m_type == eSlider)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(w->m_obj), (double)value);
		return true;
	}
	else if (w->m_type == eComboBox)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(w->m_widget), value);
		return true;
	}
	
	return false;
}

bool CGTKMainWindow::SwapBuffersGLWidget(WIDGET_HANDLE widget)
{
#ifdef USE_OPENGL
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eGLWidget)
	{
		IVTGLWidget* glw = (IVTGLWidget*)w->m_widget;
		
		if (glw->glcontext)
			glw->glcontext->SwapBuffers(GTK_WIDGET(glw)->window);
		
		return true;
	}
#endif
	
	return false;
}

bool CGTKMainWindow::MakeCurrentGLWidget(WIDGET_HANDLE widget)
{
#ifdef USE_OPENGL
	CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
	
	if (!w)
		return false;
		
	if (w->m_type == eGLWidget)
	{
		IVTGLWidget* glw = (IVTGLWidget*)w->m_widget;
		
		if (glw->glcontext)
			glw->glcontext->MakeCurrent(GTK_WIDGET(glw)->window);
		
		return true;
	}
#endif
	
	return false;
}



// window control
void CGTKMainWindow::Show(WIDGET_HANDLE widget)
{
	if (widget == NULL)
	{
		gtk_widget_show(m_main_window);
	}
	else
	{
		CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
		gtk_widget_show(w->m_widget);
	}
}
void CGTKMainWindow::Hide(WIDGET_HANDLE widget)
{
	if (widget == NULL)
	{
		gtk_widget_hide(m_main_window);
	}
	else
	{
		CGTKMainWindowWidget *w = (CGTKMainWindowWidget*)widget;
		gtk_widget_hide(w->m_widget);
	}
}


int CGTKMainWindow::GetModifierKeyState()
{
	int state = 0;
	
	if (last_modifier_key_state & GDK_SHIFT_MASK) state |= IVT_SHIFT_KEY;
	if (last_modifier_key_state & GDK_CONTROL_MASK) state |= IVT_CONTROL_KEY;
	if (last_modifier_key_state & GDK_MOD1_MASK) state |= IVT_ALT_KEY;
	
	return state;
}
