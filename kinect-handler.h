#ifndef KINECTHANDLER_H
#define KINECTHANDLER_H

#include <Windows.h>
#include "NuiApi.h"

class KinectHandler
{
public:
    KinectHandler();
    ~KinectHandler();

    void skeletonHandler();

private:
    INuiSensor * m_pSensor;

    HANDLE m_hNextSkeletonEvent;

    const int m_iXScreenDPI;
    const int m_iYScreenDPI;

    bool m_bIsOnRightSide;
    bool m_bIsOnLeftSide;

    bool createFirstConnectedKinectSensor();
    Vector4 & mapToScreen( Vector4 & skeletonData );
    void cursorMoveHandling( const Vector4 & skeletonData );
    void mouseLeftClick();
};

#endif // KINECTHANDLER_H
