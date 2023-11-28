// Terrain.cpp
//

#include "StdAfx.h"
#include <SGL/Terrain.h>

SGL_NS_BEGIN

const DWORD Terrain::TerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

BOOL Terrain::Init(IDirect3DDevice9* dev, LPCTSTR hmapFName, int vertsPerRow, int vertsPerCol, int cellSpacing, float heightScale, D3DFORMAT idxBufFmt) {
    m_D3DDev = dev;
    m_VertsPerRow = vertsPerRow;
    m_VertsPerCol = vertsPerCol;
    m_CellSpacing = cellSpacing;

    m_CellsPerRow = m_VertsPerRow - 1;
    m_CellsPerCol = m_VertsPerCol - 1;

    m_Width = m_CellsPerRow * m_CellSpacing;
    m_Depth = m_CellsPerCol * m_CellSpacing;

    m_Vertices  = m_VertsPerRow * m_VertsPerCol;
    m_Triangles = m_CellsPerRow * m_CellsPerCol * 2;

    m_HeightScale = heightScale;

    // load heightmap
    if (!ReadRawFile(hmapFName)) {
        SGL_MRPT(_CRT_ERROR, "ReadRawFile failed");
        return FALSE;
    }

    // scale heights
    for (size_t i = 0; i < m_HeightMap.size(); i++)
        m_HeightMap[i] = (int) (m_HeightMap[i] * heightScale);

    // compute the vertices
    if (!ComputeVertex()) {
        SGL_MRPT(_CRT_ERROR, "ComputeVertex failed");
        return FALSE;
    }

    if (idxBufFmt != D3DFMT_INDEX16 && idxBufFmt != D3DFMT_INDEX32) {
        SGL_MRPT(_CRT_ERROR, "Not support this index buffer format: %d", idxBufFmt);
        return FALSE;
    }
    m_IndexBufFormat = idxBufFmt;

    // compute the indices
    if (!ComputeIndex()) {
        SGL_MRPT(_CRT_ERROR, "ComputeIndex failed");
        return FALSE;
    }

    return TRUE;
}

Terrain::~Terrain() {
    Release(m_VertexBuf);
    Release(m_IndexBuf);
    Release(m_Texture);
}

BOOL Terrain::ComputeVertex() {
    HRESULT hr = m_D3DDev->CreateVertexBuffer(m_Vertices * sizeof(TerrainVertex), D3DUSAGE_WRITEONLY, TerrainVertex::FVF, D3DPOOL_MANAGED, &m_VertexBuf, NULL);
    SGL_FAILED_DO(hr, return FALSE);

    // coordinates to start generating vertices at
    int startX = -m_Width / 2;
    int startZ = m_Depth / 2;

    // coordinates to end generating vertices at
    int endX = m_Width / 2;
    int endZ = -m_Depth / 2;

    // compute the increment size of the texture coordinates
    // from one vertex to the next.
    float uCoordIncrementSize = 1.0f / (float) m_CellsPerRow;
    float vCoordIncrementSize = 1.0f / (float) m_CellsPerCol;

    TerrainVertex* v = 0;
    m_VertexBuf->Lock(0, 0, (void**) &v, 0);

    int i = 0;
    for (int z = startZ; z >= endZ; z -= m_CellSpacing) {
        int j = 0;
        for (int x = startX; x <= endX; x += m_CellSpacing) {
            // compute the correct index into the vertex buffer and heightmap
            // based on where we are in the nested loop.
            int index = i * m_VertsPerRow + j;
            v[index] = TerrainVertex((float) x, (float) m_HeightMap[index], (float) z, (float) j * uCoordIncrementSize, (float) i * vCoordIncrementSize);
            j++;    // next column
        }
        i++;    // next row
    }

    m_VertexBuf->Unlock();
    return TRUE;
}

template <typename IndexType>
void Terrain::ComputeIndexInner() {
    IndexType* indices = 0;
    m_IndexBuf->Lock(0, 0, (void**) &indices, 0);

    // index to start of a group of 6 indices that describe the
    // two triangles that make up a quad
    int baseIndex = 0;

    // loop through and compute the triangles of each quad
    for (int i = 0; i < m_CellsPerCol; i++) {
        for (int j = 0; j < m_CellsPerRow; j++) {
            indices[baseIndex]     =   i   * m_VertsPerRow + j;
            indices[baseIndex + 1] =   i   * m_VertsPerRow + j + 1;
            indices[baseIndex + 2] = (i+1) * m_VertsPerRow + j;

            indices[baseIndex + 3] = (i+1) * m_VertsPerRow + j;
            indices[baseIndex + 4] =   i   * m_VertsPerRow + j + 1;
            indices[baseIndex + 5] = (i+1) * m_VertsPerRow + j + 1;

            // next quad
            baseIndex += 6;
        }
    }
    m_IndexBuf->Unlock();
}


