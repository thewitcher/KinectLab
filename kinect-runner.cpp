#include "kinect-runner.h"

void KinectRunner::run()
{
    while( 1 )
    {
        kinectHandler.skeletonHandler();
    }
}
