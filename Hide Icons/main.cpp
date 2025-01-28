#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <sstream>
#include <Commctrl.h>
#include "resource.h"

struct HotkeyConfig {
    UINT hotkey;          // Virtual key code for the hotkey (e.g., VK_F1, VK_A, etc.)
    UINT modifier;        // Modifier key (e.g., MOD_SHIFT, MOD_CONTROL, MOD_ALT)
};

// Global variables
HINSTANCE g_hInstance;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
bool g_isStartup = false;
HotkeyConfig g_hotkey = { 0, 0 }; // Default: no hotkey, no modifier
std::wstring customIconPath;

const wchar_t* APP_NAME = L"Hide Icons";
const wchar_t* APP_VERSION = L"v1.4";

// Registry keys
const wchar_t* REG_PATH = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
const wchar_t* SETTINGS_REG_PATH = L"SOFTWARE\\Hide Icons";
const wchar_t* ICON_VALUE_NAME = L"CustomIcon";
const wchar_t* DESKTOP_ICON_STATE = L"DesktopIconsState";
const wchar_t* HOTKEY_VALUE_NAME = L"Hotkey";
const wchar_t* HOTKEY_MODIFIER_VALUE_NAME = L"HotkeyModifier";

HICON hBlackIcon = (HICON)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
HICON hWhiteIcon = (HICON)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON3), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

// Define WM_TASKBARCREATED
UINT WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");

// Function to save hotkey to the registry
void SaveHotkey(const HotkeyConfig& config) {
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, SETTINGS_REG_PATH, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, HOTKEY_VALUE_NAME, 0, REG_DWORD, (const BYTE*)&config.hotkey, sizeof(config.hotkey));
        RegSetValueEx(hKey, HOTKEY_MODIFIER_VALUE_NAME, 0, REG_DWORD, (const BYTE*)&config.modifier, sizeof(config.modifier));
        RegCloseKey(hKey);
    }
}

