#include <windows.h>
#include "clix.h"
#include "settings.h"
#include "resources.h"

////////////////////////////////////
// .data

static ClixCtx LeMegaCtx = 
{
    INVALID_HANDLE_VALUE,
    INVALID_HANDLE_VALUE,
    
    FALSE,
    FALSE,
    FALSE,
        
    {
        0,    // time increment
        0     // timer
    },
    
    {
        0,
        INVALID_HANDLE_VALUE,
        {0,0},
        {0,0}
    },
    
    {
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE,
        0
    },
    
    {
        TRUE,
        FALSE,
        20
    },
    
    {
        {0, 0, 0, 0, (HK_CALLBACK)StartStop},
        {1, 0, 0, 0, (HK_CALLBACK)SetTargetClickPt}
    }
    
};

UCHAR PrintBuf[300];
static UINT clicks = 0;
static UINT extra_clicks = 0;

////////////////////////////////////
// .text

// periodic clicks reset
VOID ResetClicks()
{
    clicks = 0;
}

//
// Routine for setting current click point
//
HRESULT
SetTargetClickPt()
{   
    if (!GetCursorPos(&LeMegaCtx.Target.p))
        MessageBox(0,"GetCursorPos failed","Error",0);
    
    //MessageBox(0,"GetCursorPos failed","Error",0);
    
    LeMegaCtx.Target.hWnd = WindowFromPoint(LeMegaCtx.Target.p);
    LeMegaCtx.Target.ClientP.x = LeMegaCtx.Target.p.x;
    LeMegaCtx.Target.ClientP.y = LeMegaCtx.Target.p.y;
    ScreenToClient(LeMegaCtx.Target.hWnd, &LeMegaCtx.Target.ClientP);
    
    return ERROR_SUCCESS;
}

VOID
StartStop()
{
    LeMegaCtx.bClicking ^= TRUE;
    
    clicks = 0;
                
    if (LeMegaCtx.bClicking)
    {
        LeMegaCtx.System.Timer = SetTimer(LeMegaCtx.ClixDlg.hWnd, LeMegaCtx.System.Timer, 1000, (TIMERPROC)ResetClicks);
        SetEvent(LeMegaCtx.hStartStopClickingEvent);
    }
    else
    {
        KillTimer(LeMegaCtx.ClixDlg.hWnd, LeMegaCtx.System.Timer);
        LeMegaCtx.System.Timer = 0;
        ResetEvent(LeMegaCtx.hStartStopClickingEvent);
    }
}

//
// Actual clicker routine
//
HRESULT
WINAPI
Clicker(PVOID Param)
{
    POINT* p = &LeMegaCtx.Target.ClientP;
    UINT curr_extraclicks = 0;
    
    while (!LeMegaCtx.bExit)
    {
        if (clicks < LeMegaCtx.Messages.MaxClicksPS)
        {
            WaitForSingleObject(LeMegaCtx.hStartStopClickingEvent, INFINITE);
            if (LeMegaCtx.Messages.SendLMBDOWN)
                PostMessage(LeMegaCtx.Target.hWnd, WM_LBUTTONDOWN, MK_LBUTTON, p->x | (p->y << 16));
            
            if (LeMegaCtx.Messages.SendLMBUP)
                PostMessage(LeMegaCtx.Target.hWnd, WM_LBUTTONUP, MK_LBUTTON, p->x | (p->y << 16));
        
             clicks++;
        }
        
        if (curr_extraclicks++ < extra_clicks)
            continue;
        
        curr_extraclicks = 0;
        Sleep(1);
    }
    
    return ERROR_SUCCESS;
}

