// ****************************************************************************
// Filename:  QtMainWindow.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifdef USE_QT

#ifndef _QT_MAIN_WINDOW_H_
#define _QT_MAIN_WINDOW_H_


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Interfaces/MainWindowInterface.h"
#include <qwidget.h>
#include <vector>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CQtMainWindowWidget;



// ****************************************************************************
// CQtMainWindow
// ****************************************************************************

class CQtMainWindow : public QWidget, public CMainWindowInterface
{
public:
	CQtMainWindow(int x, int y, int width, int height, const char *title);
	CQtMainWindow(int x, int y, int width, int height, QWidget *pParent);
	~CQtMainWindow();

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
	
	
	CMainWindowEventInterface *GetEventCallback() { return m_event_callback; }

protected:
	void closeEvent(QCloseEvent *e);

private:

	CMainWindowEventInterface *m_event_callback;

	std::vector<CQtMainWindowWidget*> m_widgets;

	static int m_ref_count;
};



#endif /* _QT_MAIN_WINDOW_H_ */

#endif