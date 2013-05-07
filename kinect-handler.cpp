#include <QDebug>
#include <QSettings>
#include "kinect-handler.h"
#include "system-input-handler.h"

/**
 * @brief KinectHandler::KinectHandler. Setting all memeber to theirs default values, read configuration from file and try to find first connected to device kinnect.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
KinectHandler::KinectHandler():
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

/**
 * @brief KinectHandler::~KinectHandler. Destructor which kill object responsible for communicating with kinect device.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
KinectHandler::~KinectHandler()
{
    m_pSensor->NuiShutdown();

    qDebug() << "m_pSensor->NuiShutdown()";
}

/**
 * @brief KinectHandler::readSettings. Initializes some parameters from configuration file. It uses bad const_cast, becuase these parameters are const
 *        in order to keep them in unchanged state. If file doesn't exists then parameters are initialized with default parameters.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void KinectHandler::readSettings()
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

/**
 * @brief KinectHandler::createFirstConnectedKinectSensor(). This method tries to find kinect device. There are some checks in order to check whether everything is ok
 *        or not. This function also updating current status about kinect in KinectDetails object.
 * @return bool Returns false when something bas was happened and connection with kinect sensor is not possible. Otherwise kinect is ready to use and function returns true.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
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
        m_kinectDetails.insert( "connection", false );

        return false;
    }

    m_kinectDetails.insert( "connection", true );
    m_kinectDetails.insert( "skeleton_found", false );

    return true;
}

/**
 * @brief KinectHandler::skeletonHandler. Skeleton handler function is responisble for tracking skeleton. It tries to find first skeleton and remember it. If it is out of kinect view then
 *        new skeleton can be tracked. New skeleton can be tracked even when first skeleton is cover by second or by other thing. This is main function in skeleton tracking, so
 *        it calls some sub functions in order to gesture handling.
 * @return bool Returns true if current kinect status was updated. If there is no change it return false.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
bool KinectHandler::skeletonHandler()
{          
    if( WAIT_OBJECT_0 == WaitForSingleObject( m_hNextSkeletonEvent, 0 ) )
    {
        NUI_SKELETON_FRAME skeletonFrame = { 0 };

        // get the skeleton frame that is ready
        if ( SUCCEEDED( m_pSensor->NuiSkeletonGetNextFrame( 0, & skeletonFrame ) ) )
        {
            m_pSensor->NuiTransformSmooth( & skeletonFrame, NULL );

            // process the skeleton frame
            for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
            {
                NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[ i ].eTrackingState;

                if( NUI_SKELETON_TRACKED == trackingState )
                {
                    // is current skeleton index is -1 then try to track another one
                    if( m_iCurrentTrackedSkeleton == -1 )
                    {
                        qDebug() << "New skeleton found: " << i;

                        m_iCurrentTrackedSkeleton = i;

                        m_kinectDetails.insert( "skeleton_found", true );
                        m_bKinectDetailsUpdated = true;
                    }

                    if( m_iCurrentTrackedSkeleton == i ) // track only one skeleton
                    {
                        bothHandMoveHandling( skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_LEFT ],
                                              skeletonFrame.SkeletonData[ i ].SkeletonPositions[ NUI_SKELETON_POSITION_HAND_RIGHT ] );

                        break;
                    }
                }
                else // untracked state
                {
                    if( m_iCurrentTrackedSkeleton == i ) // try to track another one if previous can't be tracked
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

/**
 * @brief KinectHandler::rightHandMoveHandling. Recognizing right hand sliding from the right to left and in reverse. It also calls approriate methods in order to handle
 *        this recognition.
 * @param skeletonData is a 4 dimensions vector with right hand position details. It contains x, y, z coordinates and additional "w" value.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void KinectHandler::rightHandMoveHandling( const Vector4 & skeletonData )
{
    if( m_bIsSlidingOn == true )
    {
        if( skeletonData.x > m_dSlideXRangeOnTheRight )
        {
            if( m_bIsOnLeftSide == true )
            {
                if( m_tRightTimestamp.msecsTo( QTime::currentTime() ) < m_iSlideSpeed )
                {
                    qDebug() << "Swipe right: " << m_tRightTimestamp.msecsTo( QTime::currentTime() );
                    SystemInputHandler::sendPressInput( SystemInputHandler::RIGHT_ARROW );
                }
            }

            m_bIsOnRightSide = true;
            m_bIsOnLeftSide = false;

            m_tLeftTimestamp = QTime::currentTime();
        }
        else if( skeletonData.x < m_dSlideXRangeOnTheLeft )
        {
            if( m_bIsOnRightSide == true )
            {
                if( m_tLeftTimestamp.msecsTo( QTime::currentTime() ) < m_iSlideSpeed )
                {
                    qDebug() << "Swipe left: " << m_tLeftTimestamp.msecsTo( QTime::currentTime() );
                    SystemInputHandler::sendPressInput( SystemInputHandler::LEFT_ARROW );
                }
            }

            m_bIsOnRightSide = false;
            m_bIsOnLeftSide = true;

            m_tRightTimestamp = QTime::currentTime();
        }
    }
}

/**
 * @brief KinectHandler::leftHandMoveHandling. It can be used to handling left hand. It is not necessary now.
 * @param skeletonData is a 4 dimensions vector with left hand position details. It contains x, y, z coordinates and additional "w" value.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void KinectHandler::leftHandMoveHandling( const Vector4 & skeletonData )
{
    Q_UNUSED( skeletonData )
}

/**
 * @brief KinectHandler::bothHandMoveHandling. Some gesture need two hands data. This method recognizes two hands gestures and after that it send handling to left and right
 *        hand function handling. Now it implements somethign similar to pray gesture. It is used for switching on/off sliding. Pray gesture is just mixing hands for configured
 *        time.
 * @param leftHandData is a 4 dimensions vector with left hand position details. It contains x, y, z coordinates and additional "w" value.
 * @param rightHandData is a 4 dimensions vector with right hand position details. It contains x, y, z coordinates and additional "w" value.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void KinectHandler::bothHandMoveHandling( const Vector4 & leftHandData, const Vector4 & rightHandData )
{
    if( abs( leftHandData.x - rightHandData.x ) < m_dXDelta )
    {
        if( abs( leftHandData.y - rightHandData.y ) < m_dYDelta )
        {
            if( m_tSplitedHandsTimestamp.msecsTo( QTime::currentTime() ) > m_iPrayTime )
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

/**
 * @brief KinectHandler::changeSlidingState. It changes sliding status to its reverse. If status is already true then it will be changed to false and in reverse.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void KinectHandler::changeSlidingState()
{
    m_bIsSlidingOn = ! m_bIsSlidingOn;

    qDebug() << "Sliding state: " << m_bIsSlidingOn;

    m_kinectDetails.insert( "sliding", m_bIsSlidingOn );

    m_bKinectDetailsUpdated = true;
}

/**
 * @brief KinectHandler::getKinectDetails. Kinect details getter.
 * @return KinectDetails Some informations about current kinect status.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
KinectDetails KinectHandler::getKinectDetails() const
{
    return m_kinectDetails;
}
