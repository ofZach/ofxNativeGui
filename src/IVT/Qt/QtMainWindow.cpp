// ****************************************************************************
// Filename:  QtMainWindow.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifdef USE_QT

#include "QtMainWindow.h"
#include "QtMainWindowWidget.h"
#include "QtApplicationHandler.h"
#include "Interfaces/MainWindowEventInterface.h"

#include "Image/ByteImage.h"

#include <qstring.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qcombobox.h>
#include <qapplication.h>
#include <qevent.h>

#ifdef USE_OPENGL
#include <qgl.h>
#endif

#include <stdio.h>




int CQtMainWindow::m_ref_count = 0;

CQtMainWindow::CQtMainWindow(int x, int y, int width, int height, const char *title)
{
	setFixedSize(width, height);
	#ifdef OSX_QTGUI // make sure the Qt window is competely visible under OS X
	if (y < 22)
		y = 22;
	#endif
	move(x, y);
	
	#if QT_VERSION >= 0x040000
	setWindowTitle(title);
	setAttribute(Qt::WA_QuitOnClose, true);
	#else
	setCaption(title);
	#endif
	
	m_event_callback = NULL;

	m_ref_count++;
}

CQtMainWindow::CQtMainWindow(int x, int y, int width, int height, QWidget *pParent) : QWidget(pParent)
{
	setFixedSize(width, height);
	move(x, y);
	
	m_event_callback = NULL;

	m_ref_count++;
}

CQtMainWindow::~CQtMainWindow()
{
	int c = m_widgets.size();
	
	for (int i = c-1; i >= 0; i--)
	{
		delete m_widgets[i];
	}
	
	m_widgets.clear();
}

