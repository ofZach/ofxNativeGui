// ****************************************************************************
// Filename:  CocoaApplicationHandler.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "CocoaApplicationHandler.h"

#include <stdio.h>


// ****************************************************************************
// Global variables
// ****************************************************************************

const char *app_name = "IVT_APPLICATION";

extern "C"
{
	bool CocoaInitApplication(void);
	bool CocoaProcessEventsAndGetExit(void);
	void CocoaShutdownApplication(void);
};


// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CCocoaApplicationHandler::CCocoaApplicationHandler()
{
	m_bCocoaInitialized = CocoaInitApplication();
	if (!m_bCocoaInitialized)
		printf("error: couldn't initialize the CocoaApplicationHandler\n");
}

CCocoaApplicationHandler::~CCocoaApplicationHandler()
{
	if (m_bCocoaInitialized)
		CocoaShutdownApplication();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CCocoaApplicationHandler::ProcessEventsAndGetExit()
{
	return CocoaProcessEventsAndGetExit();
}

void CCocoaApplicationHandler::Reset()
{
	// TODO
}
