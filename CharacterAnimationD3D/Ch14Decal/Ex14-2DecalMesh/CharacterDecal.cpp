#include "CharacterDecal.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  CharacterDecal                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

CharacterDecal::CharacterDecal(ID3DXMesh* pDecalMesh) {
    m_pDecalMesh = pDecalMesh;
}

CharacterDecal::~CharacterDecal() {
    if (m_pDecalMesh != NULL)
        m_pDecalMesh->Release();
}

void CharacterDecal::Render() {
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Decal");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    m_pDecalMesh->DrawSubset(0);

    g_pEffect->EndPass();
    g_pEffect->End();
}