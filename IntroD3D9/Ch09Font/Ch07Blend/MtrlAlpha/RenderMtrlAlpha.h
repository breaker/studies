// RenderMtrlAlpha.h
//

// 说明:
// 用 A 键变得不透明, S 键变得更透明
//

#ifndef __RENDERMTRLALPHA_H__
#define __RENDERMTRLALPHA_H__

////////////////////////////////////////////////////////////////////////////////
// FVF 顶点类型
////////////////////////////////////////////////////////////////////////////////

struct Vertex {
    Vertex(float x0, float y0, float z0, float nx0, float ny0, float nz0, float u0, float v0)
        : x(x0), y(y0), z(z0), nx(nx0), ny(ny0), nz(nz0), u(u0), v(v0)
    {}

    float x, y, z;
    float nx, ny, nz;
    float u, v;
    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderMtrlAlpha
////////////////////////////////////////////////////////////////////////////////

class RenderMtrlAlpha : public SGL::Render {
public:
    RenderMtrlAlpha() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void Display(float timeDelta);
    void IncAlpha();
    void DecAlpha();

private:
    CComPtr<ID3DXMesh>  m_Teapot;
    D3DMATERIAL9        m_TeapotMtrl;

    CComPtr<IDirect3DVertexBuffer9> m_BkGndQuad;
    CComPtr<IDirect3DTexture9>      m_BkGndTex;
    D3DMATERIAL9                    m_BkGndMtrl;

    static LPCTSTR TEXTURE_FILE;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 MtrlAlphaInput
////////////////////////////////////////////////////////////////////////////////

class MtrlAlphaInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERMTRLALPHA_H__
