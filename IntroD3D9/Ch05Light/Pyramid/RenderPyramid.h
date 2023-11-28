// RenderPyramid.h
//

#ifndef __RENDERPYRAMID_H__
#define __RENDERPYRAMID_H__

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
// 绘制金字塔类 RenderPyramid
////////////////////////////////////////////////////////////////////////////////

class RenderPyramid : public SGLRender {
public:
    RenderPyramid(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initPyramid();
    BOOL initCamera();
    BOOL initRender();
    BOOL initMaterial();
    BOOL initLight();

private:
    CComPtr<IDirect3DVertexBuffer9> pyramid_;
};

#endif  // __RENDERPYRAMID_H__
