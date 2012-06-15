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
// Filename:  GUIFactory.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _GUI_FACTORY_H_
#define _GUI_FACTORY_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CApplicationHandlerInterface;
class CMainWindowInterface;
class CByteImage;


// ****************************************************************************
// Declarations
// ****************************************************************************

CApplicationHandlerInterface *CreateApplicationHandler(int argc = 0, char **argv = 0);
CMainWindowInterface *CreateMainWindow(int x, int y, int width, int height, const char *title);

// opens a file dialog and returns true if a filename was selected. The complete path is returned in filename (the buffer has to be big enough)
// filter is a list of 2 * num_filter zero-terminated strings, two forming a pair of descriptor and format,
// e.g. const char *filter[2] = {"Text Files", "*.txt"}; int num_filter = 1;
bool FileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length);

// load an image from file. The supported filetypes depend on the platform
// QT/linux: PNG, BMP, XBM, XPM, PNM, JPEG, MNG and GIF (some depend on the compile settings of Qt)
// Win32: BMP, JPEG, WMF, ICO and GIF
// Cocoa/OS X: BMP, JPEG, PNG, TGA, GIF, others
bool LoadImageFromFile(const char *filename, CByteImage *pImage);



#endif /* _GUI_FACTORY_H_ */
