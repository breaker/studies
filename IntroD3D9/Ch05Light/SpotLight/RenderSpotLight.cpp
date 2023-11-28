// RenderSpotLight.cpp
//

#include "StdAfx.h"
#include "SpotLight.h"
#include "RenderSpotLight.h"

////////////////////////////////////////////////////////////////////////////////
// ���������� RenderSpotLight
////////////////////////////////////////////////////////////////////////////////

RenderSpotLight::RenderSpotLight(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType)
    : SGLRender(width, height, hwnd, windowed, devType)
{
    initMesh();
    initMaterial();
    initSpotLight();
    initCamera();
    initRender();
}

BOOL RenderSpotLight::initRender()
{
    d3ddev_->SetRenderState(D3DRS_LIGHTING, TRUE);          // Ĭ�Ϲ���Ҳ�Ǵ򿪵�
    d3ddev_->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);  // renormalize ������
    d3ddev_->SetRenderState(D3DRS_SPECULARENABLE, TRUE);    // �����������, ����ȽϺ���Դ
    return TRUE;
}

// ��ʼ����������
BOOL RenderSpotLight::initMesh()
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
BOOL RenderSpotLight::initMaterial()
{
    mtrl_[0] = SGL::RED_MTRL;
    mtrl_[1] = SGL::BLUE_MTRL;
    mtrl_[2] = SGL::GREEN_MTRL;
    mtrl_[3] = SGL::YELLOW_MTRL;

    // ���þ�������
    for(int i = 0; i < 4; i++)
        mtrl_[i].Power = 20.0f;

    return TRUE;
}

// ��ʼ������
BOOL RenderSpotLight::initSpotLight()
{
    D3DXVECTOR3 pos(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 dir(0.0f, 0.0f, 1.0f);
    D3DXCOLOR clr = SGL::WHITE;
    SGL::InitSpotLight(spotLight_, pos, dir, clr);

    d3ddev_->SetLight(0, &spotLight_);
    d3ddev_->LightEnable(0, TRUE);
    return TRUE;
}

// ��ʼ�������
BOOL RenderSpotLight::initCamera()
{
    // ���������λ��
    D3DXVECTOR3 pos(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    d3ddev_->SetTransform(D3DTS_VIEW, &V);

    // ����ͶӰ (project)
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (FLOAT) width_ / (FLOAT) height_, 1.0f, 1000.0f);
    d3ddev_->SetTransform(D3DTS_PROJECTION, &proj);
    return TRUE;
}

void RenderSpotLight::render(float timeDelta)
{
    // ����ȡ���任 (view space transform)
    static float angle  = (3.0f * D3DX_PI) / 2.0f;

    // �ƶ���Դ
    if (SGL::KEY_DOWN(VK_LEFT))
        spotLight_.Direction.x -= 0.5f * timeDelta;
    else if (SGL::KEY_DOWN(VK_RIGHT))
        spotLight_.Direction.x += 0.5f * timeDelta;
    else if (SGL::KEY_DOWN(VK_UP))
        spotLight_.Direction.y += 0.5f * timeDelta;
    else if (SGL::KEY_DOWN(VK_DOWN))
        spotLight_.Direction.y -= 0.5f * timeDelta;

    // ���¹�Դ
    d3ddev_->SetLight(0, &spotLight_);
    d3ddev_->LightEnable(0, TRUE);

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
