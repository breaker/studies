//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

#ifndef PHYSICS_NJIN
#define PHYSICS_NJIN

#include <d3dx9.h>
#include <vector>
#include "obb.h"

using namespace std;

const float GRAVITY = -9.82f;
const float TIME_STEP = 0.005f;
const float DRAG = -0.3f;
const float FRICTION = 0.995f;

class PhysicsObject {
public:
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void AddForces() = 0;
    virtual void SatisfyConstraints(vector<OBB*> &obstacles) = 0;
};

class PhysicsEngine {
public:
    PhysicsEngine();
    void Init();
    void Update(float deltaTime);
    void AddForces();
    void SatisfyConstraints();
    void Render();
    void Reset();

private:
    vector<PhysicsObject*> m_physicsObjects;
    vector<OBB*> m_obstacles;
    float m_time;
};

#endif