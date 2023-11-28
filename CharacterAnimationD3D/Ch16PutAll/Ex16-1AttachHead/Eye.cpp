#include "eye.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;

ID3DXMesh *g_pEyeMesh = NULL;
vector<IDirect3DTexture9*> g_pEyeTextures;

void LoadEyeResources() {
    //Load Eye Resources
    D3DXLoadMeshFromX("resources/facefactory/eye.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &g_pEyeMesh);

    //Load Eye Textures
    string eyeFiles[] = {"resources/facefactory/eye01.jpg", "resources/facefactory/eye02.jpg",
                         "resources/facefactory/eye03.jpg", "resources/facefactory/eye04.jpg",
                         "resources/facefactory/eye05.jpg"
                        };

    for (int i=0; i<5; i++) {
        IDirect3DTexture9* eyeTexture = NULL;
        D3DXCreateTextureFromFile(g_pDevice, eyeFiles[i].c_str(), &eyeTexture);
        g_pEyeTextures.push_back(eyeTexture);
    }
}

Eye::Eye() {

}

void Eye::Init(D3DXVECTOR3 position, int textureIndex) {
    m_textureIndex = textureIndex;

    //Set Eye position
    m_position = position;
}

void Eye::Render(D3DXMATRIX &headMatrix) {
    D3DXMATRIX p;
    D3DXMatrixTranslation(&p, m_position.x, m_position.y, m_position.z);

    g_pEffect->SetMatrix("matW", &(m_rotation * p * headMatrix));
    g_pEffect->SetTexture("texDiffuse", g_pEyeTextures[m_textureIndex]);
    g_pEffect->CommitChanges();

    g_pEyeMesh->DrawSubset(0);
}

void Eye::LookAt(D3DXVECTOR3 focus) {
    float rotY = atan2(m_position.x - focus.x, m_position.z - focus.z) * 0.8f;
    float rotZ = atan2(m_position.y - focus.y, m_position.z - focus.z) * 0.5f;
    D3DXMatrixRotationYawPitchRoll(&m_rotation, rotY, rotZ, 0.0f);
}