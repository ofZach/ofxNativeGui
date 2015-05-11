// ****************************************************************************
// Filename:  QtApplicationHandler.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifdef USE_QT

#ifndef _QT_APPLICATION_HANDLER_H_
#define _QT_APPLICATION_HANDLER_H_


// ****************************************************************************
// Includes
// ****************************************************************************

#include <qobject.h>
#include "Interfaces/ApplicationHandlerInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class QApplication;



// ****************************************************************************
// CQtApplicationHandler
// ****************************************************************************

class CQtApplicationHandler : public QObject, public CApplicationHandlerInterface
{
	Q_OBJECT
	
public:
	CQtApplicationHandler(int argc = 0, char **argv = 0);
	~CQtApplicationHandler();

	bool ProcessEventsAndGetExit();
	void Reset();

	static CQtApplicationHandler *GetApplicationHandler()
	{
		return m_pQtApplicationHandler;
	}
	
public slots:
	void Exit() {m_bExit = true; }

private:
	bool m_bExit;

	static CQtApplicationHandler	*m_pQtApplicationHandler;	
	QApplication			*m_pApplication;
};



#endif /* _QT_APPLICATION_HANDLER_H_ */

#endif