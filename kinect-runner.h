#ifndef KINECTRUNNER_H
#define KINECTRUNNER_H

#include <QThread>
#include "kinect-handler.h"

class KinectRunner : public QThread
{

    Q_OBJECT

private:
    KinectHandler kinectHandler;

    void run();
};

#endif // KINECTRUNNER_H
