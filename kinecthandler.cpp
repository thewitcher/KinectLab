/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		kinecthandler.cpp
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-07	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#include <QDebug> // USES: qDebug()
#include <QSettings> // USES: calue(), setValue()
#include "systeminputhandler.h" // USES: sendPressInput( KEYS key )
#include "kinecthandler.h" // USES

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Sets all memeber to their default values,
//			reads configuration from settings file and tries to find first connected kinect device.
*/
CKinectHandler::CKinectHandler():
	m_bIsOnRightSide( false ),
	m_bIsOnLeftSide( false ),
	m_pSensor( NULL ),
	m_tRightTimestamp( 0, 0 ),
	m_tLeftTimestamp( 0, 0 ),
	m_tSplitedHandsTimestamp( 0, 0 ),
	m_bIsSlidingOn( false ),
	m_iPrayTime( 5000 ),
	m_iSlideSpeed( 200 ),
	m_dSlideXRangeOnTheRight( 0.15 ),
	m_dSlideXRangeOnTheLeft( -0.15 ),
	m_dXDelta( 0.20 ),
	m_dYDelta( 0.50 ),
	m_iCurrentTrackedSkeleton( -1 )
{
	readSettings();
	createFirstConnectedKinectSensor();
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Destructor which kills object responsible for communicating with kinect device.
*/
CKinectHandler::~CKinectHandler()
{
	m_pSensor->NuiShutdown();

	qDebug() << "m_pSensor->NuiShutdown()";
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Initializes some parameters from configuration file. It uses bad const_cast, becuase these parameters are const
//			in order to keep them in unchanged state. If file doesn't exists then parameters are initialized with default
//			parameters.
*/
void CKinectHandler::readSettings()
{
	qDebug() << "readSettings()";

	QSettings settings( "settings.ini", QSettings::IniFormat );

	settings.beginGroup( "AlgorithmParameters" );
	const_cast< int & >( m_iPrayTime ) = settings.value( "prayTime", 5000 ).toInt();
	const_cast< int & >( m_iSlideSpeed ) = settings.value( "slideSpeed", 200 ).toInt();
	const_cast< double & >( m_dSlideXRangeOnTheLeft ) = settings.value( "slideXRangeOnTheLeft", -0.15 ).toDouble();
	const_cast< double & >( m_dSlideXRangeOnTheRight ) = settings.value( "slideXRangeOnTheRight", 0.15 ).toDouble();
	const_cast< double & >( m_dXDelta ) = settings.value( "xDelta", 0.20 ).toDouble();
	const_cast< double & >( m_dYDelta ) = settings.value( "yDelta", 0.50 ).toDouble();
	settings.endGroup();

	qDebug() << "Pray time: " << m_iPrayTime;
	qDebug() << "Slide speed: " << m_iSlideSpeed;
	qDebug() << "Slide x range on the right: " << m_dSlideXRangeOnTheRight;
	qDebug() << "Slide x range on the left: " << m_dSlideXRangeOnTheLeft;
	qDebug() << "xDelta: " << m_dXDelta;
	qDebug() << "yDelta: " << m_dYDelta;
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Sets all memeber to their default values,
//			reads configuration from settings file and tries to find first connected kinect device.
//
//\par		Return value:
//			True value is a expected value. When this method returns false then something wrong is with
//			with kinect sensor. It can be disconnected or drivers are incompatible. First thing to do
//			in this case is to check green led on kinect sensor. It should shines all the time. Flashing
//			is not allowed.
//
//\retval	true	kinect is ready to work
//\retval	false	kinect is not found
*/
bool CKinectHandler::createFirstConnectedKinectSensor()
{
	qDebug() << "createFirstConnectedKinectSensor()";

	INuiSensor * pSensor;
	int iSensorCount = 0;

	// HRESULT - just standard long
	HRESULT hr = NuiGetSensorCount( & iSensorCount );
	if ( FAILED( hr ) ) // checks if hr is less than 0
	{
		qDebug() << "Cannot get available kinect sensor count";
		return false;
	}

	// checks all sensors and takes first one which can be created
	for ( int i = 0 ; i < iSensorCount ; i++ )
	{
		hr = NuiCreateSensorByIndex( i, & pSensor );

		if ( FAILED( hr ) )
		{
			qDebug() << "Cannot create sensor with index: " << i << ", check next index";
			continue;
		}

		hr = pSensor->NuiStatus();

		if ( hr == S_OK ) // sensor is connected, it can be initialized now
		{
			m_pSensor = pSensor;
			qDebug() << "Sensor with index: " << i << " is connected - breaking loop";

			break;
		}

		// release not ok sensor
		pSensor->Release();
	}

	if ( m_pSensor != NULL )
	{
		hr = m_pSensor->NuiInitialize( NUI_INITIALIZE_FLAG_USES_SKELETON ); // in order to use skeleton

		if ( SUCCEEDED( hr ) )
		{
			// create event to inform application about new skeleton frame
			m_hNextSkeletonEvent = CreateEventW( NULL, TRUE, FALSE, NULL );

			// open skeleton stream to get skeleton data
			hr = m_pSensor->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
		}
	}

	if ( m_pSensor == NULL || FAILED( hr ) )
	{
		qDebug() << "No ready kinect found";
		m_kinectDetails.insert( "connection", false );

		return false;
	}

	m_kinectDetails.insert( "connection", true );
	m_kinectDetails.insert( "skeleton_found", false );

	return true;
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Skeleton handler function is responisble for tracking skeleton. It tries to find
//			first skeleton and remember it. If it is out of kinect view then new skeleton can
//			be tracked. New skeleton can be tracked even when first skeleton is cover by second
//			or by other thing. This is main function in skeleton tracking, so it calls some
//			subfunctions in order to gesture handling.
//
//\par		Return value:
//			Returns true when new skeleton is tracked or old skeleton is out of range. In general
//			this method return true if kinect details were changed.
//
//\retval	true	some informations about kinect have been changed
//\retval	false	no need to update kinect status, so the same skeleton is still tracked
//					or there is still no skeleton to track
*/
bool CKinectHandler::skeletonHandler()
{
	if ( WAIT_OBJECT_0 == WaitForSingleObject( m_hNextSkeletonEvent, 0 ) )
	{
		NUI_SKELETON_FRAME skeletonFrame = { 0 };

		// get the skeleton frame that is ready
		if ( SUCCEEDED( m_pSensor->NuiSkeletonGetNextFrame( 0, & skeletonFrame ) ) )
		{
			m_pSensor->NuiTransformSmooth( & skeletonFrame, NULL );

			// process the skeleton frame
			for ( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
			{
				NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[ i ].eTrackingState;

				if ( NUI_SKELETON_TRACKED == trackingState )
				{
					// is current skeleton index is -1 then try to track another one
					if ( m_iCurrentTrackedSkeleton == -1 )
					{
						qDebug() << "New skeleton found: " << i;

						m_iCurrentTrackedSkeleton = i;

						m_kinectDetails.insert( "skeleton_found", true );
						m_bKinectDetailsUpdated = true;
					}

					if ( m_iCurrentTrackedSkeleton == i ) // track only one skeleton
					{
						bothHandMoveHandling(
							skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_LEFT ],
							skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ] );

						break;
					}
				}
				else // untracked state
				{
					if ( m_iCurrentTrackedSkeleton == i ) // try to track another one if previous can't be tracked
					{
						m_iCurrentTrackedSkeleton = -1;

						qDebug() << "Skeleton lost";

						m_kinectDetails.insert( "skeleton_found", false );
						m_bKinectDetailsUpdated = true;
					}
				}
			}
		}

		ResetEvent( m_hNextSkeletonEvent );
	}

	bool update = m_bKinectDetailsUpdated;
	m_bKinectDetailsUpdated = false;

	return update;
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Recognizes right hand sliding from the right to left and in reverse.
//			It also calls approriate methods in order to handle this recognition.
//
//\par		Parameters:
//\arg		[IN]		skeletonData	four dimensions vector with x, y, z and w elements.
//										First three are standard coordinates in three dimension
//										space. The fourth element is a quality level from [0,1]
//										range. In this case contains data about right hand
*/
void CKinectHandler::rightHandMoveHandling( const Vector4 & skeletonData )
{
	if ( m_bIsSlidingOn == true )
	{
		if ( skeletonData.x > m_dSlideXRangeOnTheRight )
		{
			if ( m_bIsOnLeftSide == true )
			{
				if ( m_tRightTimestamp.msecsTo( QTime::currentTime() ) < m_iSlideSpeed )
				{
					qDebug() << "Swipe right: " << m_tRightTimestamp.msecsTo( QTime::currentTime() );
					CSystemInputHandler::sendPressInput( CSystemInputHandler::RIGHT_ARROW );
				}
			}

			m_bIsOnRightSide = true;
			m_bIsOnLeftSide = false;

			m_tLeftTimestamp = QTime::currentTime();
		}
		else if ( skeletonData.x < m_dSlideXRangeOnTheLeft )
		{
			if ( m_bIsOnRightSide == true )
			{
				if ( m_tLeftTimestamp.msecsTo( QTime::currentTime() ) < m_iSlideSpeed )
				{
					qDebug() << "Swipe left: " << m_tLeftTimestamp.msecsTo( QTime::currentTime() );
					CSystemInputHandler::sendPressInput( CSystemInputHandler::LEFT_ARROW );
				}
			}

			m_bIsOnRightSide = false;
			m_bIsOnLeftSide = true;

			m_tRightTimestamp = QTime::currentTime();
		}
	}
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			It is not necessary to handle left hand tracing.
//
//\par		Parameters:
//\arg		[IN]		skeletonData	four dimensions vector with x, y, z and w elements.
//										First three are standard coordinates in three dimension
//										space. The fourth element is a quality level from [0,1]
//										range. In this case contains data about right hand
*/
void CKinectHandler::leftHandMoveHandling( const Vector4 & skeletonData )
{
	Q_UNUSED( skeletonData )
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			This method calls leftHandMoveHandling and rightHandMoveHandling to handle
//			both hands in separated way. Sometimes is necessary to recognize a gesture
//			regarding to two hands in the same time. This is a place to implementing all
//			gestures with two hand. To do it, developer needs to have data about
//			position of both hand. Now it implements something similar to pray gesture.
//			It is used for switching on/off sliding. Pray gesture is just jointed hands
//			for configured time.
//
//\par		Parameters:
//\arg		[IN]		leftHandData	four dimensions vector with x, y, z and w elements.
//										First three are standard coordinates in three dimension
//										space. The fourth element is a quality level from [0,1]
//										range. In this case contains data about left hand
//\arg		[IN]		rightHandData	four dimensions vector with x, y, z and w elements.
//										First three are standard coordinates in three dimension
//										space. The fourth element is a quality level from [0,1]
//										range. In this case contains data about right hand
*/
void CKinectHandler::bothHandMoveHandling( const Vector4 & leftHandData, const Vector4 & rightHandData )
{
	if ( abs( leftHandData.x - rightHandData.x ) < m_dXDelta )
	{
		if ( abs( leftHandData.y - rightHandData.y ) < m_dYDelta )
		{
			if ( m_tSplitedHandsTimestamp.msecsTo( QTime::currentTime() ) > m_iPrayTime )
			{
				changeSlidingState();
				m_tSplitedHandsTimestamp = QTime::currentTime();
			}
		}
	}
	else
	{
		m_tSplitedHandsTimestamp = QTime::currentTime();
	}

	leftHandMoveHandling( leftHandData );
	rightHandMoveHandling( rightHandData );
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			It changes sliding status to its reverse. If status is already true
//			then it will be changed to false and in reverse.
//
*/
void CKinectHandler::changeSlidingState()
{
	m_bIsSlidingOn = ! m_bIsSlidingOn;

	qDebug() << "Sliding state: " << m_bIsSlidingOn;

	m_kinectDetails.insert( "sliding", m_bIsSlidingOn );

	m_bKinectDetailsUpdated = true;
}

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Kinect details getter.
//
//\par		Return value:
//			Some informations about current kinect status. For example whether
//			it is connected or whether skeleton is tracking.
*/
CKinectDetails CKinectHandler::getKinectDetails() const
{
	return m_kinectDetails;
}
