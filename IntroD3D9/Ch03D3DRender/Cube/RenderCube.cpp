// RenderCube.cpp
//

#include "StdAfx.h"
#include "Cube.h"
#include "RenderCube.h"

////////////////////////////////////////////////////////////////////////////////
// 绘制立方体异常类 RenderCubeExcept
////////////////////////////////////////////////////////////////////////////////

const char* RenderCubeExcept::ERR_INIT_VERTEX_FAILED = "initialize vertex failed";
const char* RenderCubeExcept::ERR_INIT_COORD_FAILED = "initialize coordinate failed";

////////////////////////////////////////////////////////////////////////////////
// 绘制立方体类 RenderCube
////////////////////////////////////////////////////////////////////////////////

RenderCube::RenderCube(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    if (!initVertex())
        throw RenderCubeExcept(RenderCubeExcept::ERR_INIT_VERTEX_FAILED);
    if (!initCamera())
        throw RenderCubeExcept(RenderCubeExcept::ERR_INIT_COORD_FAILED);
}

// 初始化立方体顶点
BOOL RenderCube::initVertex()
{
    // 初始化 vertex buffer
    IDirect3DVertexBuffer9* vb;
    HRESULT hr = d3ddev_->CreateVertexBuffer(8 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::CreateVertexBuffer", hr); return FALSE);
    vertex_buf_.Attach(vb);

    // 初始化 index buffer
    IDirect3DIndexBuffer9* ib;
    hr = d3ddev_->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::CreateIndexBuffer", hr); return FALSE);
    index_buf_.Attach(ib);

    // 填充 vertex buffer, 立方体顶点
    Vertex* vert;
    hr = vb->Lock(0, 0, (void**) &vert, 0);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DVertexBuffer9::Lock", hr); return FALSE);

    vert[0] = Vertex(-1.0f, -1.0f, -1.0f);
    vert[1] = Vertex(-1.0f,  1.0f, -1.0f);
    vert[2] = Vertex( 1.0f,  1.0f, -1.0f);
    vert[3] = Vertex( 1.0f, -1.0f, -1.0f);
    vert[4] = Vertex(-1.0f, -1.0f,  1.0f);
    vert[5] = Vertex(-1.0f,  1.0f,  1.0f);
    vert[6] = Vertex( 1.0f,  1.0f,  1.0f);
    vert[7] = Vertex( 1.0f, -1.0f,  1.0f);

    vb->Unlock();

    // 填充 index buffer, 立方体三角形图元绕序顶点索引
    WORD* idx = 0;
    hr = ib->Lock(0, 0, (void**) &idx, 0);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DIndexBuffer9::Lock", hr); return FALSE);

    // front side
    idx[0]  = 0;
    idx[1]  = 1;
    idx[2]  = 2;
    idx[3]  = 0;
    idx[4]  = 2;
    idx[5]  = 3;

    // back side
    idx[6]  = 4;
    idx[7]  = 6;
    idx[8]  = 5;
    idx[9]  = 4;
    idx[10] = 7;
    idx[11] = 6;

    // left side
    idx[12] = 4;
    idx[13] = 5;
    idx[14] = 1;
    idx[15] = 4;
    idx[16] = 1;
    idx[17] = 0;

    // right side
    idx[18] = 3;
    idx[19] = 2;
    idx[20] = 6;
    idx[21] = 3;
    idx[22] = 6;
    idx[23] = 7;

    // top
    idx[24] = 1;
    idx[25] = 5;
    idx[26] = 6;
    idx[27] = 1;
    idx[28] = 6;
    idx[29] = 2;

    // bottom
    idx[30] = 4;
    idx[31] = 0;
    idx[32] = 3;
    idx[33] = 4;
    idx[34] = 3;
    idx[35] = 7;
    ib->Unlock();

    return TRUE;
}

// 初始化摄像机
BOOL RenderCube::initCamera()
{
    // 设置取景变换 (view space transform)
    D3DXVECTOR3 pos(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    HRESULT hr = d3ddev_->SetTransform(D3DTS_VIEW, &V);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::SetTransform(D3DTS_VIEW)", hr); return FALSE);

    // 设置投影 (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    hr = d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::SetTransform(D3DTS_PROJECTION)", hr); return FALSE);

    // 设置绘制状态, 线框模式 (wireframe)
    hr = d3ddev_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::SetRenderState", hr); return FALSE);

    return TRUE;
}

void RenderCube::render(float timeDelta)
{
    D3DXMATRIX Rx, Ry;

    // 绕 X 轴旋转
    D3DXMatrixRotationX(&Rx, D3DX_PI / 4.0f);

    // 绕 Y 轴旋转
    static float y = 0.0f;
    D3DXMatrixRotationY(&Ry, y);
    y += timeDelta;

    // Y 轴旋转度数超过 360 度则复位
    if (y >= 2 * D3DX_PI)
        y = 0.0f;

    // 组合变化矩阵, 应用世界变换 (world transform)
    D3DXMATRIX p = Rx * Ry;
    d3ddev_->SetTransform(D3DTS_WORLD, &p);

    // 擦除背景, z-buffer=1.0, 背景白色
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);

    // 绘制场景
    d3ddev_->BeginScene();

    d3ddev_->SetStreamSource(0, vertex_buf_, 0, sizeof(Vertex));
    d3ddev_->SetIndices(index_buf_);
    d3ddev_->SetFVF(Vertex::FVF);

    // 绘制立方体, 12 个三角形图元
    d3ddev_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
