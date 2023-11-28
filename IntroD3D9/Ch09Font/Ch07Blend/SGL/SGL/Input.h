// Input.h
//

#ifndef __SGL_INPUT_H__
#define __SGL_INPUT_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>
#include <SGL/Main.h>

_SGL_NS_BEGIN

class Main;

////////////////////////////////////////////////////////////////////////////////
// ���봦��������
////////////////////////////////////////////////////////////////////////////////

inline BOOL KEY_DOWN(int vkey) {
    return (::GetAsyncKeyState(vkey) & 0x8000);
}

inline BOOL KEY_UP(int vkey) {
    return !(::GetAsyncKeyState(vkey) & 0x8000);
}

////////////////////////////////////////////////////////////////////////////////
// ���봦���� Input
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

    virtual void ProcessInput(float timeDelta);

protected:
    Main*   m_Main;
    HWND    m_hWnd;
};

_SGL_NS_END

#endif  // __SGL_INPUT_H__
