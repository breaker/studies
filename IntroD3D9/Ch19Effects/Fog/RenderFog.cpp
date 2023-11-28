// RenderFog.cpp
//

#include "StdAfx.h"
#include "Fog.h"
#include "RenderFog.h"

////////////////////////////////////////////////////////////////////////////////
// 顶点格式 FogVertex
////////////////////////////////////////////////////////////////////////////////

const DWORD FogVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX3;

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderFog
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderFog::HeightMapFile = _T("coastMountain64.raw");
LPCTSTR RenderFog::FogEffectFile = _T("fog.fx");

BOOL RenderFog::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Init Scene.
    D3DXVECTOR3 lightDir(0.0f, 1.0f, 0.0f);
    m_Terrain.reset(new SGL::Terrain());
    if (!m_Terrain->Init(m_D3DDev, HeightMapFile, 64, 64, 6, 0.5f, D3DFMT_INDEX16))
        return FALSE;
    m_Terrain->GenTexture(&lightDir);

    // Set texture filters.
    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    // Set Camera Position.
    D3DXVECTOR3 pos(100.0f, 100.0f, -250.0f);
    m_Camera.SetPosition(pos);

    // Create effect.
    ID3DXEffect* effect = NULL;
    ID3DXBuffer* errBuf = NULL;
    hr = D3DXCreateEffectFromFile(m_D3DDev, FogEffectFile, NULL, NULL, D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, NULL, &effect, &errBuf);
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCreateEffectFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateEffectFromFile", hr); return FALSE);
    m_FogEffect.Attach(effect);

    // Save Frequently Accessed Parameter Handles
    m_FogTechHandle = effect->GetTechniqueByName("Fog");

    // Set Projection.
    D3DXMATRIX P;
    D3DXMatrixPerspectiveFovLH(&P, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &P);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderFog::InitFont() {
    // ID3DXFont 内部使用 GDI (DrawText) 绘制文字
    D3DXFONT_DESC fd;
    SGL::ZeroObj(&fd);
    fd.Height           = 20;   // in logical units
    fd.Width            = 7;    // in logical units
    fd.Weight           = 500;  // boldness, range 0(light) - 1000(bold)
    fd.MipLevels        = D3DX_DEFAULT;
    fd.Italic           = FALSE;
    fd.CharSet          = DEFAULT_CHARSET;
    fd.OutputPrecision  = 0;
    fd.Quality          = 0;
    fd.PitchAndFamily   = 0;
    _tcscpy_s(fd.FaceName, _countof(fd.FaceName), _T("Times New Roman"));

    // Create an ID3DXFont based on D3DXFONT_DESC.
    ID3DXFont* font;
    HRESULT hr = D3DXCreateFontIndirect(m_D3DDev, &fd, &font);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateFontIndirect", hr); return FALSE);
    m_Font.Attach(font);

    return TRUE;
}

void RenderFog::Draw(float timeDelta) {
    D3DXMATRIX V;
    m_Camera.GetViewMatrix(&V);
    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Activate the Technique and Render

    // Render
    //m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00CCCCCC, 1.0f, 0);
    // 注意背景色的设置: 应和雾色 (远处景深渐隐) 的灰色接近, 才能表现出完整效果 (天空)
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xCC, 0xCC, 0xCC), 1.0f, 0);
    m_D3DDev->BeginScene();

    // set the technique to use
    m_FogEffect->SetTechnique(m_FogTechHandle);

    UINT numPasses = NULL;
    m_FogEffect->Begin(&numPasses, 0);

    D3DXMATRIX I;
    D3DXMatrixIdentity(&I);
    for (UINT i = 0; i < numPasses; ++i) {
        m_FogEffect->BeginPass(i);
        m_Terrain->Draw(&I, FALSE);
        m_FogEffect->EndPass();
    }
    m_FogEffect->End();

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 FogInput
////////////////////////////////////////////////////////////////////////////////

void FogInput::ProcessInput(float timeDelta) {
    RenderFog* render = (RenderFog*) m_Main->GetRender();
    SGL::Camera& camera = render->m_Camera;

    // Update: Update the camera.

    if (SGL::KEY_DOWN('W'))
        camera.Walk(60.0f * timeDelta);

    if (SGL::KEY_DOWN('S'))
        camera.Walk(-60.0f * timeDelta);

    if (SGL::KEY_DOWN('A'))
        camera.Yaw(-1.0f * timeDelta);

    if (SGL::KEY_DOWN('D'))
        camera.Yaw(1.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_UP))
        camera.Pitch(-1.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_DOWN))
        camera.Pitch(1.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_LEFT))
        camera.Strafe(-60.0f * timeDelta);

    if (SGL::KEY_DOWN(VK_RIGHT))
        camera.Strafe(60.0f * timeDelta);

    Input::QueryClose(VK_ESCAPE);
}
