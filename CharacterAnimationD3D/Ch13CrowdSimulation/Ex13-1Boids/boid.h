//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef _BOID_
#define _BOID_

#include <d3dx9.h>
#include "mesh.h"

using namespace std;

class Boid {
    friend class Flock;
public:
    Boid(Flock *pFlock);
    ~Boid();
    void Update(float deltaTime);
    void Render(bool shadow);

private:
    static Mesh* sm_pBoidMesh;

    Flock* m_pFlock;
    D3DXVECTOR3 m_position;
    D3DXVECTOR3 m_velocity;
};

#endif