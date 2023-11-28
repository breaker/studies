//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _BOUNDING_SPHERE_
#define _BOUNDING_SPHERE_

#include <d3dx9.h>

class BoundingSphere {
public:
    BoundingSphere(D3DXVECTOR3 pos, float radius);
    void Render();
    bool ResolveCollision(D3DXVECTOR3 &hairPos, float hairRadius);

private:
    static ID3DXMesh* sm_pSphereMesh;

    D3DXVECTOR3 m_position;
    float m_radius;
};

#endif