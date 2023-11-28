// RenderTexQuad.h
//

#ifndef __RENDERTEXQUAD_H__
#define __RENDERTEXQUAD_H__

////////////////////////////////////////////////////////////////////////////////
// FVF 顶点类型
////////////////////////////////////////////////////////////////////////////////

struct Vertex {
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
        : x_(x), y_(y), z_(z), nx_(nx), ny_(ny), nz_(nz), u_(u), v_(v) {}
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
// 渲染类 RenderTexQuad
////////////////////////////////////////////////////////////////////////////////

class RenderTexQuad : public SGLRender {
public:
    RenderTexQuad(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void render(float timeDelta);

private:
    CComPtr<IDirect3DVertexBuffer9> quad_;
    CComPtr<IDirect3DTexture9>      tex_;
    static LPCTSTR                  TEXTURE_FILE;
};

#endif  // __RENDERTEXQUAD_H__
