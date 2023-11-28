// SGLRender.cpp
//

#include "StdAfx.h"
#include "SGL.h"
#include <SGL/SGLRender.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�쳣�� SGLRenderExcept
////////////////////////////////////////////////////////////////////////////////

const char* SGLRenderExcept::ERR_CREATE_D3DDEV_FAILED = "create D3D device failed";

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�� SGLRender
////////////////////////////////////////////////////////////////////////////////

SGLRender::SGLRender(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType, D3DFORMAT bkBufFmt/* = D3DFMT_A8R8G8B8*/, D3DMULTISAMPLE_TYPE sampleType/* = D3DMULTISAMPLE_NONE*/, D3DFORMAT depthStencilFmt/* = D3DFMT_D24S8*/)
{
    if (!createDev(width, height, hwnd, windowed, devType, bkBufFmt, sampleType, depthStencilFmt))
        throw SGLRenderExcept(SGLRenderExcept::ERR_CREATE_D3DDEV_FAILED);
}

BOOL SGLRender::createDev(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType, D3DFORMAT bkBufFmt, D3DMULTISAMPLE_TYPE sampleType, D3DFORMAT depthStencilFmt)
{
    // ���� IDirect3D9 �ӿ�
    CComPtr<IDirect3D9> d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    SGL_TRUE_DO(d3d9.p == NULL, SGL_DXTRACE0("Direct3DCreate9"); return FALSE);

    // ���Ӳ�����㴦��
    D3DCAPS9 caps;
    HRESULT hr = d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, devType, &caps);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3D9::GetDeviceCaps", hr); return FALSE);
    DWORD devFlag = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING);

    // ���� D3D �豸
    width_ = width;
    height_ = height;
    D3DPRESENT_PARAMETERS d3dpp;
    d3dpp.BackBufferWidth            = width;
    d3dpp.BackBufferHeight           = height;
    d3dpp.BackBufferFormat           = bkBufFmt;
    d3dpp.BackBufferCount            = 1;
    d3dpp.MultiSampleType            = sampleType;
    d3dpp.MultiSampleQuality         = 0;
    d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow              = hwnd;
    d3dpp.Windowed                   = windowed;
    d3dpp.EnableAutoDepthStencil     = TRUE;
    d3dpp.AutoDepthStencilFormat     = depthStencilFmt;
    d3dpp.Flags                      = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

    IDirect3DDevice9* d3ddev;
    hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, devType, hwnd, devFlag, &d3dpp, &d3ddev);

    // ���ʧ��, ���� 16bit z-buffer
    if (FAILED(hr)) {
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, devType, hwnd, devFlag, &d3dpp, &d3ddev);
        SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3D9::CreateDevice", hr); return FALSE);
    }
    d3ddev_.Attach(d3ddev);

    return TRUE;
}

_SGL_NS_END
