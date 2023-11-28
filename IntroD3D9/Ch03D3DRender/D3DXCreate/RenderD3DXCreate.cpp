// RenderD3DXCreate.cpp
//

#include "StdAfx.h"
#include "D3DXCreate.h"
#include "RenderD3DXCreate.h"

////////////////////////////////////////////////////////////////////////////////
// 绘制多个物体类 RenderD3DXCreate
////////////////////////////////////////////////////////////////////////////////

RenderD3DXCreate::RenderD3DXCreate(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    initMesh();
    initCamera();
    initRender();
}

// 初始化多个物体网格
BOOL RenderD3DXCreate::initMesh()
{
    ID3DXMesh* obj;

    D3DXCreateTeapot(d3ddev_, &obj, NULL);
    objs_[0].Attach(obj);

    D3DXCreateBox(d3ddev_, 2.0f, 2.0f, 2.0f, &obj, NULL);
    objs_[1].Attach(obj);

    // 圆柱体以 Z 轴对齐
    D3DXCreateCylinder(d3ddev_, 1.0f, 1.0f, 3.0f, 10, 10, &obj, NULL);
    objs_[2].Attach(obj);

    D3DXCreateTorus(d3ddev_, 1.0f, 3.0f, 10, 10, &obj, NULL);
    objs_[3].Attach(obj);

    D3DXCreateSphere(d3ddev_, 1.0f, 10, 10, &obj, NULL);
    objs_[4].Attach(obj);

    // 因为绘制静止物体, 所以预先计算好世界坐标变换矩阵
    D3DXMatrixTranslation(&objWorldM_[0],  0.0f, 0.0f,  0.0f);
    D3DXMatrixTranslation(&objWorldM_[1], -5.0f, 0.0f,  5.0f);
    D3DXMatrixTranslation(&objWorldM_[2],  5.0f, 0.0f,  5.0f);
    D3DXMatrixTranslation(&objWorldM_[3], -5.0f, 0.0f, -5.0f);
    D3DXMatrixTranslation(&objWorldM_[4],  5.0f, 0.0f, -5.0f);

    return TRUE;
}

// 初始化摄像机
BOOL RenderD3DXCreate::initCamera()
{
    // 设置投影 (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
    return TRUE;
}

BOOL RenderD3DXCreate::initRender()
{
    // 设置绘制状态, 线框模式 (wireframe)
    d3ddev_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    return TRUE;
}

void RenderD3DXCreate::setCamera(float timeDelta)
{
    // 移动摄像机, 摄像机将围绕场景中心旋转, 使用 sin 和 cos 计算轨道, 然后 10 倍半径距离, 并且摄像机将上下移动
    static float angle = (3.0f * D3DX_PI) / 2.0f;
    static float cameraHeight = 0.0f;
    static float cameraHeightDirection = 5.0f;

    // 设置取景变换 (view space transform)
    D3DXVECTOR3 pos(cosf(angle) * 10.0f, cameraHeight, sinf(angle) * 10.0f);

    // 摄像机以世界坐标原点为目标
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    d3ddev_->SetTransform(D3DTS_VIEW, &V);

    // 旋转
    angle += timeDelta;
    if (angle >= 2 * D3DX_PI)
        angle = 0.0f;

    // 计算下帧摄像机高度
    cameraHeight += cameraHeightDirection * timeDelta;
    if (cameraHeight >= 10.0f)
        cameraHeightDirection = -5.0f;
    if (cameraHeight <= -10.0f)
        cameraHeightDirection = 5.0f;
}

void RenderD3DXCreate::render(float timeDelta)
{
    setCamera(timeDelta);

    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);

    // 绘制场景
    d3ddev_->BeginScene();

    // 绘制物体
    for (int i = 0; i < 5; i++) {
        d3ddev_->SetTransform(D3DTS_WORLD, &objWorldM_[i]);
        objs_[i]->DrawSubset(0);
    }

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
