// ****************************************************************************
// Filename:  Win32MainWindow.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#include "Win32MainWindow.h"
#include "Interfaces/MainWindowEventInterface.h"

#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>

#include "Image/ByteImage.h"


enum Win32WidgetType
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

struct Win32Widget
{
	Win32WidgetType m_eType;
	int m_index;		// index into the widgets array
	HWND m_hWnd;		// window handle
};

struct Win32ImageWidget : public Win32Widget
{
	HDC m_hDC;					// device context of the widget
	HDC m_hDCMem;				// device context to store the current image
	HBITMAP m_hBmp;				// the bitmap with the image
	HBITMAP m_hBmpOld;			// the previous bitmap (to restore the state)
	unsigned char *m_pPixels;	// pointer to an upside-down BGR buffer
	int m_iWidth;				// width of the current bitmap
	int m_iHeight;				// height of the current bitmap

	const CByteImage *m_pImage;	// the currently displayed image
	unsigned char *m_buffer;	// stores a copy of the upside-down BGR buffer
								//   it is used to draw the selection rectangle

	// mouse interaction
	bool mouse_down;
	unsigned int mouse_start_x;
	unsigned int mouse_start_y;
	unsigned int mouse_current_x;
	unsigned int mouse_current_y;
};

struct Win32GLWidget : public Win32Widget
{
	HDC m_hDC;					// device context of the widget
	int m_iWidth;				// width of the current bitmap
	int m_iHeight;				// height of the current bitmap
	HGLRC m_hGLRC;				// OpenGL context
};


// helper function
void DrawRect(unsigned char *img, int width, int height, int x0, int y0, int x1, int y1)
{
	const int width_ = width % 2 == 0 ? width : width + 1;
	int base0 = 3*(height - y0 - 1)*width_;
	int base1 = 3*(height - y1)*width_;

	if (x1 >= width)
		x1 = width-1;
	if (y1 >= height)
		y1 = height-1;

	for (int x = x0; x < x1; x++)
	{
		img[base0 + 3*x + 0] = 255;
		img[base0 + 3*x + 1] = 0;
		img[base0 + 3*x + 2] = 0;

		img[base1 + 3*x + 0] = 255;
		img[base1 + 3*x + 1] = 0;
		img[base1 + 3*x + 2] = 0;
	}

	for (int y = (height - y1); y < (height - y0 - 1); y++)
	{
		img[3*y*width_ + 3*x0 + 0] = 255;
		img[3*y*width_ + 3*x0 + 1] = 0;
		img[3*y*width_ + 3*x0 + 2] = 0;

		img[3*y*width_ + 3*(x1-1) + 0] = 255;
		img[3*y*width_ + 3*(x1-1) + 1] = 0;
		img[3*y*width_ + 3*(x1-1) + 2] = 0;
	}
}


#define DEFAULT_CLASS_NAME	"MainWindow"
#define IMAGE_WINDOW_CLASS_NAME	"ImageWindow"

