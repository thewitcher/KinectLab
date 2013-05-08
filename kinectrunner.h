/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		kinectrunner.h
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
// Description:
/*!\class CKinectRunner
//	KinnectRunner is a class responsible for running kinect-handler in different thread that GUI.
//	This is also a bridge between KinectHandler object and GUI. They can communicate using signals
//	with kinect details. GUI can use KinectDetails object to show some current information about kinect
//	on the screen.
//
*/
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-07	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef KINECTRUNNER_H
#define KINECTRUNNER_H

#include <QThread> // ISA
#include "kinectdetails.h" // HASA

class CKinectRunner : public QThread
{

	Q_OBJECT

signals:
	void newKinectDetailsSignal( CKinectDetails );

private:
	void run();
};

#endif // KINECTRUNNER_H
