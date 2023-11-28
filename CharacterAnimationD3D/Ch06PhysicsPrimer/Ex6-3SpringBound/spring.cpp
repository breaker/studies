#include "spring.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;

struct VERTEX {
    VERTEX(D3DXVECTOR3 p, D3DCOLOR c) {
        pos = p;
        color = c;
    }

    D3DXVECTOR3 pos;
    D3DCOLOR color;
    static const DWORD FVF;
};

const DWORD VERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

void RenderLine(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXCOLOR c) {
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    g_pEffect->SetMatrix("matW", &world);

    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    g_pDevice->SetFVF(VERTEX::FVF);
    p1.y += 0.025f;
    p2.y += 0.025f;
    VERTEX vert[] = {VERTEX(p1, c), VERTEX(p2, c)};
    g_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &vert[0], sizeof(VERTEX));

    g_pEffect->EndPass();
    g_pEffect->End();
}

//////////////////////////////////////////////////////////
//                      Spring                          //
//////////////////////////////////////////////////////////

Spring::Spring(Particle *p1, Particle *p2, float restLength) {
    m_pParticle1 = p1;
    m_pParticle2 = p2;
    m_restLength = restLength;

    SatisfyConstraints(vector<OBB*>());
    p1->m_oldPos = p1->m_oldPos = p1->m_pos;
    p2->m_oldPos = p2->m_oldPos = p2->m_pos;
}

void Spring::Render() {
    RenderLine(m_pParticle1->m_pos, m_pParticle2->m_pos, 0xffff8888);
}

void Spring::SatisfyConstraints(vector<OBB*> &obstacles) {
    D3DXVECTOR3 delta = m_pParticle1->m_pos - m_pParticle2->m_pos;
    float dist = D3DXVec3Length(&delta);
    float diff = (dist-m_restLength)/dist;
    m_pParticle1->m_pos -= delta * 0.5f * diff;
    m_pParticle2->m_pos += delta * 0.5f * diff;
}