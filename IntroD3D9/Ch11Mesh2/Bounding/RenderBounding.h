// RenderBounding.h
//

// ˵��:
// Demonstrates how to use D3DXComputeBoundingSphere and D3DXComputeBoundingBox.
// The spacebar key switches between rendering the mesh's bounding sphere and box.

#ifndef __RENDERBOUNDING_H__
#define __RENDERBOUNDING_H__

////////////////////////////////////////////////////////////////////////////////
// FVF ��������
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
        // ���谴 sizeof(float) �ֽڶ���. �� x86/Windows ������.
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
// ��Ⱦ�� RenderBounding
////////////////////////////////////////////////////////////////////////////////

class RenderBounding : public SGL::Render {
public:
    RenderBounding() : m_BoundingType(BOUNDING_SPHERE) {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    BOOL LoadXFile(LPCTSTR fname);
    void DumpMesh(LPCTSTR fname, ID3DXMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont>          m_Font;
    std::vector<D3DMATERIAL9>   m_Mtrls;
    std::vector<CComPtr<IDirect3DTexture9> > m_Tex;

    CComPtr<ID3DXMesh> m_Mesh;
    CComPtr<ID3DXMesh> m_SphereMesh;
    CComPtr<ID3DXMesh> m_BoxMesh;

    static LPCTSTR MeshDumpFile;

public:
    enum BOUNDING_TYPE {
        BOUNDING_SPHERE,
        BOUNDING_BOX
    };
    BOUNDING_TYPE m_BoundingType;
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
// ���봦���� BoundingInput
////////////////////////////////////////////////////////////////////////////////

class BoundingInput : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};

#endif  // __RENDERBOUNDING_H__
