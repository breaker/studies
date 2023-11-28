// ClearBk.cpp
//

#include "StdAfx.h"
#include "InitD3D1.h"
#include "ClearBk.h"

void ClearBk::render(float timeDelta)
{
    // z-buffer=1.0
    HRESULT hr = d3ddev_->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::Clear", hr); return);

    // flip
    hr = d3ddev_->Present(NULL, NULL, NULL, NULL);
    SGL_FAILED_DO(hr, SGL_DXTRACE1("IDirect3DDevice9::Present", hr); return);
}
