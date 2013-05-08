/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		mainwindow.cpp
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-08	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#include <QDebug> // USES: qDebug()
#include <QSettings> // USES: value(), setValue()
#include "mainwindow.h" // HASA
#include "ui_mainwindow.h" // HASA
#include "kinectrunner.h" // HASA

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Constructor. Creates new KinectRunner object in order to managing kinect
//			in different thread. This method also calls initialize function for preparing
//			all windows and createing all necessary connections.
//
//\par		Parameters:
//\arg		[IN/OUT]		parent	QWidget which will be new parent of MainWindow
*/
CMainWindow::CMainWindow( QWidget * parent ):
	QMainWindow( parent ),
	ui( new Ui::MainWindow ),
	m_pKinnectRunner( new CKinectRunner ),
	m_iAttemptsLimit( 5 )
{
	ui->setupUi( this );

	initialize();
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Main reason for creating this method is to create connections between
//			objects and slots. Before this is done some initializing function
//			are called in order to initializing about and help page. Initialize
//			function is responisble for preparing and showing main window frame.
*/
void CMainWindow::initialize()
{
	helpInitialize();
	aboutInitialize();
	showMainPage();

	connect(ui->m_pStartButton, SIGNAL(clicked()), this, SLOT(startKinectThread()));
	connect(ui->m_pExitButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->m_pAboutButton, SIGNAL(clicked()), this, SLOT(showAboutPage()));
	connect(ui->m_pBackFromAboutButton, SIGNAL(clicked()), this, SLOT(showMainPage()));
	connect(ui->m_pBackFromOptionsButton, SIGNAL(clicked()), this, SLOT(showMainPage()));
	connect(ui->m_pBackFromOptionsButton, SIGNAL(clicked()), this, SLOT(settingsUpdate()));
	connect(ui->m_pOptionsButton, SIGNAL(clicked()), this, SLOT(showOptionsPage()));
	connect(ui->m_pHelpButton, SIGNAL(clicked()), this, SLOT(showHelpPage()));
	connect(ui->m_pBackFromHelpButton, SIGNAL(clicked()), this, SLOT(showOptionsPage()));
	connect(ui->m_pDefaultButton, SIGNAL(clicked()), this, SLOT(backToDeafaultValues()));
	connect(ui->m_pStopButton, SIGNAL(clicked()), this, SLOT(stopKinectThread()));
	connect(m_pKinnectRunner, SIGNAL(newKinectDetailsSignal(CKinectDetails)), this, SLOT(updateKinectStatus(CKinectDetails)));
	connect(m_pKinnectRunner, SIGNAL(kinectConnectionInfoSignal(int)), this, SLOT(updateKinectStatus(int)));
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Destructor. Just freeing memory which was reserved earlier.
*/
CMainWindow::~CMainWindow()
{
	delete ui;
	delete m_pKinnectRunner;
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			This function is connected to signal from KinectRunner. It updates current kinect status on the screen.
//
//\par		Parameters:
//\arg		[IN]		kinectDetails	current kinect status (see CKinectDetails documentation for more information)
*/
void CMainWindow::updateKinectStatus( const CKinectDetails & kinectDetails )
{
	if ( kinectDetails.value( "connection" ) == false )
	{
		ui->m_pConnectionStatusLabel->setText( "<h3><font color=red> Kinect connection status: disconnected </color>" );
	}
	else
	{
		ui->m_pConnectionStatusLabel->setText( "<h3><font color=green> Kinect connection status: connected </color>" );
	}

	if ( kinectDetails.value( "skeleton_found" ) == true )
	{
		ui->m_pSkeletonTrackingStatusLabel->setText(
			"<h3><font color=green> Skeleton tracking status: skeleton was chosen and it is tracked </color>" );
	}
	else
	{
		ui->m_pSkeletonTrackingStatusLabel->setText(
			"<h3><font color=red> Skeleton tracking status: skeleton was lost </color>" );
	}

	if ( kinectDetails.value( "sliding" ) == true )
	{
		ui->m_pSlidingLabel->setText( "<h3><font color=green> Sliding status: enabled </color>" );
	}
	else
	{
		ui->m_pSlidingLabel->setText( "<h3><font color=red> Sliding status: disabled </color>" );
	}
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			This function is connected to signal from KinectRunner. It updates current kinect status on the screen.
//
//\par		Parameters:
//\arg		[IN]		numberOfFailedTries	how many times kinect was not found. If this value is greater then attempts limit
//						then kinect thread is terminated and main window is shown.
*/
void CMainWindow::updateKinectStatus( int a_iNumberOfFailedTries )
{
	if ( a_iNumberOfFailedTries > m_iAttemptsLimit )
	{
		ui->m_pConnectionStatusLabel->setText( "<h3><font color=red> Kinect connection status: disconnected </color>" );

		stopKinectThread();
	}

	ui->m_pConnectionStatusLabel->setText( "<h3><font color=red> Kinect connection status: disconnected ( attempt: " +
											QString::number( a_iNumberOfFailedTries ) + " ) </color>" );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Starts kinect thread and shows run page.
*/
void CMainWindow::startKinectThread()
{
	qDebug() << "Starts kinect thread";

	showRunPage();

	m_pKinnectRunner->start();
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Kills kinect thread if it still runs.
*/
void CMainWindow::stopKinectThread()
{
	qDebug() << "Stops kinect thread";

	showMainPage();

	if ( m_pKinnectRunner->isRunning() == true )
	{
		qDebug() << "Kinnect thread is running so it will be terminated before closing main window";
		m_pKinnectRunner->terminate();
	}
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Reimplemented virtual method from QWidget. Terminates kinect
//			thread and updates settings before application will be closed.
*/
void CMainWindow::closeEvent( QCloseEvent * event )
{
	qDebug() << "Closing main window";

	if ( m_pKinnectRunner->isRunning() == true )
	{
		qDebug() << "Kinnect thread is running so it will be terminated before closing main window";
		m_pKinnectRunner->terminate();
	}

	settingsUpdate();

	QMainWindow::closeEvent( event );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Shows about page. Updates settings.
*/
void CMainWindow::showAboutPage()
{
	settingsUpdate();

	ui->m_pStackedWidget->setCurrentIndex( ABOUT );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Shows run page.
*/
void CMainWindow::showRunPage()
{
	ui->m_pRunPageLabel->setText( "<h2>Run page" );
	ui->m_pConnectionStatusLabel->setText( "<h3><font color=red>Kinect status: kinect is disconnected now</color>" );
	ui->m_pSkeletonTrackingStatusLabel->setText(
		"<h3><font color=red>Skeleton tracking status: no sekelton tracking</color>" );

	ui->m_pStackedWidget->setCurrentIndex( RUN );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Shows options page. Reads options from configuration file. If file
//			does not exist then options are set to default values.
*/
void CMainWindow::showOptionsPage()
{
	qDebug() << "showOptionsPage()";

	// load settings
	QSettings settings( "settings.ini", QSettings::IniFormat );

	settings.beginGroup( "AlgorithmParameters" );
	ui->m_pPrayTimeSpinBox->setValue( settings.value( "prayTime", 5000  ).toInt() );
	ui->m_pSlideSpeedSpinBox->setValue( settings.value( "slideSpeed", 200  ).toInt() );
	ui->m_pRightRangeSpinBox->setValue( settings.value( "slideXRangeOnTheRight", 0.15  ).toDouble() );
	ui->m_pLeftRangeSpinBox->setValue( settings.value( "slideXRangeOnTheLeft", -0.15  ).toDouble() );
	ui->m_pXDeltaSpinBox->setValue( settings.value( "xDelta", 0.20  ).toDouble() );
	ui->m_pYDeltaSpinBox->setValue( settings.value( "yDelta", 0.50  ).toDouble() );
	settings.endGroup();

	ui->m_pStackedWidget->setCurrentIndex( OPTIONS );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Shows main page.
*/
void CMainWindow::showMainPage()
{
	ui->m_pStackedWidget->setCurrentIndex( MAIN );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Shows help page.
*/
void CMainWindow::showHelpPage()
{
	ui->m_pStackedWidget->setCurrentIndex( HELP );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Writes all setting/options into settings file.
*/
void CMainWindow::settingsUpdate()
{
	QSettings settings( "settings.ini", QSettings::IniFormat );

	settings.beginGroup( "AlgorithmParameters" );
	settings.setValue( "prayTime", ui->m_pPrayTimeSpinBox->value() );
	settings.setValue( "slideSpeed", ui->m_pSlideSpeedSpinBox->value() );
	settings.setValue( "slideXRangeOnTheRight", ui->m_pRightRangeSpinBox->value() );
	settings.setValue( "slideXRangeOnTheLeft", ui->m_pLeftRangeSpinBox->value() );
	settings.setValue( "xDelta", ui->m_pXDeltaSpinBox->value() );
	settings.setValue( "yDelta", ui->m_pYDeltaSpinBox->value() );
	settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Sets options values in options page to defaults.
*/
void CMainWindow::backToDeafaultValues()
{
	ui->m_pPrayTimeSpinBox->setValue( 5000 );
	ui->m_pSlideSpeedSpinBox->setValue( 200 );
	ui->m_pRightRangeSpinBox->setValue( 0.15 );
	ui->m_pLeftRangeSpinBox->setValue( -0.15 );
	ui->m_pXDeltaSpinBox->setValue( 0.20 );
	ui->m_pYDeltaSpinBox->setValue( 0.50 );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Prepares help page.
*/
void CMainWindow::helpInitialize()
{
	ui->m_pHelpLabel->setText( "<h3>This is a short description how to understand all options in options page:</h3><br>"
		"<h3>Pray time</h3> - it is a time to keep hands near each other for switching on/off sliding mode<br>"
		"<h3>Slide speed</h3> - how fast you should move your right hand in order to change slide<br>"
		"<h3>Slide x range on the right</h3> - how far you should move your hand in right direction in order to change slide<br>"
		"<h3>Slide y range on the left</h3> - how far you should move your hand in left direction in order to change slide<br>"
		"<h3>x delta</h3> - it is a difference between left hand x position and right hand x position. "
		"It defines how near hands are in horizontal<br>"
		"<h3>y delta</h3> - it is a difference between left hand y position and right hand y position. "
		"It defines how near hands are in vertical<br>");
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Prepares about page.
*/
void CMainWindow::aboutInitialize()
{
	ui->m_pAboutLabel->setText( "<h1>Application for hands slide detection</h1>" );
	ui->m_pPruftechnikLogoLabel->setText( "Logo" );
	ui->m_pAuthorDetailsLabel->setText( "Designed by Marcin Haber from Pruftechnik Technology" );
}
