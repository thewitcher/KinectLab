#include <QDebug>
#include "kinect-handler.h"

KinectHandler::KinectHandler():
    m_iXScreenDPI( 1920 ),
    m_iYScreenDPI( 1200 ),
    m_bIsOnRightSide( false ),
    m_bIsOnLeftSide( false )
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

                    cursorMoveHandling( skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ] );
                }
            }
        }

        ResetEvent( m_hNextSkeletonEvent );
    }
}

Vector4 & KinectHandler::mapToScreen( Vector4 & skeletonData )
{
    skeletonData.x = ( ( skeletonData.x + 1 ) / 2 ) * 1920;
    skeletonData.y = 1200 - ( ( skeletonData.y + 1 ) / 2 ) * 1200;

    return skeletonData;
}

void KinectHandler::cursorMoveHandling( const Vector4 & skeletonData )
{
    if( skeletonData.x > 0.15 )
    {
        if( m_bIsOnLeftSide == true )
        {
            qDebug() << "Swipe right";
        }

        m_bIsOnRightSide = true;
        m_bIsOnLeftSide = false;
    }
    else if( skeletonData.x < -0.15 )
    {
        if( m_bIsOnRightSide == true )
        {
            qDebug() << "Swipe left";
        }

        m_bIsOnRightSide = false;
        m_bIsOnLeftSide = true;

        mouseLeftClick();
    }
}

void KinectHandler::mouseLeftClick()
{
//    INPUT press;
//    ZeroMemory( & press, sizeof( INPUT ) ); // screen goes black if it does not exist

//    INPUT release;
//    ZeroMemory( & release, sizeof( INPUT ) ); // screen goes black if it does not exist

////    press.type = INPUT_MOUSE;
////    press.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

////    release.type = INPUT_MOUSE;
////    release.mi.dwFlags = MOUSEEVENTF_LEFTUP;

//    press.type = INPUT_KEYBOARD;
//    press.ki.dwFlags = WM_KEYDOWN;
//    press.ki.wVk = VK_LEFT;

//    release.type = INPUT_KEYBOARD;
//    release.ki.dwFlags = WM_KEYUP;
//    release.ki.wVk = VK_LEFT;

//    SendInput( 1, & press, sizeof( INPUT ) );
//    SendInput( 1, & release, sizeof( INPUT ) );

    INPUT input[2];
    ZeroMemory(input, sizeof(input));

    input[0].type = input[1].type = INPUT_KEYBOARD;
    input[0].ki.wVk = input[1].ki.wVk = 0x41;
    input[0].ki.time = 4;

    input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));
}
