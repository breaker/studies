//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef PHYSICS_SPRING
#define PHYSICS_SPRING

#include <d3dx9.h>
#include <vector>
#include "physicsEngine.h"
#include "particle.h"

using namespace std;

class Spring : public PhysicsObject {
public:
    Spring(Particle *p1, Particle *p2, float restLength);
    void Update(float deltaTime) {}
    void Render();
    void AddForces() {}
    void SatisfyConstraints(vector<OBB*> &obstacles);

private:

    Particle *m_pParticle1;
    Particle *m_pParticle2;
    float m_restLength;
};

#endif