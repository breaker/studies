// RenderShadow.h
//

// 说明:
// Use the arrow keys and the 'A' and 'S' key to navigate the scene and translate the teapot.
//

#ifndef __RENDERSHADOW_H__
#define __RENDERSHADOW_H__

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
// 渲染类 RenderShadow
////////////////////////////////////////////////////////////////////////////////

class RenderShadow : public SGL::Render {
public:
    RenderShadow() : m_TeapotPos(0.0f, 3.0f, -7.5f), m_ViewRadius(20.0f), m_ViewAngle(3.0f * D3DX_PI / 2.0f) {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void Display(float timeDelta);
    void DrawScene();
    void DrawShadow();

private:
    CComPtr<ID3DXMesh> m_Teapot;

    static D3DMATERIAL9 FloorMtrl;
    static D3DMATERIAL9 WallMtrl;
    static D3DMATERIAL9 MirrorMtrl;
    static D3DMATERIAL9 TeapotMtrl;

    CComPtr<IDirect3DTexture9>  m_FloorTex;
    CComPtr<IDirect3DTexture9>  m_WallTex;
    CComPtr<IDirect3DTexture9>  m_MirrorTex;

    CComPtr<IDirect3DVertexBuffer9> m_BackVB;   // 墙, 镜子和地板的背景场景

    static LPCTSTR TEX_FLOOR;
    static LPCTSTR TEX_WALL;
    static LPCTSTR TEX_MIRROR;

public:
    D3DXVECTOR3 m_TeapotPos;
    float       m_ViewRadius;
    float       m_ViewAngle;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 ShadowInput
////////////////////////////////////////////////////////////////////////////////

class ShadowInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERSHADOW_H__
