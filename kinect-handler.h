#ifndef KINECTHANDLER_H
#define KINECTHANDLER_H

/**
  * @file  kinect-handler.h
  * @brief This class is responsible for kinect handling:
  *         - finding kinect
  *         - connecting to kinect
  *         - tracking skeleton
  *         - recognizing hands gesture
  *         - collecting some information about current kinect status
  *
  * @author Marcin Haber
  * @date 06/05/2013
  */

#include <QTime>
#include <Windows.h>
#include "NuiApi.h"

#include "kinect-details.h"

class KinectHandler
{
public:
    KinectHandler();
    ~KinectHandler();

    // returns true when new skeleton is on board or when current skeleton was lost
    // generally when this method returns true then something is happen with main skeleton
    bool skeletonHandler();
    KinectDetails getKinectDetails() const;

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
    KinectDetails m_kinectDetails;

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
