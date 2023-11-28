// Input.h
//

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>
#include <SGL/Main.h>

SGL_NS_BEGIN

class Main;

////////////////////////////////////////////////////////////////////////////////
// 输入处理辅助功能
////////////////////////////////////////////////////////////////////////////////

inline BOOL KEY_DOWN(int vkey) {
    return (::GetAsyncKeyState(vkey) & 0x8000);
}

inline BOOL KEY_UP(int vkey) {
    return !(::GetAsyncKeyState(vkey) & 0x8000);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Input
////////////////////////////////////////////////////////////////////////////////

class SGL_API Input : private Uncopyable {
public:
    Input() : m_Main(NULL) {}
    virtual ~Input() {}

    void SetMain(Main* main) {
        _ASSERTE(main != NULL);
        m_Main = main;
    }

    void SetWindow(HWND hwnd) {
        _ASSERTE(hwnd != NULL);
        m_hWnd = hwnd;
    }

    BOOL QueryClose(int vkey);
    virtual void ProcessInput(float timeDelta);

protected:
    Main*   m_Main;
    HWND    m_hWnd;
};

SGL_NS_END
