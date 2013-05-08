/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		systeminputhandler.h
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
// Description:
/*!\class CSystemInputHandler
//	Simple class for overlaying WinAPI functions. It simulates key pressing by sending input signals to system.
//
*/
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-07	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEMINPUTHANDLER_H
#define SYSTEMINPUTHANDLER_H

#define NOMINMAX

#include <Windows.h> // HASA

class CSystemInputHandler
{
public:
	enum Ekeys			//! Supported keyboard keys
	{
		LEFT_ARROW,		//!< Left arrow
		RIGHT_ARROW		//!< Right arrow
	};

	static void sendPressInput( Ekeys key );
};

#endif // SYSTEMINPUTHANDLER_H
