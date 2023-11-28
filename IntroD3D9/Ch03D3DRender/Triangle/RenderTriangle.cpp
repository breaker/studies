// RenderTriangle.cpp
//

#include "StdAfx.h"
#include "Triangle.h"
#include "RenderTriangle.h"

////////////////////////////////////////////////////////////////////////////////
// 绘制三角形类 RenderTriangle
////////////////////////////////////////////////////////////////////////////////

RenderTriangle::RenderTriangle(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    initVertex();
    initCamera();
    initRender();
}

// 初始化三角形顶点
BOOL RenderTriangle::initVertex()
{
    IDirect3DVertexBuffer9* tgl;
    d3ddev_->CreateVertexBuffer(3 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED,  &tgl, NULL);

    Vertex* vert;
    tgl->Lock(0, 0, (void**) &vert, 0);

    // 同一 Z 平面 z=2.0
    vert[0] = Vertex(-1.0f, 0.0f, 2.0f);
    vert[1] = Vertex( 0.0f, 1.0f, 2.0f);
    vert[2] = Vertex( 1.0f, 0.0f, 2.0f);

    tgl->Unlock();

    triangle_.Attach(tgl);
    return TRUE;
}

// 初始化摄像机
BOOL RenderTriangle::initCamera()
{
    // 不设置取景变换 (view space transform)

    // 设置投影 (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

BOOL RenderTriangle::initRender()
{
    // 设置绘制状态, 线框模式 (wireframe)
    d3ddev_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    return TRUE;
}

void RenderTriangle::render(float timeDelta)
{
    // 不使用世界变换 (world transform)

    // 擦除背景, z-buffer=1.0, 背景白色
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);

    // 绘制场景
    d3ddev_->BeginScene();

    d3ddev_->SetStreamSource(0, triangle_, 0, sizeof(Vertex));
    d3ddev_->SetFVF(Vertex::FVF);

    // 绘制三角形
    d3ddev_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
