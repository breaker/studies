// main.cpp
//

#include "pch.h"
#include "gameutils.h"

using std::wstring;

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

const wchar_t GAME_NAME[]   = L"Anti-Virus Game";
const wchar_t DATA_DIR[]    = L"Data";

const int GAME_WIDTH    = 1024;
const int GAME_HEIGHT   = 768;

wstring g_ExeDir;
wstring g_DataDir;

////////////////////////////////////////////////////////////////////////////////
// Setup and Cleanup
////////////////////////////////////////////////////////////////////////////////

void SetupWorkDir()
{
    // 取得主可执行文件 exe 路径
    wchar_t buf[MAX_PATH];
    GetModuleFileName(NULL, buf, _countof(buf));
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        _RPTF1(_CRT_ERROR, "GetModuleFileName(): buffer too small (%Iu bytes)\n", _countof(buf));
        return;
    }

    // 设置 exe 所在目录
    wchar_t* p = wcsrchr(buf, L'\\');
    if (p != NULL)
        *p = 0;
    g_ExeDir = buf;

    wchar_t* cwd = _wgetcwd(NULL, 0);

    // 从 exe 所在目录开始, 向上查找 Data 目录
    wstring updir = L"\\";
    while (true) {
        g_DataDir = g_ExeDir + updir + DATA_DIR;

        p = _wfullpath(buf, g_DataDir.c_str(), _countof(buf));
        if (p == NULL) {
            _RPTF1(_CRT_ERROR, "_wfullpath(): buffer too small (%Iu bytes)\n", _countof(buf));
            return;
        }

        if (_wchdir(buf) == 0) {
            g_DataDir = buf;
            _RPT1(_CRT_WARN, "Data directory found: %ls\n", buf);
            break;
        }
        // 当根目录下也不存在 X:\Data 时
        // 设置 Data 目录为 exe 同目录下的 Data 子目录 [不存在的目录]
        else {
            p = wcschr(buf, L':');
            _ASSERTE(p != NULL && p[1] == L'\\');
            if (wcscmp(p + 2, DATA_DIR) == 0) {
                g_DataDir = g_ExeDir + L"\\" + DATA_DIR;
                _RPT1(_CRT_WARN, "Data directory not found: %ls\n", DATA_DIR);
                break;
            }
        }

        updir += L"..\\";
    }

    _wchdir(cwd);
    free(cwd);
}

void Setup()
{
    setlocale(LC_ALL, "");
    SetupWorkDir();

    gu::g_GameName = GAME_NAME;
    gu::g_DataDir = g_DataDir;

    gu::g_GameWidth = GAME_WIDTH;
    gu::g_GameHeight = GAME_HEIGHT;
}

void Cleanup()
{
    _CrtDumpMemoryLeaks();
}

////////////////////////////////////////////////////////////////////////////////
// Window Procedures
////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg) {
    case WM_DESTROY:
        gu::g_GameOver = TRUE;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

////////////////////////////////////////////////////////////////////////////////
// Entry Point
////////////////////////////////////////////////////////////////////////////////

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR cmdline, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInst);
    UNREFERENCED_PARAMETER(cmdline);

    Setup();

    // initialize window settings
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style        = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc  = (WNDPROC) WndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 0;
    wc.hInstance    = hInst;
    wc.hIcon        = NULL;
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = GAME_NAME;
    wc.hIconSm          = NULL;
    RegisterClassEx(&wc);

    // create window
    HWND hwnd = CreateWindow(GAME_NAME, GAME_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, GAME_WIDTH, GAME_HEIGHT, NULL, NULL, hInst, NULL);
    if (hwnd == NULL)
        return -1;

    // show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // initialize the game
    if (!gu::Game_Init(hwnd)) {
        MessageBox(hwnd, L"Game initialization failed, check resource existence.", L"Initialization failed", MB_ICONERROR | MB_OK);
        return -1;
    }

    // main message loop
    MSG msg;
    while (!gu::g_GameOver) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // process game loop
        gu::Game_Run(hwnd);
    }

    // shutdown
    gu::Game_End();
    Cleanup();
    return (int) msg.wParam;
}
