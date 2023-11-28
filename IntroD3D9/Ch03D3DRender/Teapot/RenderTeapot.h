// RenderTeapot.h
//

#ifndef __RENDERTEAPOT_H__
#define __RENDERTEAPOT_H__

// FVF 顶点类型, 只包含位置
struct Vertex {
    Vertex(float x, float y, float z) : x_(x), y_(y), z_(z) {}
    float x_;
    float y_;
    float z_;
    static const DWORD FVF = D3DFVF_XYZ;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制茶壶类 RenderTeapot
////////////////////////////////////////////////////////////////////////////////

class RenderTeapot : public SGLRender {
public:
    RenderTeapot(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initMesh();
    BOOL initCamera();

private:
    CComPtr<ID3DXMesh>  teapot_;
};

#endif  // __RENDERTEAPOT_H__
