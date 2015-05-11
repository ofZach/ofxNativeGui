// ****************************************************************************
// Filename:  QtMainWindowWidget.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifdef USE_QT

#include "QtMainWindowWidget.h"
#include "QtMainWindow.h"

#include <qpainter.h>
#include <qimage.h>
#include <qevent.h>

#include "Interfaces/MainWindowEventInterface.h"
#include "Image/ByteImage.h"

#include <stdio.h>


CQtMainWindowWidget::CQtMainWindowWidget(CQtMainWindow *main_window, QtWidgetType type)
: m_main_window(main_window), m_type(type), m_widget(NULL)
{
}


void CQtMainWindowWidget::Clicked()
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && m_type == eButton)
	{
		ec->ButtonPushed(this);
	}
}
void CQtMainWindowWidget::ValueChanged(int value)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && (m_type == eSlider || m_type == eComboBox))
	{
		ec->ValueChanged(this, value);
	}
}
void CQtMainWindowWidget::Toggled(bool flag)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && m_type == eCheckBox)
	{
		ec->ValueChanged(this, (flag ? 1 : 0));
	}
}
void CQtMainWindowWidget::TextChanged(const QString &str)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL && m_type == eTextEdit)
	{
		ec->ValueChanged(this, -1);
	}
}



CQtImageWidget::CQtImageWidget(CQtMainWindow *main_window, QWidget *pParent)
: QWidget(pParent), CQtMainWindowWidget(main_window, eImage), m_pBuffer(NULL), m_nWidth(0), m_nHeight(0), m_mouse_down(false)
{
	m_widget = this;
	
	show();
	#if QT_VERSION >= 0x040000
	setBackgroundRole(QPalette::NoRole);
	#else
	setBackgroundMode(Qt::NoBackground);
	#endif
	
	setMouseTracking(true);

	#if QT_VERSION >= 0x040000
	setFocusPolicy(Qt::ClickFocus);
	#else
	setFocusPolicy(QWidget::ClickFocus);
	#endif
}

CQtImageWidget::~CQtImageWidget()
{
	if (m_pBuffer != NULL)
		delete [] m_pBuffer;
}

void CQtImageWidget::SetImage(const CByteImage *pImage)
{
	if (m_nWidth != pImage->width || m_nHeight != pImage->height)
	{
		m_nWidth = pImage->width;
		m_nHeight = pImage->height;

		if (m_pBuffer != NULL)
			delete [] m_pBuffer;

		m_pBuffer = new unsigned char[m_nWidth * m_nHeight * 4];
	}
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		const int nPixels = m_nWidth * m_nHeight;
		const unsigned char *pixels = pImage->pixels;
		int *output = (int *) m_pBuffer;

		for (int i = 0; i < nPixels; i++)
			output[i] = 255 << 24 | pixels[i] << 16 | pixels[i] << 8 | pixels[i];
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		const int nPixels = m_nWidth * m_nHeight;
		const unsigned char *pixels = pImage->pixels;
		int *output = (int *) m_pBuffer;
		
		for (int offset = 0, i = 0; i < nPixels; i++)
		{
			output[i] = 255 << 24 | pixels[offset] << 16 | pixels[offset + 1] << 8 | pixels[offset + 2];
			offset += 3;
		}
	}
	else if (pImage->type == CByteImage::eRGB24Split)
	{
		const int nPixels = m_nWidth * m_nHeight;
		const unsigned char *pixels_r = pImage->pixels;
		const unsigned char *pixels_g = pixels_r + nPixels;
		const unsigned char *pixels_b = pixels_g + nPixels;
		int *output = (int *) m_pBuffer;
		
		for (int i = 0; i < nPixels; i++)
			output[i] = 255 << 24 | pixels_r[i] << 16 | pixels_g[i] << 8 | pixels_b[i];
	}
	
	#if QT_VERSION >= 0x040000
	repaint(0, 0, m_nWidth, m_nHeight);
	#else
	repaint(0, 0, m_nWidth, m_nHeight, false);
	#endif
}

