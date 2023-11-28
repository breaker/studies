// RenderToon.h
//

// 说明:
// 测试 VS 卡通渲染
// Deomstrates toon rendering using a vertex shader.  Note that you
// will have to switch to the REF device to view this sample if your
// graphics card does not support vertex shaders.  Or you
// can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderToon
////////////////////////////////////////////////////////////////////////////////

class RenderToon : public SGL::Render {
public:
    RenderToon() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR  VertexShaderFile;
    static LPCTSTR  ToonShadeTexFile;

    CComPtr<ID3DXMesh>              m_Meshes[4];
    D3DXMATRIX                      m_WorldMatrices[4];
    D3DXVECTOR4                     m_MeshColors[4];
    CComPtr<ID3DXConstantTable>     m_ToonConstTbl;
    CComPtr<IDirect3DVertexShader9> m_ToonShader;
    CComPtr<IDirect3DTexture9>      m_ShadeTex;

    D3DXHANDLE  m_WorldViewHandle;
    D3DXHANDLE  m_WorldViewProjHandle;
    D3DXHANDLE  m_ColorHandle;
    D3DXHANDLE  m_LightDirHandle;

    D3DXMATRIX  m_ProjMatrix;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 ToonInput
////////////////////////////////////////////////////////////////////////////////

class ToonInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
