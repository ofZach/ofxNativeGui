// ****************************************************************************
// Filename:  CocoaMainWindow.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _COCOA_MAIN_WINDOW_H_
#define _COCOA_MAIN_WINDOW_H_


// ****************************************************************************
// Includes
// ****************************************************************************

#include "MainWindowInterface.h"
#include <vector>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

struct CCocoaMainWindowWidget;


// [zach] I need these enums exposed, they were in cpp before
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



// ****************************************************************************
// CCocoaMainWindow
// ****************************************************************************

class CCocoaMainWindow : public CMainWindowInterface
{
public:
	CCocoaMainWindow(int x, int y, int width, int height, const char *title);
	~CCocoaMainWindow();

	// create widgets
	WIDGET_HANDLE AddImage(int x, int y, int width, int height, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddButton(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddLabel(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddCheckBox(int x, int y, int width, int height, const char *text, bool checked, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddTextEdit(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddSlider(int x, int y, int width, int height, int min_value, int max_value, int step, int value, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddComboBox(int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, WIDGET_HANDLE parent = 0);
	WIDGET_HANDLE AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent = 0);

	// access to widget attributes
	bool GetText(WIDGET_HANDLE widget, char *text, int len);
	bool SetText(WIDGET_HANDLE widget, const char *text);

	bool SetImage(WIDGET_HANDLE widget, const CByteImage *pImage);

	bool GetValue(WIDGET_HANDLE widget, int &value);
	bool SetValue(WIDGET_HANDLE widget, int value);
	
	bool SwapBuffersGLWidget(WIDGET_HANDLE widget);
	bool MakeCurrentGLWidget(WIDGET_HANDLE widget);

	// window control
	void Show(WIDGET_HANDLE widget = 0);
	void Hide(WIDGET_HANDLE widget = 0);

	int GetModifierKeyState();

	void SetEventCallback(CMainWindowEventInterface *callback) {m_event_callback = callback;}
	
    
	void EventCallback(void* widget, int type, int *params);
private:
	void FixPosition(int &x, int &y, int &width, int &height);
	
	void *m_cocoa_main_window;
	int m_width;
	int m_height;
	CMainWindowEventInterface *m_event_callback;

	std::vector<CCocoaMainWindowWidget*> m_widgets;
};



#endif /* _COCOA_MAIN_WINDOW_H_ */
