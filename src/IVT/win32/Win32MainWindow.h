// ****************************************************************************
// Filename:  Win32MainWindow.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _WIN32_MAIN_WINDOW_H_
#define _WIN32_MAIN_WINDOW_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/MainWindowInterface.h"

#include <windows.h>
#include <vector>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

struct Win32Widget;



// ****************************************************************************
// CWin32MainWindow
// ****************************************************************************

class CWin32MainWindow : public CMainWindowInterface
{
public:
	CWin32MainWindow(int x, int y, int width, int height, const char *title);
	~CWin32MainWindow();

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


	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:

	Win32Widget *FindWidget(HWND handle);


	HINSTANCE m_hInstance;
	HWND m_hWnd;
	static int m_ref_count;
	static int m_quit_count;
	bool m_did_quit;

	CMainWindowEventInterface *m_event_callback;

	std::vector<Win32Widget*>	m_widgets;
};



#endif /* _WIN32_MAIN_WINDOW_H_ */