LRESULT CALLBACK WndProc(HWND hWnd,	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		// extract the pointer to the object that created the window
		CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
		CWin32MainWindow *ptr = (CWin32MainWindow*)cs->lpCreateParams;

		// store it in the user data of the window
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) ptr);

		return 0;
	}
	else
	{
		// get the pointer to the object from the user data
		CWin32MainWindow *ptr = (CWin32MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		// if we got a valid pointer, call the WindowProc method of that object
		if (ptr)
			return ptr->WindowProc(hWnd, uMsg, wParam, lParam);
		else
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


LRESULT CALLBACK CWin32MainWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win32Widget *w = FindWidget(hWnd);

	switch (uMsg)
	{
		case WM_ACTIVATE:
			// check minimization state
			//bActive = !HIWORD(wParam);
			return 0;
	
		/*case WM_SYSCOMMAND:
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;*/

		case WM_CLOSE:
			m_quit_count++;
			m_did_quit = true;

			if (m_quit_count >= m_ref_count)
				PostQuitMessage(0);
			else
				ShowWindow(m_hWnd, SW_HIDE);

			return 0;

		case WM_LBUTTONDOWN:
			{
				if (w != NULL && w->m_eType == eImage)
				{
					Win32ImageWidget *iw = (Win32ImageWidget*)w;

					unsigned int x = GET_X_LPARAM(lParam);
					unsigned int y = GET_Y_LPARAM(lParam);

					if (iw->mouse_down)
					{
						iw->mouse_down = false;
					}
					else
					{
						iw->mouse_start_x = x;
						iw->mouse_start_y = y; 
						iw->mouse_current_x = iw->mouse_start_x;
						iw->mouse_current_y = iw->mouse_start_y;

						if (x < (unsigned int) iw->m_iWidth && y < (unsigned int) iw->m_iHeight)
							iw->mouse_down = true;
					}
				}
				
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseDown((WIDGET_HANDLE)w, IVT_LEFT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}

					SetFocus(hWnd);
				}
			}
			break;
			
		case WM_RBUTTONDOWN:
			{
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseDown((WIDGET_HANDLE)w, IVT_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}

					SetFocus(hWnd);
				}
			}
			break;
			
		case WM_MBUTTONDOWN:
			{
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseDown((WIDGET_HANDLE)w, IVT_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}

					SetFocus(hWnd);
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				if (w != NULL && w->m_eType == eImage)
				{
					Win32ImageWidget *iw = (Win32ImageWidget*)w;

					if (iw->mouse_down)
					{
						unsigned int old_curr_x = iw->mouse_current_x;
						unsigned int old_curr_y = iw->mouse_current_y;

						unsigned int x = GET_X_LPARAM(lParam);
						unsigned int y = GET_Y_LPARAM(lParam);

						if (x < (unsigned int) iw->m_iWidth && y < (unsigned int) iw->m_iHeight)
						{
							iw->mouse_current_x = x;
							iw->mouse_current_y = y;

							// tell the window it has to redraw the area
							RECT		WindowRect;

							unsigned int x0 = iw->mouse_start_x;
							unsigned int y0 = iw->mouse_start_y;
							unsigned int x1 = iw->mouse_current_x;
							unsigned int y1 = iw->mouse_current_y;

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

							WindowRect.left = x0;
							WindowRect.right = x1;
							WindowRect.top = y0;
							WindowRect.bottom = y1;

							InvalidateRect(iw->m_hWnd, &WindowRect, false);

							// also invalidate the previous rect
							x0 = iw->mouse_start_x;
							y0 = iw->mouse_start_y;
							x1 = old_curr_x;
							y1 = old_curr_y;

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

							WindowRect.left = x0;
							WindowRect.right = x1;
							WindowRect.top = y0;
							WindowRect.bottom = y1;

							InvalidateRect(iw->m_hWnd, &WindowRect, false);
						}
					}
				}
				
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseMove((WIDGET_HANDLE)w, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}
				}
			}
			
			break;

		case WM_LBUTTONUP:
			{
				if (w != NULL && w->m_eType == eImage)
				{
					Win32ImageWidget *iw = (Win32ImageWidget*)w;

					if (iw->mouse_down)
					{
						iw->mouse_down = false;

						if (m_event_callback != NULL)
						{
							if (iw->mouse_start_x == iw->mouse_current_x && iw->mouse_start_y == iw->mouse_current_y)
							{
								m_event_callback->PointClicked((WIDGET_HANDLE)w, iw->mouse_start_x, iw->mouse_start_y);
							}
							else
							{
								unsigned int x0 = iw->mouse_start_x;
								unsigned int y0 = iw->mouse_start_y;
								unsigned int x1 = iw->mouse_current_x;
								unsigned int y1 = iw->mouse_current_y;

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

								m_event_callback->RectSelected((WIDGET_HANDLE)w, x0, y0, x1, y1);
							}
						}

						memcpy(iw->m_pPixels, iw->m_buffer, 3*iw->m_iWidth*iw->m_iHeight);
						InvalidateRect(iw->m_hWnd, NULL, false);
					}
				}
				
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseUp((WIDGET_HANDLE)w, IVT_LEFT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}
				}
			}
			break;
			
		case WM_RBUTTONUP:
			{
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseUp((WIDGET_HANDLE)w, IVT_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}
				}
			}
			break;
		
		case WM_MBUTTONUP:
			{
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						m_event_callback->MouseUp((WIDGET_HANDLE)w, IVT_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					}
				}
			}
			break;

		case WM_KEYDOWN:
			{
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						char key[2] = {0, 0};
						BYTE keys[256];

						GetKeyboardState(keys);

						int c = ToAscii((unsigned int) wParam, MapVirtualKey((unsigned int) wParam, 0), keys, (unsigned short*)key, 0);
						if (c == 1)
							m_event_callback->KeyDown((WIDGET_HANDLE)w, key[0]);
						else
							m_event_callback->KeyDown((WIDGET_HANDLE)w, 0);
					}
				}
			}
			break;

		case WM_KEYUP:
			{
				if (w != NULL && (w->m_eType == eImage || w->m_eType == eGLWidget))
				{
					if (m_event_callback != NULL)
					{
						char key[2] = {0, 0};
						BYTE keys[256];

						GetKeyboardState(keys);

						int c = ToAscii((unsigned int) wParam, MapVirtualKey((unsigned int) wParam, 0), keys, (unsigned short*)key, 0);
						if (c == 1)
							m_event_callback->KeyUp((WIDGET_HANDLE)w, key[0]);
						else
							m_event_callback->KeyUp((WIDGET_HANDLE)w, 0);
					}
				}
			}
			break;

		case WM_COMMAND:
			{
				int idx = LOWORD(wParam); // the index into the widgets array
				int cmd = HIWORD(wParam); // the index into the widgets array

				// make sure it's a valid index
				const int nSize = (int) m_widgets.size();
				if (idx != 0 && idx > 0 && idx <= nSize)
				{
					w = m_widgets[idx-1];

					if (w != NULL)
					{
						if (w->m_eType == eButton)
						{
							if (m_event_callback != NULL)
							{
								m_event_callback->ButtonPushed((WIDGET_HANDLE)w);
							}
	
							return 0;
						}
						else if (w->m_eType == eCheckBox)
						{
							if (m_event_callback != NULL)
							{
								BOOL checked = IsDlgButtonChecked(m_hWnd, idx);

								m_event_callback->ValueChanged((WIDGET_HANDLE)w, (checked ? 1 : 0));
							}
	
							return 0;
						}
						else if (w->m_eType == eTextEdit && cmd == EN_CHANGE)
						{
							if (m_event_callback != NULL)
							{
								m_event_callback->ValueChanged((WIDGET_HANDLE)w, -1);
							}
	
							return 0;
						}
						else if (w->m_eType == eComboBox && cmd == CBN_SELCHANGE)
						{
							if (m_event_callback != NULL)
							{
								int value = (int) SendMessage(w->m_hWnd, CB_GETCURSEL, 0, 0);

								m_event_callback->ValueChanged((WIDGET_HANDLE)w, value);
							}
	
							return 0;
						}
					}
				}
			}
			break;

		case WM_NOTIFY:
			{
				int idx = (int) wParam; // the index into the widgets array
				NMHDR *nmhdr = (LPNMHDR)lParam;

				// make sure it's a valid index
				const int nSize = (int) m_widgets.size();
				if (idx != 0 && idx > 0 && idx <= nSize)
				{
					w = m_widgets[idx-1];

					if (w != NULL)
					{
						if (w->m_eType == eSlider)
						{
							if (m_event_callback != NULL && nmhdr->code == -12 /* code for dragging */)
							{
								int value = (int) SendMessage(w->m_hWnd, TBM_GETPOS, 0, 0);

								m_event_callback->ValueChanged((WIDGET_HANDLE)w, value);
							}
	
							return 0;
						}
					}
				}
			}
			break;

		case WM_SIZE:
			break;

		case WM_PAINT:
			{
				if (w != NULL && w->m_eType == eImage)
				{
					Win32ImageWidget *iw = (Win32ImageWidget*)w;

					if (iw->m_hBmp != 0)
					{
						PAINTSTRUCT ps;
						HDC dc = BeginPaint(iw->m_hWnd, &ps);

						if (iw->mouse_down && (iw->mouse_start_x != iw->mouse_current_x || iw->mouse_start_y != iw->mouse_current_y))
						{
							unsigned int x0 = iw->mouse_start_x;
							unsigned int y0 = iw->mouse_start_y;
							unsigned int x1 = iw->mouse_current_x;
							unsigned int y1 = iw->mouse_current_y;

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

							memcpy(iw->m_pPixels, iw->m_buffer, 3*iw->m_iWidth*iw->m_iHeight);
							DrawRect(iw->m_pPixels, iw->m_iWidth, iw->m_iHeight, x0, y0, x1, y1);
						}
						

						BitBlt(dc, 0, 0, iw->m_iWidth, iw->m_iHeight, iw->m_hDCMem, 0, 0, SRCCOPY);

						EndPaint(w->m_hWnd, &ps);

						return 0;
					}
				}
			}

			break;
	}
	
	// pass all unhandled messages to DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int CWin32MainWindow::m_ref_count = 0;
