// RenderTeapot.cpp
//

#include "StdAfx.h"
#include "Teapot.h"
#include "RenderTeapot.h"

////////////////////////////////////////////////////////////////////////////////
// ���Ʋ���� RenderTeapot
////////////////////////////////////////////////////////////////////////////////

RenderTeapot::RenderTeapot(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    initMesh();
    initCamera();
}

// ��ʼ���������
BOOL RenderTeapot::initMesh()
{
    ID3DXMesh* teapot;
    D3DXCreateTeapot(d3ddev_, &teapot, NULL);
    teapot_.Attach(teapot);
    return TRUE;
}

// ��ʼ�������
BOOL RenderTeapot::initCamera()
{
    // ����ȡ���任 (view space transform)
    D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    d3ddev_->SetTransform(D3DTS_VIEW, &V);

    // ����ͶӰ (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);

    // ���û���״̬, �߿�ģʽ (wireframe)
    d3ddev_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    return TRUE;
}

void RenderTeapot::render(float timeDelta)
{
    D3DXMATRIX Ry;

    // �� Y ����ת
    static float y = 0.0f;
    D3DXMatrixRotationY(&Ry, y);
    y += timeDelta;

    // Y ����ת�������� 360 ����λ
    if( y >= 2 * D3DX_PI )
        y = 0.0f;

    // ����任
    d3ddev_->SetTransform(D3DTS_WORLD, &Ry);

    // ��������, z-buffer=1.0, ������ɫ
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff), 1.0f, 0);

    // ���Ƴ���
    d3ddev_->BeginScene();

    // ���Ʋ��
    teapot_->DrawSubset(0);

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
