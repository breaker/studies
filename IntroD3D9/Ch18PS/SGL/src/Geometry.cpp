// Geometry.cpp
//

#include "StdAfx.h"
#include <SGL/Geometry.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// struct BoundingBox
////////////////////////////////////////////////////////////////////////////////

BOOL BoundingBox::ComputeBoundingBox(ID3DXBaseMesh* mesh) {
    BYTE* v = NULL;
    mesh->LockVertexBuffer(0, (void**) &v);
    HRESULT hr = D3DXComputeBoundingBox((D3DXVECTOR3*) v, mesh->GetNumVertices(), D3DXGetFVFVertexSize(mesh->GetFVF()), &min, &max);
    mesh->UnlockVertexBuffer();
    return !FAILED(hr);
}

////////////////////////////////////////////////////////////////////////////////
// struct BoundingSphere
////////////////////////////////////////////////////////////////////////////////

BOOL BoundingSphere::ComputeBoundingSphere(ID3DXBaseMesh* mesh) {
    BYTE* v = NULL;
    mesh->LockVertexBuffer(0, (void**) &v);
    HRESULT hr = D3DXComputeBoundingSphere((D3DXVECTOR3*) v, mesh->GetNumVertices(), D3DXGetFVFVertexSize(mesh->GetFVF()), &center, &radius);
    mesh->UnlockVertexBuffer();
    return !FAILED(hr);
}

////////////////////////////////////////////////////////////////////////////////
// struct Ray
////////////////////////////////////////////////////////////////////////////////

// ÉäÏßÏà½»ÐÔ²âÊÔ (Ray Test) / Åö×²¼ì²â

void Ray::CalcPickingRay(IDirect3DDevice9* dev, int x, int y) {
    float px = 0.0f;
    float py = 0.0f;

    D3DVIEWPORT9 vp;
    dev->GetViewport(&vp);

    D3DXMATRIX proj;
    dev->GetTransform(D3DTS_PROJECTION, &proj);

    px = (2.0f * x / vp.Width - 1.0f) / proj(0, 0);
    py = (-2.0f * y / vp.Height + 1.0f) / proj(1, 1);

    this->origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    this->dir = D3DXVECTOR3(px, py, 1.0f);
}

void Ray::TransformRay(const D3DXMATRIX& T) {
    // transform the ray's origin, w = 1.
    D3DXVec3TransformCoord(&this->origin, &this->origin, &T);

    // transform the ray's direction, w = 0.
    D3DXVec3TransformNormal(&this->dir, &this->dir, &T);

    // normalize the direction
    D3DXVec3Normalize(&this->dir, &this->dir);
}

BOOL Ray::RaySphereIntTest(const BoundingSphere& sphere) {
    D3DXVECTOR3 v = this->origin - sphere.center;

    float b = 2.0f * D3DXVec3Dot(&this->dir, &v);
    float c = D3DXVec3Dot(&v, &v) - (sphere.radius * sphere.radius);

    // find the discriminant
    float discriminant = (b * b) - (4.0f * c);

    // test for imaginary number
    if (discriminant < 0.0f)
        return FALSE;

    discriminant = sqrtf(discriminant);

    float s0 = (-b + discriminant) / 2.0f;
    float s1 = (-b - discriminant) / 2.0f;

    // if a solution is >= 0, then we intersected the sphere
    return (s0 >= 0.0f || s1 >= 0.0f);
}

SGL_NS_END
