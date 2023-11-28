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

    Reset(HINGE);
}

void PhysicsEngine::Reset(PHYS_CONTRAINTS c) {
    //Reset world
    if (m_dynamicsWorld != NULL) {
        //Remove constraints
        while (m_dynamicsWorld->getNumConstraints() > 0)
            m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(0));

        //Remove Old boxes
        for (int i=0; i<(int)m_boxes.size(); i++) {
            m_dynamicsWorld->removeRigidBody(m_boxes[i]->m_pBody);

            delete m_boxes[i];
        }
        m_boxes.clear();

        delete m_dynamicsWorld;
        m_dynamicsWorld = NULL;
    }

    //Create dynamics world and add a floor...
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
    m_floor = new btRigidBody(0.0f, new btDefaultMotionState(), new btStaticPlaneShape(btVector3(0,1,0),-0.5));
    m_dynamicsWorld->addRigidBody(m_floor);

    //Add some boxes with constraints
    for (int i=0; i<50; i++) {
        m_boxes.push_back(CreateOBB(D3DXVECTOR3(0.0f, i + 0.5f, 0.0f), D3DXVECTOR3(0.5f, 1.0f, 0.5f)));

        if (i > 0) {
            btRigidBody *a = m_boxes[i]->m_pBody;
            btRigidBody *b = m_boxes[i - 1]->m_pBody;

            float M_PI_2 = D3DX_PI * 0.5f;
            float M_PI_4 = D3DX_PI * 0.25f;

            btTransform localA, localB;
            localA.setIdentity();
            localB.setIdentity();
            localA.getBasis().setEulerZYX(0,0,0);
            localA.setOrigin(btVector3(0.0f, -0.5f, 0.0f));
            localB.getBasis().setEulerZYX(0,0,0);
            localB.setOrigin(btVector3(0.0f, 0.5f, 0.0f));

            if (c == HINGE) {
                btHingeConstraint *hingeC = new btHingeConstraint(*a, *b, localA, localB);
                hingeC->setLimit(-0.5f, 0.5f);
                m_dynamicsWorld->addConstraint(hingeC, true);
            }
            else if (c == TWISTCONE) {
                btConeTwistConstraint *twistC = new btConeTwistConstraint(*a, *b, localA, localB);
                twistC->setLimit(1.0f, 1.0f, 0.6f);
                m_dynamicsWorld->addConstraint(twistC, true);
            }
            else if (c == BALLPOINT) {
                btPoint2PointConstraint *pointC = new btPoint2PointConstraint(*a, *b, localA.getOrigin(), localB.getOrigin());
                m_dynamicsWorld->addConstraint(pointC, true);
            }
        }
    }
}

void PhysicsEngine::Update(float deltaTime) {
    if (m_dynamicsWorld != NULL) {
        m_dynamicsWorld->stepSimulation(deltaTime);
    }
}

void PhysicsEngine::Render() {
    for (int i=0; i<(int)m_boxes.size(); i++) {
        m_boxes[i]->Render();
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