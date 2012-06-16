// ****************************************************************************
// Filename:  Win32ApplicationHandler.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Win32ApplicationHandler.h"

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>


// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CWin32ApplicationHandler::CWin32ApplicationHandler()
{
	InitCommonControls();
}

CWin32ApplicationHandler::~CWin32ApplicationHandler()
{

}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CWin32ApplicationHandler::ProcessEventsAndGetExit()
{
	MSG msg;

	// check if a message is waiting
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return false;
}
