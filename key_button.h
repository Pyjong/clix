#ifndef _KEY_BUTTON_H
#define _KEY_BUTTON_H

#include <windows.h>

#define KEY_BUTTON_CLASSNAME TEXT("KeyButton")

typedef ULONG KB_STATE;

#define KBS_NOTHING ((KB_STATE) 0)
#define KBS_SETTING ((KB_STATE) 1)

typedef struct _KEY_BUTTON
{
    KB_STATE state;
    ULONG    lParam;
    ULONG    wParam;
    
}KEY_BUTTON, *PKEY_BUTTON;

typedef ULONG KEY_TYPE;

#define KEY_LPARAM ((KEY_TYPE) 1)
#define KEY_WPARAM ((KEY_TYPE) 2)

ULONG GetKeyButtonKey(HWND hWnd, KEY_TYPE type);
VOID SetKeyButtonKey(HWND hWnd, LPARAM lparam, WPARAM wparam);
VOID RegisterKeyButtonClass();

#endif
