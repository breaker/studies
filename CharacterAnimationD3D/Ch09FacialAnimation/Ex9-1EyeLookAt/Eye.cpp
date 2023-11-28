#include "eye.h"

extern IDirect3DDevice9* g_pDevice;

ID3DXMesh *g_pEyeMesh = NULL;
IDirect3DTexture9 *g_pEyeTexture = NULL;

Eye::Eye() {

}

void Eye::Init(D3DXVECTOR3 position) {
    //Load Eye Resources
    if (g_pEyeMesh == NULL) {
        D3DXLoadMeshFromX("resources/eye.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &g_pEyeMesh);
    }

    if (g_pEyeTexture == NULL) {
        D3DXCreateTextureFromFile(g_pDevice, "resources/eye.jpg", &g_pEyeTexture);
    }

    //Set Eye position
    m_position = position;
}

void Eye::Render(ID3DXEffect *g_pEffect) {
    D3DXMATRIX p;
    D3DXMatrixTranslation(&p, m_position.x, m_position.y, m_position.z);

    g_pEffect->SetMatrix("matW", &(m_rotation * p));
    g_pEffect->SetTexture("texDiffuse", g_pEyeTexture);
    g_pEffect->CommitChanges();

    g_pEyeMesh->DrawSubset(0);
}

void Eye::LookAt(D3DXVECTOR3 focus) {
    float rotY = atan2(m_position.x - focus.x, m_position.z - focus.z) * 0.8f;
    float rotZ = atan2(m_position.y - focus.y, m_position.z - focus.z) * 0.5f;
    D3DXMatrixRotationYawPitchRoll(&m_rotation, rotY, rotZ, 0.0f);
}