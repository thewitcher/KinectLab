#include <QDebug>
#include "kinect-handler.h"

KinectHandler::KinectHandler():
    m_bIsOnRightSide( false ),
    m_bIsOnLeftSide( false ),
    m_pSensor( NULL ),
    m_tRightTimestamp( 0, 0 ),
    m_tLeftTimestamp( 0, 0 ),
    m_tSplitedHandsTimestamp( 0, 0 ),
    m_bIsSlidingOn( false )
{
    createFirstConnectedKinectSensor();
}

KinectHandler::~KinectHandler()
{
    delete m_pSensor;
}

bool KinectHandler::createFirstConnectedKinectSensor()
{
    qDebug() << "createFirstConnectedKinectSensor()";

    INuiSensor * pSensor;
    int iSensorCount = 0;

    // HRESULT - just standard long
    HRESULT hr = NuiGetSensorCount( & iSensorCount );
    if( FAILED( hr ) ) // checks if hr is less than 0
    {
        qDebug() << "Cannot get available kinect sensor count";
        return false;
    }

    // checks all sensors and takes first one which can be created
    for( int i = 0 ; i < iSensorCount ; i++ )
    {
        hr = NuiCreateSensorByIndex( i, & pSensor );

        if( FAILED( hr ) )
        {
            qDebug() << "Cannot create sensor with index: " << i << ", check next index";
            continue;
        }

        hr = pSensor->NuiStatus();

        if( hr == S_OK ) // sensor is connected, it can be initialized now
        {
            m_pSensor = pSensor;
            qDebug() << "Sensor with index: " << i << " is connected - breaking loop";

            break;
        }

        // release not ok sensor
        pSensor->Release();
    }

    if( m_pSensor != NULL )
    {
        hr = m_pSensor->NuiInitialize( NUI_INITIALIZE_FLAG_USES_SKELETON ); // in order to use skeleton

        if( SUCCEEDED( hr ) )
        {
            // create event to inform application about new skeleton frame
            m_hNextSkeletonEvent = CreateEventW( NULL, TRUE, FALSE, NULL );

            // open skeleton stream to get skeleton data
            hr = m_pSensor->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
        }
    }

    if( m_pSensor == NULL || FAILED( hr ) )
    {
        qDebug() << "No ready kinect found";

        return false;
    }

    return true;
}

void KinectHandler::skeletonHandler()
{
    if( WAIT_OBJECT_0 == WaitForSingleObject( m_hNextSkeletonEvent, 0 ) )
    {
        NUI_SKELETON_FRAME skeletonFrame = { 0 };

        // get the skeleton frame that is ready
        if ( SUCCEEDED( m_pSensor->NuiSkeletonGetNextFrame( 0, & skeletonFrame ) ) )
        {
            m_pSensor->NuiTransformSmooth( & skeletonFrame, NULL );

            // process the skeleton frame
            //qDebug() << "Skeleton frame ready";

            for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
            {
                NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[ i ].eTrackingState;

                if( NUI_SKELETON_TRACKED == trackingState )
                {
                    /*qDebug() << "Tracked: " << i;

                    qDebug() << "X: " << skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ].x
                             << " "
                             << "Y: " << skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ].y
                             << " "
                             << "Z: " << skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ].z
                             << " "
                             << "W: " << skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ].w*/;

                    bothHandMoveHandling( skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_LEFT ],
                                          skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ] );
                }
            }
        }

        ResetEvent( m_hNextSkeletonEvent );
    }
}

void KinectHandler::rightHandMoveHandling( const Vector4 & skeletonData )
{
    if( m_bIsSlidingOn == true )
    {
        if( skeletonData.x > 0.15 )
        {
            if( m_bIsOnLeftSide == true )
            {
                if( m_tRightTimestamp.msecsTo( QTime::currentTime() ) < 200 )
                {
                    qDebug() << "Swipe right: " << m_tRightTimestamp.msecsTo( QTime::currentTime() );
                    rightArrowPressed();
                }
            }

            m_bIsOnRightSide = true;
            m_bIsOnLeftSide = false;

            m_tLeftTimestamp = QTime::currentTime();
        }
        else if( skeletonData.x < -0.15 )
        {
            if( m_bIsOnRightSide == true )
            {
                if( m_tLeftTimestamp.msecsTo( QTime::currentTime() ) < 200 )
                {
                    qDebug() << "Swipe left: " << m_tLeftTimestamp.msecsTo( QTime::currentTime() );
                    leftArrowPressed();
                }
            }

            m_bIsOnRightSide = false;
            m_bIsOnLeftSide = true;

            m_tRightTimestamp = QTime::currentTime();
        }
    }
}

void KinectHandler::leftHandMoveHandling( const Vector4 & skeletonData )
{
    Q_UNUSED( skeletonData )
}

void KinectHandler::bothHandMoveHandling( const Vector4 & leftHandData, const Vector4 & rightHandData )
{
    if( abs( leftHandData.x - rightHandData.x ) < 0.20 )
    {
        if( abs( leftHandData.y - rightHandData.y ) < 0.50 )
        {
            if( m_tSplitedHandsTimestamp.msecsTo( QTime::currentTime() ) > 5000 )
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

void KinectHandler::leftArrowPressed()
{
    INPUT input[ 2 ];
    ZeroMemory( input, sizeof( input ) );

    input[ 0 ].type = input[ 1 ].type = INPUT_KEYBOARD;
    input[ 0 ].ki.wVk = input[1].ki.wVk = VK_LEFT; // press left key
    input[ 0 ].ki.time = 0;
    input[ 0 ].ki.wScan = 0;
    input[ 0 ].ki.dwExtraInfo = 0;
    input[ 0 ].ki.dwFlags = 0;

    input[ 1 ].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput( 2, input, sizeof( INPUT ) );
}

void KinectHandler::rightArrowPressed()
{
    INPUT input[ 2 ];
    ZeroMemory( input, sizeof( input ) );

    input[ 0 ].type = input[ 1 ].type = INPUT_KEYBOARD;
    input[ 0 ].ki.wVk = input[ 1 ].ki.wVk = VK_RIGHT; // press right key
    input[ 0 ].ki.time = 0;
    input[ 0 ].ki.wScan = 0;
    input[ 0 ].ki.dwExtraInfo = 0;
    input[ 0 ].ki.dwFlags = 0;

    input[ 1 ].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput( 2, input, sizeof( INPUT ) );
}

void KinectHandler::changeSlidingState()
{
    m_bIsSlidingOn = ! m_bIsSlidingOn;

    qDebug() << "Sliding state: " << m_bIsSlidingOn;
}
