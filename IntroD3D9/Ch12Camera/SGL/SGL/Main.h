// Main.h
//

#ifndef __SGL_MAIN_H__
#define __SGL_MAIN_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>
#include <SGL/Input.h>
#include <SGL/Render.h>

SGL_NS_BEGIN

class Input;

////////////////////////////////////////////////////////////////////////////////
// 游戏控制辅助功能
////////////////////////////////////////////////////////////////////////////////

#ifdef _MFC_VER
// 符合 Main::IdleFunc 的 MFC 例程
inline void MFCIdle() {
    LONG cnt = 0;
    while (AfxGetApp()->OnIdle(cnt++));
}

// 符合 Main::PreTransMessageFunc 的 MFC 例程
inline BOOL MFCPreTransMessage(__out MSG* msg) {
    return AfxGetApp()->PreTranslateMessage(msg);
}
#endif  // _MFC_VER

////////////////////////////////////////////////////////////////////////////////
// 游戏控制类 Main
////////////////////////////////////////////////////////////////////////////////

class SGL_API Main : private Uncopyable {
public:
    Main() : m_State(STATE_INIT), m_Render(NULL), m_Input(NULL), m_PumpMessageFunc(NULL), m_ExitCode(0), m_Flag(0), m_hWnd(NULL) {}
    virtual ~Main();

    BOOL Init(Render* render, Input* input = NULL, DWORD flag = 0);

public:
    virtual void Run(float timeDelta);

    Render* GetRender() {
        return m_Render;
    }

    void AdjustWindowed(DWORD style, DWORD exStyle, HMENU hMenu);
    void AdjustFullscreen();

    void OnWindowActive(UINT state, BOOL minimized);

public:
    enum STATE {
        STATE_INIT,
        STATE_RUN,
        STATE_PAUSE,
        STATE_EXIT
    };

    void SetState(STATE state) {
        m_State = state;
    }

    typedef void (*IdleFunc)();
    typedef BOOL (*PreTransMessageFunc)(__out MSG*);
    typedef BOOL (*PumpMessageFunc)(__out int*, IdleFunc, PreTransMessageFunc);

    enum PUMPMSG_TYPE {
        PUMPMSG_GET,
        PUMPMSG_PEEK,
        PUMPMSG_NUM
    };

    enum MAIN_FLAG {
        MAIN_AUTODEL_RENDER = SGL_MASK_VAL(0),
        MAIN_AUTODEL_INPUT  = SGL_MASK_VAL(1)
    };

    void SetPumpMessageType(PUMPMSG_TYPE type);
    int StartLoop(IdleFunc idleFunc, PreTransMessageFunc preTransFunc, PUMPMSG_TYPE type = PUMPMSG_PEEK);

protected:
    static BOOL PeekMessage(__out int* exitCode, IdleFunc idleFunc, PreTransMessageFunc preTransFunc);
    static BOOL GetMessage(__out int* exitCode, IdleFunc idleFunc, PreTransMessageFunc preTransFunc);

    static void DefIdle() {}
    static BOOL DefPreTransMessage(__out MSG*) {
        return FALSE;
    }

protected:
    static PumpMessageFunc  PumpMessageFuncTable[PUMPMSG_NUM];

    PumpMessageFunc m_PumpMessageFunc;
    Render*         m_Render;
    Input*          m_Input;
    int             m_ExitCode;
    STATE           m_State;
    DWORD           m_Flag;
    HWND            m_hWnd;
};

SGL_NS_END

#endif  // __SGL_MAIN_H__
