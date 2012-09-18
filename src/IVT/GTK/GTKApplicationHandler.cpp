// ****************************************************************************
// Filename:  GTKApplicationHandler.cpp
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


#include "GTKApplicationHandler.h"

#include <locale.h>

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>

static char app_name[] = "IVT_APPLICATION\0";
static int my_argc = 1;
static char *my_argv[2] = {app_name, NULL};

CGTKApplicationHandler *CGTKApplicationHandler::m_pGTKApplicationHandler = NULL;

CGTKApplicationHandler::CGTKApplicationHandler(int argc, char **argv)
{
	if (argv != NULL)
	{
		gtk_init(&argc, &argv);
	}
	else
	{
		char **ptr = my_argv;
		gtk_init(&my_argc, &ptr);

	}
	
	// undo the setting of the locale by GTK
	// this makes dialogs appear in english on any system
	// but it doesn't screw with sscanf and fscanf, which
	// is more important to us
	setlocale(LC_ALL, "C");

	m_pGTKApplicationHandler = this;
	m_bExit = false;
}

CGTKApplicationHandler::~CGTKApplicationHandler()
{
	m_pGTKApplicationHandler = NULL;
}

bool CGTKApplicationHandler::ProcessEventsAndGetExit()
{
	while (gtk_events_pending())
		gtk_main_iteration();
	
	return m_bExit;
}

void CGTKApplicationHandler::Reset()
{
	
}
