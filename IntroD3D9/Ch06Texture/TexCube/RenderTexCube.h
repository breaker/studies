// RenderTexCube.h
//

#ifndef __RENDERTEXCUBE_H__
#define __RENDERTEXCUBE_H__

////////////////////////////////////////////////////////////////////////////////
// FVF 顶点类型
////////////////////////////////////////////////////////////////////////////////

struct Vertex {
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
        : x_(x), y_(y), z_(z), nx_(nx), ny_(ny), nz_(nz), u_(u), v_(v)
    {}

    float x_;
    float y_;
    float z_;
    float nx_;
    float ny_;
    float nz_;
    float u_;
    float v_;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

////////////////////////////////////////////////////////////////////////////////
// 立方体类 Cube
////////////////////////////////////////////////////////////////////////////////

class Cube {
public:
    Cube(IDirect3DDevice9* dev);

    void draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);

private:
    IDirect3DDevice9*               d3ddev_;
    CComPtr<IDirect3DVertexBuffer9> vb_;
    CComPtr<IDirect3DIndexBuffer9>  ib_;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染异常类 RenderTexCubeErr
////////////////////////////////////////////////////////////////////////////////

class RenderTexCubeErr : public SGL::SGLRenderExcept {
public:
    explicit RenderTexCubeErr(DWORD err = SGL::SGL_ERR::ERR_UNKNOWN, const char* msg = "") : SGLRenderExcept(err, msg) {}
    explicit RenderTexCubeErr(const char* msg) : SGLRenderExcept(msg) {}

    static const char* ERR_CREATE_TEXTURE;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderTexCube
////////////////////////////////////////////////////////////////////////////////

class RenderTexCube : public SGLRender {
public:
    RenderTexCube(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    auto_ptr<Cube>              cube_;
    CComPtr<IDirect3DTexture9>  tex_;
    static LPCTSTR              TEXTURE_FILE;
};

#endif  // __RENDERTEXCUBE_H__
