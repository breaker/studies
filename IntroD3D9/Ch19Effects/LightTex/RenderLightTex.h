// RenderLightTex.h
//

// 说明:
// 使用效果文件对 3D 模型进行照明和纹理贴图
// Deomstrates using an effect file to light and texture a 3D model.
// Use the arrow keys to rotate.

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderLightTex
////////////////////////////////////////////////////////////////////////////////

class RenderLightTex : public SGL::Render {
public:
    RenderLightTex() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    void DumpMesh(LPCTSTR fname, ID3DXMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR LightTexEffectFile;
    static LPCTSTR ModelFile;
    static LPCTSTR TexFile;

    CComPtr<ID3DXMesh>          m_Mesh;
    std::vector<D3DMATERIAL9>   m_Mtrls;
    std::vector<CComPtr<IDirect3DTexture9> > m_Textures;

    CComPtr<ID3DXEffect> m_LightTexEffect;

    D3DXHANDLE  m_WorldMatrixHandle;
    D3DXHANDLE  m_ViewMatrixHandle;
    D3DXHANDLE  m_ProjMatrixHandle;
    D3DXHANDLE  m_TexHandle;

    D3DXHANDLE  m_LightTexTechHandle;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 LightTexInput
////////////////////////////////////////////////////////////////////////////////

class LightTexInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
