#ifndef KINECTHANDLER_H
#define KINECTHANDLER_H

#include <QTime>
#include <Windows.h>
#include "NuiApi.h"

class KinectHandler
{
public:
    KinectHandler();
    ~KinectHandler();

    void skeletonHandler();

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

    // methods
    bool createFirstConnectedKinectSensor();

    void rightHandMoveHandling( const Vector4 & skeletonData );
    void leftHandMoveHandling( const Vector4 & skeletonData );
    void bothHandMoveHandling( const Vector4 & leftHandData, const Vector4 & rightHandData );

    void rightArrowPressed();
    void leftArrowPressed();

    void changeSlidingState();
};

#endif // KINECTHANDLER_H
