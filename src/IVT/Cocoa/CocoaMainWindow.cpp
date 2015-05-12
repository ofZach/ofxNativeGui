// *****************************************************************
// Filename:  CocoaMainWindow.cpp
// Copyright: Pedram Azad, Chair Prof. Dillmann (IAIM),
//            Institute for Computer Science and Engineering (CSE),
//            University of Karlsruhe. All rights reserved.
// Author:    Florian Hecht
// Date:      2008
// *****************************************************************


// *****************************************************************
// necessary includes
// *****************************************************************

#include "CocoaMainWindow.h"
#include "MainWindowEventInterface.h"

//#include "Image/ByteImage.h"
//#include "Image/ImageProcessor.h"

#ifdef USE_OPENGL

#endif

enum CocoaWidgetType
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

struct CCocoaMainWindowWidget
{
	CocoaWidgetType type;
	void *cocoa_ptr;
	int width;
	int height;
};



// these functions are defined in CocoaImplementation.m
extern "C"
{
	void* CocoaCreateMainWindow(int x, int y, int width, int height, const char *title, void* main_window_ptr);
  void CocoaDestroyMainWindow(void* window);
	void* CocoaCreateImage(void* window, int x, int y, int width, int height, void* ptr);
	void* CocoaCreateButton(void* window, int x, int y, int width, int height, const char *text, void* ptr);
	void* CocoaCreateLabel(void* window, int x, int y, int width, int height, const char *text, void* ptr);
	void* CocoaCreateCheckBox(void* window, int x, int y, int width, int height, const char *text, bool checked, void* ptr);
	void* CocoaCreateTextEdit(void* window, int x, int y, int width, int height, const char *text, void* ptr);
	void* CocoaCreateSlider(void* window, int x, int y, int width, int height, int min_value, int max_value, int step, int value, void* ptr);
	void* CocoaCreateComboBox(void* window, int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, void* ptr);
	void* CocoaCreateOpenGLWidget(void* window, int x, int y, int width, int height, void* ptr);
	
	int CocoaGetInt(void* ptr);
	void CocoaGetText(void* ptr, char *str, int len);
	void CocoaGetTitle(void* ptr, char *str, int len);
	int CocoaGetComboBoxSelection(void *ptr);
	
	void CocoaSetInt(void* ptr, int value);
	void CocoaSetText(void* ptr, const char *str);
	void CocoaSetTitle(void* ptr, const char *str);
	void CocoaSetComboBoxSelection(void *ptr, int value);
	void CocoaSetImage(void* ptr, int width, int height, unsigned char *pixels);
	
	void CocoaShow(void* ptr);
	void CocoaHide(void* ptr);
	void CocoaShowWindow(void* ptr);
	void CocoaHideWindow(void* ptr);

	int CocoaGetModifierKeyState();
	
	void CocoaSwapBuffers(void* ptr);
	void CocoaMakeCurrent(void* ptr);
	
	void CocoaGetCurrentWorkingDirectory(char* str, int len);
}

class SetTheCWDHack
{
public:
	SetTheCWDHack()
	{
		char buf[1024];
		CocoaGetCurrentWorkingDirectory(buf, 1024);
		chdir(buf);
	}
};

SetTheCWDHack hack;

// this is for callbacks from the ObjC implementation
extern "C" void EventCallback(void* window, void* widget, int type, int *params)
{
	CCocoaMainWindow *main_window = (CCocoaMainWindow*)window;
	main_window->EventCallback(widget, type, params);
}