// Function to load the hotkey from the registry
HotkeyConfig LoadHotkey() {
    HKEY hKey;
    HotkeyConfig config = { 0, 0 };  // Default: no hotkey, no modifier
    if (RegOpenKeyEx(HKEY_CURRENT_USER, SETTINGS_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwType = 0;
        DWORD dwSize = sizeof(config.hotkey);
        if (RegQueryValueEx(hKey, HOTKEY_VALUE_NAME, 0, &dwType, (LPBYTE)&config.hotkey, &dwSize) != ERROR_SUCCESS) {
            config.hotkey = 0;  // Default to no hotkey
        }

        dwSize = sizeof(config.modifier);
        if (RegQueryValueEx(hKey, HOTKEY_MODIFIER_VALUE_NAME, 0, &dwType, (LPBYTE)&config.modifier, &dwSize) != ERROR_SUCCESS) {
            config.modifier = 0;  // Default to no modifier
        }

        RegCloseKey(hKey);
    }
    return config;
}

// Function to register the global hotkey
void RegisterHotkey(HWND hWnd, HotkeyConfig hotkey) {
    if (hotkey.hotkey != 0) {
        // Unregister any existing hotkey
        UnregisterHotKey(hWnd, 1);

        // Register the new hotkey with the given key and modifier
        if (!RegisterHotKey(hWnd, 1, hotkey.modifier, hotkey.hotkey)) {
            MessageBox(hWnd, L"Failed to register hotkey.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

LRESULT CALLBACK HotkeyDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        // Load current hotkey into the control
        SendDlgItemMessage(hWnd, IDC_HOTKEY_EDIT, HKM_SETHOTKEY,
            MAKEWORD(g_hotkey.hotkey, g_hotkey.modifier), 0);

        // --- Position dialog near the cursor ---
        POINT cursorPos;
        GetCursorPos(&cursorPos);  // Get current mouse position

        // Adjust position to avoid covering the cursor
        cursorPos.x += 20;
        cursorPos.y += 20;

        // Ensure dialog stays within screen bounds
        RECT screenRect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0); // Get screen area (excluding taskbar)
        RECT dialogRect;
        GetWindowRect(hWnd, &dialogRect);
        int dialogWidth = dialogRect.right - dialogRect.left;
        int dialogHeight = dialogRect.bottom - dialogRect.top;

        // Check right edge
        if (cursorPos.x + dialogWidth > screenRect.right)
            cursorPos.x = screenRect.right - dialogWidth - 10; // 10px padding

        // Check bottom edge
        if (cursorPos.y + dialogHeight > screenRect.bottom)
            cursorPos.y = screenRect.bottom - dialogHeight - 10;

        // Move the dialog
        SetWindowPos(
            hWnd,
            HWND_TOP,
            cursorPos.x,
            cursorPos.y,
            0, 0,  // Keep current size
            SWP_NOZORDER | SWP_NOSIZE
        );

        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            // Retrieve the hotkey from the control
            WORD hotkey = (WORD)SendDlgItemMessage(hWnd, IDC_HOTKEY_EDIT, HKM_GETHOTKEY, 0, 0);
            UINT vk = LOBYTE(hotkey);
            UINT mod = HIBYTE(hotkey);

            // Convert modifier flags
            UINT modifier = 0;
            if (mod & HOTKEYF_CONTROL) modifier |= MOD_CONTROL;
            if (mod & HOTKEYF_SHIFT) modifier |= MOD_SHIFT;
            if (mod & HOTKEYF_ALT) modifier |= MOD_ALT;

            if (vk != 0) {
                g_hotkey.hotkey = vk;
                g_hotkey.modifier = modifier;
            }
            else {
                g_hotkey.hotkey = 0;
                g_hotkey.modifier = 0;
            }
            EndDialog(hWnd, IDOK);
            break;
        }

        case IDCANCEL:
            EndDialog(hWnd, IDCANCEL);
            break;

        case IDC_CLEAR_BUTTON:
            // Clear the hotkey control
            SendDlgItemMessage(hWnd, IDC_HOTKEY_EDIT, HKM_SETHOTKEY, 0, 0);
            break;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

// Function to set and save a global hotkey
void SetHotkey(HWND hWnd) {
    if (!FindWindow(nullptr, L"Hotkey Option")) {
        if (DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_HOTKEY_DIALOG), hWnd, HotkeyDialogProc) == IDOK) {
            if (g_hotkey.hotkey != 0) {
                SaveHotkey(g_hotkey);
                RegisterHotkey(hWnd, g_hotkey);
            }
            else {
                UnregisterHotKey(hWnd, 1);
            }
        }
    }
}

// Function to remove hotkey
void RemoveHotkey(HWND hWnd) {
    g_hotkey.hotkey = 0;
    g_hotkey.modifier = 0;

    SaveHotkey(g_hotkey); // save settings
    UnregisterHotKey(hWnd, 1);
}

// Function to get the current Windows theme (light or dark)
bool IsTaskbarDarkMode() {
    HKEY hKey;
    DWORD value = 0, size = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueEx(hKey, L"SystemUsesLightTheme", nullptr, nullptr, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0; // 0 means dark mode
        }
        RegCloseKey(hKey);
    }
    return false; // Default to dark mode if key not found
}

// Function to check if the app is set to run at startup
bool IsStartupEnabled() {
    HKEY key;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_READ, &key) == ERROR_SUCCESS) {
        wchar_t value[MAX_PATH];
        DWORD bufferSize = sizeof(value);
        if (RegQueryValueEx(key, APP_NAME, nullptr, nullptr, (LPBYTE)value, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(key);
            return true;
        }
        RegCloseKey(key);
    }
    return false;
}

// Function to toggle startup status
void ToggleStartup() {
    HKEY key;
    if (g_isStartup) {
        if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
            RegDeleteValue(key, APP_NAME);
            RegCloseKey(key);
        }
    }
    else {
        wchar_t exePath[MAX_PATH];
        GetModuleFileName(nullptr, exePath, MAX_PATH);
        if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
            RegSetValueEx(key, APP_NAME, 0, REG_SZ, (const BYTE*)exePath, (DWORD)(wcslen(exePath) + 1) * sizeof(wchar_t));
            RegCloseKey(key);
        }
    }
    g_isStartup = !g_isStartup;
}

