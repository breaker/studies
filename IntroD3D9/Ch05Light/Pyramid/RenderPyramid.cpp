// RenderPyramid.cpp
//

#include "StdAfx.h"
#include "Pyramid.h"
#include "RenderPyramid.h"

////////////////////////////////////////////////////////////////////////////////
// 绘制金字塔类 RenderPyramid
////////////////////////////////////////////////////////////////////////////////

RenderPyramid::RenderPyramid(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    initPyramid();
    initMaterial();
    initLight();
    initCamera();
    initRender();
}

BOOL RenderPyramid::initRender()
{
    d3ddev_->SetRenderState(D3DRS_LIGHTING, TRUE);          // 默认光照也是打开的
    d3ddev_->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);  // renormalize 法向量
    d3ddev_->SetRenderState(D3DRS_SPECULARENABLE, TRUE);    // 打开镜面光照
    return TRUE;
}

// 初始化金字塔网格
BOOL RenderPyramid::initPyramid()
{
    IDirect3DVertexBuffer9* pyd;
    d3ddev_->CreateVertexBuffer(12 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &pyd, NULL);

    Vertex* vert;
    pyd->Lock(0, 0, (void**) &vert, 0);

    // front face
    vert[0] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
    vert[1] = Vertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, -0.707f);
    vert[2] = Vertex( 1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

    // left face
    vert[3] = Vertex(-1.0f, 0.0f,  1.0f, -0.707f, 0.707f, 0.0f);
    vert[4] = Vertex( 0.0f, 1.0f,  0.0f, -0.707f, 0.707f, 0.0f);
    vert[5] = Vertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);

    // right face
    vert[6] = Vertex( 1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
    vert[7] = Vertex( 0.0f, 1.0f,  0.0f, 0.707f, 0.707f, 0.0f);
    vert[8] = Vertex( 1.0f, 0.0f,  1.0f, 0.707f, 0.707f, 0.0f);

    // back face
    vert[9]  = Vertex( 1.0f, 0.0f,  1.0f, 0.0f, 0.707f, 0.707f);
    vert[10] = Vertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, 0.707f);
    vert[11] = Vertex(-1.0f, 0.0f,  1.0f, 0.0f, 0.707f, 0.707f);

    pyd->Unlock();

    pyramid_.Attach(pyd);
    return TRUE;
}

// 初始化材质
BOOL RenderPyramid::initMaterial()
{
    D3DMATERIAL9 mtrl;
    mtrl.Ambient    = SGL::WHITE;
    mtrl.Diffuse    = SGL::WHITE;
    mtrl.Specular   = SGL::WHITE;
    mtrl.Emissive   = SGL::BLACK;
    mtrl.Power      = 5.0f;
    d3ddev_->SetMaterial(&mtrl);
    return TRUE;
}

// 初始化光照
BOOL RenderPyramid::initLight()
{
    D3DLIGHT9 dirLight;
    ZeroMemory(&dirLight, sizeof(dirLight));
    dirLight.Type      = D3DLIGHT_DIRECTIONAL;
    dirLight.Diffuse   = SGL::WHITE;
    dirLight.Specular  = SGL::WHITE * 0.3f;
    dirLight.Ambient   = SGL::WHITE * 0.6f;
    dirLight.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

    d3ddev_->SetLight(0, &dirLight);
    d3ddev_->LightEnable(0, TRUE);
    return TRUE;
}

// 初始化摄像机
BOOL RenderPyramid::initCamera()
{
    // 设置取景变换 (view space transform)
    D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    d3ddev_->SetTransform(D3DTS_VIEW, &V);

    // 设置投影 (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);

    return TRUE;
}

void RenderPyramid::render(float timeDelta)
{
    // 世界变换
    D3DXMATRIX yRot;
    static float y = 0.0f;
    D3DXMatrixRotationY(&yRot, y);
    y += timeDelta;

    if (y >= 2 * D3DX_PI)
        y = 0.0f;

    d3ddev_->SetTransform(D3DTS_WORLD, &yRot);

    // 擦除背景, z-buffer=1.0, 背景黑色
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

    // 绘制场景
    d3ddev_->BeginScene();

    // 绘制金字塔
    d3ddev_->SetStreamSource(0, pyramid_, 0, sizeof(Vertex));
    d3ddev_->SetFVF(Vertex::FVF);
    d3ddev_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
