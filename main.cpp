#include "kinect-handler.h"

int main()
{
    KinectHandler kinectHandler;

    while( 1 )
    {
        kinectHandler.skeletonHandler();
    }

    return 0;
}