// Function to save the custom icon path in the registry
void SaveCustomIconPath(const wchar_t* iconPath) {
    HKEY key;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, SETTINGS_REG_PATH, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) == ERROR_SUCCESS) {
        RegSetValueEx(key, ICON_VALUE_NAME, 0, REG_SZ, (const BYTE*)iconPath, (DWORD)(wcslen(iconPath) + 1) * sizeof(wchar_t));
        RegCloseKey(key);
    }
}

// Function to load the custom icon path from the registry
bool LoadCustomIconPath(std::wstring& iconPath) {
    HKEY key;
    wchar_t buffer[MAX_PATH];
    DWORD bufferSize = sizeof(buffer);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, SETTINGS_REG_PATH, 0, KEY_READ, &key) == ERROR_SUCCESS) {
        if (RegQueryValueEx(key, ICON_VALUE_NAME, nullptr, nullptr, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            iconPath = buffer;
            RegCloseKey(key);
            return true;
        }
        RegCloseKey(key);
    }
    return false;
}

// Function to save the desktop icons state in the registry
void SaveDesktopIconsState(BYTE state) {
    HKEY key;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, SETTINGS_REG_PATH, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) == ERROR_SUCCESS) {
        RegSetValueEx(key, DESKTOP_ICON_STATE, 0, REG_BINARY, &state, sizeof(BYTE));
        RegCloseKey(key);
    }
}

// Function to get the desktop icons state from the registry
bool GetDesktopIconsRegistryState() {
    HKEY key;
    BYTE state;
    DWORD stateSize = sizeof(state);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, SETTINGS_REG_PATH, 0, KEY_READ, &key) == ERROR_SUCCESS) {
        if (RegQueryValueEx(key, DESKTOP_ICON_STATE, nullptr, nullptr, &state, &stateSize) == ERROR_SUCCESS) {
            RegCloseKey(key);
            return state != 0;
        }
        RegCloseKey(key);
    }
    return false;
}

// Function to find the handle to the SysListView32 control
HWND GetDesktopListView() {
    HWND progman = FindWindow(L"Progman", nullptr);
    HWND desktopListView = nullptr;

    if (progman) {
        desktopListView = FindWindowEx(progman, nullptr, L"SysListView32", nullptr);
    }

    if (!desktopListView) {
        HWND shellViewWin = nullptr;
        HWND workerW = FindWindowEx(nullptr, nullptr, L"WorkerW", nullptr);
        while (workerW) {
            shellViewWin = FindWindowEx(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
            if (shellViewWin) {
                desktopListView = FindWindowEx(shellViewWin, nullptr, L"SysListView32", nullptr);
                break;
            }
            workerW = FindWindowEx(nullptr, workerW, L"WorkerW", nullptr);
        }
    }

    return desktopListView;
}

// Function to toggle the visibility of desktop icons
void ToggleDesktopIcons() {
    HWND desktopListView = GetDesktopListView();
    if (desktopListView) {
        if (IsWindowVisible(desktopListView)) {
            ShowWindow(desktopListView, SW_HIDE);
            SaveDesktopIconsState(0);
        }
        else {
            ShowWindow(desktopListView, SW_SHOW);
            SaveDesktopIconsState(1);
        }
    }
}

// Function to change the tray icon
void ChangeTrayIcon() {
    wchar_t filePath[MAX_PATH];
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = L"Icon Files\0*.ico\0All Files\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"Select a New Tray Icon";

    filePath[0] = '\0';

    if (GetOpenFileName(&ofn)) {
        HICON hNewIcon = (HICON)LoadImage(nullptr, filePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hNewIcon) {
            g_nid.hIcon = hNewIcon;
            Shell_NotifyIcon(NIM_MODIFY, &g_nid);
            SaveCustomIconPath(filePath);
        }
        else {
            MessageBox(nullptr, L"Failed to load the icon.", L"Error", MB_ICONERROR);
        }
    }
}

// Function to check taskbar theme and update icon color
void UpdateIconColor() {
    LoadCustomIconPath(customIconPath);
    if (!customIconPath.empty()) {
        return; // Skip updating if a custom icon is already set
    }
    g_nid.hIcon = IsTaskbarDarkMode() ? hWhiteIcon : hBlackIcon;
    Shell_NotifyIcon(NIM_MODIFY, &g_nid); // Update the tray icon
}

// Function to reset the tray icon to the default
void ResetTrayIcon() {
    g_nid.hIcon = IsTaskbarDarkMode() ? hWhiteIcon : hBlackIcon;
    Shell_NotifyIcon(NIM_MODIFY, &g_nid);
    SaveCustomIconPath(L""); // Clear the custom icon path
}

// Function to create a system tray icon
void CreateTrayIcon(HWND hWnd) {
    g_nid.cbSize = sizeof(NOTIFYICONDATA);
    g_nid.hWnd = hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_APP + 1;
    g_nid.hIcon = IsTaskbarDarkMode() ? hWhiteIcon : hBlackIcon;
    wcscpy_s(g_nid.szTip, (L"Hide Icons " + std::wstring(APP_VERSION)).c_str());

    // Load custom icon if available
    if (LoadCustomIconPath(customIconPath) && !customIconPath.empty()) {
        HICON hCustomIcon = (HICON)LoadImage(nullptr, customIconPath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (hCustomIcon) {
            g_nid.hIcon = hCustomIcon;
        }
    }

    // Update state from saved data
    HWND desktopListView = GetDesktopListView();
    if (desktopListView) {
        if (GetDesktopIconsRegistryState() != IsWindowVisible(desktopListView)) {
            ToggleDesktopIcons();
        }
    }

    Shell_NotifyIcon(NIM_ADD, &g_nid);
    g_isStartup = IsStartupEnabled();
}

// Function to remove the system tray icon
void RemoveTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &g_nid);
}

