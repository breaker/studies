// RenderDiffuse.cpp
//

#include "StdAfx.h"
#include "Diffuse.h"
#include "RenderDiffuse.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderDiffuse
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderDiffuse::VertexShaderFile = _T("diffuse.fx");

BOOL RenderDiffuse::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Create geometry:
    ID3DXMesh* teapot;
    D3DXCreateTeapot(m_D3DDev, &teapot, NULL);
    m_Teapot.Attach(teapot);

    // Compile shader
    ID3DXBuffer* shaderBuf = NULL;
    ID3DXBuffer* errBuf = NULL;

    // 使用 DXSDK (August 2008) 时报错:
    // error X3025: global variables are implicitly constant, enable compatibility mode to allow modification
    // 解决: 加入旗标 D3DXSHADER_USE_LEGACY_D3DX9_31_DLL 或 D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY
    ID3DXConstantTable* constTbl;
    hr = D3DXCompileShaderFromFile(VertexShaderFile, NULL, NULL,
                                   "Main", // entry point function name
                                   "vs_1_1", D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &shaderBuf, &errBuf, &constTbl);
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCompileShaderFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCompileShaderFromFile", hr); return FALSE);
    m_DiffuseConstTbl.Attach(constTbl);

    // Create shader
    IDirect3DVertexShader9* shader = NULL;
    hr = m_D3DDev->CreateVertexShader((DWORD*) shaderBuf->GetBufferPointer(), &shader);
    SGL_FAILED_DO(hr, MYTRACE_DX("CreateVertexShader", hr); return FALSE);
    m_DiffuseShader.Attach(shader);

    SGL::Release(shaderBuf);

    // Get Handles
    m_ViewMatrixHandle    = constTbl->GetConstantByName(NULL, "ViewMatrix");
    m_ViewProjMatrixHandle= constTbl->GetConstantByName(NULL, "ViewProjMatrix");
    m_AmbientMtrlHandle   = constTbl->GetConstantByName(NULL, "AmbientMtrl");
    m_DiffuseMtrlHandle   = constTbl->GetConstantByName(NULL, "DiffuseMtrl");
    m_LightDirHandle      = constTbl->GetConstantByName(NULL, "LightDirection");

    // Set shader constants:

    // Light direction:
    D3DXVECTOR4 dirToLight(-0.57f, 0.57f, -0.57f, 0.0f);
    constTbl->SetVector(m_D3DDev, m_LightDirHandle, &dirToLight);

    // Materials:
    D3DXVECTOR4 ambientMtrl(0.0f, 0.0f, 1.0f, 1.0f);
    D3DXVECTOR4 diffuseMtrl(0.0f, 0.0f, 1.0f, 1.0f);

    constTbl->SetVector(m_D3DDev, m_AmbientMtrlHandle, &ambientMtrl);
    constTbl->SetVector(m_D3DDev, m_DiffuseMtrlHandle,&diffuseMtrl);
    constTbl->SetDefaults(m_D3DDev);

    // Compute projection matrix.
    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderDiffuse::InitFont() {
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

void RenderDiffuse::Draw(float timeDelta) {
    // Update the scene: Allow user to rotate around scene.
    static float angle  = (3.0f * D3DX_PI) / 2.0f;
    static float height = 3.0f;

    if (SGL::KEY_DOWN(VK_LEFT))
        angle -= 7.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_RIGHT))
        angle += 7.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_UP))
        height += 7.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_DOWN))
        height -= 7.0f * timeDelta;

    D3DXVECTOR3 pos(cosf(angle) * 7.0f, height, sinf(angle) * 7.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);

    m_DiffuseConstTbl->SetMatrix(m_D3DDev, m_ViewMatrixHandle, &V);

    D3DXMATRIX ViewProj = V * m_ProjMatrix;
    m_DiffuseConstTbl->SetMatrix(m_D3DDev, m_ViewProjMatrixHandle, &ViewProj);

    // Render
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    m_D3DDev->SetVertexShader(m_DiffuseShader);
    m_Teapot->DrawSubset(0);

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 DiffuseInput
////////////////////////////////////////////////////////////////////////////////

void DiffuseInput::ProcessInput(float timeDelta) {
    Input::QueryClose(VK_ESCAPE);
}
