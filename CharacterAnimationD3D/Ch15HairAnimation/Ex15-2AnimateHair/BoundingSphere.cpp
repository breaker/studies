#include "BoundingSphere.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;

ID3DXMesh* BoundingSphere::sm_pSphereMesh = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  DummyFace                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////

BoundingSphere::BoundingSphere(D3DXVECTOR3 pos, float radius) {
    if (sm_pSphereMesh == NULL) {
        D3DXCreateSphere(g_pDevice, 1.0f, 10, 10, &sm_pSphereMesh, NULL);
    }

    m_position = pos;
    m_radius = radius;
}

void BoundingSphere::Render() {
    if (sm_pSphereMesh != NULL) {
        D3DXMATRIX pos, sca;
        D3DXMatrixScaling(&sca, m_radius, m_radius, m_radius);
        D3DXMatrixTranslation(&pos, m_position.x, m_position.y, m_position.z);

        g_pEffect->SetMatrix("matW", &(sca * pos));

        D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("BSLighting");
        g_pEffect->SetTechnique(hTech);
        g_pEffect->Begin(NULL, NULL);
        g_pEffect->BeginPass(0);

        sm_pSphereMesh->DrawSubset(0);

        g_pEffect->EndPass();
        g_pEffect->End();
    }
}

bool BoundingSphere::ResolveCollision(D3DXVECTOR3 &hairPos, float hairRadius) {
    //Difference between control hair point and sphere center
    D3DXVECTOR3 diff = hairPos - m_position;

    //Distance between points
    float dist = D3DXVec3Length(&diff);

    if (dist < (m_radius + hairRadius)) {
        //Collision has occurred, move hair away from bounding sphere
        D3DXVec3Normalize(&diff, &diff);
        hairPos = m_position + diff * (m_radius + hairRadius);
        return true;
    }

    return false;
}