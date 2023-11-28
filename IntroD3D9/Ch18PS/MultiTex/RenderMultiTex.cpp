// RenderMultiTex.cpp
//

#include "StdAfx.h"
#include "MultiTex.h"
#include "RenderMultiTex.h"

////////////////////////////////////////////////////////////////////////////////
// 顶点格式 MultiTexVertex
////////////////////////////////////////////////////////////////////////////////

const DWORD MultiTexVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX3;

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderMultiTex
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderMultiTex::MultiTexPSFile = _T("ps_multitex.fx");
LPCTSTR RenderMultiTex::BaseTexFile = _T("crate.bmp");
LPCTSTR RenderMultiTex::SpotLightTexFile = _T("spotlight.bmp");
LPCTSTR RenderMultiTex::StringTexFile = _T("text.bmp");

BOOL RenderMultiTex::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Create geometry.

    IDirect3DVertexBuffer9* vb;
    m_D3DDev->CreateVertexBuffer(6 * sizeof(MultiTexVertex), D3DUSAGE_WRITEONLY, MultiTexVertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
    m_QuadVB.Attach(vb);

    MultiTexVertex* v = NULL;
    vb->Lock(0, 0, (void**) &v, 0);

    v[0] = MultiTexVertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    v[1] = MultiTexVertex(-10.0f,  10.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    v[2] = MultiTexVertex( 10.0f,  10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    v[3] = MultiTexVertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    v[4] = MultiTexVertex( 10.0f,  10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    v[5] = MultiTexVertex( 10.0f, -10.0f, 5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    vb->Unlock();

    // Compile shader
    ID3DXBuffer* shaderBuf = NULL;
    ID3DXBuffer* errBuf = NULL;

    ID3DXConstantTable* constTbl;
    hr = D3DXCompileShaderFromFile(MultiTexPSFile, NULL, NULL, "Main", "ps_1_1", D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &shaderBuf, &errBuf, &constTbl);
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCompileShaderFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCompileShaderFromFile", hr); return FALSE);
    m_MultiTexCT.Attach(constTbl);

    // Create Pixel Shader
    IDirect3DPixelShader9* shader;
    hr = m_D3DDev->CreatePixelShader((DWORD*) shaderBuf->GetBufferPointer(), &shader);
    SGL_FAILED_DO(hr, MYTRACE_DX("CreatePixelShader", hr); return FALSE);
    m_MultiTexPS.Attach(shader);

    SGL::Release(shaderBuf);

    // Load textures.

    IDirect3DTexture9* tex;
    D3DXCreateTextureFromFile(m_D3DDev, BaseTexFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    m_BaseTex.Attach(tex);

    D3DXCreateTextureFromFile(m_D3DDev, SpotLightTexFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    m_SpotLightTex.Attach(tex);

    D3DXCreateTextureFromFile(m_D3DDev, StringTexFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    m_StringTex.Attach(tex);

    // Set Projection Matrix
    D3DXMATRIX P;
    D3DXMatrixPerspectiveFovLH(&P, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &P);

    // Disable lighting.
    m_D3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Get Handles
    m_BaseTexHandle      = constTbl->GetConstantByName(NULL, "BaseTex");
    m_SpotLightTexHandle = constTbl->GetConstantByName(NULL, "SpotLightTex");
    m_StringTexHandle    = constTbl->GetConstantByName(NULL, "StringTex");

    // Set constant descriptions:
    UINT count;
    constTbl->GetConstantDesc(m_BaseTexHandle, &m_BaseTexDesc, &count);
    constTbl->GetConstantDesc(m_SpotLightTexHandle, &m_SpotLightTexDesc, &count);
    constTbl->GetConstantDesc(m_StringTexHandle, &m_StringTexDesc, &count);
    constTbl->SetDefaults(m_D3DDev);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderMultiTex::InitFont() {
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

void RenderMultiTex::Draw(float timeDelta) {
    // Update the scene: Allow user to rotate around scene.
    static float angle  = (3.0f * D3DX_PI) / 2.0f;
    static float radius = 20.0f;

    if (SGL::KEY_DOWN(VK_LEFT))
        angle -= 0.5f * timeDelta;
    if (SGL::KEY_DOWN(VK_RIGHT))
        angle += 0.5f * timeDelta;
    if (SGL::KEY_DOWN(VK_UP))
        radius -= 2.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_DOWN))
        radius += 2.0f * timeDelta;

    D3DXVECTOR3 pos(cosf(angle) * radius, 0.0f, sinf(angle) * radius);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);

    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Render
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    m_D3DDev->SetPixelShader(m_MultiTexPS);
    m_D3DDev->SetFVF(MultiTexVertex::FVF);
    m_D3DDev->SetStreamSource(0, m_QuadVB, 0, sizeof(MultiTexVertex));

    // base tex
    m_D3DDev->SetTexture(m_BaseTexDesc.RegisterIndex, m_BaseTex);
    m_D3DDev->SetSamplerState(m_BaseTexDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(m_BaseTexDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(m_BaseTexDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    // spotlight tex
    m_D3DDev->SetTexture(m_SpotLightTexDesc.RegisterIndex, m_SpotLightTex);
    m_D3DDev->SetSamplerState(m_SpotLightTexDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(m_SpotLightTexDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(m_SpotLightTexDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    // string tex
    m_D3DDev->SetTexture(m_StringTexDesc.RegisterIndex, m_StringTex);
    m_D3DDev->SetSamplerState(m_StringTexDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(m_StringTexDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(m_StringTexDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 MultiTexInput
////////////////////////////////////////////////////////////////////////////////

void MultiTexInput::ProcessInput(float timeDelta) {
    Input::QueryClose(VK_ESCAPE);
}
