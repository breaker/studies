//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef PHYSICS_PARTICLE
#define PHYSICS_PARTICLE

#include <d3dx9.h>
#include <vector>
#include "physicsEngine.h"

using namespace std;

void LoadParticleResources();

class Particle : public PhysicsObject {
    friend class Spring;
public:
    Particle();
    Particle(D3DXVECTOR3 pos);
    void Update(float deltaTime);
    void Render();
    void AddForces();
    void SatisfyConstraints(vector<OBB*> &obstacles);

private:
    D3DXVECTOR3 m_pos;
    D3DXVECTOR3 m_oldPos;
    D3DXVECTOR3 m_forces;
    float m_bounce;
};

#endif