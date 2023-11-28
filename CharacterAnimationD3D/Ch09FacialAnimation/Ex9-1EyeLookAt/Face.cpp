#include "face.h"

extern IDirect3DDevice9* g_pDevice;
extern ofstream g_debug;
extern ID3DXEffect *g_pEffect;

bool KeyDown(int vk_code);
bool KeyUp(int vk_code);

Face::Face() {
    m_pFaceMesh = NULL;
    m_pFaceTexture = NULL;
}

void Face::Init() {
    //Load mesh
    D3DXLoadMeshFromX("resources/face.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pFaceMesh);

    //Load texture
    D3DXCreateTextureFromFile(g_pDevice, "resources/face.jpg", &m_pFaceTexture);

    //Init Eyes
    m_eyes[0].Init(D3DXVECTOR3(-0.037f, 0.04f, -0.057f));
    m_eyes[1].Init(D3DXVECTOR3( 0.037f, 0.04f, -0.057f));
}

void Face::Update(float deltaTime) {
    //Get look at point from cursor location
    POINT p;
    GetCursorPos(&p);

    float x = (p.x - 400) / 800.0f;
    float y = (p.y - 300) / 600.0f;

    //Tell both Eyes to look at this point
    m_eyes[0].LookAt(D3DXVECTOR3(x, y, -1.0f));
    m_eyes[1].LookAt(D3DXVECTOR3(x, y, -1.0f));
}

void Face::Render() {
    //Set Shader Variables
    D3DXMATRIX i, view, proj;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    D3DXMatrixIdentity(&i);
    D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

    g_pEffect->SetMatrix("matW", &i);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetVector("lightPos", &lightPos);
    g_pEffect->SetTexture("texDiffuse", m_pFaceTexture);

    //Start Technique
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    //Render Face
    m_pFaceMesh->DrawSubset(0);

    //Render Eyes
    m_eyes[0].Render(g_pEffect);
    m_eyes[1].Render(g_pEffect);

    g_pEffect->EndPass();
    g_pEffect->End();
}