// RenderPointLight.cpp
//

#include "StdAfx.h"
#include "PointLight.h"
#include "RenderPointLight.h"

////////////////////////////////////////////////////////////////////////////////
// ���������� RenderPointLight
////////////////////////////////////////////////////////////////////////////////

RenderPointLight::RenderPointLight(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    initMesh();
    initMaterial();
    initPointLight();
    initCamera();
    initRender();
}

BOOL RenderPointLight::initRender()
{
    d3ddev_->SetRenderState(D3DRS_LIGHTING, TRUE);          // Ĭ�Ϲ���Ҳ�Ǵ򿪵�
    d3ddev_->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);  // renormalize ������
    d3ddev_->SetRenderState(D3DRS_SPECULARENABLE, FALSE);   // �رվ������, ����ȽϺ���Դ
    return TRUE;
}

// ��ʼ����������
BOOL RenderPointLight::initMesh()
{
    ID3DXMesh* obj;
    D3DXCreateTeapot(d3ddev_, &obj, NULL);
    objects_[0].Attach(obj);

    D3DXCreateSphere(d3ddev_, 1.0f, 20, 20, &obj, NULL);
    objects_[1].Attach(obj);

    D3DXCreateTorus(d3ddev_, 0.5f, 1.0f, 20, 20, &obj, NULL);
    objects_[2].Attach(obj);

    D3DXCreateCylinder(d3ddev_, 0.5f, 0.5f, 2.0f, 20, 20, &obj, NULL);
    objects_[3].Attach(obj);

    // ��������仯����
    D3DXMatrixTranslation(&worldM_[0],  0.0f,  2.0f, 0.0f);
    D3DXMatrixTranslation(&worldM_[1],  0.0f, -2.0f, 0.0f);
    D3DXMatrixTranslation(&worldM_[2], -3.0f,  0.0f, 0.0f);
    D3DXMatrixTranslation(&worldM_[3],  3.0f,  0.0f, 0.0f);
    return TRUE;
}

// ��ʼ������
BOOL RenderPointLight::initMaterial()
{
    mtrl_[0] = SGL::RED_MTRL;
    mtrl_[1] = SGL::BLUE_MTRL;
    mtrl_[2] = SGL::GREEN_MTRL;
    mtrl_[3] = SGL::YELLOW_MTRL;
    return TRUE;
}

// ��ʼ������
BOOL RenderPointLight::initPointLight()
{
    D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
    D3DXCOLOR clr = SGL::WHITE;
    D3DLIGHT9 ptLight;;
    SGL::InitPointLight(ptLight, pos, clr);

    d3ddev_->SetLight(0, &ptLight);
    d3ddev_->LightEnable(0, TRUE);
    return TRUE;
}

// ��ʼ�������
BOOL RenderPointLight::initCamera()
{
    // ����ͶӰ (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.25f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
    return TRUE;
}

void RenderPointLight::render(float timeDelta)
{
    // ����ȡ���任 (view space transform)
    static float angle  = (3.0f * D3DX_PI) / 2.0f;
    static float height = 5.0f;

    if (SGL::KEY_DOWN(VK_LEFT))
        angle -= 0.5f * timeDelta;
    else if (SGL::KEY_DOWN(VK_RIGHT))
        angle += 0.5f * timeDelta;
    else if (SGL::KEY_DOWN(VK_UP))
        height += 5.0f * timeDelta;
    else if (SGL::KEY_DOWN(VK_DOWN))
        height -= 5.0f * timeDelta;

    D3DXVECTOR3 pos(cosf(angle) * 7.0f, height, sinf(angle) * 7.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    d3ddev_->SetTransform(D3DTS_VIEW, &V);

    // ��������, z-buffer=1.0, ������ɫ
    d3ddev_->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

    // ���Ƴ���
    d3ddev_->BeginScene();

    // ��������
    for (int i = 0; i < 4; i++) {
        d3ddev_->SetMaterial(&mtrl_[i]);                    // ÿ����Ⱦǰ��Ҫ���ò���
        d3ddev_->SetTransform(D3DTS_WORLD, &worldM_[i]);    // ����任 (world transform)
        objects_[i]->DrawSubset(0);
    }

    d3ddev_->EndScene();
    d3ddev_->Present(NULL, NULL, NULL, NULL);
}
