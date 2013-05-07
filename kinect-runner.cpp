#include "kinect-runner.h"
#include "kinect-handler.h"

/**
 * @brief KinectRunner::run. This is reimplemented virtual function from QThread. Its main task is to run reading from kinect sensor in infinite loop
 *        and emit signal when kinect status is changed. Thanks to this overload kinect can work in separated thread while GUI is always ready for
 *        user actions.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void KinectRunner::run()
{
    KinectHandler kinectHandler;

    emit newKinectDetailsSignal( kinectHandler.getKinectDetails() );

    while( 1 )
    {
        if( kinectHandler.skeletonHandler() == true )
        {
            emit newKinectDetailsSignal( kinectHandler.getKinectDetails() );
        }
    }
}
