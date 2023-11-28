// RenderToon2.h
//

// 说明:
// 测试 VS 卡通渲染, 带轮廓外框线 (Outline)
// Deomstrates toon2 rendering using a vertex shader.  Note that you
// will have to switch to the REF device to view this sample if your
// graphics card does not support vertex shaders.  Or you
// can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderToon2
////////////////////////////////////////////////////////////////////////////////

class RenderToon2 : public SGL::Render {
public:
    RenderToon2() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR  ToonVSFile;
    static LPCTSTR  OutlineVSFile;

    static LPCTSTR  ToonShadeTexFile;

    std::auto_ptr<SGL::SilhouetteEdges> m_MeshOutlines[4];
    CComPtr<ID3DXMesh>              m_Meshes[4];
    D3DXMATRIX                      m_WorldMatrices[4];
    D3DXVECTOR4                     m_MeshColors[4];

    CComPtr<ID3DXConstantTable>     m_ToonConstTable;
    CComPtr<IDirect3DVertexShader9> m_ToonShader;
    CComPtr<IDirect3DTexture9>      m_ShadeTex;

    CComPtr<ID3DXConstantTable>     m_OutlineConstTable;
    CComPtr<IDirect3DVertexShader9> m_OutlineShader;

    D3DXHANDLE  m_ToonWorldViewHandle;
    D3DXHANDLE  m_ToonWorldViewProjHandle;
    D3DXHANDLE  m_ToonColorHandle;
    D3DXHANDLE  m_ToonLightDirHandle;

    D3DXHANDLE  m_OutlineWorldViewHandle;
    D3DXHANDLE  m_OutlineProjHandle;

    D3DXMATRIX  m_ProjMatrix;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Toon2Input
////////////////////////////////////////////////////////////////////////////////

class Toon2Input : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
