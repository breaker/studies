// RenderID3DXFont.h
//

#ifndef __RENDERID3DXFONT_H__
#define __RENDERID3DXFONT_H__

////////////////////////////////////////////////////////////////////////////////
// FVF ��������
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
// �������� Cube
////////////////////////////////////////////////////////////////////////////////

class Cube {
public:
    Cube(IDirect3DDevice9* dev);
    BOOL Draw(D3DXMATRIX* world, const D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);

private:
    IDirect3DDevice9*               m_D3DDev;
    CComPtr<IDirect3DVertexBuffer9> m_VBuf;
    CComPtr<IDirect3DIndexBuffer9>  m_IBuf;
};

////////////////////////////////////////////////////////////////////////////////
// ��Ⱦ�� RenderID3DXFont
////////////////////////////////////////////////////////////////////////////////

class RenderID3DXFont : public SGL::Render {
public:
    RenderID3DXFont() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);

    virtual void Draw(float timeDelta);

private:
    std::auto_ptr<Cube>         m_Cube;
    CComPtr<IDirect3DTexture9>  m_CrateTex;
    CComPtr<ID3DXFont>          m_Font;

    static LPCTSTR CRATE_TEXTURE_FILE;
};

////////////////////////////////////////////////////////////////////////////////
// ���봦���� ID3DXFontInput
////////////////////////////////////////////////////////////////////////////////

class ID3DXFontInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERID3DXFONT_H__
