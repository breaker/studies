// RenderColorTriangle.cpp
//

#include "StdAfx.h"
#include "ColorTriangle.h"
#include "RenderColorTriangle.h"

////////////////////////////////////////////////////////////////////////////////
// 绘制三角形类 RenderColorTriangle
////////////////////////////////////////////////////////////////////////////////

RenderColorTriangle::RenderColorTriangle(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    init_vertex();
    init_camera();
    init_render();
}

// 初始化三角形顶点
BOOL RenderColorTriangle::init_vertex()
{
    IDirect3DVertexBuffer9* tgl;
    d3ddev_->CreateVertexBuffer(3 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &tgl, NULL);

    Vertex* vert;
    tgl->Lock(0, 0, (void**) &vert, 0);

    // 同一 Z 平面 z=2.0
    vert[0] = Vertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
    vert[1] = Vertex( 0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
    vert[2] = Vertex( 1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));

    tgl->Unlock();

    // 世界坐标变换矩阵
    D3DXMatrixTranslation(&leftWorldM_, -1.25f, 0.0f, 0.0f);
    D3DXMatrixTranslation(&rightWorldM_, 1.25f, 0.0f, 0.0f);

    colorTriangle_.Attach(tgl);
    return TRUE;
}

// 初始化摄像机
BOOL RenderColorTriangle::init_camera()
{
    // 不设置取景变换 (view space transform)

    // 设置投影 (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

BOOL RenderColorTriangle::init_render()
{
    // 关闭光照
    d3ddev_->SetRenderState(D3DRS_LIGHTING, FALSE);
    return TRUE;
}

void RenderColorTriangle::render(float timeDelta)
{
    // 擦除背景, z-buffer=1.0, 背景白色
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);

    // 绘制场景
    d3ddev_->BeginScene();

    d3ddev_->SetStreamSource(0, colorTriangle_, 0, sizeof(Vertex));
    d3ddev_->SetFVF(Vertex::FVF);

    // 绘制三角形

    // flat mode 着色
    d3ddev_->SetTransform(D3DTS_WORLD, &leftWorldM_);
    d3ddev_->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    d3ddev_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    // gouraud mode 着色
    d3ddev_->SetTransform(D3DTS_WORLD, &rightWorldM_);
    d3ddev_->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    d3ddev_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