BOOL Terrain::ComputeIndex() {
    HRESULT hr = 0;

    if (m_IndexBufFormat == D3DFMT_INDEX16) {
        hr = m_D3DDev->CreateIndexBuffer(m_Triangles * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_IndexBuf, NULL);
        SGL_FAILED_DO(hr, return FALSE);
        ComputeIndexInner<WORD>();
    }
    else if (m_IndexBufFormat == D3DFMT_INDEX32) {
        hr = m_D3DDev->CreateIndexBuffer(m_Triangles * 3 * sizeof(DWORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_IndexBuf, NULL);
        SGL_FAILED_DO(hr, return FALSE);
        ComputeIndexInner<DWORD>();
    }
    else
        return FALSE;
    return TRUE;
}

BOOL Terrain::LoadTexture(LPCTSTR fname) {
    HRESULT hr = D3DXCreateTextureFromFile(m_D3DDev, fname, &m_Texture);
    SGL_FAILED_DO(hr, return FALSE);
    return TRUE;
}

BOOL Terrain::GenTexture(D3DXVECTOR3* dirToLight) {
    // Method fills the top surface of a texture procedurally.  Then
    // lights the top surface.  Finally, it fills the other mipmap
    // surfaces based on the top surface data using D3DXFilterTexture.

    // texel for each quad cell
    int texWidth  = m_CellsPerRow;
    int texHeight = m_CellsPerCol;

    // create an empty texture
    HRESULT hr = D3DXCreateTexture(m_D3DDev, texWidth, texHeight, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_Texture);
    SGL_FAILED_DO(hr, return FALSE);

    D3DSURFACE_DESC texDesc;
    m_Texture->GetLevelDesc(0, &texDesc);

    // make sure we got the requested format because our code
    // that fills the texture is hard coded to a 32 bit pixel depth.
    if (texDesc.Format != D3DFMT_X8R8G8B8)
        return FALSE;

    D3DLOCKED_RECT lockedRect;
    m_Texture->LockRect(0, &lockedRect, NULL, 0);

    DWORD* imageData = (DWORD*) lockedRect.pBits;
    for (int i = 0; i < texHeight; i++) {
        for (int j = 0; j < texWidth; j++) {
            D3DXCOLOR c;

            // get height of upper left vertex of quad.
            float height = (float) GetHeightmapEntry(i, j) / m_HeightScale;

            if (height < 42.5f)
                c = BEACH_SAND;
            else if (height < 85.0f)
                c = LIGHT_YELLOW_GREEN;
            else if (height < 127.5f)
                c = PUREGREEN;
            else if (height < 170.0f)
                c = DARK_YELLOW_GREEN;
            else if (height < 212.5f)
                c = DARKBROWN;
            else
                c = WHITE;

            // fill locked data, note we divide the pitch by four because the
            // pitch is given in bytes and there are 4 bytes per DWORD.
            imageData[i * lockedRect.Pitch / 4 + j] = (D3DCOLOR) c;
        }
    }

    m_Texture->UnlockRect(0);

    if (!LightTerrain(dirToLight)) {
        SGL_MRPT(_CRT_ERROR, "LightTerrain failed");
        return FALSE;
    }

    hr = D3DXFilterTexture(m_Texture, NULL, 0, D3DX_DEFAULT);
    SGL_FAILED_DO(hr, SGL_MRPT(_CRT_ERROR, "D3DXFilterTexture failed"); return FALSE);

    return TRUE;
}

BOOL Terrain::LightTerrain(D3DXVECTOR3* dirToLight) {
    D3DSURFACE_DESC texDesc;
    m_Texture->GetLevelDesc(0, &texDesc);

    // make sure we got the requested format because our code that fills the
    // texture is hard coded to a 32 bit pixel depth.
    if (texDesc.Format != D3DFMT_X8R8G8B8)
        return FALSE;

    D3DLOCKED_RECT lockedRect;
    m_Texture->LockRect(0, &lockedRect, NULL, 0);

    DWORD* imageData = (DWORD*) lockedRect.pBits;
    for (UINT i = 0; i < texDesc.Height; i++) {
        for (UINT j = 0; j < texDesc.Width; j++) {
            // index into texture, note we use the pitch and divide by
            // four since the pitch is given in bytes and there are
            // 4 bytes per DWORD.
            int index = i * lockedRect.Pitch / 4 + j;

            // get current color of quad
            D3DXCOLOR c(imageData[index]);

            if (m_IndexBufFormat == D3DFMT_INDEX16) {
                // shade current quad
                c *= ComputeShade(i, j, dirToLight);;

                // save shaded color
                imageData[index] = (D3DCOLOR) c;
            }
            else if (m_IndexBufFormat == D3DFMT_INDEX32) {
                float s = ComputeShade(i, j, dirToLight);
                imageData[index] = D3DXCOLOR(s, s, s, 1.0f);
            }
            else
                return FALSE;
        }
    }

    m_Texture->UnlockRect(0);
    return TRUE;
}

float Terrain::ComputeShade(int cellRow, int cellCol, D3DXVECTOR3* dirToLight) {
    // get heights of three vertices on the quad
    float heightA = (float) GetHeightmapEntry(cellRow, cellCol);
    float heightB = (float) GetHeightmapEntry(cellRow, cellCol + 1);
    float heightC = (float) GetHeightmapEntry(cellRow + 1, cellCol);

    // build two vectors on the quad
    D3DXVECTOR3 u((float) m_CellSpacing, heightB - heightA, 0.0f);
    D3DXVECTOR3 v(0.0f, heightC - heightA, (float) -m_CellSpacing);

    // find the normal by taking the cross product of two
    // vectors on the quad.
    D3DXVECTOR3 n;
    D3DXVec3Cross(&n, &u, &v);
    D3DXVec3Normalize(&n, &n);

    float cosine = D3DXVec3Dot(&n, dirToLight);
    if (cosine < 0.0f)
        cosine = 0.0f;

    return cosine;
}

BOOL Terrain::ReadRawFile(LPCTSTR fname) {
    // Restriction: RAW file dimensions must be >= to the
    // dimensions of the terrain.  That is a 128x128 RAW file
    // can only be used with a terrain constructed with at most
    // 128x128 vertices.

    // A height for each vertex
    std::vector<BYTE> buf(m_Vertices);

    std::ifstream inFile(fname, std::ios_base::binary);
    if (!inFile.good())
        return FALSE;

    inFile.read((char*) &buf[0], (std::streamsize) buf.size());
    inFile.close();

    // copy BYTE vector to int vector
    m_HeightMap.resize(m_Vertices);
    for (size_t i = 0; i < buf.size(); i++)
        m_HeightMap[i] = buf[i];

    return TRUE;
}

float Terrain::GetHeight(float x, float z) {
    // Translate on xz-plane by the transformation that takes
    // the terrain START point to the origin.
    x = ((float) m_Width / 2.0f) + x;
    z = ((float) m_Depth / 2.0f) - z;

    // Scale down by the transformation that makes the
    // cellspacing equal to one.  This is given by
    // 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
    x /= (float) m_CellSpacing;
    z /= (float) m_CellSpacing;

    // From now on, we will interpret our positive z-axis as
    // going in the 'down' direction, rather than the 'up' direction.
    // This allows to extract the row and column simply by 'flooring'
    // x and z:

    float col = ::floorf(x);
    float row = ::floorf(z);

    // get the heights of the quad we're in:
    //
    //  A   B
    //  *---*
    //  | / |
    //  *---*
    //  C   D

    float A = (float) GetHeightmapEntry((int) row, (int) col);
    float B = (float) GetHeightmapEntry((int) row, (int) (col + 1));
    float C = (float) GetHeightmapEntry((int) (row + 1), (int) col);
    float D = (float) GetHeightmapEntry((int) (row + 1), (int) (col + 1));

    //
    // Find the triangle we are in:
    //

    // Translate by the transformation that takes the upper-left
    // corner of the cell we are in to the origin.  Recall that our
    // cellspacing was nomalized to 1.  Thus we have a unit square
    // at the origin of our +x -> 'right' and +z -> 'down' system.
    float dx = x - col;
    float dz = z - row;

    // Note the below compuations of u and v are unneccessary, we really
    // only need the height, but we compute the entire vector to emphasis
    // the books discussion.
    float height = 0.0f;
    if (dz < 1.0f - dx) { // upper triangle ABC
        float uy = B - A; // A->B
        float vy = C - A; // A->C

        // Linearly interpolate on each vector.  The height is the vertex
        // height the vectors u and v originate from {A}, plus the heights
        // found by interpolating on each vector u and v.
        height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
    }
    else { // lower triangle DCB
        float uy = C - D; // D->C
        float vy = B - D; // D->B

        // Linearly interpolate on each vector.  The height is the vertex
        // height the vectors u and v originate from {D}, plus the heights
        // found by interpolating on each vector u and v.
        height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
    }

    return height;
}

BOOL Terrain::Draw(D3DXMATRIX* world, BOOL drawTris) {
    if (m_D3DDev) {
        m_D3DDev->SetTransform(D3DTS_WORLD, world);

        m_D3DDev->SetStreamSource(0, m_VertexBuf, 0, sizeof(TerrainVertex));
        m_D3DDev->SetFVF(TerrainVertex::FVF);
        m_D3DDev->SetIndices(m_IndexBuf);

        m_D3DDev->SetTexture(0, m_Texture);

        // turn off lighting since we're lighting it ourselves
        m_D3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

        HRESULT hr = m_D3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_Vertices, 0, m_Triangles);
        m_D3DDev->SetRenderState(D3DRS_LIGHTING, TRUE);

        if (drawTris) {
            m_D3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
            hr = m_D3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_Vertices, 0, m_Triangles);
            m_D3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        }

        SGL_FAILED_DO(hr, return FALSE);
    }
    return TRUE;
}

SGL_NS_END
