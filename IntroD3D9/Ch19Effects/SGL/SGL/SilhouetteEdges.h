// SilhouetteEdges.h
//

// 说明:
// 轮廓外框线
// Generates the silhouette geometry of a mesh and renders it.  Note
// that we assume mesh vertex formats as described in MeshVertex.

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Utils.h>

SGL_NS_BEGIN

struct EdgeVertex {
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR3 faceNormal1;
    D3DXVECTOR3 faceNormal2;
};

struct MeshVertex {
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    static const DWORD FVF;
};

class SGL_API SilhouetteEdges : private Uncopyable {
public:
    SilhouetteEdges(IDirect3DDevice9* dev, ID3DXMesh* mesh, ID3DXBuffer* adjBuf);
    ~SilhouetteEdges() {}

    void Render();

private:
    IDirect3DDevice9*                       m_D3DDev;
    CComPtr<IDirect3DVertexBuffer9>         m_VertexBuf;
    CComPtr<IDirect3DIndexBuffer9>          m_IndexBuf;
    CComPtr<IDirect3DVertexDeclaration9>    m_VertexDecl;

    UINT m_NumVerts;
    UINT m_NumFaces;

private:
    BOOL CreateVertexDecl();
    void GetFaceNormal(ID3DXMesh* mesh, DWORD faceIndex, D3DXVECTOR3* faceNormal);
    void GetFaceNormals(ID3DXMesh* mesh, ID3DXBuffer* adjBuf, D3DXVECTOR3* curFaceNormal, D3DXVECTOR3 adjFaceNormals[3], DWORD faceIndex);
    void GenEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuf);
    void GenEdgeIndices(ID3DXMesh* mesh);
};

SGL_NS_END
