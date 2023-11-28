// RenderDiffuse.h
//

// 说明:
// 测试 VS 的漫反射 (含环境光和漫反射光)
// Deomstrates diffuse lighting using a vertex shader.  You will
// have to switch to the REF device to run this sample if your
// hardware does not support shaders.  Or you
// can use software vertex processing: D3DCREATE_SOFTWARE_VERTEXPROCESSING.

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderDiffuse
////////////////////////////////////////////////////////////////////////////////

class RenderDiffuse : public SGL::Render {
public:
    RenderDiffuse() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR VertexShaderFile;

    CComPtr<ID3DXMesh>              m_Teapot;
    CComPtr<ID3DXConstantTable>     m_DiffuseConstTbl;
    CComPtr<IDirect3DVertexShader9> m_DiffuseShader;

    D3DXHANDLE  m_ViewMatrixHandle;
    D3DXHANDLE  m_ViewProjMatrixHandle;
    D3DXHANDLE  m_AmbientMtrlHandle;
    D3DXHANDLE  m_DiffuseMtrlHandle;
    D3DXHANDLE  m_LightDirHandle;

    D3DXMATRIX  m_ProjMatrix;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 DiffuseInput
////////////////////////////////////////////////////////////////////////////////

class DiffuseInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
