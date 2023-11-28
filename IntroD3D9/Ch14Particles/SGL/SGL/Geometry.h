// Geometry.h
//

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

#include <SGL/Math.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// struct BoundingBox
////////////////////////////////////////////////////////////////////////////////

struct SGL_API BoundingBox {
    BoundingBox() : min(FLOAT_MAX, FLOAT_MAX, FLOAT_MAX), max(-FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX) {}

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

struct SGL_API BoundingSphere {
    BoundingSphere() : radius(0.0f) {}

    BOOL ComputeBoundingSphere(ID3DXBaseMesh* mesh);

    D3DXVECTOR3 center;
    float       radius;
};

SGL_NS_END
