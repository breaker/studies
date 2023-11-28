// SilhouetteEdges.cpp
//

#include "StdAfx.h"
#include <SGL/SilhouetteEdges.h>

SGL_NS_BEGIN

const DWORD MeshVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

SilhouetteEdges::SilhouetteEdges(IDirect3DDevice9* dev, ID3DXMesh* mesh, ID3DXBuffer* adjBuf) :
    m_D3DDev(dev), m_VertexBuf(NULL), m_IndexBuf(NULL), m_VertexDecl(NULL), m_NumVerts(0), m_NumFaces(0) {
    GenEdgeVertices(mesh, adjBuf);
    GenEdgeIndices(mesh);
    CreateVertexDecl();
}

BOOL SilhouetteEdges::CreateVertexDecl() {
    D3DVERTEXELEMENT9 decl[] = {
        // offsets in bytes
        {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
        {0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
        {0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
        D3DDECL_END()
    };

    IDirect3DVertexDeclaration9* vdecl;
    HRESULT hr = m_D3DDev->CreateVertexDeclaration(decl, &vdecl);
    SGL_FAILED_DO(hr, SGL_MRPTDX(_CRT_ERROR, "IDirect3DDevice9::CreateVertexDeclaration", hr); return FALSE);
    m_VertexDecl.Attach(vdecl);

    return TRUE;
}

void SilhouetteEdges::GetFaceNormal(ID3DXMesh* mesh, DWORD faceIndex, D3DXVECTOR3* faceNormal) {
    MeshVertex* vertices = NULL;
    mesh->LockVertexBuffer(0, (void**) &vertices);

    WORD* indices = NULL;
    mesh->LockIndexBuffer(0, (void**) &indices);

    // Get the triangle's indices
    WORD index0 = indices[faceIndex * 3];
    WORD index1 = indices[faceIndex * 3 + 1];
    WORD index2 = indices[faceIndex * 3 + 2];

    // Now extract the triangles vertices positions
    D3DXVECTOR3 v0 = vertices[index0].position;
    D3DXVECTOR3 v1 = vertices[index1].position;
    D3DXVECTOR3 v2 = vertices[index2].position;

    // Compute face normal
    D3DXVECTOR3 edge0, edge1;
    edge0 = v1 - v0;
    edge1 = v2 - v0;
    D3DXVec3Cross(faceNormal, &edge0, &edge1);
    D3DXVec3Normalize(faceNormal, faceNormal);

    mesh->UnlockVertexBuffer();
    mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::GetFaceNormals(ID3DXMesh* mesh, ID3DXBuffer* adjBuf, D3DXVECTOR3* curFaceNormal, D3DXVECTOR3 adjFaceNormals[3], DWORD faceIndex) {
    MeshVertex* vertices = NULL;
    mesh->LockVertexBuffer(0, (void**) &vertices);

    WORD* indices = NULL;
    mesh->LockIndexBuffer(0, (void**) &indices);

    DWORD* adj = (DWORD*) adjBuf->GetBufferPointer();

    // Get the face normal.
    GetFaceNormal(mesh, faceIndex, curFaceNormal);

    // Get adjacent face indices
    WORD faceIndex0 = (WORD) adj[faceIndex * 3];
    WORD faceIndex1 = (WORD) adj[faceIndex * 3 + 1];
    WORD faceIndex2 = (WORD) adj[faceIndex * 3 + 2];

    // Get adjacent face normals, if there is no adjacent face,
    // then set the adjacent face normal to the opposite of the
    // "currentFaceNormal".  Recall we do this because edges that
    // don't have an adjacent triangle are automatically considered
    // silhouette edges.  And in order to make that happen, we need
    // the current face normal and adjacent face normal to point
    // in the opposite direction.  Also, recall that an entry
    // in the adjacency buffer equal to -1 denotes that the edge
    // doesn't have an adjacent triangle.

    D3DXVECTOR3 faceNormal0, faceNormal1, faceNormal2;

    // is there an adjacent triangle?
    if (faceIndex0 != USHRT_MAX) {
        WORD i0 = indices[faceIndex0 * 3];
        WORD i1 = indices[faceIndex0 * 3 + 1];
        WORD i2 = indices[faceIndex0 * 3 + 2];

        D3DXVECTOR3 v0 = vertices[i0].position;
        D3DXVECTOR3 v1 = vertices[i1].position;
        D3DXVECTOR3 v2 = vertices[i2].position;

        D3DXVECTOR3 edge0 = v1 - v0;
        D3DXVECTOR3 edge1 = v2 - v0;
        D3DXVec3Cross(&faceNormal0, &edge0, &edge1);
        D3DXVec3Normalize(&faceNormal0, &faceNormal0);
    }
    else
        faceNormal0 = -(*curFaceNormal);

    // is there an adjacent triangle?
    if (faceIndex1 != USHRT_MAX) {
        WORD i0 = indices[faceIndex1 * 3];
        WORD i1 = indices[faceIndex1 * 3 + 1];
        WORD i2 = indices[faceIndex1 * 3 + 2];

        D3DXVECTOR3 v0 = vertices[i0].position;
        D3DXVECTOR3 v1 = vertices[i1].position;
        D3DXVECTOR3 v2 = vertices[i2].position;

        D3DXVECTOR3 edge0 = v1 - v0;
        D3DXVECTOR3 edge1 = v2 - v0;
        D3DXVec3Cross(&faceNormal1, &edge0, &edge1);
        D3DXVec3Normalize(&faceNormal1, &faceNormal1);
    }
    else
        faceNormal1 = -(*curFaceNormal);

    // is there an adjacent triangle?
    if (faceIndex2 != USHRT_MAX) {
        WORD i0 = indices[faceIndex2 * 3];
        WORD i1 = indices[faceIndex2 * 3 + 1];
        WORD i2 = indices[faceIndex2 * 3 + 2];

        D3DXVECTOR3 v0 = vertices[i0].position;
        D3DXVECTOR3 v1 = vertices[i1].position;
        D3DXVECTOR3 v2 = vertices[i2].position;

        D3DXVECTOR3 edge0 = v1 - v0;
        D3DXVECTOR3 edge1 = v2 - v0;
        D3DXVec3Cross(&faceNormal2, &edge0, &edge1);
        D3DXVec3Normalize(&faceNormal2, &faceNormal2);
    }
    else
        faceNormal2 = -(*curFaceNormal);

    // save adjacent face normals
    adjFaceNormals[0] = faceNormal0;
    adjFaceNormals[1] = faceNormal1;
    adjFaceNormals[2] = faceNormal2;

    mesh->UnlockVertexBuffer();
    mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::GenEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuf) {
    // 3 edges per face and 4 vertices per edge
    m_NumVerts = mesh->GetNumFaces() * 3 * 4;

    IDirect3DVertexBuffer9* vb;
    m_D3DDev->CreateVertexBuffer(m_NumVerts * sizeof(EdgeVertex), D3DUSAGE_WRITEONLY,
                                 0, // using vertex declaration
                                 D3DPOOL_MANAGED, &vb, NULL);
    m_VertexBuf.Attach(vb);

    MeshVertex* vertices = NULL;
    mesh->LockVertexBuffer(0, (void**) &vertices);

    WORD* indices = NULL;
    mesh->LockIndexBuffer(0, (void**) &indices);

    EdgeVertex* edgeVertices = NULL;
    m_VertexBuf->Lock(0, 0, (void**) &edgeVertices, 0);

    for (DWORD i = 0; i < mesh->GetNumFaces(); ++i) {
        D3DXVECTOR3 currentFaceNormal;
        D3DXVECTOR3 adjFaceNormals[3];

        GetFaceNormals(mesh, adjBuf, &currentFaceNormal, adjFaceNormals, i);

        // get the indices for this face
        WORD index0 = indices[i * 3];
        WORD index1 = indices[i * 3 + 1];
        WORD index2 = indices[i * 3 + 2];

        // get the vertices for this face
        MeshVertex v0 = vertices[index0];
        MeshVertex v1 = vertices[index1];
        MeshVertex v2 = vertices[index2];

        // A        B
        // *--------*
        // |  edge  |
        // *--------*
        // C        D
        // note, C and D are duplicates of A and B respectively,
        // such that the quad is degenerate.  The vertex shader
        // will un-degenerate the quad if it is a silhouette edge.

        // compute edge0 v0->v1, note adjacent face
        // normal is faceNormal0
        EdgeVertex A0, B0, C0, D0;

        A0.position    = v0.position;
        A0.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        A0.faceNormal1 = currentFaceNormal;
        A0.faceNormal2 = adjFaceNormals[0];

        B0.position    = v1.position;
        B0.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        B0.faceNormal1 = currentFaceNormal;
        B0.faceNormal2 = adjFaceNormals[0];

        C0 = A0;
        C0.normal = v0.normal;

        D0 = B0;
        D0.normal = v1.normal;

        *edgeVertices = A0;
        ++edgeVertices;
        *edgeVertices = B0;
        ++edgeVertices;
        *edgeVertices = C0;
        ++edgeVertices;
        *edgeVertices = D0;
        ++edgeVertices;

        // compute edge0 v1->v2, note adjacent face
        // normal is faceNormal1
        EdgeVertex A1, B1, C1, D1;

        A1.position    = v1.position;
        A1.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        A1.faceNormal1 = currentFaceNormal;
        A1.faceNormal2 = adjFaceNormals[1];

        B1.position    = v2.position;
        B1.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        B1.faceNormal1 = currentFaceNormal;
        B1.faceNormal2 = adjFaceNormals[1];

        C1 = A1;
        C1.normal = v1.normal;

        D1 = B1;
        D1.normal = v2.normal;

        *edgeVertices = A1;
        ++edgeVertices;
        *edgeVertices = B1;
        ++edgeVertices;
        *edgeVertices = C1;
        ++edgeVertices;
        *edgeVertices = D1;
        ++edgeVertices;

        // compute edge0 v0->v2, note adjacent face
        // normal is faceNormal2
        EdgeVertex A2, B2, C2, D2;

        A2.position    = v0.position;
        A2.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        A2.faceNormal1 = currentFaceNormal;
        A2.faceNormal2 = adjFaceNormals[2];

        B2.position    = v2.position;
        B2.normal      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        B2.faceNormal1 = currentFaceNormal;
        B2.faceNormal2 = adjFaceNormals[2];

        C2 = A2;
        C2.normal = v0.normal;

        D2 = B2;
        D2.normal = v2.normal;

        *edgeVertices = A2;
        ++edgeVertices;
        *edgeVertices = B2;
        ++edgeVertices;
        *edgeVertices = C2;
        ++edgeVertices;
        *edgeVertices = D2;
        ++edgeVertices;
    }

    m_VertexBuf->Unlock();
    mesh->UnlockVertexBuffer();
    mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::GenEdgeIndices(ID3DXMesh* mesh) {
    DWORD numEdges = mesh->GetNumFaces() * 3;

    m_NumFaces = numEdges * 2;

    IDirect3DIndexBuffer9* ib;
    m_D3DDev->CreateIndexBuffer(numEdges * 6 * sizeof(WORD), // 2 triangles per edge
                                D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib, NULL);
    m_IndexBuf.Attach(ib);

    WORD* indices = NULL;
    m_IndexBuf->Lock(0, 0, (void**) &indices, 0);

    // 0        1
    // *--------*
    // |  edge  |
    // *--------*
    // 2        3

    for (UINT i = 0; i < numEdges; ++i) {
        // Six indices to define the triangles of the edge,
        // so every edge we skip six entries in the
        // index buffer.  Four vertices to define the edge,
        // so every edge we skip four entries in the
        // vertex buffer.
        indices[i * 6]     = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 1;
        indices[i * 6 + 2] = i * 4 + 2;
        indices[i * 6 + 3] = i * 4 + 1;
        indices[i * 6 + 4] = i * 4 + 3;
        indices[i * 6 + 5] = i * 4 + 2;
    }

    m_IndexBuf->Unlock();
}

void SilhouetteEdges::Render() {
    m_D3DDev->SetVertexDeclaration(m_VertexDecl);
    m_D3DDev->SetStreamSource(0, m_VertexBuf, 0, sizeof(EdgeVertex));
    m_D3DDev->SetIndices(m_IndexBuf);

    m_D3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_NumVerts, 0, m_NumFaces);
}

SGL_NS_END
