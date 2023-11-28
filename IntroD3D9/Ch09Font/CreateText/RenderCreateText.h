// RenderCreateText.h
//

#ifndef __RENDERCREATETEXT_H__
#define __RENDERCREATETEXT_H__

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
// 渲染类 RenderCreateText
////////////////////////////////////////////////////////////////////////////////

class RenderCreateText : public SGL::Render {
public:
    RenderCreateText() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXMesh>  m_TextMesh;
    static LPCTSTR      STR_TEXT;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 CreateTextInput
////////////////////////////////////////////////////////////////////////////////

class CreateTextInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERCREATETEXT_H__
