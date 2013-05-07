#include "system-input-handler.h"

/**
 * @brief SystemInputHandler::sendPressInput. Simulates key pressing. It can be done by using WinAPI SendInput with key down and key up signal.
 *
 * @param key Key which pressing will be simulated by method.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void SystemInputHandler::sendPressInput( KEYS key )
{
    INPUT input[ 2 ];
    ZeroMemory( input, sizeof( input ) );

    input[ 0 ].type = input[ 1 ].type = INPUT_KEYBOARD;

    switch( key )
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
