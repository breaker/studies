#include "physicsEngine.h"

PhysicsEngine physicsEngine;

extern IDirect3DDevice9* g_pDevice;
extern ofstream g_debug;
extern ID3DXEffect *g_pEffect;

//Bullet-to-DX Helper Functions
D3DXVECTOR3 BT2DX_VECTOR3(const btVector3 &v) {
    return D3DXVECTOR3(v.x(), v.y(), v.z());
}

D3DXQUATERNION BT2DX_QUATERNION(const btQuaternion &q) {
    return D3DXQUATERNION(q.x(), q.y(), q.z(), q.w());
}

D3DXMATRIX BT2DX_MATRIX(const btMotionState &ms) {
    btTransform world;
    ms.getWorldTransform(world);
    return BT2DX_MATRIX(world);
}

D3DXMATRIX BT2DX_MATRIX(const btTransform &ms) {
    btQuaternion q = ms.getRotation();
    btVector3 p = ms.getOrigin();

    D3DXMATRIX pos, rot, world;
    D3DXMatrixTranslation(&pos, p.x(), p.y(), p.z());
    D3DXMatrixRotationQuaternion(&rot, &BT2DX_QUATERNION(q));
    D3DXMatrixMultiply(&world, &rot, &pos);

    return world;
}

//////////////////////////////////////////////////////////////
//                      PhysicsEngine                       //
//////////////////////////////////////////////////////////////

PhysicsEngine::PhysicsEngine() {
    m_time = 0.0f;
}

void PhysicsEngine::Init() {
    srand(GetTickCount());

    btVector3 worldAabbMin(-1000,-1000,-1000);
    btVector3 worldAabbMax(1000,1000,1000);
    const int maxProxies = 32766;

    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    Reset();
}

void PhysicsEngine::Reset() {
    //Reset world
    if (m_dynamicsWorld != NULL) {
        delete m_dynamicsWorld;
        m_dynamicsWorld = NULL;
    }

    for (int i=0; i<(int)m_ragdolls.size(); i++) {
        delete m_ragdolls[i];
    }
    m_ragdolls.clear();

    //Create dynamics world and add a floor...
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
    m_floor = new btRigidBody(0.0f, new btDefaultMotionState(), new btStaticPlaneShape(btVector3(0,1,0), 0.0));
    m_dynamicsWorld->addRigidBody(m_floor);
}

void PhysicsEngine::Update(float deltaTime) {
    for (int i=0; i<(int)m_ragdolls.size(); i++) {
        m_ragdolls[i]->Update(deltaTime);
    }

    if (m_dynamicsWorld != NULL) {
        m_dynamicsWorld->stepSimulation(deltaTime);
    }
}

void PhysicsEngine::Render() {
    for (int i=0; i<(int)m_ragdolls.size(); i++) {
        m_ragdolls[i]->Render();
    }
}

OBB* PhysicsEngine::CreateOBB(D3DXVECTOR3 pos, D3DXVECTOR3 size, bool dynamic) {
    OBB *obb = new OBB(pos, size, dynamic);
    m_dynamicsWorld->addRigidBody(obb->m_pBody);
    return obb;
}

D3DXVECTOR3 PhysicsEngine::RandomVector(D3DXVECTOR3 &min, D3DXVECTOR3 &max) {
    return D3DXVECTOR3(min.x + (max.x - min.x) * RandomFloat(),
                       min.y + (max.y - min.y) * RandomFloat(),
                       min.z + (max.z - min.z) * RandomFloat());
}

float PhysicsEngine::RandomFloat() {
    return (rand()%1000) / 1000.0f;
}