// RenderMultiTex.h
//

// 说明:
// 测试 PS 的多重纹理功能
// Deomstrates multi-texturing using a pixel shader.  You will have
// to switch to the REF device to run this sample if your hardware
// doesn't support pixel shaders.

#pragma once

////////////////////////////////////////////////////////////////////////////////
// 顶点格式 MultiTexVertex
////////////////////////////////////////////////////////////////////////////////

struct MultiTexVertex {
    MultiTexVertex(float x, float y, float z, float u0, float v0, float u1, float v1, float u2, float v2) :
        _x(x), _y(y), _z(z), _u0(u0), _v0(v0), _u1(u1), _v1(v1), _u2(u2), _v2(v2) {}

    float _x, _y, _z;
    float _u0, _v0;
    float _u1, _v1;
    float _u2, _v2;

    static const DWORD FVF;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderMultiTex
////////////////////////////////////////////////////////////////////////////////

class RenderMultiTex : public SGL::Render {
public:
    RenderMultiTex() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    void DumpMesh(LPCTSTR fname, ID3DXMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont> m_Font;

    static LPCTSTR MultiTexPSFile;
    static LPCTSTR BaseTexFile;
    static LPCTSTR SpotLightTexFile;
    static LPCTSTR StringTexFile;

    CComPtr<ID3DXConstantTable>     m_MultiTexCT;
    CComPtr<IDirect3DVertexBuffer9> m_QuadVB;
    CComPtr<IDirect3DPixelShader9>  m_MultiTexPS;

    CComPtr<IDirect3DTexture9>  m_BaseTex;
    CComPtr<IDirect3DTexture9>  m_SpotLightTex;
    CComPtr<IDirect3DTexture9>  m_StringTex;

    D3DXHANDLE  m_BaseTexHandle;
    D3DXHANDLE  m_SpotLightTexHandle;
    D3DXHANDLE  m_StringTexHandle;

    D3DXCONSTANT_DESC   m_BaseTexDesc;
    D3DXCONSTANT_DESC   m_SpotLightTexDesc;
    D3DXCONSTANT_DESC   m_StringTexDesc;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 MultiTexInput
////////////////////////////////////////////////////////////////////////////////

class MultiTexInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
