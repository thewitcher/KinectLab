/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		mainwindow.h
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
// Description:
/*!\class CMainWindow
//	Main window contains all necessary widgets. It is used to start/stop kinect and to show current status. There is also
//	page "About" and "Help" in order to give some help information to users. Advanced users can find out that there are some
//	options in "Options Page". Default values are enough to run application in standard way. It is recommended not to change
//	these option unless it is not necessary.
//
*/
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-07	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> // ISA
#include "kinectdetails.h" // HASA

namespace Ui
{
	class MainWindow; // HASA
}

class CKinectRunner; // HASA

class CMainWindow : public QMainWindow
{

	Q_OBJECT

public:
	explicit CMainWindow( QWidget * parent = 0 );
	~CMainWindow();

private:
	Ui::MainWindow * ui;
	CKinectRunner * m_pKinnectRunner;
	const int m_iAttemptsLimit;

	enum EstackedWidgetPage //! Pages in stacked widget
	{
		MAIN = 0,			//!< Main widget with start menu
		OPTIONS = 1,		//!< Options page
		ABOUT = 2,			//!< About page
		HELP = 3,			//!< Help page
		RUN = 4				//!< Run page
	};

	void initialize();
	void helpInitialize();
	void aboutInitialize();

private slots:
	void startKinectThread();
	void stopKinectThread();
	void closeEvent( QCloseEvent * event );
	void showAboutPage();
	void showOptionsPage();
	void showMainPage();
	void showHelpPage();
	void showRunPage();
	void settingsUpdate();
	void updateKinectStatus( const CKinectDetails & kinectDetails );
	void updateKinectStatus( int a_iNumberOfFailedTries );

	void backToDeafaultValues();
};

#endif // MAINWINDOW_H
