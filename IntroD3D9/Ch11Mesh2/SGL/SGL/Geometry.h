// Geometry.h
//

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Math.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// struct BoundingBox
////////////////////////////////////////////////////////////////////////////////

struct SGL_API BoundingBox
{
    BoundingBox() : min(SGL_FLT_MAX, SGL_FLT_MAX, SGL_FLT_MAX), max(-SGL_FLT_MAX, -SGL_FLT_MAX, -SGL_FLT_MAX) {}

    BOOL ComputeBoundingBox(ID3DXBaseMesh* mesh);

    BOOL IsPointIn(const D3DXVECTOR3& pt) {
        return (pt.x >= min.x && pt.y >= min.y && pt.z >= min.z && pt.x <= max.x && pt.y <= max.y && pt.z <= max.z);
    }

    D3DXVECTOR3 min;
    D3DXVECTOR3 max;
};

////////////////////////////////////////////////////////////////////////////////
// struct BoundingSphere
////////////////////////////////////////////////////////////////////////////////

struct SGL_API BoundingSphere
{
    BoundingSphere() : radius(0.0f) {}

    BOOL ComputeBoundingSphere(ID3DXBaseMesh* mesh);

    D3DXVECTOR3 center;
    float       radius;
};

SGL_NS_END

#endif  // __GEOMETRY_H__
