#include "particle.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;

ID3DXMesh *m_pSphere = NULL;

void LoadParticleResources() {
    D3DXCreateSphere(g_pDevice, 0.05f, 15, 15, &m_pSphere, NULL);
}

//////////////////////////////////////////////////////////////////
//                          Particle                            //
//////////////////////////////////////////////////////////////////

Particle::Particle() {
    m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_oldPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_forces = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_bounce = 0.7f;
}

Particle::Particle(D3DXVECTOR3 pos) {
    m_pos = pos;
    m_oldPos = pos;
    m_forces = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_bounce = 0.5f + ((rand()%1000) / 1000.0f) * 0.2f;
}

void Particle::Update(float deltaTime) {
    //Verlet Integration
    D3DXVECTOR3 temp = m_pos;
    m_pos += (m_pos - m_oldPos) * FRICTION + m_forces * deltaTime * deltaTime;
    m_oldPos = temp;
}

void Particle::Render() {
    if (m_pSphere != NULL) {
        D3DXMATRIX world;
        D3DXMatrixTranslation(&world, m_pos.x, m_pos.y + 0.025f, m_pos.z);
        g_pEffect->SetMatrix("matW", &world);

        D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
        g_pEffect->SetTechnique(hTech);
        g_pEffect->Begin(NULL, NULL);
        g_pEffect->BeginPass(0);

        m_pSphere->DrawSubset(0);

        g_pEffect->EndPass();
        g_pEffect->End();
    }
}

void Particle::AddForces() {
    //Gravity
    m_forces = D3DXVECTOR3(0.0f, GRAVITY, 0.0f);
}

void Particle::SatisfyConstraints(vector<OBB*> &obstacles) {
    int numObj = (int)obstacles.size();
    for (int i=0; i<numObj; i++) {
        //Collision
        if (obstacles[i]->Intersect(m_pos)) {
            //Calculate Velocity
            D3DXVECTOR3 V = m_pos - m_oldPos;
            D3DXVECTOR3 dir;
            D3DXVec3Normalize(&dir, &V);

            Ray r(m_oldPos, dir);
            r = obstacles[i]->GetContactPoint(r);

            D3DXVECTOR3 VN = D3DXVec3Dot(&r.m_dir, &V) * r.m_dir;
            D3DXVECTOR3 VT = V - VN;

            m_pos = r.m_org + 0.005f * r.m_dir;

            m_oldPos = m_pos - (VT - VN * m_bounce);
        }
    }
}