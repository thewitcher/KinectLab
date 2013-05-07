#ifndef KINECTRUNNER_H
#define KINECTRUNNER_H

/**
  * @file  kinect-runner.h
  * @brief KinnectRunner is a class responsible for running kinect-handler in different thread that GUI. This is also a bridge between KinectHandler object
  *        and GUI. They can communicate using signals with kinect details. GUI can use KinectDetails object to show some current information about kinect
  *        on the screen.
  *
  * @author Marcin Haber
  * @date 06/05/2013
  */

#include <QThread>
#include "kinect-details.h"

class KinectRunner : public QThread
{

    Q_OBJECT

private:
    void run();

signals:
    void newKinectDetailsSignal( KinectDetails );
};

#endif // KINECTRUNNER_H