void CQtImageWidget::paintEvent(QPaintEvent *pPaintEvent)
{
	if (m_pBuffer != NULL)
	{
		QPainter painter(this);
	
		#if QT_VERSION >= 0x040000
		QImage image(m_pBuffer, m_nWidth, m_nHeight, QImage::Format_RGB32);
		#else
		QImage image(m_pBuffer, m_nWidth, m_nHeight, 32, 0, 0, QImage::BigEndian);
		#endif
	
		painter.drawImage(0, 0, image);
		
		if (m_mouse_down)
		{
			unsigned int x0 = m_mouse_start_x;
			unsigned int y0 = m_mouse_start_y;
			unsigned int x1 = m_mouse_current_x;
			unsigned int y1 = m_mouse_current_y;

			if (x0 > x1)
			{
				unsigned int swap = x0;
				x0 = x1;
				x1 = swap;
			}
			if (y0 > y1)
			{
				unsigned int swap = y0;
				y0 = y1;
				y1 = swap;
			}
				
			QPen pen(Qt::white, 1);
			painter.setPen(pen);
			painter.drawRect(x0, y0, x1 - x0, y1 - y0);
		}
	}
}

void CQtImageWidget::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (m_mouse_down)
		{
			m_mouse_down = false;
		}
		else
		{
			m_mouse_down = true;
			
			m_mouse_start_x = e->x();
			m_mouse_start_y = e->y();
			
			m_mouse_current_x = m_mouse_start_x;
			m_mouse_current_y = m_mouse_start_y;
		}
	}
	
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		int btn = -1;
		
		switch (e->button())
		{
		case Qt::LeftButton: btn = IVT_LEFT_BUTTON; break;
		case Qt::RightButton: btn = IVT_RIGHT_BUTTON; break;
		case Qt::MidButton: btn = IVT_MIDDLE_BUTTON; break;
		default: printf("unknown mouse button pressed\n"); break;
		}
		
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		ec->MouseDown(w, btn, e->x(), e->y());
	}
}

void CQtImageWidget::mouseReleaseEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton && m_mouse_down)
	{
		CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
		if (m_mouse_start_x == m_mouse_current_x && m_mouse_start_y == m_mouse_current_y)
		{
			if (ec != NULL)
			{
				CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
				ec->PointClicked(w, m_mouse_start_x, m_mouse_start_y);
			}
		}
		else
		{
			unsigned int x0 = m_mouse_start_x;
			unsigned int y0 = m_mouse_start_y;
			unsigned int x1 = m_mouse_current_x;
			unsigned int y1 = m_mouse_current_y;

			if (x0 > x1)
			{
				unsigned int swap = x0;
				x0 = x1;
				x1 = swap;
			}
			if (y0 > y1)
			{
				unsigned int swap = y0;
				y0 = y1;
				y1 = swap;
			}
			
			if (ec != NULL)
			{
				CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
				ec->RectSelected(w, x0, y0, x1, y1);
			}
			
			update(x0, y0, x1 - x0, y1 - y0);
		}
		
		m_mouse_down = false;
	}
	
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		int btn = -1;
		
		switch (e->button())
		{
		case Qt::LeftButton: btn = IVT_LEFT_BUTTON; break;
		case Qt::RightButton: btn = IVT_RIGHT_BUTTON; break;
		case Qt::MidButton: btn = IVT_MIDDLE_BUTTON; break;
		default: printf("unknown mouse button released\n"); break;
		}
		
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		ec->MouseUp(w, btn, e->x(), e->y());
	}
}

