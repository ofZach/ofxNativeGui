// ****************************************************************************
// Filename:  QtApplicationHandler.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifdef USE_QT

#include "QtApplicationHandler.h"

#include <qapplication.h>

static char app_name[] = "IVT_APPLICATION\0";
static int my_argc = 1;
static char *my_argv[2] = { app_name, NULL};

CQtApplicationHandler *CQtApplicationHandler::m_pQtApplicationHandler = NULL;

CQtApplicationHandler::CQtApplicationHandler(int argc, char **argv)
{
	m_pApplication = 0;
	m_bExit = false;
	
	if (argc == 0 && argv == NULL)
	{
		m_pApplication = new QApplication(my_argc, my_argv);
	}
	else
	{
		my_argc = argc;
		m_pApplication = new QApplication(my_argc, argv);
	}
	
	m_pApplication->connect(m_pApplication, SIGNAL(lastWindowClosed()), this, SLOT(Exit()));

	m_pQtApplicationHandler = this;
	
	// undo the setting of the locale by Qt
	// this makes dialogs appear in english on any system
	// but it doesn't screw with sscanf and fscanf, which
	// is more important to us
	setlocale(LC_ALL, "C");
}

CQtApplicationHandler::~CQtApplicationHandler()
{
	if (m_pApplication)
		delete m_pApplication;

	m_pQtApplicationHandler = NULL;
}

bool CQtApplicationHandler::ProcessEventsAndGetExit()
{
	#ifdef QT_THREAD_SUPPORT
	
	m_pApplication->lock();
	m_pApplication->processEvents();
	m_pApplication->unlock();
	
	#else
	
	m_pApplication->processEvents();
	
	#endif
	
	return m_bExit;
}

void CQtApplicationHandler::Reset()
{
	if (!m_pApplication)
	{
		m_pApplication = new QApplication(my_argc, my_argv);
		m_pApplication->connect(m_pApplication, SIGNAL(lastWindowClosed()), this, SLOT(Exit()));
	}
	
	m_bExit = false;
}

#endif