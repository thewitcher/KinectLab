#ifndef KINECTDETAILS_H
#define KINECTDETAILS_H

/**
  * @file  kinect-details.h
  * @brief Typedef for QHash with some details about current kinect status. It contains data as:
  *         - sliding on/off
  *         - skeleton tracking on/off
  *         - connection on/off
  *
  * @author Marcin Haber
  * @date 06/05/2013
  */

#include <QHash>

typedef QHash<QString,bool> KinectDetails;
Q_DECLARE_METATYPE(KinectDetails);

#endif // KINECTDETAILS_H
