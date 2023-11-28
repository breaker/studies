#include "crowd.h"

extern IDirect3DDevice9 *g_pDevice;
extern ID3DXEffect* g_pEffect;

ID3DXMesh* Obstacle::sm_cylinder = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  OBSTACLE                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////

Obstacle::Obstacle(D3DXVECTOR3 pos, float radius) {
    m_position = pos;
    m_radius = radius;

    if (sm_cylinder == NULL) {
        D3DXCreateCylinder(g_pDevice,
                           1.0f,
                           1.0f,
                           6.0f,
                           20,
                           1,
                           &sm_cylinder,
                           NULL);
    }
}

D3DXVECTOR3 Obstacle::GetForce(CrowdEntity* pEntity) {
    D3DXVECTOR3 vToEntity = m_position - pEntity->m_position;
    float distToEntity = D3DXVec3Length(&vToEntity);

    //Affected by this obstacle?
    if (distToEntity < m_radius * 3.0f) {
        D3DXVec3Normalize(&vToEntity, &vToEntity);
        float force = 1.0f - (distToEntity / m_radius * 3.0f);
        return vToEntity * force * 3.0f;
    }

    return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void Obstacle::Render() {
    D3DXMATRIX world, sca, rot, pos;

    D3DXMatrixScaling(&sca, m_radius, m_radius, 1.0f);
    D3DXMatrixRotationX(&rot, D3DX_PI * 0.5f);
    D3DXMatrixTranslation(&pos, m_position.x, m_position.y, m_position.z);

    world = sca * rot * pos;

    g_pEffect->SetMatrix("matW", &world);
    g_pEffect->CommitChanges();

    sm_cylinder->DrawSubset(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  CROWD                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////

Crowd::Crowd(int numBoids) {
    for (int i=0; i<numBoids; i++) {
        m_entities.push_back(new CrowdEntity(this));
    }

    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(-5.0f, 0.0f, -3.0f), 1.0f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(5.0f, 0.0f, -3.0f), 0.5f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(-5.0f, 0.0f, 3.0f), 0.5f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(5.0f, 0.0f, 3.0f), 1.0f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0.75f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(-7.0f, 0.0f, 0.0f), 0.75f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(7.0f, 0.0f, 0.0f), 0.75f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(0.0f, 0.0f, -7.0f), 0.75f));
    m_obstacles.push_back(new Obstacle(D3DXVECTOR3(0.0f, 0.0f, 7.0f), 0.75f));

    m_pFloor = new Mesh("resources/meshes/floor.x");
}

Crowd::~Crowd() {
    for (int i=0; i<(int)m_entities.size(); i++) {
        delete m_entities[i];
    }
    m_entities.clear();
}

void Crowd::Update(float deltaTime) {
    for (int i=0; i<(int)m_entities.size(); i++) {
        m_entities[i]->Update(deltaTime);
    }
}

void Crowd::Render() {
    //Render floor
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    g_pEffect->SetMatrix("matW", &world);

    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    //Render floor
    m_pFloor->Render();

    //Render obstacles
    for (int i=0; i<(int)m_obstacles.size(); i++) {
        m_obstacles[i]->Render();
    }

    g_pEffect->EndPass();
    g_pEffect->End();

    for (int i=0; i<(int)m_entities.size(); i++) {
        m_entities[i]->Render();
    }
}

void Crowd::GetNeighbors(CrowdEntity* pEntity, float radius, vector<CrowdEntity*> &neighbors) {
    for (int i=0; i<(int)m_entities.size(); i++) {
        if (m_entities[i] != pEntity) {
            if (D3DXVec3Length(&(pEntity->m_position - m_entities[i]->m_position)) < radius) {
                neighbors.push_back(m_entities[i]);
            }
        }
    }
}

void Crowd::SetEntityGroundPos(D3DXVECTOR3 &pos) {
    //Create the test ray
    D3DXVECTOR3 org = pos + D3DXVECTOR3(0.0f, 10.0f, 0.0f);
    D3DXVECTOR3 dir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

    BOOL Hit;
    DWORD FaceIndex;
    FLOAT U;
    FLOAT V;
    FLOAT Dist;

    //Floor-ray intersection test
    D3DXIntersect(m_pFloor->m_pMesh, &org, &dir, &Hit, &FaceIndex, &U, &V, &Dist, NULL, NULL);

    if (Hit) {
        //Adjust position accordingly to the floor height
        pos.y = org.y - Dist;
    }
}