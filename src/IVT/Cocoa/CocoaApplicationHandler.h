// ****************************************************************************
// Filename:  CocoaApplicationHandler.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifdef TARGET_OSX

#ifndef _COCOA_APPLICATION_HANDLER_H_
#define _COCOA_APPLICATION_HANDLER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/ApplicationHandlerInterface.h"



// ****************************************************************************
// CCocoaApplicationHandler
// ****************************************************************************

class CCocoaApplicationHandler : public CApplicationHandlerInterface
{
	
public:
	CCocoaApplicationHandler();
	~CCocoaApplicationHandler();

	bool ProcessEventsAndGetExit();
	void Reset();

private:

	bool m_bCocoaInitialized;
};



#endif /* _COCOA_APPLICATION_HANDLER_H_ */


#endif