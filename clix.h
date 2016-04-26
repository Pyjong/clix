#ifndef _CLICKER_RES_H
#define _CLICKER_RES_H

#include <windows.h>


// You get WM_HOTKEY, find wParam in LeMegaCtx.Hotkeys
// and you call the callback
typedef VOID  (*HK_CALLBACK)(PVOID);

typedef struct _CLIX_HOTKEY
{
     UINT  Id;
     UINT  lParam;
     UINT  wParam;
     UINT  Modifier;
     HK_CALLBACK callback;

}CLIX_HOTKEY, *PCLIX_HOTKEY;

/* for eventual text based message setup
typedef struct _CLIX_MSG
{
    UINT  Msg;
    UCHAR Text[0]; // zero terminated

}CLIX_MSG, *PCLIX_MSG;
*/
typedef struct _CLIX_SETTINGS
{
    UINT      MaxClicksPS;
    BOOLEAN   SendLMBDOWN;
    BOOLEAN   SendLMBUP;
    CLIX_HOTKEY hotkeys[2];
}CLIX_SETTINGS, *PCLIX_SETTINGS;

typedef struct _ClixCtx
{
    HANDLE hClickerThread;
    HANDLE hStartStopClickingEvent;

    BOOLEAN bTargetBeingSet;
    BOOLEAN bClicking;
    BOOLEAN bExit;
    
    struct
    {
        ULONG     TimeIncrement;
        UINT_PTR Timer;
    }System;
    
    struct
    {
        UINT   ThreadId;
        HWND   hWnd;
        POINT  p;
        POINT  ClientP;
    }Target;

    struct
    {
        HWND  hWnd;
        HWND  hButtonStart;
        HWND  hButtonSelect;
        HWND  hText;
        HMENU sysMenu;
        UINT  StayOnTop;
    }ClixDlg;
    
    struct
    {
        BOOLEAN SendLMBDOWN;
        BOOLEAN SendLMBUP;
        UINT MaxClicksPS;
        //UINT ClickWhereTheMouseIs;
    }Messages;

    CLIX_HOTKEY Hotkeys[2];

}ClixCtx;

VOID Clix_ApplySettings(PCLIX_SETTINGS settings);
HRESULT SetTargetClickPt();
VOID StartStop();

#endif
