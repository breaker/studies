#include "crowd.h"

extern IDirect3DDevice9 *g_pDevice;

Crowd::Crowd(int numBoids) {
    for (int i=0; i<numBoids; i++) {
        m_entities.push_back(new CrowdEntity(this));
    }
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