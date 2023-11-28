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
#include "btBulletDynamicsCommon.h"

//Bullet-to-DX Helper Functions
D3DXVECTOR3 BT2DX_VECTOR3(const btVector3 &v);
D3DXQUATERNION BT2DX_QUATERNION(const btQuaternion &q);
D3DXMATRIX BT2DX_MATRIX(const btMotionState &ms);
D3DXMATRIX BT2DX_MATRIX(const btTransform &ms);

using namespace std;

enum PHYS_CONTRAINTS {
    HINGE,
    TWISTCONE,
    BALLPOINT
};

class PhysicsEngine {
public:
    PhysicsEngine();
    void Init();
    void Update(float deltaTime);
    void Render();
    void Reset(PHYS_CONTRAINTS c);
    OBB* CreateOBB(D3DXVECTOR3 pos, D3DXVECTOR3 size, bool dynamic=true);

    D3DXVECTOR3 RandomVector(D3DXVECTOR3 &min, D3DXVECTOR3 &max);
    float RandomFloat();

private:

    vector<OBB*> m_boxes;

    btBroadphaseInterface* m_broadphase;
    btCollisionDispatcher* m_dispatcher;
    btDynamicsWorld* m_dynamicsWorld;
    btConstraintSolver* m_solver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;

    btRigidBody *m_floor;

    float m_time;
    bool m_impulse;
};

#endif