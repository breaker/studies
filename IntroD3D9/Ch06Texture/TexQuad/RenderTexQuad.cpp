// RenderTexQuad.cpp
//

#include "StdAfx.h"
#include "TexQuad.h"
#include "RenderTexQuad.h"

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderTexQuad
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderTexQuad::TEXTURE_FILE = _T("dx5_logo.bmp");

RenderTexQuad::RenderTexQuad(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    // Create the quad vertex buffer and fill it with the
    // quad geoemtry.
    IDirect3DVertexBuffer9* quad;
    d3ddev_->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &quad, NULL);
    quad_.Attach(quad);

    Vertex* v;
    quad->Lock(0, 0, (void**) &v, 0);

    // quad built from two triangles, note texture coordinates:
    v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[1] = Vertex(-1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[2] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

    v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
    v[4] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
    v[5] = Vertex( 1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

    quad->Unlock();

    // Create the texture and set filters.

    IDirect3DTexture9* tex;
    HRESULT hr = D3DXCreateTextureFromFile(d3ddev_, TEXTURE_FILE, &tex);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("D3DXCreateTextureFromFile", hr); throw SGLRenderExcept("create texture failed"));
    tex_.Attach(tex);

    d3ddev_->SetTexture(0, tex);

    d3ddev_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // 放大过滤器
    d3ddev_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); // 缩小过滤器
    d3ddev_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);  // 多级渐进纹理过滤器

    // Don't use lighting for this sample.
    d3ddev_->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Set the projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float) width / (float) height, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
}

void RenderTexQuad::render(float timeDelta)
{
    // 绘制场景
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);
    d3ddev_->BeginScene();

    d3ddev_->SetStreamSource(0, quad_, 0, sizeof(Vertex));
    d3ddev_->SetFVF(Vertex::FVF);
    d3ddev_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
