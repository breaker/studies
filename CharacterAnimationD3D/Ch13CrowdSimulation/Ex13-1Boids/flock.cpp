#include "flock.h"

extern IDirect3DDevice9 *g_pDevice;

Flock::Flock(int numBoids) {
    for (int i=0; i<numBoids; i++) {
        m_boids.push_back(new Boid(this));
    }
}

Flock::~Flock() {
    for (int i=0; i<(int)m_boids.size(); i++) {
        delete m_boids[i];
    }
    m_boids.clear();
}

void Flock::Update(float deltaTime) {
    for (int i=0; i<(int)m_boids.size(); i++) {
        m_boids[i]->Update(deltaTime);
    }
}

void Flock::Render(bool shadow) {
    for (int i=0; i<(int)m_boids.size(); i++) {
        m_boids[i]->Render(shadow);
    }
}

void Flock::GetNeighbors(Boid* pBoid, float radius, vector<Boid*> &neighbors) {
    for (int i=0; i<(int)m_boids.size(); i++) {
        if (m_boids[i] != pBoid) {
            if (D3DXVec3Length(&(pBoid->m_position - m_boids[i]->m_position)) < radius) {
                neighbors.push_back(m_boids[i]);
            }
        }
    }
}