// create widgets
WIDGET_HANDLE CQtMainWindow::AddImage(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtImageWidget *iw = new CQtImageWidget(this, (p != NULL ? p->m_widget : this));
	iw->setFixedSize(width, height);
	iw->move(x, y);

	CQtMainWindowWidget *w = iw;
	
	m_widgets.push_back(w);
	
	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddButton(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtMainWindowWidget *w = new CQtMainWindowWidget(this, eButton);
	
	QPushButton *pBtn = new QPushButton((p != NULL ? p->m_widget : this));
	pBtn->setText(text);
	pBtn->setFixedSize(width, height);
	pBtn->move(x, y);
	
	w->m_widget = pBtn;
	
	connect(pBtn, SIGNAL(clicked()), w, SLOT(Clicked()));

	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddLabel(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtMainWindowWidget *w = new CQtMainWindowWidget(this, eLabel);

	QLabel *pLabel = new QLabel((p != NULL ? p->m_widget : this));
	pLabel->setText(text);
	pLabel->setFixedSize(width, height);
	pLabel->move(x, y);
	
	w->m_widget = pLabel;
	
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddCheckBox(int x, int y, int width, int height, const char *text, bool checked, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtMainWindowWidget *w = new CQtMainWindowWidget(this, eCheckBox);

	QCheckBox *pCheckBox = new QCheckBox((p != NULL ? p->m_widget : this));
	pCheckBox->setText(text);
	pCheckBox->setFixedSize(width, height);
	pCheckBox->move(x, y);
	pCheckBox->setChecked(checked);
	
	w->m_widget = pCheckBox;
	
	connect(pCheckBox, SIGNAL(toggled(bool)), w, SLOT(Toggled(bool)));
	
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddTextEdit(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtMainWindowWidget *w = new CQtMainWindowWidget(this, eTextEdit);

	QLineEdit *pLineEdit = new QLineEdit((p != NULL ? p->m_widget : this));
	pLineEdit->setText(text);
	pLineEdit->setFixedSize(width, height);
	pLineEdit->move(x, y);
	
	w->m_widget = pLineEdit;
	
	connect(pLineEdit, SIGNAL(textChanged(const QString &)), w, SLOT(TextChanged(const QString &)));
	
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddSlider(int x, int y, int width, int height, int min_value, int max_value, int step, int value, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtMainWindowWidget *w = new CQtMainWindowWidget(this, eSlider);

	QSlider *pSlider = new QSlider(Qt::Horizontal, (p != NULL ? p->m_widget : this));
	pSlider->setFixedSize(width, height);
	pSlider->move(x, y);
	#if QT_VERSION >= 0x040000
	pSlider->setMinimum(min_value);
	pSlider->setMaximum(max_value);
	pSlider->setTickPosition(QSlider::TicksBelow);
	pSlider->setFocusPolicy(Qt::StrongFocus);
	#else
	pSlider->setMinValue(min_value);
	pSlider->setMaxValue(max_value);
	pSlider->setTickmarks(QSlider::Below);
	pSlider->setFocusPolicy(QWidget::StrongFocus);
	#endif
	pSlider->setValue(value);
	pSlider->setPageStep(1);
	pSlider->setTickInterval(step);
	
	
	w->m_widget = pSlider;
	
	connect(pSlider, SIGNAL(valueChanged(int)), w, SLOT(ValueChanged(int)));
	
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddComboBox(int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, WIDGET_HANDLE parent)
{
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;
	
	CQtMainWindowWidget *w = new CQtMainWindowWidget(this, eComboBox);

	QComboBox *pComboBox = new QComboBox((p != NULL ? p->m_widget : this));
	pComboBox->setFixedSize(width, height);
	pComboBox->move(x, y);
	
	#if QT_VERSION >= 0x040000
	for (int i = 0; i < num_entries; i++)
	{
		pComboBox->addItem(entries[i]);
	}
	pComboBox->setCurrentIndex(current_entry);
	#else
	for (int i = 0; i < num_entries; i++)
	{
		pComboBox->insertItem(entries[i]);
	}
	pComboBox->setCurrentItem(current_entry);
	#endif
	
	w->m_widget = pComboBox;
	
	connect(pComboBox, SIGNAL(activated(int)), w, SLOT(ValueChanged(int)));
	
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CQtMainWindow::AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
#ifdef USE_OPENGL
	CQtMainWindowWidget *p = (CQtMainWindowWidget*)parent;

	CQtGLWidget *pGLWidget = new CQtGLWidget(this, (p != NULL ? p->m_widget : this));
	pGLWidget->setFixedSize(width, height);
	pGLWidget->move(x, y);
	
	CQtMainWindowWidget *w = pGLWidget;
	
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
#else
	printf("WARNING: CQtMainWindow::AddGLWidget has been called, but USE_OPENGL = 0 in Makefile.base\n");
	return (WIDGET_HANDLE)NULL;

#endif
}


// access to widget attributes
#if QT_VERSION >= 0x040000
#define GET_ASCII toAscii
#else
#define GET_ASCII ascii
#endif

bool CQtMainWindow::GetText(WIDGET_HANDLE widget, char *text, int len)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;

	if (w->m_type == eButton)
	{
		QPushButton *btn = (QPushButton*)w->m_widget;
		QString str = btn->text();
		strncpy(text, str.GET_ASCII(), len);
		
		return true;
	}
	else if(w->m_type == eCheckBox)
	{
		QCheckBox *btn = (QCheckBox*)w->m_widget;
		QString str = btn->text();
		strncpy(text, str.GET_ASCII(), len);
		
		return true;
	}
	else if (w->m_type == eTextEdit)
	{
		QLineEdit *le = (QLineEdit*)w->m_widget;
		QString str = le->text();
		strncpy(text, str.GET_ASCII(), len);
		
		return true;
	}
	else if (w->m_type == eLabel)
	{
		QLabel *label = (QLabel*)w->m_widget;
		QString str = label->text();
		strncpy(text, str.GET_ASCII(), len);
		
		return true;
	}
	
	return false;
}
bool CQtMainWindow::SetText(WIDGET_HANDLE widget, const char *text)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;
	
	if (w->m_type == eButton)
	{
		QPushButton *btn = (QPushButton*)w->m_widget;
		btn->setText(text);
		
		return true;
	}
	else if (w->m_type == eCheckBox)
	{
		QCheckBox *btn = (QCheckBox*)w->m_widget;
		btn->setText(text);
		
		return true;
	}
	else if (w->m_type == eTextEdit)
	{
		QLineEdit *le = (QLineEdit*)w->m_widget;
		le->setText(text);
		
		return true;
	}
	else if (w->m_type == eLabel)
	{
		QLabel *label = (QLabel*)w->m_widget;
		label->setText(text);
		
		return true;
	}
	
	return false;
}

bool CQtMainWindow::SetImage(WIDGET_HANDLE widget, const CByteImage *pImage)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;
	
	if (w->m_type == eImage)
	{
		CQtImageWidget *iw = (CQtImageWidget*)w;
		
		iw->SetImage(pImage);
		
		return true;
	}
	
	return false;
}

bool CQtMainWindow::GetValue(WIDGET_HANDLE widget, int &value)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;

	if (w->m_type == eCheckBox)
	{
		QCheckBox *cb = (QCheckBox*)w->m_widget;
		
		value = cb->isChecked();
		
		return true;
	}
	else if (w->m_type == eSlider)
	{
		QSlider *sl = (QSlider*)w->m_widget;
		
		value = sl->value();
		
		return true;
	}
	else if (w->m_type == eComboBox)
	{
		QComboBox *cb = (QComboBox*)w->m_widget;
		
		#if QT_VERSION >= 0x040000
		value = cb->currentIndex();
		#else
		value = cb->currentItem();
		#endif
		
		return true;
	}
	
	return false;
}

bool CQtMainWindow::SetValue(WIDGET_HANDLE widget, int value)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;

	if (w->m_type == eCheckBox)
	{
		QCheckBox *cb = (QCheckBox*)w->m_widget;
		
		cb->setChecked(value != 0);
		
		return true;
	}
	else if (w->m_type == eSlider)
	{
		QSlider *sl = (QSlider*)w->m_widget;
		
		sl->setValue(value);
		
		return true;
	}
	else if (w->m_type == eComboBox)
	{
		QComboBox *cb = (QComboBox*)w->m_widget;
		
		#if QT_VERSION >= 0x040000
		cb->setCurrentIndex(value);
		#else
		cb->setCurrentItem(value);
		#endif
		
		return true;
	}
	
	return false;
}

bool CQtMainWindow::SwapBuffersGLWidget(WIDGET_HANDLE widget)
{
#ifdef USE_OPENGL
	
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;

	if (w->m_type == eGLWidget)
	{
		QGLWidget *glw = (QGLWidget*)w->m_widget;
		
		//glw->swapBuffers();
		glw->updateGL();
		
		return true;
	}
	
#endif
	
	return false;
}

bool CQtMainWindow::MakeCurrentGLWidget(WIDGET_HANDLE widget)
{
#ifdef USE_OPENGL
	
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;

	if (w->m_type == eGLWidget)
	{
		QGLWidget *glw = (QGLWidget*)w->m_widget;
		
		glw->makeCurrent();
		
		return true;
	}
#endif
	
	return false;
}



// window control
void CQtMainWindow::Show(WIDGET_HANDLE widget)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;
	
	if (w == NULL)
	{
		show();
	}
	else
	{
		w->m_widget->show();
	}
}
void CQtMainWindow::Hide(WIDGET_HANDLE widget)
{
	CQtMainWindowWidget *w = (CQtMainWindowWidget*)widget;
	
	if (w == NULL)
	{
		hide();
	}
	else
	{
		w->m_widget->hide();
	}
}

