// RenderSpotLight.h
//

#ifndef __RENDERSPOTLIGHT_H__
#define __RENDERSPOTLIGHT_H__

// FVF 顶点类型, 只包含位置
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
// 绘制物体类 RenderSpotLight
////////////////////////////////////////////////////////////////////////////////

class RenderSpotLight : public SGLRender {
public:
    RenderSpotLight(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initMesh();
    BOOL initCamera();
    BOOL initRender();
    BOOL initMaterial();
    BOOL initSpotLight();

private:
    CComPtr<ID3DXMesh>  objects_[4];
    D3DXMATRIX          worldM_[4];
    D3DMATERIAL9        mtrl_[4];
    D3DLIGHT9           spotLight_;
};

#endif  // __RENDERSPOTLIGHT_H__