int CWin32MainWindow::m_quit_count = 0;

CWin32MainWindow::CWin32MainWindow(int x, int y, int width, int height, const char *title)
{
	m_did_quit = false;

	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

	WindowRect.left = x;
	WindowRect.right = x + width;
	WindowRect.top = y;
	WindowRect.bottom = y + height;

	m_hInstance = GetModuleHandle(NULL);

	if (m_ref_count == 0)
	{
		wc.style			= CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
		wc.cbClsExtra		= 0;									// No Extra Class Data
		wc.cbWndExtra		= 8;									// 4 bytes of Extra Window Data
		wc.hInstance		= m_hInstance;							// Set The Instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= GetSysColorBrush(COLOR_BTNFACE);	// Background
		wc.lpszMenuName		= NULL;									// We Don't Want A Menu
		wc.lpszClassName	= DEFAULT_CLASS_NAME;					// Set The Class Name

		if (!RegisterClass(&wc))
		{
			MessageBox(NULL,"Failed To Register The MainWindow Class.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	

		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
		wc.cbClsExtra		= 0;									// No Extra Class Data
		wc.cbWndExtra		= 8;									// 4 bytes of Extra Window Data
		wc.hInstance		= m_hInstance;							// Set The Instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= NULL; //GetSysColorBrush(COLOR_BTNFACE);		// No Background Required For GL
		wc.lpszMenuName		= NULL;									// We Don't Want A Menu
		wc.lpszClassName	= IMAGE_WINDOW_CLASS_NAME;					// Set The Class Name

		if (!RegisterClass(&wc))
		{
			MessageBox(NULL,"Failed To Register The Image Window Class.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}

	m_ref_count++;
	
	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size
	
	// make sure the window is positioned within the desktop
	if (WindowRect.top < 0)
	{
		WindowRect.bottom -= WindowRect.top;
		WindowRect.top = 0;
	}
	if (WindowRect.left < 0)
	{
		WindowRect.right -= WindowRect.left;
		WindowRect.left = 0;
	}

	// Create The Window
	if (!(m_hWnd = CreateWindowEx(dwExStyle,						// Extended Style For The Window
								DEFAULT_CLASS_NAME,					// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								WindowRect.left, WindowRect.top,	// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								m_hInstance,						// Instance
								this)))								// Pass this pointer to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create window.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_event_callback = NULL;

	//ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
}

CWin32MainWindow::~CWin32MainWindow()
{
	int i, c = (int) m_widgets.size();

	for (i = c - 1; i >= 0; i--)
	{
		Win32Widget *w = m_widgets[i];

		if (w->m_eType == eImage)
		{
			Win32ImageWidget *iw = (Win32ImageWidget*)w;

			if (iw->m_buffer != NULL)
			{
				delete [] iw->m_buffer;
				iw->m_buffer = NULL;
			}

			if (iw->m_hBmp != NULL)
			{
				SelectObject(iw->m_hDCMem, iw->m_hBmpOld);
				DeleteObject(iw->m_hBmp);

				iw->m_hBmp = 0;
				iw->m_hBmpOld = 0;
			}

			if (iw->m_hDCMem && !DeleteDC(iw->m_hDCMem))
			{
				MessageBox(NULL, "Release Compatible Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				iw->m_hDCMem = NULL;
			}

			if (iw->m_hDC && !ReleaseDC(iw->m_hWnd, iw->m_hDC))
			{
				MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				iw->m_hDC = NULL;
			}
		}

		if (w->m_eType == eGLWidget)
		{
			Win32GLWidget *glw = (Win32GLWidget*)w;

			wglMakeCurrent(NULL, NULL);

			if (glw->m_hGLRC && !wglDeleteContext(glw->m_hGLRC))
			{
				MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				glw->m_hGLRC = NULL;
			}

			if (glw->m_hDC && !ReleaseDC(glw->m_hWnd, glw->m_hDC))
			{
				MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
				glw->m_hDC = NULL;
			}
		}

		if (w->m_hWnd && !DestroyWindow(w->m_hWnd))
		{
			MessageBox(NULL,"Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			w->m_hWnd = NULL;
		}

		delete w;
	}

	m_widgets.clear();

	if (m_hWnd && !DestroyWindow(m_hWnd))
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		m_hWnd = NULL;
	}

	m_ref_count--;
	if (m_did_quit)
		m_quit_count--;

	if (m_ref_count == 0 && m_hInstance)
	{
		if (!UnregisterClass(DEFAULT_CLASS_NAME, m_hInstance) || !UnregisterClass(IMAGE_WINDOW_CLASS_NAME, m_hInstance))
		{
			MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			m_hInstance = NULL;
		}
	}
}

// create widgets
WIDGET_HANDLE CWin32MainWindow::AddImage(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32ImageWidget *iw = new Win32ImageWidget;

	iw->m_eType = eImage;
	iw->m_hWnd = 0;
	iw->m_hDC = 0;
	iw->m_hDCMem = 0;
	iw->m_hBmp = 0;
	iw->m_hBmpOld = 0;
	iw->m_pPixels = NULL;
	iw->m_iWidth = 0;
	iw->m_iHeight = 0;
	iw->m_pImage = NULL;
	iw->m_buffer = NULL;

	iw->mouse_down = false;
	iw->mouse_start_x = 0;
	iw->mouse_start_y = 0;
	iw->mouse_current_x = 0;
	iw->mouse_current_y = 0;

	// Create The Window
	if (!(iw->m_hWnd = CreateWindowEx(0,							// Extended Style For The Window
								IMAGE_WINDOW_CLASS_NAME,			// Class Name
								"",									// Window Title
								WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE,				// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								NULL,								// No Menu
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create image window.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		delete iw;
		return 0;
	}

	SetWindowPos(iw->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (!(iw->m_hDC = GetDC(iw->m_hWnd)))
	{
		MessageBox(NULL, "Couldn't create device context." , "ERROR", MB_OK | MB_ICONEXCLAMATION);
		delete iw;
		return 0;
	}

	if (!(iw->m_hDCMem = CreateCompatibleDC(iw->m_hDC)))
	{
		MessageBox(NULL, "Couldn't create compatible device context." , "ERROR", MB_OK | MB_ICONEXCLAMATION);
		delete iw;
		return 0;
	}

	iw->m_index = (int) m_widgets.size();
	m_widgets.push_back(iw);

	return (WIDGET_HANDLE) iw;
}

WIDGET_HANDLE CWin32MainWindow::AddButton(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32Widget *w = new Win32Widget;

	w->m_eType = eButton;
	w->m_hWnd = 0;

	// Create The Window
	if (!(w->m_hWnd = CreateWindowEx(0,								// Extended Style For The Window
								"button",							// Class Name
								text,								// Window Title
								WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE,				// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								(HMENU)(m_widgets.size()+1),		// The index into the widgets array is stored as the button id
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create button.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete w;

		return 0;
	}

	SetWindowPos(w->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	w->m_index = (int) m_widgets.size();
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CWin32MainWindow::AddLabel(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32Widget *w = new Win32Widget;

	w->m_eType = eLabel;
	w->m_hWnd = 0;

	// Create The Window
	if (!(w->m_hWnd = CreateWindowEx(0,								// Extended Style For The Window
								"STATIC",							// Class Name
								text,								// Window Title
								WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE,				// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								(HMENU)(m_widgets.size()+1),		// The index into the widgets array is stored as the button id
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create label.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete w;

		return 0;
	}

	SetWindowPos(w->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	w->m_index = (int) m_widgets.size();
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CWin32MainWindow::AddCheckBox(int x, int y, int width, int height, const char *text, bool checked, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32Widget *w = new Win32Widget;

	w->m_eType = eCheckBox;
	w->m_hWnd = 0;

	// Create The Window
	if (!(w->m_hWnd = CreateWindowEx(0,								// Extended Style For The Window
								"button",							// Class Name
								text,								// Window Title
								WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX,// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								(HMENU)(m_widgets.size()+1),		// The index into the widgets array is stored as the button id
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create check box.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete w;

		return 0;
	}

	SetWindowPos(w->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	w->m_index = (int) m_widgets.size();
	m_widgets.push_back(w);

	if (checked)
		CheckDlgButton(m_hWnd, w->m_index+1, BST_CHECKED);
	else
		CheckDlgButton(m_hWnd, w->m_index+1, BST_UNCHECKED);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CWin32MainWindow::AddTextEdit(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32Widget *w = new Win32Widget;

	w->m_eType = eTextEdit;
	w->m_hWnd = 0;

	// Create The Window
	if (!(w->m_hWnd = CreateWindowEx(WS_EX_STATICEDGE,				// Extended Style For The Window
								"Edit",								// Class Name
								text,								// Window Title
								WS_CHILD | WS_VISIBLE | WS_BORDER,	// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								(HMENU)(m_widgets.size()+1),		// The index into the widgets array is stored as the button id
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create text edit.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete w;

		return 0;
	}

	SetWindowPos(w->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	w->m_index = (int) m_widgets.size();
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CWin32MainWindow::AddSlider(int x, int y, int width, int height, int min_value, int max_value, int step, int value, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32Widget *w = new Win32Widget;

	w->m_eType = eSlider;
	w->m_hWnd = 0;

	// Create The Window
	if (!(w->m_hWnd = CreateWindowEx(0,								// Extended Style For The Window
								TRACKBAR_CLASS,						// Class Name
								"Trackbar",								// Window Title
								WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,	// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								(HMENU)(m_widgets.size()+1),		// The index into the widgets array is stored as the button id
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create text edit.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete w;

		return 0;
	}

	SendMessage(w->m_hWnd, TBM_SETRANGE,  TRUE,  MAKELONG(min_value, max_value)); 
	SendMessage(w->m_hWnd, TBM_SETPAGESIZE, 0,  step); 
	SendMessage(w->m_hWnd, TBM_SETTICFREQ, step, 0); 
	SendMessage(w->m_hWnd, TBM_SETPOS, TRUE, value); 

	SetWindowPos(w->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	w->m_index = (int) m_widgets.size();
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CWin32MainWindow::AddComboBox(int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32Widget *w = new Win32Widget;

	w->m_eType = eComboBox;
	w->m_hWnd = 0;

	// Create The Window
	if (!(w->m_hWnd = CreateWindowEx(0,								// Extended Style For The Window
								"combobox",							// Class Name
								"ComboBox",							// Window Title
								WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,	// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								2*width,							// HACK: in win32 the height should incorporate the extended drop down list
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								(HMENU)(m_widgets.size()+1),		// The index into the widgets array is stored as the button id
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create text edit.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete w;

		return 0;
	}

	for (int i = 0; i < num_entries; i++)
	{
		SendMessage(w->m_hWnd, CB_ADDSTRING, 0, (LPARAM)entries[i]);
	}

	SendMessage(w->m_hWnd, CB_SETCURSEL, current_entry, 0);

	SetWindowPos(w->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	w->m_index = (int) m_widgets.size();
	m_widgets.push_back(w);

	return (WIDGET_HANDLE)w;
}

WIDGET_HANDLE CWin32MainWindow::AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent)
{
	Win32Widget *p = (Win32Widget*)parent;

	Win32GLWidget *glw = new Win32GLWidget;

	glw->m_eType = eGLWidget;
	glw->m_hWnd = 0;
	glw->m_hDC = 0;
	glw->m_iWidth = width;
	glw->m_iHeight = height;

	// Create The Window
	if (!(glw->m_hWnd = CreateWindowEx(0,							// Extended Style For The Window
								IMAGE_WINDOW_CLASS_NAME,			// Class Name (re-use the image window class
								"",									// Window Title
								WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE,				// Required Window Style
								x, y,								// Window Position
								width,								// Calculate Window Width
								height,								// Calculate Window Height
								(p != 0 ? p->m_hWnd : m_hWnd),		// Parent Window
								NULL,								// No Menu
								m_hInstance,						// Instance
								this)))								// Pass pointer to struct to WM_CREATE
	{
		MessageBox(NULL, "Couldn't create image window.", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete glw;

		return 0;
	}

	SetWindowPos(glw->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (!(glw->m_hDC = GetDC(glw->m_hWnd)))
	{
		MessageBox(NULL, "Couldn't create device context." , "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete glw;

		return 0;
	}

	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL 
		PFD_DOUBLEBUFFER,      // double buffered 
		PFD_TYPE_RGBA,         // RGBA type 
		24,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer 
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
	}; 
	
	int  iPixelFormat; 
 
	// get the best available match of pixel format for the device context  
	iPixelFormat = ChoosePixelFormat(glw->m_hDC, &pfd); 
 
	// make that the pixel format of the device context 
	BOOL bResult = SetPixelFormat(glw->m_hDC, iPixelFormat, &pfd);
	if (!bResult)
	{
		MessageBox(NULL, "ERROR: SetPixelFormat failed", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		delete glw;

		return NULL;
	}
	
	glw->m_hGLRC = wglCreateContext(glw->m_hDC);
	if (!glw->m_hGLRC)
	{
		MessageBox(NULL, "ERROR: couldn't allocate HGLRC", "ERROR", MB_OK | MB_ICONEXCLAMATION);

		glw;

		return NULL;
	}

	wglMakeCurrent(glw->m_hDC, glw->m_hGLRC);

	glw->m_index = (int) m_widgets.size();
	m_widgets.push_back(glw);

	return (WIDGET_HANDLE)glw;
}



// access to widget attributes
bool CWin32MainWindow::GetText(WIDGET_HANDLE widget, char *text, int len)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eButton || w->m_eType == eCheckBox)
	{
		Button_GetText(w->m_hWnd, text, len);
	}
	else if (w->m_eType == eLabel)
	{
		Static_GetText(w->m_hWnd, text, len);
	}
	else if (w->m_eType == eTextEdit)
	{
		GetWindowText(w->m_hWnd, text, len);
	}
	else
		return false;

	return true;
}
bool CWin32MainWindow::SetText(WIDGET_HANDLE widget, const char *text)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eButton || w->m_eType == eCheckBox)
	{
		Button_SetText(w->m_hWnd, text);
	}
	else if (w->m_eType == eLabel)
	{
		Static_SetText(w->m_hWnd, text);
	}
	else if (w->m_eType == eTextEdit)
	{
		SetWindowText(w->m_hWnd, text);
	}
	else
		return false;

	return true;
}

bool CWin32MainWindow::SetImage(WIDGET_HANDLE widget, const CByteImage *pImage)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eImage)
	{
		Win32ImageWidget *iw = (Win32ImageWidget*)w;

		iw->m_pImage = pImage;

		if (iw->m_iWidth != pImage->width || iw->m_iHeight != pImage->height)
		{
			if (iw->m_hBmp != 0)
			{
				SelectObject(iw->m_hDCMem, iw->m_hBmpOld);
				DeleteObject(iw->m_hBmp);

				iw->m_hBmp = 0;
				iw->m_hBmpOld = 0;
			}

			if (iw->m_buffer != NULL)
			{
				delete [] iw->m_buffer;
				iw->m_buffer = NULL;
			}

			BITMAPINFO bminfo;
			// create a bitmap with rgb pixels
			bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bminfo.bmiHeader.biWidth = pImage->width;
			bminfo.bmiHeader.biHeight = pImage->height;
			bminfo.bmiHeader.biPlanes = 1;
			bminfo.bmiHeader.biBitCount = 24;
			bminfo.bmiHeader.biCompression = BI_RGB;
			bminfo.bmiHeader.biSizeImage = 0; // it's OK for RGB images
			bminfo.bmiHeader.biXPelsPerMeter = 1;
			bminfo.bmiHeader.biYPelsPerMeter = 1;
			bminfo.bmiHeader.biClrUsed = 0;
			bminfo.bmiHeader.biClrImportant = 0;

			iw->m_hBmp = CreateDIBSection(iw->m_hDC, &bminfo, DIB_RGB_COLORS, (void**)&iw->m_pPixels, NULL, 0);

			if (!iw->m_hBmp)
			{
				MessageBox(NULL, "Couldn't create DIB." , "ERROR", MB_OK | MB_ICONEXCLAMATION);
			}
			
			iw->m_hBmpOld = (HBITMAP)SelectObject(iw->m_hDCMem, iw->m_hBmp);

			if (!iw->m_hBmpOld)
			{
				MessageBox(NULL, "Couldn't Select Object." , "ERROR", MB_OK | MB_ICONEXCLAMATION);
			}

			iw->m_iWidth = pImage->width;
			iw->m_iHeight = pImage->height;

			int padding = 4 - (3 * pImage->width) % 4;
			if (padding == 4) padding = 0;
			iw->m_buffer = new unsigned char[( 3 * pImage->width + padding) * pImage->height];
		}

		// upload the image into the DIB section
		const int width = pImage->width;
		const int height = pImage->height;
		int padding = 4 - (3 * width) % 4;
		if (padding == 4) padding = 0;
		
		if (pImage->type == CByteImage::eGrayScale)
		{
			const int out_offset = 6 * width + padding;

			unsigned char *out = iw->m_buffer + (3 * width + padding) * (height - 1);
			unsigned char *in = pImage->pixels;

			for (int y = 0; y < height; y++, out -= out_offset)
			{
				for (int x = 0; x < width; x++, out += 3, in++)
				{
					const unsigned int c = *in;
					out[0] = c;
					out[1] = c;
					out[2] = c;
				}
			}
		}
		else
		{
			const int out_offset = 6 * width + padding;

			unsigned char *out = iw->m_buffer + (3 * width + padding) * (height - 1);
			unsigned char *in = pImage->pixels;

			for (int y = 0; y < height; y++, out -= out_offset)
			{
				for (int x = 0; x < width; x++, out += 3, in += 3)
				{
					out[0] = in[2];
					out[1] = in[1];
					out[2] = in[0];
				}
			}
		}

		memcpy(iw->m_pPixels, iw->m_buffer, (3 * width + padding) * height);

		// tell the window it has to redraw the area
		InvalidateRect(iw->m_hWnd, NULL, false);

		return true;
	}

	return false;
}

bool CWin32MainWindow::GetValue(WIDGET_HANDLE widget, int &value)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eCheckBox)
	{
		BOOL checked = IsDlgButtonChecked(m_hWnd, w->m_index+1);

		value = (checked ? 1 : 0);

		return true;
	}
	else if (w->m_eType == eSlider)
	{
		value = (int) SendMessage(w->m_hWnd, TBM_GETPOS, 0, 0);

		return true;
	}
	else if (w->m_eType == eComboBox)
	{
		value = (int) SendMessage(w->m_hWnd, CB_GETCURSEL, 0, 0);

		return true;
	}

	return false;
}
bool CWin32MainWindow::SetValue(WIDGET_HANDLE widget, int value)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eCheckBox)
	{
		if (value != 0)
			CheckDlgButton(m_hWnd, w->m_index+1, BST_CHECKED);
		else
			CheckDlgButton(m_hWnd, w->m_index+1, BST_UNCHECKED);

		return true;
	}
	else if (w->m_eType == eSlider)
	{
		SendMessage(w->m_hWnd, TBM_SETPOS, TRUE, value);

		return true;
	}
	else if (w->m_eType == eComboBox)
	{
		SendMessage(w->m_hWnd, CB_SETCURSEL, value, 0);

		return true;
	}

	return false;
}

bool CWin32MainWindow::SwapBuffersGLWidget(WIDGET_HANDLE widget)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eGLWidget)
	{
		Win32GLWidget *glw = (Win32GLWidget*)w;

		SwapBuffers(glw->m_hDC);

		return true;
	}

	return false;
}

bool CWin32MainWindow::MakeCurrentGLWidget(WIDGET_HANDLE widget)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w->m_eType == eGLWidget)
	{
		Win32GLWidget *glw = (Win32GLWidget*)w;

		wglMakeCurrent(glw->m_hDC, glw->m_hGLRC);

		return true;
	}

	return false;
}


// window control
void CWin32MainWindow::Show(WIDGET_HANDLE widget)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w == NULL)
	{
		ShowWindow(m_hWnd, SW_SHOW);
		InvalidateRect(m_hWnd, NULL, false);
	}
	else
	{
		ShowWindow(w->m_hWnd, SW_SHOW);
		InvalidateRect(w->m_hWnd, NULL, false);
	}
}
void CWin32MainWindow::Hide(WIDGET_HANDLE widget)
{
	Win32Widget *w = (Win32Widget*)widget;

	if (w == NULL)
	{
		ShowWindow(m_hWnd, SW_HIDE);
	}
	else
	{
		ShowWindow(w->m_hWnd, SW_HIDE);
	}
}

int CWin32MainWindow::GetModifierKeyState()
{
	int result = 0;
	
	result |= GetAsyncKeyState(VK_SHIFT) < 0 ? IVT_SHIFT_KEY : 0;
    result |= GetAsyncKeyState(VK_MENU) < 0 ? IVT_ALT_KEY : 0;
    result |= GetAsyncKeyState(VK_CONTROL) < 0 ? IVT_CONTROL_KEY : 0;
    	
    return result;
}

Win32Widget *CWin32MainWindow::FindWidget(HWND handle)
{
	int i, c = (int) m_widgets.size();

	for (i = 0; i < c; i++)
	{
		Win32Widget *w = m_widgets[i];

		if (w->m_hWnd == handle)
			return w;
	}

	return NULL;
}
