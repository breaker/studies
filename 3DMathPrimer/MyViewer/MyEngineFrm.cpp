/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// WinMain.cpp - Windows common stuff
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MyEngineFrm.h"

////////////////////////////////////////////////////////////////////////////////
// Namespace
////////////////////////////////////////////////////////////////////////////////

using namespace MyEngine;

/////////////////////////////////////////////////////////////////////////////
//
// global variables
//
/////////////////////////////////////////////////////////////////////////////

HWND gHwnd = NULL;
HINSTANCE gHInstApp;
BOOL gIdleInBackground = TRUE;
BOOL gQuitFlag = FALSE;

/////////////////////////////////////////////////////////////////////////////
//
// local stuff
//
/////////////////////////////////////////////////////////////////////////////

// Is our app in the foreground?

static BOOL AppInForeground = TRUE;

// Change the current directory to the one containing the executable.
// This simplifies a lot of stuff

static void SwitchToDirectory()
{
    // Get the directory of the command line filename and module

    char programDir[256];
    GetModuleFileName(gHInstApp,programDir,256);

    // Strip off the program name from programDir

    size_t i = strlen(programDir);
    while (i > 0) {
        if (programDir[i] == '\\') {
            programDir[i] = 0;
            break;
        }
        i--;
    }

    // Switch to the program directory
    SetCurrentDirectory(programDir);
}

/////////////////////////////////////////////////////////////////////////////
//
// Externals
//
/////////////////////////////////////////////////////////////////////////////

// mainProgram(LPCSTR cmdLine) - this is the function that your app defines

void MainProgram(LPCSTR cmdLine);

/////////////////////////////////////////////////////////////////////////////
//
// Global code
//
/////////////////////////////////////////////////////////////////////////////

// WindowProc - The windows message routine

LRESULT CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    // Make sure this message is for our app
    if (hWnd != gHwnd)
        return DefWindowProc(hWnd,message,wParam,lParam);

    KeyEvent event;
    int keyCode;

    // Process the message
    switch(message) {
    case WM_ACTIVATEAPP:
        AppInForeground = (BOOL) wParam;
        if (AppInForeground)
            gRenderer.videoRestore();
        else
            gRenderer.videoSave();
        break;

    case WM_CLOSE:
    case WM_DESTROY:
        gQuitFlag = TRUE;
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        // Extract keyboard scan code

        keyCode = (int) ((lParam >> 16) & 511);

        // Press the key

        gKeyboard.press(keyCode);

        // Queue an event

        event.type = eKeyEventPressed;
        event.keyCode = keyCode;
        event.modifiers = gKeyboard.getModifiers();
        gKeyboard.queueEvent(event);

        // Disable default windows message processing for
        // a few keys.  We don't want F10 to activate the
        // system menu, or alt-F4 to terminate the app.

        if (keyCode == kKeyF4 ||
                keyCode == kKeyF10 ||
                keyCode == kKeyLeftAlt ||
                keyCode == kKeyRightAlt)
            return 0;

        break;

    case WM_CHAR:
        // Queue an event

        event.type = eKeyEventChar;
        event.asciiCode = (char)wParam;
        if (event.asciiCode != 0) {
            event.modifiers = gKeyboard.getModifiers();
            gKeyboard.queueEvent(event);
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        // Extract keyboard scan code

        keyCode = (int) (lParam >> 16) & 511;

        // Release the key

        gKeyboard.debounce(keyCode);

        // Queue an event

        event.type = eKeyEventReleased;
        event.keyCode = keyCode;
        event.modifiers = gKeyboard.getModifiers();
        gKeyboard.queueEvent(event);
        break;
    }

    // Return the default window procedure
    return DefWindowProc(hWnd,message,wParam,lParam);
}

// idle - Perform frame-time processing tasks
void Idle()
{
    // Check if there is a message for us, repeat if program is not
    // in the foreground
    while (TRUE) {
        // Process all pending Windows messages

        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If we're in the foreground, return to normal message processing

        if (AppInForeground)
            break;

        // If we've been told to quit the app, then bail this loop

        if (gQuitFlag)
            break;

        // We're in the background - always give back some time

        Sleep(20);

        // Check if we're supposed to idle in the background

        if (!gIdleInBackground)
            break;

        // Repeat until program is in the foreground,
        // or we signal program termination
    }
}

// createAppWindow - Create a new window for the application
void CreateAppWindow(const char* title, BOOL windowed, int width, int height, HMENU hMenu)
{
    // Register the window class

    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = gHInstApp;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = title;
    RegisterClass(&wc);

    // Create our window

    DWORD style = windowed ? WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX) : WS_POPUP | WS_CLIPCHILDREN;
    DWORD exStyle = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME;
    gHwnd = CreateWindowEx(exStyle, title, title, style, CW_USEDEFAULT, 0, width, height, NULL, hMenu, gHInstApp, NULL);

    if (windowed)
        AdjustWindowed(gHwnd, style, exStyle, width, height, hMenu);
}

// Adjust window size in windowed mode
void AdjustWindowed(HWND hwnd, DWORD style, DWORD exStyle, int width, int height, HMENU hMenu)
{
    ShowWindow(hwnd, SW_HIDE);

    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    SetMenu(hwnd, hMenu);

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;

    AdjustWindowRectEx(&rect, style, hMenu != NULL, exStyle);
    long nw = rect.right - rect.left;
    long nh = rect.bottom - rect.top;

    long pw, ph;
    HWND hParent = GetParent(hwnd);
    if (hParent == NULL) {
        pw = GetSystemMetrics(SM_CXSCREEN);
        ph = GetSystemMetrics(SM_CYSCREEN);
    }
    else {
        RECT prect;
        GetWindowRect(hParent, &prect);
        pw = prect.right - prect.left;
        ph = prect.bottom - prect.top;
    }

    // 如果 rePaint = FALSE, 则从全屏变为窗口时, 一部分画面残留到屏幕上
    MoveWindow(hwnd, (pw - nw) / 2, (ph - nh) / 2, nw, nh, TRUE);
    ShowWindow(hwnd, SW_SHOW);
}

// destroyAppWindow - Destroy the application window

void DestroyAppWindow()
{
    // Make sure we exist

    if (gHwnd != NULL) {
        // Destroy it

        DestroyWindow(gHwnd);
        gHwnd = NULL;
    }
}

// WinMain - Windows entry point

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Save off our instance
    gHInstApp = hInstance;

    // Switch directory to the one containing the executable
    SwitchToDirectory();

    SetIdleFunc(Idle);

    // Run the app initialization
    MainProgram(lpCmdLine);

    // Done

    return 0;
}
