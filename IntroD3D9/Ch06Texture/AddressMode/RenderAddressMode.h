// RenderAddressMode.h
//

#ifndef __RENDERADDRESSMODE_H__
#define __RENDERADDRESSMODE_H__

// FVF 顶点类型, 只包含位置
struct Vertex {
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
        : x_(x), y_(y), z_(z), nx_(nx), ny_(ny), nz_(nz), u_(u), v_(v) {}
    float x_, y_, z_;
    float nx_, ny_, nz_;
    float u_, v_;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

////////////////////////////////////////////////////////////////////////////////
// 绘制物体类 RenderAddressMode
////////////////////////////////////////////////////////////////////////////////

class RenderAddressMode : public SGLRender {
public:
    RenderAddressMode(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    CComPtr<IDirect3DVertexBuffer9> quad_;
    CComPtr<IDirect3DTexture9>      tex_;
    static LPCTSTR TEXTURE_FILE;
};

#endif  // __RENDERADDRESSMODE_H__
