// Input.cpp
//

#include "StdAfx.h"
#include <SGL/Input.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Input
////////////////////////////////////////////////////////////////////////////////

void Input::ProcessInput(float timeDelta) {
    QueryClose(VK_ESCAPE);
}

BOOL Input::QueryClose(int vkey) {
    if (KEY_DOWN(vkey)) {
        m_Main->SetState(Main::STATE_EXIT);
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
        return TRUE;
    }
    return FALSE;
}

SGL_NS_END
