// RenderMtrlAlpha.cpp
//

#include "StdAfx.h"
#include "MtrlAlpha.h"
#include "RenderMtrlAlpha.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderMtrlAlpha
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderMtrlAlpha::TEXTURE_FILE = _T("crate.jpg");

BOOL RenderMtrlAlpha::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
{
    HRESULT hr = Render::Init(width, height, hwnd, windowed, devType);
    SGL_FAILED_DO(hr, MYTRACE_DXERR("Render::Init", hr); return FALSE);

    // 设置材质
    m_TeapotMtrl = SGL::RED_MTRL;
    m_TeapotMtrl.Diffuse.a = 0.5f;  // alpha = 50%

    m_BkGndMtrl = SGL::WHITE_MTRL;

    // Create the teapot.
    ID3DXMesh* teapot;
    D3DXCreateTeapot(m_D3DDev, &teapot, NULL);
    m_Teapot.Attach(teapot);

    // 背景矩形
    IDirect3DVertexBuffer9* bkGndQuad;
    m_D3DDev->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &bkGndQuad, NULL);
    m_BkGndQuad.Attach(bkGndQuad);

    Vertex* v;
    m_BkGndQuad->Lock(0, 0, (void**) &v, 0);

    v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[1] = Vertex(-10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[2] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[4] = Vertex( 10.0f,  10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[5] = Vertex( 10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    m_BkGndQuad->Unlock();

    // 设置方向光
    D3DLIGHT9 dir;
    SGL::ZeroObj(&dir);
    dir.Type      = D3DLIGHT_DIRECTIONAL;
    dir.Diffuse   = SGL::WHITE;
    dir.Specular  = SGL::WHITE * 0.2f;
    dir.Ambient   = SGL::WHITE * 0.6f;
    dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);  // 45 度入射

    m_D3DDev->SetLight(0, &dir);
    m_D3DDev->LightEnable(0, TRUE);

    m_D3DDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    m_D3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    // 设置纹理
    IDirect3DTexture9* bkGndTex;
    D3DXCreateTextureFromFile(m_D3DDev, TEXTURE_FILE, &bkGndTex);
    m_BkGndTex.Attach(bkGndTex);

    m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // 设置混合状态

    // use alpha in material's diffuse component for alpha
    m_D3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    m_D3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    // set blending factors so that alpha component determines transparency, 这两个也是默认混合因子
    m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Set camera.
    D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);

    m_D3DDev->SetTransform(D3DTS_VIEW, &V);

    // Set projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float) width / (float) height, 1.0f, 1000.0f);
    m_D3DDev->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

void RenderMtrlAlpha::Display(float timeDelta)
{
    // Render
    m_D3DDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);
    m_D3DDev->BeginScene();

    // Draw the background
    D3DXMATRIX W;
    D3DXMatrixIdentity(&W);
    m_D3DDev->SetTransform(D3DTS_WORLD, &W);
    m_D3DDev->SetFVF(Vertex::FVF);
    m_D3DDev->SetStreamSource(0, m_BkGndQuad, 0, sizeof(Vertex));
    m_D3DDev->SetMaterial(&m_BkGndMtrl);
    m_D3DDev->SetTexture(0, m_BkGndTex);
    m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    // Draw the teapot
    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);    // 放大
    m_D3DDev->SetTransform(D3DTS_WORLD, &W);
    m_D3DDev->SetMaterial(&m_TeapotMtrl);
    m_D3DDev->SetTexture(0, 0); // 取消纹理
    m_Teapot->DrawSubset(0);

    m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_D3DDev->EndScene();
    m_D3DDev->Present(NULL, NULL, NULL, NULL);
}

void RenderMtrlAlpha::IncAlpha()
{
    m_TeapotMtrl.Diffuse.a += 0.01f;
    if (m_TeapotMtrl.Diffuse.a > 1.0f)  // force alpha to [0, 1] interval
        m_TeapotMtrl.Diffuse.a = 1.0f;
}

void RenderMtrlAlpha::DecAlpha()
{
    m_TeapotMtrl.Diffuse.a -= 0.01f;
    if (m_TeapotMtrl.Diffuse.a < 0.0f)
        m_TeapotMtrl.Diffuse.a = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 MtrlAlphaInput
////////////////////////////////////////////////////////////////////////////////

void MtrlAlphaInput::ProcessInput(float timeDelta)
{
    RenderMtrlAlpha* render = (RenderMtrlAlpha*) m_Main->GetRender();

    // increase/decrease alpha via keyboard input
    if (SGL::KEY_DOWN('A'))
        render->IncAlpha();
    if (SGL::KEY_DOWN('S'))
        render->DecAlpha();

    Input::ProcessInput(timeDelta);
}
