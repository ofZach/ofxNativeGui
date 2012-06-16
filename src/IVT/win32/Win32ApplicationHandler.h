// ****************************************************************************
// Filename:  Win32ApplicationHandler.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _WIN32_APPLICATION_HANDLER_H_
#define _WIN32_APPLICATION_HANDLER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/ApplicationHandlerInterface.h"



// ****************************************************************************
// CWin32ApplicationHandler
// ****************************************************************************

class CWin32ApplicationHandler : public CApplicationHandlerInterface
{
public:
	CWin32ApplicationHandler();
	~CWin32ApplicationHandler();

	bool ProcessEventsAndGetExit();
	void Reset() {};
};



#endif /* _WIN32_APPLICATION_HANDLER_H_ */
