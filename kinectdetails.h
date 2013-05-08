/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		kinectdetails.h
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
// Description:
/*!\class CKinectDetails
//	Typedef for QHash with some details about current kinect status. It contains data as:
//	- sliding on/off
//	- skeleton tracking on/off
//	- connection on/off
//
*/
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-07	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef KINECTDETAILS_H
#define KINECTDETAILS_H

#include <QHash> // USES: QHash template with typedef

typedef QHash<QString, bool> CKinectDetails;
Q_DECLARE_METATYPE(CKinectDetails);

#endif // KINECTDETAILS_H
