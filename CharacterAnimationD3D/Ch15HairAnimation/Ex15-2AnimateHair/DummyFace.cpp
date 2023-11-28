#include "DummyFace.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                  DummyFace                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////

DummyFace::DummyFace() {
    m_pMesh = NULL;
    m_pTexture = NULL;

    D3DXLoadMeshFromX("resources/face01.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pMesh);
    D3DXCreateTextureFromFile(g_pDevice, "resources/face.jpg", &m_pTexture);
}

DummyFace::~DummyFace() {
    if (m_pMesh != NULL)
        m_pMesh->Release();

    if (m_pTexture != NULL)
        m_pTexture->Release();
}

void DummyFace::Render() {
    g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
    g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
    g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);

    D3DXMATRIX world;
    D3DXMatrixScaling(&world, 0.95f, 0.95f, 0.95f);
    //D3DXMatrixIdentity(&world);
    g_pDevice->SetTransform(D3DTS_WORLD, &world);

    //Set Texture
    g_pDevice->SetTexture(0, m_pTexture);

    //Render mesh
    m_pMesh->DrawSubset(0);
}
