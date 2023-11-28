#include "Hair.h"
#include <fstream>

extern IDirect3DDevice9* g_pDevice;
extern ID3DXEffect *g_pEffect;
extern ofstream g_debug;

bool KeyDown(int vk_code);

//////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Hair                                                    //
//////////////////////////////////////////////////////////////////////////////////////////////////

Hair::Hair() {
    LoadHair("resources/hair.bin");
    D3DXCreateTextureFromFile(g_pDevice, "resources/hair.png", &m_pHairTexture);
}

Hair::~Hair() {
    for (int i=0; i<(int)m_controlHairs.size(); i++)
        delete m_controlHairs[i];

    for (int i=0; i<(int)m_hairPatches.size(); i++)
        delete m_hairPatches[i];
}

void Hair::LoadHair(const char* hairFile) {
    ifstream in(hairFile, ios::binary);

    if (!in.good())
        return;

    //Version number
    long version;
    in.read((char*)&version, sizeof(long));

    //Number splines
    long numSplines = 0;
    in.read((char*)&numSplines, sizeof(long));

    //Read splines
    for (int i=0; i<numSplines; i++) {
        ControlHair* ch = new ControlHair();

        //Read points
        long numPoints = 0;
        in.read((char*)&numPoints, sizeof(long));

        for (int p=0; p<numPoints; p++) {
            D3DXVECTOR3 point;
            in.read((char*)&point, sizeof(D3DXVECTOR3));
            ch->AddPoint(point);
        }

        m_controlHairs.push_back(ch);
    }

    in.close();

    //Patches
    CreatePatch(1, 0, 5, 6);
    CreatePatch(2, 1, 6, 7);
    CreatePatch(3, 2, 7, 8);
    CreatePatch(4, 3, 8, 9);
    CreatePatch(6, 5, 14, 10);
    CreatePatch(7, 6, 10, 11);
    CreatePatch(8, 7, 11, 12);
    CreatePatch(9, 8, 12, 13);

    CreatePatch(15, 16, 21, 20);
    CreatePatch(16, 17, 22, 21);
    CreatePatch(17, 18, 23, 22);
    CreatePatch(18, 29, 24, 23);
    CreatePatch(20, 21, 25, 27);
    CreatePatch(21, 22, 26, 25);
    CreatePatch(22, 23, 19, 26);
    CreatePatch(23, 24, 28, 19);

    CreatePatch(29, 4, 24, 9);
    CreatePatch(24, 9, 13, 28);


    //Create head spheres
    m_headSpheres.push_back(BoundingSphere(D3DXVECTOR3(0.0f, 0.057f, -0.017f), 0.082f));
    m_headSpheres.push_back(BoundingSphere(D3DXVECTOR3(0.0f, 0.071f,  0.017f), 0.082f));
    m_headSpheres.push_back(BoundingSphere(D3DXVECTOR3(0.0f, 0.054f,  0.042f), 0.08f));
    m_headSpheres.push_back(BoundingSphere(D3DXVECTOR3(0.0f, 0.018f,  0.017f), 0.09f));
}

void Hair::CreatePatch(int index1, int index2, int index3, int index4) {
    m_hairPatches.push_back(
        new HairPatch(m_controlHairs[index1],
                      m_controlHairs[index2],
                      m_controlHairs[index3],
                      m_controlHairs[index4]));
}

void Hair::Update(float deltaTime) {
    deltaTime = min(deltaTime, 0.1f);

    for (int i=0; i<(int)m_controlHairs.size(); i++) {
        m_controlHairs[i]->UpdateSimulation(deltaTime, m_headSpheres);
    }
}

void Hair::Render(D3DXVECTOR3 &camPos) {
    //Render control hairs
    if (KeyDown(VK_SPACE)) {
        for (int i=0; i<(int)m_headSpheres.size(); i++)
            m_headSpheres[i].Render();

        for (int i=0; i<(int)m_controlHairs.size(); i++)
            m_controlHairs[i]->Render();

        return;
    }

    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Hair");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->SetTexture("texDiffuse", m_pHairTexture);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    //sort hair patches in z depth
    for (int i=0; i<(int)m_hairPatches.size(); i++) {
        for (int j=i+1; j<(int)m_hairPatches.size(); j++) {
            if (D3DXVec3Length(&(camPos - m_hairPatches[j]->m_center)) >
                    D3DXVec3Length(&(camPos - m_hairPatches[i]->m_center))) {
                HairPatch* temp = m_hairPatches[j];
                m_hairPatches[j] = m_hairPatches[i];
                m_hairPatches[i] = temp;
            }
        }
    }

    //Render patches
    for (int i=0; i<(int)m_hairPatches.size(); i++)
        m_hairPatches[i]->Render();

    g_pEffect->EndPass();
    g_pEffect->End();

    g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

int Hair::GetNumVertices() {
    int numVerts = 0;

    for (int i=0; i<(int)m_hairPatches.size(); i++)
        numVerts += m_hairPatches[i]->m_pHairMesh->GetNumVertices();

    return numVerts;
}

int Hair::GetNumFaces() {
    int numFaces = 0;

    for (int i=0; i<(int)m_hairPatches.size(); i++)
        numFaces += m_hairPatches[i]->m_pHairMesh->GetNumFaces();

    return numFaces;
}