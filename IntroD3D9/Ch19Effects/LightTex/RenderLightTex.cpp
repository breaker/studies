// RenderLightTex.cpp
//

#include "StdAfx.h"
#include "LightTex.h"
#include "RenderLightTex.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderLightTex
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderLightTex::ModelFile = _T("mountain.x");
LPCTSTR RenderLightTex::LightTexEffectFile = _T("light_tex.fx");
LPCTSTR RenderLightTex::TexFile = _T("Terrain_3x_diffcol.jpg");

BOOL RenderLightTex::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType) {
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DX("Render::Init", hr); return FALSE);

    // Load the XFile data.
    ID3DXBuffer* mtrlBuf = NULL;
    DWORD numMtrls = 0;
    ID3DXMesh* mesh;

    hr = D3DXLoadMeshFromX(ModelFile, D3DXMESH_MANAGED, m_D3DDev, NULL, &mtrlBuf, NULL, &numMtrls, &mesh);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXLoadMeshFromX", hr); return FALSE);
    m_Mesh.Attach(mesh);

    // Extract the materials, load textures.

    if (mtrlBuf != 0 && numMtrls != 0) {
        D3DXMATERIAL* mtrls = (D3DXMATERIAL*) mtrlBuf->GetBufferPointer();

        for (DWORD i = 0; i < numMtrls; i++) {
            // the MatD3D property doesn't have an ambient value set when its loaded, so set it now:
            mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

            // save the ith material
            m_Mtrls.push_back(mtrls[i].MatD3D);

            // check if the ith material has an associative texture
            if (mtrls[i].pTextureFilename != 0) {
                // yes, load the texture for the ith subset
                IDirect3DTexture9* tex = NULL;
                hr = D3DXCreateTextureFromFileA(m_D3DDev, mtrls[i].pTextureFilename, &tex);
                SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFileA", hr); return FALSE);

                // save the loaded texture
                m_Textures.push_back(CComPtr<IDirect3DTexture9>(tex));
            }
            // no texture for the ith subset
            else
                m_Textures.push_back(CComPtr<IDirect3DTexture9>());
        }
    }
    SGL::Release(mtrlBuf); // done w/ buffer

    // Create effect.
    ID3DXBuffer* errBuf = NULL;
    ID3DXEffect* effect;
    hr = D3DXCreateEffectFromFile(m_D3DDev,
                                  LightTexEffectFile,
                                  NULL,             // no preprocessor definitions
                                  NULL,             // no ID3DXInclude interface
                                  D3DXSHADER_DEBUG, // compile flags
                                  NULL,             // don't share parameters
                                  &effect,
                                  &errBuf);
    // output any error messages
    if (errBuf != NULL) {
        MYTRACEA("D3DXCreateEffectFromFile failed: %s", (char*) errBuf->GetBufferPointer());
        SGL::Release(errBuf);
    }
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateEffectFromFile", hr); return FALSE);
    m_LightTexEffect.Attach(effect);

    // Save Frequently Accessed Parameter Handles

    m_WorldMatrixHandle = effect->GetParameterByName(NULL, "WorldMatrix");
    m_ViewMatrixHandle  = effect->GetParameterByName(NULL, "ViewMatrix");
    m_ProjMatrixHandle  = effect->GetParameterByName(NULL, "ProjMatrix");
    m_TexHandle         = effect->GetParameterByName(NULL, "Tex");

    m_LightTexTechHandle = effect->GetTechniqueByName("LightAndTexture");

    // Set Effect Parameters

    // Set Matrices
    D3DXMATRIX W, P;

    D3DXMatrixIdentity(&W);
    effect->SetMatrix(m_WorldMatrixHandle, &W);

    D3DXMatrixPerspectiveFovLH(&P, D3DX_PI * 0.25f, (float) width / (float) height, 1.0f, 1000.0f);
    effect->SetMatrix(m_ProjMatrixHandle, &P);

    // Set texture
    IDirect3DTexture9* tex = NULL;
    hr = D3DXCreateTextureFromFile(m_D3DDev, TexFile, &tex);
    SGL_FAILED_DO(hr, MYTRACE_DX("D3DXCreateTextureFromFile", hr); return FALSE);
    effect->SetTexture(m_TexHandle, tex);

    SGL::Release(tex);

    if (!InitFont())
        return FALSE;

    return TRUE;
}

BOOL RenderLightTex::InitFont() {
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

void RenderLightTex::Draw(float timeDelta) {
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
    m_LightTexEffect->SetMatrix(m_ViewMatrixHandle, &V);

    // Activate the Technique and Render
    m_D3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0x40, 0x40, 0x40), 1.0f, 0);
    m_D3DDev->BeginScene();

    // set the technique to use
    m_LightTexEffect->SetTechnique(m_LightTexTechHandle);

    UINT numPasses = 0;
    m_LightTexEffect->Begin(&numPasses, 0);

    for (UINT i = 0; i < numPasses; ++i) {
        m_LightTexEffect->BeginPass(i);
        for (UINT j = 0; j < m_Mtrls.size(); ++j)
            m_Mesh->DrawSubset(j);
        m_LightTexEffect->EndPass();
    }
    m_LightTexEffect->End();

    // Draw FPS text
    RECT rect = {10, 10, m_Width, m_Height};
    m_Font->DrawText(NULL, m_FPS.CalcFPSStr(timeDelta), -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_ARGB(0xff, 0, 0x80, 0));

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 LightTexInput
////////////////////////////////////////////////////////////////////////////////

void LightTexInput::ProcessInput(float timeDelta) {
    Input::QueryClose(VK_ESCAPE);
}
