// RenderToon2.cpp
//

#include "StdAfx.h"
#include "Toon2.h"
#include "RenderToon2.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderToon2
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderToon2::ToonVSFile = _T("toon.fx");
LPCTSTR RenderToon2::OutlineVSFile = _T("outline.fx");
LPCTSTR RenderToon2::ToonShadeTexFile = _T("toonshade.bmp");

BOOL RenderToon2::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Create geometry and compute corresponding world matrix and color for each mesh.
    ID3DXBuffer* adjBuf[4] = {0};
    ID3DXMesh* mesh;
    D3DXCreateTeapot(m_D3DDev, &mesh, &adjBuf[0]);
    m_Meshes[0].Attach(mesh);
    D3DXCreateSphere(m_D3DDev, 1.0f, 20, 20, &mesh, &adjBuf[1]);
    m_Meshes[1].Attach(mesh);
    D3DXCreateTorus(m_D3DDev, 0.5f, 1.0f, 20, 20, &mesh, &adjBuf[2]);
    m_Meshes[2].Attach(mesh);
    D3DXCreateCylinder(m_D3DDev, 0.5f, 0.5f, 2.0f, 20, 20, &mesh, &adjBuf[3]);
    m_Meshes[3].Attach(mesh);

    D3DXMatrixTranslation(&m_WorldMatrices[0],  0.0f,  2.0f, 0.0f);
    D3DXMatrixTranslation(&m_WorldMatrices[1],  0.0f, -2.0f, 0.0f);
    D3DXMatrixTranslation(&m_WorldMatrices[2], -3.0f,  0.0f, 0.0f);
    D3DXMatrixTranslation(&m_WorldMatrices[3],  3.0f,  0.0f, 0.0f);

    m_MeshColors[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
    m_MeshColors[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
    m_MeshColors[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
    m_MeshColors[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

    // Allocate mesh outlines

    m_MeshOutlines[0].reset(new SGL::SilhouetteEdges(m_D3DDev, m_Meshes[0], adjBuf[0]));
    m_MeshOutlines[1].reset(new SGL::SilhouetteEdges(m_D3DDev, m_Meshes[1], adjBuf[1]));
    m_MeshOutlines[2].reset(new SGL::SilhouetteEdges(m_D3DDev, m_Meshes[2], adjBuf[2]));
    m_MeshOutlines[3].reset(new SGL::SilhouetteEdges(m_D3DDev, m_Meshes[3], adjBuf[3]));

    SGL::Release(adjBuf[0]);
    SGL::Release(adjBuf[1]);
    SGL::Release(adjBuf[2]);
    SGL::Release(adjBuf[3]);

    // Compile Toon Shader
    ID3DXBuffer* shaderBuf = NULL;
    ID3DXBuffer* errBuf = NULL;
    ID3DXConstantTable* constTbl;
    hr = D3DXCompileShaderFromFile(ToonVSFile, NULL, NULL, "Main", "vs_1_1", D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &shaderBuf, &errBuf, &constTbl);
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCompileShaderFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCompileShaderFromFile", hr); return FALSE);
    m_ToonConstTable.Attach(constTbl);

    // Create shader
    IDirect3DVertexShader9* shader = NULL;
    hr = m_D3DDev->CreateVertexShader((DWORD*) shaderBuf->GetBufferPointer(), &shader);
    SGL_FAILED_DO(hr, MYTRACE_DX("CreateVertexShader", hr); return FALSE);
    m_ToonShader.Attach(shader);

    SGL::Release(shaderBuf);

    // Compile Outline shader.
    hr = D3DXCompileShaderFromFile(OutlineVSFile, NULL, NULL, "Main", "vs_1_1", D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &shaderBuf, &errBuf, &constTbl);
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCompileShaderFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCompileShaderFromFile", hr); return FALSE);
    m_OutlineConstTable.Attach(constTbl);

    // Create shader
    hr = m_D3DDev->CreateVertexShader((DWORD*) shaderBuf->GetBufferPointer(), &shader);
    SGL_FAILED_DO(hr, MYTRACE_DX("CreateVertexShader", hr); return FALSE);
    m_OutlineShader.Attach(shader);

    SGL::Release(shaderBuf);

    // Load textures.

    IDirect3DTexture9* tex;
    D3DXCreateTextureFromFile(m_D3DDev, ToonShadeTexFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    m_ShadeTex.Attach(tex);

    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

    // Get Handles

    m_ToonWorldViewHandle     = m_ToonConstTable->GetConstantByName(NULL, "WorldViewMatrix");
    m_ToonWorldViewProjHandle = m_ToonConstTable->GetConstantByName(NULL, "WorldViewProjMatrix");
    m_ToonColorHandle         = m_ToonConstTable->GetConstantByName(NULL, "Color");
    m_ToonLightDirHandle      = m_ToonConstTable->GetConstantByName(NULL, "LightDirection");

    m_OutlineWorldViewHandle = m_OutlineConstTable->GetConstantByName(NULL, "WorldViewMatrix");
    m_OutlineProjHandle      = m_OutlineConstTable->GetConstantByName(NULL, "ProjMatrix");

    // Set shader constants:

    // Light direction:
    D3DXVECTOR4 dirToLight(-0.57f, 0.57f, -0.57f, 0.0f);
    m_ToonConstTable->SetVector(m_D3DDev, m_ToonLightDirHandle, &dirToLight);

    m_ToonConstTable->SetDefaults(m_D3DDev);
    m_OutlineConstTable->SetDefaults(m_D3DDev);

    // Calculate projection matrix.
    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderToon2::InitFont() {
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

void RenderToon2::Draw(float timeDelta) {
    // Update the scene: Allow user to rotate around scene.
    static float angle  = (3.0f * D3DX_PI) / 2.0f;
    static float height = 5.0f;

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

    // Render
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    // Draw Cartoon
    m_D3DDev->SetVertexShader(m_ToonShader);
    m_D3DDev->SetTexture(0, m_ShadeTex);

    D3DXMATRIX WorldView;
    D3DXMATRIX WorldViewProj;
    for (int i = 0; i < 4; ++i) {
        WorldView = m_WorldMatrices[i] * V;
        WorldViewProj = m_WorldMatrices[i] * V * m_ProjMatrix;

        m_ToonConstTable->SetMatrix(m_D3DDev, m_ToonWorldViewHandle, &WorldView);
        m_ToonConstTable->SetMatrix(m_D3DDev, m_ToonWorldViewProjHandle, &WorldViewProj);
        m_ToonConstTable->SetVector(m_D3DDev, m_ToonColorHandle, &m_MeshColors[i]);

        m_Meshes[i]->DrawSubset(0);
    }

    // Draw Outlines.
    m_D3DDev->SetVertexShader(m_OutlineShader);
    m_D3DDev->SetTexture(0, NULL);

    m_D3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    for (int i = 0; i < 4; ++i) {
        WorldView = m_WorldMatrices[i] * V;

        m_OutlineConstTable->SetMatrix(m_D3DDev, m_OutlineWorldViewHandle, &WorldView);
        m_OutlineConstTable->SetMatrix(m_D3DDev, m_OutlineProjHandle, &m_ProjMatrix);

        m_MeshOutlines[i]->Render();
    }

    m_D3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Toon2Input
////////////////////////////////////////////////////////////////////////////////

void Toon2Input::ProcessInput(float timeDelta) {
    Input::QueryClose(VK_ESCAPE);
}
