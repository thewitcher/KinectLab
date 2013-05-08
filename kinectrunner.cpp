/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		kinectrunner.cpp
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
#include "kinectrunner.h" // USES
#include "kinecthandler.h" // USES: skeletonHandler(), getKinectDetails()

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			This is reimplemented virtual function from QThread. Its main task is
//			to run reading from kinect sensor in infinite loop and emit signal
//			when kinect status is changed. Thanks to this overload kinect can work
//			in separated thread while GUI is always ready for user actions.
//			If kinect can be found, kinnect handler object tries to find another one
//			after 5 seconds. There can be maximum 5 attemps.
*/
void CKinectRunner::run()
{
	CKinectHandler kinectHandler;

	int attemptsCounter = 0;

	while ( kinectHandler.startKinect() == false )
	{
		qDebug() << "Invalid attempts: " << ++attemptsCounter;
		emit kinectConnectionInfoSignal( attemptsCounter );

		sleep( 5 );

	}

	emit newKinectDetailsSignal( kinectHandler.getKinectDetails() );

	while ( 1 )
	{
		if ( kinectHandler.skeletonHandler() == true )
		{
			emit newKinectDetailsSignal( kinectHandler.getKinectDetails() );
		}
	}
}
