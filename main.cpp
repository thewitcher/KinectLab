/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		main.cpp
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

#include <QApplication> // USES: exec()
#include "mainwindow.h" // USES: show()
#include "kinectdetails.h" // USES: class definition is needed to register new type

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Runs QApplication event loop and shows main window. It also register some
//			types in order to have possibility to keep that data in thread queue
//			(when signals are sending from one thread to another.
//
//\par		Parameters:
//\arg		[IN]		argc	argument counter, number of argument passed to program when it starts
//\arg		[IN]		argv	argument value, pointer to array of all passed parameters when program starts
//
//\par		Return value:
//			Return code.
//
//\retval	0	program has finished in normal way
//\retval	other than 0	program has finished with error code
*/
int main( int argc, char * argv[] )
{
	QApplication a( argc, argv );

	qRegisterMetaType<CKinectDetails>( "CKinectDetails" );

	CMainWindow w;
	w.show();

	return a.exec();
}