void CCocoaMainWindow::EventCallback(void* widget, int type, int *params)
{
	if (m_event_callback)
	{
		int s = m_widgets.size();
		
		for (int i = 0; i < s; i++)
		{
			CCocoaMainWindowWidget *w = m_widgets[i];
			
			if (w->cocoa_ptr == widget)
			{
				switch (w->type)
				{
				case eImage:
					{
						if (type == 1)
						{
							m_event_callback->PointClicked(w, params[0], params[1]);
						}
						else if (type == 2)
						{
							m_event_callback->RectSelected(w, params[0], params[1], params[2], params[3]);
						}
						else if (type == 3)
						{
							m_event_callback->MouseDown(w, params[0], params[1], params[2]);
						}
						else if (type == 4)
						{
							m_event_callback->MouseUp(w, params[0], params[1], params[2]);
						}
						else if (type == 5)
						{
							m_event_callback->MouseMove(w, params[0], params[1]);
						}
						else if (type == 6)
						{
							m_event_callback->KeyDown(w, params[0]);
						}
						else if (type == 7)
						{
							m_event_callback->KeyUp(w, params[0]);
						}
					}
					break;
				case eButton: m_event_callback->ButtonPushed(w); break;
				case eCheckBox:
					{
						int v = CocoaGetInt(w->cocoa_ptr);
						m_event_callback->ValueChanged(w, v);
					}
					break;
				case eTextEdit: m_event_callback->ValueChanged(w, -1); break;
				case eSlider:
					{
						int v = CocoaGetInt(w->cocoa_ptr);
						m_event_callback->ValueChanged(w, v);
					}
					break;
				case eComboBox:
					{
						int v = CocoaGetComboBoxSelection(w->cocoa_ptr);
						m_event_callback->ValueChanged(w, v);
					}
					break;
				case eGLWidget:
					{
						if (type == 3)
						{
							m_event_callback->MouseDown(w, params[0], params[1], params[2]);
						}
						else if (type == 4)
						{
							m_event_callback->MouseUp(w, params[0], params[1], params[2]);
						}
						else if (type == 5)
						{
							m_event_callback->MouseMove(w, params[0], params[1]);
						}
						else if (type == 6)
						{
							m_event_callback->KeyDown(w, params[0]);
						}
						else if (type == 7)
						{
							m_event_callback->KeyUp(w, params[0]);
						}
					}
						break;
				default: break;
				}
				
				break;
			}
		}
	}
}





CCocoaMainWindow::CCocoaMainWindow(int x, int y, int width, int height, const char *title)
{
	m_cocoa_main_window = CocoaCreateMainWindow(x, y, width, height, title, this);
	
	m_width = width;
	m_height = height;
	
	if (!m_cocoa_main_window)
		printf("error: couldn't create Cocoa main window\n");
	
	m_event_callback = NULL;
}

CCocoaMainWindow::~CCocoaMainWindow()
{
	int s = m_widgets.size();
	
	for (int i = 0; i < s; i++)
	{
		CCocoaMainWindowWidget *widget = m_widgets[i];
		delete widget;
	}
	m_widgets.clear();
  
  CocoaDestroyMainWindow(m_cocoa_main_window);
  m_cocoa_main_window = NULL;
}

// create widgets
WIDGET_HANDLE CCocoaMainWindow::AddImage(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateImage(m_cocoa_main_window, x, y, width, height, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eImage;
		widget->cocoa_ptr = ptr;
		widget->width = width;
		widget->height = height;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}

	
	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddButton(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateButton(m_cocoa_main_window, x, y, width, height, text, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eButton;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}
	
	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddLabel(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateLabel(m_cocoa_main_window, x, y, width, height, text, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eLabel;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}
	
	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddCheckBox(int x, int y, int width, int height, const char *text, bool checked, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateCheckBox(m_cocoa_main_window, x, y, width, height, text, checked, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eCheckBox;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}

	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddTextEdit(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateTextEdit(m_cocoa_main_window, x, y, width, height, text, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eTextEdit;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}

	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddSlider(int x, int y, int width, int height, int min_value, int max_value, int step, int value, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateSlider(m_cocoa_main_window, x, y, width, height, min_value, max_value, step, value, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eSlider;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}

	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddComboBox(int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, WIDGET_HANDLE parent)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateComboBox(m_cocoa_main_window, x, y, width, height, num_entries, entries, current_entry, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eComboBox;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}

	return (WIDGET_HANDLE)NULL;
}

WIDGET_HANDLE CCocoaMainWindow::AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
#ifdef USE_OPENGL
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)parent;
	
	FixPosition(x, y, width, height);
	
	void* ptr = CocoaCreateOpenGLWidget(m_cocoa_main_window, x, y, width, height, (w != NULL ? w->cocoa_ptr : NULL));
	if (ptr != NULL)
	{
		CCocoaMainWindowWidget *widget = new CCocoaMainWindowWidget;
		
		widget->type = eGLWidget;
		widget->cocoa_ptr = ptr;
		
		m_widgets.push_back(widget);
		
		return (WIDGET_HANDLE)widget;
	}
	
#endif

	return (WIDGET_HANDLE)NULL;
}


// access to widget attributes
bool CCocoaMainWindow::GetText(WIDGET_HANDLE widget, char *text, int len)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w->type != eImage && w->type != eGLWidget && w->type != eSlider && w->type != eComboBox)
	{
		if (w->type == eButton || w->type == eCheckBox)
			CocoaGetTitle(w->cocoa_ptr, text, len);
		else
			CocoaGetText(w->cocoa_ptr, text, len);
		
		return true;
	}
	  
	return false;
}
bool CCocoaMainWindow::SetText(WIDGET_HANDLE widget, const char *text)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w->type != eImage && w->type != eGLWidget && w->type != eSlider && w->type != eComboBox)
	{
		if (w->type == eButton || w->type == eCheckBox)
			CocoaSetTitle(w->cocoa_ptr, text);
		else
			CocoaSetText(w->cocoa_ptr, text);
		
		return true;
	}

	return false;
}

