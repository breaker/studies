// RenderTerrain2.h
//

// 说明:
// 渲染一张有高低的地图可以在其中行走
// W/S: 前后
// A/D: 偏航 (Yaw)
// UP/DOWN: 仰俯 (Pitch)
// LEFT/RIGHT: 左右平移
// 与 Terrain1 区别:
// 1. Index buffer 使用 D3DFMT_INDEX32
// 2. Camera 使用 CAMERA_AIR
// 3. 高度图使用 castlehm257.raw

#pragma once

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

    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

////////////////////////////////////////////////////////////////////////////////
// 渲染类 RenderTerrain2
////////////////////////////////////////////////////////////////////////////////

class RenderTerrain2 : public SGL::Render {
public:
    RenderTerrain2() : m_Camera(SGL::Camera::CAMERA_AIR) {}
    BOOL Init(UINT width, UINT height, HWND hwnd, BOOL windowed, D3DDEVTYPE devType);
    BOOL InitFont();
    void DumpMesh(LPCTSTR fname, ID3DXMesh* mesh);

    virtual void Draw(float timeDelta);

private:
    CComPtr<ID3DXFont>          m_Font;
    std::auto_ptr<SGL::Terrain> m_Terrain;

    static LPCTSTR MeshDumpFile;
    static LPCTSTR HeightMapFile;

public:
    SGL::Camera m_Camera;
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
// 输入处理类 Terrain2Input
////////////////////////////////////////////////////////////////////////////////

class Terrain2Input : public SGL::Input {
public:
    virtual void ProcessInput(float timeDelta);
};
