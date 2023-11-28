#include "Hair.h"

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

bool KeyDown(int vk_code);

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Hair                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////

Hair::Hair() {
    //Test control hairs
    m_controlHairs.push_back(ControlHair::CreateTestHair(D3DXVECTOR3(-2.0f, 0.0f, -2.0f)));
    m_controlHairs.push_back(ControlHair::CreateTestHair(D3DXVECTOR3(2.0f, 0.0f, -2.0f)));
    m_controlHairs.push_back(ControlHair::CreateTestHair(D3DXVECTOR3(2.0f, 0.0f, 2.0f)));
    m_controlHairs.push_back(ControlHair::CreateTestHair(D3DXVECTOR3(-2.0f, 0.0f, 2.0f)));

    m_hairPatches.push_back(new HairPatch(m_controlHairs[0],
                                          m_controlHairs[1],
                                          m_controlHairs[2],
                                          m_controlHairs[3]));

    D3DXCreateTextureFromFile(g_pDevice, "resources/hair.png", &m_pHairTexture);
}

Hair::~Hair() {
    for (int i=0; i<(int)m_controlHairs.size(); i++)
        delete m_controlHairs[i];

    for (int i=0; i<(int)m_hairPatches.size(); i++)
        delete m_hairPatches[i];
}

void Hair::Update(float deltaTime) {
    //Update patches
    for (int i=0; i<(int)m_controlHairs.size(); i++)
        m_controlHairs[i]->Update(deltaTime);
}

void Hair::Render() {
    //Render control hairs
    for (int i=0; i<(int)m_controlHairs.size(); i++)
        m_controlHairs[i]->Render();

    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Hair");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->SetTexture("texDiffuse", m_pHairTexture);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    if (KeyDown(VK_SPACE)) {
        g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        g_pEffect->SetTexture("texDiffuse", NULL);
        g_pEffect->CommitChanges();
    }

    //Render patches
    for (int i=0; i<(int)m_hairPatches.size(); i++)
        m_hairPatches[i]->Render();

    g_pEffect->EndPass();
    g_pEffect->End();

    g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}