// RenderAddressMode.cpp
//

#include "StdAfx.h"
#include "AddressMode.h"
#include "RenderAddressMode.h"

////////////////////////////////////////////////////////////////////////////////
// 名字空间
////////////////////////////////////////////////////////////////////////////////

using SGL::KEY_DOWN;

////////////////////////////////////////////////////////////////////////////////
// 绘制物体类 RenderAddressMode
////////////////////////////////////////////////////////////////////////////////

LPCTSTR RenderAddressMode::TEXTURE_FILE = _T("dx5_logo.bmp");

RenderAddressMode::RenderAddressMode(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    // Create the Quad.
    IDirect3DVertexBuffer9* quad;
    d3ddev_->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &quad, NULL);
    quad_.Attach(quad);

    Vertex* v;
    quad->Lock(0, 0, (void**) &v, 0);

    // quad built from two triangles:
    v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 3.0f);
    v[1] = Vertex(-1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[2] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 3.0f, 0.0f);

    v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 3.0f);
    v[4] = Vertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 3.0f, 0.0f);
    v[5] = Vertex( 1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 3.0f, 3.0f);

    quad->Unlock();

    // Create the texture and set texture filters.

    IDirect3DTexture9* tex;
    D3DXCreateTextureFromFile(d3ddev_, TEXTURE_FILE, &tex);
    tex_.Attach(tex);

    d3ddev_->SetTexture(0, tex);

    d3ddev_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    d3ddev_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d3ddev_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

    // Don't use lighting for this sample.
    d3ddev_->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Set the projection matrix.
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float) width / (float) height, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
}

void RenderAddressMode::render(float timeDelta)
{
    // 设置取景变换 (view space transform)
    static float angle  = (3.0f * D3DX_PI) / 2.0f;

    // set wrap address mode
    if (KEY_DOWN('W')) {
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    }

    // set border color address mode
    if(KEY_DOWN('B')) {
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
        d3ddev_->SetSamplerState(0,  D3DSAMP_BORDERCOLOR, D3DCOLOR_ARGB(0, 0, 0, 0xff));
    }

    // set clamp address mode
    if(KEY_DOWN('C')) {
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }

    // set mirror address mode
    if(KEY_DOWN('M')) {
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
        d3ddev_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
    }

    // 擦除背景, z-buffer=1.0, 背景黑色
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

    // 绘制场景
    d3ddev_->BeginScene();

    d3ddev_->SetStreamSource(0, quad_, 0, sizeof(Vertex));
    d3ddev_->SetFVF(Vertex::FVF);
    d3ddev_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
