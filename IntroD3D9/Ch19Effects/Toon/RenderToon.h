// RenderToon.h
//

// 说明:
// 测试使用 VS 效果文件的卡通渲染
// Deomstrates toon rendering using a vertex shader in an effect file.
// You will need to switch to the REF device to run this sample if your
// card does not support shaders.  Or you
// can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 顶点格式 ToonVertex
////////////////////////////////////////////////////////////////////////////////

struct ToonVertex {
    ToonVertex(float x, float y, float z, float u0, float v0, float u1, float v1, float u2, float v2) :
        _x(x), _y(y), _z(z), _u0(u0), _v0(v0), _u1(u1), _v1(v1), _u2(u2), _v2(v2) {}

    float _x, _y, _z;
    float _u0, _v0;
    float _u1, _v1;
    float _u2, _v2;

    static const DWORD FVF;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderToon
////////////////////////////////////////////////////////////////////////////////

class RenderToon : public SGL::Render {
public:
    RenderToon() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    void DumpMesh(LPCTSTR fname, ID3DXMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR ToonEffectFile;
    static LPCTSTR ToonShadeTexFile;

    CComPtr<ID3DXMesh>      m_Meshes[4];
    CComPtr<ID3DXEffect>    m_ToonEffect;

    D3DXMATRIX  m_WorldMatrices[4];
    D3DXVECTOR4 m_MeshColors[4];

    D3DXHANDLE  m_WorldViewHandle;
    D3DXHANDLE  m_WorldViewProjHandle;
    D3DXHANDLE  m_ColorHandle;
    D3DXHANDLE  m_LightDirHandle;
    D3DXHANDLE  m_ShadeTexHandle;
    D3DXHANDLE  m_ToonTechHandle;

    D3DXMATRIX  m_ProjMatrix;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 ToonInput
////////////////////////////////////////////////////////////////////////////////

class ToonInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
