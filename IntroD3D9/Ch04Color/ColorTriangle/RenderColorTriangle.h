// RenderColorTriangle.h
//

#ifndef __RENDERCOLORTRIANGLE_H__
#define __RENDERCOLORTRIANGLE_H__

// FVF 顶点类型, 只包含位置
struct Vertex {
    Vertex(float x, float y, float z, D3DCOLOR clr) : x_(x), y_(y), z_(z), color_(clr) {}
    float x_;
    float y_;
    float z_;
    D3DCOLOR color_;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制三角形类 RenderColorTriangle
////////////////////////////////////////////////////////////////////////////////

class RenderColorTriangle : public SGLRender {
public:
    RenderColorTriangle(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL init_vertex();
    BOOL init_camera();
    BOOL init_render();

private:
    CComPtr<IDirect3DVertexBuffer9> colorTriangle_;
    D3DXMATRIX                      leftWorldM_;
    D3DXMATRIX                      rightWorldM_;
};


#endif  // __RENDERCOLORTRIANGLE_H__