//bool CCocoaMainWindow::SetImage(WIDGET_HANDLE widget, const CByteImage *pImage)
//{
//	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
//	
//	if (w->type == eImage)
//	{
//		if (w->width != pImage->width || w->height != pImage->height)
//		{
//			printf("error: CCocoaMainWindow::SetImage: image dimensions do not match dimensions of image widget!\n");
//			return false;
//		}
//		
//		if (pImage->type == CByteImage::eRGB24)
//		{
//			CocoaSetImage(w->cocoa_ptr, pImage->width, pImage->height, pImage->pixels);
//		}
//		else
//		{
//			CByteImage temp(pImage->width, pImage->height, CByteImage::eRGB24);
//			
//			ImageProcessor::ConvertImage(pImage, &temp);
//			
//			CocoaSetImage(w->cocoa_ptr, pImage->width, pImage->height, temp.pixels);
//		}
//		
//		return true;
//	}
//	
//	return false;
//}

bool CCocoaMainWindow::GetValue(WIDGET_HANDLE widget, int &value)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w->type == eComboBox)
	{
		value = CocoaGetComboBoxSelection(w->cocoa_ptr);
		
		return true;
	}
	else if (w->type == eCheckBox || w->type == eSlider)
	{
		value = CocoaGetInt(w->cocoa_ptr);
		
		return true;
	}
	
	return false;
}

bool CCocoaMainWindow::SetValue(WIDGET_HANDLE widget, int value)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w->type == eComboBox)
	{
		CocoaSetComboBoxSelection(w->cocoa_ptr, value);
		
		return true;
	}
	else if (w->type == eCheckBox || w->type == eSlider)
	{
		CocoaSetInt(w->cocoa_ptr, value);
		
		return true;
	}
	
	return false;
}

bool CCocoaMainWindow::SwapBuffersGLWidget(WIDGET_HANDLE widget)
{
#ifdef USE_OPENGL
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w->type == eGLWidget)
	{
		CocoaSwapBuffers(w->cocoa_ptr);
		
		return true;
	}
	
#endif
	
	return false;
}

bool CCocoaMainWindow::MakeCurrentGLWidget(WIDGET_HANDLE widget)
{
#ifdef USE_OPENGL
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w->type == eGLWidget)
	{
		CocoaMakeCurrent(w->cocoa_ptr);
		
		return true;
	}
	
#endif
	
	return false;
}



// window control
void CCocoaMainWindow::Show(WIDGET_HANDLE widget)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w != NULL)
	{
		CocoaShow(w->cocoa_ptr);
	}
	else
	{
		CocoaShowWindow(m_cocoa_main_window);
	}
}
void CCocoaMainWindow::Hide(WIDGET_HANDLE widget)
{
	CCocoaMainWindowWidget *w = (CCocoaMainWindowWidget*)widget;
	
	if (w != NULL)
	{
		CocoaHide(w->cocoa_ptr);
	}
	else
	{
		CocoaHideWindow(m_cocoa_main_window);
	}
}

int CCocoaMainWindow::GetModifierKeyState()
{
	return CocoaGetModifierKeyState();
}

void CCocoaMainWindow::FixPosition(int &x, int &y, int &width, int &height)
{
	y = m_height - y - height;
}
