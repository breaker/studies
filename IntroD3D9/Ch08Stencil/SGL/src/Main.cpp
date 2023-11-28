// Main.cpp
//

#include "StdAfx.h"
#include <SGL/Main.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ��Ϸ������ Main
////////////////////////////////////////////////////////////////////////////////

Main::PumpMessageFunc Main::PumpMessageFuncTable[PUMPMSG_NUM] = {
    Main::GetMessage,
    Main::PeekMessage
};

Main::~Main()
{
    m_State = STATE_EXIT;

    if (m_Flag & MAIN_AUTODEL_RENDER)
        delete m_Render;
    if (m_Flag & MAIN_AUTODEL_INPUT)
        delete m_Input;
}

BOOL Main::Init(Render* render, Input* input, DWORD flag)
{
    if (render == NULL)
        return FALSE;
    m_hWnd = render->GetWindow();

    if (input == NULL) {
        input = new Input();
        flag |= MAIN_AUTODEL_INPUT;
    }
    input->SetMain(this);
    input->SetWindow(m_hWnd);

    m_Render = render;
    m_Input = input;
    m_Flag = flag;
    return TRUE;
}

void Main::SetPumpMessageType(PUMPMSG_TYPE type)
{
    m_PumpMessageFunc = PumpMessageFuncTable[type];
}

int Main::StartLoop(IdleFunc idleFunc, PreTransMessageFunc preTransFunc, PUMPMSG_TYPE type)
{
    this->SetPumpMessageType(type);

    if (idleFunc == NULL)
        idleFunc = DefIdle;
    if (preTransFunc == NULL)
        preTransFunc = DefPreTransMessage;

    m_State = STATE_RUN;

    Timer timer;
    while (m_PumpMessageFunc(&m_ExitCode, idleFunc, preTransFunc))
        this->Run(timer.ElapseSecondReset());

    return m_ExitCode;
}

BOOL Main::PeekMessage(__out int* exitCode, IdleFunc idleFunc, PreTransMessageFunc preTransFunc)
{
    _ASSERTE(exitCode != NULL);
    _ASSERTE(idleFunc != NULL);
    _ASSERTE(preTransFunc != NULL);

    MSG msg;
    if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            *exitCode = (int) msg.wParam;
            return FALSE;
        }
        if (!preTransFunc(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    else
        idleFunc();

    return TRUE;
}

BOOL Main::GetMessage(__out int* exitCode, IdleFunc idleFunc, PreTransMessageFunc preTransFunc)
{
    _ASSERTE(exitCode != NULL);
    _ASSERTE(idleFunc != NULL);
    _ASSERTE(preTransFunc != NULL);

    MSG msg;
    DWORD ret = ::GetMessage(&msg, NULL, 0, 0);
    if (ret == 0) {
        *exitCode = (int) msg.wParam;
        return FALSE;
    }
    else if (ret == -1) {
        *exitCode = (int) GetLastError();
        return FALSE;
    }

    // ��������Ϣת��Ϊ WM_COMMAND ��Ϣ (��ݼ���Ϣת��), ��ͬ��������Ŀ�괰��
    if (!preTransFunc(&msg)) {
        ::TranslateMessage(&msg);   // virtual-key ��Ϣת��Ϊ WM_CHAR ��Ϣ
        ::DispatchMessage(&msg);
    }

    if (!::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        idleFunc();

    return TRUE;
}

void Main::Run(float timeDelta)
{
    if (m_State == STATE_EXIT)
        return;

    m_Input->ProcessInput(timeDelta);

    if (m_State == STATE_EXIT || m_State == STATE_PAUSE)
        return;

    m_Render->Display(timeDelta);
}

void Main::AdjustWindowed(DWORD style, DWORD exStyle, HMENU hMenu)
{
    ::ShowWindow(m_hWnd, SW_HIDE);

    ::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);
    ::SetMenu(m_hWnd, hMenu);

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = m_Render->Width();
    rect.bottom = m_Render->Height();

    ::AdjustWindowRectEx(&rect, style, hMenu != NULL, exStyle);
    long nw = rect.right - rect.left;
    long nh = rect.bottom - rect.top;

    long pw, ph;
    HWND hParent = ::GetParent(m_hWnd);
    if (hParent == NULL) {
        pw = ::GetSystemMetrics(SM_CXSCREEN);
        ph = ::GetSystemMetrics(SM_CYSCREEN);
    }
    else {
        RECT prect;
        ::GetWindowRect(hParent, &prect);
        pw = prect.right - prect.left;
        ph = prect.bottom - prect.top;
    }

    ::MoveWindow(m_hWnd, (pw - nw) / 2, (ph - nh) / 2, nw, nh, TRUE);   // ��� rePaint = FALSE, ���ȫ����Ϊ����ʱ, һ���ֻ����������Ļ��
    ::ShowWindow(m_hWnd, SW_SHOW);
}

void Main::AdjustFullscreen()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
    ::SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP);
    ::SetMenu(m_hWnd, NULL);
    ::ShowWindow(m_hWnd, SW_SHOW);
}

void Main::OnWindowActive(UINT state, BOOL minimized)
{
    // NOTE:
    // Ӧͬʱ��� Inactive �� Minimized ״̬, ��Ϊ�� Minimized Active ״̬
    // Minimized Active: ���������������ťʱ, ��������״̬ (��С�� + ����)
    // Minimized Active �Ⱥ�������� WM_ACTIVATE ��Ϣ: 1. WA_INACTIVE 2. WA_ACTIVE + Minimized
    // �������С����ťֻ����һ�� WA_INACTIVE ��Ϣ
    if (state == WA_INACTIVE || minimized) {
        this->SetState(STATE_PAUSE);
        this->SetPumpMessageType(PUMPMSG_GET);
    }
    else {
        this->SetState(STATE_RUN);
        this->SetPumpMessageType(PUMPMSG_PEEK);
    }
}

SGL_NS_END
