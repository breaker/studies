// RenderMesh1.h
//

#ifndef __RENDERMESH1_H__
#define __RENDERMESH1_H__

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
// 渲染类 RenderMesh1
////////////////////////////////////////////////////////////////////////////////

class RenderMesh1 : public SGL::Render {
public:
    RenderMesh1() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont>          m_Font;
    static const int            NumSubsets = 3;
    CComPtr<IDirect3DTexture9>  m_CubeTex[NumSubsets];  // texture for each subset
    CComPtr<ID3DXMesh>          m_CubeMesh;

    static LPCTSTR TEXTURE_FILE[NumSubsets];
};

////////////////////////////////////////////////////////////////////////////////
// class MeshDump
////////////////////////////////////////////////////////////////////////////////

class MeshDump {
public:
    MeshDump() : m_DumpStream(NULL) {}
    ~MeshDump() {
        Clean();
    }

    BOOL Init(LPCTSTR fname);
    void Clean();

    void DumpVertex(ID3DXMesh* mesh);
    void DumpIndex(ID3DXMesh* mesh);
    void DumpAttributeBuffer(ID3DXMesh* mesh);
    void DumpAdjacencyBuffer(ID3DXMesh* mesh);
    void DumpAttributeTable(ID3DXMesh* mesh);

private:
    FILE* m_DumpStream;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 Mesh1Input
////////////////////////////////////////////////////////////////////////////////

class Mesh1Input : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERMESH1_H__