#if QT_VERSION < 0x040000

#ifdef Q_WS_WIN
#    include <windows.h>
#    include <shellapi.h>
#elif defined Q_WS_X11
#    include <X11/Xlib.h>
#else /* Q_WS_MAC */
#    include <Carbon/Carbon.h>
/* //probably don't need these
#define shiftKeyBit		9
#define optionKeyBit		11
#define controlKeyBit		12
#define rightShiftKeyBit	13
#define rightOptionKeyBit	14
#define rightControlKeyBit	15
*/
#endif
#include <qpaintdevice.h>
 
// Implement a way to easily deduce which mousebuttons and modifier
// keys are held down
static Qt::ButtonState get_modifiers()
{
	int result = Qt::NoButton;

#if defined(Q_WS_WIN)
    
	result |= GetAsyncKeyState(VK_SHIFT) < 0 ? Qt::ShiftButton : 0;
	result |= GetAsyncKeyState(VK_MENU) < 0 ? Qt::AltButton : 0;
	result |= GetAsyncKeyState(VK_CONTROL) < 0 ? Qt::ControlButton : 0;
 
#elif defined(Q_WS_X11)
    
	Window root, child;
	int root_x, root_y;
	int win_x, win_y;
	uint keys_buttons;
	bool status = XQueryPointer(QPaintDevice::x11AppDisplay(),
	    QPaintDevice::x11AppRootWindow(),
	    &root, &child,
	    &root_x, &root_y,
	    &win_x, &win_y,
	    &keys_buttons);

	if (status)
	{        
	result |=  keys_buttons & ShiftMask ? Qt::ShiftButton : 0;
	result |=  keys_buttons & Mod1Mask ? Qt::AltButton : 0;
	result |=  keys_buttons & ControlMask ? Qt::ControlButton : 0; 
	}
    
#else /* Q_WS_MAC */

	UInt32 modifiers = GetCurrentKeyModifiers();
	if (modifiers & (1 << shiftKeyBit) modifiers |= Qt::ShiftButton;
	if (modifiers & (1 << rightShiftKeyBit) modifiers |= Qt::ShiftButton;
	if (modifiers & (1 << optionKeyBit) modifiers |= Qt::AltButton;
	if (modifiers & (1 << rightOptionKeyBit) modifiers |= Qt::AltButton;
	if (modifiers & (1 << controlKeyBit) modifiers |= Qt::ControlButton;
	if (modifiers & (1 << rightControlKeyBit) modifiers |= Qt::ControlButton;

#endif

    return Qt::ButtonState(result);
}

#endif /* end QT_VERSION < 0x040000 */



int CQtMainWindow::GetModifierKeyState()
{
	int state = 0;
	int mod = 0;
	
	#if QT_VERSION >= 0x040000
	mod = qApp->keyboardModifiers();
	if (mod & Qt::ShiftModifier)
		state |= IVT_SHIFT_KEY;
	if (mod & Qt::ControlModifier)
		state |= IVT_CONTROL_KEY;
	if (mod & Qt::AltModifier)
		state |= IVT_ALT_KEY;
	#else
	mod = get_modifiers();
	if (mod & Qt::ShiftButton)
		state |= IVT_SHIFT_KEY;
	if (mod & Qt::ControlButton)
		state |= IVT_CONTROL_KEY;
	if (mod & Qt::AltButton)
		state |= IVT_ALT_KEY;
	#endif
	
	return state;
}


void CQtMainWindow::closeEvent(QCloseEvent *e)
{
	m_ref_count--;
	if (m_ref_count == 0)
	{
		CQtApplicationHandler *app_handler = CQtApplicationHandler::GetApplicationHandler();

		if (app_handler)
			app_handler->Exit();
	}
}

#endif