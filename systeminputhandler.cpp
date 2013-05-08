/////////////////////////////////////////////////////////////////////////////
//
// This file may not be reproduced, disclosed or used in whole or in part
// without the express written permission of PRUFTECHNIK Technology.
// Copyright PRUFTECHNIK Technology, 2013.
//
/////////////////////////////////////////////////////////////////////////////
//
// File:		systeminputhandler.cpp
// Author:		Marcin Haber
// Email:		marcin.haber@pruftechnik.com.pl
// Begin:		2013-05-06
//
/////////////////////////////////////////////////////////////////////////////
// Modification history:
//
//	2013-05-08	Marcin Haber	Formating code to make it compatible with coding standard.
//
/////////////////////////////////////////////////////////////////////////////

#include "systeminputhandler.h" // USES

///////////////////////////////////////////////////////////////////////////
/*!
//\par		Description:
//			Simulates key pressing. It can be done by using WinAPI SendInput with key down and key up signal.
//
//\par		Parameters:
//\arg		[IN]		key	Key which pressing will be simulated by method.
*/
void CSystemInputHandler::sendPressInput( Ekeys key )
{
	INPUT input[ 2 ];
	ZeroMemory( input, sizeof( input ) );

	input[ 0 ].type = input[ 1 ].type = INPUT_KEYBOARD;

	switch ( key )
	{
		case LEFT_ARROW:
			input[ 0 ].ki.wVk = input[ 1 ].ki.wVk = VK_LEFT; // press left key
		break;
		case RIGHT_ARROW:
			input[ 0 ].ki.wVk = input[ 1 ].ki.wVk = VK_RIGHT; // press right key
		break;
	}

	input[ 0 ].ki.time = 0;
	input[ 0 ].ki.wScan = 0;
	input[ 0 ].ki.dwExtraInfo = 0;
	input[ 0 ].ki.dwFlags = 0;

	input[ 1 ].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput( 2, input, sizeof( INPUT ) );
}