void CQtImageWidget::mouseMoveEvent(QMouseEvent * e)
{
	if (m_mouse_down)
	{
		unsigned int x0 = m_mouse_start_x;
		unsigned int y0 = m_mouse_start_y;
		unsigned int x1 = m_mouse_current_x;
		unsigned int y1 = m_mouse_current_y;

		if (x0 > x1)
		{
			unsigned int swap = x0;
			x0 = x1;
			x1 = swap;
		}
		if (y0 > y1)
		{
			unsigned int swap = y0;
			y0 = y1;
			y1 = swap;
		}
				
		update(x0, y0, x1 - x0, y1 - y0);
		
		m_mouse_current_x = e->x();
		m_mouse_current_y = e->y();
		
		if (m_mouse_current_x < 0)
			m_mouse_current_x = 0;
		if (m_mouse_current_x >= m_nWidth)
			m_mouse_current_x = m_nWidth-1;
			
		if (m_mouse_current_y < 0)
			m_mouse_current_y = 0;
		if (m_mouse_current_y >= m_nHeight)
			m_mouse_current_y = m_nHeight-1;
		
		x0 = m_mouse_start_x;
		y0 = m_mouse_start_y;
		x1 = m_mouse_current_x;
		y1 = m_mouse_current_y;

		if (x0 > x1)
		{
			unsigned int swap = x0;
			x0 = x1;
			x1 = swap;
		}
		if (y0 > y1)
		{
			unsigned int swap = y0;
			y0 = y1;
			y1 = swap;
		}
				
		update(x0, y0, x1 - x0, y1 - y0);
	}
	
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		ec->MouseMove(w, e->x(), e->y());
	}
}

void CQtImageWidget::keyPressEvent(QKeyEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		
		#if QT_VERSION >= 0x040000
		ec->KeyDown(w, e->text().toInt());
		#else
		ec->KeyDown(w, e->ascii());
		#endif
	}
}

void CQtImageWidget::keyReleaseEvent(QKeyEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		
		#if QT_VERSION >= 0x040000
		ec->KeyUp(w, e->text().toInt());
		#else
		ec->KeyUp(w, e->ascii());
		#endif
	}
}



#ifdef USE_OPENGL

CQtGLWidget::CQtGLWidget(CQtMainWindow *main_window, QWidget *pParent)
: QGLWidget(pParent), CQtMainWindowWidget(main_window, eGLWidget)
{
	m_widget = this;
	
	show();
	
	setMouseTracking(true);
	
	#if QT_VERSION >= 0x040000
	setFocusPolicy(Qt::ClickFocus);
	#else
	setFocusPolicy(QWidget::ClickFocus);
	#endif
}

CQtGLWidget::~CQtGLWidget()
{
}

void CQtGLWidget::mousePressEvent(QMouseEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		int btn = -1;
		
		switch (e->button())
		{
		case Qt::LeftButton: btn = IVT_LEFT_BUTTON; break;
		case Qt::RightButton: btn = IVT_RIGHT_BUTTON; break;
		case Qt::MidButton: btn = IVT_MIDDLE_BUTTON; break;
		default: printf("unknown mouse button pressed\n"); break;
		}
		
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		ec->MouseDown(w, btn, e->x(), e->y());
	}
}

void CQtGLWidget::mouseReleaseEvent(QMouseEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		int btn = -1;
		
		switch (e->button())
		{
		case Qt::LeftButton: btn = IVT_LEFT_BUTTON; break;
		case Qt::RightButton: btn = IVT_RIGHT_BUTTON; break;
		case Qt::MidButton: btn = IVT_MIDDLE_BUTTON; break;
		default: printf("unknown mouse button released\n"); break;
		}
		
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		ec->MouseUp(w, btn, e->x(), e->y());
	}
}

void CQtGLWidget::mouseMoveEvent(QMouseEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		ec->MouseMove(w, e->x(), e->y());
	}
}

void CQtGLWidget::keyPressEvent(QKeyEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;
		
		#if QT_VERSION >= 0x040000
		ec->KeyDown(w, e->text().toInt());
		#else
		ec->KeyDown(w, e->ascii());
		#endif
	}
}

void CQtGLWidget::keyReleaseEvent(QKeyEvent * e)
{
	CMainWindowEventInterface *ec = m_main_window->GetEventCallback();
	
	if (ec != NULL)
	{
		CQtMainWindowWidget *w = (CQtMainWindowWidget*)this;

		#if QT_VERSION >= 0x040000
		ec->KeyUp(w, e->text().toInt());
		#else
		ec->KeyUp(w, e->ascii());
		#endif
	}
}

#endif

#endif