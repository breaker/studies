// Input.cpp
//

#include "StdAfx.h"
#include <SGL/Input.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ���봦���� Input
////////////////////////////////////////////////////////////////////////////////

void Input::ProcessInput(float timeDelta) {
    if (KEY_DOWN(VK_ESCAPE)) {
        m_Main->SetState(Main::STATE_EXIT);
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    }
}

SGL_NS_END
