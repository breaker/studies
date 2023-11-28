#include "physicsEngine.h"
#include "particle.h"
#include "spring.h"

PhysicsEngine physicsEngine;

extern ofstream g_debug;

PhysicsEngine::PhysicsEngine() {

}

void PhysicsEngine::Init() {
    Reset();

    //Add Obstacles
    OBB *obb = new OBB(D3DXVECTOR3(5.0f, 0.2f, 5.0f));
    obb->m_pos.y -= 1.0f;
    m_obstacles.push_back(obb);

    obb = new OBB(D3DXVECTOR3(0.8f, 0.1f, 2.0f));
    obb->m_pos = D3DXVECTOR3(-0.5f, 1.0f, 0.0f);
    D3DXQuaternionRotationYawPitchRoll(&obb->m_rot, 0.0f, -0.1f, -0.5f);
    m_obstacles.push_back(obb);

    obb = new OBB(D3DXVECTOR3(0.8f, 0.1f, 2.0f));
    obb->m_pos = D3DXVECTOR3(0.5f, 2.0f, 0.0f);
    D3DXQuaternionRotationYawPitchRoll(&obb->m_rot, 0.1f, 0.0f, 0.4f);
    m_obstacles.push_back(obb);

    obb = new OBB(D3DXVECTOR3(1.0f, 0.1f, 2.0f));
    obb->m_pos = D3DXVECTOR3(-0.5f, 3.0f, 0.0f);
    D3DXQuaternionRotationYawPitchRoll(&obb->m_rot, 0.0f, -0.1f, -0.3f);
    m_obstacles.push_back(obb);

    obb = new OBB(D3DXVECTOR3(0.5f, 0.1f, 1.0f));
    obb->m_pos = D3DXVECTOR3(0.8f, 4.0f, 0.0f);
    D3DXQuaternionRotationYawPitchRoll(&obb->m_rot, 0.1f, 0.0f, 0.2f);
    m_obstacles.push_back(obb);
}

void PhysicsEngine::Reset() {
    //Remove old objects
    for (int i=0; i<(int)m_physicsObjects.size(); i++) {
        delete m_physicsObjects[i];
    }
    m_physicsObjects.clear();

    //Add new Particles
    srand(GetTickCount());

    for (int i=0; i<20; i++) {
        float x = ((rand()%1000) / 1000.0f) * 3.0f - 1.5f;
        float z = ((rand()%1000) / 1000.0f) * 3.0f - 1.5f;
        float y = ((rand()%1000) / 1000.0f) * 4.0f + 5.0f;
        m_physicsObjects.push_back(new Particle(D3DXVECTOR3(x, y, z)));
    }

    //Add Springs
    int numParticles = (int)m_physicsObjects.size();
    for (int i=0; i<numParticles-1; i+=2) {
        m_physicsObjects.push_back(new Spring((Particle*)m_physicsObjects[i],
                                              (Particle*)m_physicsObjects[i + 1],
                                              0.4f));
    }
}

void PhysicsEngine::Update(float deltaTime) {
    //For safety allow max 3 timesteps
    m_time = min(m_time + deltaTime, TIME_STEP * 3.0f);

    while (m_time > TIME_STEP) {
        AddForces();

        //Update Objects
        int numObj = (int)m_physicsObjects.size();
        for (int i=0; i<numObj; i++) {
            m_physicsObjects[i]->Update(TIME_STEP);
        }

        SatisfyConstraints();

        m_time -= TIME_STEP;
    }
}

void PhysicsEngine::AddForces() {
    //Update forces
    int numObj = (int)m_physicsObjects.size();
    for (int i=0; i<numObj; i++) {
        m_physicsObjects[i]->AddForces();
    }
}

void PhysicsEngine::SatisfyConstraints() {
    //Satisfy Constraints
    int numObj = (int)m_physicsObjects.size();
    for (int n=0; n<NUM_ITERATIONS; n++) {
        for (int i=numObj - 1; i >= 0; i--) {
            m_physicsObjects[i]->SatisfyConstraints(m_obstacles);
        }
    }
}

void PhysicsEngine::Render() {
    //Render Objects
    int numObj = (int)m_physicsObjects.size();
    for (int i=0; i<numObj; i++) {
        m_physicsObjects[i]->Render();
    }

    //Render Obstacles
    numObj = (int)m_obstacles.size();
    for (int i=0; i<numObj; i++) {
        m_obstacles[i]->Render();
    }
}