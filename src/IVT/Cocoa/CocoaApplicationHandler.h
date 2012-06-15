// ****************************************************************************
// Filename:  CocoaApplicationHandler.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _COCOA_APPLICATION_HANDLER_H_
#define _COCOA_APPLICATION_HANDLER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "ApplicationHandlerInterface.h"



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
