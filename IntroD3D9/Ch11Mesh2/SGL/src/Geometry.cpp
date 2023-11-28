// Geometry.cpp
//

#include "StdAfx.h"
#include <SGL/Geometry.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// struct BoundingBox
////////////////////////////////////////////////////////////////////////////////

BOOL BoundingBox::ComputeBoundingBox(ID3DXBaseMesh* mesh)
{
    BYTE* v = NULL;
    mesh->LockVertexBuffer(0, (void**) &v);
    HRESULT hr = D3DXComputeBoundingBox((D3DXVECTOR3*) v, mesh->GetNumVertices(), D3DXGetFVFVertexSize(mesh->GetFVF()), &min, &max);
    mesh->UnlockVertexBuffer();
    return !FAILED(hr);
}

////////////////////////////////////////////////////////////////////////////////
// struct BoundingSphere
////////////////////////////////////////////////////////////////////////////////

BOOL BoundingSphere::ComputeBoundingSphere(ID3DXBaseMesh* mesh)
{
    BYTE* v = NULL;
    mesh->LockVertexBuffer(0, (void**) &v);
    HRESULT hr = D3DXComputeBoundingSphere((D3DXVECTOR3*) v, mesh->GetNumVertices(), D3DXGetFVFVertexSize(mesh->GetFVF()), &center, &radius);
    mesh->UnlockVertexBuffer();
    return !FAILED(hr);
}

SGL_NS_END
