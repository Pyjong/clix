#include "key_button.h"


static
WNDCLASSEX KeyButtonCls;

static WNDPROC ButtonWndProc;
static int ButtonWndExtra = 0;


LRESULT
WINAPI
KeyButtonWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    UCHAR KeyText[50];
    PKEY_BUTTON kb = (PKEY_BUTTON)GetWindowLongPtr(hWnd, ButtonWndExtra);
    
    switch (Msg)
    {   
        case WM_NCCREATE:
            if (!CallWindowProc(ButtonWndProc, hWnd, Msg, wParam, lParam))
                return FALSE;
            
            kb = HeapAlloc(GetProcessHeap(), 0, sizeof(KEY_BUTTON));
            
            if (!kb)
                return FALSE;
            
            SetWindowLongPtr(hWnd, ButtonWndExtra, (LONG_PTR)kb);
        return TRUE;
            
        case WM_NCDESTROY:
            if (kb)
                HeapFree(GetProcessHeap(), 0, kb);
        break;
        
        case WM_LBUTTONUP:
            kb->state = KBS_SETTING;
            kb->lParam = 0;
            kb->wParam = 0;
            SetWindowText(hWnd, "Press key...");
        break;//return TRUE;
        
        case WM_KEYDOWN:
            if (kb->state == KBS_SETTING)
            {
                kb->state = KBS_NOTHING;
                
                if (wParam == VK_ESCAPE)
                {
                    SetWindowText(hWnd, "");
                    kb->lParam = 0;
                    kb->wParam = 0;
                    
                    break;
                }
                kb->lParam = lParam;
                kb->wParam = wParam;
                
                if (GetKeyNameText(lParam, KeyText, 50))
                    SetWindowText(hWnd, KeyText);
                else
                    SetWindowText(hWnd, "");
            }
            
        break;
        
        // on losing focus
        case WM_KILLFOCUS:
            
            if (kb->state == KBS_SETTING)
            {
                SetWindowText(hWnd, "");
                kb->state = KBS_NOTHING;
            }
        break;
        
        case WM_GETDLGCODE:
            return DLGC_WANTALLKEYS; //DLGC_WANTCHARS;
        break;
        
        /*case WM_MOUSELEAVE:
        return TRUE;*/
    }
    
    return CallWindowProc(ButtonWndProc, hWnd, Msg, wParam, lParam);
}

VOID RegisterKeyButtonClass()
{
    UCHAR err[100];
    
    GetClassInfoEx(GetModuleHandle(NULL), "BUTTON", &KeyButtonCls);
    
    ButtonWndProc  = KeyButtonCls.lpfnWndProc;
    ButtonWndExtra = KeyButtonCls.cbWndExtra;
    
    KeyButtonCls.cbSize        = sizeof(KeyButtonCls);
    KeyButtonCls.lpszClassName = KEY_BUTTON_CLASSNAME;
    KeyButtonCls.lpfnWndProc   = KeyButtonWndProc;
    KeyButtonCls.style        &= ~CS_GLOBALCLASS;
    KeyButtonCls.hInstance     = GetModuleHandle(NULL);
    KeyButtonCls.cbWndExtra   += sizeof(PKEY_BUTTON);
    
    if (!RegisterClassEx(&KeyButtonCls))
    {
        sprintf(err, "Error: 0x%x", GetLastError());
        MessageBox(0, err, "Error", 0);
    }
}

ULONG GetKeyButtonKey(HWND hWnd, KEY_TYPE type)
{
    PKEY_BUTTON kb = (PKEY_BUTTON)GetWindowLongPtr(hWnd, ButtonWndExtra);
    
    if (type == KEY_LPARAM)
        return kb->lParam;
    if (type == KEY_WPARAM)
        return kb->wParam;
    
    return 0;
}

VOID SetKeyButtonKey(HWND hWnd, LPARAM lParam, WPARAM wParam)
{
    UCHAR KeyText[50];
    PKEY_BUTTON kb = (PKEY_BUTTON)GetWindowLongPtr(hWnd, ButtonWndExtra);
    
    kb->lParam = lParam;
    kb->wParam = wParam;
    
    if (GetKeyNameText(lParam, KeyText, 50))
        SetWindowText(hWnd, KeyText);
    else
        SetWindowText(hWnd, "");
}