VOID
Clix_ApplySettings(PCLIX_SETTINGS settings)
{
    LeMegaCtx.Messages.MaxClicksPS = settings->MaxClicksPS;
    extra_clicks = 100 / (LeMegaCtx.System.TimeIncrement / 10000 );
    extra_clicks = settings->MaxClicksPS / extra_clicks;
    
    LeMegaCtx.Messages.SendLMBDOWN = settings->SendLMBDOWN;
    LeMegaCtx.Messages.SendLMBUP   = settings->SendLMBUP;

    /* Set hotkeys */
    if (LeMegaCtx.Hotkeys[0].wParam && !settings->hotkeys[0].wParam)
    {
        UnregisterHotKey(LeMegaCtx.ClixDlg.hWnd, 0);
    }

    if (LeMegaCtx.Hotkeys[1].wParam && !settings->hotkeys[1].wParam)
    {
        UnregisterHotKey(LeMegaCtx.ClixDlg.hWnd, 1);
    }
    
    if (settings->hotkeys[0].wParam)
    {
        RegisterHotKey(LeMegaCtx.ClixDlg.hWnd, 0, 0, settings->hotkeys[0].wParam);
    }
    
    if (settings->hotkeys[1].wParam)
    {
        RegisterHotKey(LeMegaCtx.ClixDlg.hWnd, 1, 0, settings->hotkeys[1].wParam);
    }
    
    LeMegaCtx.Hotkeys[0].lParam = settings->hotkeys[0].lParam;
    LeMegaCtx.Hotkeys[0].wParam = settings->hotkeys[0].wParam;
    LeMegaCtx.Hotkeys[1].lParam = settings->hotkeys[1].lParam;
    LeMegaCtx.Hotkeys[1].wParam = settings->hotkeys[1].wParam;
    
}

INT_PTR
WINAPI
ClixDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    UCHAR   ClassName[MAX_PATH];
    ULONG   size;
    INT_PTR Ret;

    CLIX_SETTINGS Settings;
    PCLIX_HOTKEY  hotkey = NULL;
    HWND          hNext;
    UINT          uFlags;
    
    UCHAR err[100];
    
    switch (Msg)
    {
        /*case WM_INITDIALOG:
            return TRUE;*/
        
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_SET_POINT:
                    LeMegaCtx.bTargetBeingSet = TRUE;
                    SetWindowText(LeMegaCtx.ClixDlg.hText, "Now click target window.");
                    return TRUE;
                break;
                
                case IDC_START_STOP_CLICKING:
                    StartStop();
                    
                    return TRUE;
                break;
                
            }
            
            
        break;
        
        case WM_SYSCOMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_BRING_UP_SETTINGS:
                    ZeroMemory(&Settings, sizeof(Settings));

                    Settings.hotkeys[0].lParam = LeMegaCtx.Hotkeys[0].lParam;
                    Settings.hotkeys[0].wParam = LeMegaCtx.Hotkeys[0].wParam;
                    Settings.hotkeys[1].lParam = LeMegaCtx.Hotkeys[1].lParam;
                    Settings.hotkeys[1].wParam = LeMegaCtx.Hotkeys[1].wParam;
                    
                    Settings.MaxClicksPS = LeMegaCtx.Messages.MaxClicksPS;
                    Settings.SendLMBDOWN = LeMegaCtx.Messages.SendLMBDOWN;
                    Settings.SendLMBUP   = LeMegaCtx.Messages.SendLMBUP;

                    Ret = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CLIX_SETTINGS), hWnd, SettingsDlgProc, (LPARAM)&Settings);

                    if (Ret == -1)
                    {
                        sprintf(err, "Error: 0x%x", GetLastError());
                        MessageBox(0,err,"Error",0);
                    }
                    
                    if (Ret == IDOK)
                        Clix_ApplySettings(&Settings);
                    
                    return TRUE;
                break;

                case IDC_STAY_ON_TOP:
                    LeMegaCtx.ClixDlg.StayOnTop ^= MF_CHECKED;
                    
                    CheckMenuItem(LeMegaCtx.ClixDlg.sysMenu, IDC_STAY_ON_TOP, LeMegaCtx.ClixDlg.StayOnTop);

                    uFlags = SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE;

                    if (LeMegaCtx.ClixDlg.StayOnTop & MF_CHECKED)
                    {
                        hNext   = HWND_TOPMOST;
                    }
                    else
                    {
                        hNext  = HWND_NOTOPMOST;
                    }

                    SetWindowPos(LeMegaCtx.ClixDlg.hWnd, 
                                 hNext, 
                                 0,0,0,0,
                                 uFlags);

                    return TRUE;
                break;
            }
            
        break;
        
        // on losing focus
        case WM_ACTIVATE:
            
            if (LeMegaCtx.bTargetBeingSet && wParam == WA_INACTIVE)
            {
                //LeMegaCtx.Target.hWnd = (HWND)wParam;
                SetTargetClickPt();
                LeMegaCtx.bTargetBeingSet = FALSE;
                
                if ((size = GetClassName(LeMegaCtx.Target.hWnd, ClassName, MAX_PATH)))
                {
                    ClassName[size] = 0;
                    sprintf(PrintBuf, "ClassName: %s\nX:%d, Y:%d", ClassName, LeMegaCtx.Target.p.x, LeMegaCtx.Target.p.y);
                    SetWindowText(LeMegaCtx.ClixDlg.hText, PrintBuf);
                }
                else
                {
                    if (sprintf(PrintBuf, "hWnd: 0x%x\nX:%d, Y:%d", LeMegaCtx.Target.hWnd, LeMegaCtx.Target.p.x, LeMegaCtx.Target.p.y) != -1)
                        SetWindowText(LeMegaCtx.ClixDlg.hText, PrintBuf);
                }
            }
            
            return TRUE;
        break;

	case WM_HOTKEY:
            hotkey = &LeMegaCtx.Hotkeys[wParam];

            if (hotkey->wParam)
                hotkey->callback(NULL);
	return TRUE;
        
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return TRUE;
        case WM_DESTROY:
            PostQuitMessage(0);
            return TRUE;
    }
    
    
    return FALSE;
}

