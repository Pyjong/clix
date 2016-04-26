#include "clix.h"
#include "settings.h"
#include "resources.h"
#include "key_button.h"

static PCLIX_SETTINGS pSettings;

static struct
{
    HWND    hWnd;
    UINT    uSettingHK;
    HWND    hSSHKButton;
    HWND    hTargetHKButton;
    HWND    hEditMaxClicks;
    HWND    hSendLMBDOWN;
    HWND    hSendLMBUP;

}SettingsCtx;

VOID
StartStopCallback(PVOID param)
{
}

VOID 
SetTargetWndCallback(PVOID param)
{
}

typedef VOID (*SETTING_CB)(PVOID p);

static
SETTING_CB pFnArray[] = {StartStopCallback, SetTargetWndCallback};

INT_PTR
CALLBACK
SettingsDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    UCHAR Buffer[260];

    switch (Msg)
    {
        case WM_INITDIALOG:
            pSettings = (PCLIX_SETTINGS)lParam;

            SettingsCtx.hWnd            = hWnd;
            SettingsCtx.hSSHKButton     = GetDlgItem(hWnd, IDC_BUTTON_SET_HK_SS);
            SettingsCtx.hTargetHKButton = GetDlgItem(hWnd, IDC_BUTTON_SET_HK_TARGET);
            SettingsCtx.hEditMaxClicks  = GetDlgItem(hWnd, IDC_EDIT_MAX_CLICKS);
            SettingsCtx.hSendLMBDOWN    = GetDlgItem(hWnd, IDC_CHECK_LMBDOWN);
            SettingsCtx.hSendLMBUP      = GetDlgItem(hWnd, IDC_CHECK_LMBUP);
            SettingsCtx.uSettingHK  = 0;
            
            SetKeyButtonKey(SettingsCtx.hSSHKButton, pSettings->hotkeys[0].lParam, pSettings->hotkeys[0].wParam);
            SetKeyButtonKey(SettingsCtx.hTargetHKButton, pSettings->hotkeys[1].lParam, pSettings->hotkeys[1].wParam);
             
            sprintf(Buffer, "%d", pSettings->MaxClicksPS);
            SetDlgItemText(SettingsCtx.hWnd, IDC_EDIT_MAX_CLICKS, Buffer);
            
            if (pSettings->SendLMBDOWN)
                PostMessage(SettingsCtx.hSendLMBDOWN, BM_SETCHECK, BST_CHECKED, 0);

            if (pSettings->SendLMBUP)
                PostMessage(SettingsCtx.hSendLMBUP, BM_SETCHECK, BST_CHECKED, 0);
            

            return TRUE;
        break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_BUTTON_UNSET_HK_SS:
                    SettingsCtx.uSettingHK = 0;
                    SetKeyButtonKey(SettingsCtx.hSSHKButton, 0, 0);
                    return TRUE;
                break;
                
                case IDC_BUTTON_UNSET_HK_TARGET:
                    SettingsCtx.uSettingHK = 0;
                    SetKeyButtonKey(SettingsCtx.hTargetHKButton, 0, 0);
                    return TRUE;
                break;

                case IDC_BUTTON_SET_HK_SS:
                    SettingsCtx.uSettingHK = 1;
                    return TRUE;
                break;

                case IDC_BUTTON_SET_HK_TARGET:
                    SettingsCtx.uSettingHK = 2;
                    return TRUE;
                break;
                
                case IDOK:
                
                    if (SendDlgItemMessage(SettingsCtx.hWnd, IDC_CHECK_LMBDOWN, BM_GETCHECK, 0, 0) == BST_CHECKED)
                        pSettings->SendLMBDOWN = TRUE;
                    else
                        pSettings->SendLMBDOWN = FALSE;
            
                    if (SendDlgItemMessage(SettingsCtx.hWnd, IDC_CHECK_LMBUP, BM_GETCHECK, 0, 0) == BST_CHECKED)
                        pSettings->SendLMBUP   = TRUE;
                    else
                        pSettings->SendLMBUP   = FALSE;
                    
                    GetWindowText(SettingsCtx.hEditMaxClicks, Buffer, 20);
                    
                    pSettings->MaxClicksPS = atoi(Buffer);
                    
                    //hotkeys
                    pSettings->hotkeys[0].lParam = GetKeyButtonKey(SettingsCtx.hSSHKButton, KEY_LPARAM);
                    pSettings->hotkeys[0].wParam = GetKeyButtonKey(SettingsCtx.hSSHKButton, KEY_WPARAM);
                    pSettings->hotkeys[1].lParam = GetKeyButtonKey(SettingsCtx.hTargetHKButton, KEY_LPARAM);
                    pSettings->hotkeys[1].wParam = GetKeyButtonKey(SettingsCtx.hTargetHKButton, KEY_WPARAM);
                    
                case IDCANCEL:
                    EndDialog(hWnd, wParam);
                    return TRUE;
                break;

            }
            return FALSE;
        break;
        
        case WM_KEYUP:
        
            if (wParam == VK_ESCAPE)// && SettingsCtx.uSettingHK)
            {
                // set state
                // unset key
                // unset settinghk
                return FALSE;
            }
        
            /*if (SettingsCtx.uSettingHK)
            {
                pSettings->hotkeys[SettingsCtx.uSettingHK].Id       = SettingsCtx.uSettingHK;
                pSettings->hotkeys[SettingsCtx.uSettingHK].KeyCode  = wParam;
                pSettings->hotkeys[SettingsCtx.uSettingHK].Modifier = lParam;
                pSettings->hotkeys[SettingsCtx.uSettingHK].callback = pFnArray[SettingsCtx.uSettingHK];
                SettingsCtx.uSettingHK = 0;
                return TRUE;
            }*/
        break;
            
        return FALSE;
        
    }

    return FALSE;
}

