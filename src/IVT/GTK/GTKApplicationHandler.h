// ****************************************************************************
// Filename:  GTKApplicationHandler.h
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


#ifndef _GTK_APPLICATION_HANDLER_H_
#define _GTK_APPLICATION_HANDLER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/ApplicationHandlerInterface.h"



// ****************************************************************************
// CGTKApplicationHandler
// ****************************************************************************

class CGTKApplicationHandler : public CApplicationHandlerInterface
{
public:
	CGTKApplicationHandler(int argc = 0, char **argv = 0);
	~CGTKApplicationHandler();

	bool ProcessEventsAndGetExit();
	void Reset();
	
	void SetExit(bool exit) { m_bExit = exit; } 

	static CGTKApplicationHandler *GetApplicationHandler() { return m_pGTKApplicationHandler; }
private:

	bool m_bExit;
	static CGTKApplicationHandler *m_pGTKApplicationHandler;
};



#endif /* _GTK_APPLICATION_HANDLER_H_ */