//
// Possible names: *clix, clicktoris, clickstarter
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
    MSG Msg;
    HMENU sysMenu, clixMenu;
    HICON clixIcon;
    HWND  hParent;
    UCHAR WndClass[32];
    ULONG Dummy0, Dummy1;
    
    GetSystemTimeAdjustment(&LeMegaCtx.System.TimeIncrement, &Dummy0, &Dummy1);
    
    // slight init
    LeMegaCtx.ClixDlg.hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CLIX), 0, ClixDlgProc);
    LeMegaCtx.ClixDlg.hButtonStart = GetDlgItem(LeMegaCtx.ClixDlg.hWnd, IDC_START_STOP_CLICKING);
    LeMegaCtx.ClixDlg.hButtonSelect = GetDlgItem(LeMegaCtx.ClixDlg.hWnd, IDC_SET_POINT);
    LeMegaCtx.ClixDlg.hText = GetDlgItem(LeMegaCtx.ClixDlg.hWnd, IDC_TEXT);
    
    clixIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIX_ICON));
    SendMessage(LeMegaCtx.ClixDlg.hWnd, WM_SETICON, ICON_BIG, (LPARAM)clixIcon);
    
    // set menu
    sysMenu  = GetSystemMenu(LeMegaCtx.ClixDlg.hWnd, FALSE);
    AppendMenu(sysMenu, 0, IDC_BRING_UP_SETTINGS, "Settings");
    AppendMenu(sysMenu, 0, IDC_STAY_ON_TOP, "Always on top");
    LeMegaCtx.ClixDlg.sysMenu = sysMenu;
    
    LeMegaCtx.hStartStopClickingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    LeMegaCtx.hClickerThread = CreateThread(NULL,  // secAttr
                                            0,     // stack
                                            Clicker,
                                            NULL,
                                            0,
                                            NULL);
    
    ShowWindow(LeMegaCtx.ClixDlg.hWnd, SW_SHOW);
    UpdateWindow(LeMegaCtx.ClixDlg.hWnd);
    
    while (GetMessage(&Msg, 0, 0, 0) != 0)
    {
        //if (!IsDialogMessage(LeMegaCtx.ClixDlg.hWnd, &Msg))
        //{
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
        //}
    }
    
    LeMegaCtx.bExit = TRUE;
    
    WaitForSingleObject(LeMegaCtx.hClickerThread, 2000);
    
    
    return 0;
}

//
// Pre-main initialization
//
int clix_init()
{
    RegisterKeyButtonClass();
    return 0;
}

#pragma section(".CRT$XIU",long,read)
__declspec(allocate(".CRT$XIU"))
int (*global_initializer)() = clix_init;

