// RenderProgMesh.h
//

// 说明:
// Demonstrates how to use the progressive mesh interface (ID3DXPMesh).  Use
// the 'A' key to add triangles, use the 'S' key to remove triangles.  Note
// that we outline the triangles in yellow so that you can see them get 
// removed and added.

#ifndef __RENDERPROGMESH_H__
#define __RENDERPROGMESH_H__

////////////////////////////////////////////////////////////////////////////////
// FVF 顶点类型
////////////////////////////////////////////////////////////////////////////////

struct Vertex {
    Vertex(float x0, float y0, float z0, float nx0, float ny0, float nz0, float u0, float v0)
        : x(x0), y(y0), z(z0), nx(nx0), ny(ny0), nz(nz0), u(u0), v(v0)
    {}

    // D3DFVF_XYZ, D3DFVF_NORMAL, D3DFVF_TEX1
    void Dump(FILE* stream, DWORD FVF) {
        if (FVF & D3DFVF_XYZ)
            _ftprintf_s(stream, _T(" xyz = (%f, %f, %f)"), x, y, z);
        if (FVF & D3DFVF_NORMAL)
            _ftprintf_s(stream, _T(" normal = (%f, %f)"), nx, ny);
        if (FVF & D3DFVF_TEX1)
            _ftprintf_s(stream, _T(" tex1 = (%f, %f)"), u, v);
        _fputts(_T("\n"), stream);
    }

    static void DumpFVF(FILE* stream, DWORD FVF) {
        _fputts(_T("FVF:"), stream);
        if (FVF & D3DFVF_XYZ)
            _fputts(_T(" D3DFVF_XYZ"), stream);
        if (FVF & D3DFVF_NORMAL)
            _fputts(_T(" D3DFVF_NORMAL"), stream);
        if (FVF & D3DFVF_TEX1)
            _fputts(_T(" D3DFVF_TEX1"), stream);
        _fputts(_T("\n"), stream);
    }

    static size_t Size(DWORD FVF) {
        size_t sz = 0;
        // 假设按 sizeof(float) 字节对齐. 在 x86/Windows 上如是.
        if (FVF & D3DFVF_XYZ)
            sz += 3 * sizeof(float);
        if (FVF & D3DFVF_NORMAL)
            sz += 3 * sizeof(float);
        if (FVF & D3DFVF_TEX1)
            sz += 2 * sizeof(float);
        return sz;
    }

    // sz MUST be Size()
    static Vertex* Data(Vertex* arr, size_t i, size_t sz) {
        return (Vertex*) ((char*) arr + i * sz);
    }

    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderProgMesh
////////////////////////////////////////////////////////////////////////////////

class RenderProgMesh : public SGL::Render {
public:
    RenderProgMesh() {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    BOOL LoadXFile(LPCTSTR fname);
    void DumpMesh(LPCTSTR fname, ID3DXBaseMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont>          m_Font;
    std::vector<D3DMATERIAL9>   m_Mtrls;
    std::vector<CComPtr<IDirect3DTexture9> > m_Tex;

    static LPCTSTR MeshDumpFile;

public:
    CComPtr<ID3DXPMesh> m_Mesh;
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

    void DumpVertex(ID3DXBaseMesh* mesh);
    void DumpIndex(ID3DXBaseMesh* mesh);
    void DumpAttributeBuffer(ID3DXMesh* mesh);
    void DumpAdjacencyBuffer(ID3DXBaseMesh* mesh);
    void DumpAttributeTable(ID3DXBaseMesh* mesh);

private:
    FILE* m_DumpStream;
};

////////////////////////////////////////////////////////////////////////////////
// 输入处理类 ProgMeshInput
////////////////////////////////////////////////////////////////////////////////

class ProgMeshInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERPROGMESH_H__
