// Terrain.h
//

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>
#include <SGL/Math.h>
#include <SGL/Render.h>

SGL_NS_BEGIN

class SGL_API Terrain : private Uncopyable {
public:
    Terrain() : m_D3DDev(NULL), m_Texture(NULL), m_VertexBuf(NULL), m_IndexBuf(NULL) {}
    ~Terrain();

    BOOL Init(IDirect3DDevice9* dev, LPCTSTR hmapFName, int vertsPerRow, int vertsPerCol, int cellSpacing, float heightScale, D3DFORMAT idxBufFmt);

    int GetHeightmapEntry(int row, int col) const {
        size_t i = row * m_VertsPerRow + col;
        if (i >= m_HeightMap.size())
            i = m_HeightMap.size() - 1;
        return m_HeightMap[i];
    }

    void SetHeightmapEntry(int row, int col, int value) {
        m_HeightMap[row * m_VertsPerRow + col] = value;
    }

    float GetHeight(float x, float z);

    BOOL LoadTexture(LPCTSTR fname);
    BOOL GenTexture(D3DXVECTOR3* dirToLight);
    BOOL Draw(D3DXMATRIX* world, BOOL drawTris);

private:
    IDirect3DDevice9*       m_D3DDev;
    IDirect3DTexture9*      m_Texture;
    IDirect3DVertexBuffer9* m_VertexBuf;
    IDirect3DIndexBuffer9*  m_IndexBuf;

    int m_VertsPerRow;
    int m_VertsPerCol;
    int m_CellSpacing;

    int m_CellsPerRow;
    int m_CellsPerCol;
    int m_Width;
    int m_Depth;
    int m_Vertices;
    int m_Triangles;

    float m_HeightScale;
    D3DFORMAT m_IndexBufFormat;

    std::vector<int> m_HeightMap;

    // helper methods
    BOOL ReadRawFile(LPCTSTR fname);
    BOOL ComputeVertex();

    template <typename IndexType>
    void ComputeIndexInner();
    BOOL ComputeIndex();

    BOOL LightTerrain(D3DXVECTOR3* dirToLight);
    float ComputeShade(int cellRow, int cellCol, D3DXVECTOR3* dirToLight);

    struct TerrainVertex {
        TerrainVertex() {}
        TerrainVertex(float x0, float y0, float z0, float u0, float v0) : x(x0), y(y0), z(z0), u(u0), v(v0) {}
        float x, y, z;
        float u, v;
        static const DWORD FVF;
    };
};

SGL_NS_END
