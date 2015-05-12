#ifdef USE_QT

// ****************************************************************************
// Filename:  QtMainWindowWidget.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _QT_MAIN_WINDOW_WIDGET_H_
#define _QT_MAIN_WINDOW_WIDGET_H_


// ****************************************************************************
// Includes
// ****************************************************************************

#include <qwidget.h>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CQtMainWindow;
class CByteImage;


// ****************************************************************************
// Enums
// ****************************************************************************

enum QtWidgetType
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
// CQtMainWindowWidget
// ****************************************************************************

class CQtMainWindowWidget : public QObject
{
	Q_OBJECT
	
public:
	CQtMainWindowWidget(CQtMainWindow *main_window, QtWidgetType type);
	~CQtMainWindowWidget() {}

public slots:
	void Clicked();
	void ValueChanged(int value);
	void Toggled(bool flag);
	void TextChanged(const QString &str);
	
public:
	CQtMainWindow	*m_main_window;
	QtWidgetType	m_type;
	QWidget		*m_widget;
};

class CQtImageWidget : public QWidget, public CQtMainWindowWidget
{
public:
	CQtImageWidget(CQtMainWindow *main_window, QWidget *pParent = 0);
	~CQtImageWidget();
	
	void SetImage(const CByteImage *pImage);
private:
	void paintEvent(QPaintEvent *pPaintEvent);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	int m_nWidth;
	int m_nHeight;
	unsigned char *m_pBuffer;
	
	bool m_mouse_down;
	int m_mouse_start_x;
	int m_mouse_start_y;
	int m_mouse_current_x;
	int m_mouse_current_y;
};



#ifdef USE_OPENGL

#include <qgl.h>

// ****************************************************************************
// CQtGLWidget
// ****************************************************************************

class CQtGLWidget : public QGLWidget, public CQtMainWindowWidget
{
public:
	CQtGLWidget(CQtMainWindow *main_window, QWidget *pParent = 0);
	~CQtGLWidget();
	
private:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
}; 

#endif


#endif /* _QT_MAIN_WINDOW_WIDGET_H_ */

#endif