// Window procedure to handle messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_TASKBARCREATED) {
        // Taskbar has been recreated, add the tray icon back
        CreateTrayIcon(hWnd);
        return 0;
    }

    switch (message) {
    case WM_APP + 1:
        if (LOWORD(lParam) == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd);

            CheckMenuItem(g_hMenu, 1, MF_BYCOMMAND | (g_isStartup ? MF_CHECKED : MF_UNCHECKED));
            TrackPopupMenu(g_hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
        }
        else if (LOWORD(lParam) == WM_LBUTTONUP) {
            ToggleDesktopIcons();
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1:
            ToggleStartup();
            break;
        case 2:
            ChangeTrayIcon();
            break;
        case 3:
            ResetTrayIcon();
            break;
        case 4:
            SetHotkey(hWnd);
            break;
        case 5:
            RemoveHotkey(hWnd);
            break;
        case 6:
            MessageBox(hWnd, (L"Hide Icons " + std::wstring(APP_VERSION) + L"\nCreated by emp0ry").c_str(), L"About", MB_OK | MB_ICONINFORMATION);
            break;
        case 7:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_SETTINGCHANGE: // Check if the system theme changed
        UpdateIconColor();
        break;

    case WM_HOTKEY:
        if (wParam == 1) {
            ToggleDesktopIcons(); // Execute hotkey action
        }
        break;

    case WM_DESTROY:
        RemoveTrayIcon();
        UnregisterHotKey(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    CreateMutexA(0, FALSE, "Local\\HideIcons_TrayApp");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(NULL, L"Hide Icons is already running!", NULL, MB_ICONERROR | MB_OK);
        return -1;
    }

    g_hInstance = hInstance;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"HideIconsTrayApp";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(0, L"HideIconsTrayApp", L"Hide Icons", 0, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return -1;
    }

    // Load the hotkey from the registry and register it
    g_hotkey = LoadHotkey();
    RegisterHotkey(hWnd, g_hotkey);

    CreateTrayIcon(hWnd);

    g_hMenu = CreatePopupMenu();
    AppendMenu(g_hMenu, MF_STRING, 1, L"Startup");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(g_hMenu, MF_STRING, 2, L"Change Icon");
    AppendMenu(g_hMenu, MF_STRING, 3, L"Reset Icon");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(g_hMenu, MF_STRING, 4, L"Set Hotkey");
    AppendMenu(g_hMenu, MF_STRING, 5, L"Remove Hotkey");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(g_hMenu, MF_STRING, 6, L"About");
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(g_hMenu, MF_STRING, 7, L"Exit");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyMenu(g_hMenu);
    return 0;
}