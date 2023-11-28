// Render.cpp
//

#include "StdAfx.h"
#include <SGL/Render.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�� Render
////////////////////////////////////////////////////////////////////////////////

HRESULT Render::Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType,
                     D3DFORMAT bkBufFmt, D3DMULTISAMPLE_TYPE sampleType, D3DFORMAT depthStencilFmt)
{
    // ���� IDirect3D9 �ӿ�
    CComPtr<IDirect3D9> d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    SGL_TRUE_DO(d3d9.p == NULL, return S_FALSE);

    // ���Ӳ�����㴦��
    D3DCAPS9 caps;
    HRESULT hr = d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, devType, &caps);
    SGL_FAILED_DO(hr, return hr);
    DWORD devFlag = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING);

    // ���� D3D �豸
    m_Width = width;
    m_Height = height;
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
        SGL_FAILED_DO(hr, return hr);
    }
    m_D3DDev.Attach(d3ddev);
    m_hWnd = hwnd;

    return S_OK;
}

SGL_NS_END
