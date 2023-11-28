// RenderToon.cpp
//

#include "StdAfx.h"
#include "Toon.h"
#include "RenderToon.h"

////////////////////////////////////////////////////////////////////////////////
// 顶点格式 ToonVertex
////////////////////////////////////////////////////////////////////////////////

const DWORD ToonVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX3;

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderToon
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderToon::ToonShadeTexFile = _T("toonshade.bmp");
LPCTSTR RenderToon::ToonEffectFile = _T("tooneffect.fx");

BOOL RenderToon::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Create geometry and compute corresponding world matrix and color for each mesh.

    ID3DXMesh* mesh;
    D3DXCreateTeapot(m_D3DDev, &mesh, NULL);
    m_Meshes[0].Attach(mesh);
    D3DXCreateSphere(m_D3DDev, 1.0f, 20, 20, &mesh, NULL);
    m_Meshes[1].Attach(mesh);
    D3DXCreateTorus(m_D3DDev, 0.5f, 1.0f, 20, 20, &mesh, NULL);
    m_Meshes[2].Attach(mesh);
    D3DXCreateCylinder(m_D3DDev, 0.5f, 0.5f, 2.0f, 20, 20, &mesh, NULL);
    m_Meshes[3].Attach(mesh);

    D3DXMatrixTranslation(&m_WorldMatrices[0],  0.0f,  2.0f, 0.0f);
    D3DXMatrixTranslation(&m_WorldMatrices[1],  0.0f, -2.0f, 0.0f);
    D3DXMatrixTranslation(&m_WorldMatrices[2], -3.0f,  0.0f, 0.0f);
    D3DXMatrixTranslation(&m_WorldMatrices[3],  3.0f,  0.0f, 0.0f);

    m_MeshColors[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
    m_MeshColors[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
    m_MeshColors[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
    m_MeshColors[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

    // Create effects.

    ID3DXEffect* effect = NULL;
    ID3DXBuffer* errBuf = NULL;
    hr = D3DXCreateEffectFromFile(m_D3DDev,             // associated m_D3DDev
                                  ToonEffectFile,       // effect filename
                                  NULL,                 // no preprocessor definitions
                                  NULL,                 // no ID3DXInclude interface
                                  D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, // compile flags
                                  NULL,         // don't share parameters
                                  &effect,      // return effect
                                  &errBuf);     // return error messages
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCreateEffectFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateEffectFromFile", hr); return FALSE);
    m_ToonEffect.Attach(effect);

    // Save Frequently Accessed Parameter Handles
    m_ToonTechHandle = effect->GetTechniqueByName("Toon");
    m_ShadeTexHandle = effect->GetParameterByName(NULL, "ShadeTex");
    m_WorldViewHandle = effect->GetParameterByName(NULL, "WorldViewMatrix");
    m_WorldViewProjHandle = effect->GetParameterByName(NULL, "WorldViewProjMatrix");
    m_ColorHandle    = effect->GetParameterByName(NULL, "Color");
    m_LightDirHandle = effect->GetParameterByName(NULL, "LightDirection");

    // Set Effect Parameters

    // Projection:
    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);

    // Light direction:
    D3DXVECTOR4 lightDirection(-0.57f, 0.57f, -0.57f, 0.0f);
    effect->SetVector(m_LightDirHandle, &lightDirection);

    // Set texture:
    IDirect3DTexture9* tex = NULL;
    D3DXCreateTextureFromFile(m_D3DDev, ToonShadeTexFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    effect->SetTexture(m_ShadeTexHandle, tex);
    SGL::Release(tex);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderToon::InitFont() {
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

void RenderToon::Draw(float timeDelta) {
    // Update the scene: Allow user to rotate around scene.
    static float angle  = (3.0f * D3DX_PI) / 2.0f;
    static float height = 5.0f;

    if (SGL::KEY_DOWN(VK_LEFT))
        angle -= 0.5f * timeDelta;
    if (SGL::KEY_DOWN(VK_RIGHT))
        angle += 0.5f * timeDelta;
    if (SGL::KEY_DOWN(VK_UP))
        height += 5.0f * timeDelta;
    if (SGL::KEY_DOWN(VK_DOWN))
        height -= 5.0f * timeDelta;

    D3DXVECTOR3 pos(cosf(angle) * 10.0f, height, sinf(angle) * 10.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);

    // Activate the Technique and Render

    // Render
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    m_ToonEffect->SetTechnique(m_ToonTechHandle);

    UINT numPasses = 0;
    m_ToonEffect->Begin(&numPasses, 0);

    D3DXMATRIX WorldView;
    D3DXMATRIX WorldViewProj;
    for (UINT i = 0; i < numPasses; ++i) {
        m_ToonEffect->BeginPass(i); // 原示例代码中是 Pass, 现无此方法, 取而代之的是 BeginPass/EndPass
        for (int j = 0; j < 4; ++j) {
            WorldView     = m_WorldMatrices[j] * V;
            WorldViewProj = m_WorldMatrices[j] * V * m_ProjMatrix;

            m_ToonEffect->SetMatrix(m_WorldViewHandle, &WorldView);
            m_ToonEffect->SetMatrix(m_WorldViewProjHandle, &WorldViewProj);
            m_ToonEffect->SetVector(m_ColorHandle, &m_MeshColors[j]);
            m_Meshes[j]->DrawSubset(0);
        }
        m_ToonEffect->EndPass();
    }
    m_ToonEffect->End();

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 ToonInput
////////////////////////////////////////////////////////////////////////////////

void ToonInput::ProcessInput(float timeDelta) {
    Input::QueryClose(VK_ESCAPE);
}
