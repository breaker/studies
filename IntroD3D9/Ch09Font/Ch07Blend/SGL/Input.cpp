// Input.cpp
//

#include "StdAfx.h"
#include <SGL/Input.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Input
////////////////////////////////////////////////////////////////////////////////

void Input::ProcessInput(float timeDelta) {
    if (KEY_DOWN(VK_ESCAPE)) {
        m_Main->SetState(Main::STATE_EXIT);
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    }
}

_SGL_NS_END
