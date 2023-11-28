// RenderCreateText.cpp
//

#include "StdAfx.h"
#include "CreateText.h"
#include "RenderCreateText.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderCreateText
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderCreateText::STR_TEXT = _T("Hello D3D");

BOOL RenderCreateText::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
{
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DXERR("Render::Init", hr); return FALSE);

    // Get a handle to a device context.
    HDC hdc = CreateCompatibleDC(NULL);

    // Describe the font we want.
    LOGFONT lf;
    SGL::ZeroObj(&lf);

    lf.lfHeight         = 25;   // in logical units
    lf.lfWidth          = 12;   // in logical units
    lf.lfEscapement     = 0;
    lf.lfOrientation    = 0;
    lf.lfWeight         = 500;  // boldness, range 0(light) - 1000(bold)
    lf.lfItalic         = FALSE;
    lf.lfUnderline      = FALSE;
    lf.lfStrikeOut      = FALSE;
    lf.lfCharSet        = DEFAULT_CHARSET;
    lf.lfOutPrecision   = 0;
    lf.lfClipPrecision  = 0;
    lf.lfQuality        = 0;
    lf.lfPitchAndFamily = 0;
    _tcscpy_s(lf.lfFaceName, _countof(lf.lfFaceName), _T("Times New Roman"));   // font style

    // Create the font and select it with the device context.
    HFONT hFont = CreateFontIndirect(&lf);
    HFONT hFontOld = (HFONT) SelectObject(hdc, hFont);

    // Create the text mesh based on the selected font in the HDC.
    ID3DXMesh* textMesh;
    D3DXCreateText(m_D3DDev, hdc, STR_TEXT, 0.001f, 0.4f, &textMesh, NULL, NULL);
    m_TextMesh.Attach(textMesh);

    // Restore the old font and free the acquired HDC.
    SelectObject(hdc, hFontOld);
    DeleteObject(hFont);
    DeleteDC(hdc);

    // Lights.
    D3DXVECTOR3 dir(0.0f, -0.5f, 1.0f);
    D3DXCOLOR clr = SGL::WHITE;
    D3DLIGHT9 light;
    SGL::InitDirLight(&light, dir, clr);

    m_D3DDev->SetLight(0, &light);
    m_D3DDev->LightEnable(0, TRUE);

    m_D3DDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    // Set camera.
    D3DXVECTOR3 pos(0.0f, 1.5f, -3.3f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Set projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

void RenderCreateText::Draw(float timeDelta)
{
    // Update: Spin the 3D text.
    D3DXMATRIX yRot, T;
    static float y = 0.0f;

    D3DXMatrixRotationY(&yRot, y);
    y += timeDelta;

    if (y >= 2 * D3DX_PI)
        y = 0.0f;

    D3DXMatrixTranslation(&T, -1.6f, 0.0f, 0.0f);
    T *= yRot;

    m_D3DDev->SetTransform(D3DTS_WORLD, &T);

    // Render

    m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0, 0, 0), 1.0f, 0);
    m_D3DDev->BeginScene();

    m_D3DDev->SetMaterial(&SGL::WHITE_MTRL);
    m_TextMesh->DrawSubset(0);

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 CreateTextInput
////////////////////////////////////////////////////////////////////////////////

void CreateTextInput::ProcessInput(float timeDelta)
{
    RenderCreateText* render = (RenderCreateText*) m_Main->GetRender();
    Input::ProcessInput(timeDelta);
}
