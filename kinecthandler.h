/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		kinecthandler.h
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
// Description:
/*!\class CKinectHandler
//	This class is responsible for kinect handling:
//	- finding kinect
//	- connecting to kinect
//	- tracking skeleton
//	- recognizing hands gesture
//	- collecting some information about current kinect status
//
*/
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-07	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef KINECTHANDLER_H
#define KINECTHANDLER_H

#define NOMINMAX

#include <QTime> // HASA
#include <Windows.h> // HASA
#include "NuiApi.h" // HASA
#include "kinectdetails.h" // HASA

class CKinectHandler
{
public:
	CKinectHandler();
	~CKinectHandler();

	// returns true when new skeleton is on board or when current skeleton was lost
	// generally when this method returns true then something is happen with main skeleton
	bool skeletonHandler();
	CKinectDetails getKinectDetails() const;
	bool startKinect();

private:
	// members
	INuiSensor * m_pSensor;

	HANDLE m_hNextSkeletonEvent;

	bool m_bIsOnRightSide;
	bool m_bIsOnLeftSide;

	QTime m_tRightTimestamp;
	QTime m_tLeftTimestamp;

	QTime m_tSplitedHandsTimestamp;

	bool m_bIsSlidingOn;
	bool m_bKinectDetailsUpdated; // true, then skeletonHandling should return true to say that something has changed

	int m_iCurrentTrackedSkeleton;

	// notification
	CKinectDetails m_kinectDetails;

	// time for switching on/off sliding in application
	const int m_iPrayTime;
	// how fast you should move your hand from left to right or reverse to slide
	const int m_iSlideSpeed;
	// how far you should move your hand on the right in order to slide
	const double m_dSlideXRangeOnTheRight;
	// how far you should move your hand on the left in order to slide
	const double m_dSlideXRangeOnTheLeft;
	// difference between left hand x and right hand x
	const double m_dXDelta;
	// difference between left hand y and right hand y
	const double m_dYDelta;

	// methods
	bool createFirstConnectedKinectSensor();

	void rightHandMoveHandling( const Vector4 & skeletonData );
	void leftHandMoveHandling( const Vector4 & skeletonData );
	void bothHandMoveHandling( const Vector4 & leftHandData, const Vector4 & rightHandData );

	void changeSlidingState();
	void readSettings();
};

#endif // KINECTHANDLER_H
