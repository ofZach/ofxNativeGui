// ****************************************************************************
// This file is part of the Integrating Vision Toolkit (IVT).
//
// The IVT is maintained by the Karlsruhe Institute of Technology (KIT)
// (www.kit.edu) in cooperation with the company Keyetech (www.keyetech.de).
//
// Copyright (C) 2012 Karlsruhe Institute of Technology (KIT).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the KIT nor the names of its contributors may be
//    used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE KIT AND CONTRIBUTORS “AS IS” AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE KIT OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ****************************************************************************
// ****************************************************************************
// Filename:  MainWindowInterface.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

/** \defgroup GUI Graphical User Interface (GUI) API */


#ifndef _MAIN_WINDOW_INTERFACE_H_
#define _MAIN_WINDOW_INTERFACE_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
class CMainWindowEventInterface;


// ****************************************************************************
// Typedefs
// ****************************************************************************

typedef void* WIDGET_HANDLE;


// ****************************************************************************
// Defines
// ****************************************************************************

#define IVT_SHIFT_KEY		1
#define IVT_CONTROL_KEY		2
#define IVT_ALT_KEY		4



// ****************************************************************************
// CMainWindowInterface
// ****************************************************************************

/*!
	\ingroup GUI
	\brief Interface for the creation of GUIs with the GUI toolkit of the IVT.
*/
class CMainWindowInterface
{
public:
	// destructor
	virtual ~CMainWindowInterface() { }


	// public methods

	// create widgets
	virtual WIDGET_HANDLE AddImage(int x, int y, int width, int height, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddButton(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddLabel(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddCheckBox(int x, int y, int width, int height, const char *text, bool checked, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddTextEdit(int x, int y, int width, int height, const char *text, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddSlider(int x, int y, int width, int height, int min_value, int max_value, int step, int value, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddComboBox(int x, int y, int width, int height, int num_entries, const char **entries, int current_entry, WIDGET_HANDLE parent = 0) = 0;
	virtual WIDGET_HANDLE AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent = 0) = 0;

    
    //virtual WIDGET_HANDLE AddGLWidget(int x, int y, int width, int height, WIDGET_HANDLE parent = 0) = 0;
    

    
    
	// access to widget attributes
	virtual bool GetText(WIDGET_HANDLE widget, char *text, int len) = 0;
	virtual bool SetText(WIDGET_HANDLE widget, const char *text) = 0;

	virtual bool SetImage(WIDGET_HANDLE widget, const CByteImage *pImage) = 0;

	virtual bool GetValue(WIDGET_HANDLE widget, int &value) = 0;
	virtual bool SetValue(WIDGET_HANDLE widget, int value) = 0;

	virtual bool SwapBuffersGLWidget(WIDGET_HANDLE widget) = 0;
	virtual bool MakeCurrentGLWidget(WIDGET_HANDLE widget) = 0;

	// window control
	virtual void Show(WIDGET_HANDLE widget = 0) = 0;
	virtual void Hide(WIDGET_HANDLE widget = 0) = 0;
	
	virtual int GetModifierKeyState() = 0;

	virtual void SetEventCallback(CMainWindowEventInterface *callback) = 0;
};



#endif /* _MAIN_WINDOW_INTERFACE_H_ */
