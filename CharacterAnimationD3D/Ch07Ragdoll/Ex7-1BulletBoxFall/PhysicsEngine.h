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

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();
    void Init();
    void Release();
    void Update(float deltaTime);
    void Render();
    void Reset();
    OBB* CreateOBB(D3DXVECTOR3 pos, D3DXVECTOR3 size);

    D3DXVECTOR3 RandomVector(D3DXVECTOR3 &min, D3DXVECTOR3 &max);
    float RandomFloat();

private:

    vector<OBB*> m_boxes;

    btDynamicsWorld* m_dynamicsWorld;

    btRigidBody *m_floor;

    float m_time;
};

#endif