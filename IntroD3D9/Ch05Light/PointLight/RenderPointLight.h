// RenderPointLight.h
//

#ifndef __RENDERPOINTLIGHT_H__
#define __RENDERPOINTLIGHT_H__

// FVF ��������, ֻ����λ��
struct Vertex {
    Vertex(float x, float y, float z, float nx, float ny, float nz) : x_(x), y_(y), z_(z), nx_(nx), ny_(ny), nz_(nz) {}
    float x_;
    float y_;
    float z_;
    float nx_;
    float ny_;
    float nz_;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// ���������� RenderPointLight
////////////////////////////////////////////////////////////////////////////////

class RenderPointLight : public SGLRender {
public:
    RenderPointLight(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initMesh();
    BOOL initCamera();
    BOOL initRender();
    BOOL initMaterial();
    BOOL initPointLight();

private:
    CComPtr<ID3DXMesh>  objects_[4];
    D3DXMATRIX          worldM_[4];
    D3DMATERIAL9        mtrl_[4];
};

#endif  // __RENDERPOINTLIGHT_H__
