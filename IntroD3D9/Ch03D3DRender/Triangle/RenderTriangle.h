// RenderTriangle.h
//

#ifndef __RENDERTRIANGLE_H__
#define __RENDERTRIANGLE_H__

// FVF 顶点类型, 只包含位置
struct Vertex {
    Vertex(float x, float y, float z) : x_(x), y_(y), z_(z) {}
    float x_;
    float y_;
    float z_;
    static const DWORD FVF = D3DFVF_XYZ;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制三角形类 RenderTriangle
////////////////////////////////////////////////////////////////////////////////

class RenderTriangle : public SGLRender {
public:
    RenderTriangle(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    BOOL initVertex();
    BOOL initCamera();
    BOOL initRender();

private:
    CComPtr<IDirect3DVertexBuffer9>  triangle_;
};


#endif  // __RENDERTRIANGLE_H__
