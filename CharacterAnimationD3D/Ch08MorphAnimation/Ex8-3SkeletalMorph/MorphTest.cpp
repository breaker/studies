#include "morphTest.h"

#pragma warning(disable:4996)

extern IDirect3DDevice9* g_pDevice;
extern ofstream g_debug;
extern ID3DXFont *g_pFont;
extern ID3DXEffect *g_pEffect;

bool KeyDown(int vk_code);
bool KeyUp(int vk_code);

D3DVERTEXELEMENT9 morphVertexDecl[] = {
    //1st Stream: Human Skinned Mesh
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
    {0, 16, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
    {0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

    //2nd Stream: Werewolf Morph Target
    {1,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
    {1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},

    D3DDECL_END()
};

MorphTest::MorphTest() : SkinnedMesh() {
    m_pDecl = NULL;
    m_animControl = NULL;
}

void MorphTest::Init() {
    //Create Vertex Declaration
    g_pDevice->CreateVertexDeclaration(morphVertexDecl, &m_pDecl);

    //Load Base Mesh
    Load("resources/werewolf.x");
    m_animControl = GetController();

    //Load Textures
    D3DXCreateTextureFromFile(g_pDevice, "resources/human.jpg", &m_pHumanTexture);
    D3DXCreateTextureFromFile(g_pDevice, "resources/werewolf.jpg", &m_pWolfTexture);
}

void MorphTest::Update(float deltaTime) {
    //Update Skinned Mesh
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    m_animControl->AdvanceTime(deltaTime, NULL);
    SetPose(world);
}

void MorphTest::Render() {
    //Set Shader Variables
    D3DXMATRIX world, view, proj;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    D3DXMatrixIdentity(&world);
    D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

    g_pEffect->SetMatrix("matW", &world);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetVector("lightPos", &lightPos);

    g_pEffect->SetTexture("texHuman", m_pHumanTexture);
    g_pEffect->SetTexture("texWolf", m_pWolfTexture);

    float shift = max(min(sin(GetTickCount() * 0.0005f) * 1.5f + 0.5f, 1.0f), 0.0f);
    g_pEffect->SetFloat("shapeShift", shift);

    //Set Active Vertex Declaration
    g_pDevice->SetVertexDeclaration(m_pDecl);

    //Set werewolf & human stream
    D3DXFRAME* wolfBone = D3DXFrameFind(m_pRootBone, "werewolf");
    if (wolfBone != NULL) {
        if (wolfBone->pMeshContainer != NULL) {
            ID3DXMesh* wolfmesh = wolfBone->pMeshContainer->MeshData.pMesh;
            DWORD vSize = D3DXGetFVFVertexSize(wolfmesh->GetFVF());
            IDirect3DVertexBuffer9* wolfMeshBuffer = NULL;
            wolfmesh->GetVertexBuffer(&wolfMeshBuffer);
            g_pDevice->SetStreamSource(1, wolfMeshBuffer, 0, vSize);
        }
    }

    //Render Human / Werewolf
    D3DXFRAME* humanBone = D3DXFrameFind(m_pRootBone, "Bruce");
    RenderHuman((Bone*)humanBone);

    //Render Shift text
    RECT r = {10, 10, 0, 0};
    string s = "Shape Shift: ";
    char num[10];
    itoa((int)(shift * 100), num, 10);
    s += num;
    s += "%";
    g_pFont->DrawText(NULL, s.c_str(), -1, &r, DT_TOP | DT_LEFT | DT_NOCLIP, 0xAA000000);
}

void MorphTest::RenderHuman(Bone *bone) {
    if (bone == NULL)bone = (Bone*)m_pRootBone;

    //If there is a mesh to render...
    if (bone->pMeshContainer != NULL) {
        BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

        if (boneMesh->pSkinInfo != NULL) {
            // set up bone transforms
            int numBones = boneMesh->pSkinInfo->GetNumBones();
            for (int i=0; i < numBones; i++) {
                D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i],
                                   &boneMesh->boneOffsetMatrices[i],
                                   boneMesh->boneMatrixPtrs[i]);
            }

            g_pEffect->SetMatrixArray("FinalTransforms",
                                      boneMesh->currentBoneMatrices,
                                      boneMesh->pSkinInfo->GetNumBones());

            //Get Size per vertex in bytes
            DWORD vSize = D3DXGetFVFVertexSize(boneMesh->MeshData.pMesh->GetFVF());

            //Set base stream (human)
            IDirect3DVertexBuffer9* baseMeshBuffer = NULL;
            boneMesh->MeshData.pMesh->GetVertexBuffer(&baseMeshBuffer);
            g_pDevice->SetStreamSource(0, baseMeshBuffer, 0, vSize);

            //Set Index buffer
            IDirect3DIndexBuffer9* ib = NULL;
            boneMesh->MeshData.pMesh->GetIndexBuffer(&ib);
            g_pDevice->SetIndices(ib);

            //Start Shader
            D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Skinning");
            g_pEffect->SetTechnique(hTech);
            g_pEffect->Begin(NULL, NULL);
            g_pEffect->BeginPass(0);

            //Draw mesh
            g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
                                            boneMesh->MeshData.pMesh->GetNumVertices(), 0,
                                            boneMesh->MeshData.pMesh->GetNumFaces());

            g_pEffect->EndPass();
            g_pEffect->End();
        }
    }

    if (bone->pFrameSibling != NULL)RenderHuman((Bone*)bone->pFrameSibling);
    if (bone->pFrameFirstChild != NULL)RenderHuman((Bone*)bone->pFrameFirstChild);
}