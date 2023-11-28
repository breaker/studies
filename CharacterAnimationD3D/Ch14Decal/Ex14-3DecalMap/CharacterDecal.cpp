#include "CharacterDecal.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

IDirect3DTexture9* CharacterDecal::sm_pDecalTexture = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  CharacterDecal                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

CharacterDecal::CharacterDecal(ID3DXMesh* pDecalMesh) {
    m_pDecalMesh = pDecalMesh;

    //Load static decal texture
    if (sm_pDecalTexture == NULL) {
        D3DXCreateTextureFromFile(g_pDevice, "resources/meshes/decal.dds", &sm_pDecalTexture);
    }
}

CharacterDecal::~CharacterDecal() {
    if (m_pDecalMesh != NULL)
        m_pDecalMesh->Release();
}

void CharacterDecal::Render() {
    g_pEffect->SetTexture("texDecal", sm_pDecalTexture);

    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Decal");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    m_pDecalMesh->DrawSubset(0);

    g_pEffect->EndPass();
    g_pEffect->End();
}