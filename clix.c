#include <windows.h>
#include "clix.h"


typedef struct _ClixCtx
{
    HANDLE hClickerThread;
    HANDLE hStartStopClickingEvent;

    BOOLEAN bTargetBeingSet;
    BOOLEAN bClicking;
    BOOLEAN bExit;
    
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
    }ClixDlg;
    
    struct
    {
        UINT SendLMBDOWN;
        UINT SendLMBUP;
    }Messages;
}ClixCtx;

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
        0,
        INVALID_HANDLE_VALUE,
        {0,0}
    },
    
    {
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE
    },
    
    {
        MF_BYCOMMAND | MF_CHECKED,
        MF_BYCOMMAND | MF_UNCHECKED
    }
};


UCHAR PrintBuf[300];


////////////////////////////////////
// .text

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

//
// Actual clicker routine
//
HRESULT
WINAPI
Clicker(PVOID Param)
{
    POINT* p = &LeMegaCtx.Target.ClientP;
    
    while (!LeMegaCtx.bExit)
    {
        WaitForSingleObject(LeMegaCtx.hStartStopClickingEvent, INFINITE);
        if (LeMegaCtx.Messages.SendLMBDOWN)
            PostMessage(LeMegaCtx.Target.hWnd, WM_LBUTTONDOWN, MK_LBUTTON, p->x | (p->y << 16));
        
        if (LeMegaCtx.Messages.SendLMBUP)
            PostMessage(LeMegaCtx.Target.hWnd, WM_LBUTTONUP, MK_LBUTTON, p->x | (p->y << 16));
        
        Sleep(1);
    }
    
    return ERROR_SUCCESS;
}

INT_PTR
WINAPI
ClixDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    UCHAR ClassName[MAX_PATH];
    ULONG size;
    
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
                    LeMegaCtx.bClicking ^= TRUE;
                
                    if (LeMegaCtx.bClicking)
                        SetEvent(LeMegaCtx.hStartStopClickingEvent);
                    else
                        ResetEvent(LeMegaCtx.hStartStopClickingEvent);
                    
                    return TRUE;
                break;
                
            }
            
            
        break;
        
        case WM_SYSCOMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_LMB_DOWN:
                    LeMegaCtx.Messages.SendLMBDOWN ^= MF_CHECKED;
                
                    CheckMenuItem(LeMegaCtx.ClixDlg.sysMenu, IDC_LMB_DOWN, LeMegaCtx.Messages.SendLMBDOWN);
                    return TRUE;
                break;
                
                case IDC_LMB_UP:
                    LeMegaCtx.Messages.SendLMBUP ^= MF_CHECKED;
                    
                    CheckMenuItem(LeMegaCtx.ClixDlg.sysMenu, IDC_LMB_UP, LeMegaCtx.Messages.SendLMBUP);
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
    
    
    // slight init
    LeMegaCtx.ClixDlg.hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CLIX), 0, ClixDlgProc);
    LeMegaCtx.ClixDlg.hButtonStart = GetDlgItem(LeMegaCtx.ClixDlg.hWnd, IDC_START_STOP_CLICKING);
    LeMegaCtx.ClixDlg.hButtonSelect = GetDlgItem(LeMegaCtx.ClixDlg.hWnd, IDC_SET_POINT);
    LeMegaCtx.ClixDlg.hText = GetDlgItem(LeMegaCtx.ClixDlg.hWnd, IDC_TEXT);
    
    clixIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIX_ICON));
    SendMessage(LeMegaCtx.ClixDlg.hWnd, WM_SETICON, ICON_BIG, (LPARAM)clixIcon);
    
    // set menu
    sysMenu  = GetSystemMenu(LeMegaCtx.ClixDlg.hWnd, FALSE);
    //DeleteMenu(sysMenu, 0, MF_BYPOSITION);
    clixMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MENU));
    InsertMenu(sysMenu, 0, MF_POPUP, (UINT_PTR)clixMenu, "Clix msgs");
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
    
    // our shitty dialog message pump
    while (GetMessage(&Msg, 0, 0, 0) != 0)
    {
        if (!IsDialogMessage(LeMegaCtx.ClixDlg.hWnd, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
    
    LeMegaCtx.bExit = TRUE;
    
    WaitForSingleObject(LeMegaCtx.hClickerThread, 2000);
    
    
    return 0;
}