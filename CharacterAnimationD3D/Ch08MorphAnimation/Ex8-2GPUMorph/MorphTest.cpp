#include "morphTest.h"

extern IDirect3DDevice9* g_pDevice;
extern ofstream g_debug;
extern ID3DXEffect *g_pEffect;

bool KeyDown(int vk_code);
bool KeyUp(int vk_code);

//Our morph vertex format
D3DVERTEXELEMENT9 morphVertexDecl[] = {
    //1st Stream: Base Mesh
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
    {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

    //2nd Stream: 1st Morph Target
    {1,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
    {1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   1},
    {1, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},

    //3rd Stream: 2st Morph Target
    {2,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2},
    {2, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   2},
    {2, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},

    //4th Stream: 3st Morph Target
    {3,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 3},
    {3, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   3},
    {3, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},

    //5th Stream: 4st Morph Target
    {4,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 4},
    {4, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   4},
    {4, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},

    D3DDECL_END()
};

MorphTest::MorphTest() {
    m_pBaseMesh = NULL;
    m_pTargets[0] = NULL;
    m_pTargets[1] = NULL;
    m_pTargets[2] = NULL;
    m_pTargets[3] = NULL;

    m_pDecl = NULL;

    m_oldWeights = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
    m_newWeights = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
    m_prc = 1.0f;
}

void MorphTest::Init() {
    //Create Vertex Declaration
    g_pDevice->CreateVertexDeclaration(morphVertexDecl, &m_pDecl);

    //Load Base Mesh
    D3DXLoadMeshFromX("resources/baseMesh.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pBaseMesh);

    //Load Targets
    D3DXLoadMeshFromX("resources/target01.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pTargets[0]);
    D3DXLoadMeshFromX("resources/target02.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pTargets[1]);
    D3DXLoadMeshFromX("resources/target03.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pTargets[2]);
    D3DXLoadMeshFromX("resources/target04.x", D3DXMESH_MANAGED, g_pDevice, NULL, NULL, NULL, NULL, &m_pTargets[3]);
}

void MorphTest::Update(float deltaTime) {
    m_prc += deltaTime * 1.0f;

    if (m_prc > 1.0f) {
        m_prc = 0.0f;
        m_oldWeights = m_newWeights;
        m_newWeights = D3DXVECTOR4(max(((rand()%1000) / 1000.0f) * 2.0f - 1.0f, 0.0f),
                                   max(((rand()%1000) / 1000.0f) * 2.0f - 1.0f, 0.0f),
                                   max(((rand()%1000) / 1000.0f) * 2.0f - 1.0f, 0.0f),
                                   max(((rand()%1000) / 1000.0f) * 2.0f - 1.0f, 0.0f));
    }
}

void MorphTest::Render() {
    //Set Active Vertex Declaration
    g_pDevice->SetVertexDeclaration(m_pDecl);

    //Get Size per vertex in bytes
    DWORD vSize = D3DXGetFVFVertexSize(m_pBaseMesh->GetFVF());

    //Set base stream
    IDirect3DVertexBuffer9* baseMeshBuffer = NULL;
    m_pBaseMesh->GetVertexBuffer(&baseMeshBuffer);
    g_pDevice->SetStreamSource(0, baseMeshBuffer, 0, vSize);

    //Set Target streams
    for (int i=0; i<4; i++) {
        IDirect3DVertexBuffer9* targetMeshBuffer = NULL;
        m_pTargets[i]->GetVertexBuffer(&targetMeshBuffer);
        g_pDevice->SetStreamSource(i + 1, targetMeshBuffer, 0, vSize);
    }

    //Set Index buffer
    IDirect3DIndexBuffer9* ib = NULL;
    m_pBaseMesh->GetIndexBuffer(&ib);
    g_pDevice->SetIndices(ib);

    //Set Shader Variables
    D3DXMATRIX world, view, proj;
    g_pDevice->GetTransform(D3DTS_VIEW, &view);
    g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    D3DXMatrixIdentity(&world);
    D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

    D3DXVECTOR4 weights = m_newWeights * m_prc + (1.0f - m_prc) * m_oldWeights;

    g_pEffect->SetMatrix("matW", &world);
    g_pEffect->SetMatrix("matVP", &(view * proj));
    g_pEffect->SetVector("weights", &weights);
    g_pEffect->SetVector("lightPos", &lightPos);
    D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
    g_pEffect->SetTechnique(hTech);
    g_pEffect->Begin(NULL, NULL);
    g_pEffect->BeginPass(0);

    //Draw mesh
    g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
                                    m_pBaseMesh->GetNumVertices(), 0,
                                    m_pBaseMesh->GetNumFaces());

    g_pEffect->EndPass();
    g_pEffect->End();

    //Restore
    g_pDevice->SetVertexDeclaration(NULL);

    //Set Stream Sources to NULL
    for (int i=0; i<5; i++) {
        g_pDevice->SetStreamSource(i, NULL, 0, 0);
    }
}

