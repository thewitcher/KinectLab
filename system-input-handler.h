#ifndef SYSTEMINPUTHANDLER_H
#define SYSTEMINPUTHANDLER_H

/**
  * @file  kinect-handler.h
  * @brief Simple class for overlaying WinAPI functions. It simulates key pressing by sending input signals to system.
  *
  * @author Marcin Haber
  * @date 06/05/2013
  */

#include <Windows.h>

class SystemInputHandler
{
public:
    enum KEYS { LEFT_ARROW, RIGHT_ARROW };

    static void sendPressInput( KEYS key );
};

#endif // SYSTEMINPUTHANDLER_H
