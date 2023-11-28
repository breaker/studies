//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef ORIENTED_BBOX
#define ORIENTED_BBOX

#include <d3dx9.h>
#include <vector>

using namespace std;

struct Ray {
    Ray() {
        m_org = m_dir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }

    Ray(D3DXVECTOR3 org, D3DXVECTOR3 dir) {
        m_org = org;
        m_dir = dir;
    }

    D3DXVECTOR3 m_org, m_dir;
};

class OBB {
    friend class RIGIDBOX;
public:
    OBB();
    OBB(D3DXVECTOR3 size);
    ~OBB();
    void Release();
    void SetSize(D3DXVECTOR3 size);

    void Render();

    bool Intersect(OBB &b);
    bool Intersect(Ray &ray);
    bool Intersect(D3DXVECTOR3 &point);

    Ray GetContactPoint(Ray &ray);

    vector<D3DXVECTOR3> GetCorners();

public:
    D3DXVECTOR3 m_size, m_pos;
    D3DXQUATERNION m_rot;

private:
    ID3DXMesh *m_pMesh;
    float m_radius;
};

#endif